#include "drop_system_canvas.h"

void ZyiDropSystemCanvas::_bind_methods() {
	GDVIRTUAL_BIND(_handle_pick_start, "data", "picker_node_ref");
	GDVIRTUAL_BIND(_handle_pick_finished, "data", "picker_node_ref");
	GDVIRTUAL_BIND(_handle_pick_finished_clean);

	ClassDB::bind_method(D_METHOD("get_source_texture"), &ZyiDropSystemCanvas::get_source_texture);
	ClassDB::bind_method(D_METHOD("set_source_texture", "value"), &ZyiDropSystemCanvas::set_source_texture);
	ClassDB::bind_method(D_METHOD("get_draw_space"), &ZyiDropSystemCanvas::get_draw_space);
	ClassDB::bind_method(D_METHOD("set_draw_space", "value"), &ZyiDropSystemCanvas::set_draw_space);
	ClassDB::bind_method(D_METHOD("get_drop_item_count"), &ZyiDropSystemCanvas::get_drop_item_count);
	ClassDB::bind_method(D_METHOD("get_picking_item_count"), &ZyiDropSystemCanvas::get_picking_item_count);

	ClassDB::bind_method(D_METHOD("update_world_grid", "world_rect", "grid_cell_size"), &ZyiDropSystemCanvas::update_world_grid);
	ClassDB::bind_method(D_METHOD("clear"), &ZyiDropSystemCanvas::clear);
	ClassDB::bind_method(D_METHOD("idle_process_pick", "delta", "picker_node", "pick_range"), &ZyiDropSystemCanvas::idle_process_pick);
	ClassDB::bind_method(D_METHOD("pick_all", "picker_node", "pick_range"), &ZyiDropSystemCanvas::pick_all);
	ClassDB::bind_method(D_METHOD("clean_drop_items"), &ZyiDropSystemCanvas::clean_drop_items);
	ClassDB::bind_method(D_METHOD("clean_picking_items"), &ZyiDropSystemCanvas::clean_picking_items);
	ClassDB::bind_method(D_METHOD("draw_in_space", "space"), &ZyiDropSystemCanvas::draw_in_space);
	ClassDB::bind_method(D_METHOD("add_drop_item", "texture_rect", "pos", "data"), &ZyiDropSystemCanvas::add_drop_item);

	ADD_SIGNAL(MethodInfo(SNAME("pick_start"), PropertyInfo(Variant::ARRAY, "data"), PropertyInfo(Variant::OBJECT, "picker_ref", PROPERTY_HINT_RESOURCE_TYPE, "WeakRef")));
	ADD_SIGNAL(MethodInfo(SNAME("pick_finished"), PropertyInfo(Variant::ARRAY, "data"), PropertyInfo(Variant::OBJECT, "picker_ref", PROPERTY_HINT_RESOURCE_TYPE, "WeakRef")));
	ADD_SIGNAL(MethodInfo(SNAME("pick_finished_clean")));

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source_texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_source_texture", "get_source_texture");
	ADD_PROPERTY(PropertyInfo(Variant::RECT2, "draw_space"), "set_draw_space", "get_draw_space");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "drop_item_count"), "", "get_drop_item_count");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "picking_item_count"), "", "get_picking_item_count");
}

void ZyiDropSystemCanvas::_notification(int p_notification) {
#ifdef TOOLS_ENABLED
	if (is_part_of_edited_scene()) {
		return;
	}
#endif
	if (_source_texture.is_null()) {
		return;
	}
	switch (p_notification) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			// _process
			queue_redraw();
		} break;
		case NOTIFICATION_READY: {
			// _ready
			_atlas_texture = memnew(AtlasTexture());
			_atlas_texture->set_atlas(_source_texture);
			_atlas_texture->set_region(Rect2(0, 0, _source_texture->get_width(), _source_texture->get_height()));
			set_process_internal(true);
		} break;
		case NOTIFICATION_DRAW: {
			draw_in_space(draw_space);
		} break;
	}
}

uint16_t ZyiDropSystemCanvas::get_picking_item_count() const {
	return picking_item_list.size();
}
void ZyiDropSystemCanvas::update_world_grid(const Rect2i &p_world_rect, const Size2i &p_grid_cell_size) {
	if (grid_cell_to_drop_list != nullptr && world_rect == p_world_rect && grid_cell_size == p_grid_cell_size) {
		return;
	}
	draw_space = p_world_rect;
	world_rect = p_world_rect;
	grid_cell_size = p_grid_cell_size;
	world_coord_size = get_coord_size(world_rect);
	grid_cell_count = world_coord_size.x * world_coord_size.y;
	if (grid_cell_to_drop_list == nullptr) {
		grid_cell_to_drop_list = (std::optional<std::vector<InternalDropItem>> *)memalloc(sizeof(std::optional<std::vector<InternalDropItem>>) * grid_cell_count);
		// 初始化每个元素，保证后续可用
		for (int i = 0; i < grid_cell_count; i++) {
			new (&grid_cell_to_drop_list[i]) std::optional<std::vector<InternalDropItem>>();
		}
	} else {
		// 先销毁之前的 vector 对象
		for (int i = 0; i < grid_cell_count; i++) {
			grid_cell_to_drop_list[i].reset();
		}
		grid_cell_to_drop_list = (std::optional<std::vector<InternalDropItem>> *)memrealloc(grid_cell_to_drop_list, sizeof(std::optional<std::vector<InternalDropItem>>) * grid_cell_count);
		// 初始化每个元素，保证后续可用
		for (int i = 0; i < grid_cell_count; i++) {
			new (&grid_cell_to_drop_list[i]) std::optional<std::vector<InternalDropItem>>();
		}
	}
}

void ZyiDropSystemCanvas::clear() {
	picking_item_list.clear();
	drop_item_count = 0;
	clear_world();
}

void ZyiDropSystemCanvas::clear_world() {
	grid_cell_count = 0;
	if (grid_cell_to_drop_list != nullptr) {
		memfree(grid_cell_to_drop_list);
		grid_cell_to_drop_list = nullptr;
	}
}
void ZyiDropSystemCanvas::idle_process_pick(double p_delta, Node2D *p_picker_node, double p_pick_range) {
	if (drop_item_count <= 0 || p_picker_node == nullptr || p_picker_node->is_queued_for_deletion() || !p_picker_node->is_inside_tree() || grid_cell_to_drop_list == nullptr || grid_cell_count == 0) {
		return;
	}
	const Vector2 picker_pos = p_picker_node->get_global_position();
	const Rect2i coord_rect = get_coord_range_in_circle(picker_pos, p_pick_range);
	const Vector2i start_coord = coord_rect.position;
	const Vector2i end_coord = coord_rect.get_end();
	if (start_coord == end_coord) {
		return;
	}
	const size_t origin_size = picking_item_list.size();
	LocalVector<int64_t> picked_index_list;
	int64_t origin_drop_item_count = drop_item_count;
	// 遍历在范围内的网格
	for (int32_t x = start_coord.x; x < end_coord.x; x++) {
		for (int32_t y = start_coord.y; y < end_coord.y; y++) {
			const int64_t index = calc_grid_index(x, y);
			std::optional<std::vector<InternalDropItem>> &item_data = grid_cell_to_drop_list[index];
			if (!item_data.has_value()) {
				continue;
			}
			const size_t item_num = item_data->size();
			for (size_t index = 0; index < item_num; index++) {
				const InternalDropItem &item = item_data->operator[](index);
				if (picker_pos.distance_to(item.drop_pos) <= p_pick_range + item.texture_rect.size.length()) {
					// 新拾取的
					picked_index_list.push_back(index);
					apply_item_pick(item, p_picker_node);
					if (drop_item_count > 0) {
						drop_item_count -= 1;
					}
				}
			}
			if (!picked_index_list.is_empty()) {
				for (int64_t index = picked_index_list.size() - 1; index >= 0; index--) {
					item_data->erase(item_data->begin() + picked_index_list[index]);
				}
				picked_index_list.clear();
			}
		}
	}
	if (origin_size != picking_item_list.size()) {
		queue_redraw();
	}
}

int64_t ZyiDropSystemCanvas::pick_all(Node2D *p_picker_node, double p_pick_range) {
	int64_t result = 0;
	for (size_t index = 0; index < grid_cell_count; index++) {
		std::optional<std::vector<InternalDropItem>> &item_data = grid_cell_to_drop_list[index];
		if (!item_data.has_value()) {
			continue;
		}
		for (const ZyiDropSystemCanvas::InternalDropItem &item : *item_data) {
			apply_item_pick(item, p_picker_node);
			result += 1;
		}
		item_data->clear();
	}
	drop_item_count = 0;
	return result;
}

void ZyiDropSystemCanvas::clean_drop_items() {
	for (size_t index = 0; index < grid_cell_count; index++) {
		std::optional<std::vector<InternalDropItem>> &item_data = grid_cell_to_drop_list[index];
		if (!item_data.has_value()) {
			continue;
		}
		item_data->clear();
	}
	drop_item_count = 0;
}

void ZyiDropSystemCanvas::clean_picking_items() {
	picking_item_list.clear();
}

void ZyiDropSystemCanvas::add_drop_item(const Rect2 &p_texture_rect, const Vector2 &p_pos, const Array &p_data) {
	if (grid_cell_to_drop_list == nullptr || grid_cell_count == 0) {
		return;
	}
	const int64_t index = get_grid_index(p_pos);
	if (index == -1) {
		int64_t i = get_grid_index(p_pos);
		return;
	}
	std::optional<std::vector<InternalDropItem>> &item_data = grid_cell_to_drop_list[index];
	if (!item_data.has_value()) {
		item_data = std::vector<InternalDropItem>();
	}
	item_data->emplace_back();
	ZyiDropSystemCanvas::InternalDropItem &item = item_data->back();
	item.drop_pos = p_pos;
	item.texture_rect = p_texture_rect;
	item.data = p_data;
	drop_item_count += 1;
	queue_redraw();
}

void ZyiDropSystemCanvas::handle_pick_start(const Array &p_data, Ref<WeakRef> p_picker_node_ref) {
	GDVIRTUAL_CALL(_handle_pick_start, p_data, p_picker_node_ref);
}

void ZyiDropSystemCanvas::handle_pick_finished(const Array &p_data, Ref<WeakRef> p_picker_node_ref) {
	GDVIRTUAL_CALL(_handle_pick_finished, p_data, p_picker_node_ref);
}

void ZyiDropSystemCanvas::handle_pick_finished_clean() {
	GDVIRTUAL_CALL(_handle_pick_finished_clean);
}

ZyiDropSystemCanvas::~ZyiDropSystemCanvas() {
	clear();
}

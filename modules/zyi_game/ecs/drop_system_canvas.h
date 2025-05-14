#ifndef DROP_SYSTEM_CANVAS_H
#define DROP_SYSTEM_CANVAS_H

#include "core/os/time.h"
#include "core/templates/hash_map.h"
#include "core/templates/local_vector.h"
#include "core/variant/variant_utility.h"
#include "scene/2d/node_2d.h"
#include "scene/resources/atlas_texture.h"

#include <optional>
#include <vector>

class ZyiDropSystemCanvas : public Node2D {
	GDCLASS(ZyiDropSystemCanvas, Node2D)

	struct InternalDropItem {
		Vector2 drop_pos;
		Rect2 texture_rect;
		Array data;
	};
	struct InternalPickedDropItem {
		Vector2 drop_pos;
		Rect2 texture_rect;
		Array data;
		Ref<WeakRef> picker_node_ref;
		bool picking;
		uint64_t pick_ticks_msec;
		bool pick_finished;
	};
	Rect2 draw_space;
	// world rect 指定网格计算的规格（cell size应该设置为比较大的）
	Rect2i world_rect;
	Size2i world_coord_size;
	// 单个网格大小
	Size2i grid_cell_size;
	// 网格单元总大小
	uint64_t grid_cell_count = 0;
	int64_t drop_item_count = 0;
	// 记录网格到掉落物品的映射
	std::optional<std::vector<InternalDropItem>> *grid_cell_to_drop_list = nullptr;
	// 已拾取的
	std::vector<InternalPickedDropItem> picking_item_list;

	Ref<AtlasTexture> _atlas_texture;
	Ref<Texture2D> _source_texture;
	uint16_t animation_min_dist_squared = INIT_MIN_PICK_ANIMATION_DITS_SQUARED;
	uint16_t animation_duration_ms = INIT_ANIMATION_DURATION_MS;

protected:
	static void _bind_methods();
	void _notification(int p_notification);

public:
	enum {
		INIT_CAPACITY = 256,
		INIT_MIN_PICK_ANIMATION_DITS_SQUARED = 360,
		INIT_ANIMATION_DURATION_MS = 500
	};
	enum InternalDrawResult {
		DRAW_RESULT_OK = 0,
		DRAW_RESULT_INVALID = 1,
		DRAW_RESULT_PICK_FINISHED = 2
	};
	Ref<Texture2D> get_source_texture() const { return _source_texture; }
	void set_source_texture(Ref<Texture2D> p_texture) { _source_texture = p_texture; }
	Rect2 get_draw_space() const { return draw_space; }
	void set_draw_space(const Rect2 &p_space) { draw_space = p_space; }
	uint64_t get_drop_item_count() const { return drop_item_count; }
	uint16_t get_picking_item_count() const;

	void update_world_grid(const Rect2i &p_world_rect, const Size2i &p_grid_cell_size);
	void clear();
	void clear_world();
	void idle_process_pick(double p_delta, Node2D *p_picker_node, double p_pick_range);
	int64_t pick_all(Node2D *p_picker_node, double p_pick_range);
	void clean_drop_items();
	void clean_picking_items();
	void add_drop_item(const Rect2 &p_texture_rect, const Vector2 &p_pos, const Array &p_data);

	virtual void handle_pick_start(const Array &p_data, Ref<WeakRef> p_picker_node_ref);
	GDVIRTUAL2C(_handle_pick_start, Array, Ref<WeakRef>);
	virtual void handle_pick_finished(const Array &p_data, Ref<WeakRef> p_picker_node_ref);
	GDVIRTUAL2C(_handle_pick_finished, Array, Ref<WeakRef>);
	virtual void handle_pick_finished_clean();
	GDVIRTUAL0C(_handle_pick_finished_clean);

	_ALWAYS_INLINE_ Size2i get_coord_size(const Rect2i &p_rect) const {
		return Size2i(VariantUtilityFunctions::ceili(p_rect.size.x / grid_cell_size.x), VariantUtilityFunctions::ceili(p_rect.size.y / grid_cell_size.y));
	}

	_ALWAYS_INLINE_ Rect2i get_coord_range_in_rect(const Rect2 &p_rect) const {
		Vector2i start_coord = get_grid_coord(p_rect.position.max(world_rect.position));
		Vector2i end_coord = get_grid_coord((p_rect.get_end() + grid_cell_size).min(world_rect.get_end()));
		return Rect2i(start_coord, end_coord - start_coord);
	}

	_ALWAYS_INLINE_ Rect2i get_coord_range_in_circle(const Vector2 &p_center, double p_radius) const {
		return get_coord_range_in_rect(Rect2(p_center - Vector2(p_radius, p_radius), Vector2(p_radius * 2, p_radius * 2)));
	}

	_ALWAYS_INLINE_ Vector2i get_grid_coord(const Vector2i &p_pos) const {
		const Vector2 pos = p_pos - world_rect.position;
		return Vector2i(pos.x / grid_cell_size.x, pos.y / grid_cell_size.y);
	}

	_ALWAYS_INLINE_ bool is_grid_coord_valid(const Vector2i &p_coord) const {
		return p_coord.x >= 0 && p_coord.x < world_coord_size.x && p_coord.y >= 0 && p_coord.y < world_coord_size.y;
	}

	_ALWAYS_INLINE_ int64_t get_grid_index(const Vector2i &p_pos) const {
		return get_grid_index_by_coord(get_grid_coord(p_pos));
	}

	_ALWAYS_INLINE_ int64_t get_grid_index_by_coord(const Vector2i &p_coord) const {
		if (!is_grid_coord_valid(p_coord)) {
			return -1;
		}
		return calc_grid_index(p_coord);
	}

	_ALWAYS_INLINE_ int64_t calc_grid_index(const Vector2i &p_coord) const {
		return calc_grid_index(p_coord.x, p_coord.y);
	}

	_ALWAYS_INLINE_ int64_t calc_grid_index(int32_t x, int32_t y) const {
		return x + y * world_coord_size.x;
	}

	_ALWAYS_INLINE_ void draw_in_space(const Rect2 &p_space) {
		// 处理 pick 之后需要执行动画的
		LocalVector<int64_t> indices_to_delete;
		const size_t item_num = picking_item_list.size();
		uint64_t cur_time = Time::get_singleton()->get_ticks_msec();
		for (size_t index = 0; index < item_num; index++) {
			InternalPickedDropItem &item = picking_item_list[index];
			InternalDrawResult result = draw_animating_item_in_space(p_space, item, cur_time);
			if (result != DRAW_RESULT_OK) {
				indices_to_delete.push_back(index);
			}
		}
		if (!indices_to_delete.is_empty()) {
			for (int64_t index = indices_to_delete.size() - 1; index >= 0; index--) {
				picking_item_list.erase(picking_item_list.begin() + indices_to_delete[index]);
			}
			if (get_picking_item_count() == 0) {
				handle_pick_finished_clean();
				emit_signal(SNAME("pick_finished_clean"));
			}
		}
		// 处理未被拾取的
		Rect2i coord_rect = get_coord_range_in_rect(p_space);
		const Vector2i start_coord = coord_rect.position;
		const Vector2i end_coord = coord_rect.get_end();
		if (start_coord != end_coord) {
			// 遍历在范围内的网格
			for (int32_t x = start_coord.x; x < end_coord.x; x++) {
				for (int32_t y = start_coord.y; y < end_coord.y; y++) {
					const int64_t index = calc_grid_index(x, y);
					std::optional<std::vector<InternalDropItem>> &item_data = grid_cell_to_drop_list[index];
					if (!item_data.has_value()) {
						continue;
					}
					for (const InternalDropItem &item : *item_data) {
						draw_item_in_space(item.texture_rect, item.drop_pos);
					}
				}
			}
		}
		// 重置变换
		draw_set_transform(Vector2());
	}

	_ALWAYS_INLINE_ InternalDrawResult draw_animating_item_in_space(const Rect2 &p_space, InternalPickedDropItem &p_item, uint64_t p_cur_time) {
		Variant picker_node = p_item.picker_node_ref->get_ref();
		if (picker_node.is_null() || picker_node.get_type() != Variant::OBJECT) {
			return DRAW_RESULT_INVALID;
		}
		Node2D *picker_node_2d = Object::cast_to<Node2D>(picker_node);
		Vector2 picker_pos = picker_node_2d->get_global_position();
		uint64_t elapsed_time = p_cur_time - p_item.pick_ticks_msec;
		if (elapsed_time >= animation_duration_ms || elapsed_time < 0) {
			if (!p_item.pick_finished) {
				apply_item_pick_finished(p_item);
				return DRAW_RESULT_PICK_FINISHED;
			}
			return DRAW_RESULT_INVALID;
		}
		// 计算位置
		double t_delta = 1.0 * elapsed_time / animation_duration_ms;
		double t = t_delta * t_delta * t_delta;
		double jump_t = 1.0 - (1.0 - t_delta) * (1.0 - t_delta);
		Vector2 pos = p_item.drop_pos.lerp(picker_pos, t) + picker_pos.direction_to(p_item.drop_pos) * 100 * (4 * jump_t * (1 - jump_t));
		if (p_space.has_point(pos)) {
			draw_item_in_space(p_item.texture_rect, pos);
		}
		if (picker_pos.distance_squared_to(pos) <= animation_min_dist_squared) {
			if (!p_item.pick_finished) {
				apply_item_pick_finished(p_item);
				return DRAW_RESULT_PICK_FINISHED;
			}
			return DRAW_RESULT_INVALID;
		}
		return DRAW_RESULT_OK;
	}

	_ALWAYS_INLINE_ void draw_item_in_space(const Rect2 &p_texture_rect, const Vector2 &p_pos) {
		Size2 rect_size = p_texture_rect.size;
		// 设置变换
		draw_set_transform(p_pos, 0);
		// 绘制
		draw_texture_rect_region(_atlas_texture, Rect2(-rect_size.x / 2, -rect_size.y / 2, rect_size.x, rect_size.y), p_texture_rect);
	}

	_ALWAYS_INLINE_ void apply_item_pick_finished(InternalPickedDropItem &item) {
		item.pick_finished = true;
		handle_pick_finished(item.data, item.picker_node_ref);
		emit_signal(SNAME("pick_finished"), item.data, item.picker_node_ref);
	}

	_ALWAYS_INLINE_ void apply_item_pick(const InternalDropItem &p_item, Node2D *p_picker_node) {
		picking_item_list.emplace_back();
		InternalPickedDropItem &picked_item = picking_item_list.back();
		picked_item.drop_pos = p_item.drop_pos;
		picked_item.texture_rect = p_item.texture_rect;
		picked_item.data = p_item.data;
		if (picked_item.picker_node_ref.is_null()) {
			picked_item.picker_node_ref = memnew(WeakRef);
		}
		picked_item.picker_node_ref->set_obj(p_picker_node);
		picked_item.picking = true;
		picked_item.pick_ticks_msec = Time::get_singleton()->get_ticks_msec();
		picked_item.pick_finished = false;
		handle_pick_start(picked_item.data, picked_item.picker_node_ref);
		emit_signal(SNAME("pick_start"), picked_item.data, picked_item.picker_node_ref);
	}

	~ZyiDropSystemCanvas();
};

#endif /* DROP_SYSTEM_CANVAS_H */

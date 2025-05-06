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
	uint64_t grid_cell_count;
	uint64_t drop_item_count;
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
	uint16_t get_picking_item_count() const;

	_ALWAYS_INLINE_ Size2i get_coord_size(const Rect2i &p_rect) const;
	_ALWAYS_INLINE_ Rect2i get_coord_range_in_rect(const Rect2 &p_rect) const;
	_ALWAYS_INLINE_ Rect2i get_coord_range_in_circle(const Vector2 &p_center, double p_radius) const;
	_ALWAYS_INLINE_ Vector2i get_grid_coord(const Vector2i &p_pos) const;
	_ALWAYS_INLINE_ bool is_grid_coord_valid(const Vector2i &p_coord) const;
	_ALWAYS_INLINE_ int64_t get_grid_index(const Vector2i &p_pos) const;
	_ALWAYS_INLINE_ int64_t get_grid_index_by_coord(const Vector2i &p_coord) const;
	_ALWAYS_INLINE_ int64_t calc_grid_index(const Vector2i &p_coord) const;
	_ALWAYS_INLINE_ int64_t calc_grid_index(int32_t x, int32_t y) const;

	void update_world_grid(const Rect2i &p_world_rect, const Size2i &p_grid_cell_size);
	void clear();
	void clear_world();
	_ALWAYS_INLINE_ void draw_in_space(const Rect2 &p_space);
	_ALWAYS_INLINE_ InternalDrawResult draw_animating_item_in_space(const Rect2 &p_space, InternalPickedDropItem &p_item, uint64_t p_cur_time);
	_ALWAYS_INLINE_ void draw_item_in_space(const Rect2 &p_texture_rect, const Vector2 &p_pos);
	_ALWAYS_INLINE_ void apply_item_pick_finished(InternalPickedDropItem &item);
	void idle_process_pick(double p_delta, Node2D *p_picker_node, double p_pick_range);
	int64_t pick_all(Node2D *p_picker_node, double p_pick_range);
	void clean_items();
	_ALWAYS_INLINE_ void apply_item_pick(const InternalDropItem &p_item, Node2D *p_picker_node);
	void add_drop_item(const Rect2 &p_texture_rect, const Vector2 &p_pos, const Array &p_data);

	virtual void handle_pick_start(const Array &p_data, Ref<WeakRef> p_picker_node_ref);
	GDVIRTUAL2C(_handle_pick_start, Array, Ref<WeakRef>);
	virtual void handle_pick_finished(const Array &p_data, Ref<WeakRef> p_picker_node_ref);
	GDVIRTUAL2C(_handle_pick_finished, Array, Ref<WeakRef>);
	virtual void handle_pick_finished_clean();
	GDVIRTUAL0C(_handle_pick_finished_clean);

	~ZyiDropSystemCanvas();
};

#endif /* DROP_SYSTEM_CANVAS_H */

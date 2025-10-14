#ifndef MOVE_BOIDS_GRID_H
#define MOVE_BOIDS_GRID_H

#include "core/math/rect2i.h"
#include "core/math/vector2.h"
#include "core/math/vector2i.h"
#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/os/memory.h"
#include "core/templates/hash_map.h"
#include "core/templates/hash_set.h"
#include "core/variant/typed_array.h"
#include "core/variant/variant_utility.h"
#include <algorithm>

class ZyiMoveBoidsGrid : public RefCounted {
	GDCLASS(ZyiMoveBoidsGrid, RefCounted)

protected:
	static void _bind_methods();

public:
	// 网格空间大小，网格空间原点为左上角
	Rect2i *grid_space_list = nullptr;
	// 网格空间数量
	uint8_t grid_space_count;
	// 单个网格大小
	Size2i grid_cell_size;
	// 网格中物体数量映射
	uint16_t *boid_grid = nullptr;
	// 网格中物体数量映射大小
	uint64_t boid_cell_count;
	// 物体位置映射
	HashMap<ObjectID, int64_t *> boid_object_pos_list_map;

	static Ref<ZyiMoveBoidsGrid> create(const TypedArray<Rect2i> &p_grid_space_list, const Size2i &p_grid_cell_size);
	void init(const TypedArray<Rect2i> &p_grid_space_list, const Size2i &p_grid_cell_size);
	void update_space_list(const TypedArray<Rect2i> &p_grid_space_list);
	void clear();
	void update_object_leave(ObjectID p_object_id);
	void update_object_map(ObjectID p_object_id, const Vector2 &p_pos);
	// 计算排斥力
	Vector2 get_repulsive_force(ObjectID p_object_id, const Vector2 &p_pos) const;

	_ALWAYS_INLINE_ Vector2i get_grid_coord(uint8_t p_space_index, const Vector2i &p_pos) const {
		Rect2i grid_space = grid_space_list[p_space_index];
		Vector2 pos = p_pos - grid_space.position;
		return Vector2i(pos.x / grid_cell_size.x, pos.y / grid_cell_size.y);
	}

	_ALWAYS_INLINE_ Size2i get_space_coord_size(const Rect2i &space) const {
		return Size2i(VariantUtilityFunctions::ceili(space.size.x / grid_cell_size.x), VariantUtilityFunctions::ceili(space.size.y / grid_cell_size.y));
	}

	_ALWAYS_INLINE_ bool is_grid_coord_valid(const Size2i &p_space_coord_size, const Vector2i &p_coord) const {
		return p_coord.x >= 0 && p_coord.x < p_space_coord_size.x && p_coord.y >= 0 && p_coord.y < p_space_coord_size.y;
	}

	// 需要扩大边界，保证物体在space交叉处时，只用计算一次
	_ALWAYS_INLINE_ Rect2i normalize_space(const Rect2i &p_grid_space, const Size2i &p_grid_cell_size) const {
		return Rect2i(p_grid_space.position - p_grid_cell_size, p_grid_space.size + p_grid_cell_size * 2);
	}

	_ALWAYS_INLINE_ int64_t get_grid_index(uint8_t p_space_index, const Vector2i &p_pos) const {
		return get_grid_index_by_coord(p_space_index, get_grid_coord(p_space_index, p_pos));
	}

	_ALWAYS_INLINE_ int64_t get_grid_index_by_coord(uint8_t p_space_index, const Vector2i &p_coord) const {
		Rect2i grid_space = grid_space_list[p_space_index];
		Size2i space_coord_size = get_space_coord_size(grid_space);
		if (!is_grid_coord_valid(space_coord_size, p_coord)) {
			return -1;
		}
		int64_t start_index = 0;
		for (int i = 0; i < p_space_index; i++) {
			Size2i space_coord_size = get_space_coord_size(grid_space_list[i]);
			start_index += space_coord_size.x * space_coord_size.y;
		}
		return start_index + p_coord.x + p_coord.y * space_coord_size.x;
	}
	_ALWAYS_INLINE_ void clear_boid_object_pos_list_map() {
		for (KeyValue<ObjectID, int64_t *> &item : boid_object_pos_list_map) {
			memfree(item.value);
		}
		boid_object_pos_list_map.clear();
	}

	~ZyiMoveBoidsGrid();
};

#endif /* MOVE_BOIDS_GRID_H */

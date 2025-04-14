#include "move_boids_grid.h"

const real_t MIN_DIST = 0.001f;
// sobel 权重矩阵
const int gx_weight[3][3] = {
	{ -1, 0, 1 },
	{ -2, 0, 2 },
	{ -1, 0, 1 },
};
const int gy_weight[3][3] = {
	{ -1, -2, -1 },
	{ 0, 0, 0 },
	{ 1, 2, 1 },
};
void ZyiMoveBoidsGrid::_bind_methods() {
	ClassDB::bind_static_method("ZyiMoveBoidsGrid", D_METHOD("create", "grid_space_list", "grid_cell_size"), &ZyiMoveBoidsGrid::create);
	ClassDB::bind_method(D_METHOD("init", "grid_space_list", "grid_cell_size"), &ZyiMoveBoidsGrid::init);
	ClassDB::bind_method(D_METHOD("update_space_list", "grid_space_list"), &ZyiMoveBoidsGrid::update_space_list);
	ClassDB::bind_method(D_METHOD("clear"), &ZyiMoveBoidsGrid::clear);
	ClassDB::bind_method(D_METHOD("update_object_map", "object_id", "pos", "origin_pos"), &ZyiMoveBoidsGrid::update_object_map);
	ClassDB::bind_method(D_METHOD("get_repulsive_force", "object_id", "pos"), &ZyiMoveBoidsGrid::get_repulsive_force);
}

Ref<ZyiMoveBoidsGrid> ZyiMoveBoidsGrid::create(const TypedArray<Rect2i> &p_grid_space_list, const Size2i &p_grid_cell_size) {
	Ref<ZyiMoveBoidsGrid> result = memnew(ZyiMoveBoidsGrid());
	result->init(p_grid_space_list, p_grid_cell_size);
	return result;
}

_FORCE_INLINE_ Vector2i ZyiMoveBoidsGrid::get_grid_coord(uint8_t p_space_index, const Vector2i &p_pos) const {
	Rect2i grid_space = grid_space_list[p_space_index];
	Vector2 pos = p_pos - grid_space.position;
	return Vector2i(pos.x / grid_cell_size.x, pos.y / grid_cell_size.y);
}

_ALWAYS_INLINE_ bool ZyiMoveBoidsGrid::is_grid_coord_valid(const Rect2i &p_grid_space, const Vector2i &p_coord) const {
	return p_coord.x >= 0 && p_coord.x < p_grid_space.size.x && p_coord.y >= 0 && p_coord.y < p_grid_space.size.y;
}

_ALWAYS_INLINE_ Rect2i ZyiMoveBoidsGrid::normalize_space(const Rect2i &p_grid_space, const Size2i &p_grid_cell_size) const {
	return Rect2i(p_grid_space.position - p_grid_cell_size, p_grid_space.size + p_grid_cell_size * 2);
}

_FORCE_INLINE_ int64_t ZyiMoveBoidsGrid::get_grid_index(uint8_t p_space_index, const Vector2i &p_pos) const {
	return get_grid_index_by_coord(p_space_index, get_grid_coord(p_space_index, p_pos));
}

_FORCE_INLINE_ int64_t ZyiMoveBoidsGrid::get_grid_index_by_coord(uint8_t p_space_index, const Vector2i &p_coord) const {
	Rect2i grid_space = grid_space_list[p_space_index];
	if (!is_grid_coord_valid(grid_space, p_coord)) {
		return -1;
	}
	int64_t start_index = 0;
	for (int i = 0; i < p_space_index; i++) {
		Size2i grid_space_size = grid_space_list[i].size;
		start_index += grid_space_size.x * grid_space_size.y;
	}
	return start_index + p_coord.x + p_coord.y * grid_space.size.x;
}

void ZyiMoveBoidsGrid::init(const TypedArray<Rect2i> &p_grid_space_list, const Size2i &p_grid_cell_size) {
	grid_space_count = p_grid_space_list.size();
	grid_space_list = (Rect2i *)memalloc(sizeof(Rect2i) * grid_space_count);
	boid_cell_count = 0;
	for (int i = 0; i < grid_space_count; i++) {
		Rect2i rect = normalize_space(p_grid_space_list[i], p_grid_cell_size);
		grid_space_list[i] = rect;
		boid_cell_count += rect.size.x * rect.size.y;
	}
	grid_cell_size = p_grid_cell_size;
	boid_grid = (uint16_t *)memalloc(sizeof(uint16_t) * boid_cell_count);
}

void ZyiMoveBoidsGrid::update_space_list(const TypedArray<Rect2i> &p_grid_space_list) {
	uint8_t space_count = p_grid_space_list.size();
	if (grid_space_count != space_count) {
		clear();
		init(p_grid_space_list, grid_cell_size);
	} else {
		boid_object_set.clear();
		uint64_t cell_count = 0;
		for (int i = 0; i < grid_space_count; i++) {
			Rect2i rect = normalize_space(p_grid_space_list[i], grid_cell_size);
			grid_space_list[i] = rect;
			cell_count += rect.size.x * rect.size.y;
		}
		if (boid_cell_count != cell_count) {
			// 大小一致，需要重新分配
			boid_cell_count = cell_count;
			boid_grid = (uint16_t *)memrealloc(boid_grid, sizeof(uint16_t) * boid_cell_count);
		} else {
			// 大小一致，重置为0
			memset(boid_grid, 0, sizeof(uint16_t) + boid_cell_count);
		}
	}
}

void ZyiMoveBoidsGrid::clear() {
	grid_space_count = 0;
	boid_cell_count = 0;
	if (grid_space_list != nullptr) {
		memfree(grid_space_list);
		grid_space_list = nullptr;
	}
	if (boid_grid != nullptr) {
		memfree(boid_grid);
		boid_grid = nullptr;
	}
	boid_object_set.clear();
}

void ZyiMoveBoidsGrid::update_object_map(ObjectID p_object_id, const Vector2 &p_pos, const Vector2 &p_origin_pos) {
	if (p_object_id.is_null()) {
		return;
	}
	bool recorded = boid_object_set.has(p_object_id);
	for (int i = 0; i < grid_space_count; i++) {
		int64_t grid_index = get_grid_index(i, p_pos);
		int64_t origin_grid_index = get_grid_index(i, p_origin_pos);
		if ((grid_index < 0 && origin_grid_index < 0) || recorded && grid_index == origin_grid_index) {
			continue;
		}
		if (recorded && origin_grid_index >= 0 && boid_grid[origin_grid_index] > 0) {
			// 移除
			boid_grid[origin_grid_index]--;
		}
		if (grid_index >= 0) {
			// 添加
			boid_grid[grid_index]++;
		}
		if (!recorded) {
			boid_object_set.insert(p_object_id);
		}
	}
}

Vector2 ZyiMoveBoidsGrid::get_repulsive_force(ObjectID p_object_id, const Vector2 &p_pos) const {
	Vector2 result = Vector2(0, 0);
	if (p_object_id.is_null()) {
		return result;
	}
	if (!boid_object_set.has(p_object_id)) {
		return result;
	}
	// 确定要使用的网格空间
	Vector2i coord;
	int16_t space_index = -1;
	Rect2i grid_space;
	for (int i = 0; i < grid_space_count; i++) {
		coord = get_grid_coord(i, p_pos);
		grid_space = grid_space_list[i];
		if (is_grid_coord_valid(grid_space, coord)) {
			space_index = i;
			break;
		}
	}
	if (space_index < 0) {
		return result;
	}
	// 计算力
	Size2i grid_space_size = grid_space.size;
	int32_t w = grid_space_size.x;
	int32_t h = grid_space_size.y;
	float gx = 0.0f;
	float gy = 0.0f;
	// 计算当前坐标相对于所在网格中心的偏移
	// 9 宫格，计算 8 个方位的邻居
	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			Vector2i neighbor_coord = coord + Vector2i(dx, dy);
			if (neighbor_coord.x < 0 || neighbor_coord.x >= w || neighbor_coord.y < 0 || neighbor_coord.y >= h) {
				// 跳过边界
				continue;
			}
			int64_t neighbor_grid_index = get_grid_index_by_coord(space_index, neighbor_coord);
			uint16_t boid_count = boid_grid[neighbor_grid_index];
			if (boid_count <= 0) {
				continue;
			}
			gx += boid_count * gx_weight[dy + 1][dx + 1];
			gy += boid_count * gy_weight[dy + 1][dx + 1];
		}
	}
	result = Vector2(-gx, -gy);
	return result;
}

ZyiMoveBoidsGrid::~ZyiMoveBoidsGrid() {
	clear();
}

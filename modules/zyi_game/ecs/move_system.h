#ifndef MOVE_SYSTEM_H
#define MOVE_SYSTEM_H

#include "core/math/math_funcs.h"
#include "core/object/ref_counted.h"
#include "move_boids_grid.h"
#include "move_component.h"
#include <memory>
#include <vector>

template <typename T>
class ZyiMoveSystemInternalComponentPool {
public:
	std::vector<int64_t> released_index_pool;
	std::vector<T> pool;
	int64_t acquire_component();
	void release_component(int64_t id);
	T *get_component_ptr(int64_t id);
	void release_all();
};

class ZyiMoveSystem : public RefCounted {
	GDCLASS(ZyiMoveSystem, RefCounted)

protected:
	static void _bind_methods();

public:
	enum {
		NORMAL_MOVE_INIT_CAPACITY = 128,
		CHARACTER_MOVE_INIT_CAPACITY = 128,
		KNOCKBACK_MOVE_INIT_CAPACITY = 64,
	};
	static const double MIN_ROTATION_ANGLE;

	Ref<ZyiMoveBoidsGrid> boids_grid;
	ZyiMoveSystemInternalComponentPool<ZyiNormalMoveComponent> normal_move_component_pool;
	ZyiMoveSystemInternalComponentPool<ZyiCharacterMoveComponent> character_move_component_pool;
	ZyiMoveSystemInternalComponentPool<ZyiKnockbackMoveComponent> knockback_move_component_pool;

	static Ref<ZyiMoveSystem> create(int p_normal_move_capacity = NORMAL_MOVE_INIT_CAPACITY, int p_character_move_capacity = CHARACTER_MOVE_INIT_CAPACITY, int p_knockback_move_capacity = KNOCKBACK_MOVE_INIT_CAPACITY);
	int64_t acquire_normal_move_component();
	void release_normal_move_component(int64_t p_id);
	ZyiNormalMoveComponent *get_normal_move_component_ptr(int64_t p_id);
	int64_t acquire_character_move_component();
	void release_character_move_component(int64_t p_id);
	ZyiCharacterMoveComponent *get_character_move_component_ptr(int64_t p_id);
	int64_t acquire_knockback_move_component();
	void release_knockback_move_component(int64_t p_id);
	ZyiKnockbackMoveComponent *get_knockback_move_component_ptr(int64_t p_id);
	void set_boids_grid(Ref<ZyiMoveBoidsGrid> p_boids_grid);

	void idle_process_update(double p_delta);
	void idle_physics_process_update(double p_delta);
	void clean();
	static _ALWAYS_INLINE_ double lerp_velocity_like_rate(double p_cur_rate, double p_target_rate, double p_delta, double p_acceleration_rate) {
		return Math::lerp(p_cur_rate, p_target_rate, 1.0 - Math::exp(-p_delta * p_acceleration_rate));
	}
};

#endif /* MOVE_SYSTEM_H */

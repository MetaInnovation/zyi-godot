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
	_ALWAYS_INLINE_ void idle_process_update_normal_move(double p_delta) {
		Node2D *node;
		for (ZyiNormalMoveComponent &component : normal_move_component_pool.pool) {
			node = component.move_node;
			if (!node || node->is_queued_for_deletion()) {
				continue;
			}
			bool need_update_boids_grid = component.is_in_boid_grid() && boids_grid.is_valid();
			if (!node->is_inside_tree()) {
				if (need_update_boids_grid) {
					boids_grid->update_object_leave(node->get_instance_id());
				}
				continue;
			}
			Point2 origin_pos = node->get_global_position();
			if (!component.check_can_move()) {
				if (need_update_boids_grid) {
					boids_grid->update_object_map(node->get_instance_id(), origin_pos);
				}
				continue;
			}
			if (component.use_preset_pos_for_single_frame) {
				component.use_preset_pos_for_single_frame = false;
				node->set_global_position(component.preset_pos);
			} else {
				node->set_global_position(origin_pos + component.resolve_velocity() * p_delta);
			}
			Point2 pos = node->get_global_position();
			if (need_update_boids_grid) {
				boids_grid->update_object_map(node->get_instance_id(), pos);
			}
		}
		for (ZyiKnockbackMoveComponent &component : knockback_move_component_pool.pool) {
			node = component.move_node;
			if (!node || node->is_queued_for_deletion() || !node->is_inside_tree()) {
				continue;
			}
			if (!component.moving) {
				continue;
			}
			Point2 origin_pos = node->get_global_position();
			node->set_global_position(origin_pos + component.resolve_knockback_velocity() * p_delta);
		}
	}

	_ALWAYS_INLINE_ void idle_process_update_character_move(double p_delta) {
		CharacterBody2D *character_body;
		for (ZyiCharacterMoveComponent &component : character_move_component_pool.pool) {
			character_body = component.move_node;
			if (!character_body || character_body->is_queued_for_deletion() || !character_body->is_inside_tree()) {
				continue;
			}
			bool need_update_boids_grid = component.is_in_boid_grid() && boids_grid.is_valid();
			if (!character_body->is_inside_tree()) {
				if (need_update_boids_grid) {
					boids_grid->update_object_leave(character_body->get_instance_id());
				}
				continue;
			}
			Point2 origin_pos = character_body->get_global_position();
			Vector2 velocity = component.resolve_velocity() + component.resolve_knockback_velocity();
			if ((!component.check_can_move() && !component.knockback_moving) || velocity.is_zero_approx()) {
				if (need_update_boids_grid) {
					boids_grid->update_object_map(character_body->get_instance_id(), origin_pos);
				}
				continue;
			}
			if (component.flags & ZyiMoveConstant::MOVE_FLAG_WITHOUT_CHARACTER_COLLIDE) {
				// 直接移动
				character_body->set_global_position(origin_pos + velocity * p_delta);
			} else if (PhysicsServer2D::get_singleton()->body_get_space(character_body->get_rid()).is_valid()) {
				character_body->set_velocity(velocity);
				character_body->move_and_slide();
			}
			Point2 pos = character_body->get_global_position();
			if (need_update_boids_grid) {
				boids_grid->update_object_map(character_body->get_instance_id(), pos);
			}
		}
	}

	_ALWAYS_INLINE_ void idle_physics_process_update_normal_move(double p_delta) {
		Node2D *node;
		Node2D *follow_target;
		uint64_t physics_frames = Engine::get_singleton()->get_physics_frames();
		for (ZyiNormalMoveComponent &component : normal_move_component_pool.pool) {
			node = component.move_node;
			if (!component.check_can_auto_update_velocity() || !node || node->is_queued_for_deletion() || !node->is_inside_tree()) {
				continue;
			}
			Vector2 self_pos = node->get_global_position();
			follow_target = component.follow_target;
			ZyiInternalMoveFollowResult follow_result = component.resolve_follow_result(p_delta, self_pos);
			Vector2 direction = component.cur_velocity.normalized();
			Vector2 origin_direction = Vector2(direction);
			double follow_dist_squared = -1.0;
			double dist_angle = -1.0;
			Vector2 follow_target_pos = follow_result.follow_target_position;
			Vector2 follow_direction = self_pos.direction_to(follow_target_pos);
			if (follow_result.valid_follow) {
				component.cur_rotation_rate = lerp_velocity_like_rate(component.cur_rotation_rate, component.max_rotation_rate, p_delta, component.rotation_acceleration_rate);
				double angle = direction.angle_to(follow_direction);
				if (Math::abs(angle) <= MIN_ROTATION_ANGLE || Math::abs(angle) < component.cur_rotation_rate) {
					direction = follow_direction;
				} else {
					direction = direction.rotated(SIGN(angle) * component.cur_rotation_rate);
				}
				dist_angle = direction.angle_to(follow_direction);
				if (Math::is_zero_approx(dist_angle)) {
					// 不再跟随
					component.last_follow_valid = false;
					component.force_stop_follow();
				}
			} else if (follow_result.can_follow) {
				Vector2 follow_direction = self_pos.direction_to(follow_target_pos);
				// 距离太近，则直接角度转过去
				direction = follow_direction;
				// 不再跟随
				component.last_follow_valid = false;
				component.force_stop_follow();
				dist_angle = 0;
			}
			double cur_velocity_rate = component.cur_velocity.length();
			cur_velocity_rate = lerp_velocity_like_rate(cur_velocity_rate, component.max_velocity_rate, p_delta, component.acceleration_rate);
			component.set_cur_velocity(direction * cur_velocity_rate);
			// 计算boid排斥力
			if (is_boid_idle_physics_process(physics_frames)) {
				Vector2 force = Vector2(0, 0);
				if (component.is_forced_in_boid_grid()) {
					force = resolve_extra_force(node->get_instance_id(), self_pos);
				}
				component.idle_update_extra_force(force, p_delta);
			}
			// 移动预测：如果移动后相当于远离，且方向夹角小于 90 度 距离也小于速度，则强制更新 global_position
			if (follow_dist_squared > 0.0 && !!component.follow_target) {
				bool is_leave = (self_pos + component.resolve_velocity() * p_delta).distance_squared_to(follow_target_pos) > follow_dist_squared;
				bool is_near_than_velocity = follow_dist_squared < component.cur_velocity.length_squared();
				if (is_leave && is_near_than_velocity && Math::abs(dist_angle) < Math_PI / 2.0) {
					component.use_preset_pos_for_single_frame = true;
					component.preset_pos = follow_target_pos;
					component.last_follow_valid = false;
					// 不再跟随
					component.force_stop_follow();
					component.set_cur_velocity(origin_direction * cur_velocity_rate);
				}
			}
		}
		for (ZyiKnockbackMoveComponent &component : knockback_move_component_pool.pool) {
			node = component.move_node;
			if (!component.moving || !node || node->is_queued_for_deletion() || !node->is_inside_tree()) {
				continue;
			}
			Vector2 direction = component.cur_knockback_velocity.normalized();
			double cur_velocity_rate = component.cur_knockback_velocity.length();
			cur_velocity_rate = lerp_velocity_like_rate(cur_velocity_rate, 0, p_delta, component.knockback_deceleration_rate);
			if (Math::is_zero_approx(cur_velocity_rate * p_delta)) {
				// 结束击退
				component.set_knockback_moving(false);
			} else {
				component.set_cur_knockback_velocity(direction * cur_velocity_rate);
			}
		}
	}

	_ALWAYS_INLINE_ void idle_physics_process_update_character_move(double p_delta) {
		Node2D *follow_target;
		CharacterBody2D *character_body;
		uint64_t physics_frames = Engine::get_singleton()->get_physics_frames();
		for (ZyiCharacterMoveComponent &component : character_move_component_pool.pool) {
			character_body = component.move_node;
			if ((!component.check_can_auto_update_velocity() && !component.knockback_moving) || !character_body || character_body->is_queued_for_deletion() || !character_body->is_inside_tree()) {
				continue;
			}
			Vector2 self_pos = character_body->get_global_position();
			// 移动
			follow_target = component.follow_target;
			Vector2 direction = component.cur_velocity.normalized();
			double cur_velocity_rate = component.cur_velocity.length();
			// 应用跟随
			ZyiInternalMoveFollowResult follow_result = component.resolve_follow_result(p_delta, self_pos);
			Vector2 follow_target_pos = follow_result.follow_target_position;
			if (follow_result.valid_follow) {
				Vector2 follow_direction = character_body->get_global_position().direction_to(follow_target_pos);
				direction = follow_direction;
			} else if (follow_result.can_follow) {
				if (component.flags & ZyiMoveConstant::MOVE_FLAG_CHARACTER_FOLLOW_POSITIVE) {
					// 慢速插值
					direction = direction.lerp(Vector2(0, 0), p_delta);
				} else {
					// 快速插值
					float weight = 8.0f * p_delta;
					if (weight > 1.0) {
						weight = 1.0;
					}
					direction = direction.lerp(Vector2(0, 0), weight);
				}
			}
			cur_velocity_rate = lerp_velocity_like_rate(cur_velocity_rate, component.max_velocity_rate, p_delta, component.acceleration_rate);
			component.set_cur_velocity(direction * cur_velocity_rate);
			// 计算boid排斥力
			if (is_boid_idle_physics_process(physics_frames)) {
				Vector2 force = Vector2(0, 0);
				if (component.is_forced_in_boid_grid()) {
					force = resolve_extra_force(character_body->get_instance_id(), self_pos);
				}
				component.idle_update_extra_force(force, p_delta);
			}
			// 击退
			Vector2 knockback_direction = component.cur_knockback_velocity.normalized();
			double cur_knockback_velocity_rate = component.cur_knockback_velocity.length();
			cur_knockback_velocity_rate = lerp_velocity_like_rate(cur_knockback_velocity_rate, 0, p_delta, component.knockback_deceleration_rate);
			if (Math::is_zero_approx(cur_knockback_velocity_rate * p_delta)) {
				// 结束击退
				component.set_knockback_moving(false);
			} else {
				component.set_cur_knockback_velocity(knockback_direction * cur_knockback_velocity_rate);
			}
		}
	}

	_ALWAYS_INLINE_ bool is_boid_idle_physics_process(uint64_t p_physics_frames) {
		return boids_grid.is_valid();
	}

	_ALWAYS_INLINE_ Vector2 resolve_extra_force(const ObjectID &p_object_id, const Vector2 &p_pos) const {
		Vector2 force = boids_grid->get_repulsive_force(p_object_id, p_pos);
		return force;
	}
};

#endif /* MOVE_SYSTEM_H */

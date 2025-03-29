#include "move_system.h"

template <typename T>
inline int64_t ZyiMoveSystemInternalComponentPool<T>::acquire_component() {
	if (released_index_pool.empty()) {
		pool.emplace_back();
		return pool.size() - 1;
	}
	int64_t result = released_index_pool.back();
	released_index_pool.pop_back();
	return result;
}

template <typename T>
void ZyiMoveSystemInternalComponentPool<T>::release_component(int64_t id) {
	released_index_pool.push_back(id);
}

template <typename T>
T *ZyiMoveSystemInternalComponentPool<T>::get_component_ptr(int64_t id) {
	if (id >= 0 && id < static_cast<int64_t>(pool.size())) {
		return &pool[id];
	}
	return nullptr;
}

template <typename T>
void ZyiMoveSystemInternalComponentPool<T>::release_all() {
	pool.clear();
}

const double ZyiMoveSystem::MIN_ROTATION_ANGLE = 0.01;

void ZyiMoveSystem::_bind_methods() {
	ClassDB::bind_static_method("ZyiMoveSystem", D_METHOD("create", "normal_move_capacity", "character_move_capacity", "knockback_move_capacity"), &ZyiMoveSystem::create, DEFVAL(NORMAL_MOVE_INIT_CAPACITY), DEFVAL(CHARACTER_MOVE_INIT_CAPACITY), DEFVAL(KNOCKBACK_MOVE_INIT_CAPACITY));
	ClassDB::bind_method(D_METHOD("idle_process_update", "delta"), &ZyiMoveSystem::idle_process_update);
	ClassDB::bind_method(D_METHOD("idle_physics_process_update", "delta"), &ZyiMoveSystem::idle_physics_process_update);
	ClassDB::bind_method(D_METHOD("clean"), &ZyiMoveSystem::clean);

	BIND_CONSTANT(NORMAL_MOVE_INIT_CAPACITY);
	BIND_CONSTANT(CHARACTER_MOVE_INIT_CAPACITY);
	BIND_CONSTANT(KNOCKBACK_MOVE_INIT_CAPACITY);
}

Ref<ZyiMoveSystem> ZyiMoveSystem::create(int p_normal_move_capacity, int p_character_move_capacity, int p_knockback_move_capacity) {
	Ref<ZyiMoveSystem> result = memnew(ZyiMoveSystem());
	result->normal_move_component_pool.pool.reserve(p_normal_move_capacity);
	result->character_move_component_pool.pool.reserve(p_character_move_capacity);
	result->knockback_move_component_pool.pool.reserve(p_knockback_move_capacity);
	return result;
}

int64_t ZyiMoveSystem::acquire_normal_move_component() {
	return normal_move_component_pool.acquire_component();
}

void ZyiMoveSystem::release_normal_move_component(int64_t p_id) {
	ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr(p_id);
	if (ptr) {
		ptr->reset();
		normal_move_component_pool.release_component(p_id);
	}
}

ZyiNormalMoveComponent *ZyiMoveSystem::get_normal_move_component_ptr(int64_t p_id) {
	return normal_move_component_pool.get_component_ptr(p_id);
}

int64_t ZyiMoveSystem::acquire_character_move_component() {
	return character_move_component_pool.acquire_component();
}

void ZyiMoveSystem::release_character_move_component(int64_t p_id) {
	ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr(p_id);
	if (ptr) {
		ptr->reset();
		character_move_component_pool.release_component(p_id);
	}
}

ZyiCharacterMoveComponent *ZyiMoveSystem::get_character_move_component_ptr(int64_t p_id) {
	return character_move_component_pool.get_component_ptr(p_id);
}

int64_t ZyiMoveSystem::acquire_knockback_move_component() {
	return knockback_move_component_pool.acquire_component();
}

void ZyiMoveSystem::release_knockback_move_component(int64_t p_id) {
	ZyiKnockbackMoveComponent *ptr = get_knockback_move_component_ptr(p_id);
	if (ptr) {
		ptr->reset();
		knockback_move_component_pool.release_component(p_id);
	}
}

ZyiKnockbackMoveComponent *ZyiMoveSystem::get_knockback_move_component_ptr(int64_t p_id) {
	return knockback_move_component_pool.get_component_ptr(p_id);
}

void ZyiMoveSystem::idle_process_update(double p_delta) {
	Node2D *node;
	for (ZyiNormalMoveComponent &component : normal_move_component_pool.pool) {
		node = component.move_node;
		if (!component.check_can_move() || !node || node->is_queued_for_deletion() || !node->is_inside_tree()) {
			continue;
		}
		if (component.use_preset_pos_for_single_frame) {
			component.use_preset_pos_for_single_frame = false;
			node->set_global_position(component.preset_pos);
		} else {
			node->set_global_position(node->get_global_position() + component.resolve_velocity() * p_delta);
		}
	}
	for (ZyiKnockbackMoveComponent &component : knockback_move_component_pool.pool) {
		node = component.move_node;
		if (!component.moving || !node || node->is_queued_for_deletion() || !node->is_inside_tree()) {
			continue;
		}
		node->set_global_position(node->get_global_position() + component.resolve_knockback_velocity() * p_delta);
	}
	CharacterBody2D *character_body;
	for (ZyiCharacterMoveComponent &component : character_move_component_pool.pool) {
		character_body = component.move_node;
		if ((!component.check_can_move() && !component.knockback_moving) || !character_body || character_body->is_queued_for_deletion() || !character_body->is_inside_tree()) {
			continue;
		}
		if (character_body->get_velocity().is_zero_approx()) {
			continue;
		}
		// TODO 改为 start 前判断？
		if (PhysicsServer2D::get_singleton()->body_get_space(character_body->get_rid()).is_valid()) {
			character_body->move_and_slide();
		}
	}
}

void ZyiMoveSystem::idle_physics_process_update(double p_delta) {
	Node2D *node;
	Node2D *follow_target;
	for (ZyiNormalMoveComponent &component : normal_move_component_pool.pool) {
		node = component.move_node;
		if (!component.check_can_move() || !node || node->is_queued_for_deletion() || !node->is_inside_tree()) {
			continue;
		}
		follow_target = component.follow_target;
		Vector2 follow_target_pos;
		bool can_follow = false;
		if (follow_target && !follow_target->is_queued_for_deletion() && follow_target->is_visible_in_tree()) {
			follow_target_pos = follow_target->get_global_position();
			component.last_follow_valid = true;
			component.last_follow_pos = follow_target_pos;
			can_follow = true;
		} else if (component.last_follow_valid) {
			follow_target_pos = component.last_follow_pos;
			component.last_follow_valid = false;
			can_follow = true;
		}
		Vector2 direction = component.cur_velocity.normalized();
		Vector2 node_pos = node->get_global_position();
		double follow_dist_squared = -1.0;
		double dist_angle = -1.0;
		if (can_follow) {
			follow_dist_squared = node_pos.distance_squared_to(follow_target_pos);
			Vector2 follow_direction = node_pos.direction_to(follow_target_pos);
			if (follow_dist_squared <= component.min_rotation_follow_dist_squared) {
				// 如果距离太近，则直接角度转过去
				direction = follow_direction;
				// 不再跟随
				component.force_stop_follow();
			} else {
				component.cur_rotation_rate = lerp_velocity_like_rate(component.cur_rotation_rate, component.max_rotation_rate, p_delta, component.rotation_acceleration_rate);
				double angle = direction.angle_to(follow_direction);
				if (angle <= MIN_ROTATION_ANGLE || angle < component.cur_rotation_rate) {
					direction = follow_direction;
				} else {
					direction = direction.rotated(component.cur_rotation_rate);
				}
				if (Math::is_zero_approx(dist_angle)) {
					// 不再跟随
					component.force_stop_follow();
				}
			}
			dist_angle = direction.angle_to(follow_direction);
		}
		double cur_velocity_rate = component.cur_velocity.length();
		cur_velocity_rate = lerp_velocity_like_rate(cur_velocity_rate, component.max_velocity_rate, p_delta, component.acceleration_rate);
		component.set_cur_velocity(direction * cur_velocity_rate);
		// 移动预测：如果移动后相当于远离，且方向夹角小于 90 度 距离也小于速度，则强制更新 global_position
		if (follow_dist_squared > 0.0) {
			bool is_leave = (node_pos + component.cur_velocity * p_delta).distance_squared_to(follow_target_pos) > follow_dist_squared;
			bool is_near_than_velocity = follow_dist_squared < component.cur_velocity.length_squared();
			if (is_leave && is_near_than_velocity && Math::abs(dist_angle) < Math_PI / 2.0) {
				component.use_preset_pos_for_single_frame = true;
				component.preset_pos = follow_target_pos;
				// 不再跟随
				component.force_stop_follow();
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
	CharacterBody2D *character_body;
	for (ZyiCharacterMoveComponent &component : character_move_component_pool.pool) {
		character_body = component.move_node;
		if ((!component.check_can_move() && !component.knockback_moving) || !character_body || character_body->is_queued_for_deletion() || !character_body->is_inside_tree()) {
			continue;
		}
		// 移动
		follow_target = component.follow_target;
		Vector2 direction = component.cur_velocity.normalized();
		double cur_velocity_rate = component.cur_velocity.length();
		if (follow_target && !follow_target->is_queued_for_deletion()) {
			direction = character_body->get_global_position().direction_to(follow_target->get_global_position());
		}
		if (direction.is_zero_approx()) {
			continue;
		}
		cur_velocity_rate = lerp_velocity_like_rate(cur_velocity_rate, component.max_velocity_rate, p_delta, component.acceleration_rate);
		component.set_cur_velocity(direction * cur_velocity_rate);
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
		// 速度与击退速度叠加
		character_body->set_velocity(component.resolve_velocity() + component.resolve_knockback_velocity());
	}
}

void ZyiMoveSystem::clean() {
	for (size_t i = 0; i < normal_move_component_pool.pool.size(); i++) {
		ZyiNormalMoveComponent &component = normal_move_component_pool.pool[i];
		component.reset();
	}
	normal_move_component_pool.release_all();
	for (size_t i = 0; i < character_move_component_pool.pool.size(); i++) {
		ZyiCharacterMoveComponent &component = character_move_component_pool.pool[i];
		component.reset();
	}
	character_move_component_pool.release_all();
	for (size_t i = 0; i < knockback_move_component_pool.pool.size(); i++) {
		ZyiKnockbackMoveComponent &component = knockback_move_component_pool.pool[i];
		component.reset();
	}
	knockback_move_component_pool.release_all();
}

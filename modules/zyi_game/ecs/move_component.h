#ifndef MOVE_COMPONENT_H
#define MOVE_COMPONENT_H

#include "core/object/ref_counted.h"
#include "move_constant.h"
#include "scene/2d/node_2d.h"
#include "scene/2d/physics/character_body_2d.h"

enum ZyiMoveNodeType {
	MOVE_NODE_TYPE_NORMAL,
	MOVE_NODE_TYPE_CHARACTER_BODY_2D,
};

struct ZyiInternalMoveFollowResult {
	Vector2 follow_target_position;
	bool can_follow;
	bool valid_follow;
};

struct ZyiMoveBasicComponent {
	bool moving = false;
	bool freezed = false;
	bool move_disabled = false;
	int32_t flags = ZyiMoveConstant::MOVE_FLAG_NORMAL;
	// 当前移动速度
	Vector2 cur_velocity;
	// 外部力
	Vector2 extra_force;
	// 跟随的物体，如果没有，则按照指定方向移动
	Node2D *follow_target = nullptr;
	// 跟随的偏移量
	Vector2 follow_offset;
	// 应用跟随的最小距离平方
	int64_t min_follow_dist_squared = 900;
	// 前一次跟随是否有效，决定是否使用 last_follow_pos
	bool last_follow_valid = false;
	// 前一次跟随的位置，当 follow_target 为 nullptr 时，使用该位置
	Vector2 last_follow_pos;
	// 最大移动速度——速度向量的最大长度
	double max_velocity_rate;
	// 移动速度放大比例——特殊情况时加持
	double velocity_scale_add_rate = 0.0;
	// 初始移动速度
	Vector2 initial_velocity;
	// 加速度——沿着速度方向的
	double acceleration_rate;
	Callable moving_changed_callback;
	Callable moved_callback;
	_ALWAYS_INLINE_ bool is_in_boid_grid() {
		return flags & (ZyiMoveConstant::MOVE_FLAG_BOID_GRID_CHILD | ZyiMoveConstant::MOVE_FLAG_BOID_GRID_STATIC);
	}
	_ALWAYS_INLINE_ bool is_forced_in_boid_grid() {
		return (flags & ZyiMoveConstant::MOVE_FLAG_BOID_GRID_CHILD) && !(flags & ZyiMoveConstant::MOVE_FLAG_UNFORCED);
	}
	_ALWAYS_INLINE_ bool is_proxy_by_manual() {
		return flags & ZyiMoveConstant::MOVE_FLAG_PROXY_BY_MANUAL;
	}
	_ALWAYS_INLINE_ void reset() {
		follow_target = nullptr;
		last_follow_valid = false;
		moving = false;
		freezed = false;
		move_disabled = false;
		velocity_scale_add_rate = 0.0;
		extra_force = Vector2(0, 0);
		moving_changed_callback = Callable();
		moved_callback = Callable();
	}
	_ALWAYS_INLINE_ bool check_can_move() {
		return moving && !freezed && !move_disabled;
	}
	_ALWAYS_INLINE_ bool check_can_auto_update_velocity() {
		return check_can_move() && !is_proxy_by_manual();
	}
	_ALWAYS_INLINE_ void set_freezed(bool value) {
		if (freezed == value) {
			return;
		}
		freezed = value;
		if (moving_changed_callback.is_valid()) {
			moving_changed_callback.call_deferred(moving && !freezed);
		}
	}
	_ALWAYS_INLINE_ void set_moving(bool value) {
		if (moving == value) {
			return;
		}
		moving = value;
		if (moving_changed_callback.is_valid()) {
			moving_changed_callback.call_deferred(moving && !freezed);
		}
	}
	_ALWAYS_INLINE_ void set_extra_force(const Vector2 &p_value) {
		extra_force = p_value;
	}
	_ALWAYS_INLINE_ void set_cur_velocity(const Vector2 &p_value) {
		Vector2 old_velocity = cur_velocity;
		cur_velocity = p_value;
		if (moved_callback.is_valid()) {
			moved_callback.call_deferred(cur_velocity, old_velocity);
		}
	}
	_ALWAYS_INLINE_ Vector2 resolve_velocity() {
		return cur_velocity * (1.0 + velocity_scale_add_rate) + extra_force;
	}
	_ALWAYS_INLINE_ double resolve_max_velocity_rate() {
		return max_velocity_rate * (1.0 + velocity_scale_add_rate);
	}
	_ALWAYS_INLINE_ ZyiInternalMoveFollowResult resolve_follow_result(const Vector2 &self_pos) {
		Vector2 follow_target_pos = self_pos;
		bool can_follow = false;
		bool valid_follow = false;
		if (follow_target && !follow_target->is_queued_for_deletion() && follow_target->is_visible_in_tree()) {
			last_follow_pos = follow_target->get_global_position();
			last_follow_valid = true;
			follow_target_pos = last_follow_pos + follow_offset;
			can_follow = true;
		} else if (last_follow_valid) {
			last_follow_valid = false;
			follow_target_pos = last_follow_pos + follow_offset;
			can_follow = true;
		}
		if (can_follow) {
			valid_follow = is_valid_follow_target_position(follow_target_pos, self_pos);
		}
		return ZyiInternalMoveFollowResult{ follow_target_pos, can_follow, valid_follow };
	}
	_ALWAYS_INLINE_ bool is_valid_follow_target_position(const Vector2 &follow_target_pos, const Vector2 &self_pos) {
		float dist = follow_target_pos.distance_squared_to(self_pos);
		return dist > resolve_velocity().length_squared() && dist > min_follow_dist_squared;
	}
};

struct ZyiNormalMoveComponent : public ZyiMoveBasicComponent {
	// 实际移动的物体
	Node2D *move_node;
	// 强行停止跟随的回调
	Callable force_stop_follow_callback;
	// 预设置的位置
	Vector2 preset_pos;
	// 是否使用预设置的位置
	bool use_preset_pos_for_single_frame;
	// 最大角速度
	double max_rotation_rate;
	// 初始角速度——垂直于速度方向的
	double initial_rotation_rate;
	// 初始角加速度
	double rotation_acceleration_rate;
	// 当前角速度——垂直于速度方向的
	double cur_rotation_rate;

	_ALWAYS_INLINE_ void reset() {
		ZyiMoveBasicComponent::reset();
		force_stop_follow_callback = Callable();
	}
	_ALWAYS_INLINE_ void force_stop_follow() {
		if (force_stop_follow_callback.is_valid()) {
			force_stop_follow_callback.call();
		}
		follow_target = nullptr;
	}
};

struct ZyiKnockbackMoveComponent {
	bool moving = false;
	// 实际移动的物体
	Node2D *move_node;
	// 当前击退速度
	Vector2 cur_knockback_velocity;
	// 初始击退速度
	Vector2 init_knockback_velocity;
	// 击退衰减速度，击退速度衰减到接近0时停止击退
	double knockback_deceleration_rate;
	Callable knockback_moving_changed_callback;
	ZyiMoveConstant::Flags flags = ZyiMoveConstant::MOVE_FLAG_NORMAL;

	_ALWAYS_INLINE_ bool is_in_boid_grid() {
		return flags & (ZyiMoveConstant::MOVE_FLAG_BOID_GRID_CHILD | ZyiMoveConstant::MOVE_FLAG_BOID_GRID_STATIC);
	}
	_ALWAYS_INLINE_ void reset() {
		move_node = nullptr;
		moving = false;
		knockback_moving_changed_callback = Callable();
	}
	_ALWAYS_INLINE_ void set_knockback_moving(bool value) {
		if (moving == value) {
			return;
		}
		moving = value;
		if (!moving) {
			cur_knockback_velocity = Vector2();
		}
		if (knockback_moving_changed_callback.is_valid()) {
			knockback_moving_changed_callback.call_deferred(moving);
		}
	}
	_ALWAYS_INLINE_ void set_cur_knockback_velocity(const Vector2 &value) {
		cur_knockback_velocity = value;
	}
	_ALWAYS_INLINE_ Vector2 resolve_knockback_velocity() {
		return cur_knockback_velocity;
	}
};

struct ZyiCharacterMoveComponent : public ZyiMoveBasicComponent {
	// 实际移动的物体
	CharacterBody2D *move_node;
	bool knockback_moving = false;
	// 当前击退速度
	Vector2 cur_knockback_velocity;
	// 初始击退速度
	Vector2 init_knockback_velocity;
	// 击退衰减速度，击退速度衰减到接近0时停止击退
	double knockback_deceleration_rate;
	Callable knockback_moving_changed_callback;

	_ALWAYS_INLINE_ void reset() {
		ZyiMoveBasicComponent::reset();
		knockback_moving = false;
		knockback_moving_changed_callback = Callable();
	}
	_ALWAYS_INLINE_ void set_knockback_moving(bool value) {
		if (knockback_moving == value) {
			return;
		}
		knockback_moving = value;
		if (!knockback_moving) {
			cur_knockback_velocity = Vector2();
		}
		if (knockback_moving_changed_callback.is_valid()) {
			knockback_moving_changed_callback.call_deferred(knockback_moving);
		}
	}
	_ALWAYS_INLINE_ void set_cur_knockback_velocity(const Vector2 &value) {
		cur_knockback_velocity = value;
	}
	_ALWAYS_INLINE_ Vector2 resolve_knockback_velocity() {
		return cur_knockback_velocity;
	}
};

#endif /* MOVE_COMPONENT_H */

#ifndef MOVE_COMPONENT_H
#define MOVE_COMPONENT_H

#include "core/object/ref_counted.h"
#include "scene/2d/node_2d.h"
#include "scene/2d/physics/character_body_2d.h"
#include <memory>
#include <type_traits>

enum ZyiMoveNodeType {
	MOVE_NODE_TYPE_NORMAL,
	MOVE_NODE_TYPE_CHARACTER_BODY_2D,
};

struct ZyiMoveBasicComponent {
	bool moving = false;
	bool freezed = false;
	bool move_disabled = false;
	// 当前移动速度
	Vector2 cur_velocity;
	// 跟随的物体，如果没有，则按照指定方向移动
	Node2D *follow_target = nullptr;
	// 最大移动速度——速度向量的最大长度
	double max_velocity_rate;
	// 移动速度放大比例——特殊情况时加持
	double velocity_scale_add_rate = 0.0;
	// 最大移动速度扰动
	double velocity_random_rate = 0.0;
	// 初始移动速度
	Vector2 initial_velocity;
	// 加速度——沿着速度方向的
	double acceleration_rate;
	Callable moving_changed_callback;
	Callable moved_callback;

	_ALWAYS_INLINE_ void reset() {
		follow_target = nullptr;
		moving = false;
		freezed = false;
		move_disabled = false;
		velocity_scale_add_rate = 0.0;
		velocity_random_rate = 0.0;
		moving_changed_callback = Callable();
		moved_callback = Callable();
	}
	_ALWAYS_INLINE_ bool check_can_move() {
		return moving and not freezed and not move_disabled;
	}
	_ALWAYS_INLINE_ void set_moving(bool value) {
		if (moving == value) {
			return;
		}
		moving = value;
		if (moving_changed_callback.is_valid()) {
			moving_changed_callback.call_deferred(moving);
		}
	}
	_ALWAYS_INLINE_ void set_cur_velocity(Vector2 value) {
		Vector2 old_velocity = cur_velocity;
		cur_velocity = value;
		if (moved_callback.is_valid()) {
			moved_callback.call_deferred(cur_velocity, old_velocity);
		}
	}
	_ALWAYS_INLINE_ Vector2 resolve_velocity() {
		return cur_velocity * (1.0 + velocity_scale_add_rate);
	}
	_ALWAYS_INLINE_ double resolve_max_velocity_rate() {
		return max_velocity_rate * (1.0 + velocity_scale_add_rate);
	}
};

struct ZyiNormalMoveComponent : public ZyiMoveBasicComponent {
	// 实际移动的物体
	Node2D *move_node;
	// 强行停止跟随的回调
	Callable force_stop_follow_callback;
	// 前一次跟随是否有效，决定是否使用 last_follow_pos
	bool last_follow_valid = false;
	// 前一次跟随的位置，当 always_following 为 true 且 follow_target 为 nullptr 时，使用该位置
	Vector2 last_follow_pos;
	// 跟随时应用角速度的最小距离，否则直接修改运动方向，而非通过角速度慢慢旋转运动方向
	int64_t min_rotation_follow_dist_squared = 900;
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
		last_follow_valid = false;
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
	_ALWAYS_INLINE_ void set_cur_knockback_velocity(Vector2 value) {
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
	_ALWAYS_INLINE_ void set_cur_knockback_velocity(Vector2 value) {
		cur_knockback_velocity = value;
	}
	_ALWAYS_INLINE_ Vector2 resolve_knockback_velocity() {
		return cur_knockback_velocity;
	}
};

#endif /* MOVE_COMPONENT_H */

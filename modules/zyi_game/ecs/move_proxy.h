#ifndef MOVE_PROXY_H
#define MOVE_PROXY_H

#include "../util/util_signal_helper.h"
#include "core/object/callable_method_pointer.h"
#include "core/object/ref_counted.h"
#include "core/variant/variant_utility.h"
#include "move_system.h"
#include "scene/2d/node_2d.h"

class ZyiMoveComponentProxy : public RefCounted {
	GDCLASS(ZyiMoveComponentProxy, RefCounted)

protected:
	static void _bind_methods();

public:
	Ref<ZyiMoveSystem> system;
	bool can_knockback = false;
	bool direction_locked = false;
	bool freezed = false;
	bool knockback_disabled = false;
	ZyiMoveNodeType node_type = MOVE_NODE_TYPE_NORMAL;
	Node2D *node = nullptr;
	int64_t character_move_component_id = -1;
	int64_t normal_move_component_id = -1;
	int64_t knockback_move_component_id = -1;

	static Ref<ZyiMoveComponentProxy> create();

	Vector2 get_cur_velocity();
	double get_max_velocity_rate();
	Vector2 get_extra_force();
	bool get_move_disabled();

	void register_to_system(const Ref<ZyiMoveSystem> &p_system, Node2D *p_node, bool p_can_knockback = false, BitField<ZyiMoveConstant::Flags> p_flags = ZyiMoveConstant::MOVE_FLAG_NORMAL, int64_t p_min_follow_dist_squared = 900);
	void on_node_tree_exiting();
	void stop_follow();
	void unregister();
	bool is_registered();
	void handle_force_stop_follow();
	void handle_moving_changed(bool moving);
	void handle_moved(const Vector2 &velocity, const Vector2 &old_velocity);
	void handle_knockback_moving_changed(bool moving);
	void update_flags(BitField<ZyiMoveConstant::Flags> p_flags);
	void add_flags(BitField<ZyiMoveConstant::Flags> p_flags);
	void remove_flags(BitField<ZyiMoveConstant::Flags> p_flags);
	BitField<ZyiMoveConstant::Flags> get_flags();
	void update_extra_force(const Vector2 &p_force);
	bool check_can_knockback() const;
	void update_knockback_enabled(bool p_value);
	ZyiNormalMoveComponent *get_normal_move_component_ptr();
	ZyiCharacterMoveComponent *get_character_move_component_ptr();
	ZyiKnockbackMoveComponent *get_knockback_move_component_ptr();
	Node2D *get_move_node();
	Vector2 resolve_velocity();
	double resolve_max_velocity_rate();
	void update_move_linear(const Vector2 &p_initial_velocity, double p_acceleration_rate, double p_max_velocity_rate);
	void update_move_linear_velocity(const Vector2 &p_velocity);
	void update_move_linear_max_velocity_rate(double p_max_velocity_rate);
	double get_move_velocity_scale_add_rate();
	void update_move_velocity_scale_add_rate(double p_velocity_scale_add_rate);
	void update_move_rotate(double p_initial_rotation_rate, double p_rotation_acceleration_rate, double p_max_rotation_rate);
	void update_move_follow(Node2D *p_follow_target, const Vector2 &p_follow_offset = Vector2(0, 0));
	void update_move_follow_pos(const Vector2 &p_pos);
	void update_move_freezed(bool p_value);
	void update_move_disabled(bool p_value);
	void update_move_knockback(const Vector2 &p_init_knockback_velocity, double p_knockback_deceleration_rate);
	void update_move_knockback_disabled(bool p_value);
	void lock_move_direction();
	void unlock_move_direction();
	void start_move_basic();
	void start_move_knockback();
	void stop_move_basic();
	void stop_move_knockback();
	void stop_move();
	bool start_move_towards_point(const Vector2 &p_pos);
	void start_move_towards_direction(const Vector2 &p_direction);
	~ZyiMoveComponentProxy();
};

#endif /* MOVE_PROXY_H */

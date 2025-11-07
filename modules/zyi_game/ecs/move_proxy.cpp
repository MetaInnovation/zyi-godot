#include "move_proxy.h"

void ZyiMoveComponentProxy::_bind_methods() {
	ClassDB::bind_static_method("ZyiMoveComponentProxy", D_METHOD("create"), &ZyiMoveComponentProxy::create);
	ClassDB::bind_method(D_METHOD("get_cur_velocity"), &ZyiMoveComponentProxy::get_cur_velocity);
	ClassDB::bind_method(D_METHOD("get_max_velocity_rate"), &ZyiMoveComponentProxy::get_max_velocity_rate);
	ClassDB::bind_method(D_METHOD("get_extra_force"), &ZyiMoveComponentProxy::get_extra_force);
	ClassDB::bind_method(D_METHOD("get_move_disabled"), &ZyiMoveComponentProxy::get_move_disabled);
	ClassDB::bind_method(D_METHOD("register_to_system", "system", "node", "can_knockback", "flags", "min_follow_dist_squared"), &ZyiMoveComponentProxy::register_to_system, DEFVAL(false), DEFVAL(ZyiMoveConstant::MOVE_FLAG_NORMAL), DEFVAL(900));
	ClassDB::bind_method(D_METHOD("unregister"), &ZyiMoveComponentProxy::unregister);
	ClassDB::bind_method(D_METHOD("is_registered"), &ZyiMoveComponentProxy::is_registered);
	ClassDB::bind_method(D_METHOD("update_flags", "flags"), &ZyiMoveComponentProxy::update_flags);
	ClassDB::bind_method(D_METHOD("add_flags", "flags"), &ZyiMoveComponentProxy::add_flags);
	ClassDB::bind_method(D_METHOD("remove_flags", "flags"), &ZyiMoveComponentProxy::remove_flags);
	ClassDB::bind_method(D_METHOD("get_flags"), &ZyiMoveComponentProxy::get_flags);
	ClassDB::bind_method(D_METHOD("clean_boids_force"), &ZyiMoveComponentProxy::clean_boids_force);
	ClassDB::bind_method(D_METHOD("update_extra_force", "force"), &ZyiMoveComponentProxy::update_extra_force);
	ClassDB::bind_method(D_METHOD("check_can_knockback"), &ZyiMoveComponentProxy::check_can_knockback);
	ClassDB::bind_method(D_METHOD("update_knockback_enabled", "value"), &ZyiMoveComponentProxy::update_knockback_enabled);
	ClassDB::bind_method(D_METHOD("get_move_node"), &ZyiMoveComponentProxy::get_move_node);
	ClassDB::bind_method(D_METHOD("resolve_velocity"), &ZyiMoveComponentProxy::resolve_velocity);
	ClassDB::bind_method(D_METHOD("resolve_max_velocity_rate"), &ZyiMoveComponentProxy::resolve_max_velocity_rate);
	ClassDB::bind_method(D_METHOD("update_move_linear", "initial_velocity", "acceleration_rate", "max_velocity_rate"), &ZyiMoveComponentProxy::update_move_linear);
	ClassDB::bind_method(D_METHOD("update_move_linear_velocity", "velocity"), &ZyiMoveComponentProxy::update_move_linear_velocity);
	ClassDB::bind_method(D_METHOD("update_move_linear_max_velocity_rate", "max_velocity_rate"), &ZyiMoveComponentProxy::update_move_linear_max_velocity_rate);
	ClassDB::bind_method(D_METHOD("get_move_velocity_scale_add_rate"), &ZyiMoveComponentProxy::get_move_velocity_scale_add_rate);
	ClassDB::bind_method(D_METHOD("update_move_velocity_scale_add_rate", "velocity_scale_add_rate"), &ZyiMoveComponentProxy::update_move_velocity_scale_add_rate);
	ClassDB::bind_method(D_METHOD("update_move_rotate", "initial_rotation_rate", "rotation_acceleration_rate", "max_rotation_rate"), &ZyiMoveComponentProxy::update_move_rotate);
	ClassDB::bind_method(D_METHOD("update_move_follow", "follow_target", "follow_offset"), &ZyiMoveComponentProxy::update_move_follow, DEFVAL(Vector2(0, 0)));
	ClassDB::bind_method(D_METHOD("update_move_follow_pos", "pos"), &ZyiMoveComponentProxy::update_move_follow_pos);
	ClassDB::bind_method(D_METHOD("update_move_freezed", "value"), &ZyiMoveComponentProxy::update_move_freezed);
	ClassDB::bind_method(D_METHOD("update_move_disabled", "value"), &ZyiMoveComponentProxy::update_move_disabled);
	ClassDB::bind_method(D_METHOD("update_move_knockback", "init_knockback_velocity", "knockback_deceleration_rate"), &ZyiMoveComponentProxy::update_move_knockback);
	ClassDB::bind_method(D_METHOD("update_move_knockback_disabled", "value"), &ZyiMoveComponentProxy::update_move_knockback_disabled);
	ClassDB::bind_method(D_METHOD("lock_move_direction"), &ZyiMoveComponentProxy::lock_move_direction);
	ClassDB::bind_method(D_METHOD("unlock_move_direction"), &ZyiMoveComponentProxy::unlock_move_direction);
	ClassDB::bind_method(D_METHOD("start_move_basic"), &ZyiMoveComponentProxy::start_move_basic);
	ClassDB::bind_method(D_METHOD("start_move_knockback"), &ZyiMoveComponentProxy::start_move_knockback);
	ClassDB::bind_method(D_METHOD("stop_move_basic"), &ZyiMoveComponentProxy::stop_move_basic);
	ClassDB::bind_method(D_METHOD("stop_move_knockback"), &ZyiMoveComponentProxy::stop_move_knockback);
	ClassDB::bind_method(D_METHOD("stop_move"), &ZyiMoveComponentProxy::stop_move);
	ClassDB::bind_method(D_METHOD("start_move_towards_point", "pos"), &ZyiMoveComponentProxy::start_move_towards_point);
	ClassDB::bind_method(D_METHOD("start_move_towards_follow_point", "pos"), &ZyiMoveComponentProxy::start_move_towards_follow_point);
	ClassDB::bind_method(D_METHOD("start_move_towards_direction", "direction", "follow", "follow_pos"), &ZyiMoveComponentProxy::start_move_towards_direction, DEFVAL(false), DEFVAL(Vector2()));

	ADD_SIGNAL(MethodInfo(SNAME("moving_changed"), PropertyInfo(Variant::BOOL, "moving")));
	ADD_SIGNAL(MethodInfo(SNAME("follow_force_stop")));
	ADD_SIGNAL(MethodInfo(SNAME("moved"), PropertyInfo(Variant::VECTOR2, "velocity"), PropertyInfo(Variant::VECTOR2, "old_velocity")));
	ADD_SIGNAL(MethodInfo(SNAME("move_h_changed"), PropertyInfo(Variant::VECTOR2, "velocity"), PropertyInfo(Variant::VECTOR2, "old_velocity")));
	ADD_SIGNAL(MethodInfo(SNAME("move_v_changed"), PropertyInfo(Variant::VECTOR2, "velocity"), PropertyInfo(Variant::VECTOR2, "old_velocity")));
	ADD_SIGNAL(MethodInfo(SNAME("knockback_moving_changed"), PropertyInfo(Variant::BOOL, "moving")));

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "cur_velocity"), "", "get_cur_velocity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "velocity_scale_add_rate"), "", "get_move_velocity_scale_add_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_velocity_rate"), "", "get_max_velocity_rate");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "extra_force"), "", "get_extra_force");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "flags"), "", "get_flags");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "move_disabled"), "", "get_move_disabled");
}

Ref<ZyiMoveComponentProxy> ZyiMoveComponentProxy::create() {
	Ref<ZyiMoveComponentProxy> result = memnew(ZyiMoveComponentProxy());
	return result;
}

Vector2 ZyiMoveComponentProxy::get_cur_velocity() {
	Vector2 result;
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				result = ptr->cur_velocity;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				result = ptr->cur_velocity;
			}
		} break;
		default:
			break;
	}
	return result;
}

double ZyiMoveComponentProxy::get_max_velocity_rate() {
	double result;
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				result = ptr->max_velocity_rate;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				result = ptr->max_velocity_rate;
			}
		} break;
		default:
			break;
	}
	return result;
}

Vector2 ZyiMoveComponentProxy::get_extra_force() {
	Vector2 result;
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				result = ptr->extra_force;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				result = ptr->extra_force;
			}
		} break;
		default:
			break;
	}
	return result;
}

bool ZyiMoveComponentProxy::get_move_disabled() {
	bool result = false;
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				result = ptr->move_disabled;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				result = ptr->move_disabled;
			}
		} break;
		default:
			break;
	}
	return result;
}

void ZyiMoveComponentProxy::register_to_system(const Ref<ZyiMoveSystem> &p_system, Node2D *p_node, bool p_can_knockback, BitField<ZyiMoveConstant::Flags> p_flags, int64_t p_min_follow_dist_squared) {
	system = p_system;
	can_knockback = p_can_knockback;
	node = p_node;
	if (node->is_class("CharacterBody2D")) {
		node_type = MOVE_NODE_TYPE_CHARACTER_BODY_2D;
		character_move_component_id = system->acquire_character_move_component();
		ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
		ptr->flags = p_flags;
		ptr->min_follow_dist_squared = p_min_follow_dist_squared;
		ptr->moved_callback = callable_mp(this, &ZyiMoveComponentProxy::handle_moved);
		ptr->moving_changed_callback = callable_mp(this, &ZyiMoveComponentProxy::handle_moving_changed);
		ptr->knockback_moving_changed_callback = callable_mp(this, &ZyiMoveComponentProxy::handle_knockback_moving_changed);
	} else {
		node_type = MOVE_NODE_TYPE_NORMAL;
		normal_move_component_id = system->acquire_normal_move_component();
		{
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			ptr->flags = p_flags;
			ptr->min_follow_dist_squared = p_min_follow_dist_squared;
			ptr->moved_callback = callable_mp(this, &ZyiMoveComponentProxy::handle_moved);
			ptr->moving_changed_callback = callable_mp(this, &ZyiMoveComponentProxy::handle_moving_changed);
		}
		if (check_can_knockback()) {
			knockback_move_component_id = system->acquire_knockback_move_component();
			{
				ZyiKnockbackMoveComponent *ptr = get_knockback_move_component_ptr();
				ptr->knockback_moving_changed_callback = callable_mp(this, &ZyiMoveComponentProxy::handle_knockback_moving_changed);
			}
		} else {
			knockback_move_component_id = -1;
		}
	}
	node->connect(SceneStringName(tree_exiting), callable_mp(this, &ZyiMoveComponentProxy::on_node_tree_exiting), CONNECT_ONE_SHOT);
}

void ZyiMoveComponentProxy::on_node_tree_exiting() {
	unregister();
}

void ZyiMoveComponentProxy::stop_follow() {
	update_move_follow(nullptr);
}

void ZyiMoveComponentProxy::unregister() {
	if (node != nullptr && system != nullptr && system->boids_grid.is_valid()) {
		bool is_in_boids_grid = false;
		switch (node_type) {
			case MOVE_NODE_TYPE_NORMAL: {
				ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
				if (ptr && ptr->is_in_boid_grid()) {
					is_in_boids_grid = true;
				}
			} break;
			case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
				ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
				if (ptr && ptr->is_in_boid_grid()) {
					is_in_boids_grid = true;
				}
			} break;
			default:
				break;
		}
		if (is_in_boids_grid) {
			system->boids_grid->update_object_leave(node->get_instance_id());
		}
	}
	if (normal_move_component_id >= 0) {
		system->release_normal_move_component(normal_move_component_id);
		normal_move_component_id = -1;
	}
	if (character_move_component_id >= 0) {
		system->release_character_move_component(character_move_component_id);
		character_move_component_id = -1;
	}
	if (check_can_knockback() && knockback_move_component_id >= 0) {
		system->release_knockback_move_component(knockback_move_component_id);
		knockback_move_component_id = -1;
	}
	system = nullptr;
	can_knockback = false;
	direction_locked = false;
	freezed = false;
	knockback_disabled = false;
	node_type = MOVE_NODE_TYPE_NORMAL;
	node = nullptr;
}

bool ZyiMoveComponentProxy::is_registered() {
	return system != nullptr;
}

void ZyiMoveComponentProxy::handle_force_stop_follow() {
	update_move_follow(nullptr);
	emit_signal(SNAME("follow_force_stop"));
}

void ZyiMoveComponentProxy::handle_moving_changed(bool moving) {
	emit_signal(SNAME("moving_changed"), moving);
}

void ZyiMoveComponentProxy::handle_moved(const Vector2 &velocity, const Vector2 &old_velocity) {
	emit_signal(SNAME("moved"), velocity, old_velocity);
	if (VariantUtilityFunctions::signi(velocity.x) != VariantUtilityFunctions::signi(old_velocity.x)) {
		emit_signal(SNAME("move_h_changed"), velocity, old_velocity);
	}
	if (VariantUtilityFunctions::signi(velocity.y) != VariantUtilityFunctions::signi(old_velocity.y)) {
		emit_signal(SNAME("move_v_changed"), velocity, old_velocity);
	}
}

void ZyiMoveComponentProxy::handle_knockback_moving_changed(bool moving) {
	emit_signal(SNAME("knockback_moving_changed"), moving);
}

void ZyiMoveComponentProxy::update_flags(BitField<ZyiMoveConstant::Flags> p_flags) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->flags = p_flags;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->flags = p_flags;
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::add_flags(BitField<ZyiMoveConstant::Flags> p_flags) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->flags |= p_flags;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->flags |= p_flags;
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::remove_flags(BitField<ZyiMoveConstant::Flags> p_flags) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->flags &= ~p_flags;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->flags &= ~p_flags;
			}
		} break;
		default:
			break;
	}
}

BitField<ZyiMoveConstant::Flags> ZyiMoveComponentProxy::get_flags() {
	BitField<ZyiMoveConstant::Flags> result;
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				result = ptr->flags;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				result = ptr->flags;
			}
		} break;
		default:
			break;
	}
	return result;
}

void ZyiMoveComponentProxy::clean_boids_force() {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->set_boids_repulsive_force(Vector2());
				ptr->set_boids_surround_force(Vector2());
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->set_boids_repulsive_force(Vector2());
				ptr->set_boids_surround_force(Vector2());
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::update_extra_force(const Vector2 &p_force) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->set_extra_force(p_force);
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->set_extra_force(p_force);
			}
		} break;
		default:
			break;
	}
}

bool ZyiMoveComponentProxy::check_can_knockback() const {
	return can_knockback;
}

void ZyiMoveComponentProxy::update_knockback_enabled(bool p_value) {
	can_knockback = p_value;
}

ZyiNormalMoveComponent *ZyiMoveComponentProxy::get_normal_move_component_ptr() {
	if (system.is_null()) {
		return nullptr;
	}
	return system->get_normal_move_component_ptr(normal_move_component_id);
}

ZyiCharacterMoveComponent *ZyiMoveComponentProxy::get_character_move_component_ptr() {
	if (system.is_null()) {
		return nullptr;
	}
	return system->get_character_move_component_ptr(character_move_component_id);
}

ZyiKnockbackMoveComponent *ZyiMoveComponentProxy::get_knockback_move_component_ptr() {
	if (system.is_null()) {
		return nullptr;
	}
	return system->get_knockback_move_component_ptr(normal_move_component_id);
}

Node2D *ZyiMoveComponentProxy::get_move_node() {
	return node;
}

Vector2 ZyiMoveComponentProxy::resolve_velocity() {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr && ptr->moving) {
				return ptr->resolve_velocity();
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr && ptr->moving) {
				return ptr->resolve_velocity();
			}
		} break;
		default:
			break;
	}
	return Vector2();
}

double ZyiMoveComponentProxy::resolve_max_velocity_rate() {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr && ptr->moving) {
				return ptr->resolve_max_velocity_rate();
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr && ptr->moving) {
				return ptr->resolve_max_velocity_rate();
			}
		} break;
		default:
			break;
	}
	return 0.0;
}

void ZyiMoveComponentProxy::update_move_linear(const Vector2 &p_initial_velocity, double p_acceleration_rate, double p_max_velocity_rate) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				if (!direction_locked) {
					ptr->initial_velocity = p_initial_velocity;
				}
				ptr->acceleration_rate = p_acceleration_rate;
				ptr->max_velocity_rate = p_max_velocity_rate;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				if (!direction_locked) {
					ptr->initial_velocity = p_initial_velocity;
				}
				ptr->acceleration_rate = p_acceleration_rate;
				ptr->max_velocity_rate = p_max_velocity_rate;
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::update_move_linear_velocity(const Vector2 &p_velocity) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->set_cur_velocity(p_velocity);
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->set_cur_velocity(p_velocity);
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::update_move_linear_max_velocity_rate(double p_max_velocity_rate) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->max_velocity_rate = p_max_velocity_rate;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->max_velocity_rate = p_max_velocity_rate;
			}
		} break;
		default:
			break;
	}
}

double ZyiMoveComponentProxy::get_move_velocity_scale_add_rate() {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				return ptr->velocity_scale_add_rate;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				return ptr->velocity_scale_add_rate;
			}
		} break;
		default:
			break;
	}
	return 0.0;
}

void ZyiMoveComponentProxy::update_move_velocity_scale_add_rate(double p_velocity_scale_add_rate) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->velocity_scale_add_rate = p_velocity_scale_add_rate;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->velocity_scale_add_rate = p_velocity_scale_add_rate;
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::update_move_rotate(double p_initial_rotation_rate, double p_rotation_acceleration_rate, double p_max_rotation_rate) {
	if (node_type != MOVE_NODE_TYPE_NORMAL) {
		return;
	}
	ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
	if (!ptr) {
		return;
	}
	ptr->initial_rotation_rate = p_initial_rotation_rate;
	ptr->rotation_acceleration_rate = p_rotation_acceleration_rate;
	ptr->max_rotation_rate = p_max_rotation_rate;
}

void ZyiMoveComponentProxy::update_move_follow(Node2D *p_follow_target, const Vector2 &p_follow_offset) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				if (p_follow_target && !p_follow_target->is_queued_for_deletion()) {
					ptr->follow_target = p_follow_target;
					ptr->follow_offset = p_follow_offset;
					ptr->force_stop_follow_callback = callable_mp(this, &ZyiMoveComponentProxy::handle_force_stop_follow);
					if (p_follow_target->is_visible_in_tree()) {
						ptr->last_follow_valid = true;
						ptr->last_follow_pos = p_follow_target->get_global_position();
					} else {
						ptr->last_follow_valid = false;
					}
					ZyiUtilSignalHelper::object_safe_connect(p_follow_target, SceneStringName(tree_exiting), callable_mp(this, &ZyiMoveComponentProxy::stop_follow), CONNECT_ONE_SHOT);
				} else {
					if (ptr->follow_target) {
						ZyiUtilSignalHelper::object_safe_disconnect(static_cast<Object *>(ptr->follow_target), SceneStringName(tree_exiting), callable_mp(this, &ZyiMoveComponentProxy::stop_follow));
					}
					ptr->force_stop_follow_callback = Callable();
					ptr->follow_target = nullptr;
					ptr->follow_offset = p_follow_offset;
					ptr->last_follow_valid = false;
				}
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				if (p_follow_target && !p_follow_target->is_queued_for_deletion()) {
					ptr->follow_target = p_follow_target;
					ptr->follow_offset = p_follow_offset;
					if (p_follow_target->is_visible_in_tree()) {
						ptr->last_follow_valid = true;
						ptr->last_follow_pos = p_follow_target->get_global_position();
					} else {
						ptr->last_follow_valid = false;
					}
					ZyiUtilSignalHelper::object_safe_connect(p_follow_target, SceneStringName(tree_exiting), callable_mp(this, &ZyiMoveComponentProxy::stop_follow), CONNECT_ONE_SHOT);
				} else {
					if (ptr->follow_target) {
						ZyiUtilSignalHelper::object_safe_disconnect(static_cast<Object *>(ptr->follow_target), SceneStringName(tree_exiting), callable_mp(this, &ZyiMoveComponentProxy::stop_follow));
					}
					ptr->follow_target = nullptr;
					ptr->follow_offset = p_follow_offset;
					ptr->last_follow_valid = false;
				}
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::update_move_follow_pos(const Vector2 &p_pos) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->last_follow_valid = true;
				ptr->last_follow_pos = p_pos;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->last_follow_valid = true;
				ptr->last_follow_pos = p_pos;
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::update_move_freezed(bool p_value) {
	freezed = p_value;
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->set_freezed(p_value);
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->set_freezed(p_value);
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::update_move_disabled(bool p_value) {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->move_disabled = p_value;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->move_disabled = p_value;
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::update_move_knockback(const Vector2 &p_init_knockback_velocity, double p_knockback_deceleration_rate) {
	if (knockback_disabled) {
		return;
	}
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiKnockbackMoveComponent *ptr = get_knockback_move_component_ptr();
			if (ptr) {
				ptr->init_knockback_velocity = p_init_knockback_velocity;
				ptr->knockback_deceleration_rate = p_knockback_deceleration_rate;
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->init_knockback_velocity = p_init_knockback_velocity;
				ptr->knockback_deceleration_rate = p_knockback_deceleration_rate;
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::update_move_knockback_disabled(bool p_value) {
	knockback_disabled = p_value;
}

void ZyiMoveComponentProxy::lock_move_direction() {
	direction_locked = true;
}

void ZyiMoveComponentProxy::unlock_move_direction() {
	direction_locked = false;
}

void ZyiMoveComponentProxy::start_move_basic() {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr && !ptr->moving && !ptr->initial_velocity.is_zero_approx()) {
				ptr->set_cur_velocity(ptr->initial_velocity);
				ptr->cur_rotation_rate = ptr->initial_rotation_rate;
				ptr->move_node = node;
				ptr->set_moving(true);
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr && !ptr->moving && !ptr->initial_velocity.is_zero_approx()) {
				ptr->set_cur_velocity(ptr->initial_velocity);
				ptr->move_node = static_cast<CharacterBody2D *>(node);
				ptr->set_moving(true);
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::start_move_knockback() {
	if (knockback_disabled || freezed) {
		return;
	}
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiKnockbackMoveComponent *ptr = get_knockback_move_component_ptr();
			if (ptr && !ptr->moving && !ptr->init_knockback_velocity.is_zero_approx()) {
				ptr->set_cur_knockback_velocity(ptr->init_knockback_velocity);
				ptr->move_node = node;
				ptr->set_knockback_moving(true);
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr && !ptr->knockback_moving && !ptr->init_knockback_velocity.is_zero_approx()) {
				ptr->set_cur_knockback_velocity(ptr->init_knockback_velocity);
				ptr->move_node = static_cast<CharacterBody2D *>(node);
				ptr->set_knockback_moving(true);
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::stop_move_basic() {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				ptr->set_moving(false);
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->set_moving(false);
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::stop_move_knockback() {
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiKnockbackMoveComponent *ptr = get_knockback_move_component_ptr();
			if (ptr) {
				ptr->set_knockback_moving(false);
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				ptr->set_knockback_moving(false);
			}
		} break;
		default:
			break;
	}
}

void ZyiMoveComponentProxy::stop_move() {
	stop_move_basic();
	stop_move_knockback();
}

bool ZyiMoveComponentProxy::start_move_towards_point(const Vector2 &p_pos) {
	if (node && !node->is_queued_for_deletion() && node->is_visible_in_tree()) {
		Vector2 pos = node->get_global_position();
		if (pos.is_equal_approx(p_pos)) {
			return false;
		}
		start_move_towards_direction(pos.direction_to(p_pos));
		return true;
	} else {
		return false;
	}
}
bool ZyiMoveComponentProxy::start_move_towards_follow_point(const Vector2 &p_pos) {
	if (node && !node->is_queued_for_deletion() && node->is_visible_in_tree()) {
		Vector2 pos = node->get_global_position();
		if (pos.is_equal_approx(p_pos)) {
			return false;
		}
		start_move_towards_direction(pos.direction_to(p_pos), true, p_pos);
		return true;
	} else {
		return false;
	}
}

void ZyiMoveComponentProxy::start_move_towards_direction(const Vector2 &p_direction, bool p_follow, const Vector2 &follow_pos) {
	if (p_direction.is_zero_approx()) {
		stop_move();
		return;
	}
	switch (node_type) {
		case MOVE_NODE_TYPE_NORMAL: {
			ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr();
			if (ptr) {
				if (p_follow) {
					ptr->last_follow_valid = true;
					ptr->last_follow_pos = follow_pos;
				}
				if (ptr->moving) {
					if (!direction_locked) {
						ptr->set_cur_velocity(p_direction * ptr->cur_velocity.length());
					}
				} else {
					if (!direction_locked) {
						ptr->initial_velocity = p_direction;
					}
					start_move_basic();
				}
			}
		} break;
		case MOVE_NODE_TYPE_CHARACTER_BODY_2D: {
			ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr();
			if (ptr) {
				if (p_follow) {
					ptr->last_follow_valid = true;
					ptr->last_follow_pos = follow_pos;
				}
				if (ptr->moving) {
					if (!direction_locked) {
						ptr->set_cur_velocity(p_direction * ptr->cur_velocity.length());
					}
				} else {
					if (!direction_locked) {
						ptr->initial_velocity = p_direction;
					}
					start_move_basic();
				}
			}
		} break;
		default:
			break;
	}
}

ZyiMoveComponentProxy::~ZyiMoveComponentProxy() {
	unregister();
}

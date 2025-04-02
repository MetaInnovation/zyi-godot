#include "damage_manager.h"
#include "core/math/math_funcs.h"
#include "core/object/callable_method_pointer.h"
#include "core/variant/variant_utility.h"
#include <algorithm>

void ZyiDamageManager::_bind_methods() {
	ClassDB::bind_static_method("ZyiDamageManager", D_METHOD("calc_damage", "attack", "attack_damage", "defense", "attack_damage_manager", "defense_damage_manager"), &ZyiDamageManager::calc_damage);
	ClassDB::bind_static_method("ZyiDamageManager", D_METHOD("calc_damage_value_with_defense", "damage", "defense"), &ZyiDamageManager::calc_damage_value_with_defense);
	ClassDB::bind_static_method("ZyiDamageManager", D_METHOD("compare_middleware_sort", "a", "b"), &ZyiDamageManager::compare_middleware_sort);

	ClassDB::bind_method(D_METHOD("get_attack_middleware_list"), &ZyiDamageManager::get_attack_middleware_list);
	ClassDB::bind_method(D_METHOD("set_attack_middleware_list", "middleware_list"), &ZyiDamageManager::set_attack_middleware_list);
	ClassDB::bind_method(D_METHOD("get_defense_middleware_list"), &ZyiDamageManager::get_defense_middleware_list);
	ClassDB::bind_method(D_METHOD("set_defense_middleware_list", "middleware_list"), &ZyiDamageManager::set_defense_middleware_list);
	ClassDB::bind_method(D_METHOD("get_context"), &ZyiDamageManager::get_context);
	ClassDB::bind_method(D_METHOD("set_context", "context"), &ZyiDamageManager::set_context);

	ClassDB::bind_method(D_METHOD("get_sorted_attack_middleware_list", "ignore_cache"), &ZyiDamageManager::get_sorted_attack_middleware_list);
	ClassDB::bind_method(D_METHOD("get_sorted_defense_middleware_list", "ignore_cache"), &ZyiDamageManager::get_sorted_defense_middleware_list);
	ClassDB::bind_method(D_METHOD("create_attack", "attack"), &ZyiDamageManager::create_attack);
	ClassDB::bind_method(D_METHOD("create_defense", "defense", "damage", "attack_damage_manager"), &ZyiDamageManager::create_defense);
	ClassDB::bind_method(D_METHOD("add_attack_middleware", "value"), &ZyiDamageManager::add_attack_middleware);
	ClassDB::bind_method(D_METHOD("remove_attack_middleware", "value"), &ZyiDamageManager::remove_attack_middleware);
	ClassDB::bind_method(D_METHOD("add_attack_middleware_list", "value"), &ZyiDamageManager::add_attack_middleware_list);
	ClassDB::bind_method(D_METHOD("remove_attack_middleware_list", "value"), &ZyiDamageManager::remove_attack_middleware_list);
	ClassDB::bind_method(D_METHOD("add_defense_middleware", "value"), &ZyiDamageManager::add_defense_middleware);
	ClassDB::bind_method(D_METHOD("remove_defense_middleware", "value"), &ZyiDamageManager::remove_defense_middleware);
	ClassDB::bind_method(D_METHOD("add_defense_middleware_list", "value"), &ZyiDamageManager::add_defense_middleware_list);
	ClassDB::bind_method(D_METHOD("remove_defense_middleware_list", "value"), &ZyiDamageManager::remove_defense_middleware_list);
	ClassDB::bind_method(D_METHOD("handle_hit_calculated_damage", "damage", "attack", "defense", "defense_damage_manager"), &ZyiDamageManager::handle_hit_calculated_damage);
	ClassDB::bind_method(D_METHOD("handle_hurt_calculated_damage", "damage", "attack", "defense", "attack_damage_manager"), &ZyiDamageManager::handle_hurt_calculated_damage);
	ClassDB::bind_method(D_METHOD("emit_damage_hit", "payload"), &ZyiDamageManager::emit_damage_hit);
	ClassDB::bind_method(D_METHOD("emit_damage_hit_die", "payload"), &ZyiDamageManager::emit_damage_hit_die);
	ClassDB::bind_method(D_METHOD("emit_damage_hurt", "payload"), &ZyiDamageManager::emit_damage_hurt);
	ClassDB::bind_method(D_METHOD("pool_recycle"), &ZyiDamageManager::pool_recycle);

	ADD_SIGNAL(MethodInfo(SNAME("damage_hit"), PropertyInfo(Variant::OBJECT, "payload")));
	ADD_SIGNAL(MethodInfo(SNAME("damage_hit_die"), PropertyInfo(Variant::OBJECT, "payload")));
	ADD_SIGNAL(MethodInfo(SNAME("damage_hurt"), PropertyInfo(Variant::OBJECT, "payload")));

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "attack_middleware_list", PROPERTY_HINT_RESOURCE_TYPE, "ZyiDamageAttackMiddlewareResource"), "set_attack_middleware_list", "get_attack_middleware_list");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "defense_middleware_list", PROPERTY_HINT_RESOURCE_TYPE, "ZyiDamageDefenseMiddlewareResource"), "set_defense_middleware_list", "get_defense_middleware_list");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_RESOURCE_TYPE, "ZyiDamageManagerContext", PROPERTY_USAGE_SCRIPT_VARIABLE), "set_context", "get_context");
}

TypedArray<ZyiDamageAttackMiddlewareResource> ZyiDamageManager::get_attack_middleware_list() {
	return attack_middleware_list;
}

void ZyiDamageManager::set_attack_middleware_list(const TypedArray<ZyiDamageAttackMiddlewareResource> &p_middleware_list) {
	attack_middleware_list = p_middleware_list;
}

TypedArray<ZyiDamageDefenseMiddlewareResource> ZyiDamageManager::get_defense_middleware_list() {
	return defense_middleware_list;
}

void ZyiDamageManager::set_defense_middleware_list(const TypedArray<ZyiDamageDefenseMiddlewareResource> &p_middleware_list) {
	defense_middleware_list = p_middleware_list;
}

Ref<ZyiDamageManagerContext> ZyiDamageManager::get_context() {
	return context;
}

void ZyiDamageManager::set_context(const Ref<ZyiDamageManagerContext> &p_context) {
	context = p_context;
}

/**
 * 计算 attack 攻击 defense 的伤害
 * attack_damage_manager 作为攻击方的manager，处理造成的伤害
 * defense_damage_manager 作为被击方的manager，处理承受的伤害
 */
TypedArray<ZyiDamageValue> ZyiDamageManager::calc_damage(const Ref<ZyiDamageAttackResource> &p_attack, const TypedArray<ZyiDamageValue> &p_damage, const Ref<ZyiDamageDefenseResource> &p_defense, const Ref<ZyiDamageManager> &p_attack_damage_manager, const Ref<ZyiDamageManager> &p_defense_damage_manager) {
	TypedArray<ZyiDamageValue> damage_attack = p_damage.duplicate();
	Ref<ZyiDamageDefenseResource> defense = p_defense->deep_clone();
	if (p_attack_damage_manager.is_valid() && p_defense_damage_manager.is_valid()) {
		defense = p_defense_damage_manager->create_defense(defense, damage_attack, p_attack_damage_manager);
	}
	TypedArray<ZyiDamageValue> result;
	for (const Ref<ZyiDamageValue> &value : damage_attack) {
		Ref<ZyiDamageValue> result_value = p_defense_damage_manager->calc_damage_value_with_defense(value, defense);
		result.append(result_value);
	}
	if (p_attack_damage_manager.is_valid() && p_defense_damage_manager.is_valid()) {
		p_attack_damage_manager->handle_hit_calculated_damage(result, p_attack, defense, p_defense_damage_manager);
		p_defense_damage_manager->handle_hurt_calculated_damage(result, p_attack, defense, p_attack_damage_manager);
	}
	return result;
}

Ref<ZyiDamageValue> ZyiDamageManager::calc_damage_value_with_defense(const Ref<ZyiDamageValue> &p_damage, const Ref<ZyiDamageDefenseResource> &p_defense) {
	const int64_t damage = p_damage->get_value();
	const int64_t real_damage = p_damage->get_real_value();
	// 根据防御计算伤害免疫率（使用1.0时为了转为浮点运算）
	const int64_t defense_damage_sum = p_defense->get_value() + damage;
	double defense_immunity_radio = 0.0;
	if (defense_damage_sum != 0) {
		defense_immunity_radio = (double)p_defense->get_value() / (double)defense_damage_sum;
	}
	// 真实伤害剩余比例
	const double real_damage_radio = 1.0;
	// 普通伤害剩余比例
	const double damage_radio = std::max(1.0 - defense_immunity_radio, 0.0);
	Ref<ZyiDamageValue> result = p_damage->deep_clone();
	result->set_value(VariantUtilityFunctions::ceili(damage * damage_radio));
	result->set_real_value(VariantUtilityFunctions::ceili(real_damage * real_damage_radio));
	return result;
}

bool ZyiDamageManager::compare_middleware_sort(const Ref<ZyiDamageMiddlewareResource> &p_a, const Ref<ZyiDamageMiddlewareResource> &p_b) {
	const int64_t a_order = p_a->get_order_value();
	const int64_t b_order = p_b->get_order_value();
	if (a_order == b_order) {
		return p_a->get_unique_key() < p_b->get_unique_key();
	}
	return a_order < b_order;
}

TypedArray<ZyiDamageAttackMiddlewareResource> ZyiDamageManager::get_sorted_attack_middleware_list(bool p_ignore_cache) {
	if (!is_attack_cache_dirty && !p_ignore_cache) {
		return cached_sorted_attack_middleware_list;
	}
	TypedArray<ZyiDamageAttackMiddlewareResource> data = attack_middleware_list.duplicate();
	data.sort_custom(callable_mp_static(&ZyiDamageManager::compare_middleware_sort));
	cached_sorted_attack_middleware_list = data;
	is_attack_cache_dirty = false;
	return data;
}

TypedArray<ZyiDamageDefenseMiddlewareResource> ZyiDamageManager::get_sorted_defense_middleware_list(bool p_ignore_cache) {
	if (!is_defense_cache_dirty && !p_ignore_cache) {
		return cached_sorted_defense_middleware_list;
	}
	TypedArray<ZyiDamageDefenseMiddlewareResource> data = defense_middleware_list.duplicate();
	data.sort_custom(callable_mp_static(&ZyiDamageManager::compare_middleware_sort));
	cached_sorted_defense_middleware_list = data;
	is_defense_cache_dirty = false;
	return data;
}

TypedArray<ZyiDamageValue> ZyiDamageManager::create_attack(const Ref<ZyiDamageAttackResource> &p_attack) {
	TypedArray<ZyiDamageValue> damage = p_attack->flat_to_value();
	const TypedArray<ZyiDamageAttackMiddlewareResource> sorted_attack_middleware_list = get_sorted_attack_middleware_list();
	for (const Ref<ZyiDamageAttackMiddlewareResource> &item : sorted_attack_middleware_list) {
		if (item->check_condition_match(context)) {
			item->handle_attack(damage, context);
		}
	}
	return damage;
}

Ref<ZyiDamageDefenseResource> ZyiDamageManager::create_defense(const Ref<ZyiDamageDefenseResource> &p_defense, const TypedArray<ZyiDamageValue> &p_damage, const Ref<ZyiDamageManager> &p_attack_damage_manager) {
	Ref<ZyiDamageDefenseResource> defense = p_defense->deep_clone();
	const TypedArray<ZyiDamageDefenseMiddlewareResource> sorted_defense_middleware_list = get_sorted_defense_middleware_list();
	const Ref<ZyiDamageManagerContext> attack_context = p_attack_damage_manager->get_context();
	for (const Ref<ZyiDamageDefenseMiddlewareResource> &item : sorted_defense_middleware_list) {
		if (item->check_condition_match(attack_context, context)) {
			item->handle_defense(defense, p_damage, attack_context, context);
		}
	}
	return defense;
}

void ZyiDamageManager::add_attack_middleware(const Ref<ZyiDamageAttackMiddlewareResource> &p_middleware) {
	attack_middleware_list.push_back(p_middleware);
	is_attack_cache_dirty = true;
}

void ZyiDamageManager::remove_attack_middleware(const Ref<ZyiDamageAttackMiddlewareResource> &p_middleware) {
	attack_middleware_list.erase(p_middleware);
	is_attack_cache_dirty = true;
}

void ZyiDamageManager::add_attack_middleware_list(const TypedArray<ZyiDamageAttackMiddlewareResource> &p_middleware_list) {
	attack_middleware_list.append_array(p_middleware_list);
	is_attack_cache_dirty = true;
}

void ZyiDamageManager::remove_attack_middleware_list(const TypedArray<ZyiDamageAttackMiddlewareResource> &p_middleware_list) {
	for (const Ref<ZyiDamageAttackMiddlewareResource> &item : p_middleware_list) {
		attack_middleware_list.erase(item);
	}
	is_attack_cache_dirty = true;
}

void ZyiDamageManager::add_defense_middleware(const Ref<ZyiDamageDefenseMiddlewareResource> &p_middleware) {
	defense_middleware_list.push_back(p_middleware);
	is_defense_cache_dirty = true;
}

void ZyiDamageManager::remove_defense_middleware(const Ref<ZyiDamageDefenseMiddlewareResource> &p_middleware) {
	defense_middleware_list.erase(p_middleware);
	is_defense_cache_dirty = true;
}

void ZyiDamageManager::add_defense_middleware_list(const TypedArray<ZyiDamageDefenseMiddlewareResource> &p_middleware_list) {
	defense_middleware_list.append_array(p_middleware_list);
	is_defense_cache_dirty = true;
}

void ZyiDamageManager::remove_defense_middleware_list(const TypedArray<ZyiDamageDefenseMiddlewareResource> &p_middleware_list) {
	for (const Ref<ZyiDamageDefenseMiddlewareResource> &item : p_middleware_list) {
		defense_middleware_list.erase(item);
	}
	is_defense_cache_dirty = true;
}

void ZyiDamageManager::handle_hit_calculated_damage(const TypedArray<ZyiDamageValue> &p_damage, const Ref<ZyiDamageAttackResource> &p_attack, const Ref<ZyiDamageDefenseResource> &p_defense, const Ref<ZyiDamageManager> &p_defense_damage_manager) {
	const TypedArray<ZyiDamageAttackMiddlewareResource> sorted_attack_middleware_list = get_sorted_attack_middleware_list();
	const Ref<ZyiDamageManagerContext> defense_context = p_defense_damage_manager->get_context();
	for (const Ref<ZyiDamageAttackMiddlewareResource> &item : sorted_attack_middleware_list) {
		if (item->check_condition_match(context, defense_context)) {
			item->handle_calculated_damage(p_damage, p_attack, p_defense, context, defense_context);
		}
	}
}

void ZyiDamageManager::handle_hurt_calculated_damage(const TypedArray<ZyiDamageValue> &p_damage, const Ref<ZyiDamageAttackResource> &p_attack, const Ref<ZyiDamageDefenseResource> &p_defense, const Ref<ZyiDamageManager> &p_attack_damage_manager) {
	const TypedArray<ZyiDamageDefenseMiddlewareResource> sorted_defense_middleware_list = get_sorted_defense_middleware_list();
	const Ref<ZyiDamageManagerContext> attack_context = p_attack_damage_manager->get_context();
	for (const Ref<ZyiDamageDefenseMiddlewareResource> &item : sorted_defense_middleware_list) {
		if (item->check_condition_match(attack_context, context)) {
			item->handle_calculated_damage(p_damage, p_attack, p_defense, attack_context, context);
		}
	}
}

void ZyiDamageManager::emit_damage_hit(const Variant &p_payload) {
	emit_signal(SNAME("damage_hit"), p_payload);
}

void ZyiDamageManager::emit_damage_hit_die(const Variant &p_payload) {
	emit_signal(SNAME("damage_hit_die"), p_payload);
}

void ZyiDamageManager::emit_damage_hurt(const Variant &p_payload) {
	emit_signal(SNAME("damage_hurt"), p_payload);
}

void ZyiDamageManager::pool_recycle() {
	attack_middleware_list.clear();
	defense_middleware_list.clear();
	cached_sorted_attack_middleware_list.clear();
	cached_sorted_defense_middleware_list.clear();
	is_attack_cache_dirty = true;
	is_defense_cache_dirty = true;
	context->clear();
	context->set_owner_shared_state(Variant());
}

ZyiDamageManager::ZyiDamageManager() {
	context = memnew(ZyiDamageManagerContext());
}

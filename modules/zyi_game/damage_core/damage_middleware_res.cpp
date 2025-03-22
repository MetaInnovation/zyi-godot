#include "damage_middleware_res.h"

void ZyiDamageMiddlewareResource::_bind_methods() {
	GDVIRTUAL_BIND(_get_raw_unique_key);
	GDVIRTUAL_BIND(_get_unique_key);
	GDVIRTUAL_BIND(_get_order_value);
	GDVIRTUAL_BIND(_check_condition_match, "attack_context", "defense_context");
	GDVIRTUAL_BIND(_handle_calculated_damage, "attack_damage", "attack", "defense", "attack_context", "defense_context");

	ClassDB::bind_method(D_METHOD("get_raw_unique_key"), &ZyiDamageMiddlewareResource::get_raw_unique_key);
	ClassDB::bind_method(D_METHOD("get_unique_key_value"), &ZyiDamageMiddlewareResource::get_unique_key_value);
	ClassDB::bind_method(D_METHOD("set_unique_key_value", "key"), &ZyiDamageMiddlewareResource::set_unique_key_value);
	ClassDB::bind_method(D_METHOD("get_condition"), &ZyiDamageMiddlewareResource::get_condition);
	ClassDB::bind_method(D_METHOD("set_condition", "condition"), &ZyiDamageMiddlewareResource::set_condition);
	ClassDB::bind_method(D_METHOD("get_unique_key"), &ZyiDamageMiddlewareResource::get_unique_key);
	ClassDB::bind_method(D_METHOD("get_order_value"), &ZyiDamageMiddlewareResource::get_order_value);
	ClassDB::bind_method(D_METHOD("check_condition_match", "attack_context", "defense_context"), &ZyiDamageMiddlewareResource::check_condition_match, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("handle_calculated_damage", "attack_damage", "attack", "defense", "attack_context", "defense_context"), &ZyiDamageMiddlewareResource::handle_calculated_damage);

	BIND_CONSTANT(ORDER_VALUE_PRE);
	BIND_CONSTANT(ORDER_VALUE_DEFAULT);
	BIND_CONSTANT(ORDER_VALUE_POST);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "unique_key"), "set_unique_key_value", "get_unique_key_value");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "condition", PROPERTY_HINT_RESOURCE_TYPE, "ZyiDamageMiddlewareConditionResource"), "set_condition", "get_condition");
}

String ZyiDamageMiddlewareResource::get_unique_key_value() const {
	return unique_key;
}

String ZyiDamageMiddlewareResource::get_raw_unique_key() const {
	String result;
	if (GDVIRTUAL_CALL(_get_raw_unique_key, result)) {
		return result;
	}
	return get_unique_key_value();
}

void ZyiDamageMiddlewareResource::set_unique_key_value(const String &value) {
	unique_key = value;
}

Ref<ZyiDamageMiddlewareConditionResource> ZyiDamageMiddlewareResource::get_condition() const {
	return condition;
}

void ZyiDamageMiddlewareResource::set_condition(const Ref<ZyiDamageMiddlewareConditionResource> &value) {
	condition = value;
}

String ZyiDamageMiddlewareResource::get_unique_key() const {
	String result;
	if (GDVIRTUAL_CALL(_get_unique_key, result)) {
		return result;
	}
	return get_raw_unique_key();
}

int64_t ZyiDamageMiddlewareResource::get_order_value() const {
	int64_t result;
	if (GDVIRTUAL_CALL(_get_order_value, result)) {
		return result;
	}
	return ORDER_VALUE_DEFAULT;
}

bool ZyiDamageMiddlewareResource::check_condition_match(const Ref<ZyiDamageManagerContext> &p_attack_context, const Ref<ZyiDamageManagerContext> &p_defense_context) const {
	bool result;
	if (GDVIRTUAL_CALL(_check_condition_match, p_attack_context, p_defense_context, result)) {
		return result;
	}
	if (condition.is_null()) {
		return true;
	}
	return condition->check_match(p_attack_context, p_defense_context);
}

void ZyiDamageMiddlewareResource::handle_calculated_damage(TypedArray<ZyiDamageValue> p_attack_damage, const Ref<ZyiDamageAttackResource> &p_attack, const Ref<ZyiDamageDefenseResource> &p_defense, const Ref<ZyiDamageManagerContext> &p_attack_context, const Ref<ZyiDamageManagerContext> &p_defense_context) const {
	GDVIRTUAL_CALL(_handle_calculated_damage, p_attack_damage, p_attack, p_defense, p_attack_context, p_defense_context);
}

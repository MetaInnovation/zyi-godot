#include "damage_defense_res.h"
#include "core/os/memory.h"

void ZyiDamageDefenseResource::_bind_methods() {
	ClassDB::bind_static_method("ZyiDamageDefenseResource", D_METHOD("create", "value"), &ZyiDamageDefenseResource::create, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("get_value"), &ZyiDamageDefenseResource::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &ZyiDamageDefenseResource::set_value);

	ClassDB::bind_method(D_METHOD("deep_clone"), &ZyiDamageDefenseResource::deep_clone);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "value"), "set_value", "get_value");
}

int64_t ZyiDamageDefenseResource::get_value() const {
	return value;
}

void ZyiDamageDefenseResource::set_value(int64_t p_value) {
	value = p_value;
}
Ref<ZyiDamageDefenseResource> ZyiDamageDefenseResource::deep_clone() {
	Ref<ZyiDamageDefenseResource> result = memnew(ZyiDamageDefenseResource(value));
	return result;
}

Ref<ZyiDamageDefenseResource> ZyiDamageDefenseResource::create(int64_t p_value) {
	Ref<ZyiDamageDefenseResource> result = memnew(ZyiDamageDefenseResource(p_value));
	return result;
}

ZyiDamageDefenseResource::ZyiDamageDefenseResource(int64_t p_value) :
		value(p_value) {
}

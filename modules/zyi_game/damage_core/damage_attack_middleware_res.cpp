#include "damage_attack_middleware_res.h"

void ZyiDamageAttackMiddlewareResource::_bind_methods() {
	GDVIRTUAL_BIND(_handle_attack, "attack_damage", "attack_context");
	ClassDB::bind_method(D_METHOD("handle_attack", "attack_damage", "attack_context"), &ZyiDamageAttackMiddlewareResource::handle_attack);
}

void ZyiDamageAttackMiddlewareResource::handle_attack(TypedArray<ZyiDamageValue> p_attack_damage, const Ref<ZyiDamageManagerContext> &p_attack_context) {
	GDVIRTUAL_CALL(_handle_attack, p_attack_damage, p_attack_context);
}

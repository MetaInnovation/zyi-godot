#include "damage_defense_middleware_res.h"

void ZyiDamageDefenseMiddlewareResource::_bind_methods() {
	GDVIRTUAL_BIND(_handle_defense, "defense", "attack_damage", "attack_context", "defense_context");
	ClassDB::bind_method(D_METHOD("handle_defense", "defense", "attack_damage", "attack_context", "defense_context"), &ZyiDamageDefenseMiddlewareResource::handle_defense);
}

void ZyiDamageDefenseMiddlewareResource::handle_defense(const Ref<ZyiDamageDefenseResource> &p_defense, TypedArray<ZyiDamageValue> p_attack_damage, const Ref<ZyiDamageManagerContext> &p_attack_context, const Ref<ZyiDamageManagerContext> &p_defense_context) {
	GDVIRTUAL_CALL(_handle_defense, p_defense, p_attack_damage, p_attack_context, p_defense_context);
}

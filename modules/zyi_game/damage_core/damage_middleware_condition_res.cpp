#include "damage_middleware_condition_res.h"

void ZyiDamageMiddlewareConditionResource::_bind_methods() {
	GDVIRTUAL_BIND(_check_match, "attack_context", "defense_context");
	ClassDB::bind_method(D_METHOD("check_match", "attack_context", "defense_context"), &ZyiDamageMiddlewareConditionResource::check_match);
}

bool ZyiDamageMiddlewareConditionResource::check_match(const Ref<ZyiDamageManagerContext> &p_attack_context, const Ref<ZyiDamageManagerContext> &p_defense_context) const {
	bool result;
	if (GDVIRTUAL_CALL(_check_match, p_attack_context, p_defense_context, result)) {
		return result;
	}
	return true;
}

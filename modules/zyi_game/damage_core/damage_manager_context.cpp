#include "./damage_manager_context.h"
#include "damage_manager_context.h"

void ZyiDamageManagerContext::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_owner_shared_state"), &ZyiDamageManagerContext::get_owner_shared_state);
	ClassDB::bind_method(D_METHOD("set_owner_shared_state", "owner_shared_state"), &ZyiDamageManagerContext::set_owner_shared_state);
	ADD_PROPERTY(PropertyInfo(Variant::NIL, "owner_shared_state", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_owner_shared_state", "get_owner_shared_state");
}
const Variant &ZyiDamageManagerContext::get_owner_shared_state() const {
	return owner_shared_state;
}
void ZyiDamageManagerContext::set_owner_shared_state(const Variant &p_owner_shared_state) {
	owner_shared_state = p_owner_shared_state;
}

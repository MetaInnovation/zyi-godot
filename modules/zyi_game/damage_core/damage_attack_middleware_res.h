#ifndef DAMAGE_ATTACK_MIDDLEWARE_RES_H
#define DAMAGE_ATTACK_MIDDLEWARE_RES_H

#include "core/object/gdvirtual.gen.inc"
#include "core/object/object.h"
#include "damage_defense_res.h"
#include "damage_manager_context.h"
#include "damage_middleware_res.h"
#include "damage_value.h"

class ZyiDamageAttackMiddlewareResource : public ZyiDamageMiddlewareResource {
	GDCLASS(ZyiDamageAttackMiddlewareResource, ZyiDamageMiddlewareResource);

protected:
	static void _bind_methods();

public:
	virtual void handle_attack(TypedArray<ZyiDamageValue> p_attack_damage, const Ref<ZyiDamageManagerContext> &p_attack_context);
	GDVIRTUAL2C(_handle_attack, TypedArray<ZyiDamageValue>, Ref<ZyiDamageManagerContext>);
};

#endif /* DAMAGE_ATTACK_MIDDLEWARE_RES_H */

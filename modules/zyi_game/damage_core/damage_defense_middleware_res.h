#ifndef DAMAGE_DEFENSE_MIDDLEWARE_RES_H
#define DAMAGE_DEFENSE_MIDDLEWARE_RES_H

#include "core/object/gdvirtual.gen.inc"
#include "core/object/object.h"
#include "damage_defense_res.h"
#include "damage_manager_context.h"
#include "damage_middleware_res.h"
#include "damage_value.h"

class ZyiDamageDefenseMiddlewareResource : public ZyiDamageMiddlewareResource {
	GDCLASS(ZyiDamageDefenseMiddlewareResource, ZyiDamageMiddlewareResource);

protected:
	static void _bind_methods();

public:
	virtual void handle_defense(const Ref<ZyiDamageDefenseResource> &p_defense, TypedArray<ZyiDamageValue> p_attack_damage, const Ref<ZyiDamageManagerContext> &p_attack_context, const Ref<ZyiDamageManagerContext> &p_defense_context);
	GDVIRTUAL4C(_handle_defense, Ref<ZyiDamageDefenseResource>, TypedArray<ZyiDamageValue>, Ref<ZyiDamageManagerContext>, Ref<ZyiDamageManagerContext>);
};

#endif /* DAMAGE_DEFENSE_MIDDLEWARE_RES_H */

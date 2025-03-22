#ifndef DAMAGE_MIDDLEWARE_CONDITION_H
#define DAMAGE_MIDDLEWARE_CONDITION_H

#include "core/io/resource.h"
#include "core/object/class_db.h"
#include "core/object/gdvirtual.gen.inc"
#include "core/object/ref_counted.h"
#include "damage_manager_context.h"

class ZyiDamageMiddlewareConditionResource : public Resource {
	GDCLASS(ZyiDamageMiddlewareConditionResource, Resource);

protected:
	static void _bind_methods();

public:
	virtual bool check_match(const Ref<ZyiDamageManagerContext> &p_attack_context, const Ref<ZyiDamageManagerContext> &p_defense_context) const;
	GDVIRTUAL2RC(bool, _check_match, Ref<ZyiDamageManagerContext>, Ref<ZyiDamageManagerContext>);
};

#endif // DAMAGE_MIDDLEWARE_CONDITION_H

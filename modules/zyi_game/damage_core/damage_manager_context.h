#ifndef DAMAGE_MANAGER_CONTEXT_H
#define DAMAGE_MANAGER_CONTEXT_H

#include "../util/util_attribute.h"
#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/variant/variant.h"

class ZyiDamageManagerContext : public ZyiUtilAttribute {
	GDCLASS(ZyiDamageManagerContext, ZyiUtilAttribute);

protected:
	static void _bind_methods();

public:
	Variant owner_shared_state;
	const Variant &get_owner_shared_state() const;
	void set_owner_shared_state(const Variant &p_owner_shared_state);
};

#endif /* DAMAGE_MANAGER_CONTEXT_H */

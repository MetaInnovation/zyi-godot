#ifndef DAMAGE_DEFENSE_RES_H
#define DAMAGE_DEFENSE_RES_H

#include "core/io/resource.h"
#include "core/object/class_db.h"
#include "core/object/ref_counted.h"

class ZyiDamageDefenseResource : public Resource {
	GDCLASS(ZyiDamageDefenseResource, Resource);

protected:
	static void _bind_methods();

public:
	// 防御
	int64_t value = 0;

	int64_t get_value() const;
	void set_value(int64_t p_value);

	Ref<ZyiDamageDefenseResource> deep_clone();

	static Ref<ZyiDamageDefenseResource> create(int64_t p_value = 0);

	ZyiDamageDefenseResource(int64_t p_value = 0);
};
#endif /* DAMAGE_DEFENSE_RES_H */

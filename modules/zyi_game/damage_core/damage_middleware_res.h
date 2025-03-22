#ifndef DAMAGE_MIDDLEWARE_RES_H
#define DAMAGE_MIDDLEWARE_RES_H

#include "core/io/resource.h"
#include "core/object/class_db.h"
#include "core/object/gdvirtual.gen.inc"
#include "core/object/ref_counted.h"
#include "damage_attack_res.h"
#include "damage_defense_res.h"
#include "damage_manager_context.h"
#include "damage_middleware_condition_res.h"
#include "damage_value.h"

class ZyiDamageMiddlewareResource : public Resource {
	GDCLASS(ZyiDamageMiddlewareResource, Resource);

protected:
	static void _bind_methods();

public:
	enum {
		ORDER_VALUE_PRE = 100,
		ORDER_VALUE_DEFAULT = 10000,
		ORDER_VALUE_POST = 100000,
	};

	String unique_key = "";
	Ref<ZyiDamageMiddlewareConditionResource> condition;

	virtual String get_raw_unique_key() const;
	GDVIRTUAL0RC(String, _get_raw_unique_key);
	String get_unique_key_value() const;
	void set_unique_key_value(const String &value);
	Ref<ZyiDamageMiddlewareConditionResource> get_condition() const;
	void set_condition(const Ref<ZyiDamageMiddlewareConditionResource> &value);
	virtual String get_unique_key() const;
	GDVIRTUAL0RC(String, _get_unique_key);
	virtual int64_t get_order_value() const;
	GDVIRTUAL0RC(int64_t, _get_order_value);
	virtual bool check_condition_match(const Ref<ZyiDamageManagerContext> &p_attack_context, const Ref<ZyiDamageManagerContext> &p_defense_context = Variant()) const;
	GDVIRTUAL2RC(bool, _check_condition_match, Ref<ZyiDamageManagerContext>, Ref<ZyiDamageManagerContext>);
	virtual void handle_calculated_damage(TypedArray<ZyiDamageValue> p_attack_damage, const Ref<ZyiDamageAttackResource> &p_attack, const Ref<ZyiDamageDefenseResource> &p_defense, const Ref<ZyiDamageManagerContext> &p_attack_context, const Ref<ZyiDamageManagerContext> &p_defense_context) const;
	GDVIRTUAL5C(_handle_calculated_damage, TypedArray<ZyiDamageValue>, Ref<ZyiDamageAttackResource>, Ref<ZyiDamageDefenseResource>, Ref<ZyiDamageManagerContext>, Ref<ZyiDamageManagerContext>);
};

#endif /* DAMAGE_MIDDLEWARE_RES_H */

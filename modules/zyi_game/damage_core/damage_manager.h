#ifndef DAMAGE_MANAGER_H
#define DAMAGE_MANAGER_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "damage_attack_middleware_res.h"
#include "damage_defense_middleware_res.h"
#include "damage_value.h"
#include "scene/2d/node_2d.h"

class ZyiDamageManager : public RefCounted {
	GDCLASS(ZyiDamageManager, RefCounted);

protected:
	static void _bind_methods();

public:
	TypedArray<ZyiDamageAttackMiddlewareResource> attack_middleware_list;
	TypedArray<ZyiDamageDefenseMiddlewareResource> defense_middleware_list;
	TypedArray<ZyiDamageAttackMiddlewareResource> cached_sorted_attack_middleware_list;
	TypedArray<ZyiDamageDefenseMiddlewareResource> cached_sorted_defense_middleware_list;
	bool is_attack_cache_dirty = true;
	bool is_defense_cache_dirty = true;
	Ref<ZyiDamageManagerContext> context;

	TypedArray<ZyiDamageAttackMiddlewareResource> get_attack_middleware_list();
	void set_attack_middleware_list(const TypedArray<ZyiDamageAttackMiddlewareResource> &p_middleware_list);
	TypedArray<ZyiDamageDefenseMiddlewareResource> get_defense_middleware_list();
	void set_defense_middleware_list(const TypedArray<ZyiDamageDefenseMiddlewareResource> &p_middleware_list);
	Ref<ZyiDamageManagerContext> get_context();
	void set_context(const Ref<ZyiDamageManagerContext> &p_context);

	static TypedArray<ZyiDamageValue> calc_damage(const Ref<ZyiDamageAttackResource> &p_attack, const TypedArray<ZyiDamageValue> &p_damage, const Ref<ZyiDamageDefenseResource> &p_defense, const Ref<ZyiDamageManager> &p_attack_damage_manager, const Ref<ZyiDamageManager> &p_defense_damage_manager);
	static Ref<ZyiDamageValue> calc_damage_value_with_defense(const Ref<ZyiDamageValue> &p_damage, const Ref<ZyiDamageDefenseResource> &p_defense);
	static bool compare_middleware_sort(const Ref<ZyiDamageMiddlewareResource> &p_a, const Ref<ZyiDamageMiddlewareResource> &p_b);

	TypedArray<ZyiDamageAttackMiddlewareResource> get_sorted_attack_middleware_list(bool p_ignore_cache = false);
	TypedArray<ZyiDamageDefenseMiddlewareResource> get_sorted_defense_middleware_list(bool p_ignore_cache = false);
	TypedArray<ZyiDamageValue> create_attack(const Ref<ZyiDamageAttackResource> &p_attack);
	Ref<ZyiDamageDefenseResource> create_defense(const Ref<ZyiDamageDefenseResource> &p_defense, const TypedArray<ZyiDamageValue> &p_damage, const Ref<ZyiDamageManager> &p_attack_damage_manager);

	void add_attack_middleware(const Ref<ZyiDamageAttackMiddlewareResource> &p_middleware);
	void remove_attack_middleware(const Ref<ZyiDamageAttackMiddlewareResource> &p_middleware);
	void add_attack_middleware_list(const TypedArray<ZyiDamageAttackMiddlewareResource> &p_middleware_list);
	void remove_attack_middleware_list(const TypedArray<ZyiDamageAttackMiddlewareResource> &p_middleware_list);
	void add_defense_middleware(const Ref<ZyiDamageDefenseMiddlewareResource> &p_middleware);
	void remove_defense_middleware(const Ref<ZyiDamageDefenseMiddlewareResource> &p_middleware);
	void add_defense_middleware_list(const TypedArray<ZyiDamageDefenseMiddlewareResource> &p_middleware_list);
	void remove_defense_middleware_list(const TypedArray<ZyiDamageDefenseMiddlewareResource> &p_middleware_list);
	void handle_hit_calculated_damage(const TypedArray<ZyiDamageValue> &p_damage, const Ref<ZyiDamageAttackResource> &p_attack, const Ref<ZyiDamageDefenseResource> &p_defense, const Ref<ZyiDamageManager> &p_defense_damage_manager);
	void handle_hurt_calculated_damage(const TypedArray<ZyiDamageValue> &p_damage, const Ref<ZyiDamageAttackResource> &p_attack, const Ref<ZyiDamageDefenseResource> &p_defense, const Ref<ZyiDamageManager> &p_attack_damage_manager);

	void emit_damage_hit(const Variant &p_payload);
	void emit_damage_hit_die(const Variant &p_payload);
	void emit_damage_hurt(const Variant &p_payload);

	void pool_recycle();

	ZyiDamageManager();
};

#endif /* DAMAGE_MANAGER_H */

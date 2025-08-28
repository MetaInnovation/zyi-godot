#ifndef DAMAGE_ATTACK_RES_H
#define DAMAGE_ATTACK_RES_H

#include "core/io/resource.h"
#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "damage_constant.h"
#include "damage_extra_meta_res.h"
#include "damage_value.h"

class ZyiDamageAttackResource : public Resource {
	GDCLASS(ZyiDamageAttackResource, Resource);

protected:
	static void _bind_methods();

public:
	// 伤害
	int64_t value = 0;
	// 真实伤害
	int64_t real_value = 0;
	// 伤害类型
	String type = ZyiDamageConstant::get_type_normal();
	// 伤害来源
	String source = "";
	// 额外分类的元数据，不可修改
	Ref<ZyiDamageExtraMetaResource> extra_meta;
	// 是否是附加伤害
	bool is_append = false;
	// 暴击几率
	double crit_rate = 0;
	// 暴击伤害倍率
	double crit_damage_rate = ZyiDamageConstant::get_default_crit_damage_rate();
	// 多重攻击
	TypedArray<ZyiDamageAttackResource>
			children = {};

	int64_t get_value() const;
	void set_value(int64_t p_value);
	int64_t get_real_value() const;
	void set_real_value(int64_t p_real_value);
	String get_type() const;
	void set_type(const String &p_type);
	String get_source() const;
	void set_source(const String &p_source);
	Ref<ZyiDamageExtraMetaResource> get_extra_meta() const;
	void set_extra_meta(const Ref<ZyiDamageExtraMetaResource> &p_extra_meta);
	bool get_is_append() const;
	void set_is_append(bool p_is_append);
	double get_crit_rate() const;
	void set_crit_rate(double p_crit_rate);
	double get_crit_damage_rate() const;
	void set_crit_damage_rate(double p_crit_damage_rate);
	TypedArray<ZyiDamageAttackResource> get_children() const;
	void set_children(const TypedArray<ZyiDamageAttackResource> &p_children);

	Ref<ZyiDamageAttackResource> deep_clone();
	Ref<ZyiDamageAttackResource> multiply(double p_rate, bool p_children_multiply = false);
	Ref<ZyiDamageValue> build_value();
	Dictionary merge_to_value_by_type();
	TypedArray<ZyiDamageValue> flat_and_merge_to_value_by_type();
	TypedArray<ZyiDamageValue> flat_to_value();

	static Ref<ZyiDamageAttackResource> create(int64_t p_value = 0, int64_t p_real_value = 0, const String &p_type = ZyiDamageConstant::get_type_normal(), const String &p_source = "", const Ref<ZyiDamageExtraMetaResource> &p_extra_meta = Variant(), bool p_is_append = false, double p_crit = 0, double p_crit_damage_rate = ZyiDamageConstant::get_default_crit_damage_rate(), const TypedArray<ZyiDamageAttackResource> &p_children = TypedArray<ZyiDamageAttackResource>());

	ZyiDamageAttackResource(int64_t p_value = 0, int64_t p_real_value = 0, const String &p_type = ZyiDamageConstant::get_type_normal(), const String &p_source = "", const Ref<ZyiDamageExtraMetaResource> &p_extra_meta = Variant(), bool p_is_append = false, double p_crit = 0, double p_crit_damage_rate = ZyiDamageConstant::get_default_crit_damage_rate(), const TypedArray<ZyiDamageAttackResource> &p_children = TypedArray<ZyiDamageAttackResource>());
};
#endif /* DAMAGE_ATTACK_RES_H */

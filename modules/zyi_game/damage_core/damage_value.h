#ifndef DAMAGE_VALUE_H
#define DAMAGE_VALUE_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/string/string_name.h"
#include "core/variant/callable.h"
#include "core/variant/dictionary.h"
#include "core/variant/typed_array.h"
#include "core/variant/variant_utility.h"
#include "damage_constant.h"
#include "damage_extra_meta_res.h"

class ZyiDamageValue : public RefCounted {
	GDCLASS(ZyiDamageValue, RefCounted);

protected:
	static void _bind_methods();

public:
	static const String TYPE_LIST[5];

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
	// 是否是暴击伤害
	bool is_crit = false;
	// 是否是附加伤害
	bool is_append = false;
	// 暴击几率
	double crit_rate = 0;
	// 暴击伤害倍率
	double crit_damage_rate = ZyiDamageConstant::get_default_crit_damage_rate();

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
	bool get_is_crit() const;
	void set_is_crit(bool p_is_crit);
	bool get_is_append() const;
	void set_is_append(bool p_is_append);
	double get_crit_rate() const;
	void set_crit_rate(double p_crit_rate);
	double get_crit_damage_rate() const;
	void set_crit_damage_rate(double p_crit_damage_rate);

	Dictionary to_json() const;
	Ref<ZyiDamageValue> deep_clone() const;
	void merge_value_with(const Ref<ZyiDamageValue> &p_damage_value);

	static Dictionary calc_and_resolve_value_by_type(const TypedArray<ZyiDamageValue> &p_data, bool p_ignore_append = false, const Callable &p_func = *reinterpret_cast<const Callable *>(VariantUtilityFunctions::randf_range));
	static Ref<ZyiDamageValue> from_json(const Dictionary &p_data);
	static Ref<ZyiDamageValue> create(int64_t p_value = 0, int64_t p_real_value = 0, const String &p_type = ZyiDamageConstant::get_type_normal(), const String &p_source = "", const Ref<ZyiDamageExtraMetaResource> &p_extra_meta = Variant(), bool p_is_append = false, double p_crit = 0, double p_crit_damage_rate = ZyiDamageConstant::get_default_crit_damage_rate());

	ZyiDamageValue(int64_t p_value = 0, int64_t p_real_value = 0, const String &p_type = ZyiDamageConstant::get_type_normal(), const String &p_source = "", const Ref<ZyiDamageExtraMetaResource> &p_extra_meta = Variant(), bool p_is_append = false, double p_crit = 0, double p_crit_damage_rate = ZyiDamageConstant::get_default_crit_damage_rate());
};

#endif /* DAMAGE_VALUE_H */

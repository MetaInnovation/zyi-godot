#include "damage_value.h"
#include "core/math/math_funcs.h"
#include "core/object/callable_method_pointer.h"
#include "core/os/memory.h"
#include "core/templates/local_vector.h"
#include "core/variant/variant_utility.h"

const String ZyiDamageValue::TYPE_LIST[5] = {
	ZyiDamageConstant::get_type_normal(),
	ZyiDamageConstant::get_type_crit(),
	ZyiDamageConstant::get_type_real(),
	ZyiDamageConstant::get_type_crit_real(),
	ZyiDamageConstant::get_type_total(),
};

void ZyiDamageValue::_bind_methods() {
	ClassDB::bind_static_method("ZyiDamageValue", D_METHOD("calc_and_resolve_value_by_type", "data", "ignore_append", "rand_func"), &ZyiDamageValue::calc_and_resolve_value_by_type, DEFVAL(false), DEFVAL(callable_mp_static(&VariantUtilityFunctions::randf_range)));
	ClassDB::bind_static_method("ZyiDamageValue", D_METHOD("from_json", "data"), &ZyiDamageValue::from_json);
	ClassDB::bind_static_method("ZyiDamageValue", D_METHOD("create", "value", "real_value", "type", "source", "extra_meta", "is_append", "crit_rate", "crit_damage_rate"), &ZyiDamageValue::create, DEFVAL(0), DEFVAL(0), DEFVAL(ZyiDamageConstant::get_type_normal()), DEFVAL(""), DEFVAL(Variant()), DEFVAL(false), DEFVAL(0), DEFVAL(ZyiDamageConstant::get_default_crit_damage_rate()));

	ClassDB::bind_method(D_METHOD("get_value"), &ZyiDamageValue::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &ZyiDamageValue::set_value);
	ClassDB::bind_method(D_METHOD("get_real_value"), &ZyiDamageValue::get_real_value);
	ClassDB::bind_method(D_METHOD("set_real_value", "real_value"), &ZyiDamageValue::set_real_value);
	ClassDB::bind_method(D_METHOD("get_type"), &ZyiDamageValue::get_type);
	ClassDB::bind_method(D_METHOD("set_type", "type"), &ZyiDamageValue::set_type);
	ClassDB::bind_method(D_METHOD("get_source"), &ZyiDamageValue::get_source);
	ClassDB::bind_method(D_METHOD("set_source", "source"), &ZyiDamageValue::set_source);
	ClassDB::bind_method(D_METHOD("get_extra_meta"), &ZyiDamageValue::get_extra_meta);
	ClassDB::bind_method(D_METHOD("set_extra_meta", "extra_meta"), &ZyiDamageValue::set_extra_meta);
	ClassDB::bind_method(D_METHOD("get_is_crit"), &ZyiDamageValue::get_is_crit);
	ClassDB::bind_method(D_METHOD("set_is_crit", "is_crit"), &ZyiDamageValue::set_is_crit);
	ClassDB::bind_method(D_METHOD("get_is_append"), &ZyiDamageValue::get_is_append);
	ClassDB::bind_method(D_METHOD("set_is_append", "is_append"), &ZyiDamageValue::set_is_append);
	ClassDB::bind_method(D_METHOD("get_crit_rate"), &ZyiDamageValue::get_crit_rate);
	ClassDB::bind_method(D_METHOD("set_crit_rate", "crit_rate"), &ZyiDamageValue::set_crit_rate);
	ClassDB::bind_method(D_METHOD("get_crit_damage_rate"), &ZyiDamageValue::get_crit_damage_rate);
	ClassDB::bind_method(D_METHOD("set_crit_damage_rate", "crit_damage_rate"), &ZyiDamageValue::set_crit_damage_rate);

	ClassDB::bind_method(D_METHOD("deep_clone"), &ZyiDamageValue::deep_clone);
	ClassDB::bind_method(D_METHOD("merge_value_with", "damage"), &ZyiDamageValue::merge_value_with);
	ClassDB::bind_method(D_METHOD("to_json"), &ZyiDamageValue::to_json);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "value"), "set_value", "get_value");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "real_value"), "set_real_value", "get_real_value");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "type"), "set_type", "get_type");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "source"), "set_source", "get_source");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "extra_meta", PROPERTY_HINT_RESOURCE_TYPE, "ZyiDamageExtraMetaResource"), "set_extra_meta", "get_extra_meta");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_crit"), "set_is_crit", "get_is_crit");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_append"), "set_is_append", "get_is_append");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "crit_rate"), "set_crit_rate", "get_crit_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "crit_damage_rate"), "set_crit_damage_rate", "get_crit_damage_rate");
}

int64_t ZyiDamageValue::get_value() const {
	return value;
}

void ZyiDamageValue::set_value(int64_t p_value) {
	value = p_value;
}

int64_t ZyiDamageValue::get_real_value() const {
	return real_value;
}

void ZyiDamageValue::set_real_value(int64_t p_real_value) {
	real_value = p_real_value;
}

String ZyiDamageValue::get_type() const {
	return type;
}

void ZyiDamageValue::set_type(const String &p_type) {
	type = p_type;
}

String ZyiDamageValue::get_source() const {
	return source;
}

void ZyiDamageValue::set_source(const String &p_source) {
	source = p_source;
}

Ref<ZyiDamageExtraMetaResource> ZyiDamageValue::get_extra_meta() const {
	return extra_meta;
}

void ZyiDamageValue::set_extra_meta(const Ref<ZyiDamageExtraMetaResource> &p_extra_meta) {
	extra_meta = p_extra_meta;
}

bool ZyiDamageValue::get_is_crit() const {
	return is_crit;
}

void ZyiDamageValue::set_is_crit(bool p_is_crit) {
	is_crit = p_is_crit;
}

bool ZyiDamageValue::get_is_append() const {
	return is_append;
}

void ZyiDamageValue::set_is_append(bool p_is_append) {
	is_append = p_is_append;
}

double ZyiDamageValue::get_crit_rate() const {
	return crit_rate;
}

void ZyiDamageValue::set_crit_rate(double p_crit_rate) {
	crit_rate = p_crit_rate;
}

double ZyiDamageValue::get_crit_damage_rate() const {
	return crit_damage_rate;
}

void ZyiDamageValue::set_crit_damage_rate(double p_crit_damage_rate) {
	crit_damage_rate = p_crit_damage_rate;
}

Dictionary ZyiDamageValue::to_json() const {
	Dictionary result;
	result["value"] = value;
	result["real_value"] = real_value;
	result["type"] = type;
	result["source"] = source;
	result["extra_meta"] = extra_meta;
	result["is_crit"] = is_crit;
	result["is_append"] = is_append;
	result["crit_rate"] = crit_rate;
	result["crit_damage_rate"] = crit_damage_rate;
	return result;
}

Ref<ZyiDamageValue> ZyiDamageValue::deep_clone() const {
	Ref<ZyiDamageValue> result = memnew(ZyiDamageValue(value, real_value, type, source, extra_meta, is_append, crit_rate, crit_damage_rate));
	return result;
}

void ZyiDamageValue::merge_value_with(const Ref<ZyiDamageValue> &p_damage_value) {
	value += p_damage_value->get_value();
	real_value += p_damage_value->get_real_value();
}

Dictionary ZyiDamageValue::calc_and_resolve_value_by_type(const TypedArray<ZyiDamageValue> &p_data, bool p_ignore_append, const Callable &p_func) {
	int64_t total_normal = 0;
	int64_t total_crit = 0;
	int64_t real_val = 0;
	int64_t crit_real_val = 0;
	LocalVector<Dictionary> append_list;
	for (const Ref<ZyiDamageValue> &item : p_data) {
		if (!p_ignore_append && item->get_is_append()) {
			append_list.push_back(calc_and_resolve_value_by_type({ item }, true));
		} else {
			const double crit = item->get_crit_rate();
			const double crit_damage_rate = item->get_crit_damage_rate();
			if (crit < 1.0 && double(p_func.call(0, 1)) > crit) {
				total_normal += item->get_value();
				real_val += item->get_real_value();
			} else {
				item->set_is_crit(true);
				total_crit += VariantUtilityFunctions::ceili(item->get_value() * crit_damage_rate);
				crit_real_val += VariantUtilityFunctions::ceili(item->get_real_value() * crit_damage_rate);
			}
		}
	}
	Dictionary type_to_value;
	int64_t append_total = 0;
	if (!p_ignore_append) {
		Dictionary append_type_to_value;
		for (const Dictionary &item : append_list) {
			for (const String &key : ZyiDamageValue::TYPE_LIST) {
				const double value = double(item.get(key, 0.0)) + double(append_type_to_value.get(key, 0.0));
				append_type_to_value[key] = value;
			}
		}
		append_total = append_type_to_value.get(ZyiDamageConstant::get_type_total(), 0.0);
		type_to_value[ZyiDamageConstant::get_inner_type_append()] = append_type_to_value;
	}
	type_to_value[ZyiDamageConstant::get_type_normal()] = total_normal;
	type_to_value[ZyiDamageConstant::get_type_crit()] = total_crit;
	type_to_value[ZyiDamageConstant::get_type_real()] = real_val;
	type_to_value[ZyiDamageConstant::get_type_crit_real()] = crit_real_val;
	type_to_value[ZyiDamageConstant::get_type_total()] = total_normal + total_crit + real_val + crit_real_val + append_total;
	return type_to_value;
}

Ref<ZyiDamageValue> ZyiDamageValue::from_json(const Dictionary &p_data) {
	Ref<ZyiDamageValue> result = memnew(ZyiDamageValue(p_data["value"], p_data["real_value"], p_data["type"], p_data["source"], p_data["extra_meta"], p_data["is_append"], p_data["crit_rate"], p_data["crit_damage_rate"]));
	return result;
}

Ref<ZyiDamageValue> ZyiDamageValue::create(int64_t p_value, int64_t p_real_value, const String &p_type, const String &p_source, const Ref<ZyiDamageExtraMetaResource> &p_extra_meta, bool p_is_append, double p_crit, double p_crit_damage_rate) {
	Ref<ZyiDamageValue> result = memnew(ZyiDamageValue(p_value, p_real_value, p_type, p_source, p_extra_meta, p_is_append, p_crit, p_crit_damage_rate));
	return result;
}

ZyiDamageValue::ZyiDamageValue(int64_t p_value, int64_t p_real_value, const String &p_type, const String &p_source, const Ref<ZyiDamageExtraMetaResource> &p_extra_meta, bool p_is_append, double p_crit, double p_crit_damage_rate) :
		value(p_value), real_value(p_real_value), type(p_type), source(p_source), extra_meta(p_extra_meta), is_append(p_is_append), crit_rate(p_crit), crit_damage_rate(p_crit_damage_rate) {
}

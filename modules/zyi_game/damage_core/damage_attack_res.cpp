#include "damage_attack_res.h"
#include "core/object/object.h"
#include "core/os/memory.h"
#include "core/variant/variant.h"
#include "core/variant/variant_utility.h"

void ZyiDamageAttackResource::_bind_methods() {
	ClassDB::bind_static_method("ZyiDamageAttackResource", D_METHOD("create", "value", "real_value", "type", "source", "extra_meta", "is_append", "crit_rate", "crit_damage_rate", "children"), &ZyiDamageAttackResource::create, DEFVAL(0), DEFVAL(0), DEFVAL(ZyiDamageConstant::get_type_total()), DEFVAL(""), DEFVAL(Variant()), DEFVAL(false), DEFVAL(0), DEFVAL(ZyiDamageConstant::get_default_crit_damage_rate()), DEFVAL(TypedArray<ZyiDamageAttackResource>()));

	ClassDB::bind_method(D_METHOD("get_value"), &ZyiDamageAttackResource::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &ZyiDamageAttackResource::set_value);
	ClassDB::bind_method(D_METHOD("get_real_value"), &ZyiDamageAttackResource::get_real_value);
	ClassDB::bind_method(D_METHOD("set_real_value", "real_value"), &ZyiDamageAttackResource::set_real_value);
	ClassDB::bind_method(D_METHOD("get_type"), &ZyiDamageAttackResource::get_type);
	ClassDB::bind_method(D_METHOD("set_type", "type"), &ZyiDamageAttackResource::set_type);
	ClassDB::bind_method(D_METHOD("get_source"), &ZyiDamageAttackResource::get_source);
	ClassDB::bind_method(D_METHOD("set_source", "source"), &ZyiDamageAttackResource::set_source);
	ClassDB::bind_method(D_METHOD("get_extra_meta"), &ZyiDamageAttackResource::get_extra_meta);
	ClassDB::bind_method(D_METHOD("set_extra_meta", "extra_meta"), &ZyiDamageAttackResource::set_extra_meta);
	ClassDB::bind_method(D_METHOD("get_is_append"), &ZyiDamageAttackResource::get_is_append);
	ClassDB::bind_method(D_METHOD("set_is_append", "is_append"), &ZyiDamageAttackResource::set_is_append);
	ClassDB::bind_method(D_METHOD("get_crit_rate"), &ZyiDamageAttackResource::get_crit_rate);
	ClassDB::bind_method(D_METHOD("set_crit_rate", "crit_rate"), &ZyiDamageAttackResource::set_crit_rate);
	ClassDB::bind_method(D_METHOD("get_crit_damage_rate"), &ZyiDamageAttackResource::get_crit_damage_rate);
	ClassDB::bind_method(D_METHOD("set_crit_damage_rate", "crit_damage_rate"), &ZyiDamageAttackResource::set_crit_damage_rate);
	ClassDB::bind_method(D_METHOD("get_children"), &ZyiDamageAttackResource::get_children);
	ClassDB::bind_method(D_METHOD("set_children", "children"), &ZyiDamageAttackResource::set_children);

	ClassDB::bind_method(D_METHOD("deep_clone"), &ZyiDamageAttackResource::deep_clone);
	ClassDB::bind_method(D_METHOD("multiply", "rate"), &ZyiDamageAttackResource::multiply);
	ClassDB::bind_method(D_METHOD("build_value"), &ZyiDamageAttackResource::build_value);
	ClassDB::bind_method(D_METHOD("merge_to_value_by_type"), &ZyiDamageAttackResource::merge_to_value_by_type);
	ClassDB::bind_method(D_METHOD("flat_and_merge_to_value_by_type"), &ZyiDamageAttackResource::flat_and_merge_to_value_by_type);
	ClassDB::bind_method(D_METHOD("flat_to_value"), &ZyiDamageAttackResource::flat_to_value);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "value"), "set_value", "get_value");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "real_value"), "set_real_value", "get_real_value");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "type"), "set_type", "get_type");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "source"), "set_source", "get_source");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "extra_meta", PROPERTY_HINT_RESOURCE_TYPE, "ZyiDamageExtraMetaResource"), "set_extra_meta", "get_extra_meta");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_append"), "set_is_append", "get_is_append");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "crit_rate"), "set_crit_rate", "get_crit_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "crit_damage_rate"), "set_crit_damage_rate", "get_crit_damage_rate");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "children", PROPERTY_HINT_RESOURCE_TYPE, "ZyiDamageAttackResource"), "set_children", "get_children");
}

int64_t ZyiDamageAttackResource::get_value() const {
	return value;
}

void ZyiDamageAttackResource::set_value(int64_t p_value) {
	value = p_value;
}

int64_t ZyiDamageAttackResource::get_real_value() const {
	return real_value;
}

void ZyiDamageAttackResource::set_real_value(int64_t p_real_value) {
	real_value = p_real_value;
}

String ZyiDamageAttackResource::get_type() const {
	return type;
}

void ZyiDamageAttackResource::set_type(const String &p_type) {
	type = p_type;
}

String ZyiDamageAttackResource::get_source() const {
	return source;
}

void ZyiDamageAttackResource::set_source(const String &p_source) {
	source = p_source;
}

Ref<ZyiDamageExtraMetaResource> ZyiDamageAttackResource::get_extra_meta() const {
	return extra_meta;
}

void ZyiDamageAttackResource::set_extra_meta(const Ref<ZyiDamageExtraMetaResource> &p_extra_meta) {
	extra_meta = p_extra_meta;
}

bool ZyiDamageAttackResource::get_is_append() const {
	return is_append;
}

void ZyiDamageAttackResource::set_is_append(bool p_is_append) {
	is_append = p_is_append;
}

double ZyiDamageAttackResource::get_crit_rate() const {
	return crit_rate;
}

void ZyiDamageAttackResource::set_crit_rate(double p_crit_rate) {
	crit_rate = p_crit_rate;
}

double ZyiDamageAttackResource::get_crit_damage_rate() const {
	return crit_damage_rate;
}

void ZyiDamageAttackResource::set_crit_damage_rate(double p_crit_damage_rate) {
	crit_damage_rate = p_crit_damage_rate;
}

TypedArray<ZyiDamageAttackResource> ZyiDamageAttackResource::get_children() const {
	return children;
}

void ZyiDamageAttackResource::set_children(const TypedArray<ZyiDamageAttackResource> &p_children) {
	children = p_children;
}

Ref<ZyiDamageAttackResource> ZyiDamageAttackResource::deep_clone() {
	TypedArray<ZyiDamageAttackResource> result_children = {};
	for (const Ref<ZyiDamageAttackResource> &item : children) {
		result_children.push_back(item->deep_clone());
	}
	Ref<ZyiDamageAttackResource> result = memnew(ZyiDamageAttackResource(value, real_value, type, source, extra_meta, is_append, crit_rate, crit_damage_rate, result_children));
	return result;
}

Ref<ZyiDamageAttackResource> ZyiDamageAttackResource::multiply(double p_rate) {
	TypedArray<ZyiDamageAttackResource> result_children = {};
	for (const Ref<ZyiDamageAttackResource> &item : children) {
		result_children.push_back(item->deep_clone());
	}
	Ref<ZyiDamageAttackResource> result = memnew(ZyiDamageAttackResource(VariantUtilityFunctions::ceili(value * p_rate), VariantUtilityFunctions::ceili(real_value * p_rate), type, source, extra_meta, is_append, crit_rate, crit_damage_rate, result_children));
	return result;
}

Ref<ZyiDamageValue> ZyiDamageAttackResource::build_value() {
	Ref<ZyiDamageValue> result = memnew(ZyiDamageValue(value, real_value, type, source, extra_meta, is_append, crit_rate, crit_damage_rate));
	return result;
}

Dictionary ZyiDamageAttackResource::merge_to_value_by_type() {
	Dictionary type_to_value;
	type_to_value[type] = build_value();
	TypedArray<ZyiDamageAttackResource> data_queue = children.duplicate();
	while (!data_queue.is_empty()) {
		Ref<ZyiDamageAttackResource> child = data_queue.pop_front();
		String child_type = child->type;
		Ref<ZyiDamageValue> damage = type_to_value.get_valid(child_type);
		if (damage == nullptr) {
			type_to_value[child_type] = child->build_value();
		} else {
			damage->merge_value_with(child->build_value());
		}
		if (!child->children.is_empty()) {
			data_queue.append_array(child->children);
		}
	}
	return type_to_value;
}

TypedArray<ZyiDamageValue> ZyiDamageAttackResource::flat_and_merge_to_value_by_type() {
	Dictionary type_to_value = merge_to_value_by_type();
	TypedArray<ZyiDamageValue> result;
	for (const Ref<ZyiDamageValue> &item : type_to_value.values()) {
		result.push_back(item);
	}
	return result;
}

TypedArray<ZyiDamageValue> ZyiDamageAttackResource::flat_to_value() {
	TypedArray<ZyiDamageValue> result;
	result.push_back(build_value());
	TypedArray<ZyiDamageAttackResource> data_queue = children.duplicate();
	while (!data_queue.is_empty()) {
		Ref<ZyiDamageAttackResource> child = data_queue.pop_front();
		result.append(child->build_value());
		if (!child->children.is_empty()) {
			data_queue.append_array(child->children);
		}
	}
	return result;
}

Ref<ZyiDamageAttackResource> ZyiDamageAttackResource::create(int64_t p_value, int64_t p_real_value, const String &p_type, const String &p_source, const Ref<ZyiDamageExtraMetaResource> &p_extra_meta, bool p_is_append, double p_crit, double p_crit_damage_rate, const TypedArray<ZyiDamageAttackResource> &p_children) {
	Ref<ZyiDamageAttackResource> result = memnew(ZyiDamageAttackResource(p_value, p_real_value, p_type, p_source, p_extra_meta, p_is_append, p_crit, p_crit_damage_rate, p_children));
	return result;
}

ZyiDamageAttackResource::ZyiDamageAttackResource(int64_t p_value, int64_t p_real_value, const String &p_type, const String &p_source, const Ref<ZyiDamageExtraMetaResource> &p_extra_meta, bool p_is_append, double p_crit, double p_crit_damage_rate, const TypedArray<ZyiDamageAttackResource> &p_children) :
		value(p_value), real_value(p_real_value), type(p_type), source(p_source), extra_meta(p_extra_meta), is_append(p_is_append), crit_rate(p_crit), crit_damage_rate(p_crit_damage_rate), children(p_children) {
}

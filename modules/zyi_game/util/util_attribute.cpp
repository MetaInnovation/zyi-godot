#include "util_attribute.h"
#include "util_object_helper.h"

void ZyiUtilAttribute::_bind_methods() {
	ClassDB::bind_method(D_METHOD("keys"), &ZyiUtilAttribute::keys);
	ClassDB::bind_method(D_METHOD("get_data"), &ZyiUtilAttribute::get_data);
	ClassDB::bind_method(D_METHOD("set_data", "data"), &ZyiUtilAttribute::set_data);
	ClassDB::bind_method(D_METHOD("get_v", "key", "default"), &ZyiUtilAttribute::get_v, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("duplicate", "deep"), &ZyiUtilAttribute::duplicate, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_json"), &ZyiUtilAttribute::to_json);
	ClassDB::bind_method(D_METHOD("set_v", "key", "value", "disable_override"), &ZyiUtilAttribute::set_v, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("remove_v", "key"), &ZyiUtilAttribute::remove_v);
	ClassDB::bind_method(D_METHOD("from_json", "data"), &ZyiUtilAttribute::from_json);
	ClassDB::bind_method(D_METHOD("clear"), &ZyiUtilAttribute::clear);
	ClassDB::bind_method(D_METHOD("merge_from_other", "other"), &ZyiUtilAttribute::merge_from_other);
	ClassDB::bind_method(D_METHOD("merge_from_json", "data"), &ZyiUtilAttribute::merge_from_json);

	ADD_SIGNAL(MethodInfo(SNAME("changed"), PropertyInfo(Variant::STRING, "key"), PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), PropertyInfo(Variant::NIL, "old_value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT)));
	ADD_SIGNAL(MethodInfo(SNAME("changed_without_payload")));

	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data"), "set_data", "get_data");
}

Array ZyiUtilAttribute::keys() const {
	return data.keys();
}
Dictionary ZyiUtilAttribute::get_data() {
	return data;
}
void ZyiUtilAttribute::set_data(const Dictionary &p_data) {
	data = p_data;
}

Variant ZyiUtilAttribute::get_v(const String &p_key, const Variant &p_default) {
	const Variant *value = data.getptr(p_key);
	if (!value) {
		return p_default;
	}
	if (ZyiUtilObjectHelper::check_is_invalid_object(*value)) {
		remove_v(p_key);
		return Variant();
	}
	return *value;
}

Ref<ZyiUtilAttribute> ZyiUtilAttribute::duplicate(bool p_deep) const {
	Ref<ZyiUtilAttribute> result = memnew(ZyiUtilAttribute());
	result->data = data.duplicate(p_deep);
	return result;
}

// 返回一个常量引用，指向data
const Dictionary &ZyiUtilAttribute::to_json() const {
	return data;
}

bool ZyiUtilAttribute::set_v(const String &p_key, const Variant &p_value, const bool disable_override) {
	if (disable_override && data.has(p_key)) {
		return false;
	}
	const Variant *value = data.getptr(p_key);
	if (!value || value != &p_value) {
		data[p_key] = p_value;
		if (!value) {
			emit_signal(SNAME("changed"), p_key, p_value, Variant());
		} else {
			emit_signal(SNAME("changed"), p_key, p_value, *value);
		}
		emit_signal(SNAME("changed_without_payload"));
		return true;
	}
	return false;
}

// 删除指定键的值
bool ZyiUtilAttribute::remove_v(const String &p_key) {
	const Variant &v_nil = Variant();
	const Variant &value = data.get(p_key, v_nil);
	if (data.erase(p_key)) {
		emit_signal(SNAME("changed"), p_key, v_nil, value);
		emit_signal(SNAME("changed_without_payload"));
	}
	return false;
}

void ZyiUtilAttribute::from_json(const Dictionary &p_data) {
	data.clear();
	data.merge(p_data);
}

void ZyiUtilAttribute::clear() {
	data.clear();
}

void ZyiUtilAttribute::merge_from_other(const Ref<ZyiUtilAttribute> &p_other) {
	data.merge(p_other->data, true);
}

void ZyiUtilAttribute::merge_from_json(const Dictionary &p_data) {
	data.merge(p_data, true);
}
ZyiUtilAttribute::ZyiUtilAttribute(const Dictionary &p_data) :
		data(p_data) {
}

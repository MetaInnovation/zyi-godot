#include "util_attr_manager.h"

void ZyiUtilAttrManager::_bind_methods() {
	ClassDB::bind_static_method("ZyiUtilAttrManager", D_METHOD("create", "getter", "setter"), &ZyiUtilAttrManager::create);
	ClassDB::bind_method(D_METHOD("get_attr_list"), &ZyiUtilAttrManager::get_attr_list);
	ClassDB::bind_method(D_METHOD("has_attr", "key"), &ZyiUtilAttrManager::has_attr);
	ClassDB::bind_method(D_METHOD("get_v", "key"), &ZyiUtilAttrManager::get_v);
	ClassDB::bind_method(D_METHOD("set_v", "key", "value"), &ZyiUtilAttrManager::set_v);
	ClassDB::bind_method(D_METHOD("force_exclude", "key"), &ZyiUtilAttrManager::force_exclude);
	ClassDB::bind_method(D_METHOD("add_attr", "key", "override", "getter", "setter"), &ZyiUtilAttrManager::add_attr, DEFVAL(false), DEFVAL(Variant()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("clear"), &ZyiUtilAttrManager::clear);
}

Ref<ZyiUtilAttrManager> ZyiUtilAttrManager::create(const Callable &p_getter, const Callable &p_setter) {
	Ref<ZyiUtilAttrManager> result = memnew(ZyiUtilAttrManager());
	result->_getter = p_getter;
	result->_setter = p_setter;
	return result;
}

Array ZyiUtilAttrManager::get_attr_list() const {
	Array result;
	if (attr_map.is_empty()) {
		return result;
	}
	result.resize(attr_map.size());
	int i = 0;
	for (const KeyValue<StringName, AttrConfig> &item : attr_map) {
		result[i] = item.key;
		i++;
	}
	return result;
}

bool ZyiUtilAttrManager::has_attr(const String &p_key) const {
	return attr_map.has(p_key);
}

Variant ZyiUtilAttrManager::get_v(const String &p_key) {
	AttrConfig *data = attr_map.getptr(p_key);
	Variant ret;
	if (data == nullptr) {
		return ret;
	}
	Callable handler = _getter;
	if (data->getter.is_valid()) {
		handler = data->getter;
	}
	Callable::CallError ce;
	handler.callp(nullptr, 0, ret, ce);
	if (ce.error != Callable::CallError::CALL_OK) {
		ERR_PRINT(vformat("Error calling ZyiUtilAttrManager get_v '%s' to callable: %s.", String(handler.get_method()), Variant::get_callable_error_text(handler, nullptr, 0, ce)));
	}
	return ret;
}

void ZyiUtilAttrManager::set_v(const String &p_key, const Variant &p_value) {
	AttrConfig *data = attr_map.getptr(p_key);
	if (data == nullptr) {
		return;
	}
	Callable handler = _setter;
	if (data->getter.is_valid()) {
		handler = data->setter;
	}
	Callable::CallError ce;
	Variant ret;
	const Variant *argptrs[2];
	const Variant key = p_key;
	argptrs[0] = &key;
	argptrs[1] = &p_value;
	handler.callp(argptrs, 2, ret, ce);
	if (ce.error != Callable::CallError::CALL_OK) {
		ERR_PRINT(vformat("Error calling ZyiUtilAttrManager get_v '%s' to callable: %s.", String(handler.get_method()), Variant::get_callable_error_text(handler, argptrs, 2, ce)));
	}
}

void ZyiUtilAttrManager::force_exclude(const String &p_key) {
	force_exclude_attr_set.insert(p_key);
	attr_map.erase(p_key);
}

void ZyiUtilAttrManager::add_attr(const String &p_key, bool override, const Variant &p_getter, const Variant &p_setter) {
	if (force_exclude_attr_set.has(p_key)) {
		return;
	}
	if (!override && attr_map.has(p_key)) {
		return;
	}
	attr_map[p_key] = AttrConfig{ p_getter, p_setter };
}

void ZyiUtilAttrManager::clear() {
	attr_map.clear();
}

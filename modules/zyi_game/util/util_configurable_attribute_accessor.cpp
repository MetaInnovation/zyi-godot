#include "util_configurable_attribute_accessor.h"

void ZyiUtilConfigurableAttributeAccessor::emit_change_without_payload() {
	emit_signal(SNAME("changed_without_payload"));
}

void ZyiUtilConfigurableAttributeAccessor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("config_key", "key", "getter", "set_callback"), &ZyiUtilConfigurableAttributeAccessor::config_key);
	ClassDB::bind_method(D_METHOD("reserve", "capacity"), &ZyiUtilConfigurableAttributeAccessor::reserve);
	ClassDB::bind_method(D_METHOD("to_dict"), &ZyiUtilConfigurableAttributeAccessor::to_dict);
	ClassDB::bind_method(D_METHOD("get_v", "key", "default"), &ZyiUtilConfigurableAttributeAccessor::get_v, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("set_v", "key", "value"), &ZyiUtilConfigurableAttributeAccessor::set_v);
	ClassDB::bind_method(D_METHOD("remove_v", "key"), &ZyiUtilConfigurableAttributeAccessor::remove_v);
	ClassDB::bind_method(D_METHOD("clear", "include_config"), &ZyiUtilConfigurableAttributeAccessor::clear, DEFVAL(false));

	ADD_SIGNAL(MethodInfo(SNAME("changed_without_payload")));
}

void ZyiUtilConfigurableAttributeAccessor::config_key(const String &p_key, const Callable &p_getter, const Callable &p_set_callback) {
	Data *data = _map.getptr(p_key);
	if (!data) {
		Data new_data{
			false,
			p_getter,
			p_set_callback
		};
		_map[p_key] = new_data;
	} else {
		data->getter = p_getter;
		data->set_callback = p_set_callback;
	}
	emit_change_without_payload();
}

void ZyiUtilConfigurableAttributeAccessor::reserve(int64_t capacity) {
	_map.reserve(capacity);
}

Dictionary ZyiUtilConfigurableAttributeAccessor::to_dict() {
	return _data;
}

Variant ZyiUtilConfigurableAttributeAccessor::get_v(const String &p_key, const Variant &p_default) {
	Variant *val = _data.getptr(p_key);
	if (val == nullptr) {
		return p_default;
	}
	Data *data = _map.getptr(p_key);
	if (data && data->getter.is_valid()) {
		return data->getter.call(*val);
	} else {
		return *val;
	}
}

void ZyiUtilConfigurableAttributeAccessor::set_v(const String &p_key, const Variant &p_value) {
	if (_data.getptr(p_key) == &p_value) {
		return;
	}
	_data[p_key] = p_value;
	Data *data = _map.getptr(p_key);
	if (data && data->set_callback.is_valid()) {
		data->set_callback.call(p_value);
	}
	emit_change_without_payload();
}

void ZyiUtilConfigurableAttributeAccessor::remove_v(const String &p_key) {
	if (_data.has(p_key)) {
		_data.erase(p_key);
		emit_change_without_payload();
	}
}

void ZyiUtilConfigurableAttributeAccessor::clear(bool include_config) {
	if (_data.is_empty() && (!include_config || _map.is_empty())) {
		return;
	}
	if (include_config) {
		_map.clear();
		_data.clear();
	} else {
		_data.clear();
	}
	emit_change_without_payload();
}

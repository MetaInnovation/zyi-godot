#include "util_configurable_attribute_accessor.h"

void ZyiUtilConfigurableAttributeAccessor::emit_change_without_payload() {
	emit_signal(SNAME("changed_without_payload"));
}

void ZyiUtilConfigurableAttributeAccessor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("config_key", "key", "getter", "set_callback"), &ZyiUtilConfigurableAttributeAccessor::config_key);
	ClassDB::bind_method(D_METHOD("reserve", "capacity"), &ZyiUtilConfigurableAttributeAccessor::reserve);
	ClassDB::bind_method(D_METHOD("get_v", "key", "default"), &ZyiUtilConfigurableAttributeAccessor::get_v, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("set_v", "key", "value"), &ZyiUtilConfigurableAttributeAccessor::set_v);
	ClassDB::bind_method(D_METHOD("remove_v", "key"), &ZyiUtilConfigurableAttributeAccessor::remove_v);
	ClassDB::bind_method(D_METHOD("clear", "include_config"), &ZyiUtilConfigurableAttributeAccessor::clear, DEFVAL(false));

	ADD_SIGNAL(MethodInfo(SNAME("changed_without_payload")));
}

void ZyiUtilConfigurableAttributeAccessor::config_key(const String &p_key, const Callable &p_getter, const Callable &p_set_callback) {
	Data *data = _map.lookup_ptr(p_key);
	if (!data) {
		Data new_data{
			Variant(),
			false,
			p_getter,
			p_set_callback
		};
		_map.set(p_key, new_data);
	} else {
		data->getter = p_getter;
		data->set_callback = p_set_callback;
	}
	emit_change_without_payload();
}

void ZyiUtilConfigurableAttributeAccessor::reserve(int64_t capacity) {
	_map.reserve(capacity);
}

Variant ZyiUtilConfigurableAttributeAccessor::get_v(const String &p_key, const Variant &p_default) {
	Data *data = _map.lookup_ptr(p_key);
	if (!data || data->removed) {
		return p_default;
	}
	if (data->getter.is_valid()) {
		return data->getter.call(data->value);
	} else {
		return data->value;
	}
}

void ZyiUtilConfigurableAttributeAccessor::set_v(const String &p_key, const Variant &p_value) {
	Data *data = _map.lookup_ptr(p_key);
	if (!data) {
		Data new_data{
			p_value,
			false,
			Callable(),
			Callable()
		};
		_map.set(p_key, new_data);
	} else {
		data->removed = false;
		data->value = p_value;
		if (data->set_callback.is_valid()) {
			data->set_callback.call(p_value);
		}
	}
	emit_change_without_payload();
}

void ZyiUtilConfigurableAttributeAccessor::remove_v(const String &p_key) {
	Data *data = _map.lookup_ptr(p_key);
	if (!data || data->removed) {
		return;
	}
	data->value = Variant();
	data->removed = true;
	emit_change_without_payload();
}

void ZyiUtilConfigurableAttributeAccessor::clear(bool include_config) {
	if (include_config) {
		_map.clear();
	} else {
		for (OAHashMap<StringName, ZyiUtilConfigurableAttributeAccessor::Data>::Iterator it = _map.iter(); it.valid; it = _map.next_iter(it)) {
			Data *data = _map.lookup_ptr(*(it.key));
			if (data && !data->removed) {
				data->value = Variant();
				data->removed = true;
			}
		}
	}
	emit_change_without_payload();
}

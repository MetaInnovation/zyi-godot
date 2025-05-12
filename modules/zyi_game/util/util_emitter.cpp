#include "util_emitter.h"

void ZyiUtilEmitter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("check_has_listener", "event_name"), &ZyiUtilEmitter::check_has_listener);
	ClassDB::bind_method(D_METHOD("on", "event_name", "callback"), &ZyiUtilEmitter::on);
	ClassDB::bind_method(D_METHOD("off", "event_name", "callback"), &ZyiUtilEmitter::off);
	ClassDB::bind_method(D_METHOD("off_all", "event_name"), &ZyiUtilEmitter::off_all);
	ClassDB::bind_method(D_METHOD("once", "event_name", "callback"), &ZyiUtilEmitter::once);
	ClassDB::bind_method(D_METHOD("emit", "event_name", "payload"), &ZyiUtilEmitter::emit, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("clear_listeners_map", "event_name"), &ZyiUtilEmitter::clear_listeners_map);
	ClassDB::bind_method(D_METHOD("clear"), &ZyiUtilEmitter::clear);
}

bool ZyiUtilEmitter::check_has_listener(const StringName &p_name) const {
	return listener_map.has(p_name);
}

bool ZyiUtilEmitter::call_listener(const Variant &p_value, const Variant &p_payload) {
	if (p_value.get_type() == Variant::OBJECT) {
		ZyiUtilCallableObject *obj = Object::cast_to<ZyiUtilCallableObject>(p_value);
		if (obj) {
			if (!obj->is_valid()) {
				return false;
			}
			obj->call_with_payload(p_payload);
			obj->remove_handler();
			return false;
		}
		return false;
	} else {
		Callable handler = p_value;
		if (!handler.is_valid()) {
			return false;
		}
		Variant ret;
		Callable::CallError ce;
		const Variant *argptrs[1];
		argptrs[0] = &p_payload;
		handler.callp(argptrs, 1, ret, ce);
		if (ce.error != Callable::CallError::CALL_OK) {
			ERR_PRINT(vformat("Error calling ZyiUtilEmitter listener '%s' to callable: %s.", String(handler.get_method()), Variant::get_callable_error_text(handler, argptrs, 1, ce)));
		}
		return true;
	}
}

void ZyiUtilEmitter::on(const StringName &p_name, const Callable &p_callback) {
	_on(p_name, p_callback);
}

void ZyiUtilEmitter::_on(const StringName &p_name, const Variant &p_callback) {
	Array *items = listener_map.getptr(p_name);
	if (!items) {
		listener_map[p_name] = Array();
		items = &listener_map[p_name];
	}
	items->push_back(p_callback);
}

void ZyiUtilEmitter::off(const StringName &p_name, const Callable &p_callback) {
	Array *items = listener_map.getptr(p_name);
	if (!items) {
		return;
	}
	ZyiUtilCallableHelper::erase_callable_from_array(*items, p_callback);
}

bool ZyiUtilEmitter::off_all(const StringName &p_name) {
	return listener_map.erase(p_name);
}

void ZyiUtilEmitter::once(const StringName &p_name, const Callable &p_callback) {
	_on(p_name, ZyiUtilCallableObject::create(p_callback));
}

void ZyiUtilEmitter::emit(const StringName &p_name, const Variant &p_payload) {
	Array *items = listener_map.getptr(p_name);
	if (!items) {
		return;
	}
	LocalVector<int64_t> invalid_indices;
	int64_t size = items->size();
	for (int64_t i = 0; i < size; i++) {
		if (!call_listener((*items)[i], p_payload)) {
			invalid_indices.push_back(i);
		}
	}
	if (invalid_indices.is_empty()) {
		return;
	}
	// 先删除大的索引，这样删除后的移位代价更小
	for (int64_t i = invalid_indices.size() - 1; i >= 0; i--) {
		items->remove_at(invalid_indices[i]);
	}
}

void ZyiUtilEmitter::clear_listeners_map(const StringName &p_name) {
	off_all(p_name);
}

void ZyiUtilEmitter::clear() {
	listener_map.clear();
}

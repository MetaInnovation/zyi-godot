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

void ZyiUtilEmitter::on(const StringName &p_name, const Callable &p_callback) {
	_on(p_name, p_callback, false);
}

void ZyiUtilEmitter::_on(const StringName &p_name, const Callable &p_callback, bool p_once) {
	std::vector<ZyiUtilEmitter::InternalListenerItem> *data = listener_map.getptr(p_name);
	if (data == nullptr) {
		listener_map[p_name] = std::vector<ZyiUtilEmitter::InternalListenerItem>();
		data = &listener_map[p_name];
	}
	data->push_back(InternalListenerItem{ p_callback, p_once });
}

void ZyiUtilEmitter::off(const StringName &p_name, const Callable &p_callback) {
	std::vector<ZyiUtilEmitter::InternalListenerItem> *data = listener_map.getptr(p_name);
	if (data == nullptr) {
		return;
	}
	data->erase(std::remove_if(data->begin(), data->end(),
						[&p_callback](const InternalListenerItem &item) {
							return ZyiUtilCallableHelper::is_same_callable(item.callback, p_callback);
						}),
			data->end());
}

bool ZyiUtilEmitter::off_all(const StringName &p_name) {
	return listener_map.erase(p_name);
}

void ZyiUtilEmitter::once(const StringName &p_name, const Callable &p_callback) {
	_on(p_name, p_callback, true);
}

void ZyiUtilEmitter::emit(const StringName &p_name, const Variant &p_payload) {
	std::vector<ZyiUtilEmitter::InternalListenerItem> *data = listener_map.getptr(p_name);
	if (data == nullptr) {
		return;
	}
	int64_t size = data->size();
	LocalVector<Callable> callback_list;
	for (int64_t i = 0; i < size; i++) {
		ZyiUtilEmitter::InternalListenerItem &listener_item = (*data)[i];
		if (listener_item.invalid) {
			continue;
		}
		if (listener_item.callback.is_valid()) {
			// 先复制出去，避免这里直接执行callback触发off导致data被修改
			callback_list.push_back(listener_item.callback);
			if (listener_item.once) {
				listener_item.invalid = true;
			}
		} else {
			listener_item.invalid = true;
		}
	}
	for (int64_t i = 0; i < callback_list.size(); i++) {
		_call_with_payload(callback_list[i], p_payload);
	}
	// 删除所有 invalid 的元素
	data->erase(std::remove_if(data->begin(), data->end(),
						[](const InternalListenerItem &item) {
							return item.invalid;
						}),
			data->end());
}

void ZyiUtilEmitter::clear_listeners_map(const StringName &p_name) {
	off_all(p_name);
}

void ZyiUtilEmitter::clear() {
	listener_map.clear();
}

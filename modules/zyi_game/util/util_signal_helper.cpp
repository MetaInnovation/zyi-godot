#include "util_signal_helper.h"

void ZyiUtilSignalHelper::_bind_methods() {
	ClassDB::bind_static_method("ZyiUtilSignalHelper", D_METHOD("safe_connect", "sig", "handler", "flags"), &ZyiUtilSignalHelper::safe_connect, DEFVAL(0));
	ClassDB::bind_static_method("ZyiUtilSignalHelper", D_METHOD("safe_disconnect", "sig", "handler"), &ZyiUtilSignalHelper::safe_disconnect);
	ClassDB::bind_static_method("ZyiUtilSignalHelper", D_METHOD("clear_connections", "sig"), &ZyiUtilSignalHelper::clear_connections);
	ClassDB::bind_static_method("ZyiUtilSignalHelper", D_METHOD("object_safe_connect", "obj", "sig_name", "handler", "flags"), &ZyiUtilSignalHelper::object_safe_connect, DEFVAL(0));
	ClassDB::bind_static_method("ZyiUtilSignalHelper", D_METHOD("object_safe_disconnect", "obj", "sig_name", "handler"), &ZyiUtilSignalHelper::object_safe_disconnect);
	ClassDB::bind_static_method("ZyiUtilSignalHelper", D_METHOD("object_clear_connections", "obj", "sig_name"), &ZyiUtilSignalHelper::object_clear_connections);
}

void ZyiUtilSignalHelper::safe_connect(Signal p_signal, const Callable &p_callable, uint32_t p_flags) {
	object_safe_connect(p_signal.get_object(), p_signal.get_name(), p_callable, p_flags);
}

void ZyiUtilSignalHelper::safe_disconnect(Signal p_signal, const Callable &p_callable) {
	object_safe_disconnect(p_signal.get_object(), p_signal.get_name(), p_callable);
}

void ZyiUtilSignalHelper::clear_connections(Signal p_signal) {
	object_clear_connections(p_signal.get_object(), p_signal.get_name());
}

void ZyiUtilSignalHelper::object_safe_connect(Object *p_object, const StringName &p_name, const Callable &p_callable, uint32_t p_flags) {
	if (!p_object) {
		return;
	}
	if (!p_object->is_connected(p_name, p_callable)) {
		p_object->connect(p_name, p_callable, p_flags);
	}
}

void ZyiUtilSignalHelper::object_safe_disconnect(Object *p_object, const StringName &p_name, const Callable &p_callable) {
	if (!p_object) {
		return;
	}
	if (p_object->is_connected(p_name, p_callable)) {
		p_object->disconnect(p_name, p_callable);
	}
}

void ZyiUtilSignalHelper::object_clear_connections(Object *p_object, const StringName &p_name) {
	if (!p_object) {
		return;
	}
	List<Object::Connection> conn_list;
	const String sub = "::";
	p_object->get_signal_connection_list(p_name, &conn_list);
	for (const Object::Connection &conn : conn_list) {
		const Callable cb = conn.callable;
		if (cb.is_valid()) {
			if (cb.is_custom() && (cb.get_method() == "" || String(cb.get_method()).contains(sub))) {
				// 避免提前取消关联: Area2D::_area_enter_tree   Area2D::_area_exit_tree
				continue;
			}
			p_object->disconnect(p_name, cb);
		}
	}
}

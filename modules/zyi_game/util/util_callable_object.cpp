#include "util_callable_object.h"

void ZyiUtilCallableObject::_bind_methods() {
	ClassDB::bind_static_method("ZyiUtilCallableObject", D_METHOD("create", "handler"), &ZyiUtilCallableObject::create);
	ClassDB::bind_method(D_METHOD("get_handler"), &ZyiUtilCallableObject::get_handler);
	ClassDB::bind_method(D_METHOD("set_handler", "handler"), &ZyiUtilCallableObject::set_handler);
	ClassDB::bind_method(D_METHOD("remove_handler"), &ZyiUtilCallableObject::remove_handler);
	ClassDB::bind_method(D_METHOD("is_valid"), &ZyiUtilCallableObject::is_valid);
	ClassDB::bind_method(D_METHOD("call_with_payload", "payload"), &ZyiUtilCallableObject::call_with_payload, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("try_callv", "args"), &ZyiUtilCallableObject::try_callv, DEFVAL_ARRAY);
	ClassDB::bind_method(D_METHOD("call_without_payload"), &ZyiUtilCallableObject::call_without_payload);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "handler"), "set_handler", "get_handler");
}

Ref<ZyiUtilCallableObject> ZyiUtilCallableObject::create(const Callable &p_handler) {
	Ref<ZyiUtilCallableObject> result = memnew(ZyiUtilCallableObject(p_handler));
	return result;
}

const Callable &ZyiUtilCallableObject::get_handler() const {
	return handler;
}

void ZyiUtilCallableObject::set_handler(const Callable &p_handler) {
	handler = p_handler;
}

void ZyiUtilCallableObject::remove_handler() {
	handler = Callable();
}

bool ZyiUtilCallableObject::is_valid() const {
	return handler.is_valid();
}

Variant ZyiUtilCallableObject::call_with_payload(const Variant &p_payload) const {
	return handler.call(p_payload);
}

Variant ZyiUtilCallableObject::try_callv(const Array &p_args) const {
	return handler.callv(p_args);
}

Variant ZyiUtilCallableObject::call_without_payload() const {
	return try_callv();
}

ZyiUtilCallableObject::ZyiUtilCallableObject(const Callable &p_handler) :
		handler(p_handler) {
}

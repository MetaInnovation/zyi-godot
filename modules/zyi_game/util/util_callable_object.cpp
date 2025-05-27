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
	Variant ret;
	if (!handler.is_valid()) {
		return ret;
	}
	Callable::CallError ce;
	const Variant *argptrs[1];
	argptrs[0] = &p_payload;
	handler.callp(argptrs, 1, ret, ce);
	if (ce.error != Callable::CallError::CALL_OK) {
		ERR_PRINT(vformat("Error calling ZyiUtilCallableObject call_with_payload '%s' to callable: %s.", String(handler.get_method()), Variant::get_callable_error_text(handler, argptrs, 1, ce)));
	}
	return ret;
}

Variant ZyiUtilCallableObject::try_callv(const Array &p_args) const {
	Variant ret;
	if (!handler.is_valid()) {
		return ret;
	}
	Callable::CallError ce;
	int p_argcount = p_args.size();
	const Variant **argptrs = nullptr;
	if (p_argcount) {
		argptrs = (const Variant **)alloca(sizeof(Variant *) * p_argcount);
		for (int i = 0; i < p_argcount; i++) {
			argptrs[i] = &p_args[i];
		}
	}
	handler.callp(argptrs, p_argcount, ret, ce);
	if (ce.error != Callable::CallError::CALL_OK) {
		ERR_PRINT(vformat("Error calling ZyiUtilCallableObject try_callv '%s' to callable: %s.", String(handler.get_method()), Variant::get_callable_error_text(handler, argptrs, p_argcount, ce)));
	}
	return ret;
}

Variant ZyiUtilCallableObject::call_without_payload() const {
	return try_callv();
}

ZyiUtilCallableObject::ZyiUtilCallableObject(const Callable &p_handler) :
		handler(p_handler) {
}

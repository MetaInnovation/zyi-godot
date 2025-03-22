#include "util_callable_helper.h"

void ZyiUtilCallableHelper::_bind_methods() {
	ClassDB::bind_static_method("ZyiUtilCallableHelper", D_METHOD("try_callv", "obj", "method", "args"), &ZyiUtilCallableHelper::try_callv, DEFVAL_ARRAY);
}

Variant ZyiUtilCallableHelper::try_callv(Object *p_object, String method, const Array &args) {
	if (!p_object || p_object->is_queued_for_deletion() || !p_object->has_method(method)) {
		return Variant();
	}
	return p_object->callv(method, args);
}

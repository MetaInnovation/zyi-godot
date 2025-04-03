#include "util_callable_helper.h"

void ZyiUtilCallableHelper::_bind_methods() {
	ClassDB::bind_static_method("ZyiUtilCallableHelper", D_METHOD("try_callv", "obj", "method", "args"), &ZyiUtilCallableHelper::try_callv, DEFVAL_ARRAY);
	ClassDB::bind_static_method("ZyiUtilCallableHelper", D_METHOD("erase_callable_from_array", "value", "p_callable"), &ZyiUtilCallableHelper::erase_callable_from_array);
}

Variant ZyiUtilCallableHelper::try_callv(Object *p_object, String method, const Array &args) {
	if (!p_object || p_object->is_queued_for_deletion() || !p_object->has_method(method)) {
		return Variant();
	}
	return p_object->callv(method, args);
}

void ZyiUtilCallableHelper::erase_callable_from_array(Array p_value, const Variant &p_callable) {
	if (p_callable.get_type() != Variant::CALLABLE) {
		p_value.erase(p_callable);
		return;
	}
	Callable handler = p_callable.operator Callable();
	for (int64_t i = 0; i < p_value.size(); i++) {
		Variant item = p_value[i];
		if (item.get_type() == Variant::CALLABLE) {
			Callable callable = item.operator Callable();
			if (callable.get_object_id() == handler.get_object_id() && callable.get_method() == handler.get_method()) {
				p_value.remove_at(i);
				return;
			}
		}
	}
}

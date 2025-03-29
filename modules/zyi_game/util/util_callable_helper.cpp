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
	Callable *handler = Object::cast_to<Callable>(p_callable);
	if (!handler) {
		p_value.erase(p_callable);
		return;
	}
	for (int64_t i = 0; i < p_value.size(); i++) {
		Variant item = p_value[i];
		if (item.get_type() == Variant::CALLABLE) {
			Callable callable = item;
			if (callable.get_object_id() == handler->get_object_id() && callable.get_method() == handler->get_method()) {
				p_value.erase(i);
				return;
			}
		}
	}
}

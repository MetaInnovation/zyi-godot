#ifndef UTIL_CALLABLE_HELPER_H
#define UTIL_CALLABLE_HELPER_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/templates/vector.h"
#include "core/variant/callable.h"
#include "core/variant/variant.h"

class ZyiUtilCallableHelper : public Object {
	GDCLASS(ZyiUtilCallableHelper, Object);

protected:
	static void _bind_methods();

public:
	static Variant try_callv(Object *p_object, String method, const Array &args = {});
	static void erase_callable_from_array(Array p_value, const Variant &p_callable);
};

#endif /* UTIL_CALLABLE_HELPER_H */

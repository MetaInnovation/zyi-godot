#ifndef UTIL_CALLABLE_HELPER_H
#define UTIL_CALLABLE_HELPER_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/variant/callable.h"
#include "core/variant/variant.h"

class ZyiUtilCallableHelper : public Object {
	GDCLASS(ZyiUtilCallableHelper, Object);

protected:
	static void _bind_methods();

public:
	static Variant try_callv(Object *p_object, String method, const Array &args = {});
};

#endif /* UTIL_CALLABLE_HELPER_H */

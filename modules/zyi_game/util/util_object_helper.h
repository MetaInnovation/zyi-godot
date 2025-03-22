#ifndef UTIL_OBJECT_HELPER_H
#define UTIL_OBJECT_HELPER_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/variant/variant.h"

class ZyiUtilObjectHelper : public Object {
	GDCLASS(ZyiUtilObjectHelper, Object);

protected:
	static void _bind_methods();

public:
	static bool check_is_valid_object(const Variant &value);
	static bool safe_check_is_object(const Variant &value);
	static bool check_is_invalid_object(const Variant &value);
};

#endif // UTIL_OBJECT_HELPER_H

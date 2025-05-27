#ifndef UTIL_ASYNC_H
#define UTIL_ASYNC_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/templates/hash_set.h"
#include "core/variant/array.h"
#include "core/variant/variant.h"
#include "modules/gdscript/gdscript.h"

class ZyiUtilAsync : public RefCounted {
	GDCLASS(ZyiUtilAsync, RefCounted);

protected:
	static void _bind_methods();

public:
	static Array debug_get_function_states();
};

#endif /* UTIL_ASYNC_H */

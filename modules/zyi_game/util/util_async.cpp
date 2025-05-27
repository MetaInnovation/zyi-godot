#include "util_async.h"

static Array data;

static void _record_objects(Object *p_obj) {
	GDScriptFunctionState *gnc = Object::cast_to<GDScriptFunctionState>(p_obj);
	if (gnc == nullptr) {
		return;
	}
	data.append(p_obj);
}
void ZyiUtilAsync::_bind_methods() {
	ClassDB::bind_static_method("ZyiUtilAsync", D_METHOD("debug_get_function_states"), &ZyiUtilAsync::debug_get_function_states);
}

Array ZyiUtilAsync::debug_get_function_states() {
	data.clear();
	ObjectDB::debug_objects(_record_objects);
	return data;
}

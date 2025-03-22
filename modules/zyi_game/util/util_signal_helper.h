#ifndef UTIL_SIGNAL_HELPER_H
#define UTIL_SIGNAL_HELPER_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/variant/callable.h"
#include "core/variant/variant.h"

class ZyiUtilSignalHelper : public Object {
	GDCLASS(ZyiUtilSignalHelper, Object);

protected:
	static void _bind_methods();

public:
	static void safe_connect(Signal p_signal, const Callable &p_callable, uint32_t p_flags = 0);
	static void safe_disconnect(Signal p_signal, const Callable &p_callable);
	static void clear_connections(Signal p_signal);
	static void object_safe_connect(Object *p_object, const StringName &p_name, const Callable &p_callable, uint32_t p_flags = 0);
	static void object_safe_disconnect(Object *p_object, const StringName &p_name, const Callable &p_callable);
	static void object_clear_connections(Object *p_object, const StringName &p_name);
};

#endif /* UTIL_SIGNAL_HELPER_H */

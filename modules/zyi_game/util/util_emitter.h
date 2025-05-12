#ifndef UTIL_EMITTER_H
#define UTIL_EMITTER_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/templates/hash_map.h"
#include "core/templates/local_vector.h"
#include "core/variant/array.h"
#include "core/variant/variant.h"
#include "util_callable_helper.h"
#include "util_callable_object.h"

class ZyiUtilEmitter : public RefCounted {
	GDCLASS(ZyiUtilEmitter, RefCounted)

private:
	HashMap<StringName, Array> listener_map;

protected:
	static void _bind_methods();

public:
	bool check_has_listener(const StringName &p_name) const;
	bool call_listener(const Variant &p_value, const Variant &p_payload);
	void on(const StringName &p_name, const Callable &p_callback);
	void _on(const StringName &p_name, const Variant &p_callback);
	void off(const StringName &p_name, const Callable &p_callback);
	bool off_all(const StringName &p_name);
	void once(const StringName &p_name, const Callable &p_callback);
	void emit(const StringName &p_name, const Variant &p_payload = Variant());
	void clear_listeners_map(const StringName &p_name);
	void clear();
};

#endif /* UTIL_EMITTER_H */

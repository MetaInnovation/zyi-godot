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

#include <algorithm>
#include <vector>

class ZyiUtilEmitter : public RefCounted {
	GDCLASS(ZyiUtilEmitter, RefCounted)

private:
	struct InternalListenerItem {
		Callable callback;
		bool once = false;
		String unique_key = "";
		bool invalid = false;
	};
	HashMap<StringName, std::vector<InternalListenerItem>> listener_map;

protected:
	static void _bind_methods();

public:
	bool check_has_listener(const StringName &p_name) const;
	void on(const StringName &p_name, const Callable &p_callback, String p_unique_key = "");
	void _on(const StringName &p_name, const Callable &p_callback, bool p_once, String p_unique_key = "");
	void off(const StringName &p_name, const Callable &p_callback, String p_unique_key = "");
	bool off_all(const StringName &p_name);
	void once(const StringName &p_name, const Callable &p_callback, String p_unique_key = "");
	void emit(const StringName &p_name, const Variant &p_payload = Variant());
	void clear_listeners_map(const StringName &p_name);
	void clear();

	static _ALWAYS_INLINE_ void _call_with_payload(const Callable &p_callback, const Variant &p_payload) {
		if (!p_callback.is_valid()) {
			return;
		}
		Variant ret;
		Callable::CallError ce;
		const Variant *argptrs[1];
		argptrs[0] = &p_payload;
		p_callback.callp(argptrs, 1, ret, ce);
		if (ce.error != Callable::CallError::CALL_OK) {
			ERR_PRINT(vformat("Error calling ZyiUtilEmitter listener '%s' to callable: %s.", String(p_callback.get_method()), Variant::get_callable_error_text(p_callback, argptrs, 1, ce)));
		}
	}
};

#endif /* UTIL_EMITTER_H */

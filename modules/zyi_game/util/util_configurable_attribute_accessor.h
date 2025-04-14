#ifndef UTIL_CONFIGURABLE_ATTRIBUTE_ACCESSOR_H
#define UTIL_CONFIGURABLE_ATTRIBUTE_ACCESSOR_H

#include "core/object/ref_counted.h"
#include "core/templates/hash_map.h"

class ZyiUtilConfigurableAttributeAccessor : public RefCounted {
	GDCLASS(ZyiUtilConfigurableAttributeAccessor, RefCounted);

private:
	struct Data {
		Variant value;
		bool removed;
		Callable getter;
		Callable set_callback;
	};
	HashMap<StringName, Data> _map;
	void emit_change_without_payload();

protected:
	static void _bind_methods();

public:
	void config_key(const String &p_key, const Callable &p_getter, const Callable &p_set_callback);
	void reserve(int64_t capacity);

	Variant get_v(const String &p_key, const Variant &p_default = Variant());
	void set_v(const String &p_key, const Variant &p_value);
	void remove_v(const String &p_key);
	void clear(bool include_config = false);
};
#endif /* UTIL_CONFIGURABLE_ATTRIBUTE_ACCESSOR_H */

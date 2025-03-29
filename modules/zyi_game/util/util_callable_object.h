#ifndef UTIL_CALLABLE_OBJECT_H
#define UTIL_CALLABLE_OBJECT_H

#include "core/object/ref_counted.h"

class ZyiUtilCallableObject : public RefCounted {
	GDCLASS(ZyiUtilCallableObject, RefCounted)

protected:
	static void _bind_methods();

public:
	Callable handler;

	static Ref<ZyiUtilCallableObject> create(const Callable &p_handler);
	const Callable &get_handler() const;
	void set_handler(const Callable &p_handler);
	void remove_handler();
	bool is_valid() const;
	Variant call_with_payload(const Variant &p_payload) const;
	Variant try_callv(const Array &p_args) const;

	ZyiUtilCallableObject(const Callable &p_handler);
	ZyiUtilCallableObject() = default;
};

#endif /* UTIL_CALLABLE_OBJECT_H */

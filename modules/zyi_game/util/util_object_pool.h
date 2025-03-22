#ifndef UTIL_OBJECT_POOL_H
#define UTIL_OBJECT_POOL_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/variant/variant.h"
#include <vector>

class ZyiUtilObjectPool : public RefCounted {
	GDCLASS(ZyiUtilObjectPool, RefCounted);

private:
	std::vector<Object> _vector_pool;

protected:
	static void _bind_methods();

public:
	enum {
		POOL_INIT_CAPACITY = 128
	};
	static Ref<ZyiUtilObjectPool>
	create(int64_t capacity = POOL_INIT_CAPACITY);
	Object &acquire();
	void release(const Object &value);
};

#endif /* UTIL_OBJECT_POOL_H */

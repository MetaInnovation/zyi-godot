#ifndef GAME_OBJECT_POOL_MANAGER_H
#define GAME_OBJECT_POOL_MANAGER_H

#include "../util/util_callable_helper.h"
#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/os/memory.h"
#include "core/variant/variant.h"
#include "node_pool.h"
#include <vector>

class ZyiGameNodePoolManager : public RefCounted {
	GDCLASS(ZyiGameNodePoolManager, RefCounted);

private:
	struct PoolContainer {
		Ref<ZyiNodePool> pool;
		String key;
	};
	std::vector<PoolContainer> _vector_pool_list;

protected:
	static void _bind_methods();

public:
	enum {
		POOL_LIST_INIT_CAPACITY = 8
	};

	static const String META_POOL_RELEASED;
	static const String METHOD_POOL_IS_MANAGED;
	static const String REUSE_METHOD;
	static const String RECYCLE_METHOD;
	static Ref<ZyiGameNodePoolManager> create(int64_t capacity = POOL_LIST_INIT_CAPACITY);
	static bool is_released(const Object *p_object);
	static void mark_released(Object *p_object);
	static void unmark_released(Object *p_object);
	static void reuse_object(Object *p_object);
	static void recycle_object(Object *p_object);
	static bool is_object_manage_by_pool(Object *p_object);

	Ref<ZyiNodePool> add_pool(String key, int64_t capacity = ZyiNodePool::POOL_INIT_CAPACITY);
	Ref<ZyiNodePool> get_pool(String key);
	Ref<ZyiNodePool> get_or_add_pool(String key, int64_t capacity = ZyiNodePool::POOL_INIT_CAPACITY);
	void clean();
};

#endif /* GAME_OBJECT_POOL_MANAGER_H */

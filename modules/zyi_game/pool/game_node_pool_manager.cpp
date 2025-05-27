#include "game_node_pool_manager.h"

const String ZyiGameNodePoolManager::META_POOL_RELEASED = "pool_released";
const String ZyiGameNodePoolManager::METHOD_POOL_IS_MANAGED = "pool_is_managed";
const String ZyiGameNodePoolManager::REUSE_METHOD = "pool_reuse";
const String ZyiGameNodePoolManager::RECYCLE_METHOD = "pool_recycle";

void ZyiGameNodePoolManager::_bind_methods() {
	ClassDB::bind_static_method("ZyiGameNodePoolManager", D_METHOD("create", "capacity"), &ZyiGameNodePoolManager::create, DEFVAL(POOL_LIST_INIT_CAPACITY));
	ClassDB::bind_static_method("ZyiGameNodePoolManager", D_METHOD("is_released", "p_object"), &ZyiGameNodePoolManager::is_released);
	ClassDB::bind_static_method("ZyiGameNodePoolManager", D_METHOD("mark_released", "p_object"), &ZyiGameNodePoolManager::mark_released);
	ClassDB::bind_static_method("ZyiGameNodePoolManager", D_METHOD("unmark_released", "p_object"), &ZyiGameNodePoolManager::unmark_released);
	ClassDB::bind_static_method("ZyiGameNodePoolManager", D_METHOD("reuse_object", "p_object"), &ZyiGameNodePoolManager::reuse_object);
	ClassDB::bind_static_method("ZyiGameNodePoolManager", D_METHOD("recycle_object", "p_object"), &ZyiGameNodePoolManager::recycle_object);
	ClassDB::bind_static_method("ZyiGameNodePoolManager", D_METHOD("is_object_manage_by_pool", "p_object"), &ZyiGameNodePoolManager::is_object_manage_by_pool);

	ClassDB::bind_method(D_METHOD("add_pool", "key", "capacity"), &ZyiGameNodePoolManager::add_pool, DEFVAL(ZyiNodePool::POOL_INIT_CAPACITY));
	ClassDB::bind_method(D_METHOD("get_pool", "key"), &ZyiGameNodePoolManager::get_pool);
	ClassDB::bind_method(D_METHOD("get_or_add_pool", "key", "capacity"), &ZyiGameNodePoolManager::get_or_add_pool, DEFVAL(ZyiNodePool::POOL_INIT_CAPACITY));
	ClassDB::bind_method(D_METHOD("clean", "force_free_node"), &ZyiGameNodePoolManager::clean, DEFVAL(false));
}

Ref<ZyiGameNodePoolManager> ZyiGameNodePoolManager::create(int64_t capacity) {
	Ref<ZyiGameNodePoolManager> result = memnew(ZyiGameNodePoolManager());
	result->_vector_pool_list.reserve(capacity);
	return result;
}

bool ZyiGameNodePoolManager::is_released(const Object *p_object) {
	return p_object->has_meta(META_POOL_RELEASED);
}

void ZyiGameNodePoolManager::mark_released(Object *p_object) {
	p_object->set_meta(META_POOL_RELEASED, true);
}

void ZyiGameNodePoolManager::unmark_released(Object *p_object) {
	p_object->remove_meta(META_POOL_RELEASED);
}

void ZyiGameNodePoolManager::reuse_object(Object *p_object) {
	ZyiUtilCallableHelper::try_callv(p_object, REUSE_METHOD);
}

void ZyiGameNodePoolManager::recycle_object(Object *p_object) {
	ZyiUtilCallableHelper::try_callv(p_object, RECYCLE_METHOD);
}

bool ZyiGameNodePoolManager::is_object_manage_by_pool(Object *p_object) {
	Variant result = ZyiUtilCallableHelper::try_callv(p_object, METHOD_POOL_IS_MANAGED);
	return result.get_type() == Variant::BOOL && result.operator bool();
}

Ref<ZyiNodePool> ZyiGameNodePoolManager::add_pool(String key, int64_t capacity) {
	PoolContainer container = PoolContainer{ ZyiNodePool::create(capacity), key };
	_vector_pool_list.push_back(container);
	return container.pool;
}

Ref<ZyiNodePool> ZyiGameNodePoolManager::get_pool(String key) {
	for (auto item = _vector_pool_list.begin(); item != _vector_pool_list.end(); ++item) {
		if (item->key == key) {
			return item->pool;
		}
	}
	return nullptr;
}

Ref<ZyiNodePool> ZyiGameNodePoolManager::get_or_add_pool(String key, int64_t capacity) {
	Ref<ZyiNodePool> result = get_pool(key);
	if (result.is_null()) {
		result = add_pool(key, capacity);
	}
	return result;
}

void ZyiGameNodePoolManager::clean(bool force_free_node) {
	while (!_vector_pool_list.empty()) {
		PoolContainer data = _vector_pool_list.back();
		data.pool->clean(force_free_node);
		data.pool->set_invalid(true);
		_vector_pool_list.pop_back();
	}
}

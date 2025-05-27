#include "node_pool.h"

void ZyiNodePool::_bind_methods() {
	ClassDB::bind_static_method("ZyiNodePool", D_METHOD("create", "capacity"), &ZyiNodePool::create, DEFVAL(POOL_INIT_CAPACITY));

	ClassDB::bind_method(D_METHOD("acquire_node", "record"), &ZyiNodePool::acquire_node, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("release_node", "node", "record"), &ZyiNodePool::release_node, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("release_node_by_id", "node_id", "record"), &ZyiNodePool::release_node_by_id, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("has_node", "node"), &ZyiNodePool::has_node);
	ClassDB::bind_method(D_METHOD("get_available_count"), &ZyiNodePool::get_available_count);
	ClassDB::bind_method(D_METHOD("clean", "force_free_node"), &ZyiNodePool::clean, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("force_free_active_nodes"), &ZyiNodePool::force_free_active_nodes);

	BIND_CONSTANT(POOL_INIT_CAPACITY);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "available_count"), "", "get_available_count");
}

Ref<ZyiNodePool> ZyiNodePool::create(int64_t capacity) {
	Ref<ZyiNodePool> result = memnew(ZyiNodePool);
	result->_vector_node_pool.reserve(capacity);
	return result;
}

Node *ZyiNodePool::acquire_node(bool record) {
	if (_vector_node_pool.empty()) {
		return nullptr;
	}
	Node *node = _vector_node_pool.back();
	_vector_node_pool.pop_back();
	_active_node_id_set.insert(node->get_instance_id());
	if (record) {
		_node_id_set.erase(node->get_instance_id());
	}
	return node;
}

void ZyiNodePool::release_node(Node *node, bool record) {
	if (node == nullptr) {
		return;
	}
	_vector_node_pool.push_back(node);
	_active_node_id_set.erase(node->get_instance_id());
	if (record) {
		_node_id_set.insert(node->get_instance_id());
	}
}
void ZyiNodePool::release_node_by_id(ObjectID p_node_id, bool record) {
	Object *object = ObjectDB::get_instance(p_node_id);
	Node *node = Object::cast_to<Node>(object);
	release_node(node, record);
}

bool ZyiNodePool::has_node(Node *node) {
	return node && _node_id_set.has(node->get_instance_id());
}

int64_t ZyiNodePool::get_available_count() const {
	return _vector_node_pool.size();
}

void ZyiNodePool::clean(bool force_free_node) {
	while (!_vector_node_pool.empty()) {
		Node *node = _vector_node_pool.back();
		node->queue_free();
		_vector_node_pool.pop_back();
	}
	_node_id_set.clear();
	if (force_free_node) {
		force_free_active_nodes();
	} else {
		_active_node_id_set.clear();
	}
}

void ZyiNodePool::force_free_active_nodes() {
	for (auto item = _active_node_id_set.begin(); item != _active_node_id_set.end(); ++item) {
		Object *obj = ObjectDB::get_instance(ObjectID(*item));
		Node *node = Object::cast_to<Node>(obj);
		if (!node || node->is_queued_for_deletion()) {
			continue;
		}
		node->queue_free();
	}
	_active_node_id_set.clear();
}

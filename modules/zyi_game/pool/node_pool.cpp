#include "node_pool.h"

void ZyiNodePool::_bind_methods() {
	ClassDB::bind_static_method("ZyiNodePool", D_METHOD("create", "capacity"), &ZyiNodePool::create, DEFVAL(POOL_INIT_CAPACITY));

	ClassDB::bind_method(D_METHOD("acquire_node", "record"), &ZyiNodePool::acquire_node, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("release_node", "node", "record"), &ZyiNodePool::release_node, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("has_node", "node"), &ZyiNodePool::has_node);
	ClassDB::bind_method(D_METHOD("get_available_count"), &ZyiNodePool::get_available_count);
	ClassDB::bind_method(D_METHOD("clean"), &ZyiNodePool::clean);

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
	if (record) {
		_node_id_set.remove(node->get_instance_id());
	}
	return node;
}

void ZyiNodePool::release_node(Node *node, bool record) {
	if (node == nullptr) {
		return;
	}
	_vector_node_pool.push_back(node);
	if (record) {
		_node_id_set.add(node->get_instance_id());
	}
}

bool ZyiNodePool::has_node(Node *node) {
	return node && _node_id_set.has(node->get_instance_id());
}

int64_t ZyiNodePool::get_available_count() const {
	return _vector_node_pool.size();
}

void ZyiNodePool::clean() {
	while (!_vector_node_pool.empty()) {
		Node *node = _vector_node_pool.back();
		node->queue_free();
		_vector_node_pool.pop_back();
	}
}

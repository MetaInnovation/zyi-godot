#include "sync_store_node.h"

void ZyiSyncStoreNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_host_node_key", "node"), &ZyiSyncStoreNode::get_host_node_key);
	ClassDB::bind_method(D_METHOD("get_client_node_key", "node"), &ZyiSyncStoreNode::get_client_node_key);
	ClassDB::bind_method(D_METHOD("record_host_node", "node"), &ZyiSyncStoreNode::record_host_node);
	ClassDB::bind_method(D_METHOD("record_client_node", "node", "node_key"), &ZyiSyncStoreNode::record_client_node);
	ClassDB::bind_method(D_METHOD("sync_get_node_or_null", "node_key"), &ZyiSyncStoreNode::sync_get_node_or_null);
	ClassDB::bind_method(D_METHOD("record_custom_node", "node", "node_key"), &ZyiSyncStoreNode::record_custom_node);
	ClassDB::bind_method(D_METHOD("sync_get_custom_node_or_null", "node_key"), &ZyiSyncStoreNode::sync_get_custom_node_or_null);
	ClassDB::bind_method(D_METHOD("clean"), &ZyiSyncStoreNode::clean);
}

uint64_t ZyiSyncStoreNode::get_host_node_key(Node *p_node) {
	if (p_node == nullptr || !VariantUtilityFunctions::is_instance_valid(p_node)) {
		return 0;
	}
	return p_node->get_instance_id();
}

uint64_t ZyiSyncStoreNode::get_client_node_key(Node *p_node) {
	const std::unordered_map<uint64_t, ObjectID>::iterator it = _node_id_to_node_key.find(p_node->get_instance_id());
	if (it == _node_id_to_node_key.end()) {
		return 0;
	}
	return it->second;
}

uint64_t ZyiSyncStoreNode::record_host_node(Node *p_node) {
	if (p_node == nullptr || p_node->is_queued_for_deletion()) {
		return 0;
	}
	const uint64_t node_key = p_node->get_instance_id();
	_node_key_to_node_id[node_key] = node_key;
	return node_key;
}

uint64_t ZyiSyncStoreNode::record_client_node(Node *p_node, uint64_t p_node_key) {
	if (p_node == nullptr || p_node->is_queued_for_deletion() || p_node_key == 0) {
		return 0;
	}
	const ObjectID node_id = _node_key_to_node_id[p_node_key] = p_node->get_instance_id();
	_node_id_to_node_key[node_id] = p_node_key;
	return p_node_key;
}

Node *ZyiSyncStoreNode::sync_get_node_or_null(uint64_t p_node_key) {
	if (p_node_key == 0) {
		return nullptr;
	}
	const std::unordered_map<uint64_t, ObjectID>::iterator it = _node_key_to_node_id.find(p_node_key);
	if (it == _node_key_to_node_id.end()) {
		// 无效的
		return nullptr;
	}
	const ObjectID node_id = it->second;
	Node *result = Object::cast_to<Node>(ObjectDB::get_instance(node_id));
	if (result == nullptr || result->is_queued_for_deletion()) {
		_node_key_to_node_id.erase(p_node_key);
		_node_id_to_node_key.erase(node_id);
		return nullptr;
	}
	return result;
}

uint64_t ZyiSyncStoreNode::record_custom_node(Node *p_node, uint64_t p_node_key) {
	if (p_node == nullptr || p_node->is_queued_for_deletion() || p_node_key == 0) {
		return 0;
	}
	const ObjectID node_id = _custom_node_key_to_node_id[p_node_key] = p_node->get_instance_id();
	_node_id_to_custom_node_key[node_id] = p_node_key;
	return p_node_key;
}

Node *ZyiSyncStoreNode::sync_get_custom_node_or_null(uint64_t p_node_key) {
	if (p_node_key == 0) {
		return nullptr;
	}
	const std::unordered_map<uint64_t, ObjectID>::iterator it = _custom_node_key_to_node_id.find(p_node_key);
	if (it == _custom_node_key_to_node_id.end()) {
		// 无效的
		return nullptr;
	}
	const ObjectID node_id = it->second;
	Node *result = Object::cast_to<Node>(ObjectDB::get_instance(node_id));
	if (result == nullptr || result->is_queued_for_deletion()) {
		_custom_node_key_to_node_id.erase(p_node_key);
		_node_id_to_custom_node_key.erase(node_id);
		return nullptr;
	}
	return result;
}

void ZyiSyncStoreNode::clean() {
	_node_key_to_node_id.clear();
	_node_id_to_node_key.clear();

	_custom_node_key_to_node_id.clear();
	_node_id_to_custom_node_key.clear();
}

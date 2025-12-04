#include "sync_consumer.h"

void ZyiSyncConsumer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_log_handler", "handler"), &ZyiSyncConsumer::set_log_handler);
	ClassDB::bind_method(D_METHOD("mount", "node"), &ZyiSyncConsumer::mount);
	ClassDB::bind_method(D_METHOD("unmount"), &ZyiSyncConsumer::unmount);
	ClassDB::bind_method(D_METHOD("idle", "delta"), &ZyiSyncConsumer::idle);
	ClassDB::bind_method(D_METHOD("call_node_method", "node_key", "method_name", "args", "is_custom_node_key", "node_getter"), &ZyiSyncConsumer::call_node_method, DEFVAL(false), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("queue_reliable_rpc_call_node_method", "node_key", "method_name", "args", "is_custom_node_key", "node_getter"), &ZyiSyncConsumer::queue_reliable_rpc_call_node_method, DEFVAL(false), DEFVAL(Variant()));
}

void ZyiSyncConsumer::set_log_handler(const Callable &p_handler) {
	log_handler = p_handler;
}

void ZyiSyncConsumer::mount(ZyiSyncStoreNode *node) {
	anchor_node = node;
}

void ZyiSyncConsumer::unmount() {
	anchor_node = nullptr;
}

void ZyiSyncConsumer::idle(double delta) {
	while (!_add_queue.empty()) {
		_reliable_rpc_call_node_method_queue.push(_add_queue.front());
		_add_queue.pop();
	}
	if (_lazy_reliable_rpc_call_node_method_queue.empty() && _reliable_rpc_call_node_method_queue.empty()) {
		return;
	}
	_flush_call_node_method_queue();
}

void ZyiSyncConsumer::_flush_call_node_method_queue() {
	// 先处理遗留下来的 lazy
	_flush_call_node_method_queue_with_type(true);
	// 再处理新的
	_flush_call_node_method_queue_with_type(false);
}

void ZyiSyncConsumer::_flush_call_node_method_queue_with_type(bool is_lazy) {
	int8_t max_retry = MAX_RELIABLE_RPC_CALL_NODE_METHOD_RETRY;
	std::queue<InternalCallData> *queue = &_reliable_rpc_call_node_method_queue;
	if (is_lazy) {
		max_retry = MAX_LAZY_RELIABLE_RPC_CALL_NODE_METHOD_RETRY;
		queue = &_lazy_reliable_rpc_call_node_method_queue;
	}
	HashSet<String> invalid_node_key_set;
	while (!queue->empty()) {
		InternalCallData &item = queue->front();
		bool valid = true;
		if (invalid_node_key_set.has(item.node_key)) {
			valid = false;
		} else {
			valid = call_node_method(item.node_key, item.method_name, item.args, item.is_custom_node_key, item.node_getter);
		}
		if (!valid) {
			item.retry_count += 1;
			invalid_node_key_set.insert(item.node_key);
			if (item.retry_count >= max_retry) {
				if (!is_lazy) {
					_lazy_reliable_rpc_call_node_method_queue.push(item);
					log("rpc call failed, put to lazy retry. " + item);
				} else {
					log("rpc call finally failed, " + item);
				}
				queue->pop();
			} else {
				log("rpc call failed, need retry. " + item);
				break;
			}
		} else {
			queue->pop();
		}
	}
}

bool ZyiSyncConsumer::call_node_method(const String &p_node_key, const String &p_method_name, const Array &p_args, bool is_custom_node_key, const Variant &p_custom_node_getter) {
	Node *node = nullptr;
	if (p_custom_node_getter.get_type() == Variant::CALLABLE) {
		Callable node_getter = p_custom_node_getter;
		Variant ret;
		Callable::CallError ce;
		const Variant *argptrs[3];
		const Variant anchor_node_v = anchor_node;
		const Variant node_key = p_node_key;
		const Variant v_is_custom_node_key = Variant(is_custom_node_key);
		argptrs[0] = &anchor_node_v;
		argptrs[1] = &node_key;
		argptrs[2] = &v_is_custom_node_key;
		node_getter.callp(argptrs, 3, ret, ce);
		if (ce.error != Callable::CallError::CALL_OK) {
			ERR_PRINT(vformat("Error calling ZyiSyncConsumer call_node_method '%s' to callable: %s.", String(node_getter.get_method()), Variant::get_callable_error_text(node_getter, argptrs, 3, ce)));
		}
		node = Object::cast_to<Node>(ret);
	} else if (p_node_key.is_valid_int()) {
		if (is_custom_node_key) {
			node = anchor_node->sync_get_custom_node_or_null(p_node_key.to_int());
		} else {
			node = anchor_node->sync_get_node_or_null(p_node_key.to_int());
		}
	} else {
		node = anchor_node->get_node_or_null(p_node_key);
	}
	if (node != nullptr && !node->is_queued_for_deletion()) {
		node->callv(p_method_name, p_args);
		return true;
	} else {
		log("call remote " + p_method_name + " failed for invalid node key " + p_node_key + ".");
		return false;
	}
}

void ZyiSyncConsumer::queue_reliable_rpc_call_node_method(const String &p_node_key, const String &p_method_name, const Array &p_args, bool is_custom_node_key, const Variant &p_custom_node_getter) {
	InternalCallData &item = _add_queue.emplace();
	item.node_key = p_node_key;
	item.method_name = p_method_name;
	item.args = p_args;
	item.retry_count = 0;
	item.is_custom_node_key = is_custom_node_key;
	item.node_getter = p_custom_node_getter;
}

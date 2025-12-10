#ifndef SYNC_CONSUMER_H
#define SYNC_CONSUMER_H

#include "./sync_store_node.h"
#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/templates/hash_set.h"
#include <queue>

class ZyiSyncConsumer : public RefCounted {
	GDCLASS(ZyiSyncConsumer, RefCounted)

protected:
	static void _bind_methods();

public:
	static constexpr const int8_t MAX_LAZY_RELIABLE_RPC_CALL_NODE_METHOD_RETRY = 3;
	static constexpr const int8_t MAX_RELIABLE_RPC_CALL_NODE_METHOD_RETRY = 2;

	struct InternalCallData {
		String node_key = "";
		String method_name = "";
		Array args;
		int8_t retry_count = 0;
		bool is_custom_node_key = false;
		Variant node_getter;
		operator String() {
			return "[" + node_key + "," + method_name + "," + Variant(args).stringify(0) + "," + Variant(is_custom_node_key).stringify(0) + "," + node_getter.stringify(0) + "," + Variant(retry_count).stringify(0) + "]";
		}
	};

	Callable log_handler;
	int8_t max_retry_count;
	int8_t max_lazy_retry_count;
	ZyiSyncStoreNode *anchor_node;
	std::queue<InternalCallData> _add_queue;
	std::queue<InternalCallData> _lazy_reliable_rpc_call_node_method_queue;
	std::queue<InternalCallData> _reliable_rpc_call_node_method_queue;

	_FORCE_INLINE_ void log(const Variant &msg) {
		Callable::CallError ce;
		Variant ret;
		const Variant *argptrs[1];
		argptrs[0] = &msg;
		log_handler.callp(argptrs, 1, ret, ce);
		if (ce.error != Callable::CallError::CALL_OK) {
			ERR_PRINT(vformat("Error calling ZyiSyncConsumer log '%s' to callable: %s.", String(log_handler.get_method()), Variant::get_callable_error_text(log_handler, argptrs, 1, ce)));
		}
	}

	void set_log_handler(const Callable &p_handler);
	void set_max_retry_count(int8_t p_retry_count = MAX_RELIABLE_RPC_CALL_NODE_METHOD_RETRY, int8_t p_lazy_retry_count = MAX_LAZY_RELIABLE_RPC_CALL_NODE_METHOD_RETRY);
	void mount(ZyiSyncStoreNode *node);
	void unmount();
	void idle(double delta);
	void _flush_call_node_method_queue();
	void _flush_call_node_method_queue_with_type(bool is_lazy = false);
	bool call_node_method(const String &p_node_key, const String &p_method_name, const Array &p_args, bool is_custom_node_key = false, const Variant &p_custom_node_getter = Variant());
	void queue_reliable_rpc_call_node_method(const String &p_node_key, const String &p_method_name, const Array &p_args, bool is_custom_node_key = false, const Variant &p_custom_node_getter = Variant());
};

#endif /* SYNC_CONSUMER_H */

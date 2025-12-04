#ifndef SYNC_STORE_NODE_H
#define SYNC_STORE_NODE_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/templates/hash_set.h"
#include "core/variant/variant_utility.h"

#include "scene/main/node.h"
#include <unordered_map>

class ZyiSyncStoreNode : public Node {
	GDCLASS(ZyiSyncStoreNode, Node)

protected:
	static void _bind_methods();

public:
	std::unordered_map<uint64_t, ObjectID> _node_key_to_node_id;
	std::unordered_map<uint64_t, ObjectID> _node_id_to_node_key;

	uint64_t get_host_node_key(Node *p_node);
	uint64_t get_client_node_key(Node *p_node);
	uint64_t record_host_node(Node *p_node);
	uint64_t record_client_node(Node *p_node, uint64_t p_node_key);
	Node *sync_get_node_or_null(uint64_t p_node_key);

	std::unordered_map<uint64_t, ObjectID> _custom_node_key_to_node_id;
	std::unordered_map<uint64_t, ObjectID> _node_id_to_custom_node_key;
	uint64_t record_custom_node(Node *p_node, uint64_t p_node_key);
	Node *sync_get_custom_node_or_null(uint64_t p_node_key);
	void clean();
};

#endif /* SYNC_STORE_NODE_H */

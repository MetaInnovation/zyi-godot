#ifndef NODE_POOL_H
#define NODE_POOL_H

#include "../util/util_set.h"
#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/templates/hash_set.h"
#include "core/variant/variant.h"
#include "scene/main/node.h"
#include <vector>

class ZyiNodePool : public RefCounted {
	GDCLASS(ZyiNodePool, RefCounted);

private:
	std::vector<Node *> _vector_node_pool;
	HashSet<uint64_t> _active_node_id_set;
	HashSet<uint64_t> _node_id_set;
	bool _invalid = false;

protected:
	static void _bind_methods();

public:
	enum {
		POOL_INIT_CAPACITY = 128
	};
	static Ref<ZyiNodePool> create(int64_t capacity = POOL_INIT_CAPACITY);

	bool is_invalid() const { return _invalid; }
	void set_invalid(bool invalid) { _invalid = invalid; }

	Node *acquire_node(bool record = false);
	void release_node(Node *node, bool record = false);
	void release_node_by_id(ObjectID p_node_id, bool record = false);
	bool has_node(Node *node);
	void record_active_node(Node *node);
	int64_t get_available_count() const;
	void clean(bool force_free_node = false);
	void force_free_active_nodes();
};

#endif /* NODE_POOL_H */

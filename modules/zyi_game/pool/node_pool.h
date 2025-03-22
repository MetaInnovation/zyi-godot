#ifndef NODE_POOL_H
#define NODE_POOL_H

#include "../util/util_set.h"
#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/variant/variant.h"
#include "scene/main/node.h"
#include <vector>

class ZyiNodePool : public RefCounted {
	GDCLASS(ZyiNodePool, RefCounted);

private:
	std::vector<Node *> _vector_node_pool;
	ZyiUtilSet _node_id_set;

protected:
	static void _bind_methods();

public:
	enum {
		POOL_INIT_CAPACITY = 128
	};
	static Ref<ZyiNodePool> create(int64_t capacity = POOL_INIT_CAPACITY);
	Node *acquire_node(bool record = false);
	void release_node(Node *node, bool record = false);
	bool has_node(Node *node);
	int64_t get_available_count() const;
	void clean();
};

#endif /* NODE_POOL_H */

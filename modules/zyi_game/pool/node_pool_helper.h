#ifndef NODE2D_POOL_HELPER_H
#define NODE2D_POOL_HELPER_H

#include "../pool/game_node_pool_manager.h"
#include "../util/util_callable_helper.h"
#include "../util/util_callable_object.h"
#include "../util/util_object_helper.h"
#include "../util/util_signal_helper.h"
#include "core/object/callable_method_pointer.h"
#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/variant/variant.h"
#include "node_pool.h"
#include "scene/2d/node_2d.h"
#include "scene/resources/packed_scene.h"

class ZyiNodePoolHelper : public Object {
	GDCLASS(ZyiNodePoolHelper, Object);

protected:
	static void _bind_methods();

public:
	static const String METHOD_RE_READY;

	static void _on_node_tree_entered(Node *node);
	static void handle_lazy_pool_reready(ObjectID p_node_id);
	static Node *prepare_node_by_scene(const Ref<PackedScene> &scene, Ref<ZyiNodePool> pool);
	static void release_node(Node *node, Ref<ZyiNodePool> pool);
	static void lazy_release_pool_node(ObjectID p_node_id, Ref<ZyiNodePool> pool, Ref<ZyiUtilCallableObject> p_callable_obj);
};

#endif /* NODE2D_POOL_HELPER_H */

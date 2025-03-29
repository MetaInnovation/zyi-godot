#include "node_pool_helper.h"

const String ZyiNodePoolHelper::METHOD_RE_READY = "pool_re_ready";

void ZyiNodePoolHelper::_bind_methods() {
	ClassDB::bind_static_method("ZyiNodePoolHelper", D_METHOD("handle_lazy_pool_reready", "node"), &ZyiNodePoolHelper::handle_lazy_pool_reready);
	ClassDB::bind_static_method("ZyiNodePoolHelper", D_METHOD("prepare_node_by_scene", "scene", "pool"), &ZyiNodePoolHelper::prepare_node_by_scene);
	ClassDB::bind_static_method("ZyiNodePoolHelper", D_METHOD("release_node", "node", "pool"), &ZyiNodePoolHelper::release_node);
}

void ZyiNodePoolHelper::_on_node_tree_entered(Node *node) {
	callable_mp_static(&ZyiNodePoolHelper::handle_lazy_pool_reready).call_deferred(node);
}

void ZyiNodePoolHelper::handle_lazy_pool_reready(Node *node) {
	if (node->is_inside_tree()) {
		ZyiUtilCallableHelper::try_callv(node, METHOD_RE_READY);
	}
}

Node *ZyiNodePoolHelper::prepare_node_by_scene(const Ref<PackedScene> &scene, Ref<ZyiNodePool> pool) {
	Node *result = pool->acquire_node();
	if (!result) {
		result = scene->instantiate();
	} else {
		Node2D *node2d = Object::cast_to<Node2D>(result);
		if (node2d) {
			node2d->set_visible(true);
		}
		result->set_process_mode(Node::PROCESS_MODE_INHERIT);
		ZyiGameNodePoolManager::reuse_object(result);
		result->connect(SNAME("tree_entered"), callable_mp_static(&ZyiNodePoolHelper::_on_node_tree_entered).bind(result), CONNECT_ONE_SHOT);
	}
	return result;
}

void ZyiNodePoolHelper::release_node(Node *node, Ref<ZyiNodePool> pool) {
	if (!node || node->is_queued_for_deletion()) {
		return;
	}
	ZyiGameNodePoolManager::recycle_object(node);
	Node2D *node2d = Object::cast_to<Node2D>(node);
	if (node2d) {
		node2d->set_visible(false);
	}
	node->set_process_mode(Node::PROCESS_MODE_DISABLED);
	ZyiUtilSignalHelper::object_clear_connections(node, SceneStringName(tree_entered));
	Node *parent = node->get_parent();
	if (parent) {
		parent->remove_child(node);
	}
	ZyiUtilSignalHelper::object_clear_connections(node, SceneStringName(tree_exiting));
	ZyiUtilSignalHelper::object_clear_connections(node, SceneStringName(tree_exited));
	// 延迟加入对象池
	callable_mp(pool.ptr(), &ZyiNodePool::release_node).call_deferred(node, false);
}

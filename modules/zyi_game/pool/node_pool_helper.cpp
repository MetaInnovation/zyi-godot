#include "node_pool_helper.h"

const String ZyiNodePoolHelper::METHOD_RE_READY = "pool_re_ready";

void ZyiNodePoolHelper::_bind_methods() {
	ClassDB::bind_static_method("ZyiNodePoolHelper", D_METHOD("handle_lazy_pool_reready", "node_id"), &ZyiNodePoolHelper::handle_lazy_pool_reready);
	ClassDB::bind_static_method("ZyiNodePoolHelper", D_METHOD("prepare_node_by_scene", "scene", "pool"), &ZyiNodePoolHelper::prepare_node_by_scene);
	ClassDB::bind_static_method("ZyiNodePoolHelper", D_METHOD("release_node", "node", "pool"), &ZyiNodePoolHelper::release_node);
}

void ZyiNodePoolHelper::_on_node_tree_entered(Node *node) {
	callable_mp_static(&ZyiNodePoolHelper::handle_lazy_pool_reready).call_deferred(node->get_instance_id());
}

void ZyiNodePoolHelper::handle_lazy_pool_reready(ObjectID p_node_id) {
	Object *object = ObjectDB::get_instance(p_node_id);
	if (object == nullptr) {
		return;
	}
	Node *node = Object::cast_to<Node>(object);
	if (node != nullptr && node->is_inside_tree()) {
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
	SceneTree *tree = nullptr;
	if (node->is_inside_tree()) {
		tree = node->get_tree();
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
	// 需要等待1个物理帧，保证处理完物理碰撞；延迟加入对象池
	if (tree != nullptr) {
		Ref<ZyiUtilCallableObject> obj = memnew(ZyiUtilCallableObject());
		// 这里有互相引用，需要在 lazy_release_pool_node 内主动释放
		obj->handler = callable_mp_static(&ZyiNodePoolHelper::lazy_release_pool_node).bind(node->get_instance_id(), pool, obj);
		tree->connect("physics_frame", callable_mp(obj.ptr(), &ZyiUtilCallableObject::call_without_payload), CONNECT_ONE_SHOT);
	} else {
		callable_mp_static(&ZyiNodePoolHelper::lazy_release_pool_node).call_deferred(node->get_instance_id(), pool, Variant());
	}
}

void ZyiNodePoolHelper::lazy_release_pool_node(ObjectID p_node_id, Ref<ZyiNodePool> pool, Ref<ZyiUtilCallableObject> p_callable_obj) {
	// 延迟加入对象池
	callable_mp(pool.ptr(), &ZyiNodePool::release_node_by_id).call_deferred(p_node_id, false);
	if (p_callable_obj.is_valid()) {
		p_callable_obj->remove_handler();
		p_callable_obj.unref();
	}
}

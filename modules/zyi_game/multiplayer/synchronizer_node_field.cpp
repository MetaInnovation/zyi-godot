#include "synchronizer_node_field.h"

void ZyiSynchronizerNodeField::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_node_path"), &ZyiSynchronizerNodeField::get_node_path);
	ClassDB::bind_method(D_METHOD("set_node_path", "node_path"), &ZyiSynchronizerNodeField::set_node_path);
	ClassDB::bind_method(D_METHOD("get_property_path_list"), &ZyiSynchronizerNodeField::get_property_path_list);
	ClassDB::bind_method(D_METHOD("set_property_path_list", "property_path_list"), &ZyiSynchronizerNodeField::set_property_path_list);

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "node_path"), "set_node_path", "get_node_path");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "property_path_list", PROPERTY_HINT_TYPE_STRING), "set_property_path_list", "get_property_path_list");
}

NodePath ZyiSynchronizerNodeField::get_node_path() const {
	return node_path;
}

void ZyiSynchronizerNodeField::set_node_path(NodePath p_node_path) {
	node_path = p_node_path;
}

TypedArray<String> ZyiSynchronizerNodeField::get_property_path_list() {
	return property_path_list;
}

void ZyiSynchronizerNodeField::set_property_path_list(const TypedArray<String> &p_property_path_list) {
	property_path_list = p_property_path_list;
}

Variant ZyiSynchronizerNodeField::get_cache_data(Node *p_controller_node) {
	if (!p_controller_node || !p_controller_node->is_inside_tree()) {
		return Variant();
	}
	Node *node = p_controller_node;
	if (!node_path.is_empty()) {
		node = p_controller_node->get_node_or_null(node_path);
	}
	return node->get_instance_id();
}

Variant ZyiSynchronizerNodeField::get_prepare_data(Node *p_controller_node, const Variant &p_cache_data, bool p_is_update, int8_t p_data_type) {
	if (!p_controller_node || !p_controller_node->is_inside_tree()) {
		return Variant();
	}
	Node *node = nullptr;
	const ObjectID node_id = p_cache_data;
	if (node_id.is_valid()) {
		node = Object::cast_to<Node>(ObjectDB::get_instance(node_id));
	}
	if (p_is_update) {
		return node;
	}
	Array result;
	if (node == nullptr) {
		return result;
	}
	bool is_frequency = p_data_type == DATA_IMPORTANT_TRANSFORM;
	bool is_unfrequency = p_data_type == DATA_OTHER;
	for (int i = 0; i < property_path_list.size(); i++) {
		const String &property_path = property_path_list[i];
		if (is_frequency && !is_frequency_property_path(property_path)) {
			continue;
		}
		if (is_unfrequency && is_frequency_property_path(property_path)) {
			continue;
		}
		Array item = build_data_item(property_path, node->get_indexed(NodePath(property_path).get_as_property_path().get_subnames()));
		result.push_back(item);
	}
	return result;
}

void ZyiSynchronizerNodeField::update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action) {
	if (p_data.is_null()) {
		return;
	}
	Node *node = Object::cast_to<Node>(p_data);
	if (node) {
		node->set_indexed(NodePath(p_value_key).get_as_property_path().get_subnames(), p_value);
	}
}

#ifndef SYNCHRONIZER_NODE_FIELD_H
#define SYNCHRONIZER_NODE_FIELD_H

#include "synchronizer_data_field.h"

class ZyiSynchronizerNodeField : public ZyiSynchronizerDataField {
	GDCLASS(ZyiSynchronizerNodeField, ZyiSynchronizerDataField)

protected:
	static void _bind_methods();

public:
	NodePath node_path;
	TypedArray<String> property_path_list;
	NodePath get_node_path() const;
	void set_node_path(NodePath p_node_path);
	TypedArray<String> get_property_path_list();
	void set_property_path_list(const TypedArray<String> &p_property_path_list);

	Variant get_cache_data(Node *p_controller_node);
	Variant get_prepare_data(Node *p_controller_node, const Variant &p_cache_data = Variant(), bool p_is_update = false);
	void update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action = ACTION_CHANGE);
};

#endif /* SYNCHRONIZER_NODE_FIELD_H */

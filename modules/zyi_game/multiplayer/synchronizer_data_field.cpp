#include "synchronizer_data_field.h"

void ZyiSynchronizerDataField::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_data_value_changed", "value_key", "value", "other"), &ZyiSynchronizerDataField::is_data_value_changed);
	ClassDB::bind_method(D_METHOD("get_cache_data", "controller_node"), &ZyiSynchronizerDataField::get_cache_data);
	ClassDB::bind_method(D_METHOD("get_prepare_data", "controller_node", "cache_data", "is_update"), &ZyiSynchronizerDataField::get_prepare_data, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_data_list", "controller_node", "data"), &ZyiSynchronizerDataField::get_data_list);
	ClassDB::bind_method(D_METHOD("update_data", "controller_node", "data", "value_key", "value", "action_type"), &ZyiSynchronizerDataField::update_data);

	BIND_CONSTANT(ACTION_CHANGE);
	BIND_CONSTANT(ACTION_ADD);
	BIND_CONSTANT(ACTION_REMOVE);
}
bool ZyiSynchronizerDataField::is_data_value_changed(const String &p_value_key, const Variant &p_value, const Variant &p_other) const {
	return p_value != p_other;
}

Variant ZyiSynchronizerDataField::get_cache_data(Node *p_controller_node) {
	return true;
}

Variant ZyiSynchronizerDataField::get_prepare_data(Node *p_controller_node, const Variant &p_cache_data, bool p_is_update) {
	return p_cache_data;
}

Array ZyiSynchronizerDataField::get_data_list(const Variant &p_data) {
	if (p_data.is_array()) {
		return p_data;
	}
	return Array();
}

void ZyiSynchronizerDataField::update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action) {
}

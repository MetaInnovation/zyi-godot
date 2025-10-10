#include "synchronizer_data_field.h"

void ZyiSynchronizerDataField::_bind_methods() {
	GDVIRTUAL_BIND(_get_cache_data, "controller_node");
	GDVIRTUAL_BIND(_get_prepare_data, "controller_node", "cache_data", "is_update");
	GDVIRTUAL_BIND(_update_data, "controller_node", "data", "value_key", "value", "action_type");

	ClassDB::bind_method(D_METHOD("get_cache_data", "controller_node"), &ZyiSynchronizerDataField::get_cache_data);
	ClassDB::bind_method(D_METHOD("get_prepare_data", "controller_node", "cache_data", "is_update"), &ZyiSynchronizerDataField::get_prepare_data, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_data_list", "data"), &ZyiSynchronizerDataField::get_data_list);
	ClassDB::bind_method(D_METHOD("update_data", "controller_node", "data", "value_key", "value", "action_type"), &ZyiSynchronizerDataField::update_data);

	BIND_CONSTANT(ACTION_CHANGE);
	BIND_CONSTANT(ACTION_ADD);
}

Variant ZyiSynchronizerDataField::get_cache_data(Node *p_controller_node) {
	Variant result;
	if (GDVIRTUAL_CALL(_get_cache_data, p_controller_node, result)) {
		return result;
	}
	return true;
}

Variant ZyiSynchronizerDataField::get_prepare_data(Node *p_controller_node, const Variant &p_cache_data, bool p_is_update) {
	Variant result;
	if (GDVIRTUAL_CALL(_get_prepare_data, p_controller_node, p_cache_data, p_is_update, result)) {
		return result;
	}
	return p_cache_data;
}

Array ZyiSynchronizerDataField::get_data_list(const Variant &p_data) {
	if (p_data.is_array()) {
		return p_data;
	}
	return Array();
}

Callable ZyiSynchronizerDataField::get_threading_data_list_normalizer() {
	return Callable();
}

void ZyiSynchronizerDataField::update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action) {
	GDVIRTUAL_CALL(_update_data, p_controller_node, p_data, p_value_key, p_value, p_action);
}

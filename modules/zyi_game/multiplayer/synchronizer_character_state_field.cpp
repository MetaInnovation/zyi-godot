#include "synchronizer_character_state_field.h"

Variant ZyiSynchronizerCharacterStateField::get_prepare_data(Node *p_controller_node, const Variant &p_cache_data, bool p_is_update) {
	if (!is_valid_unit(p_controller_node)) {
		return Variant();
	}
	ZyiUtilAttrManager *attr_manager = get_sync_attr_manager(p_controller_node);
	if (attr_manager == nullptr) {
		return Variant();
	}
	Array result;
	for (const KeyValue<StringName, ZyiUtilAttrManager::AttrConfig> &item : attr_manager->attr_map) {
		const String item_value_key = format_value_key(InternalDataSourceType::SYNC_ATTR_MANAGER, SYNC_ATTR_MANAGER_ATTR_MAP_PREFIX + item.key);
		const ZyiUtilAttrManager::AttrConfig &item_cfg = item.value;
		Array args;
		args.push_back(item_cfg.getter);
		args.push_back(item_cfg.setter);
		result.push_back(build_data_item(item_value_key, p_controller_node->callv(METHOD_FORMAT_ATTR_HANDLERS, args)));
	}
	for (const KeyValue<StringName, ZyiUtilAttrManager::AttrConfig> &item : attr_manager->attr_map) {
		const String value_key = format_value_key(InternalDataSourceType::EXTENSIBLE_STATE, item.key);
		const Variant &value = attr_manager->get_v(item.key);
		result.push_back(build_data_item(value_key, value));
	}
	return result;
}

void ZyiSynchronizerCharacterStateField::update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action) {
	if (p_controller_node.is_null()) {
		return;
	}
	Node *controller_node = Object::cast_to<Node>(p_controller_node);
	if (controller_node == nullptr) {
		return;
	}
	const ZyiSynchronizerCharacterStateField::InternalKeyContext key_context = parse_value_key(p_value_key);
	int8_t type = key_context.type;
	String key = key_context.key;
	switch (type) {
		case InternalDataSourceType::SYNC_ATTR_MANAGER: {
			ZyiUtilAttrManager *attr_manager = get_sync_attr_manager(controller_node);
			if (key.begins_with(SYNC_ATTR_MANAGER_ATTR_MAP_PREFIX)) {
				const String attr_key = key.substr(SYNC_ATTR_MANAGER_ATTR_MAP_PREFIX_SIZE);
				Array args;
				args.push_back(p_value);
				Variant data = controller_node->callv(METHOD_PARSE_ATTR_HANDLERS, args);
				if (data.is_array()) {
					Array arr = data;
					attr_manager->attr_map[attr_key] = ZyiUtilAttrManager::AttrConfig{ arr[0], arr[1] };
				}
			}
		} break;
		case InternalDataSourceType::EXTENSIBLE_STATE: {
			ZyiUtilAttrManager *attr_manager = get_sync_attr_manager(controller_node);
			attr_manager->set_v(key, p_value);
		} break;
		default:
			break;
	}
}

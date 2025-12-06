#include "synchronizer_character_state_field.h"

Array ZyiSynchronizerCharacterStateField::threading_data_list_normalizer(const Array &p_data) {
	if (p_data.size() != 2) {
		return p_data;
	}
	Array attr_configs = p_data[0];
	Dictionary state_copy = p_data[1];
	Array result;
	size_t attr_num = attr_configs.size();
	result.resize(attr_num * 2);
	for (size_t index = 0; index < attr_num; index++) {
		const Array &item = attr_configs[index];
		const StringName &item_key = item[0].operator StringName();
		const String item_value_key = format_value_key(InternalDataSourceType::SYNC_ATTR_MANAGER, SYNC_ATTR_MANAGER_ATTR_MAP_PREFIX + item_key);
		result[index] = build_data_item(item_value_key, item[1]);
		const String value_key = format_value_key(InternalDataSourceType::EXTENSIBLE_STATE, item_key);
		const Callable &value_getter = item[2];
		if (value_getter.is_valid()) {
			const Variant &value = value_getter.call(item_key, state_copy);
			result[attr_num + index] = build_data_item(value_key, value);
		} else {
			result[attr_num + index] = build_data_item(value_key, state_copy.get(item_key, Variant()));
		}
	}
	return result;
}

Callable ZyiSynchronizerCharacterStateField::get_threading_data_list_normalizer() {
	return callable_mp_static(&ZyiSynchronizerCharacterStateField::threading_data_list_normalizer);
}

Variant ZyiSynchronizerCharacterStateField::get_prepare_data(Node *p_controller_node, const Variant &p_cache_data, bool p_is_update) {
	if (p_is_update) {
		return true;
	}
	if (!is_valid_unit(p_controller_node)) {
		return Variant();
	}
	ZyiUtilAttrManager *attr_manager = get_sync_attr_manager(p_controller_node);
	if (attr_manager == nullptr) {
		return Variant();
	}
	ObjectID attr_manager_id = attr_manager->get_instance_id();
	if (_cache_attr_manager_id != attr_manager_id) {
		// 新创建 _cache_attr_configs
		_cache_attr_manager_id = attr_manager_id;
		_cache_attr_configs.resize(attr_manager->attr_map.size());
		size_t index = 0;
		for (const KeyValue<StringName, ZyiUtilAttrManager::AttrConfig> &item : attr_manager->attr_map) {
			if (attr_manager->force_exclude_attr_set.has(item.key)) {
				continue;
			}
			const ZyiUtilAttrManager::AttrConfig &item_cfg = item.value;
			Array args = { item_cfg.getter, item_cfg.setter };
			_cache_attr_configs[index] = { item.key, p_controller_node->callv(METHOD_FORMAT_ATTR_HANDLERS, args), item_cfg.getter };
			index += 1;
		}
		attr_manager->clear_cached_new_attr();
	} else {
		// 增量更新 _cache_attr_configs
		const HashSet<StringName> &cached_new_attr_set = attr_manager->get_cached_new_attr_set();
		size_t index = _cache_attr_configs.size();
		_cache_attr_configs.resize(index + cached_new_attr_set.size());
		for (const StringName &item : cached_new_attr_set) {
			if (attr_manager->force_exclude_attr_set.has(item)) {
				continue;
			}
			const ZyiUtilAttrManager::AttrConfig &item_cfg = attr_manager->attr_map[item];
			Array args = { item_cfg.getter, item_cfg.setter };
			_cache_attr_configs[index] = { item, p_controller_node->callv(METHOD_FORMAT_ATTR_HANDLERS, args), item_cfg.getter };
			index += 1;
		}
		attr_manager->clear_cached_new_attr();
	}
	Array result = {
		_cache_attr_configs, p_controller_node->callv(METHOD_GET_SYNC_STATE_DICT, Array())
	};
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

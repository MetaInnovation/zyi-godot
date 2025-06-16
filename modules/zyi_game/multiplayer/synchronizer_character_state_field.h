#ifndef SYNCHRONIZER_CHARACTER_STATE_FIELD_H
#define SYNCHRONIZER_CHARACTER_STATE_FIELD_H

#include "../util/util_attr_manager.h"
#include "../util/util_object_helper.h"
#include "synchronizer_data_field.h"

class ZyiSynchronizerCharacterStateField : public ZyiSynchronizerDataField {
	GDCLASS(ZyiSynchronizerCharacterStateField, ZyiSynchronizerDataField)

public:
	struct InternalKeyContext {
		int8_t type;
		String key;
	};

	static constexpr const char *VALUE_KEY_SEP = ".";
	static constexpr const int8_t VALUE_KEY_SEP_SIZE = 1;
	static constexpr const char *SYNC_ATTR_MANAGER_ATTR_MAP_PREFIX = ":";
	static constexpr const int8_t SYNC_ATTR_MANAGER_ATTR_MAP_PREFIX_SIZE = 1;
	static constexpr const bool SYNC_STATUS_SET = false;
	static constexpr const bool SYNC_FORCE_EXCLUDE_SYNC_ATTR = false;
	static constexpr const char *METHOD_IS_VALID = "sync_is_valid_field_controller_node";
	static constexpr const char *METHOD_GET_SYNC_ATTR_MANAGER = "sync_get_attr_manager";
	static constexpr const char *METHOD_FORMAT_ATTR_HANDLERS = "sync_format_attr_handlers";
	static constexpr const char *METHOD_PARSE_ATTR_HANDLERS = "sync_parse_attr_handlers";
	enum InternalDataSourceType {
		STATUS_SET,
		SYNC_ATTR_MANAGER,
		EXTENSIBLE_STATE
	};

	_FORCE_INLINE_ String format_value_key(int8_t p_type, const String &p_key) {
		return String::num_int64(p_type) + VALUE_KEY_SEP + p_key;
	}
	_FORCE_INLINE_ InternalKeyContext parse_value_key(const String &p_value_key) {
		int index = p_value_key.find(VALUE_KEY_SEP);
		return InternalKeyContext{ static_cast<int8_t>((p_value_key.substr(0, index)).to_int()),
			p_value_key.substr(index + VALUE_KEY_SEP_SIZE) };
	}
	_FORCE_INLINE_ bool is_valid_unit(Node *p_controller_node) {
		if (p_controller_node == nullptr || p_controller_node->is_queued_for_deletion()) {
			return false;
		}
		if (p_controller_node->has_method(METHOD_IS_VALID)) {
			return call_method_on_controller_node(p_controller_node, METHOD_IS_VALID);
		}
		return true;
	}
	_FORCE_INLINE_ Variant call_method_on_controller_node(Node *p_controller_node, const StringName &p_method) {
		Callable::CallError ce;
		Variant ret = p_controller_node->callp(p_method, nullptr, 0, ce);
		if (ce.error != Callable::CallError::CALL_OK) {
			ERR_FAIL_V_MSG(Variant(), vformat("Error calling method from ZyiSynchronizerCharacterStateField %s: %s.", p_method, Variant::get_call_error_text(this, p_method, nullptr, 0, ce)));
		}
		return ret;
	}
	_FORCE_INLINE_ ZyiUtilAttrManager *get_sync_attr_manager(Node *p_controller_node) {
		ZyiUtilAttrManager *attr_manager = nullptr;
		const Variant sync_attr_manager = call_method_on_controller_node(p_controller_node, METHOD_GET_SYNC_ATTR_MANAGER);
		if (sync_attr_manager.is_null() || sync_attr_manager.get_type() != Variant::OBJECT) {
			return attr_manager;
		}
		attr_manager = Object::cast_to<ZyiUtilAttrManager>(sync_attr_manager);
		return attr_manager;
	}

	Variant get_prepare_data(Node *p_controller_node, const Variant &p_cache_data = Variant(), bool p_is_update = false);
	void update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action = ACTION_CHANGE);
};

#endif /* SYNCHRONIZER_CHARACTER_STATE_FIELD_H */

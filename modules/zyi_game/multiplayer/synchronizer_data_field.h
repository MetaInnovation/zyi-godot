#ifndef SYNCHRONIZER_DATA_FIELD_H
#define SYNCHRONIZER_DATA_FIELD_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "scene/main/node.h"

class ZyiSynchronizerDataField : public RefCounted {
	GDCLASS(ZyiSynchronizerDataField, RefCounted)

protected:
	static void _bind_methods();

public:
	enum ActionType {
		ACTION_CHANGE,
		ACTION_ADD,
		ACTION_REMOVE
	};

	_FORCE_INLINE_ Array build_data_item(const String &p_value_key, const Variant &p_value) {
		Array item;
		item.resize(2);
		item[0] = p_value_key;
		item[1] = p_value;
		return item;
	}

	bool is_data_value_changed(const String &p_value_key, const Variant &p_value, const Variant &p_other) const;
	Variant get_cache_data(Node *p_controller_node);
	Variant get_prepare_data(Node *p_controller_node, const Variant &p_cache_data = Variant(), bool p_is_update = false);
	Array get_data_list(const Variant &p_data);
	void update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action = ACTION_CHANGE);
};

#endif /* SYNCHRONIZER_DATA_FIELD_H */

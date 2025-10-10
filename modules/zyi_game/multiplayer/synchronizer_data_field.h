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
	enum ActionType : int8_t {
		ACTION_CHANGE,
		ACTION_ADD
	};

	_FORCE_INLINE_ static Array build_data_item(const String &p_value_key, const Variant &p_value) {
		return { p_value_key, p_value };
	}

	virtual Variant get_cache_data(Node *p_controller_node);
	GDVIRTUAL1RC(Variant, _get_cache_data, Node *);
	virtual Variant get_prepare_data(Node *p_controller_node, const Variant &p_cache_data = Variant(), bool p_is_update = false);
	GDVIRTUAL3RC(Variant, _get_prepare_data, Node *, const Variant &, bool);
	virtual Array get_data_list(const Variant &p_data);
	virtual Callable get_threading_data_list_normalizer();
	virtual void update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action = ACTION_CHANGE);
	GDVIRTUAL5C(_update_data, const Variant &, const Variant &, const String &, const Variant &, int8_t);
};

#endif /* SYNCHRONIZER_DATA_FIELD_H */

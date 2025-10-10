#ifndef UTIL_ATTR_MANAGER_H
#define UTIL_ATTR_MANAGER_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/templates/hash_map.h"
#include "core/templates/hash_set.h"

class ZyiUtilAttrManager : public RefCounted {
	GDCLASS(ZyiUtilAttrManager, RefCounted)

protected:
	static void _bind_methods();

public:
	struct AttrConfig {
		Callable getter;
		Callable setter;
	};
	HashMap<StringName, AttrConfig> attr_map;
	HashSet<StringName> cached_new_attr_set;
	HashSet<StringName> force_exclude_attr_set;
	Callable _getter;
	Callable _setter;

	static Ref<ZyiUtilAttrManager> create(const Callable &p_getter, const Callable &p_setter);

	HashSet<StringName> get_cached_new_attr_set();
	void clear_cached_new_attr();

	Array get_attr_list() const;
	bool has_attr(const String &p_key) const;
	Variant get_v(const String &p_key);
	void set_v(const String &p_key, const Variant &p_value);
	void force_exclude(const String &p_key);
	void add_attr(const String &p_key, bool override = false, const Variant &p_getter = Variant(), const Variant &p_setter = Variant());
	void clear();
};

#endif /* UTIL_ATTR_MANAGER_H */

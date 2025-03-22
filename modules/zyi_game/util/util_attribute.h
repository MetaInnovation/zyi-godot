#ifndef UTIL_ATTRIBUTE_H
#define UTIL_ATTRIBUTE_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/string/string_name.h"
#include "core/string/ustring.h"
#include "core/variant/array.h"
#include "core/variant/dictionary.h"
#include "core/variant/variant.h"

class ZyiUtilAttribute : public RefCounted {
	GDCLASS(ZyiUtilAttribute, RefCounted);

protected:
	static void _bind_methods();

public:
	Dictionary data;
	Dictionary get_data();
	void set_data(const Dictionary &p_data);
	Array keys() const;
	Variant get_v(const String &p_key, const Variant &p_default = Variant());
	Ref<ZyiUtilAttribute> duplicate(bool p_deep = false) const;
	const Dictionary &to_json() const;
	bool set_v(const String &p_key, const Variant &p_value, const bool disable_override = false);
	bool remove_v(const String &p_key);
	void from_json(const Dictionary &p_data);
	void clear();
	void merge_from_other(const Ref<ZyiUtilAttribute> &p_other);
	void merge_from_json(const Dictionary &p_data);

	// 默认构造函数 + 数据构造函数
	ZyiUtilAttribute(const Dictionary &p_data);
	ZyiUtilAttribute() = default;
	~ZyiUtilAttribute() = default;
};

#endif /* UTIL_ATTRIBUTE_H */

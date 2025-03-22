#ifndef DAMAGE_EXTRA_META_RES_H
#define DAMAGE_EXTRA_META_RES_H

#include "core/io/resource.h"
#include "core/object/class_db.h"

class ZyiDamageExtraMetaResource : public Resource {
	GDCLASS(ZyiDamageExtraMetaResource, Resource);

protected:
	static void _bind_methods();

public:
	// 额外元数据类型
	String type = "";
	// 额外元数据来源
	String source = "";
	// 额外元数据统计类型
	String statistics = "";
	String get_type() const;
	void set_type(const String &p_type);
	String get_source() const;
	void set_source(const String &p_source);
	String get_statistics() const;
	void set_statistics(const String &p_statistics);
};

#endif /* DAMAGE_EXTRA_META_RES_H */

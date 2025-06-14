#ifndef SYNC_HELPER_H
#define SYNC_HELPER_H

#include "core/io/resource.h"
#include "core/io/resource_loader.h"
#include "core/object/class_db.h"
#include "core/object/ref_counted.h"

class ZyiSyncHelper : public RefCounted {
	GDCLASS(ZyiSyncHelper, RefCounted)

protected:
	static void _bind_methods();

public:
	static Variant format_variant(const Variant &p_value);
	static Variant parse_variant(const Variant &p_value);
};

#endif /* SYNC_HELPER_H */

#ifndef UTIL_SET_H
#define UTIL_SET_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/templates/hash_set.h"
#include "core/variant/array.h"
#include "core/variant/variant.h"

class ZyiUtilSet : public RefCounted {
	GDCLASS(ZyiUtilSet, RefCounted);

private:
	HashSet<Variant, VariantHasher, StringLikeVariantComparator> _variant_set;

protected:
	static void _bind_methods();

public:
	static Ref<ZyiUtilSet> create_from_array(const Array &value);

	void add(const Variant &value);
	void add_array(const Array &value);
	void extend_from(const Ref<ZyiUtilSet> &value);
	void remove(const Variant &value);
	int64_t size();
	bool is_empty();
	bool has(const Variant &value);
	void clear();
	Ref<ZyiUtilSet> duplicate();
	Array keys();
};

#endif /* UTIL_SET_H */

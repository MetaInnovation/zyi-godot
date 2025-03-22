#include "util_object_helper.h"
#include "core/variant/variant_utility.h"

void ZyiUtilObjectHelper::_bind_methods() {
	ClassDB::bind_static_method("ZyiUtilObjectHelper", D_METHOD("check_is_valid_object", "value"), &ZyiUtilObjectHelper::check_is_valid_object);
	ClassDB::bind_static_method("ZyiUtilObjectHelper", D_METHOD("safe_check_is_object", "value"), &ZyiUtilObjectHelper::safe_check_is_object);
	ClassDB::bind_static_method("ZyiUtilObjectHelper", D_METHOD("check_is_invalid_object", "value"), &ZyiUtilObjectHelper::check_is_invalid_object);
}

bool ZyiUtilObjectHelper::check_is_valid_object(const Variant &value) {
	return VariantUtilityFunctions::is_instance_valid(value) && !Object::cast_to<Object>(value)->is_queued_for_deletion();
}

bool ZyiUtilObjectHelper::safe_check_is_object(const Variant &value) {
	return value.get_type() == Variant::OBJECT;
}

bool ZyiUtilObjectHelper::check_is_invalid_object(const Variant &value) {
	return value.get_type() == Variant::NIL || (value.get_type() == Variant::OBJECT && !check_is_valid_object(value));
}

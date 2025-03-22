#include "damage_extra_meta_res.h"

void ZyiDamageExtraMetaResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_type"), &ZyiDamageExtraMetaResource::get_type);
	ClassDB::bind_method(D_METHOD("set_type", "type"), &ZyiDamageExtraMetaResource::set_type);
	ClassDB::bind_method(D_METHOD("get_source"), &ZyiDamageExtraMetaResource::get_source);
	ClassDB::bind_method(D_METHOD("set_source", "source"), &ZyiDamageExtraMetaResource::set_source);
	ClassDB::bind_method(D_METHOD("get_statistics"), &ZyiDamageExtraMetaResource::get_statistics);
	ClassDB::bind_method(D_METHOD("set_statistics", "statistics"), &ZyiDamageExtraMetaResource::set_statistics);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "type"), "set_type", "get_type");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "source"), "set_source", "get_source");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "statistics"), "set_statistics", "get_statistics");
}

String ZyiDamageExtraMetaResource::get_type() const {
	return type;
}

void ZyiDamageExtraMetaResource::set_type(const String &p_type) {
	type = p_type;
}

String ZyiDamageExtraMetaResource::get_source() const {
	return source;
}

void ZyiDamageExtraMetaResource::set_source(const String &p_source) {
	source = p_source;
}

String ZyiDamageExtraMetaResource::get_statistics() const {
	return statistics;
}

void ZyiDamageExtraMetaResource::set_statistics(const String &p_statistics) {
	statistics = p_statistics;
}

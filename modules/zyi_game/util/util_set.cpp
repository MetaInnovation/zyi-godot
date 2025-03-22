#include "util_set.h"

void ZyiUtilSet::_bind_methods() {
	ClassDB::bind_static_method("ZyiUtilSet", D_METHOD("create_from_array", "value"), &ZyiUtilSet::create_from_array);

	ClassDB::bind_method(D_METHOD("add", "value"), &ZyiUtilSet::add);
	ClassDB::bind_method(D_METHOD("add_array", "value"), &ZyiUtilSet::add_array);
	ClassDB::bind_method(D_METHOD("extend_from", "value"), &ZyiUtilSet::extend_from);
	ClassDB::bind_method(D_METHOD("remove", "value"), &ZyiUtilSet::remove);
	ClassDB::bind_method(D_METHOD("size"), &ZyiUtilSet::size);
	ClassDB::bind_method(D_METHOD("is_empty"), &ZyiUtilSet::is_empty);
	ClassDB::bind_method(D_METHOD("has", "value"), &ZyiUtilSet::has);
	ClassDB::bind_method(D_METHOD("clear"), &ZyiUtilSet::clear);
	ClassDB::bind_method(D_METHOD("duplicate"), &ZyiUtilSet::duplicate);
	ClassDB::bind_method(D_METHOD("keys"), &ZyiUtilSet::keys);
}

Ref<ZyiUtilSet> ZyiUtilSet::create_from_array(const Array &value) {
	Ref<ZyiUtilSet> result = memnew(ZyiUtilSet());
	result->_variant_set.reserve(value.size());
	for (const Variant &item : value) {
		result->_variant_set.insert(item);
	}
	return result;
}

void ZyiUtilSet::add(const Variant &value) {
	_variant_set.insert(value);
}

void ZyiUtilSet::add_array(const Array &value) {
	uint32_t new_size = value.size() + _variant_set.size();
	if (_variant_set.get_capacity() < new_size) {
		_variant_set.reserve(new_size);
	}
	for (const Variant &item : value) {
		_variant_set.insert(item);
	}
}

void ZyiUtilSet::extend_from(const Ref<ZyiUtilSet> &value) {
	uint32_t new_size = value->_variant_set.size() + _variant_set.size();
	if (_variant_set.get_capacity() < new_size) {
		_variant_set.reserve(new_size);
	}
	for (auto item = value->_variant_set.begin(); item != value->_variant_set.end(); ++item) {
		_variant_set.insert(*item);
	}
}

void ZyiUtilSet::remove(const Variant &value) {
	_variant_set.erase(value);
}

int64_t ZyiUtilSet::size() {
	return _variant_set.size();
}

bool ZyiUtilSet::is_empty() {
	return _variant_set.is_empty();
}

bool ZyiUtilSet::has(const Variant &value) {
	return _variant_set.has(value);
}

void ZyiUtilSet::clear() {
	_variant_set.clear();
}

Ref<ZyiUtilSet> ZyiUtilSet::duplicate() {
	return Ref<ZyiUtilSet>();
}

Array ZyiUtilSet::keys() {
	Array result;
	result.resize(_variant_set.size());
	int64_t index = 0;
	for (auto item = _variant_set.begin(); item != _variant_set.end(); ++item) {
		result[index++] = *item;
	}
	return result;
}

#include "sync_helper.h"

void ZyiSyncHelper::_bind_methods() {
	ClassDB::bind_static_method("ZyiSyncHelper", D_METHOD("format_variant", "value"), &ZyiSyncHelper::format_variant);
	ClassDB::bind_static_method("ZyiSyncHelper", D_METHOD("parse_variant", "value"), &ZyiSyncHelper::parse_variant);
}

Variant ZyiSyncHelper::format_variant(const Variant &p_value) {
	Array result;
	result.resize(2);
	switch (p_value.get_type()) {
		case Variant::VECTOR2: {
			result[0] = "Vector2";
			Vector2 val = p_value;
			Array arr;
			arr.resize(2);
			arr[0] = val.x;
			arr[1] = val.y;
			result[1] = arr;
		} break;
		case Variant::VECTOR2I: {
			result[0] = "Vector2i";
			Vector2i val = p_value;
			Array arr;
			arr.resize(2);
			arr[0] = val.x;
			arr[1] = val.y;
			result[1] = arr;
		} break;
		case Variant::RECT2: {
			result[0] = "Rect2";
			Rect2 val = p_value;
			Array arr;
			arr.resize(4);
			arr[0] = val.position.x;
			arr[1] = val.position.y;
			arr[2] = val.size.x;
			arr[3] = val.size.y;
			result[1] = arr;
		} break;
		case Variant::RECT2I: {
			result[0] = "Rect2i";
			Rect2i val = p_value;
			Array arr;
			arr.resize(4);
			arr[0] = val.position.x;
			arr[1] = val.position.y;
			arr[2] = val.size.x;
			arr[3] = val.size.y;
			result[1] = arr;
		} break;
		case Variant::COLOR: {
			result[0] = "Color";
			result[1] = p_value.operator Color().to_html();
		} break;
		case Variant::OBJECT: {
			Object *obj = p_value;
			if (obj != nullptr && obj->is_class("Resource")) {
				result[0] = "Resource";
				result[1] = Object::cast_to<Resource>(obj)->get_path();
			} else {
				result[0] = "";
				result[1] = p_value;
			}
		} break;
		default: {
			result[0] = "";
			result[1] = p_value;
		} break;
	}
	return result;
}

Variant ZyiSyncHelper::parse_variant(const Variant &p_value) {
	if (!p_value.is_array()) {
		return Variant();
	}
	Array arr = p_value;
	String data_type = arr[0];
	Variant data_value = arr[1];
	if (data_type == "Vector2") {
		Array val_arr = data_value;
		return Vector2(val_arr[0], val_arr[1]);
	} else if (data_type == "Vector2i") {
		Array val_arr = data_value;
		return Vector2i(val_arr[0], val_arr[1]);
	} else if (data_type == "Rect2") {
		Array val_arr = data_value;
		return Rect2(val_arr[0], val_arr[1], val_arr[2], val_arr[3]);
	} else if (data_type == "Rect2i") {
		Array val_arr = data_value;
		return Rect2i(val_arr[0], val_arr[1], val_arr[2], val_arr[3]);
	} else if (data_type == "Color") {
		String val_str = data_value;
		return Color::html("#" + val_str);
	} else if (data_type == "Resource") {
		return ResourceLoader::load(data_value);
	}
	return data_value;
}

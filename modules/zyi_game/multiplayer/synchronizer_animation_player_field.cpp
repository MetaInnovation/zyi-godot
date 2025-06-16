#include "synchronizer_animation_player_field.h"

void ZyiSynchronizerAnimationPlayerField::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_animation_player_path_list"), &ZyiSynchronizerAnimationPlayerField::get_animation_player_path_list);
	ClassDB::bind_method(D_METHOD("set_animation_player_path_list", "animation_player_path_list"), &ZyiSynchronizerAnimationPlayerField::set_animation_player_path_list);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "animation_player_path_list", PROPERTY_HINT_TYPE_STRING, "NodePath"), "set_animation_player_path_list", "get_animation_player_path_list");
}

TypedArray<NodePath> ZyiSynchronizerAnimationPlayerField::get_animation_player_path_list() {
	return animation_player_path_list;
}

void ZyiSynchronizerAnimationPlayerField::set_animation_player_path_list(const TypedArray<NodePath> &p_animation_player_path_list) {
	animation_player_path_list = p_animation_player_path_list;
}

Variant ZyiSynchronizerAnimationPlayerField::get_cache_data(Node *p_controller_node) {
	if (p_controller_node == nullptr || !p_controller_node->is_inside_tree()) {
		return Variant();
	}
	Array result;
	result.resize(animation_player_path_list.size());
	for (int i = 0; i < animation_player_path_list.size(); i++) {
		NodePath animation_player_path = animation_player_path_list[i];
		Node *node = nullptr;
		if (animation_player_path.is_absolute()) {
			node = p_controller_node->get_node_or_null(animation_player_path);
		} else {
			node = p_controller_node->get_node_or_null(NodePath(String(p_controller_node->get_path()) + "/" + String(animation_player_path)));
		}
		AnimationPlayer *animation_player = Object::cast_to<AnimationPlayer>(node);
		if (animation_player != nullptr) {
			result[i] = animation_player->get_instance_id();
		} else {
			result[i] = ObjectID();
		}
	}
	return result;
}

Variant ZyiSynchronizerAnimationPlayerField::get_prepare_data(Node *p_controller_node, const Variant &p_cache_data, bool p_is_update) {
	if (p_controller_node == nullptr || !p_controller_node->is_inside_tree()) {
		return Variant();
	}
	TypedArray<AnimationPlayer> node_list;
	if (p_cache_data.is_array()) {
		Array arr = p_cache_data;
		node_list.resize(arr.size());
		for (int i = 0; i < arr.size(); i++) {
			Object *obj = ObjectDB::get_instance(arr[i]);
			if (obj != nullptr) {
				AnimationPlayer *node = Object::cast_to<AnimationPlayer>(obj);
				node_list[i] = node;
			} else {
				node_list[i] = Variant();
			}
		}
	}
	if (p_is_update) {
		return node_list;
	}
	if (node_list.is_empty()) {
		return node_list;
	}
	Array result;
	for (int i = 0; i < node_list.size(); i++) {
		AnimationPlayer *node = Object::cast_to<AnimationPlayer>(node_list[i]);
		if (node != nullptr) {
			result.push_back(build_data_item(String::num_int64(i), node->get_current_animation()));
		}
	}
	return result;
}

void ZyiSynchronizerAnimationPlayerField::update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action) {
	if (!p_data.is_array()) {
		return;
	}
	int64_t index = p_value_key.to_int();
	Array arr = p_data;
	if (index < 0 || index >= arr.size()) {
		return;
	}
	AnimationPlayer *node = Object::cast_to<AnimationPlayer>(arr[index]);
	if (node == nullptr) {
		return;
	}
	switch (p_action) {
		case ActionType::ACTION_CHANGE:
		case ActionType::ACTION_ADD: {
			node->stop();
			node->play(ANIM_RESET);
			if (p_value != ANIM_RESET) {
				node->play(p_value);
			}
		} break;
		case ActionType::ACTION_REMOVE:
			node->stop();
			node->play(ANIM_RESET);
			break;
		default:
			break;
	}
}

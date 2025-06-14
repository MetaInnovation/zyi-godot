#ifndef SYNCHRONIZER_ANIMATION_PLAYER_FIELD_H
#define SYNCHRONIZER_ANIMATION_PLAYER_FIELD_H

#include "scene/animation/animation_player.h"
#include "synchronizer_data_field.h"

class ZyiSynchronizerAnimationPlayerField : public ZyiSynchronizerDataField {
	GDCLASS(ZyiSynchronizerAnimationPlayerField, ZyiSynchronizerDataField)

protected:
	static void _bind_methods();

public:
	static constexpr const char *ANIM_RESET = "RESET";

	TypedArray<NodePath> animation_player_path_list;
	TypedArray<NodePath> get_animation_player_path_list();
	void set_animation_player_path_list(const TypedArray<NodePath> &p_animation_player_path_list);

	Variant get_cache_data(Node *p_controller_node);
	Variant get_prepare_data(Node *p_controller_node, const Variant &p_cache_data = Variant(), bool p_is_update = false);
	void update_data(const Variant &p_controller_node, const Variant &p_data, const String &p_value_key, const Variant &p_value, int8_t p_action = ACTION_CHANGE);
};

#endif /* SYNCHRONIZER_ANIMATION_PLAYER_FIELD_H */

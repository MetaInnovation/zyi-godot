#ifndef FLOATING_TEXT_SYSTEM_CANVAS_H
#define FLOATING_TEXT_SYSTEM_CANVAS_H

#include "core/os/time.h"
#include "core/templates/local_vector.h"
#include "core/variant/variant_utility.h"
#include "scene/2d/node_2d.h"
#include "scene/resources/atlas_texture.h"

#include <vector>

class ZyiFloatingTextSystemCanvas : public Node2D {
	GDCLASS(ZyiFloatingTextSystemCanvas, Node2D)

	struct InternalDamageTextItem {
		String value;
		uint64_t start_ticks_msec;
		Vector2 base_pos;
		Color color;
		Vector2 top_pos;
	};
	std::vector<InternalDamageTextItem> _damage_text_list;
	Ref<AtlasTexture> _atlas_texture;
	HashMap<double, Rect2> _scale_cache;

protected:
	static void _bind_methods();
	void _notification(int p_notification);

public:
	static const double MAX_LIVE_TIME;
	enum {
		DAMAGE_TEXT_INIT_CAPACITY = 256,
	};

	int64_t max_damage_text_count = -1;
	int64_t get_max_damage_text_count() const { return max_damage_text_count; }
	void set_max_damage_text_count(int64_t p_limit) { max_damage_text_count = p_limit; }
	Ref<Texture2D> digit_texture;
	Ref<Texture2D> get_digit_texture() const { return digit_texture; }
	void set_digit_texture(Ref<Texture2D> p_texture) { digit_texture = p_texture; }
	Vector2 digit_size = Vector2(20, 30);
	Vector2 get_digit_size() const { return digit_size; }
	void set_digit_size(const Vector2 &p_digit_size) { digit_size = p_digit_size; }
	int64_t atlas_cols = 5;
	int64_t get_atlas_cols() const { return atlas_cols; }
	void set_atlas_cols(int64_t p_cols) { atlas_cols = p_cols; }

	_ALWAYS_INLINE_ static double ease_out_quad(double p_t) {
		return 1.0 - (1.0 - p_t) * (1.0 - p_t);
	}
	_ALWAYS_INLINE_ static double ease_out_cubic(double p_t) {
		return 1.0 - Math::pow(1.0 - p_t, 3.0);
	}
	void draw_all();
	void show_damage_text(String p_value, const Vector2 &p_pos, Color p_color);
	Rect2 get_scaled_rect(double p_scale);
	void draw_scaled_damage_text_optimized(String p_text, const Vector2 &p_center, double p_scale, Color p_color);
};

#endif /* FLOATING_TEXT_SYSTEM_CANVAS_H */

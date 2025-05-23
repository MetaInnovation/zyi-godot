#include "floating_text_system_canvas.h"

const double ZyiFloatingTextSystemCanvas::MAX_LIVE_TIME = 400;

void ZyiFloatingTextSystemCanvas::_bind_methods() {
	ClassDB::bind_method(D_METHOD("show_damage_text", "value", "pos", "color"), &ZyiFloatingTextSystemCanvas::show_damage_text);
	ClassDB::bind_method(D_METHOD("get_max_damage_text_count"), &ZyiFloatingTextSystemCanvas::get_max_damage_text_count);
	ClassDB::bind_method(D_METHOD("set_max_damage_text_count", "value"), &ZyiFloatingTextSystemCanvas::set_max_damage_text_count);
	ClassDB::bind_method(D_METHOD("get_digit_texture"), &ZyiFloatingTextSystemCanvas::get_digit_texture);
	ClassDB::bind_method(D_METHOD("set_digit_texture", "value"), &ZyiFloatingTextSystemCanvas::set_digit_texture);
	ClassDB::bind_method(D_METHOD("get_digit_size"), &ZyiFloatingTextSystemCanvas::get_digit_size);
	ClassDB::bind_method(D_METHOD("set_digit_size", "value"), &ZyiFloatingTextSystemCanvas::set_digit_size);
	ClassDB::bind_method(D_METHOD("get_atlas_cols"), &ZyiFloatingTextSystemCanvas::get_atlas_cols);
	ClassDB::bind_method(D_METHOD("set_atlas_cols", "value"), &ZyiFloatingTextSystemCanvas::set_atlas_cols);
	ClassDB::bind_method(D_METHOD("draw_all"), &ZyiFloatingTextSystemCanvas::draw_all);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_damage_text_count"), "set_max_damage_text_count", "get_max_damage_text_count");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "digit_texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_digit_texture", "get_digit_texture");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "digit_size"), "set_digit_size", "get_digit_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "atlas_cols"), "set_atlas_cols", "get_atlas_cols");
}

void ZyiFloatingTextSystemCanvas::_notification(int p_notification) {
#ifdef TOOLS_ENABLED
	if (is_part_of_edited_scene()) {
		return;
	}
#endif
	if (digit_texture.is_null()) {
		return;
	}
	switch (p_notification) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			// _process
			queue_redraw();
		} break;
		case NOTIFICATION_READY: {
			// _ready
			_damage_text_list.reserve(DAMAGE_TEXT_INIT_CAPACITY);
			_atlas_texture = memnew(AtlasTexture());
			_atlas_texture->set_atlas(digit_texture);
			_atlas_texture->set_region(Rect2(0, 0, digit_texture->get_width(), digit_texture->get_height()));
			set_process_internal(true);
		} break;
		case NOTIFICATION_DRAW: {
			// _draw
			draw_all();
		} break;
	}
}

void ZyiFloatingTextSystemCanvas::draw_all() {
	uint64_t current_time = Time::get_singleton()->get_ticks_msec();
	LocalVector<size_t> to_remove_index_list;
	for (size_t i = 0; i < _damage_text_list.size(); i++) {
		InternalDamageTextItem &item = _damage_text_list[i];
		uint64_t elapsed = current_time - item.start_ticks_msec;
		if (elapsed >= MAX_LIVE_TIME || elapsed < 0) {
			to_remove_index_list.push_back(i);
		} else {
			double scale_v = 1.0;
			Vector2 pos = item.base_pos;
			double alpha = 1.0;
			if (elapsed < 200.0) {
				double progress = elapsed / 200.0;
				pos.y -= ease_out_quad(progress) * 80;
				// 放大1.3倍
				scale_v = 1.0 + ease_out_quad(progress) * 0.3;
				item.top_pos = pos;
			} else if (elapsed < 300.0) {
				pos = item.top_pos;
				scale_v = 1.1;
			} else {
				double fade_progress = (elapsed - 300.0) / (MAX_LIVE_TIME - 300.0);
				alpha = 1.0 - ease_out_cubic(fade_progress);
				// pos.y = item.top_pos.y - ease_out_cubic(fade_progress) * 70;
				// scale从1.1倍开始缩小
				scale_v = 1.1 - ease_out_quad(fade_progress) * 0.2;
			}
			draw_scaled_damage_text_optimized(item.value, pos, scale_v, Color(item.color, alpha));
		}
	}
	// 移除过期数字
	if (!to_remove_index_list.is_empty()) {
		for (int64_t i = to_remove_index_list.size() - 1; i >= 0; i--) {
			_damage_text_list.erase(_damage_text_list.begin() + to_remove_index_list[i]);
		}
	}
}

void ZyiFloatingTextSystemCanvas::show_damage_text(String p_value, const Vector2 &p_pos, Color p_color) {
	if (max_damage_text_count >= 0 && _damage_text_list.size() >= static_cast<size_t>(max_damage_text_count)) {
		return;
	}
	uint64_t start_ticks_msec = Time::get_singleton()->get_ticks_msec();
	_damage_text_list.emplace_back();
	InternalDamageTextItem &item = _damage_text_list.back();
	item.value = p_value;
	item.start_ticks_msec = start_ticks_msec;
	item.base_pos = p_pos;
	item.color = p_color;
	item.top_pos = Vector2();
	queue_redraw();
}

Rect2 ZyiFloatingTextSystemCanvas::get_scaled_rect(double p_scale) {
	if (!_scale_cache.has(p_scale)) {
		double w = digit_size.x * p_scale;
		double h = digit_size.y * p_scale;
		_scale_cache[p_scale] = Rect2(-w / 2, -h / 2, w, h);
	}
	return _scale_cache[p_scale];
}

void ZyiFloatingTextSystemCanvas::draw_scaled_damage_text_optimized(String p_text, const Vector2 &p_center, double p_scale, Color p_color) {
	int len = p_text.length();
	double total_spacing = digit_size.x * p_scale * len;
	double start_x = p_center.x - total_spacing / 2.0;
	Rect2 rect_template = get_scaled_rect(p_scale);
	for (int i = 0; i < len; i++) {
		int digit = String::to_int(&p_text[i], 1);
		int col = digit % atlas_cols;
		int row = digit / atlas_cols;
		Vector2 pos = Vector2(start_x + i * digit_size.x * p_scale, p_center.y);
		// 设置变换
		draw_set_transform(pos, 0.0, Vector2(1, 1) * p_scale);
		// 绘制
		draw_texture_rect_region(
				_atlas_texture,
				rect_template,
				Rect2(col * digit_size.x, row * digit_size.y, digit_size.x, digit_size.y),
				p_color);
	}
	// 重置变换
	draw_set_transform(Vector2());
}

#ifndef DAMAGE_CONSTANT_H
#define DAMAGE_CONSTANT_H

#include "core/math/color.h"
#include "core/object/class_db.h"
#include "core/object/object.h"

class ZyiDamageConstant : public Object {
	GDCLASS(ZyiDamageConstant, Object)

private:
	static const int64_t MAX_KILL_DAMAGE;
	static const double DEFAULT_CRIT_DAMAGE_RATE;
	static const String TYPE_NORMAL;
	static const String TYPE_REAL;
	static const String TYPE_CRIT;
	static const String TYPE_CRIT_REAL;
	static const String TYPE_TOTAL;
	static const String INNER_TYPE_APPEND;

protected:
	static void _bind_methods();

public:
	static int64_t get_max_kill_damage();
	static double get_default_crit_damage_rate();
	static const String &get_type_normal();
	static const String &get_type_real();
	static const String &get_type_crit();
	static const String &get_type_crit_real();
	static const String &get_type_total();
	static const String &get_inner_type_append();
};

#endif /* DAMAGE_CONSTANT_H */

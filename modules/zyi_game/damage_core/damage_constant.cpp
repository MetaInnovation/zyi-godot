#include "damage_constant.h"
#include "core/core_constants.h"
#include "core/variant/variant_utility.h"

const int64_t ZyiDamageConstant::MAX_KILL_DAMAGE = 99999999;
const double ZyiDamageConstant::DEFAULT_CRIT_DAMAGE_RATE = 2.0;
const String ZyiDamageConstant::TYPE_NORMAL = "";
const String ZyiDamageConstant::TYPE_REAL = "real";
const String ZyiDamageConstant::TYPE_CRIT = "crit";
const String ZyiDamageConstant::TYPE_CRIT_REAL = "crit_real";
const String ZyiDamageConstant::TYPE_TOTAL = "total";
const String ZyiDamageConstant::INNER_TYPE_APPEND = "_append";

void ZyiDamageConstant::_bind_methods() {
	ClassDB::bind_static_method("ZyiDamageConstant", D_METHOD("sc_MAX_KILL_DAMAGE"), &ZyiDamageConstant::get_max_kill_damage);
	ClassDB::bind_static_method("ZyiDamageConstant", D_METHOD("sc_DEFAULT_CRIT_DAMAGE_RATE"), &ZyiDamageConstant::get_default_crit_damage_rate);
	ClassDB::bind_static_method("ZyiDamageConstant", D_METHOD("sc_TYPE_NORMAL"), &ZyiDamageConstant::get_type_normal);
	ClassDB::bind_static_method("ZyiDamageConstant", D_METHOD("sc_TYPE_REAL"), &ZyiDamageConstant::get_type_real);
	ClassDB::bind_static_method("ZyiDamageConstant", D_METHOD("sc_TYPE_CRIT"), &ZyiDamageConstant::get_type_crit);
	ClassDB::bind_static_method("ZyiDamageConstant", D_METHOD("sc_TYPE_CRIT_REAL"), &ZyiDamageConstant::get_type_crit_real);
	ClassDB::bind_static_method("ZyiDamageConstant", D_METHOD("sc_TYPE_TOTAL"), &ZyiDamageConstant::get_type_total);
	ClassDB::bind_static_method("ZyiDamageConstant", D_METHOD("sc_INNER_TYPE_APPEND"), &ZyiDamageConstant::get_inner_type_append);
}

int64_t ZyiDamageConstant::get_max_kill_damage() {
	return MAX_KILL_DAMAGE;
}

double ZyiDamageConstant::get_default_crit_damage_rate() {
	return DEFAULT_CRIT_DAMAGE_RATE;
}

const String &ZyiDamageConstant::get_type_normal() {
	return TYPE_NORMAL;
}

const String &ZyiDamageConstant::get_type_real() {
	return TYPE_REAL;
}

const String &ZyiDamageConstant::get_type_crit() {
	return TYPE_CRIT;
}

const String &ZyiDamageConstant::get_type_crit_real() {
	return TYPE_CRIT_REAL;
}

const String &ZyiDamageConstant::get_type_total() {
	return TYPE_TOTAL;
}

const String &ZyiDamageConstant::get_inner_type_append() {
	return INNER_TYPE_APPEND;
}

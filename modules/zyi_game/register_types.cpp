#include "register_types.h"

#include "./damage_core/damage_attack_middleware_res.h"
#include "./damage_core/damage_attack_res.h"
#include "./damage_core/damage_constant.h"
#include "./damage_core/damage_defense_middleware_res.h"
#include "./damage_core/damage_defense_res.h"
#include "./damage_core/damage_extra_meta_res.h"
#include "./damage_core/damage_manager.h"
#include "./damage_core/damage_manager_context.h"
#include "./damage_core/damage_middleware_condition_res.h"
#include "./damage_core/damage_middleware_res.h"
#include "./damage_core/damage_value.h"
#include "./pool/game_node_pool_manager.h"
#include "./pool/node_pool.h"
#include "./pool/node_pool_helper.h"
#include "./util/util_attribute.h"
#include "./util/util_callable_helper.h"
#include "./util/util_object_helper.h"
#include "./util/util_set.h"
#include "./util/util_signal_helper.h"
#include "core/object/class_db.h"

void initialize_zyi_game_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(ZyiUtilSet);
	GDREGISTER_CLASS(ZyiDamageConstant);
	GDREGISTER_CLASS(ZyiUtilObjectHelper);
	GDREGISTER_CLASS(ZyiUtilCallableHelper);
	GDREGISTER_CLASS(ZyiUtilSignalHelper);
	GDREGISTER_CLASS(ZyiUtilAttribute);
	GDREGISTER_CLASS(ZyiNodePool);
	GDREGISTER_CLASS(ZyiGameNodePoolManager);
	GDREGISTER_CLASS(ZyiNodePoolHelper);
	GDREGISTER_CLASS(ZyiDamageExtraMetaResource);
	GDREGISTER_CLASS(ZyiDamageValue);
	GDREGISTER_CLASS(ZyiDamageDefenseResource);
	GDREGISTER_CLASS(ZyiDamageAttackResource);
	GDREGISTER_CLASS(ZyiDamageManagerContext);
	GDREGISTER_CLASS(ZyiDamageMiddlewareConditionResource);
	GDREGISTER_CLASS(ZyiDamageMiddlewareResource);
	GDREGISTER_CLASS(ZyiDamageAttackMiddlewareResource);
	GDREGISTER_CLASS(ZyiDamageDefenseMiddlewareResource);
	GDREGISTER_CLASS(ZyiDamageManager);
}

void uninitialize_zyi_game_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	// Nothing to do here in this example.
}

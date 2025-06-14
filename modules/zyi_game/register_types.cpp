#include "register_types.h"

#include "core/object/class_db.h"

#include "./util/util_async.h"
#include "./util/util_attr_manager.h"
#include "./util/util_attribute.h"
#include "./util/util_callable_helper.h"
#include "./util/util_callable_object.h"
#include "./util/util_configurable_attribute_accessor.h"
#include "./util/util_emitter.h"
#include "./util/util_object_helper.h"
#include "./util/util_set.h"
#include "./util/util_signal_helper.h"

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

#include "./task/task_queue.h"

#include "./ecs/drop_system_canvas.h"
#include "./ecs/floating_text_system_canvas.h"
#include "./ecs/move_boids_grid.h"
#include "./ecs/move_constant.h"
#include "./ecs/move_proxy.h"
#include "./ecs/move_system.h"

#include "./multiplayer/sync_consumer.h"
#include "./multiplayer/sync_helper.h"
#include "./multiplayer/synchronizer_animation_player_field.h"
#include "./multiplayer/synchronizer_character_state_field.h"
#include "./multiplayer/synchronizer_data_field.h"
#include "./multiplayer/synchronizer_node_field.h"
#include "./multiplayer/synchronizer_state_task.h"

void initialize_zyi_game_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(ZyiUtilSet);
	GDREGISTER_CLASS(ZyiUtilEmitter);
	GDREGISTER_CLASS(ZyiUtilCallableObject);
	GDREGISTER_CLASS(ZyiUtilObjectHelper);
	GDREGISTER_CLASS(ZyiUtilCallableHelper);
	GDREGISTER_CLASS(ZyiUtilSignalHelper);
	GDREGISTER_CLASS(ZyiUtilAttribute);
	GDREGISTER_CLASS(ZyiUtilConfigurableAttributeAccessor);
	GDREGISTER_CLASS(ZyiUtilAsync);
	GDREGISTER_CLASS(ZyiUtilAttrManager);

	GDREGISTER_CLASS(ZyiDamageConstant);
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

	GDREGISTER_CLASS(ZyiNodePool);
	GDREGISTER_CLASS(ZyiGameNodePoolManager);
	GDREGISTER_CLASS(ZyiNodePoolHelper);

	GDREGISTER_CLASS(ZyiTaskQueue);

	GDREGISTER_CLASS(ZyiMoveConstant);
	GDREGISTER_CLASS(ZyiMoveBoidsGrid);
	GDREGISTER_CLASS(ZyiDropSystemCanvas);
	GDREGISTER_CLASS(ZyiFloatingTextSystemCanvas);
	GDREGISTER_CLASS(ZyiMoveComponentProxy);
	GDREGISTER_CLASS(ZyiMoveSystem);

	GDREGISTER_CLASS(ZyiSyncHelper);
	GDREGISTER_CLASS(ZyiSynchronizerDataField);
	GDREGISTER_CLASS(ZyiSynchronizerNodeField);
	GDREGISTER_CLASS(ZyiSynchronizerAnimationPlayerField);
	GDREGISTER_CLASS(ZyiSynchronizerCharacterStateField);
	GDREGISTER_CLASS(ZyiSyncConsumer);
	GDREGISTER_CLASS(ZyiMultiplayerSynchronizerStateTask);
}

void uninitialize_zyi_game_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	// Nothing to do here in this example.
}

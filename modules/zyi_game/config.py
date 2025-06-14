# config.py


def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_path():
    return "doc_classes"


def get_doc_classes():
    return [
        "ZyiUtilSet",
        "ZyiUtilEmitter",
        "ZyiUtilCallableObject",
        "ZyiUtilObjectHelper",
        "ZyiUtilCallableHelper",
        "ZyiUtilSignalHelper",
        "ZyiUtilAttribute",
        "ZyiUtilConfigurableAttributeAccessor",
        "ZyiUtilAsync",
        "ZyiUtilAttrManager",
        "ZyiDamageConstant",
        "ZyiDamageExtraMetaResource",
        "ZyiDamageValue",
        "ZyiDamageDefenseResource",
        "ZyiDamageAttackResource",
        "ZyiDamageManagerContext",
        "ZyiDamageMiddlewareConditionResource",
        "ZyiDamageMiddlewareResource",
        "ZyiDamageAttackMiddlewareResource",
        "ZyiDamageDefenseMiddlewareResource",
        "ZyiDamageManager",
        "ZyiNodePool",
        "ZyiGameNodePoolManager",
        "ZyiNodePoolHelper",
        "ZyiTaskQueue",
        "ZyiMoveConstant",
        "ZyiMoveBoidsGrid",
        "ZyiDropSystemCanvas",
        "ZyiFloatingTextSystemCanvas",
        "ZyiMoveComponentProxy",
        "ZyiMoveSystem",
        "ZyiSyncHelper",
        "ZyiSynchronizerDataField",
        "ZyiSynchronizerNodeField",
        "ZyiSynchronizerAnimationPlayerField",
        "ZyiSynchronizerCharacterStateField",
        "ZyiSyncConsumer",
        "ZyiMultiplayerSynchronizerStateTask",
    ]

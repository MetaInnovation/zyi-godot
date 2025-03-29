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
        "ZyiMoveSystem",
        "ZyiMoveProxy",
    ]

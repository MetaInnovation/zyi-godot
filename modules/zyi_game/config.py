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
        "ZyiDamageConstant",
        "ZyiUtilObjectHelper",
        "ZyiUtilCallableHelper",
        "ZyiUtilSignalHelper",
        "ZyiUtilAttribute",
        "ZyiNodePool",
        "ZyiGameNodePoolManager",
        "ZyiNodePoolHelper",
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
    ]

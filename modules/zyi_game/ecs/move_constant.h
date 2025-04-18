#ifndef MOVE_CONSTANT_H
#define MOVE_CONSTANT_H

#include "core/object/class_db.h"
#include "core/variant/binder_common.h"

class ZyiMoveConstant : public Object {
	GDCLASS(ZyiMoveConstant, Object)

protected:
	static void _bind_methods() {
		BIND_BITFIELD_FLAG(MOVE_FLAG_NORMAL);
		BIND_BITFIELD_FLAG(MOVE_FLAG_WITHOUT_CHARACTER_COLLIDE);
		BIND_BITFIELD_FLAG(MOVE_FLAG_BOID_GRID_CHILD);
		BIND_BITFIELD_FLAG(MOVE_FLAG_BOID_GRID_STATIC);
		BIND_BITFIELD_FLAG(MOVE_FLAG_UNFORCED);
		BIND_BITFIELD_FLAG(MOVE_FLAG_CHARACTER_FOLLOW_POSITIVE);
		BIND_BITFIELD_FLAG(MOVE_FLAG_PROXY_BY_MANUAL);
	}

public:
	enum Flags : int16_t {
		MOVE_FLAG_NORMAL = 0,
		// 移动时不考虑碰撞的 character_body_2d
		MOVE_FLAG_WITHOUT_CHARACTER_COLLIDE = 1,
		// 使用 boid 算法的子节点
		MOVE_FLAG_BOID_GRID_CHILD = 2,
		// 参与 boid 算法的静态节点
		MOVE_FLAG_BOID_GRID_STATIC = 4,
		// 不受力的
		MOVE_FLAG_UNFORCED = 8,
		// 移动到跟随目标点时，仍然不断积极尝试移动
		MOVE_FLAG_CHARACTER_FOLLOW_POSITIVE = 16,
		// 代理移动
		MOVE_FLAG_PROXY_BY_MANUAL = 32
	};
};

VARIANT_BITFIELD_CAST(ZyiMoveConstant::Flags);

#endif /* MOVE_CONSTANT_H */

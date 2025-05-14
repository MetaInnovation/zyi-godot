#include "move_system.h"

template <typename T>
inline int64_t ZyiMoveSystemInternalComponentPool<T>::acquire_component() {
	if (released_index_pool.empty()) {
		pool.emplace_back();
		return pool.size() - 1;
	}
	int64_t result = released_index_pool.back();
	released_index_pool.pop_back();
	return result;
}

template <typename T>
void ZyiMoveSystemInternalComponentPool<T>::release_component(int64_t id) {
	released_index_pool.push_back(id);
}

template <typename T>
T *ZyiMoveSystemInternalComponentPool<T>::get_component_ptr(int64_t id) {
	if (id >= 0 && id < static_cast<int64_t>(pool.size())) {
		return &pool[id];
	}
	return nullptr;
}

template <typename T>
void ZyiMoveSystemInternalComponentPool<T>::release_all() {
	pool.clear();
	released_index_pool.clear();
}

const double ZyiMoveSystem::MIN_ROTATION_ANGLE = 0.01;

void ZyiMoveSystem::_bind_methods() {
	ClassDB::bind_static_method("ZyiMoveSystem", D_METHOD("create", "normal_move_capacity", "character_move_capacity", "knockback_move_capacity"), &ZyiMoveSystem::create, DEFVAL(NORMAL_MOVE_INIT_CAPACITY), DEFVAL(CHARACTER_MOVE_INIT_CAPACITY), DEFVAL(KNOCKBACK_MOVE_INIT_CAPACITY));
	ClassDB::bind_method(D_METHOD("set_boids_grid", "boids_grid"), &ZyiMoveSystem::set_boids_grid);
	ClassDB::bind_method(D_METHOD("idle_process_update", "delta"), &ZyiMoveSystem::idle_process_update);
	ClassDB::bind_method(D_METHOD("idle_physics_process_update", "delta"), &ZyiMoveSystem::idle_physics_process_update);
	ClassDB::bind_method(D_METHOD("clean"), &ZyiMoveSystem::clean);

	BIND_CONSTANT(NORMAL_MOVE_INIT_CAPACITY);
	BIND_CONSTANT(CHARACTER_MOVE_INIT_CAPACITY);
	BIND_CONSTANT(KNOCKBACK_MOVE_INIT_CAPACITY);
}

Ref<ZyiMoveSystem> ZyiMoveSystem::create(int p_normal_move_capacity, int p_character_move_capacity, int p_knockback_move_capacity) {
	Ref<ZyiMoveSystem> result = memnew(ZyiMoveSystem());
	result->normal_move_component_pool.pool.reserve(p_normal_move_capacity);
	result->character_move_component_pool.pool.reserve(p_character_move_capacity);
	result->knockback_move_component_pool.pool.reserve(p_knockback_move_capacity);
	return result;
}

int64_t ZyiMoveSystem::acquire_normal_move_component() {
	return normal_move_component_pool.acquire_component();
}

void ZyiMoveSystem::release_normal_move_component(int64_t p_id) {
	ZyiNormalMoveComponent *ptr = get_normal_move_component_ptr(p_id);
	if (ptr) {
		ptr->reset();
		normal_move_component_pool.release_component(p_id);
	}
}

ZyiNormalMoveComponent *ZyiMoveSystem::get_normal_move_component_ptr(int64_t p_id) {
	return normal_move_component_pool.get_component_ptr(p_id);
}

int64_t ZyiMoveSystem::acquire_character_move_component() {
	return character_move_component_pool.acquire_component();
}

void ZyiMoveSystem::release_character_move_component(int64_t p_id) {
	ZyiCharacterMoveComponent *ptr = get_character_move_component_ptr(p_id);
	if (ptr) {
		ptr->reset();
		character_move_component_pool.release_component(p_id);
	}
}

ZyiCharacterMoveComponent *ZyiMoveSystem::get_character_move_component_ptr(int64_t p_id) {
	return character_move_component_pool.get_component_ptr(p_id);
}

int64_t ZyiMoveSystem::acquire_knockback_move_component() {
	return knockback_move_component_pool.acquire_component();
}

void ZyiMoveSystem::release_knockback_move_component(int64_t p_id) {
	ZyiKnockbackMoveComponent *ptr = get_knockback_move_component_ptr(p_id);
	if (ptr) {
		ptr->reset();
		knockback_move_component_pool.release_component(p_id);
	}
}

ZyiKnockbackMoveComponent *ZyiMoveSystem::get_knockback_move_component_ptr(int64_t p_id) {
	return knockback_move_component_pool.get_component_ptr(p_id);
}

void ZyiMoveSystem::set_boids_grid(Ref<ZyiMoveBoidsGrid> p_boids_grid) {
	boids_grid = p_boids_grid;
}

void ZyiMoveSystem::idle_process_update(double p_delta) {
	idle_process_update_normal_move(p_delta);
	idle_process_update_character_move(p_delta);
}

void ZyiMoveSystem::idle_physics_process_update(double p_delta) {
	idle_physics_process_update_normal_move(p_delta);
	idle_physics_process_update_character_move(p_delta);
}

void ZyiMoveSystem::clean() {
	for (size_t i = 0; i < normal_move_component_pool.pool.size(); i++) {
		ZyiNormalMoveComponent &component = normal_move_component_pool.pool[i];
		component.reset();
	}
	normal_move_component_pool.release_all();
	for (size_t i = 0; i < character_move_component_pool.pool.size(); i++) {
		ZyiCharacterMoveComponent &component = character_move_component_pool.pool[i];
		component.reset();
	}
	character_move_component_pool.release_all();
	for (size_t i = 0; i < knockback_move_component_pool.pool.size(); i++) {
		ZyiKnockbackMoveComponent &component = knockback_move_component_pool.pool[i];
		component.reset();
	}
	knockback_move_component_pool.release_all();
}

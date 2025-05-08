#include "task_queue.h"

void ZyiTaskQueue::_bind_methods() {
	ClassDB::bind_static_method("ZyiTaskQueue", D_METHOD("create"), &ZyiTaskQueue::create);
	ClassDB::bind_method(D_METHOD("get_max_ms_per_idle"), &ZyiTaskQueue::get_max_ms_per_idle);
	ClassDB::bind_method(D_METHOD("get_max_task_per_idle"), &ZyiTaskQueue::get_max_task_per_idle);
	ClassDB::bind_method(D_METHOD("set_max_ms_per_idle", "value"), &ZyiTaskQueue::set_max_ms_per_idle);
	ClassDB::bind_method(D_METHOD("set_max_task_per_idle", "value"), &ZyiTaskQueue::set_max_task_per_idle);
	ClassDB::bind_method(D_METHOD("idle_process_task", "delta", "custom_max_ms_per_idle", "custom_max_task_per_idle"), &ZyiTaskQueue::idle_process_task, DEFVAL(0), DEFVAL(0));
	ClassDB::bind_method(D_METHOD("add_task", "handler", "args"), &ZyiTaskQueue::add_task);
	ClassDB::bind_method(D_METHOD("clean"), &ZyiTaskQueue::clean);

	BIND_CONSTANT(INIT_MAX_MS_PER_IDLE);
	BIND_CONSTANT(INIT_MAX_TASK_PER_IDLE);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_ms_per_idle"), "set_max_ms_per_idle", "get_max_ms_per_idle");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_task_per_idle"), "set_max_task_per_idle", "get_max_task_per_idle");
}

Ref<ZyiTaskQueue> ZyiTaskQueue::create() {
	Ref<ZyiTaskQueue> result = memnew(ZyiTaskQueue());
	return result;
}

uint16_t ZyiTaskQueue::get_max_ms_per_idle() const {
	return _max_ms_per_idle;
}

uint16_t ZyiTaskQueue::get_max_task_per_idle() const {
	return _max_task_per_idle;
}

void ZyiTaskQueue::set_max_ms_per_idle(uint16_t p_max_ms_per_idle) {
	_max_ms_per_idle = p_max_ms_per_idle;
}

void ZyiTaskQueue::set_max_task_per_idle(uint16_t p_max_task_per_idle) {
	_max_task_per_idle = p_max_task_per_idle;
}

void ZyiTaskQueue::idle_process_task(double p_delta, uint16_t p_max_ms_per_idle, uint16_t p_max_task_per_idle) {
	if (_task_queue.empty()) {
		return;
	}
	int64_t start_us = Time::get_singleton()->get_ticks_usec();
	int64_t elapsed_us = 0;
	uint16_t task_count = 0;
	uint16_t max_task_count = _max_task_per_idle;
	if (p_max_task_per_idle > 0) {
		max_task_count = p_max_task_per_idle;
	}
	uint32_t max_us = _max_ms_per_idle * 1000;
	if (p_max_ms_per_idle > 0) {
		max_us = p_max_ms_per_idle * 1000;
	}
	while (!_task_queue.empty() && task_count < max_task_count && elapsed_us < max_us) {
		InternalTask &task = _task_queue.front();
		if (task.handler.is_valid()) {
			task.handler.callv(task.args);
		}
		_task_queue.pop();
		elapsed_us = Time::get_singleton()->get_ticks_usec() - start_us;
		task_count += 1;
	}
}

void ZyiTaskQueue::add_task(const Callable &p_handler, const Array &p_args) {
	_task_queue.emplace(p_handler, p_args);
}

void ZyiTaskQueue::clean() {
	_task_queue = std::queue<InternalTask>();
}

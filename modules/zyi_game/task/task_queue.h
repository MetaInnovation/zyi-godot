#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "core/object/ref_counted.h"
#include "core/os/time.h"
#include <queue>

class ZyiTaskQueue : public RefCounted {
	GDCLASS(ZyiTaskQueue, RefCounted)

	struct InternalTask {
		Callable handler;
		Array args;
		InternalTask(const Callable &p_handler, const Array &p_args) :
				handler(p_handler), args(p_args) {}
	};
	uint16_t _max_ms_per_idle = INIT_MAX_MS_PER_IDLE;
	uint16_t _max_task_per_idle = INIT_MAX_TASK_PER_IDLE;
	uint16_t _min_task_per_idle = INIT_MIN_TASK_PER_IDLE;
	std::queue<InternalTask> _task_queue;

protected:
	static void _bind_methods();

public:
	enum {
		INIT_MAX_MS_PER_IDLE = 2,
		INIT_MAX_TASK_PER_IDLE = 16,
		INIT_MIN_TASK_PER_IDLE = 8,
	};

	static Ref<ZyiTaskQueue> create();

	uint16_t get_max_ms_per_idle() const;
	uint16_t get_max_task_per_idle() const;
	uint16_t get_min_task_per_idle() const;
	void set_max_ms_per_idle(uint16_t p_max_ms_per_idle);
	void set_max_task_per_idle(uint16_t p_max_task_per_idle);
	void set_min_task_per_idle(uint16_t p_min_task_per_idle);

	void idle_process_task(double p_delta, uint16_t p_max_ms_per_idle = 0, uint16_t p_max_task_per_idle = 0, uint16_t p_min_task_per_idle = 0);
	void add_task(const Callable &p_handler, const Array &p_args);
	void clean();
};

#endif /* TASK_QUEUE_H */

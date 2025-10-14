#include "synchronizer_state_task.h"

void ZyiMultiplayerSynchronizerStateTask::_bind_methods() {
	ClassDB::bind_method(D_METHOD("receive_update_data_queue", "queue"), &ZyiMultiplayerSynchronizerStateTask::receive_update_data_queue);
	ClassDB::bind_method(D_METHOD("consume_interpolate_update_data", "delta", "ref_node", "node_getter_resolver"), &ZyiMultiplayerSynchronizerStateTask::consume_interpolate_update_data);
	ClassDB::bind_method(D_METHOD("consume_next_update_data", "ref_node", "node_getter_resolver"), &ZyiMultiplayerSynchronizerStateTask::consume_next_update_data);
	ClassDB::bind_method(D_METHOD("add_node", "node", "meta"), &ZyiMultiplayerSynchronizerStateTask::add_node);
	ClassDB::bind_method(D_METHOD("resolve_update_data"), &ZyiMultiplayerSynchronizerStateTask::resolve_update_data);
	ClassDB::bind_method(D_METHOD("prepare_run_data"), &ZyiMultiplayerSynchronizerStateTask::prepare_run_data);
	ClassDB::bind_method(D_METHOD("run"), &ZyiMultiplayerSynchronizerStateTask::run);
	ClassDB::bind_method(D_METHOD("add_to_pool", "high_priority", "description"), &ZyiMultiplayerSynchronizerStateTask::add_to_pool, DEFVAL(false), DEFVAL(""));
	ClassDB::bind_method(D_METHOD("try_finish_in_pool"), &ZyiMultiplayerSynchronizerStateTask::try_finish_in_pool);
	ClassDB::bind_method(D_METHOD("get_task_id"), &ZyiMultiplayerSynchronizerStateTask::get_task_id);
	ClassDB::bind_method(D_METHOD("is_working"), &ZyiMultiplayerSynchronizerStateTask::is_working);
	ClassDB::bind_method(D_METHOD("accept_work", "task_id"), &ZyiMultiplayerSynchronizerStateTask::accept_work);
	ClassDB::bind_method(D_METHOD("finish_work"), &ZyiMultiplayerSynchronizerStateTask::finish_work);
	ClassDB::bind_method(D_METHOD("clean", "force"), &ZyiMultiplayerSynchronizerStateTask::clean, DEFVAL(false));

	BIND_CONSTANT(UPDATE_NODE);
	BIND_CONSTANT(UPDATE_PLAYER);
}

void ZyiMultiplayerSynchronizerStateTask::add_node(Node *node, Dictionary meta) {
	bool meta_cacheable = meta.get("meta_cacheable", false);
	ObjectID node_id = node->get_instance_id();
	if (meta_cacheable && _id_to_cached_node_data.has(node_id)) {
		node_list.push_back(_id_to_cached_node_data[node_id]);
		return;
	}
	InternalNodeData &node_data = node_list.emplace_back();
	node_data.node_id = node->get_instance_id();
	node_data.meta = meta;
	Array sync_list = meta["sync_list"];
	int64_t num = sync_list.size();
	node_data.field_list.resize(num);
	for (int64_t index = 0; index < num; index++) {
		ZyiSynchronizerDataField *field = Object::cast_to<ZyiSynchronizerDataField>(sync_list[index]);
		node_data.field_list[index] = InternalFieldData{ field, field->get_cache_data(node) };
	}
	if (meta_cacheable) {
		_id_to_cached_node_data[node_id] = node_data;
	}
}

// 获取需要更新的数据
PackedByteArray ZyiMultiplayerSynchronizerStateTask::resolve_update_data() {
	MutexLock l(mutex);
	PackedByteArray result = _shared_normalized_update_data;
	_shared_normalized_update_data = PackedByteArray();
	return result;
}

// 准备运行数据
void ZyiMultiplayerSynchronizerStateTask::prepare_run_data() {
	{
		MutexLock l(mutex);
		if (_shared_data_prepared) {
			return;
		}
	}
	LocalVector<int64_t> need_remove_index_list;
	LocalVector<InternalFieldPrepareData> prepare_data_list;
	size_t num = node_list.size();
	need_remove_index_list.reserve(num);
	prepare_data_list.reserve(num);
	for (size_t index = 0; index < node_list.size(); index++) {
		InternalNodeData &item = node_list[index];
		bool is_unused = false;
		Node *node = Object::cast_to<Node>(ObjectDB::get_instance(item.node_id));
		if (node == nullptr || node->is_queued_for_deletion()) {
			is_unused = true;
		} else {
			bool valid = node->is_inside_tree();
			String update_key = item.meta.get("update_key", "");
			if (valid && update_key == "") {
				update_key = node->get_path().operator String();
			}
			if (node->has_method(METHOD_IS_STATE_SYNC_VALID)) {
				valid = node->callv(METHOD_IS_STATE_SYNC_VALID, Array());
			}
			if (valid && update_key != "") {
				size_t num = item.field_list.size();
				LocalVector<InternalFieldPrepareDataItem> prepare_data_arr;
				prepare_data_arr.resize(num);
				for (size_t index = 0; index < num; index++) {
					const InternalFieldData &field_data = item.field_list[index];
					const Callable threading_data_list_normalizer = field_data.field->get_threading_data_list_normalizer();
					prepare_data_arr[index] = InternalFieldPrepareDataItem{
						threading_data_list_normalizer,
						field_data.field->get_data_list(field_data.field->get_prepare_data(node, field_data.cache_data)),
					};
				}
				int8_t update_type = item.meta.get("update_type", UpdateType::UPDATE_NODE);
				prepare_data_list.push_back(InternalFieldPrepareData{ update_key, update_type, prepare_data_arr });
			} else {
				is_unused = item.meta.get("allow_outside_tree", false).operator bool() != true;
			}
		}
		if (is_unused) {
			need_remove_index_list.push_back(index);
		}
	}
	if (!need_remove_index_list.is_empty()) {
		for (int64_t index = need_remove_index_list.size() - 1; index >= 0; index--) {
			node_list.erase(node_list.begin() + need_remove_index_list[index]);
		}
	}
	{
		MutexLock l(mutex);
		_shared_prepare_data_list = prepare_data_list;
		_shared_data_prepared = true;
	}
}

void ZyiMultiplayerSynchronizerStateTask::run() {
	LocalVector<InternalFieldPrepareData> prepare_data_list;
	{
		MutexLock l(mutex);
		if (!_shared_data_prepared) {
			return;
		}
		prepare_data_list = _shared_prepare_data_list;
	}
	_update_data[0].operator Dictionary().clear();
	_update_data[1].operator Dictionary().clear();
	size_t num = prepare_data_list.size();
	HashMap<String, LocalVector<HashMap<String, Variant>>> update_key_to_prepare_data_map;
	for (size_t index = 0; index < num; index++) {
		InternalFieldPrepareData field_prepare_data = prepare_data_list[index];
		size_t pn = field_prepare_data.prepare_data_arr.size();
		Dictionary update_key_to_sync_record_list = _update_data[field_prepare_data.update_type];
		Variant *sync_record_list_ptr = update_key_to_sync_record_list.getptr(field_prepare_data.update_key);
		Array sync_record_list;
		if (sync_record_list_ptr != nullptr) {
			sync_record_list = sync_record_list_ptr->operator Array();
		}
		sync_record_list.resize(pn);
		sync_record_list.fill(Variant());
		bool has_update_data = false;
		update_key_to_prepare_data_map[field_prepare_data.update_key] = LocalVector<HashMap<String, Variant>>();
		LocalVector<HashMap<String, Variant>> &map_list = update_key_to_prepare_data_map[field_prepare_data.update_key];
		map_list.resize(pn);
		const LocalVector<HashMap<String, Variant>> *prev_prepare_data_map_list = _prev_update_key_to_prepare_data_map.getptr(field_prepare_data.update_key);
		if (prev_prepare_data_map_list == nullptr) {
			// 所有都是新增
			for (size_t pi = 0; pi < pn; pi++) {
				map_list[pi] = HashMap<String, Variant>();
				HashMap<String, Variant> &map = map_list[pi];
				const InternalFieldPrepareDataItem &field_prepare_data_item = field_prepare_data.prepare_data_arr[pi];
				Array prepare_data = field_prepare_data_item.prepare_data;
				if (field_prepare_data_item.threading_data_list_normalizer.is_valid()) {
					prepare_data = field_prepare_data_item.threading_data_list_normalizer.call(prepare_data);
				}
				if (prepare_data.is_empty()) {
					continue;
				}
				Dictionary value_key_to_data_dict;
				for (const Array &data_item : prepare_data) {
					const Variant &value = data_item[1];
					if (value.get_type() == Variant::NIL) {
						continue;
					}
					Array arr = { format_value(value), ZyiSynchronizerDataField::ACTION_ADD };
					const String &value_key = data_item[0];
					value_key_to_data_dict[value_key] = arr;
					map[value_key] = value;
				}
				if (!value_key_to_data_dict.is_empty()) {
					has_update_data = true;
					sync_record_list[pi] = value_key_to_data_dict;
				}
			}
		} else {
			// 需要 diff
			for (size_t pi = 0; pi < pn; pi++) {
				map_list[pi] = HashMap<String, Variant>();
				HashMap<String, Variant> &map = map_list[pi];
				const InternalFieldPrepareDataItem &field_prepare_data_item = field_prepare_data.prepare_data_arr[pi];
				Array prepare_data = field_prepare_data_item.prepare_data;
				if (field_prepare_data_item.threading_data_list_normalizer.is_valid()) {
					prepare_data = field_prepare_data_item.threading_data_list_normalizer.call(prepare_data);
				}
				if (prepare_data.is_empty()) {
					continue;
				}
				Dictionary value_key_to_data_dict;
				const HashMap<String, Variant> &prev_map = prev_prepare_data_map_list->operator[](pi);
				for (const Array &data_item : prepare_data) {
					const Variant &value = data_item[1];
					if (value.get_type() == Variant::NIL) {
						continue;
					}
					const String &value_key = data_item[0];
					map[value_key] = value;
					const Variant *prev_value = prev_map.getptr(value_key);
					int8_t action;
					if (prev_value == nullptr) {
						// 新增
						action = ZyiSynchronizerDataField::ACTION_ADD;
					} else if (value != *prev_value) {
						// 修改
						action = ZyiSynchronizerDataField::ACTION_CHANGE;
					} else {
						continue;
					}
					Array arr = { format_value(value), action };
					value_key_to_data_dict[value_key] = arr;
				}
				if (!value_key_to_data_dict.is_empty()) {
					has_update_data = true;
					sync_record_list[pi] = value_key_to_data_dict;
				}
			}
		}
		if (sync_record_list_ptr == nullptr && has_update_data) {
			update_key_to_sync_record_list[field_prepare_data.update_key] = sync_record_list;
		}
	}
	_prev_update_key_to_prepare_data_map = update_key_to_prepare_data_map;
	{
		MutexLock l(mutex);
		if (!_update_data[0].operator Dictionary().is_empty() || !_update_data[1].operator Dictionary().is_empty()) {
			_shared_normalized_update_data = encode_byte_data(_update_data);
		} else {
			_shared_normalized_update_data = PackedByteArray();
		}
		_shared_data_prepared = false;
	}
}

void ZyiMultiplayerSynchronizerStateTask::receive_update_data_queue(const TypedArray<PackedByteArray> &p_queue) {
	for (const PackedByteArray &item : p_queue) {
		_received_update_data_queue.push(item);
	}
}

void ZyiMultiplayerSynchronizerStateTask::consume_interpolate_update_data(float delta, Node *ref_node, const Callable &p_node_getter_resolver) {
	// update_key_to_just_changed_float_value_map
}

void ZyiMultiplayerSynchronizerStateTask::consume_next_update_data(Node *ref_node, const Callable &p_node_getter_resolver) {
	if (_received_update_data_queue.empty()) {
		return;
	}
	const PackedByteArray &raw_received_update_data = _received_update_data_queue.front();
	_prev_received_update_data = decode_byte_data(raw_received_update_data);
	_received_update_data_queue.pop();
	if (_prev_received_update_data.is_empty()) {
		return;
	}
	uint64_t ticks_usec = OS::get_singleton()->get_ticks_usec();
	for (int64_t update_type = 0; update_type < _prev_received_update_data.size(); update_type++) {
		Dictionary update_key_to_sync_record_list = _prev_received_update_data[update_type];
		if (update_key_to_sync_record_list.is_empty()) {
			continue;
		}
		Callable node_getter = resolve_node_getter(p_node_getter_resolver, update_type);
		for (const String &update_key : update_key_to_sync_record_list.keys()) {
			Node *node = resolve_node(node_getter, update_key);
			if (node == nullptr || node->is_queued_for_deletion()) {
				continue;
			}
			ObjectID node_id = node->get_instance_id();
			InternalNodeData *node_data = _receiver_id_to_cached_node_data.getptr(node_id);
			if (node_data == nullptr) {
				// 记录 node_data 并缓存
				node_data = &(_receiver_id_to_cached_node_data[node_id] = InternalNodeData{ node_id, node->callv(METHOD_GET_STATE_SYNC_AUTO_META, Array()) });
				Array sync_list = node_data->meta["sync_list"];
				size_t num = sync_list.size();
				node_data->field_list.resize(num);
				for (size_t i = 0; i < num; i++) {
					InternalFieldData &field_data = (node_data->field_list[i] = InternalFieldData{ Object::cast_to<ZyiSynchronizerDataField>(sync_list[i]) });
					field_data.cache_data = field_data.field->get_cache_data(node);
				}
			}
			Array sync_record_list = update_key_to_sync_record_list[update_key];
			for (int64_t item_index = 0; item_index < sync_record_list.size(); item_index++) {
				Variant value_key_to_data_val = sync_record_list[item_index];
				if (value_key_to_data_val.get_type() != Variant::DICTIONARY) {
					continue;
				}
				InternalFieldData &field_data = node_data->field_list[item_index];
				const Ref<ZyiSynchronizerDataField> &field = field_data.field;
				Variant prepare_data = field->get_prepare_data(node, field_data.cache_data, true);
				if (is_invalid_prepare_data(prepare_data)) {
					continue;
				}
				Dictionary value_key_to_data = value_key_to_data_val;
				for (const String &value_key : value_key_to_data.keys()) {
					Array item = value_key_to_data[value_key];
					Variant value = parse_value(item[0]);
					int8_t action = item[1];
					field->update_data(node, prepare_data, value_key, value, action);
				}
			}
		}
	}
}

bool ZyiMultiplayerSynchronizerStateTask::add_to_pool(bool high_priority, String description) {
	WorkerThreadPool::TaskID pool_task_id = WorkerThreadPool::get_singleton()->add_task(callable_mp(this, &ZyiMultiplayerSynchronizerStateTask::run), high_priority, description);
	return accept_work(pool_task_id);
}

bool ZyiMultiplayerSynchronizerStateTask::try_finish_in_pool() {
	if (is_working() && WorkerThreadPool::get_singleton()->is_task_completed(task_id)) {
		finish_work();
		return true;
	} else {
		return false;
	}
}
int64_t ZyiMultiplayerSynchronizerStateTask::get_task_id() const {
	return task_id;
}

bool ZyiMultiplayerSynchronizerStateTask::is_working() {
	return task_id != INVALID_TASK_ID;
}

bool ZyiMultiplayerSynchronizerStateTask::accept_work(int64_t p_id) {
	if (is_working()) {
		return false;
	}
	task_id = p_id;
	return true;
}

bool ZyiMultiplayerSynchronizerStateTask::finish_work() {
	if (!is_working()) {
		return false;
	}
	task_id = INVALID_TASK_ID;
	return true;
}

void ZyiMultiplayerSynchronizerStateTask::clean(bool force) {
	task_id = INVALID_TASK_ID;
	_shared_normalized_update_data.clear();
	_id_to_cached_node_data.clear();
	_receiver_id_to_cached_node_data.clear();
	if (force) {
		_shared_prepare_data_list.clear();
		_shared_normalized_update_data.clear();
		node_list.clear();
		_update_data.resize(2);
		_update_data[0] = Dictionary();
		_update_data[1] = Dictionary();
		_prev_update_key_to_prepare_data_map.clear();
		_receiver_id_to_cached_node_data.clear();
		_received_update_data_queue = std::queue<PackedByteArray>();
	}
}

ZyiMultiplayerSynchronizerStateTask::ZyiMultiplayerSynchronizerStateTask() {
	_update_data.resize(2);
	_update_data[0] = Dictionary();
	_update_data[1] = Dictionary();
}

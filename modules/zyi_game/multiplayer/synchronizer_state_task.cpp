#include "synchronizer_state_task.h"

void ZyiMultiplayerSynchronizerStateTask::_bind_methods() {
	ClassDB::bind_static_method("ZyiMultiplayerSynchronizerStateTask", D_METHOD("rpc_apply_update_data", "ref_node", "update_data_bytes", "node_getter_resolver"), &ZyiMultiplayerSynchronizerStateTask::rpc_apply_update_data);
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

	BIND_CONSTANT(UPDATE_NODE);
	BIND_CONSTANT(UPDATE_PLAYER);
}

void ZyiMultiplayerSynchronizerStateTask::rpc_apply_update_data(Node *ref_node, const PackedByteArray &p_update_data_bytes, const Callable &p_node_getter_resolver) {
	Array remote_update_data = decode_byte_data(p_update_data_bytes);
	for (int64_t update_type = 0; update_type < remote_update_data.size(); update_type++) {
		Dictionary update_key_to_sync_record_list = remote_update_data[update_type];
		if (update_key_to_sync_record_list.is_empty()) {
			continue;
		}
		Callable node_getter = resolve_node_getter(p_node_getter_resolver, update_type);
		for (const String &update_key : update_key_to_sync_record_list.keys()) {
			Node *node = resolve_node(node_getter, update_key);
			if (node == nullptr || node->is_queued_for_deletion()) {
				continue;
			}
			Variant meta_val = node->callv(METHOD_GET_STATE_SYNC_AUTO_META, Array());
			if (meta_val.get_type() != Variant::DICTIONARY) {
				continue;
			}
			Dictionary meta = meta_val;
			Array sync_list = meta["sync_list"];
			if (sync_list.is_empty()) {
				continue;
			}
			Array sync_record_list = update_key_to_sync_record_list[update_key];
			for (int64_t item_index = 0; item_index < sync_record_list.size(); item_index++) {
				Variant value_key_to_records_val = sync_record_list[item_index];
				if (value_key_to_records_val.get_type() != Variant::DICTIONARY) {
					continue;
				}
				Ref<ZyiSynchronizerDataField> field = Object::cast_to<ZyiSynchronizerDataField>(sync_list[item_index]);
				Variant prepare_data = field->get_prepare_data(node, field->get_cache_data(node), true);
				if (is_invalid_prepare_data(prepare_data)) {
					continue;
				}
				Dictionary value_key_to_records = value_key_to_records_val;
				for (const String &value_key : value_key_to_records.keys()) {
					Array records = value_key_to_records[value_key];
					for (int64_t record_index = 0; record_index < records.size(); record_index++) {
						Array item = records[record_index];
						Variant value = parse_value(item[0]);
						int8_t action = item[1];
						field->update_data(node, prepare_data, value_key, value, action);
					}
				}
			}
		}
	}
}

void ZyiMultiplayerSynchronizerStateTask::add_node(Node *node, Dictionary meta) {
	MutexLock l(mutex);
	InternalNodeData &node_data = node_list.emplace_back();
	node_data.node_id = node->get_instance_id();
	node_data.meta = meta;
	Array sync_list = meta["sync_list"];
	int64_t num = sync_list.size();
	for (int64_t index = 0; index < num; index++) {
		ZyiMultiplayerSynchronizerStateTask::InternalStateCacheItem &val = node_data.state_cache.emplace_back();
		val.index = index;
		val.max_index = num;
		val.field = Object::cast_to<ZyiSynchronizerDataField>(sync_list[index]);
		val.field_cache_data = val.field->get_cache_data(node);
	}
}

// 获取需要更新的数据
PackedByteArray ZyiMultiplayerSynchronizerStateTask::resolve_update_data() {
	MutexLock l(mutex);
	PackedByteArray result = _normalized_update_data;
	_normalized_update_data = PackedByteArray();
	return result;
}

// 准备运行数据
void ZyiMultiplayerSynchronizerStateTask::prepare_run_data() {
	MutexLock l(mutex);
	if (data_prepared) {
		return;
	}
	LocalVector<int64_t> need_remove_index_list;
	need_remove_index_list.reserve(node_list.size());
	for (size_t index = 0; index < node_list.size(); index++) {
		InternalNodeData &item = node_list[index];
		bool is_unused = false;
		Node *node = Object::cast_to<Node>(ObjectDB::get_instance(item.node_id));
		if (node == nullptr || node->is_queued_for_deletion()) {
			is_unused = true;
		} else {
			bool valid = false;
			if (node->has_method(METHOD_IS_STATE_SYNC_VALID)) {
				valid = node->callv(METHOD_IS_STATE_SYNC_VALID, Array());
			} else {
				valid = node->is_inside_tree();
			}
			if (valid) {
				for (ZyiMultiplayerSynchronizerStateTask::InternalStateCacheItem &cache_item : item.state_cache) {
					cache_item.prepare_data = cache_item.field->get_prepare_data(node, cache_item.field_cache_data);
				}
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
	data_prepared = true;
}

void ZyiMultiplayerSynchronizerStateTask::run() {
	MutexLock l(mutex);
	if (!data_prepared) {
		return;
	}
	_update_data[0].operator Dictionary().clear();
	_update_data[1].operator Dictionary().clear();
	LocalVector<int64_t> need_remove_index_list;
	need_remove_index_list.reserve(node_list.size());
	for (size_t item_index = 0; item_index < node_list.size(); item_index++) {
		InternalNodeData &item = node_list[item_index];
		bool is_unused = false;
		Node *node = Object::cast_to<Node>(ObjectDB::get_instance(item.node_id));
		if (node == nullptr || node->is_queued_for_deletion()) {
			is_unused = true;
		} else {
			String update_key = item.meta.get("update_key", "");
			if (update_key == "") {
				if (node->is_inside_tree()) {
					update_key = node->get_path().operator String();
				} else {
					is_unused = true;
					continue;
				}
			}
			for (ZyiMultiplayerSynchronizerStateTask::InternalStateCacheItem &cache_item : item.state_cache) {
				if (is_invalid_prepare_data(cache_item.prepare_data)) {
					continue;
				}
				Ref<ZyiSynchronizerDataField> &field = cache_item.field;
				Array data_list = field->get_data_list(cache_item.prepare_data);
				if (data_list.is_empty()) {
					continue;
				}
				// diff
				Array &cached_list = cache_item.cached_list;
				Dictionary &cached_list_key_to_index = cache_item.cached_list_key_to_index;
				Dictionary cached_list_key_visited = cached_list_key_to_index.duplicate();
				int64_t num = data_list.size();
				const int64_t invalid_index = -1;
				const int64_t visited_index = -2;
				for (int64_t index = 0; index < num; index++) {
					Array data = data_list[index];
					const String &value_key = data[0];
					int64_t cached_index = cached_list_key_to_index.get(value_key, invalid_index);
					if (cached_list_key_visited.get(value_key, invalid_index).operator int64_t() == visited_index) {
						continue;
					}
					cached_list_key_visited[value_key] = visited_index;
					const Variant &value = data[1];
					if (cached_index == invalid_index) {
						// 新增
						if (cache_item.unused_indices.empty()) {
							cached_list_key_to_index[value_key] = cached_list.size();
							cached_list.push_back(data);
						} else {
							cached_index = cache_item.unused_indices.back();
							cache_item.unused_indices.pop_back();
							cached_list_key_to_index[value_key] = cached_index;
							cached_list[cached_index] = data;
						}
						record_update(item, cache_item, data, ZyiSynchronizerDataField::ACTION_ADD, update_key);
					} else {
						// 修改
						const Array &cached = cached_list[cached_index];
						if (field->is_data_value_changed(value_key, cached[1], value)) {
							cached_list[cached_index] = data;
							record_update(item, cache_item, data, ZyiSynchronizerDataField::ACTION_CHANGE, update_key);
						}
					}
				}

				LocalVector<Variant> keys = cached_list_key_visited.get_key_list();
				for (const String &item_key : keys) {
					if (cached_list_key_visited[item_key].operator int64_t() == visited_index) {
						continue;
					}
					int64_t index = cached_list_key_to_index[item_key];
					cache_item.unused_indices.push_back(index);
					cached_list_key_to_index.erase(item_key);
					cached_list[index] = Variant();
					Array arr;
					arr.resize(2);
					arr[0] = item_key;
					arr[1] = Variant();
					record_update(item, cache_item, arr, ZyiSynchronizerDataField::ACTION_REMOVE, update_key);
				}
			}
		}
		if (is_unused) {
			need_remove_index_list.push_back(item_index);
		}
	}
	if (!_update_data[0].operator Dictionary().is_empty() || !_update_data[1].operator Dictionary().is_empty()) {
		_normalized_update_data = encode_byte_data(_update_data);
	} else {
		_normalized_update_data = PackedByteArray();
	}
	if (!need_remove_index_list.is_empty()) {
		for (int64_t index = need_remove_index_list.size() - 1; index >= 0; index--) {
			node_list.erase(node_list.begin() + need_remove_index_list[index]);
		}
	}
	data_prepared = false;
}

void ZyiMultiplayerSynchronizerStateTask::record_update(const InternalNodeData &p_node_data, const InternalStateCacheItem &p_item, const Variant &p_data, int8_t p_action, const String &p_update_key) {
	int8_t update_type = p_node_data.meta.get("update_type", UpdateType::UPDATE_NODE);
	Array data_arr = p_data;
	if (data_arr[1].get_type() == Variant::NIL) {
		return;
	}
	const String &value_key = data_arr[0];
	const Variant value = format_value(data_arr[1]);
	Dictionary update_key_to_sync_record_list = _update_data[update_type];
	Variant sync_record_list = update_key_to_sync_record_list.get(p_update_key, Variant());
	if (!sync_record_list.is_array()) {
		Array arr = Array();
		arr.resize(p_item.max_index);
		arr.fill(Variant());
		sync_record_list = arr;
		update_key_to_sync_record_list[p_update_key] = sync_record_list;
	}
	int64_t item_index = p_item.index;
	Array sync_record_arr = sync_record_list;
	Variant value_key_to_records = sync_record_arr[item_index];
	if (value_key_to_records.get_type() != Variant::DICTIONARY) {
		value_key_to_records = Dictionary();
		sync_record_arr[item_index] = value_key_to_records;
	}
	Dictionary value_key_to_records_dict = value_key_to_records;
	Variant value_list = value_key_to_records_dict.get(value_key, Variant());
	if (!value_list.is_array()) {
		value_list = Array();
		value_key_to_records_dict[value_key] = value_list;
	}
	Array arr;
	arr.resize(2);
	arr[0] = value;
	arr[1] = p_action;
	value_list.operator Array().append(arr);
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

ZyiMultiplayerSynchronizerStateTask::ZyiMultiplayerSynchronizerStateTask() {
	_update_data.resize(2);
	_update_data[0] = Dictionary();
	_update_data[1] = Dictionary();
}

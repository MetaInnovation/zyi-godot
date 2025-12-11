#ifndef SYNCHRONIZER_STATE_TASK_H
#define SYNCHRONIZER_STATE_TASK_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/templates/hash_map.h"
#include "core/templates/local_vector.h"
#include "core/variant/variant_utility.h"
#include "sync_helper.h"
#include "sync_store_node.h"
#include "synchronizer_data_field.h"
#include <queue>
#include <vector>

class ZyiMultiplayerSynchronizerStateTask : public RefCounted {
	GDCLASS(ZyiMultiplayerSynchronizerStateTask, RefCounted)

protected:
	static void _bind_methods();

public:
	static constexpr const int64_t INVALID_TASK_ID = -1;
	static constexpr const char *METHOD_GET_STATE_SYNC_AUTO_META = "get_state_sync_auto_meta";
	static constexpr const char *METHOD_IS_STATE_SYNC_VALID = "is_state_sync_valid";
	static constexpr const char *CACHED_NODE_DATA_KEY = "cached_node_data";
	enum UpdateType {
		UPDATE_NODE,
		UPDATE_PLAYER
	};
	struct InternalStateCacheItem {
		int64_t index;
		int64_t max_index;
		Ref<ZyiSynchronizerDataField> field;
		Variant field_cache_data;
		Variant prepare_data;
		Array cached_list;
		Dictionary cached_list_key_to_index;
		std::vector<int64_t> unused_indices;
	};
	struct InternalFieldPrepareDataItem {
		Callable threading_data_list_normalizer;
		Array prepare_data;
	};
	struct InternalFieldPrepareData {
		uint64_t update_key;
		int8_t update_type;
		LocalVector<InternalFieldPrepareDataItem> prepare_data_arr;
	};
	struct InternalFieldChangeData {
		uint32_t changed_count;
		Vector2 p1_vector;
		Vector2 p2_vector;
	};
	struct InternalFieldData {
		Ref<ZyiSynchronizerDataField> field;
		Variant cache_data;
		HashMap<String, InternalFieldChangeData> value_key_to_just_changed_float_value_map;
	};
	struct InternalNodeData {
		ObjectID node_id;
		Dictionary meta;
		LocalVector<InternalFieldData> field_list;
	};

	int8_t data_type = ZyiSynchronizerDataField::DATA_ALL;

	int64_t task_id = INVALID_TASK_ID;
	Mutex mutex;
	// 共享的
	bool _shared_data_prepared = false;
	LocalVector<InternalFieldPrepareData> _shared_prepare_data_list;
	PackedByteArray _shared_normalized_update_data;
	// 不共享的
	std::vector<InternalNodeData> node_list;
	HashMap<ObjectID, InternalNodeData> _id_to_cached_node_data;

	TypedArray<Dictionary> _update_data;
	HashMap<uint64_t, LocalVector<HashMap<String, Variant>>> _prev_update_key_to_prepare_data_map;

	HashMap<ObjectID, InternalNodeData> _receiver_id_to_cached_node_data;
	Array _prev_received_update_data;
	std::queue<PackedByteArray> _received_update_data_queue;

	_FORCE_INLINE_ static bool is_invalid_prepare_data(const Variant &p_value) {
		return p_value.get_type() == Variant::NIL || (p_value.get_type() == Variant::OBJECT && p_value.is_null());
	}
	_FORCE_INLINE_ static PackedByteArray encode_byte_data(const Variant &p_value) {
		return VariantUtilityFunctions::var_to_bytes(p_value);
	}
	_FORCE_INLINE_ static Variant decode_byte_data(const PackedByteArray &p_value) {
		return VariantUtilityFunctions::bytes_to_var(p_value);
	}
	_FORCE_INLINE_ static Variant format_value(const Variant &p_value) {
		return ZyiSyncHelper::format_variant(p_value);
	}
	_FORCE_INLINE_ static Variant parse_value(const Variant &p_value) {
		return ZyiSyncHelper::parse_variant(p_value);
	}
	void add_node(Node *node, Dictionary meta);
	PackedByteArray resolve_update_data();
	void prepare_run_data();
	void run();

	void receive_update_data_queue(const TypedArray<PackedByteArray> &p_queue);
	void consume_interpolate_update_data(float delta, ZyiSyncStoreNode *ref_node);
	void consume_next_update_data(ZyiSyncStoreNode *ref_node);

	bool add_to_pool(bool high_priority = false, String description = "");
	bool try_finish_in_pool();
	int64_t get_task_id() const;
	bool is_working();
	bool accept_work(int64_t p_id);
	bool finish_work();
	void clean(bool force = false);
	static Ref<ZyiMultiplayerSynchronizerStateTask> create(int8_t p_data_type = ZyiSynchronizerDataField::DATA_ALL);

	ZyiMultiplayerSynchronizerStateTask(int8_t p_data_type = ZyiSynchronizerDataField::DATA_ALL);
};

#endif /* SYNCHRONIZER_STATE_TASK_H */

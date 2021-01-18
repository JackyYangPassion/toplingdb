//
// Created by leipeng on 2021/1/11.
//
#pragma once
#include "compaction_job.h"

namespace ROCKSDB_NAMESPACE {

struct ObjectRpcParam {
  std::string clazz;
  std::string content; // serialized bytes for rpc
};

struct CompactionParams {
  int job_id;
  //int level;
  int output_level;
  int num_levels;
  uint32_t cf_id;
  const std::vector<CompactionInputFiles>* inputs;
  uint64_t current_next_file_number;
  uint64_t target_file_size;
  uint64_t max_compaction_bytes;

  // we add a dedicated path to compaction worker's cf_path as
  // output path, thus reduce changes to the existing rocksdb code.
  // the output_path_id should be the last elem of cf_paths, so it
  // needs not the field output_path_id.
  //uint32_t output_path_id; // point to the extra cf_path

  uint32_t max_subcompactions; // num_threads
  CompressionType compression;
  CompressionOptions compression_opts;
  const std::vector<FileMetaData*>* grandparents;
  double score;
  bool manual_compaction;
  bool deletion_compaction;
  CompactionReason compaction_reason;

  VersionSet* version_set;
  SequenceNumber preserve_deletes_seqnum;
  const std::vector<SequenceNumber>* existing_snapshots;
  SequenceNumber earliest_write_conflict_snapshot;
  bool paranoid_file_checks;
  std::string dbname;
  std::string db_id;
  std::string db_session_id;
  std::string full_history_ts_low;
  CompactionJobStats* compaction_job_stats;
  //SnapshotChecker* snapshot_checker; // not used
  //FSDirectory* db_directory;
  //FSDirectory* output_directory;
  //FSDirectory* blob_output_directory;
  Statistics* stats;

  //ObjectRpcParam compaction_filter; // don't use compaction_filter
  ObjectRpcParam compaction_filter_factory; // always use
  ObjectRpcParam merge_operator;
  ObjectRpcParam user_comparator;
  ObjectRpcParam table_factory;
  ObjectRpcParam prefix_extractor;
  ObjectRpcParam sst_partitioner;

  //bool skip_filters;
  bool allow_ingest_behind;
  bool preserve_deletes;
  bool bottommost_level;
  std::string smallest_user_key;
  std::string largest_user_key;
  //std::vector<ObjectRpcParam> event_listner;
  std::vector<ObjectRpcParam> int_tbl_prop_collector_factories;
};

struct CompactionResults {
  struct FileMinMeta {
    std::string fname;
    uint64_t    fsize;
    uint64_t    smallest_seqno;
    uint64_t    largest_seqno;
  };
  struct ObjectRpcRetVal {
    std::string compaction_filter;
    std::string merge_operator;
    std::string user_comparator;
    std::string table_builder;
    std::string prefix_extractor;
    std::vector<std::string> int_tbl_prop_collector;
    std::vector<std::string> event_listner;
    std::vector<FileMinMeta> output_files;
    CompactionJobStats job_stats;
    uint64_t num_output_records;
  };
  // collect remote statistics
  struct StatisticsResult {
    uint64_t tickers[INTERNAL_TICKER_ENUM_MAX] = {0};
    HistogramStat histograms[INTERNAL_HISTOGRAM_ENUM_MAX];
  };
  std::vector<ObjectRpcRetVal> sub_compacts;
  StatisticsResult stat_result;
  Status status;
};

struct CompactionRpcStub {
  struct OneSub {
    std::unique_ptr<CompactionFilter> compaction_filter;
    std::unique_ptr<SstPartitioner> sst_partitioner;
    //std::unique_ptr<EventListener> event_listner;
    //std::unique_ptr<MergeOperator> merge_operator;
    //std::unique_ptr<TableBuilder> table_builder;
    //std::unique_ptr<FileChecksumGenerator> file_checksum_generator;
  };
  std::vector<OneSub> sub_compacts;
};

class CompactionExecutor {
 public:
  virtual ~CompactionExecutor();
  virtual Status Execute(const CompactionParams&, CompactionResults*) = 0;
};

class CompactionExecutorFactory {
 public:
  virtual ~CompactionExecutorFactory();
  virtual CompactionExecutor* NewExecutor(const Compaction*) const = 0;
};

CompactionExecutorFactory* GetLocalCompactionExecutorFactory();

} // namespace ROCKSDB_NAMESPACE

#ifndef _MAGIC_COMMON_H
#define _MAGIC_COMMON_H

/* LLVM-20 Magic constants and configuration */
#define LLVM_VERSION_MAJOR 20
#define MAGIC_LLVM20_COMPAT 1

/* Magic constants. */
#define MAGIC_PREFIX magic_
#define MAGIC_PREFIX_STR "magic_"
#define MAGIC_ASR_PREFIX magic_asr_
#define MAGIC_ASR_PREFIX_STR "magic_asr_"
#define MAGIC_NESTED_PREFIX_STR "nested_"
#define MAGIC_EVAL_FUNC_PREFIX "me_"
#define MAGIC_ANON_MEMBER_PREFIX "magic.anon"

/* Magic instrumentation identifiers */
#define MAGIC_VOID_PTR _____magic_instr_void_ptr
#define MAGIC_VOID_PTR_NAME "_____magic_instr_void_ptr"
#define MAGIC_VOID_ARRAY _____magic_instr_void_arr
#define MAGIC_VOID_ARRAY_NAME "_____magic_instr_void_arr"

/* Magic type system identifiers */
#define MAGIC_VOID_PTR_TYPE _magic_void_ptr_type_ptr
#define MAGIC_VOID_PTR_TYPE_ID 1
#define MAGIC_VOID_PTR_INT_CAST_TYPE _magic_void_ptr_int_cast_type_ptr
#define MAGIC_VOID_PTR_INT_CAST_TYPE_ID 2
#define MAGIC_VOID_ARRAY_TYPE _magic_void_array_type_ptr
#define MAGIC_VOID_ARRAY_TYPE_ID 3
#define MAGIC_PTRINT_TYPE _magic_ptrint_type_ptr
#define MAGIC_PTRINT_TYPE_ID 4

#ifdef __MINIX
#define GLOBAL_VARS_IN_SECTION 1
#define MAGIC_USE_MINIX_EXTENSIONS 1
#define MAGIC_MINIX_MEMORY_PROTECTION 1
#else
#define GLOBAL_VARS_IN_SECTION 0
#define MAGIC_USE_MINIX_EXTENSIONS 0
#endif

/* Magic section definitions */
#define GLOBAL_VARS_SECTION_PREFIX ".gvars"
#define GLOBAL_VARS_SECTION_DATA GLOBAL_VARS_SECTION_PREFIX
#define GLOBAL_VARS_SECTION_RO (GLOBAL_VARS_SECTION_PREFIX "_ro")
#define MAGIC_LLVM_METADATA_SECTION "llvm.metadata"
#define MAGIC_LLVM20_METADATA_SECTION "llvm.metadata.20"
#define MAGIC_STATIC_FUNCTIONS_SECTION "magic_functions"
#define MAGIC_STATIC_VARS_SECTION_PREFIX "magic_data"
#define MAGIC_SHADOW_VARS_SECTION_PREFIX "magic_shadow_data"

/* Magic configuration for LLVM-20. */
#ifndef MAGIC_OUTPUT_CTL
#define MAGIC_OUTPUT_CTL 0
#endif
#define MAGIC_CHECK_LEVEL 1
#define MAGIC_FLATTEN_FUNCTION_ARGS 0
#define MAGIC_CHECK_INVARIANTS 1
#define MAGIC_SHRINK_TYPE_STR 1
#define MAGIC_MAX_NAME_LEN 128
#define MAGIC_MAX_TYPE_STR_LEN 512
#define MAGIC_MAX_RECURSIVE_TYPES 2048
#define MAGIC_TYPE_STR_PRINT_MAX 10000
#define MAGIC_TYPE_STR_PRINT_MAX_LEVEL 15
#define MAGIC_MAX_DEAD_DSENTRIES 20
#define MAGIC_MAX_DEAD_DSENTRIES_SIZE (1024 * 8 * 20)
#define MAGIC_NAMED_ALLOC_USE_DBG_INFO 1

#ifndef MAGIC_MEM_USAGE_OUTPUT_CTL
#define MAGIC_MEM_USAGE_OUTPUT_CTL 1
#endif

/* LLVM-20 Enhanced instrumentation settings */
#define MAGIC_INSTRUMENT_MEM_FUNCS_ASR_ONLY 0
#define MAGIC_INSTRUMENT_MEM_CUSTOM_WRAPPERS 1
#define MAGIC_INSTRUMENT_MEM_FUNCS 1
#define MAGIC_INSTRUMENT_STACK 1
#define MAGIC_INSTRUMENT_HEAP_TRACKING 1
#define MAGIC_INSTRUMENT_TYPE_SAFETY 1
#define MAGIC_FORCE_RAW_UNIONS 0
#define MAGIC_FORCE_RAW_BITFIELDS 0
#define MAGIC_FORCE_DYN_MEM_ZERO_INIT 1
#define MAGIC_INDEX_DYN_LIBS 1
#define MAGIC_USE_DYN_MEM_WRAPPERS 1
#define MAGIC_USE_DYN_DL_WRAPPERS 1
#define MAGIC_ALLOW_DYN_MEM_WRAPPER_NESTING 1

/* LLVM-20 enhanced profiling and debugging settings */
#define MAGIC_USE_ENHANCED_DEBUGGING 1
#define MAGIC_THREAD_SAFE 1
#define MAGIC_FORCE_LOCKS 0
#define MAGIC_ATOMIC_OPERATIONS 1
#define MAGIC_LOCKFREE_DATASTRUCTURES 1
#define MAGIC_LOOKUP_CACHE_OPTIMIZATION 1

/* Magic lookup optimizations */
#define MAGIC_LOOKUP_SENTRY 1
#define MAGIC_LOOKUP_DSENTRY 1
#define MAGIC_LOOKUP_FUNCTION 1
#define MAGIC_LOOKUP_DFUNCTION 1
#define MAGIC_LOOKUP_TYPE 1
#define MAGIC_LOOKUP_SENTRY_ALLOW_RANGE_INDEX 1
#define MAGIC_LOOKUP_SENTRY_ALLOW_NAME_HASH 1
#define MAGIC_LOOKUP_FUNCTION_ALLOW_ADDR_HASH 1

/* Magic casting and type analysis */
#define MAGIC_INDEX_INT_CAST 1
#define MAGIC_INDEX_FUN_PTR_INT_CAST 1
#define MAGIC_INDEX_STR_PTR_INT_CAST 1
#define MAGIC_INDEX_VOID_PTR_INT_CAST 1
#define MAGIC_INDEX_OTH_PTR_INT_CAST 1
#define MAGIC_INDEX_BIT_CAST 1
#define MAGIC_INDEX_TRANSITIVE_BIT_CASTS 1
#define MAGIC_INDEX_FUN_PTR_BIT_CAST 1
#define MAGIC_INDEX_STR_PTR_BIT_CAST 1
#define MAGIC_INDEX_VOID_PTR_BIT_CAST 1
#define MAGIC_INDEX_OTH_PTR_BIT_CAST 1

#ifdef __MINIX
#define MAGIC_SKIP_TOVOID_PTR_BIT_CAST 0
#else
#define MAGIC_SKIP_TOVOID_PTR_BIT_CAST 1
#endif

/* Magic optimizations */
#define MAGIC_COMPACT_COMP_TYPES 1
#define MAGIC_OFF_BY_N_PROTECTION_N 1
#define MAGIC_VARSIZED_STRUCTS_SUPPORT 1
#define MAGIC_ALLOW_DEAD_DSENTRIES_DEFAULT 0
#define MAGIC_WALK_UNIONS_AS_VOID_ARRAYS_DEFAULT 1

/* Magic debug and output control macros */
#define MAGIC_DEBUG_SELECT(D, ND) (MAGIC_OUTPUT_CTL >= 1 ? (ND) : (D))
#define MAGIC_OUTPUT_SELECT(D, ND, NO)                                         \
  (MAGIC_OUTPUT_CTL >= 2 ? (NO) : MAGIC_DEBUG_SELECT(D, ND))
#define MAGIC_DEBUG_SET(D) MAGIC_DEBUG_SELECT(D, 0)
#define MAGIC_LLVM20_DEBUG_ENHANCED(D) (MAGIC_USE_ENHANCED_DEBUGGING ? (D) : 0)

/* Magic system integration */
#define MAGIC_ENABLED "_magic_enabled"
#define MAGIC_ARRAY_NAME "_magic_sentries_array"
#define MAGIC_TYPE_ARRAY_NAME "_magic_types_array"
#define MAGIC_FUNC_ARRAY_NAME "_magic_functions_array"
#define MAGIC_DSINDEX_ARRAY_NAME "_magic_dsindexes_array"
#define MAGIC_ROOT_VAR_NAME "_magic_vars_buff"

/* Magic function names */
#define MAGIC_ENTRY_POINT "main"
#define MAGIC_INIT_FUNC_NAME "magic_init"
#define MAGIC_DATA_INIT_FUNC_NAME "magic_data_init"
#define MAGIC_STACK_DSENTRIES_CREATE_FUNC_NAME "magic_stack_dsentries_create"
#define MAGIC_STACK_DSENTRIES_DESTROY_FUNC_NAME "magic_stack_dsentries_destroy"
#define MAGIC_GET_PAGE_SIZE_FUNC_NAME "magic_get_sys_pagesize"

/* Magic allocation naming */
#define MAGIC_ALLOC_NAME_SUFFIX "#"
#define MAGIC_ALLOC_NAME_SEP "%"
#define MAGIC_ALLOC_NONAME "%UNKNOWN"
#define MAGIC_ALLOC_EXT_NAME "%EXT"
#define MAGIC_ALLOC_EXT_PARENT_NAME "%EXT_PARENT"
#define MAGIC_ALLOC_RET_ADDR_NAME "%RET_ADDR"
#define MAGIC_ALLOC_INITIAL_STACK_NAME "%INITIAL_STACK_AREA"

/* Type IDs */
#define MAGIC_TYPE_VOID 1
#define MAGIC_TYPE_FLOAT 2
#define MAGIC_TYPE_INTEGER 3
#define MAGIC_TYPE_FUNCTION 4
#define MAGIC_TYPE_ARRAY 5
#define MAGIC_TYPE_ENUM 6
#define MAGIC_TYPE_VECTOR 7
#define MAGIC_TYPE_UNION 8
#define MAGIC_TYPE_STRUCT 9
#define MAGIC_TYPE_POINTER 10
#define MAGIC_TYPE_OPAQUE 11

/* LLVM-20 Enhanced type flags. */
#define MAGIC_TYPE_EXTERNAL 0x001
#define MAGIC_TYPE_IS_ROOT 0x002
#define MAGIC_TYPE_DYNAMIC 0x004
#define MAGIC_TYPE_INT_CAST 0x008
#define MAGIC_TYPE_STRICT_VALUE_SET 0x010
#define MAGIC_TYPE_VARSIZE 0x020
#define MAGIC_TYPE_UNSIGNED 0x040
#define MAGIC_TYPE_NO_INNER_PTRS 0x080
#define MAGIC_TYPE_LLVM20_ENHANCED 0x100
#define MAGIC_TYPE_ATOMIC_SAFE 0x200
#define MAGIC_TYPE_MEMORY_TAGGED 0x400

/* State flags for sentries and functions */
#define MAGIC_STATE_DIRTY 0x00000001
#define MAGIC_STATE_CONSTANT 0x00000002
#define MAGIC_STATE_DYNAMIC 0x00000004
#define MAGIC_STATE_DETACHED 0x00000008
#define MAGIC_STATE_DATA 0x00000010
#define MAGIC_STATE_HEAP 0x00000020
#define MAGIC_STATE_MAP 0x00000040
#define MAGIC_STATE_SHM 0x00000080
#define MAGIC_STATE_STACK 0x00000100
#define MAGIC_STATE_TEXT 0x00000200
#define MAGIC_STATE_LIB 0x00000400
#define MAGIC_STATE_LIB_SO 0x00000800
#define MAGIC_STATE_LIB_DSO 0x00001000
#define MAGIC_STATE_ADDR_NOT_TAKEN 0x00002000
#define MAGIC_STATE_EXT 0x00004000
#define MAGIC_STATE_OUT_OF_BAND 0x00008000
#define MAGIC_STATE_STRING 0x00010000
#define MAGIC_STATE_NAMED_STRING 0x00020000
#define MAGIC_STATE_MODE_R 0x00040000
#define MAGIC_STATE_MODE_W 0x00080000
#define MAGIC_STATE_MODE_X 0x00100000
#define MAGIC_STATE_THREAD_LOCAL 0x00200000
#define MAGIC_STATE_MEMPOOL 0x00400000
#define MAGIC_STATE_MEMBLOCK 0x00800000
#define MAGIC_STATE_EXTERNAL 0x01000000
#define MAGIC_STATE_TYPE_SIZE_MISMATCH 0x02000000
#define MAGIC_STATE_IMMUTABLE 0x04000000
#define MAGIC_STATE_INIT 0x08000000
#define MAGIC_STATE_DIRTY_PAGE 0x10000000
#define MAGIC_STATE_SKIP_BYTE_INDICES 0x20000000

/* LLVM-20 Enhanced state flags for sentries and functions. */
#define MAGIC_STATE_LLVM20_INSTRUMENTED 0x40000000
#define MAGIC_STATE_ENHANCED_TRACKING 0x80000000

/* Magic memory functions for LLVM-20 with enhanced MINIX support */
#define MAGIC_MEMA_FUNCS                                                       \
  __X(malloc), __X(calloc), __X(realloc), __X(reallocarray),                   \
      __X(posix_memalign), __X(valloc), __X(memalign), __X(aligned_alloc),     \
      __X(mmap), __X(brk), __X(sbrk), __X(shmat), __X(mmap64)

#ifdef __MINIX
#define MAGIC_MEMA_EXTRA_FUNCS , __X(vm_map_cacheblock), __X(vm_malloc_debug)
#define MAGIC_MEMN_FUNCS __X(mmap), __X(munmap), __X(vm_map_cacheblock)
#else
#define MAGIC_MEMA_EXTRA_FUNCS
#define MAGIC_MEMN_FUNCS ""
#endif

#define MAGIC_MEMD_FUNCS __X(free), __X(munmap), __X(shmdt)
#define MAGIC_DL_FUNCS __X(dlopen), __X(dlclose)

/* LLVM-20 Compatibility and feature detection */
#define MAGIC_LLVM20_FEATURE_DETECTION 1
#define MAGIC_ENHANCED_TYPE_ANALYSIS 1
#define MAGIC_IMPROVED_MEMORY_TRACKING 1
#define MAGIC_ATOMIC_MEMORY_OPERATIONS 1

/* Memory allocation tracking flags */
#define MAGIC_MEMA_FUNC_ALLOC_FLAGS                                            \
  MAGIC_STATE_HEAP, MAGIC_STATE_HEAP, MAGIC_STATE_HEAP, MAGIC_STATE_HEAP,      \
      MAGIC_STATE_HEAP, MAGIC_STATE_HEAP, MAGIC_STATE_HEAP, MAGIC_STATE_HEAP,  \
      MAGIC_STATE_MAP, MAGIC_STATE_HEAP, MAGIC_STATE_HEAP, MAGIC_STATE_SHM,    \
      MAGIC_STATE_MAP

/* LLVM-20 memory instrumentation settings */
#if MAGIC_INSTRUMENT_MEM_FUNCS_ASR_ONLY
#define MAGIC_MEM_PREFIX_STRS                                                  \
  MAGIC_ASR_PREFIX_STR, MAGIC_PREFIX_STR, "llvm20_", ""
#else
#define MAGIC_MEM_PREFIX_STRS MAGIC_PREFIX_STR, "llvm20_", ""
#endif

/* Version string for identification */
#define MAGIC_VERSION_STRING "MAGIC_LLVM20_MINIX318_2025"

#endif /* _MAGIC_COMMON_H */
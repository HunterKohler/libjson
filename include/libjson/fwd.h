/**
 * @file libjson/fwd.h
 *
 * JSON Forward Declarations
 */
#ifndef LIBJSON_FWD_H_
#define LIBJSON_FWD_H_

#include <limits.h>
#include <stddef.h>

typedef void *json_null;
typedef _Bool json_bool;
typedef long long json_int;
typedef unsigned long long json_uint;
typedef long double json_float;
typedef size_t json_size;

struct json_allocator;
struct json_array;
struct json_entry;
struct json_memory_resource;
struct json_object;
struct json_string;
struct json_value;
struct json_visitor;
struct json_write_options;
struct json_read_options;

#define JSON_INT_MAX LLONG_MAX
#define JSON_INT_MIN LLONG_MIN

#define JSON_UINT_MAX ULLONG_MAX

#define json_true ((json_bool)1)
#define json_false ((json_bool)0)

#endif

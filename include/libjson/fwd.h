/**
 * @file libjson/fwd.h
 *
 * JSON Forward Declarations
 */
#ifndef LIBJSON_FWD_H_
#define LIBJSON_FWD_H_

#include <limits.h>
#include <stddef.h>

/**
 * @defgroup Forward Forward
 * Forward declaration and library types.
 * @{
 */

typedef void *json_null;
typedef _Bool json_bool;
typedef long long json_int;
typedef unsigned long long json_uint;
typedef long double json_float;
typedef size_t json_size;

#define JSON_INT_MAX LLONG_MAX
#define JSON_INT_MIN LLONG_MIN

#define JSON_UINT_MAX ULLONG_MAX

#define json_true ((json_bool)1)
#define json_false ((json_bool)0)

/**
 * @}
 */

#endif

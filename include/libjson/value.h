/**
 * @file libjson/value.h
 *
 * JSON Value
 */
#ifndef LIBJSON_VALUE_H_
#define LIBJSON_VALUE_H_

#include <libjson/fwd.h>
#include <libjson/memory.h>
#include <libjson/type.h>

/**
 * @defgroup Value Value
 * JSON Value
 * @{
 */

/**
 * Represents a JSON value.
 *
 * This is a variant type of all JSON types. It tracks the current type,
 * allocates and manages the current value.
 */
struct json_value {
    /** @private */
    enum json_type _type;

    /** @private */
    struct {
        struct {
            union {
                json_null _null;
                json_bool _bool;
                json_int _int;
                json_float _float;
            } _value;

            struct json_allocator *_alloc;
        } _primitive;
        struct json_string *_string;
        struct json_array *_array;
        struct json_object *_object;
    } _data;
};

void json_value_construct(
    struct json_value *value, struct json_allocator *alloc);

void json_value_construct_null(
    struct json_value *value, struct json_allocator *alloc);

void json_value_construct_bool(struct json_value *value, json_bool bool_value,
                               struct json_allocator *alloc);

void json_value_construct_int(struct json_value *value, json_int int_value,
                              struct json_allocator *alloc);

void json_value_construct_float(
    struct json_value *value, json_float float_value,
    struct json_allocator *alloc);

enum json_errc json_value_construct_string_copy(
    struct json_value *value, const struct json_string *string_value,
    struct json_allocator *alloc);

enum json_errc json_value_construct_string_move(
    struct json_value *value, struct json_string *string_value,
    struct json_allocator *alloc);

enum json_errc json_value_construct_array_copy(
    struct json_value *value, const struct json_array *array_value,
    struct json_allocator *alloc);

enum json_errc json_value_construct_array_move(
    struct json_value *value, struct json_array *array_value,
    struct json_allocator *alloc);

enum json_errc json_value_construct_object_copy(
    struct json_value *value, const struct json_object *object_value,
    struct json_allocator *alloc);

enum json_errc json_value_construct_object_move(
    struct json_value *value, struct json_object *object_value,
    struct json_allocator *alloc);

enum json_errc json_value_construct_copy(
    struct json_value *value, const struct json_value *other,
    struct json_allocator *alloc);

enum json_errc json_value_construct_move(
    struct json_value *value, struct json_value *other,
    struct json_allocator *alloc);

void json_value_destruct(struct json_value *value);

struct json_allocator *json_value_get_allocator(
    const struct json_value *value);

enum json_type json_value_type(const struct json_value *value);

json_bool json_value_is_null(const struct json_value *value);

json_bool json_value_is_bool(const struct json_value *value);

json_bool json_value_is_int(const struct json_value *value);

json_bool json_value_is_float(const struct json_value *value);

json_bool json_value_is_string(const struct json_value *value);

json_bool json_value_is_array(const struct json_value *value);

json_bool json_value_is_object(const struct json_value *value);

void json_value_assign_null(struct json_value *value);

void json_value_assign_bool(struct json_value *value, json_bool new_value);

void json_value_assign_int(struct json_value *value, json_int new_value);

void json_value_assign_float(struct json_value *value, json_float new_value);

enum json_errc json_value_assign_string_copy(
    struct json_value *value, const struct json_string *new_value);

enum json_errc json_value_assign_string_move(
    struct json_value *value, struct json_string *new_value);

enum json_errc json_value_assign_array_copy(
    struct json_value *value, const struct json_array *new_value);

enum json_errc json_value_assign_array_move(
    struct json_value *value, struct json_array *new_value);

enum json_errc json_value_assign_object_copy(
    struct json_value *value, const struct json_object *new_value);

enum json_errc json_value_assign_object_move(
    struct json_value *value, struct json_object *new_value);

enum json_errc json_value_assign_copy(
    struct json_value *value, const struct json_value *other);

enum json_errc json_value_assign_move(
    struct json_value *value, struct json_value *other);

json_bool *json_value_as_bool(struct json_value *value);

json_int *json_value_as_int(struct json_value *value);

json_float *json_value_as_float(struct json_value *value);

struct json_string *json_value_as_string(struct json_value *value);

struct json_array *json_value_as_array(struct json_value *value);

struct json_object *json_value_as_object(struct json_value *value);

struct json_value *json_value_new(struct json_allocator *alloc);

struct json_value *json_value_new_null(struct json_allocator *alloc);

struct json_value *json_value_new_bool(
    json_bool value, struct json_allocator *alloc);

struct json_value *json_value_new_int(
    json_int value, struct json_allocator *alloc);

struct json_value *json_value_new_float(
    json_float value, struct json_allocator *alloc);

struct json_value *json_value_new_string_copy(
    const struct json_string *value, struct json_allocator *alloc);

struct json_value *json_value_new_string_move(
    struct json_string *value, struct json_allocator *alloc);

struct json_value *json_value_new_array_copy(
    const struct json_array *value, struct json_allocator *alloc);

struct json_value *json_value_new_array_move(
    struct json_array *value, struct json_allocator *alloc);

struct json_value *json_value_new_object_copy(
    const struct json_object *value, struct json_allocator *alloc);

struct json_value *json_value_new_object_move(
    struct json_object *value, struct json_allocator *alloc);

struct json_value *json_value_new_copy(
    const struct json_value *other, struct json_allocator *alloc);

struct json_value *json_value_new_move(
    struct json_value *other, struct json_allocator *alloc);

void json_value_delete(struct json_value *value);

/**
 * @}
 */

#endif

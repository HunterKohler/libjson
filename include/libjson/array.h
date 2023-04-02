/**
 * @file libjson/array.h
 *
 * JSON Array
 */
#ifndef LIBJSON_ARRAY_H_
#define LIBJSON_ARRAY_H_

#include <libjson/fwd.h>
#include <libjson/memory.h>

/**
 * @defgroup Array Array
 * JSON Array
 * @{
 */

struct json_array {
    /** @private */
    json_size _size;

    /** @private */
    json_size _capacity;

    /** @private */
    struct json_value *_data;

    /** @private */
    struct json_allocator *_alloc;
};

void json_array_construct(
    struct json_array *array, struct json_allocator *alloc);

enum json_errc json_array_construct_copy(
    struct json_array *array, const struct json_array *other,
    struct json_allocator *alloc);

enum json_errc json_array_construct_move(
    struct json_array *array, struct json_array *other,
    struct json_allocator *alloc);

void json_array_destruct(struct json_array *array);

enum json_errc json_array_assign_copy(
    struct json_array *array, const struct json_array *other);

enum json_errc json_array_assign_move(
    struct json_array *array, struct json_array *other);

struct json_allocator *json_array_get_allocator(
    const struct json_array *array);

void json_array_clear(struct json_array *array);

json_bool json_array_empty(const struct json_array *array);

json_size json_array_size(const struct json_array *array);

json_size json_array_capacity(const struct json_array *array);

void json_array_pop_back(struct json_array *array);

enum json_errc json_array_push_back_copy(
    struct json_array *array, const struct json_value *value);

enum json_errc json_array_push_back_move(
    struct json_array *array, struct json_value *value);

enum json_errc json_array_emplace_back(
    struct json_array *array, struct json_allocator *alloc);

enum json_errc json_array_emplace_back_null(
    struct json_array *array, struct json_allocator *alloc);

enum json_errc json_array_emplace_back_bool(
    struct json_array *array, json_bool value, struct json_allocator *alloc);

enum json_errc json_array_emplace_back_int(
    struct json_array *array, json_int value, struct json_allocator *alloc);

enum json_errc json_array_emplace_back_float(
    struct json_array *array, json_float value, struct json_allocator *alloc);

enum json_errc json_array_emplace_back_string_copy(
    struct json_array *array, const struct json_string *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_back_string_move(
    struct json_array *array, const struct json_string *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_back_array_copy(
    struct json_array *array, const struct json_array *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_back_array_move(
    struct json_array *array, const struct json_array *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_back_object_copy(
    struct json_array *array, const struct json_object *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_back_object_move(
    struct json_array *array, const struct json_object *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_back_copy(
    struct json_array *array, const struct json_value *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_back_move(
    struct json_array *array, struct json_value *value,
    struct json_allocator *alloc);

enum json_errc json_array_insert_copy(
    struct json_array *array, json_size pos, const struct json_value *value);

enum json_errc json_array_insert_move(
    struct json_array *array, json_size pos, struct json_value *value);

enum json_errc json_array_emplace(struct json_array *array, json_size pos);

enum json_errc json_array_emplace_null(
    struct json_array *array, json_size pos, struct json_allocator *alloc);

enum json_errc json_array_emplace_bool(
    struct json_array *array, json_size pos, json_bool value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_int(
    struct json_array *array, json_size pos, json_int value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_float(
    struct json_array *array, json_size pos, json_float value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_string_copy(
    struct json_array *array, json_size pos, const struct json_string *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_string_move(
    struct json_array *array, json_size pos, const struct json_string *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_array_copy(
    struct json_array *array, json_size pos, const struct json_array *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_array_move(
    struct json_array *array, json_size pos, const struct json_array *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_object_copy(
    struct json_array *array, json_size pos, const struct json_object *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_object_move(
    struct json_array *array, json_size pos, const struct json_object *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_copy(
    struct json_array *array, json_size pos, const struct json_value *value,
    struct json_allocator *alloc);

enum json_errc json_array_emplace_move(
    struct json_array *array, json_size pos, struct json_value *value,
    struct json_allocator *alloc);

void json_array_erase(
    struct json_array *array, json_size pos, json_size count);

enum json_errc json_array_shrink_to_fit(struct json_array *array);

enum json_errc json_array_reserve(struct json_array *array, json_size n);

enum json_errc json_array_resize(
    struct json_array *array, json_size n, const struct json_value *value);

struct json_value *json_array_front(struct json_array *array);

struct json_value *json_array_back(struct json_array *array);

struct json_value *json_array_at(struct json_array *array, json_size pos);

struct json_value *json_array_data(struct json_array *array);

void json_array_swap(struct json_array *a, struct json_array *b);

struct json_array *json_array_new(struct json_allocator *alloc);

struct json_array *json_array_new_copy(
    const struct json_array *other, struct json_allocator *alloc);

struct json_array *json_array_new_move(
    struct json_array *other, struct json_allocator *alloc);

void json_array_delete(struct json_array *array);

/**
 * @}
 */

#endif

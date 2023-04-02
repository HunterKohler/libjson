/**
 * @file libjson/object.h
 *
 * JSON Object
 */
#ifndef LIBJSON_OBJECT_H_
#define LIBJSON_OBJECT_H_

#include <libjson/fwd.h>
#include <libjson/memory.h>

/**
 * @defgroup Object Object
 * JSON Object
 * @{
 */

/**
 * Represents a JSON object.
 */
struct json_object {
    /** @private */
    json_size _size;

    /** @private */
    json_size _bucket_count;

    /** @private */
    struct json_bucket *_buckets;

    /** @private */
    struct json_allocator *_alloc;
};

struct json_object_iter {
    /** @private */
    struct json_object *_object;

    /** @private */
    struct json_entry *_entry;

    /** @private */
    json_size _pos;
};

void json_object_construct(
    struct json_object *object, struct json_allocator *alloc);

enum json_errc json_object_construct_copy(
    struct json_object *object, const struct json_object *other,
    struct json_allocator *alloc);

enum json_errc json_object_construct_move(
    struct json_object *object, struct json_object *other,
    struct json_allocator *alloc);

void json_object_destruct(struct json_object *object);

enum json_errc json_object_assign_copy(
    struct json_object *object, const struct json_object *other);

enum json_errc json_object_assign_move(
    struct json_object *object, struct json_object *other);

struct json_allocator *json_object_get_allocator(
    const struct json_object *object);

void json_object_begin(
    struct json_object *object, struct json_object_iter *iter);

void json_object_end(
    struct json_object *object, struct json_object_iter *iter);

json_bool json_object_empty(const struct json_object *object);

json_size json_object_size(const struct json_object *object);

void json_object_clear(struct json_object *object);

enum json_errc json_object_reserve(json_size n);

void json_object_swap(struct json_object *a, struct json_object *b);

json_bool json_object_contains(
    const struct json_object *object, const char *key, json_size n);

struct json_value *json_object_at(
    struct json_object *object, const char *key, json_size n);

void json_object_find(struct json_object *object, const char *key, json_size n,
                      struct json_object_iter *iter);

json_bool json_object_insert_copy(
    struct json_object *object, const char *key, json_size n,
    const struct json_value *value, struct json_object_iter *it);

json_bool json_object_insert_move(
    struct json_object *object, const char *key, json_size n,
    struct json_value *value, struct json_object_iter *it);

json_bool json_object_emplace(struct json_object *object, const char *key,
                              json_size n, struct json_allocator *alloc);

json_bool json_object_emplace_null(
    struct json_object *object, const char *key, json_size n,
    struct json_allocator *alloc);

json_bool json_object_emplace_bool(
    struct json_object *object, const char *key, json_size n, json_bool value,
    struct json_allocator *alloc);

json_bool json_object_emplace_int(
    struct json_object *object, const char *key, json_size n, json_int value,
    struct json_allocator *alloc);

json_bool json_object_emplace_float(
    struct json_object *object, const char *key, json_size n, json_float value,
    struct json_allocator *alloc);

json_bool json_object_emplace_string_copy(
    struct json_object *object, const char *key, json_size n,
    const struct json_string *value, struct json_allocator *alloc);

json_bool json_object_emplace_string_move(
    struct json_object *object, const char *key, json_size n,
    struct json_string *value, struct json_allocator *alloc);

json_bool json_object_emplace_array_copy(
    struct json_object *object, const char *key, json_size n,
    const struct json_array *value, struct json_allocator *alloc);

json_bool json_object_emplace_array_move(
    struct json_object *object, const char *key, json_size n,
    struct json_array *value, struct json_allocator *alloc);

json_bool json_object_emplace_object_copy(
    struct json_object *object, const char *key, json_size n,
    const struct json_object *value, struct json_allocator *alloc);

json_bool json_object_emplace_object_move(
    struct json_object *object, const char *key, json_size n,
    struct json_object *value, struct json_allocator *alloc);

json_bool json_object_emplace_copy(
    struct json_object *object, const char *key, json_size n,
    const struct json_value *value, struct json_allocator *alloc);

json_bool json_object_emplace_move(
    struct json_object *object, const char *key, json_size n,
    struct json_value *value, struct json_allocator *alloc);

struct json_object *json_object_new(struct json_allocator *alloc);

struct json_object *json_object_new_copy(
    const struct json_object *object, struct json_allocator *alloc);

struct json_object *json_object_new_move(
    struct json_object *object, struct json_allocator *alloc);

void json_object_delete(struct json_object *object);

struct json_entry *json_object_iter_entry(struct json_object_iter *iter);

struct json_string *json_object_iter_key(struct json_object_iter *iter);

struct json_value *json_object_iter_value(struct json_object_iter *iter);

void json_object_iter_next(struct json_object_iter *iter);

json_bool json_object_iter_done(const struct json_object_iter *a);

json_bool json_object_iter_is_equal(
    const struct json_object_iter *iter, const struct json_object_iter *other);

/**
 * @}
 */

#endif

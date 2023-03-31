#include <libjson/entry.h>
#include <libjson/fwd.h>
#include <libjson/object.h>
#include <libjson/value.h>
#include "./bucket.h"
#include "./util.h"

static struct json_bucket *json_object_find_bucket(
    struct json_object *object, const char *key, json_size n)
{
    json_uint64 pos = json_hash(key, n) & (object->_bucket_count - 1);
    return object->_buckets + pos;
}

static struct json_entry *json_object_find_bucket_entry(
    struct json_bucket *bucket, const char *key, json_size n)
{
    struct json_entry *entry = bucket->_first;

    for (; entry; entry = entry->_next) {
        if (n == entry->_key._impl->_size &&
            !memcmp(key, entry->_key._impl->_data, n)) {
            break;
        }
    }

    return entry;
}

static struct json_entry *json_object_find_entry(
    struct json_object *object, const char *key, json_size n)
{
    return json_object_find_bucket_entry(
        json_object_find_bucket(object, key, n), key, n);
}

static struct json_bucket *json_object_next_bucket(
    struct json_object *object, json_size pos)
{
    for (; pos < object->_bucket_count; ++pos) {
        if (object->_buckets[pos]._first) {
            return object->_buckets + pos;
        }
    }

    return NULL;
}

void json_object_construct(
    struct json_object *object, struct json_allocator *alloc)
{
    object->_alloc = alloc ? alloc : json_get_default_allocator();
    object->_size = 0;
    object->_buckets = NULL;
    object->_bucket_count = 0;
}

enum json_errc json_object_construct_copy(
    struct json_object *object, const struct json_object *other,
    struct json_allocator *alloc);

enum json_errc json_object_construct_move(
    struct json_object *object, struct json_object *other,
    struct json_allocator *alloc)
{
    if (!alloc) {
        alloc = other->_alloc;
    } else if (!json_allocator_is_equal(alloc, other->_alloc)) {
        return json_object_construct_copy(object, other, alloc);
    }

    object->_alloc = alloc;
    object->_size = other->_size;
    object->_buckets = other->_buckets;
    object->_bucket_count = other->_bucket_count;
    other->_size = 0;
    other->_buckets = NULL;
    other->_bucket_count = 0;

    return JSON_ERRC_OK;
}

void json_object_destruct(struct json_object *object)
{
    for (json_size pos = 0; pos < object->_bucket_count; ++pos) {
        for (struct json_entry *entry = object->_buckets[pos]._first; entry;) {
            struct json_entry *next = entry->_next;

            json_entry_delete(entry, object->_alloc);
            entry = next;
        }
    }

    json_deallocate_entries(
        object->_alloc, object->_buckets, object->_bucket_count);
}

enum json_errc json_object_assign_copy(
    struct json_object *object, const struct json_object *other);

enum json_errc json_object_assign_move(
    struct json_object *object, struct json_object *other);

struct json_allocator *json_object_get_allocator(
    const struct json_object *object)
{
    return object->_alloc;
}

void json_object_begin(
    struct json_object *object, struct json_object_iter *iter)
{
    struct json_bucket *bucket = json_object_next_bucket(object, 0);

    if (bucket->_first) {
        iter->_entry = bucket->_first;
        iter->_pos = bucket - object->_buckets;
        iter->_object = object;
    } else {
        json_object_end(object, iter);
    }
}

void json_object_end(struct json_object *object, struct json_object_iter *iter)
{
    iter->_entry = NULL;
    iter->_pos = object->_bucket_count;
    iter->_object = object;
}

json_bool json_object_empty(const struct json_object *object)
{
    return !object->_size;
}

json_size json_object_size(const struct json_object *object)
{
    return object->_size;
}

void json_object_clear(struct json_object *object)
{
    for (json_size pos = 0; pos < object->_bucket_count; ++pos) {
        for (struct json_entry *entry = object->_buckets[pos]._first; entry;
             entry = entry->_next) {
            json_entry_delete(entry, object->_alloc);
        }
    }

    object->_size = 0;
}

// enum json_errc json_object_reserve(json_size n);

void json_object_swap(struct json_object *object, struct json_object *other)
{
    json_size size = object->_size;
    json_size bucket_count = object->_bucket_count;
    struct json_bucket *buckets = object->_buckets;

    object->_size = other->_size;
    object->_bucket_count = other->_bucket_count;
    object->_buckets = other->_buckets;
    other->_size = size;
    other->_bucket_count = bucket_count;
    other->_buckets = buckets;
}

json_bool json_object_contains(
    const struct json_object *object, const char *key, json_size n)
{
    return json_object_find_entry(object, key, n);
}

struct json_value *json_object_at(
    struct json_object *object, const char *key, json_size n)
{
    return &json_object_find_entry(object, key, n)->_value;
}

void json_object_find(struct json_object *object, const char *key, json_size n,
                      struct json_object_iter *iter)
{
    struct json_bucket *bucket = json_object_find_bucket(object, key, n);
    struct json_entry *entry = json_object_find_bucket_entry(bucket, key, n);

    if (entry) {
        iter->_object = object;
        iter->_pos = bucket - object->_buckets;
        iter->_entry = json_object_find_bucket_entry(entry, key, n);
    } else {
        json_object_end(object, iter);
    }
}

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

struct json_object *json_object_new(struct json_allocator *alloc)
{
    struct json_object *object = json_allocate_objects(alloc, 1);

    if (object) {
        json_object_construct(object, alloc);
    }

    return object;
}

struct json_object *json_object_new_copy(
    const struct json_object *other, struct json_allocator *alloc)
{
    alloc = alloc ? alloc : other->_alloc;
    struct json_object *object = json_allocate_objects(alloc, 1);

    if (object) {
        if (json_object_construct_copy(object, other, alloc)) {
            json_deallocate_objects(alloc, object, 1);
            return NULL;
        }
    }

    return object;
}

struct json_object *json_object_new_move(
    struct json_object *other, struct json_allocator *alloc)
{
    alloc = alloc ? alloc : other->_alloc;
    struct json_object *object = json_allocate_objects(alloc, 1);

    if (object) {
        if (json_object_construct_move(object, other, alloc)) {
            json_deallocate_objects(alloc, object, 1);
            return NULL;
        }
    }

    return object;
}

void json_object_delete(struct json_object *object)
{
    struct json_allocator *alloc = object->_alloc;

    json_object_destruct(object);
    json_deallocate_objects(alloc, object, 1);
}

struct json_entry *json_object_iter_entry(struct json_object_iter *iter)
{
    return iter->_entry;
}

struct json_string *json_object_iter_key(struct json_object_iter *iter)
{
    return &iter->_entry->_key;
}

struct json_value *json_object_iter_value(struct json_object_iter *iter)
{
    return &iter->_entry->_value;
}

void json_object_iter_next(struct json_object_iter *iter)
{
    if (!iter->_entry) {
        return;
    } else if (iter->_entry->_next) {
        iter->_entry = iter->_entry->_next;
    } else {
        struct json_bucket *bucket =
            json_object_next_bucket(iter->_object, iter->_pos + 1);

        if (bucket) {
            iter->_entry = bucket->_first;
            iter->_pos = bucket - iter->_object->_buckets;
        } else {
            iter->_entry = NULL;
            iter->_pos = iter->_object->_bucket_count - 1;
        }
    }
}

json_bool json_object_iter_is_equal(
    const struct json_object_iter *iter, const struct json_object_iter *other)
{
    return iter->_entry == other->_entry;
}

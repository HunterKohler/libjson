/**
 * @file libjson/string.h
 *
 * JSON String
 */
#ifndef LIBJSON_STRING_H_
#define LIBJSON_STRING_H_

#include <libjson/errc.h>
#include <libjson/fwd.h>

/**
 * @defgroup String String
 * @{
 */

struct json_string {
    /** @private */
    struct json_allocator *_alloc;

    /** @private */
    struct json_string_impl {
        json_size _size;
        json_size _capacity;
        char _data[];
    } *_impl;
};

void json_string_construct(
    struct json_string *string, struct json_allocator *alloc);

enum json_errc json_string_construct_copy(
    struct json_string *string, const struct json_string *other,
    struct json_allocator *alloc);

enum json_errc json_string_construct_move(
    struct json_string *string, struct json_string *other,
    struct json_allocator *alloc);

void json_string_destruct(struct json_string *string);

enum json_errc json_string_assign_copy(
    struct json_string *string, const struct json_string *other);

enum json_errc json_string_assign_move(
    struct json_string *string, struct json_string *other);

struct json_allocator *json_string_get_allocator(
    const struct json_string *string);

json_bool json_string_empty(const struct json_string *string);

json_size json_string_size(const struct json_string *string);

json_size json_string_capacity(const struct json_string *string);

void json_string_clear(struct json_string *string);

enum json_errc json_string_reserve(struct json_string *string, json_size n);

enum json_errc json_string_resize(
    struct json_string *string, json_size new_size, char c);

enum json_errc json_string_shrink_to_fit(struct json_string *string);

char *json_string_front(struct json_string *string);

char *json_string_back(struct json_string *string);

char *json_string_at(struct json_string *string, json_size pos);

char *json_string_data(struct json_string *string);

void json_string_swap(struct json_string *a, struct json_string *b);

int json_string_compare(
    const struct json_string *a, const struct json_string *b);

void json_string_copy(const struct json_string *string, json_size start,
                      json_size count, char *dest);

void json_string_pop_back(struct json_string *string);

enum json_errc json_string_push_back(struct json_string *string, char c);

enum json_errc json_string_append(
    struct json_string *string, const char *src, json_size count);

enum json_errc json_string_insert(
    struct json_string *string, json_size pos, const char *src,
    json_size count);

enum json_errc json_string_erase(
    struct json_string *string, json_size pos, json_size count);

struct json_string *json_string_new(struct json_allocator *alloc);

struct json_string *json_string_new_copy(
    const struct json_string *string, struct json_allocator *alloc);

struct json_string *json_string_new_move(
    struct json_string *string, struct json_allocator *alloc);

void json_string_delete(struct json_string *string);

/**
 * @}
 */

#endif

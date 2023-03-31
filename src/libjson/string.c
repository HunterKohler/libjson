#include <string.h>
#include <libjson/errc.h>
#include <libjson/fwd.h>
#include <libjson/string.h>
#include "./util.h"

static struct {
    json_size size;
    json_size capacity;
    char data;
} json_string_impl_null = {};

static struct json_string_impl *json_string_impl_new(
    json_size size, json_size capacity, struct json_allocator *alloc)
{
    struct json_string_impl *impl = json_allocator_allocate(
        alloc, sizeof(*impl) + capacity + 1, _Alignof(*impl));

    if (impl) {
        impl->_size = size;
        impl->_capacity = capacity;
    }

    return impl;
}

static void json_string_impl_delete(
    struct json_string_impl *impl, struct json_allocator *alloc)
{
    json_allocator_deallocate(
        alloc, impl, sizeof(*impl) + impl->_capacity + 1, _Alignof(*impl));
}

static void json_string_set_null(struct json_string *string)
{
    string->_impl = (void *)&json_string_impl_null;
}

void json_string_construct(
    struct json_string *string, struct json_allocator *alloc)
{
    string->_alloc = alloc ? alloc : json_get_default_allocator();
    json_string_set_null(string);
}

enum json_errc json_string_construct_copy(
    struct json_string *string, const struct json_string *other,
    struct json_allocator *alloc)
{
    string->_alloc = alloc ? alloc : other->_alloc;

    if (other->_impl->_size) {
        string->_impl = json_string_impl_new(
            other->_impl->_size, other->_impl->_size, string->_alloc);

        if (!string->_impl) {
            return JSON_ERRC_NOT_ENOUGH_MEMORY;
        }

        memcpy(string->_impl->_data, other->_impl->_data,
               other->_impl->_size + 1);
    } else {
        json_string_set_null(string);
    }

    return JSON_ERRC_OK;
}

enum json_errc json_string_construct_move(
    struct json_string *string, struct json_string *other,
    struct json_allocator *alloc)
{
    if (!alloc) {
        alloc = other->_alloc;
    } else if (!json_allocator_is_equal(alloc, other->_alloc)) {
        return json_string_construct_copy(string, other, alloc);
    }

    string->_impl = other->_impl;
    json_string_set_null(other);

    return JSON_ERRC_OK;
}

void json_string_destruct(struct json_string *string)
{
    if (string->_impl->_capacity) {
        json_string_impl_delete(string->_impl, string->_alloc);
    }
}

enum json_errc json_string_assign_copy(
    struct json_string *string, const struct json_string *other)
{
    if (json_string_reserve(string, other->_impl->_size)) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    memcpy(string->_impl->_data, other->_impl->_data, other->_impl->_size + 1);
    string->_impl->_size = other->_impl->_size;
    return JSON_ERRC_NOT_ENOUGH_MEMORY;
}

enum json_errc json_string_assign_move(
    struct json_string *string, struct json_string *other)
{
    if (!json_allocator_is_equal(string->_alloc, other->_alloc)) {
        return json_string_assign_copy(string, other);
    }

    json_string_impl_delete(string->_impl, string->_alloc);
    string->_impl = other->_impl;
    json_string_set_null(other);
    return JSON_ERRC_OK;
}

struct json_allocator *json_string_get_allocator(
    const struct json_string *string)
{
    return string->_alloc;
}

json_bool json_string_empty(const struct json_string *string)
{
    return !string->_impl->_size;
}

json_size json_string_size(const struct json_string *string)
{
    return string->_impl->_size;
}

json_size json_string_capacity(const struct json_string *string)
{
    return string->_impl->_capacity;
}

void json_string_clear(struct json_string *string)
{
    if (string->_impl->_size) {
        string->_impl->_size = 0;
        string->_impl->_data[0] = 0;
    }
}

enum json_errc json_string_reserve(struct json_string *string, json_size n)
{
    if (n > string->_impl->_capacity) {
        struct json_string_impl *new_impl = json_string_impl_new(
            string->_impl->_size, string->_impl->_size, string->_alloc);

        if (!new_impl) {
            return JSON_ERRC_NOT_ENOUGH_MEMORY;
        }

        memcpy(
            new_impl->_data, string->_impl->_data, string->_impl->_size + 1);
        json_string_impl_delete(string->_impl, string->_alloc);
        string->_impl = new_impl;
    }

    return JSON_ERRC_OK;
}

enum json_errc json_string_resize(
    struct json_string *string, json_size new_size, char c)
{
    if (string->_impl->_size < new_size) {
        if (json_string_reserve(string, new_size)) {
            return JSON_ERRC_NOT_ENOUGH_MEMORY;
        }

        memset(string->_impl->_data, c, new_size);
        string->_impl->_data[new_size] = 0;
        string->_impl->_size = new_size;
    } else if (string->_impl->_size < new_size) {
        string->_impl->_size = new_size;
        string->_impl->_data[new_size] = 0;
    }

    return JSON_ERRC_OK;
}

enum json_errc json_string_shrink_to_fit(struct json_string *string)
{
    if (string->_impl->_size < string->_impl->_capacity) {
        if (string->_impl->_size) {
            struct json_string_impl *new_impl = json_string_impl_new(
                string->_impl->_size, string->_impl->_size, string->_alloc);

            if (!new_impl) {
                return JSON_ERRC_NOT_ENOUGH_MEMORY;
            }

            memcpy(new_impl->_data, string->_impl->_data,
                   string->_impl->_size + 1);
            json_string_impl_delete(string->_impl, string->_alloc);
            string->_impl = new_impl;
        } else {
            json_string_impl_delete(string->_impl, string->_alloc);
            json_string_set_null(string);
        }
    }

    return JSON_ERRC_OK;
}

char *json_string_front(struct json_string *string)
{
    return string->_impl->_data;
}

char *json_string_back(struct json_string *string)
{
    return string->_impl->_data + string->_impl->_size - 1;
}

char *json_string_at(struct json_string *string, json_size pos)
{
    return string->_impl->_data + pos;
}

char *json_string_data(struct json_string *string)
{
    return string->_impl->_data;
}

void json_string_swap(struct json_string *string, struct json_string *other)
{
    struct json_string_impl *impl = string->_impl;

    string->_impl = other->_impl;
    other->_impl = impl;
}

int json_string_compare(
    const struct json_string *string, const struct json_string *other)
{
    json_size n = string->_impl->_size;
    json_size m = string->_impl->_size;
    int cmp = memcmp(string->_impl->_data, other->_impl->_data, n < m ? n : m);

    return cmp ? cmp : json_compare_int(n, m);
}

void json_string_copy(const struct json_string *string, json_size start,
                      json_size count, char *dest)
{
    memcpy(dest, string->_impl->_data + start, count);
}

void json_string_pop_back(struct json_string *string)
{
    --string->_impl->_size;
}

enum json_errc json_string_push_back(struct json_string *string, char c)
{
    enum json_errc ec = json_string_reserve(string, string->_impl->_size + 1);

    if (ec) {
        return ec;
    }

    string->_impl->_data[string->_impl->_size] = c;
    return JSON_ERRC_OK;
}

enum json_errc json_string_append(
    struct json_string *string, const char *src, json_size count)
{
    enum json_errc ec =
        json_string_reserve(string, string->_impl->_size + count);

    if (ec) {
        return ec;
    }

    memcpy(string->_impl->_data + string->_impl->_size, src, count);
    return JSON_ERRC_OK;
}

enum json_errc json_string_insert(
    struct json_string *string, json_size pos, const char *src,
    json_size count);

enum json_errc json_string_erase(
    struct json_string *string, json_size pos, json_size count);

struct json_string *json_string_new(struct json_allocator *alloc)
{
    struct json_string *string;

    alloc = alloc ? alloc : json_get_default_allocator();
    string = json_allocate_strings(alloc, 1);

    if (string) {
        json_string_construct(string, alloc);
    }

    return string;
}

struct json_string *json_string_new_copy(
    const struct json_string *other, struct json_allocator *alloc)
{
    struct json_string *string;

    alloc = alloc ? alloc : other->_alloc;
    string = json_allocate_strings(alloc, 1);

    if (string) {
        json_string_construct_copy(string, other, alloc);
    }

    return string;
}

struct json_string *json_string_new_move(
    struct json_string *other, struct json_allocator *alloc)
{
    struct json_string *string;

    alloc = alloc ? alloc : other->_alloc;
    string = json_allocate_strings(alloc, 1);

    if (string) {
        json_string_construct_move(string, other, alloc);
    }

    return string;
}

void json_string_delete(struct json_string *string)
{
    struct json_allocator *alloc = string->_alloc;

    json_string_destruct(string);
    json_deallocate_strings(alloc, string, 1);
}

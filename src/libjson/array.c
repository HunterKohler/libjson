#include <libjson/array.h>
#include <libjson/errc.h>
#include <libjson/fwd.h>
#include "./util.h"

void json_array_construct(
    struct json_array *array, struct json_allocator *alloc)
{
    array->_alloc = alloc;
    array->_capacity = 0;
    array->_size = 0;
    array->_data = NULL;
}

enum json_errc json_array_construct_copy(
    struct json_array *array, const struct json_array *other,
    struct json_allocator *alloc)
{
    array->_alloc = alloc ? alloc : other->_alloc;
    array->_size = other->_size;
    array->_capacity = other->_size;

    if (array->_size) {
        array->_data = json_allocate_values(alloc, array->_size);

        if (!array->_data) {
            return JSON_ERRC_NOT_ENOUGH_MEMORY;
        }

        for (json_size i = 0; i < array->_size; i++) {
            json_value_construct_copy(
                array->_data + i, other->_data + i, array->_alloc);
        }
    } else {
        array->_data = NULL;
    }
}

enum json_errc json_array_construct_move(
    struct json_array *array, struct json_array *other,
    struct json_allocator *alloc)
{
    if (!alloc) {
        array->_alloc = other->_alloc;
    } else if (!json_allocator_is_equal(alloc, other->_alloc)) {
        return json_array_construct_copy(array, other, alloc);
    } else {
        array->_alloc = alloc;
    }

    array->_data = other->_data;
    array->_size = other->_size;
    array->_capacity = other->_capacity;
    other->_data = NULL;
    other->_size = 0;
    other->_capacity = 0;
}

void json_array_destruct(struct json_array *array)
{
    for (json_size i = 0; i < array->_size; i++) {
        json_value_destruct(array->_data + i);
    }

    json_deallocate_values(array->_alloc, array->_data, array->_capacity);
}

enum json_errc json_array_assign_copy(
    struct json_array *array, const struct json_array *other)
{
    enum json_errc ec = json_array_resize(array, other->_size, NULL);

    if (ec) {
        return ec;
    }

    for (json_size i = 0; i < other->_size; i++) {
        json_value_assign_copy(array->_data + i, other->_data + i);
    }

    array->_size = other->_size;
}

enum json_errc json_array_assign_move(
    struct json_array *array, struct json_array *other)
{
    enum json_errc ec = json_array_resize(array, other->_size, NULL);

    if (ec) {
        return ec;
    }

    for (json_size i = 0; i < other->_size; i++) {
        json_value_assign_move(array->_data + i, other->_data + i);
    }

    array->_size = other->_size;
}

struct json_allocator *json_array_get_allocator(const struct json_array *array)
{
    return array->_alloc;
}

void json_array_clear(struct json_array *array)
{
    for (json_size i = 0; i < array->_size; i++) {
        json_value_destruct(array->_data + i);
    }

    array->_size = 0;
}

json_bool json_array_empty(const struct json_array *array)
{
    return !array->_size;
}

json_size json_array_size(const struct json_array *array)
{
    return array->_size;
}

json_size json_array_capacity(const struct json_array *array)
{
    return array->_capacity;
}

void json_array_pop_back(struct json_array *array)
{
    json_value_destruct(array->_data + --array->_size);
}

enum json_errc json_array_push_back_copy(
    struct json_array *array, const struct json_value *value)
{
    enum json_errc ec;

    if ((ec = json_array_reserve(array, array->_size + 1)) ||
        (ec = json_value_construct_copy(
             array->_data + array->_size, value, array->_alloc))) {
        return ec;
    }

    ++array->_size;
    return JSON_ERRC_OK;
}

enum json_errc json_array_push_back_move(
    struct json_array *array, struct json_value *value)
{
    enum json_errc ec;

    if ((ec = json_array_reserve(array, array->_size + 1)) ||
        (ec = json_value_construct_move(
             array->_data + array->_size, value, array->_alloc))) {
        return ec;
    }

    ++array->_size;
    return JSON_ERRC_OK;
}

enum json_errc json_array_emplace_back(
    struct json_array *array, struct json_allocator *alloc)
{
    enum json_errc ec;

    if ((ec = json_array_reserve(array, array->_size + 1))) {
        return ec;
    }

    json_value_construct(array->_data + array->_size++, alloc);
    return JSON_ERRC_OK;
}

enum json_errc json_array_emplace_back_null(
    struct json_array *array, struct json_allocator *alloc)
{
    return json_array_emplace_back(array, alloc);
}

enum json_errc json_array_emplace_back_bool(
    struct json_array *array, json_bool value, struct json_allocator *alloc)
{
    if (json_array_reserve(array, array->_size + 1)) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    json_value_construct_bool(array->_data + array->_size++, value, alloc);
    return JSON_ERRC_OK;
}

enum json_errc json_array_emplace_back_int(
    struct json_array *array, json_int value, struct json_allocator *alloc)
{
    if (json_array_reserve(array, array->_size + 1)) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    json_value_construct_int(array->_data + array->_size++, value, alloc);
    return JSON_ERRC_OK;
}

enum json_errc json_array_emplace_back_float(
    struct json_array *array, json_float value, struct json_allocator *alloc)
{
    if (json_array_reserve(array, array->_size + 1)) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    json_value_construct_float(array->_data + array->_size++, value, alloc);
    return JSON_ERRC_OK;
}

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

void json_array_erase(struct json_array *array, json_size pos, json_size count)
{
    for (json_size i = 0; i < count; i++) {
        json_value_destruct(array->_data + pos + i);
    }

    memmove(array->_data + pos, array->_data + pos + count,
            (array->_data - pos - count));
}

enum json_errc json_array_shrink_to_fit(struct json_array *array)
{
    if (array->_size < array->_capacity) {
        struct json_value *new_data =
            json_allocate_values(array->_alloc, array->_size);

        if (!new_data) {
            return JSON_ERRC_NOT_ENOUGH_MEMORY;
        }

        json_memcpy_values(new_data, array->_data, array->_size);
        json_deallocate_values(array->_alloc, array->_data, array->_capacity);
        array->_data = new_data;
        array->_capacity = array->_size;
    }

    return JSON_ERRC_OK;
}

enum json_errc json_array_reserve(struct json_array *array, json_size n)
{
    if (n < array->_capacity) {
        struct json_value *new_data =
            json_allocate_values(array->_alloc, array->_size);

        if (!new_data) {
            return JSON_ERRC_NOT_ENOUGH_MEMORY;
        }

        json_memcpy_values(new_data, array->_data, array->_size);
        json_deallocate_values(array->_alloc, array->_data, array->_capacity);
        array->_data = new_data;
        array->_capacity = n;
    }
}

enum json_errc json_array_resize(
    struct json_array *array, json_size n, const struct json_value *value)
{
    if (n > array->_size) {
        json_array_reserve(array, n);

        if (value) {
            for (json_size i = array->_size; i < n; i++) {
                json_value_construct_copy(
                    array->_data + i, value, array->_alloc);
            }
        } else {
            for (json_size i = array->_size; i < n; i++) {
                json_value_construct(array->_data + i, array->_alloc);
            }
        }
    } else {
        array->_size = n;
    }

    return JSON_ERRC_OK;
}

struct json_value *json_array_front(struct json_array *array)
{
    return array->_data;
}

struct json_value *json_array_back(struct json_array *array)
{
    return array->_data + array->_size - 1;
}

struct json_value *json_array_at(struct json_array *array, json_size pos)
{
    return array->_data + pos;
}

struct json_value *json_array_data(struct json_array *array)
{
    return array->_data;
}

void json_array_swap(struct json_array *array, struct json_array *other)
{
    struct json_value *data = array->_data;
    json_size size = array->_size;
    json_size capacity = array->_capacity;

    array->_data = other->_data;
    array->_size = other->_size;
    array->_capacity = other->_capacity;

    other->_data = data;
    other->_size = size;
    other->_capacity = capacity;
}

struct json_array *json_array_new(struct json_allocator *alloc)
{
    struct json_array *array;

    alloc = alloc ? alloc : json_get_default_allocator();
    array = json_allocate_arrays(alloc, 1);

    if (array) {
        json_array_construct(array, alloc);
    }

    return array;
}

struct json_array *json_array_new_copy(
    const struct json_array *other, struct json_allocator *alloc)
{
    struct json_array *array;

    alloc = alloc ? alloc : other->_alloc;
    array = json_allocate_arrays(alloc, 1);

    if (array && json_array_construct_copy(array, other, alloc)) {
        json_deallocate_arrays(alloc, array, 1);
        array = NULL;
    }

    return array;
}

struct json_array *json_array_new_move(
    struct json_array *other, struct json_allocator *alloc)
{
    struct json_array *array;

    alloc = alloc ? alloc : other->_alloc;
    array = json_allocate_arrays(alloc, 1);

    if (array && json_array_construct_move(array, other, alloc)) {
        json_deallocate_arrays(alloc, array, 1);
        array = NULL;
    }

    return array;
}

void json_array_delete(struct json_array *array)
{
    struct json_allocator *alloc = array->_alloc;

    json_array_destruct(array);
    json_deallocate_arrays(alloc, array, 1);
}

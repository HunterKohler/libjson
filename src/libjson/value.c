#include <libjson/array.h>
#include <libjson/fwd.h>
#include <libjson/object.h>
#include <libjson/string.h>
#include <libjson/value.h>
#include "./util.h"

void json_value_construct(
    struct json_value *value, struct json_allocator *alloc)
{
    json_value_construct_null(value, alloc);
}

void json_value_construct_null(
    struct json_value *value, struct json_allocator *alloc)
{
    value->_type = JSON_TYPE_NULL;
    value->_data._primitive._alloc =
        alloc ? alloc : json_get_default_allocator();
}

void json_value_construct_bool(struct json_value *value, json_bool bool_value,
                               struct json_allocator *alloc)
{
    value->_type = JSON_TYPE_BOOL;
    value->_data._primitive._value._bool = bool_value;
    value->_data._primitive._alloc =
        alloc ? alloc : json_get_default_allocator();
}

void json_value_construct_int(
    struct json_value *value, json_int int_value, struct json_allocator *alloc)
{
    value->_type = JSON_TYPE_INT;
    value->_data._primitive._value._int = int_value;
    value->_data._primitive._alloc =
        alloc ? alloc : json_get_default_allocator();
}

void json_value_construct_float(
    struct json_value *value, json_float float_value,
    struct json_allocator *alloc)
{
    value->_type = JSON_TYPE_FLOAT;
    value->_data._primitive._value._float = float_value;
    value->_data._primitive._alloc =
        alloc ? alloc : json_get_default_allocator();
}

enum json_errc json_value_construct_string_copy(
    struct json_value *value, const struct json_string *string_value,
    struct json_allocator *alloc)
{
    alloc = alloc ? alloc : json_get_default_allocator();
    value->_type = JSON_TYPE_STRING;
    value->_data._string = json_allocate_strings(alloc, 1);

    if (!value->_data._string) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    return json_string_construct_copy(
        value->_data._string, string_value, alloc);
}

enum json_errc json_value_construct_string_move(
    struct json_value *value, struct json_string *string_value,
    struct json_allocator *alloc)
{
    alloc = alloc ? alloc : json_get_default_allocator();
    value->_type = JSON_TYPE_STRING;
    value->_data._string = json_allocate_strings(alloc, 1);

    if (!value->_data._string) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    return json_string_construct_move(
        value->_data._string, string_value, alloc);
}

enum json_errc json_value_construct_array_copy(
    struct json_value *value, const struct json_array *array_value,
    struct json_allocator *alloc)
{
    alloc = alloc ? alloc : json_get_default_allocator();
    value->_type = JSON_TYPE_ARRAY;
    value->_data._array = json_allocate_arrays(alloc, 1);

    if (!value->_data._array) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    return json_array_construct_copy(value->_data._array, array_value, alloc);
}

enum json_errc json_value_construct_array_move(
    struct json_value *value, struct json_array *array_value,
    struct json_allocator *alloc)
{
    alloc = alloc ? alloc : json_get_default_allocator();
    value->_type = JSON_TYPE_ARRAY;
    value->_data._array = json_allocate_arrays(alloc, 1);

    if (!value->_data._array) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    return json_array_construct_move(value->_data._array, array_value, alloc);
}

enum json_errc json_value_construct_object_copy(
    struct json_value *value, const struct json_object *object_value,
    struct json_allocator *alloc)
{
    alloc = alloc ? alloc : json_get_default_allocator();
    value->_type = JSON_TYPE_OBJECT;
    value->_data._object = json_allocate_objects(alloc, 1);

    if (!value->_data._object) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    return json_object_construct_copy(
        value->_data._object, object_value, alloc);
}

enum json_errc json_value_construct_object_move(
    struct json_value *value, struct json_object *object_value,
    struct json_allocator *alloc)
{
    alloc = alloc ? alloc : json_get_default_allocator();
    value->_type = JSON_TYPE_OBJECT;
    value->_data._object = json_allocate_objects(alloc, 1);

    if (!value->_data._object) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    return json_object_construct_move(
        value->_data._object, object_value, alloc);
}

enum json_errc json_value_construct_copy(
    struct json_value *value, const struct json_value *other,
    struct json_allocator *alloc);

static enum json_errc json_value_construct_move_with_string(
    struct json_value *value, struct json_value *other,
    struct json_allocator *alloc);

static enum json_errc json_value_construct_move_with_array(
    struct json_value *value, struct json_value *other,
    struct json_allocator *alloc);

static enum json_errc json_value_construct_move_with_object(
    struct json_value *value, struct json_value *other,
    struct json_allocator *alloc);

enum json_errc json_value_construct_move(
    struct json_value *value, struct json_value *other,
    struct json_allocator *alloc)
{
    switch (other->_type) {
    case JSON_TYPE_NULL:
        json_value_construct_null(
            value, alloc ? alloc : other->_data._primitive._alloc);
        break;
    case JSON_TYPE_BOOL:
        json_value_construct_bool(
            value, other->_data._primitive._value._bool,
            alloc ? alloc : other->_data._primitive._alloc);
        break;
    case JSON_TYPE_INT:
        json_value_construct_int(
            value, other->_data._primitive._value._int,
            alloc ? alloc : other->_data._primitive._alloc);
        break;
    case JSON_TYPE_FLOAT:
        json_value_construct_float(
            value, other->_data._primitive._value._float,
            alloc ? alloc : other->_data._primitive._alloc);
        break;
    case JSON_TYPE_STRING:
        return json_value_construct_move_with_string(value, other, alloc);
    case JSON_TYPE_ARRAY:
        return json_value_construct_move_with_array(value, other, alloc);
    case JSON_TYPE_OBJECT:
        return json_value_construct_move_with_object(value, other, alloc);
    default:
        json_unreachable();
    }

    return 0;
}

static void json_value_destruct_string(struct json_value *value)
{
    struct json_string *string = value->_data._string;
    struct json_allocator *alloc = string->_alloc;

    json_string_destruct(string);
    json_deallocate_strings(alloc, string, 1);
}

static void json_value_destruct_array(struct json_value *value)
{
    struct json_array *array = value->_data._array;
    struct json_allocator *alloc = array->_alloc;

    json_array_destruct(array);
    json_deallocate_arrays(alloc, array, 1);
}

static void json_value_destruct_object(struct json_value *value)
{
    struct json_object *object = value->_data._object;
    struct json_allocator *alloc = object->_alloc;

    json_object_destruct(object);
    json_deallocate_objects(alloc, object, 1);
}

void json_value_destruct(struct json_value *value)
{
    switch (value->_type) {
    case JSON_TYPE_NULL:
    case JSON_TYPE_BOOL:
    case JSON_TYPE_INT:
    case JSON_TYPE_FLOAT:
        break;
    case JSON_TYPE_STRING:
        json_value_destruct_string(value);
        break;
    case JSON_TYPE_ARRAY:
        json_value_destruct_array(value);
        break;
    case JSON_TYPE_OBJECT:
        json_value_destruct_object(value);
        break;
    default:
        json_unreachable();
    }
}

struct json_allocator *json_value_get_allocator(const struct json_value *value)
{
    switch (value->_type) {
    case JSON_TYPE_NULL:
    case JSON_TYPE_BOOL:
    case JSON_TYPE_INT:
    case JSON_TYPE_FLOAT:
        return value->_data._primitive._alloc;
    case JSON_TYPE_STRING:
        return value->_data._string->_alloc;
    case JSON_TYPE_ARRAY:
        return value->_data._array->_alloc;
    case JSON_TYPE_OBJECT:
        return value->_data._object->_alloc;
    default:
        json_unreachable();
    }
}

enum json_type json_value_get_type(const struct json_value *value)
{
    return value->_type;
}

json_bool json_value_is_null(const struct json_value *value)
{
    return value->_type == JSON_TYPE_NULL;
}

json_bool json_value_is_bool(const struct json_value *value)
{
    return value->_type == JSON_TYPE_BOOL;
}

json_bool json_value_is_int(const struct json_value *value)
{
    return value->_type == JSON_TYPE_INT;
}

json_bool json_value_is_float(const struct json_value *value)
{
    return value->_type == JSON_TYPE_FLOAT;
}

json_bool json_value_is_string(const struct json_value *value)
{
    return value->_type == JSON_TYPE_STRING;
}

json_bool json_value_is_array(const struct json_value *value)
{
    return value->_type == JSON_TYPE_ARRAY;
}

json_bool json_value_is_object(const struct json_value *value)
{
    return value->_type == JSON_TYPE_OBJECT;
}

void json_value_assign_null(struct json_value *value)
{
    struct json_allocator *alloc;

    switch (value->_type) {
    case JSON_TYPE_NULL:
        break;
    case JSON_TYPE_BOOL:
    case JSON_TYPE_INT:
    case JSON_TYPE_FLOAT:
        value->_type = JSON_TYPE_NULL;
        break;
    case JSON_TYPE_STRING:
        alloc = value->_data._string->_alloc;
        json_value_destruct_string(value);
        json_value_construct_null(value, alloc);
        break;
    case JSON_TYPE_ARRAY:
        alloc = value->_data._array->_alloc;
        json_value_destruct_array(value);
        json_value_construct_null(value, alloc);
        break;
    case JSON_TYPE_OBJECT:
        alloc = value->_data._object->_alloc;
        json_value_destruct_object(value);
        json_value_construct_null(value, alloc);
        break;
    default:
        json_unreachable();
    }
}

void json_value_assign_bool(struct json_value *value, json_bool new_value)
{
    struct json_allocator *alloc;

    switch (value->_type) {
    case JSON_TYPE_NULL:
        break;
    case JSON_TYPE_BOOL:
    case JSON_TYPE_INT:
    case JSON_TYPE_FLOAT:
        value->_type = JSON_TYPE_BOOL;
        value->_data._primitive._value._bool = new_value;
        break;
    case JSON_TYPE_STRING:
        alloc = value->_data._string->_alloc;
        json_value_destruct_string(value);
        json_value_construct_bool(value, new_value, alloc);
        break;
    case JSON_TYPE_ARRAY:
        alloc = value->_data._array->_alloc;
        json_value_destruct_array(value);
        json_value_construct_bool(value, new_value, alloc);
        break;
    case JSON_TYPE_OBJECT:
        alloc = value->_data._object->_alloc;
        json_value_destruct_object(value);
        json_value_construct_bool(value, new_value, alloc);
        break;
    default:
        json_unreachable();
    }
}

void json_value_assign_int(struct json_value *value, json_int new_value)
{
    struct json_allocator *alloc;

    switch (value->_type) {
    case JSON_TYPE_NULL:
        break;
    case JSON_TYPE_BOOL:
    case JSON_TYPE_INT:
    case JSON_TYPE_FLOAT:
        value->_type = JSON_TYPE_INT;
        value->_data._primitive._value._int = new_value;
        break;
    case JSON_TYPE_STRING:
        alloc = value->_data._string->_alloc;
        json_value_destruct_string(value);
        json_value_construct_int(value, new_value, alloc);
        break;
    case JSON_TYPE_ARRAY:
        alloc = value->_data._array->_alloc;
        json_value_destruct_array(value);
        json_value_construct_int(value, new_value, alloc);
        break;
    case JSON_TYPE_OBJECT:
        alloc = value->_data._object->_alloc;
        json_value_destruct_object(value);
        json_value_construct_int(value, new_value, alloc);
        break;
    default:
        json_unreachable();
    }
}

void json_value_assign_float(struct json_value *value, json_float new_value)
{
    struct json_allocator *alloc;

    switch (value->_type) {
    case JSON_TYPE_NULL:
        break;
    case JSON_TYPE_BOOL:
    case JSON_TYPE_INT:
    case JSON_TYPE_FLOAT:
        value->_type = JSON_TYPE_FLOAT;
        value->_data._primitive._value._float = new_value;
        break;
    case JSON_TYPE_STRING:
        alloc = value->_data._string->_alloc;
        json_value_destruct_string(value);
        json_value_construct_float(value, new_value, alloc);
        break;
    case JSON_TYPE_ARRAY:
        alloc = value->_data._array->_alloc;
        json_value_destruct_array(value);
        json_value_construct_float(value, new_value, alloc);
        break;
    case JSON_TYPE_OBJECT:
        alloc = value->_data._object->_alloc;
        json_value_destruct_object(value);
        json_value_construct_float(value, new_value, alloc);
        break;
    default:
        json_unreachable();
    }
}

enum json_errc json_value_assign_string_copy(
    struct json_value *value, const struct json_string *new_value)
{
    struct json_allocator *alloc;

    switch (value->_type) {
    case JSON_TYPE_NULL:
    case JSON_TYPE_BOOL:
    case JSON_TYPE_INT:
    case JSON_TYPE_FLOAT:
        alloc = value->_data._primitive._alloc;
        return json_value_construct_string_copy(value, new_value, alloc);
    case JSON_TYPE_STRING:
        return json_string_assign_copy(value->_data._string, new_value);
    case JSON_TYPE_ARRAY:
        alloc = value->_data._array->_alloc;
        json_value_destruct_array(value);
        return json_value_construct_string_copy(value, new_value, alloc);
    case JSON_TYPE_OBJECT:
        alloc = value->_data._object->_alloc;
        json_value_destruct_object(value);
        return json_value_construct_string_copy(value, new_value, alloc);
    default:
        json_unreachable();
    }
}

enum json_errc json_value_assign_string_move(
    struct json_value *value, struct json_string *new_value)
{
    struct json_allocator *alloc;

    switch (value->_type) {
    case JSON_TYPE_NULL:
    case JSON_TYPE_BOOL:
    case JSON_TYPE_INT:
    case JSON_TYPE_FLOAT:
        alloc = value->_data._primitive._alloc;
        return json_value_construct_string_move(value, new_value, alloc);
    case JSON_TYPE_STRING:
        return json_string_assign_move(value->_data._string, new_value);
    case JSON_TYPE_ARRAY:
        alloc = value->_data._array->_alloc;
        json_value_destruct_array(value);
        return json_value_construct_string_move(value, new_value, alloc);
    case JSON_TYPE_OBJECT:
        alloc = value->_data._object->_alloc;
        json_value_destruct_object(value);
        return json_value_construct_string_move(value, new_value, alloc);
    default:
        json_unreachable();
    }
}

enum json_errc json_value_assign_array_copy(
    struct json_value *value, const struct json_array *new_value)
{
    if (json_value_is_array(value)) {
        json_array_assign_copy(value->_data._array, new_value);
    } else {
        struct json_allocator *alloc = json_value_get_allocator(value);

        json_value_destruct(value);
        json_value_construct_array_copy(value, new_value, alloc);
    }
}

enum json_errc json_value_assign_array_move(
    struct json_value *value, struct json_array *new_value)
{
    if (json_value_is_array(value)) {
        json_array_assign_move(value->_data._array, new_value);
    } else {
        struct json_allocator *alloc = json_value_get_allocator(value);

        json_value_destruct(value);
        json_value_construct_array_move(value, new_value, alloc);
    }
}

enum json_errc json_value_assign_object_copy(
    struct json_value *value, const struct json_object *new_value);

enum json_errc json_value_assign_object_move(
    struct json_value *value, struct json_object *new_value);

enum json_errc json_value_assign_copy(
    struct json_value *value, const struct json_value *other)
{
    if (value->_type == other->_type) {
        switch (value->_type) {
        case JSON_TYPE_NULL:
            break;
        case JSON_TYPE_BOOL:
        case JSON_TYPE_INT:
        case JSON_TYPE_FLOAT:
            value->_data._primitive._value = other->_data._primitive._value;
            return JSON_ERRC_OK;
        case JSON_TYPE_STRING:
            return json_string_assign_copy(
                value->_data._string, other->_data._string);
        case JSON_TYPE_ARRAY:
            return json_array_assign_copy(
                value->_data._array, other->_data._array);
        case JSON_TYPE_OBJECT:
            return json_object_assign_copy(
                value->_data._object, other->_data._object);
        default:
            json_unreachable();
        }
    }

    struct json_allocator *alloc = json_value_get_allocator(value);
    json_value_destruct(value);
    return json_value_construct_copy(value, other, alloc);
}

enum json_errc json_value_assign_move(
    struct json_value *value, struct json_value *other);

json_bool *json_value_as_bool(struct json_value *value)
{
    return &value->_data._primitive._value._bool;
}

json_int *json_value_as_int(struct json_value *value)
{
    return &value->_data._primitive._value._int;
}

json_float *json_value_as_float(struct json_value *value)
{
    return &value->_data._primitive._value._float;
}

struct json_string *json_value_as_string(struct json_value *value)
{
    return value->_data._string;
}

struct json_array *json_value_as_array(struct json_value *value)
{
    return value->_data._array;
}

struct json_object *json_value_as_object(struct json_value *value)
{
    return value->_data._object;
}

struct json_value *json_value_new(struct json_allocator *alloc)
{
    return json_value_new_null(alloc);
}

struct json_value *json_value_new_null(struct json_allocator *alloc)
{
    alloc = alloc ? alloc : json_get_default_allocator();
    struct json_value *ret = json_allocate_values(alloc, 1);

    if (ret) {
        json_value_construct_null(ret, alloc);
    }

    return ret;
}

#define JSON_DEFINE_JSON_VALUE_NEW(suffix, value_type)           \
    struct json_value *json_value_new_##suffix(                  \
        value_type value, struct json_allocator *alloc)          \
    {                                                            \
        alloc = alloc ? alloc : json_get_default_allocator();    \
        struct json_value *ret = json_allocate_values(alloc, 1); \
        if (ret) {                                               \
            json_value_construct_##suffix(ret, value, alloc);    \
        }                                                        \
        return ret;                                              \
    }

JSON_DEFINE_JSON_VALUE_NEW(bool, json_bool);
JSON_DEFINE_JSON_VALUE_NEW(int, json_int);
JSON_DEFINE_JSON_VALUE_NEW(float, json_float);
JSON_DEFINE_JSON_VALUE_NEW(string_copy, const struct json_string *);
JSON_DEFINE_JSON_VALUE_NEW(string_move, struct json_string *);
JSON_DEFINE_JSON_VALUE_NEW(array_copy, const struct json_array *);
JSON_DEFINE_JSON_VALUE_NEW(array_move, struct json_array *);
JSON_DEFINE_JSON_VALUE_NEW(object_copy, const struct json_object *);
JSON_DEFINE_JSON_VALUE_NEW(object_move, struct json_object *);
JSON_DEFINE_JSON_VALUE_NEW(copy, const struct json_value *);
JSON_DEFINE_JSON_VALUE_NEW(move, struct json_value *);

void json_value_delete(struct json_value *value)
{
    struct json_allocator *alloc = json_value_get_allocator(value);
    json_value_destruct(value);
    json_deallocate_values(alloc, value, 1);
}

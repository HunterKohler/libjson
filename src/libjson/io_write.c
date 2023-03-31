#include <libjson/array.h>
#include <libjson/entry.h>
#include <libjson/fwd.h>
#include <libjson/io.h>
#include <libjson/object.h>
#include <libjson/string.h>
#include <libjson/value.h>
#include "./util.h"

struct json_writer {
    char *first;
    char *last;
    const struct json_write_options *options;
    json_size depth;
};

static inline struct json_write_result json_make_write_result(
    char *ptr, enum json_errc ec)
{
    return (struct json_write_result){ .ptr = ptr, .ec = ec };
}

static const struct json_write_options json_default_write_options = {
    .indent_size = 0
};

static inline struct json_writer json_make_writer(
    char *first, char *last, const struct json_write_options *options)
{
    return (struct json_writer){
        .first = first,
        .last = last,
        .depth = 0,
        .options = options ? options : &json_default_write_options,
    };
}

static inline enum json_errc json_writer_write_char(
    struct json_writer *w, char c)
{
    if (w->last - w->first < 1) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    *w->first++ = c;
    return JSON_ERRC_OK;
}

static inline enum json_errc json_writer_indent(struct json_writer *w)
{
    json_size n = w->depth * w->options->indent_size;

    if (w->last - w->first < n) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    for (; n; --n) {
        *w->first++ = ' ';
    }

    return JSON_ERRC_OK;
}

static inline enum json_errc json_writer_newline(struct json_writer *w)
{
    if (w->options->indent_size) {
        return json_writer_write_char(w, '\n');
    }

    return JSON_ERRC_OK;
}

static inline enum json_errc json_writer_value_sep(struct json_writer *w)
{
    return json_writer_write_char(w, ',');
}

static inline enum json_errc json_writer_name_sep(struct json_writer *w)
{
    enum json_errc ec;

    if ((json_writer_write_char(w, ':')) ||
        (w->options->indent_size && (ec = json_writer_write_char(w, ' ')))) {
        return ec;
    }

    return JSON_ERRC_OK;
}

static inline enum json_errc json_writer_begin_object(struct json_writer *w)
{
    enum json_errc ec = json_writer_write_char(w, '{');
    w->depth += (ec == JSON_ERRC_OK);
    return ec;
}

static inline enum json_errc json_writer_end_object(struct json_writer *w)
{
    enum json_errc ec = json_writer_write_char(w, '}');
    w->depth -= (ec == JSON_ERRC_OK);
    return ec;
}

static inline enum json_errc json_writer_begin_array(struct json_writer *w)
{
    enum json_errc ec = json_writer_write_char(w, '[');
    w->depth += (ec == JSON_ERRC_OK);
    return ec;
}

static inline enum json_errc json_writer_end_array(struct json_writer *w)
{
    enum json_errc ec = json_writer_write_char(w, ']');
    w->depth -= (ec == JSON_ERRC_OK);
    return ec;
}

static enum json_errc json_writer_write_null(struct json_writer *w)
{
    if (w->last - w->first < 4) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    *w->first++ = 'n';
    *w->first++ = 'u';
    *w->first++ = 'l';
    *w->first++ = 'l';

    return JSON_ERRC_OK;
}

static enum json_errc json_writer_write_bool(
    struct json_writer *w, json_bool value)
{
    if (value) {
        if (w->last - w->first < 4) {
            return JSON_ERRC_NOT_ENOUGH_MEMORY;
        }

        *w->first++ = 't';
        *w->first++ = 'r';
        *w->first++ = 'u';
        *w->first++ = 'e';
    } else if (w->last - w->first < 5) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    } else {
        *w->first++ = 'f';
        *w->first++ = 'a';
        *w->first++ = 'l';
        *w->first++ = 's';
        *w->first++ = 'e';
    }

    return JSON_ERRC_OK;
}

static json_uint json_uint_log10(json_uint value)
{
    json_uint n = 0;
    json_uint b1 = 10;
    json_uint b2 = 100;
    json_uint b3 = 1000;
    json_uint b4 = 10000;

    for (;;) {
        if (value < b1) {
            return n;
        } else if (value < b2) {
            return n + 1;
        } else if (value < b3) {
            return n + 2;
        } else if (value < b4) {
            return n + 3;
        }

        value /= b4;
        n += 4;
    }
}

static const char json_write_int_impl_table[] =
    "0001020304050607080910111213141516171819"
    "2021222324252627282930313233343536373839"
    "4041424344454647484950515253545556575859"
    "6061626364656667686970717273747576777879"
    "8081828384858687888990919293949596979899";

static enum json_errc json_writer_write_int(
    struct json_writer *w, json_int value)
{
    json_size length;

    if (value < 0) {
        value = -value;
        length = 1 + json_uint_log10(value);

        if (w->first - w->last < length) {
            return JSON_ERRC_NOT_ENOUGH_MEMORY;
        }

        *w->first = '-';
    } else {
        length = json_uint_log10(value);

        if (w->first - w->last < length) {
            return JSON_ERRC_NOT_ENOUGH_MEMORY;
        }
    }

    w->first += length;

    while (value >= 100) {
        json_size n = 2 * (value % 100);

        value /= 100;
        w->first[--length] = json_write_int_impl_table[n + 1];
        w->first[--length] = json_write_int_impl_table[n];
    }

    if (value >= 10) {
        json_size n = 2 * value;

        w->first[--length] = json_write_int_impl_table[n + 1];
        w->first[--length] = json_write_int_impl_table[n];
    } else {
        w->first[--length] = '0' + value;
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_writer_write_float(
    struct json_writer *w, json_float value);

static enum json_errc json_writer_write_string(
    struct json_writer *w, const struct json_string *value);

/* pre-declaration */
static enum json_errc json_writer_write_value(
    struct json_writer *w, const struct json_value *value);

static enum json_errc json_writer_write_array(
    struct json_writer *w, const struct json_array *value)
{
    enum json_errc ec;
    json_size size = value->_size;

    if ((ec = json_writer_begin_array(w)) || (ec = json_writer_newline(w))) {
        return ec;
    }

    if (size) {
        --size;
        for (json_size i = 0; i < size; i++) {
            if ((ec = json_writer_indent(w)) ||
                (ec = json_writer_write_value(w, value->_data + i)) ||
                (ec = json_writer_value_sep(w)) ||
                (ec = json_writer_newline(w))) {
                return ec;
            }
        }

        if ((ec = json_writer_indent(w)) ||
            (ec = json_writer_write_value(w, value->_data + size)) ||
            (ec = json_writer_newline(w))) {
            return ec;
        }
    }

    return json_writer_end_array(w);
}

static enum json_errc json_writer_write_entry(
    struct json_writer *w, const struct json_entry *value)
{
    enum json_errc ec;

    if ((ec = json_writer_write_string(w, &value->_key)) ||
        (ec = json_writer_name_sep(w)) ||
        (ec = json_writer_write_value(w, &value->_value))) {
        return ec;
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_writer_write_object(
    struct json_writer *w, const struct json_object *value);

static enum json_errc json_writer_write_value(
    struct json_writer *w, const struct json_value *value)
{
    switch (value->_type) {
    case JSON_TYPE_NULL:
        return json_writer_write_null(w);
    case JSON_TYPE_BOOL:
        return json_writer_write_bool(w, value->_data._primitive._value._bool);
    case JSON_TYPE_INT:
        return json_writer_write_int(w, value->_data._primitive._value._int);
    case JSON_TYPE_FLOAT:
        return json_writer_write_float(
            w, value->_data._primitive._value._float);
    case JSON_TYPE_STRING:
        return json_writer_write_string(w, value->_data._string);
    case JSON_TYPE_ARRAY:
        return json_writer_write_array(w, value->_data._array);
    case JSON_TYPE_OBJECT:
        return json_writer_write_object(w, value->_data._object);
    default:
        json_unreachable();
    }
}

struct json_write_result json_write_null(
    char *first, char *last, const struct json_write_options *options)
{
    struct json_writer w = json_make_writer(first, last, options);
    enum json_errc ec = json_writer_write_null(&w);
    return json_make_write_result(w.first, ec);
}

struct json_write_result json_write_bool(
    char *first, char *last, json_bool value,
    const struct json_write_options *options)
{
    struct json_writer w = json_make_writer(first, last, options);
    enum json_errc ec = json_writer_write_bool(&w, value);
    return json_make_write_result(w.first, ec);
}

struct json_write_result json_write_int(
    char *first, char *last, json_int value,
    const struct json_write_options *options)
{
    struct json_writer w = json_make_writer(first, last, options);
    enum json_errc ec = json_writer_write_int(&w, value);
    return json_make_write_result(w.first, ec);
}

struct json_write_result json_write_float(
    char *first, char *last, json_float value,
    const struct json_write_options *options)
{
    struct json_writer w = json_make_writer(first, last, options);
    enum json_errc ec = json_writer_write_float(&w, value);
    return json_make_write_result(w.first, ec);
}

struct json_write_result json_write_string(
    char *first, char *last, const struct json_string *value,
    const struct json_write_options *options)
{
    struct json_writer w = json_make_writer(first, last, options);
    enum json_errc ec = json_writer_write_string(&w, value);
    return json_make_write_result(w.first, ec);
}

struct json_write_result json_write_array(
    char *first, char *last, const struct json_array *value,
    const struct json_write_options *options)
{
    struct json_writer w = json_make_writer(first, last, options);
    enum json_errc ec = json_writer_write_array(&w, value);
    return json_make_write_result(w.first, ec);
}

struct json_write_result json_write_object(
    char *first, char *last, const struct json_object *value,
    const struct json_write_options *options)
{
    struct json_writer w = json_make_writer(first, last, options);
    enum json_errc ec = json_writer_write_object(&w, value);
    return json_make_write_result(w.first, ec);
}

struct json_write_result json_write_value(
    char *first, char *last, const struct json_value *value,
    const struct json_write_options *options)
{
    struct json_writer w = json_make_writer(first, last, options);
    enum json_errc ec = json_writer_write_value(&w, value);
    return json_make_write_result(w.first, ec);
}

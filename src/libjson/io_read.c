#include <uchar.h>
#include <libjson/array.h>
#include <libjson/entry.h>
#include <libjson/fwd.h>
#include <libjson/io.h>
#include <libjson/object.h>
#include <libjson/string.h>
#include <libjson/value.h>
#include "./util.h"

struct json_reader {
    const char *first;
    const char *last;
    const struct json_read_options *options;
    json_size depth;
};

static inline struct json_read_result json_make_read_result(
    const char *ptr, enum json_errc ec)
{
    return (struct json_read_result){ .ec = ec, .ptr = ptr };
}

static const struct json_read_options json_default_read_options = {
    .max_depth = 250,
    .accept_invalid_code_points = json_false,
    .replace_invalid_code_points = json_false,
    .accept_trailing_commas = json_false,
    .accept_comments = json_false,
    .accept_duplicate_keys = json_false
};

static inline struct json_reader json_make_reader(
    const char *first, const char *last,
    const struct json_read_options *options)
{
    return (struct json_reader){
        .depth = 0,
        .first = first,
        .last = last,
        .options = options ? options : &json_default_read_options
    };
}

// https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf#page=49
static inline json_bool json_unicode_is_noncharacter(char32_t value)
{
    return (value >= 0xFDD0 && value <= 0xFDEF) ||
           (value <= 0x10FFFF &&
            ((value & 0xFFFF) == 0xFFFE || (value & 0xFFFF) == 0xFFFF));
}

// https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf#page=49
static inline json_bool json_unicode_is_surrogate(char32_t value)
{
    return value >= 0xD800 && value <= 0xDFFF;
}

// https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf#page=49
static inline json_bool json_unicode_is_high_surrogate(char32_t value)
{
    return value >= 0xD800 && value <= 0xDBFF;
}

// https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf#page=49
static inline json_bool json_unicode_is_low_surrogate(char32_t value)
{
    return value >= 0xDC00 && value <= 0xDFFF;
}

// No check for invalid surrogates.
static inline char32_t json_unicode_surrogate_code_point(
    char16_t high, char16_t low)
{
    return (((char32_t)high - 0xD800) << 10) + ((char32_t)low - 0xDC00) +
           0x10000;
}

// See code point bit table:
// https://en.wikipedia.org/wiki/UTF-8#Encoding
static inline enum json_errc json_reader_read_utf8_char(
    struct json_reader *r, char32_t *dest)
{
    if (r->first == r->last) {
        return JSON_ERRC_UNEXPECTED_TOKEN;
    }

    char32_t b1 = *r->first;

    if ((b1 >> 7) == 0b0) {
        *dest = b1;
    } else if ((b1 >> 5) == 0b110) {
        if (++r->first == r->last) {
            return JSON_ERRC_INVALID_ENCODING;
        }

        char32_t b2 = r->first[0];

        if ((b2 >> 6) != 0b110) {
            return JSON_ERRC_INVALID_ENCODING;
        }

        r->first += 1;
        *dest = (b1 & 0x1F) << 6 | (b2 & 0x3F);
    } else if ((b1 >> 4) == 0b1110) {
        if (r->last - ++r->first < 2) {
            return JSON_ERRC_INVALID_ENCODING;
        }

        char32_t b2 = r->first[0];
        char32_t b3 = r->first[1];

        if ((b2 >> 6) != 0b110 || (b3 >> 6) != 0b110) {
            return JSON_ERRC_INVALID_ENCODING;
        }

        r->first += 2;
        *dest = (b1 & 0x0F) << 12 | (b2 & 0x3F) << 6 | (b3 & 0x3F);
    } else if ((b1 >> 3) == 0b11110) {
        if (r->last - ++r->first < 3) {
            return JSON_ERRC_INVALID_ENCODING;
        }

        char32_t b2 = r->first[0];
        char32_t b3 = r->first[1];
        char32_t b4 = r->first[2];

        if ((b2 >> 6) != 0b110 || (b3 >> 6) != 0b110 || (b4 >> 6) != 0b110) {
            return JSON_ERRC_INVALID_ENCODING;
        }

        r->first += 3;
        *dest = (b1 & 0x07) << 17 | (b2 & 0x3F) << 12 | (b3 & 0x3F) << 6 |
                (b4 & 0x3F);
    } else {
        return JSON_ERRC_INVALID_ENCODING;
    }

    return JSON_ERRC_OK;
}

static inline json_size json_write_utf8_char(char32_t code_point, char *out)
{
    if (code_point > 0x10FFFF) {
        return 0;
    } else if (!(code_point >> 7)) {
        *out++ = code_point;
        return 1;
    } else if (!(code_point >> 11)) {
        *out++ = 0xC0 | (code_point >> 6);
        *out++ = 0x80 | (code_point & 0x3F);
        return 2;
    } else if (!(code_point >> 16)) {
        *out++ = 0xE0 | (code_point >> 12);
        *out++ = 0x80 | ((code_point >> 6) & 0x3F);
        *out++ = 0x80 | (code_point & 0x3F);
        return 3;
    } else {
        *out++ = 0xF0 | (code_point >> 18);
        *out++ = 0x80 | ((code_point >> 12) & 0x3F);
        *out++ = 0x80 | ((code_point >> 6) & 0x3F);
        *out++ = 0x80 | (code_point & 0x3F);
        return 4;
    }
}

static enum json_errc json_reader_consume_comment(struct json_reader *r);

static enum json_errc json_reader_consume_space(struct json_reader *r)
{
    enum json_errc ec;

    while (r->first != r->last) {
        switch (*r->first) {
        case '/':
            if ((ec = json_reader_consume_comment(r))) {
                return ec;
            } else {
                break;
            }
        case '\n':
        case '\r':
        case '\t':
        case ' ':
            ++r->first;
            break;
        default:
            return JSON_ERRC_OK;
        }
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_reader_append_code_point(
    struct json_reader *reader, struct json_string *string,
    char32_t code_point)
{
    if (json_unicode_is_surrogate(code_point) ||
        json_unicode_is_noncharacter(code_point)) {
        if (!reader->options->accept_invalid_code_points) {
            return JSON_ERRC_INVALID_ESCAPE;
        } else if (reader->options->replace_invalid_code_points) {
            code_point = 0xFFFD;
        }
    }

    char buf[4];
    json_size n = json_write_utf8_char(code_point, buf);

    if (json_string_append(string, buf, n)) {
        return JSON_ERRC_NOT_ENOUGH_MEMORY;
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_null(struct json_reader *r)
{
    if (r->first - r->last < 4 || *r->first++ != 'n' || *r->first++ != 'u' ||
        *r->first++ != 'l' || *r->first++ != 'l') {
        return JSON_ERRC_UNEXPECTED_TOKEN;
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_bool(
    struct json_reader *r, json_bool *value)
{
    if (r->first == r->last) {
        return JSON_ERRC_UNEXPECTED_TOKEN;
    } else if (*r->first == 't') {
        if (r->last - r->first < 3 || *++r->first != 'r' ||
            *++r->first != 'u' || *++r->first != 'e') {
            return JSON_ERRC_UNEXPECTED_TOKEN;
        }

        *value = 1;
    } else if (*r->first == 'f') {
        if (r->last - r->first < 4 || *++r->first != 'a' ||
            *++r->first != 'l' || *++r->first != 's' || *++r->first != 'e') {
            return JSON_ERRC_UNEXPECTED_TOKEN;
        }

        *value = 0;
    } else {
        return JSON_ERRC_UNEXPECTED_TOKEN;
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_int(
    struct json_reader *r, json_int *dest)
{
    json_bool negative;
    json_int value;

    if (r->last - r->first < 2) {
        return JSON_ERRC_UNEXPECTED_TOKEN;
    }

    negative = *r->first == '-';
    if (negative && ++r->first == r->last) {
        return JSON_ERRC_UNEXPECTED_TOKEN;
    }

    value = (unsigned char)*r->first;
    if (value > 10) {
        return JSON_ERRC_UNEXPECTED_TOKEN;
    } else if (value) {
        while (++r->first != r->last) {
            json_int next_value = (unsigned char)*r->first;

            if (next_value > 10) {
                break;
            }

            // TODO: Ensure defined behavior.
            next_value += 10 * value;
            if (next_value < value) {
                return JSON_ERRC_NUMBER_OUT_OF_RANGE;
            }

            value = next_value;
        }

        if (negative) {
            value = -value;
        }
    }

    *dest = value;
    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_float(
    struct json_reader *r, json_float *value);

static enum json_errc json_reader_read_string(
    struct json_reader *r, struct json_string *value);

/** pre-declaration */
static enum json_errc json_reader_read_value(
    struct json_reader *r, struct json_value *value);

static enum json_errc json_reader_read_array(
    struct json_reader *r, struct json_array *array)
{
    enum json_errc ec;

    json_reader_consume_space(r);

    if (r->first == r->last || *r->first != '[') {
        return JSON_ERRC_UNEXPECTED_TOKEN;
    }

    ++r->first;
    json_reader_consume_space(r);

    while (++r->first != r->last) {
        if (*r->first == ']') {
            ++r->first;
            break;
        }

        if ((ec = json_array_emplace_back(array, NULL)) ||
            (ec = json_reader_read_value(r, json_array_back(array)))) {
            return ec;
        }

        json_reader_consume_space(r);

        if (*r->first == ',') {
            ++r->first;
        }
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_object(
    struct json_reader *r, struct json_object *object);

static enum json_errc json_reader_read_value_null(
    struct json_reader *r, struct json_value *value)
{
    enum json_errc ec;

    if ((ec = json_reader_read_null(r))) {
        return ec;
    }

    json_value_assign_null(value);
    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_value_bool(
    struct json_reader *r, struct json_value *value)
{
    json_bool new_value;
    enum json_errc ec;

    if ((ec = json_reader_read_bool(r, &new_value))) {
        return ec;
    }

    json_value_assign_bool(value, new_value);
    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_value_number(
    struct json_reader *r, struct json_value *value);

static enum json_errc json_reader_read_value_string(
    struct json_reader *r, struct json_value *value)
{
    struct json_string string;
    enum json_errc ec;

    json_string_construct(&string, json_value_get_allocator(value));

    if ((ec = json_reader_read_string(r, &string)) ||
        (ec = json_value_assign_string_move(value, &string))) {
        return ec;
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_value_array(
    struct json_reader *r, struct json_value *value)
{
    struct json_array array;
    enum json_errc ec;

    json_array_construct(&array, json_value_get_allocator(value));

    if ((ec = json_reader_read_array(r, &array)) ||
        (ec = json_value_assign_array_move(value, &array))) {
        return ec;
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_value_object(
    struct json_reader *r, struct json_value *value)
{
    struct json_object object;
    enum json_errc ec;

    json_object_construct(&object, json_value_get_allocator(value));

    if ((ec = json_reader_read_object(r, &object)) ||
        (ec = json_value_assign_object_move(value, &object))) {
        return ec;
    }

    return JSON_ERRC_OK;
}

static enum json_errc json_reader_read_value(
    struct json_reader *r, struct json_value *value)
{
    json_reader_consume_space(r);

    if (r->first == r->last) {
        return JSON_ERRC_UNEXPECTED_TOKEN;
    }

    switch (*r->first) {
    case 'n':
        return json_reader_read_value_null(r, value);
    case 't':
    case 'f':
        return json_reader_read_value_bool(r, value);
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return json_reader_read_value_number(r, value);
    case '"':
        return json_reader_read_value_string(r, value);
    case '[':
        return json_reader_read_value_array(r, value);
    case '{':
        return json_reader_read_value_object(r, value);
    default:
        return JSON_ERRC_UNEXPECTED_TOKEN;
    }
}

struct json_read_result json_read_null(
    const char *first, const char *last,
    const struct json_read_options *options)
{
    struct json_reader r = json_make_reader(first, last, options);
    enum json_errc ec = json_reader_read_null(&r);
    return json_make_read_result(r.first, ec);
}

struct json_read_result json_read_bool(
    const char *first, const char *last, json_bool *value,
    const struct json_read_options *options)
{
    struct json_reader r = json_make_reader(first, last, options);
    enum json_errc ec = json_reader_read_bool(&r, value);
    return json_make_read_result(r.first, ec);
}

struct json_read_result json_read_int(
    const char *first, const char *last, json_int *value,
    const struct json_read_options *options)
{
    struct json_reader r = json_make_reader(first, last, options);
    enum json_errc ec = json_reader_read_int(&r, value);
    return json_make_read_result(r.first, ec);
}

struct json_read_result json_read_float(
    const char *first, const char *last, json_float *value,
    const struct json_read_options *options)
{
    struct json_reader r = json_make_reader(first, last, options);
    enum json_errc ec = json_reader_read_float(&r, value);
    return json_make_read_result(r.first, ec);
}

struct json_read_result json_read_string(
    const char *first, const char *last, struct json_string *value,
    const struct json_read_options *options)
{
    struct json_reader r = json_make_reader(first, last, options);
    enum json_errc ec = json_reader_read_string(&r, value);
    return json_make_read_result(r.first, ec);
}

struct json_read_result json_read_array(
    const char *first, const char *last, struct json_array *value,
    const struct json_read_options *options)
{
    struct json_reader r = json_make_reader(first, last, options);
    enum json_errc ec = json_reader_read_array(&r, value);
    return json_make_read_result(r.first, ec);
}

struct json_read_result json_read_object(
    const char *first, const char *last, struct json_object *value,
    const struct json_read_options *options)
{
    struct json_reader r = json_make_reader(first, last, options);
    enum json_errc ec = json_reader_read_object(&r, value);
    return json_make_read_result(r.first, ec);
}

struct json_read_result json_read_value(
    const char *first, const char *last, struct json_value *value,
    const struct json_read_options *options)
{
    struct json_reader r = json_make_reader(first, last, options);
    enum json_errc ec = json_reader_read_value(&r, value);
    return json_make_read_result(r.first, ec);
}

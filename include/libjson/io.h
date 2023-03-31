/**
 * @file libjson/io.h
 *
 * JSON Input Output
 */
#ifndef LIBJSON_PARSER_H_
#define LIBJSON_PARSER_H_

#include <stddef.h>
#include <stdio.h>
#include <libjson/errc.h>
#include <libjson/fwd.h>
#include <libjson/memory.h>

/**
 * @defgroup IO Input/Output
 * @{
 */

/**
 * Options for json read operations.
 */
struct json_read_options {
    /**
     * Max depth of json value while parsing.
     *
     * If the max depth is reached during parsing, `JSON_ERRC_MAX_DEPTH` will
     * be returned.
     */
    json_size max_depth;

    /**
     * Accept invalid code points.
     *
     * If set to `json_true`, invalid UTF-8 inside of strings will not be
     * rejected by the parser, and copied or replaced based on the
     * `replace_invalie_code_point` option.
     *
     * If set to `json_false`, invalid UTF-8 will be rejected and
     * `JSON_ERRC_INVALID_ENCODING` will be returned.
     */
    json_bool accept_invalid_code_points;

    /**
     * Replace invalid code points.
     *
     * If `replace_invalid_code_points` and `accept_invalid_code_points` are
     * set to `json_true`, then invalid code points will be replaced by the
     * [Unicode Replacement Character][1] when copied into the string.
     *
     * If `replace_invalid_code_points` is `json_false` and
     * `accept_invalid_code_points` is `json_true`, then invalid code points
     * will be copied directly to the string.
     *
     * If `accept_invalid_code_points` is `json_false`, then this option has
     * no effect.
     *
     * [1]: https://unicode.org/glossary/#replacement_character
     */
    json_bool replace_invalid_code_points;

    /**
     * Accept trailing commas.
     *
     * If set to `json_true`, trailing commas will be accepted, otherwise
     *
     * If set to `json_false`, trailing commas will be rejected.
     *
     * A trailing comma is one after the last entry of an array, or last entry
     * of an object before the closing braces of each.
     */
    json_bool accept_trailing_commas;

    /**
     * Accept trailing commas.
     *
     * If set to `json_true`, comments will be accepted.
     *
     * If set to `json_false`, comments will be rejected.
     *
     * Comments may be `C` or `C++` style.
     */
    json_bool accept_comments;

    /**
     * Accept duplicate keys.
     *
     * If set to `json_true`, duplicate keys in objects will be accepted. Later
     * entries with a duplicate key will overwrite their former entries.
     *
     * If set to `json_false`, duplicate keys will be rejected and the read
     * operation will return `JSON_ERRC_DUPLICATE_KEY`.
     */
    json_bool accept_duplicate_keys;
};

struct json_write_options {
    json_size indent_size;
};

/**
 * The result of read operation.
 */
struct json_read_result {
    /**
     * Pointer to the next character after the read operation.
     */
    const char *ptr;

    /**
     * Read error.
     */
    enum json_errc ec;
};

struct json_write_result {
    /**
     * Pointer to the next character after the write operation.
     */
    char *ptr;

    /**
     * Write error.
     */
    enum json_errc ec;
};

struct json_read_result json_read_null(
    const char *first, const char *last,
    const struct json_read_options *options);

struct json_read_result json_read_bool(
    const char *first, const char *last, json_bool *value,
    const struct json_read_options *options);

struct json_read_result json_read_int(
    const char *first, const char *last, json_int *value,
    const struct json_read_options *options);

struct json_read_result json_read_float(
    const char *first, const char *last, json_float *value,
    const struct json_read_options *options);

struct json_read_result json_read_string(
    const char *first, const char *last, struct json_string *value,
    const struct json_read_options *options);

struct json_read_result json_read_array(
    const char *first, const char *last, struct json_array *value,
    const struct json_read_options *options);

struct json_read_result json_read_object(
    const char *first, const char *last, struct json_object *value,
    const struct json_read_options *options);

struct json_read_result json_read_value(
    const char *first, const char *last, struct json_value *value,
    const struct json_read_options *options);

struct json_write_result json_write_null(
    char *first, char *last, const struct json_write_options *options);

struct json_write_result json_write_bool(
    char *first, char *last, json_bool value,
    const struct json_write_options *options);

struct json_write_result json_write_int(
    char *first, char *last, json_int value,
    const struct json_write_options *options);

struct json_write_result json_write_float(
    char *first, char *last, json_float value,
    const struct json_write_options *options);

struct json_write_result json_write_string(
    char *first, char *last, const struct json_string *value,
    const struct json_write_options *options);

struct json_write_result json_write_array(
    char *first, char *last, const struct json_array *value,
    const struct json_write_options *options);

struct json_write_result json_write_object(
    char *first, char *last, const struct json_object *value,
    const struct json_write_options *options);

struct json_write_result json_write_value(
    char *first, char *last, const struct json_value *value,
    const struct json_write_options *options);

/**
 * @}
 */

#endif

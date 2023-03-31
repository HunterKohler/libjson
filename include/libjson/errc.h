/**
 * @file libjson/errc.h
 *
 * JSON Error Handling
 */
#ifndef LIBJSON_ERRC_H_
#define LIBJSON_ERRC_H_

/**
 * @defgroup Error Error
 * @{
 */

enum json_errc {
    JSON_ERRC_OK = 0,
    JSON_ERRC_NOT_ENOUGH_MEMORY,
    JSON_ERRC_UNEXPECTED_TOKEN,
    JSON_ERRC_INVALID_ESCAPE,
    JSON_ERRC_INVALID_ENCODING,
    JSON_ERRC_MAX_DEPTH,
    JSON_ERRC_NUMBER_OUT_OF_RANGE,
    JSON_ERRC_DUPLICATE_KEY,
};

const char *json_errc_message(enum json_errc err);

/**
 * @}
 */

#endif

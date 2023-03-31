/**
 * @file libjson/type.h
 *
 * JSON Type
 */
#ifndef LIBJSON_TYPE_H_
#define LIBJSON_TYPE_H_

/**
 * @defgroup Type Type
 * @{
 */

enum json_type {
    JSON_TYPE_NULL,
    JSON_TYPE_BOOL,
    JSON_TYPE_INT,
    JSON_TYPE_FLOAT,
    JSON_TYPE_STRING,
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT,
};

/**
 * @}
 */

#endif

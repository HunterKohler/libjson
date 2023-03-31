/**
 * @file libjson/entry.h
 *
 * JSON Object Entry
 */
#ifndef LIBJSON_ENTRY_H_
#define LIBJSON_ENTRY_H_

#include <libjson/fwd.h>
#include <libjson/string.h>
#include <libjson/value.h>

/**
 * @defgroup Entry Entry
 * @{
 */

struct json_entry {
    /** @private */
    struct json_entry *_next;

    /** @private */
    struct json_entry *_prev;

    /** @private */
    struct json_string _key;

    /** @private */
    struct json_value _value;
};

const struct json_string *json_entry_key(const struct json_entry *entry);

struct json_value *json_entry_value(struct json_entry *entry);

/**
 * @}
 */

#endif

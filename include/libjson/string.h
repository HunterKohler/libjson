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
 * JSON String
 * @{
 */

/**
 * Represents a JSON string.
 *
 * The data is always null terminated.
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

/**
 * Default construct empty string.
 *
 * @param string String to initialize.
 * @param alloc Allocator to use. If `NULL`, the default allocator is used.
 */
void json_string_construct(
    struct json_string *string, struct json_allocator *alloc);

/**
 * Copy construct string.
 *
 * Errors:
 * - `JSON_ERRC_NOT_ENOUGH_MEMORY`
 *
 * @param string String to initialize.
 * @param other String to copy.
 * @param alloc Allocator to use. If `Null`, the default allocator is used.
 */
enum json_errc json_string_construct_copy(
    struct json_string *string, const struct json_string *other,
    struct json_allocator *alloc);

enum json_errc json_string_construct_move(
    struct json_string *string, struct json_string *other,
    struct json_allocator *alloc);

/**
 * Destruct string.
 *
 * @param string String to destroy.
 */
void json_string_destruct(struct json_string *string);

/**
 * Copy assign string.
 *
 * Copy assigns the value of `other` to `string`. Nothing occurs if `string`
 * and `other` point to the same object.
 *
 * Errors:
 * - `JSON_ERRC_NOT_ENOUGH_MEMORY`
 *
 * @param string String to assign.
 * @param other String to copy.
 */
enum json_errc json_string_assign_copy(
    struct json_string *string, const struct json_string *other);

/**
 * Move assign string.
 *
 * If the allocators are equal, the value of `other` is moved to `string`;
 * otherwise, the value of `other` is copied to `string`. There can be no
 * errors if the allocators are equal. Nothing occurs if `string` and `other`
 * point to the same object.
 *
 * Errors:
 * - `JSON_ERRC_NOT_ENOUGH_MEMORY`
 *
 * @param string
 * @param other
 */
enum json_errc json_string_assign_move(
    struct json_string *string, struct json_string *other);

/**
 * Returns the associated allocator with the string.
 *
 * @param string
 */
struct json_allocator *json_string_get_allocator(
    const struct json_string *string);

/**
 * Check if the string is empty.
 *
 * Returns true if the string is empty, and false otherwise.
 *
 * @param string
 */
json_bool json_string_empty(const struct json_string *string);

/**
 * Get string size.
 *
 * Returns the size of string; that is, the number of characters in the string
 * not including the null terminator.
 *
 * @param string
 */
json_size json_string_size(const struct json_string *string);

/**
 * Get string capacity.
 *
 * Returns the capacity of string; that is, the number of characters that can be
 * represented without re-allocation not including the null terminator.
 *
 * @param string
 */
json_size json_string_capacity(const struct json_string *string);

/**
 * Clear the string.
 *
 * @param string
 */
void json_string_clear(struct json_string *string);

/**
 * Reserve space.
 *
 * The string reserves at least enough space for at least `n` characters, not
 * including the null terminator. An allocation only occurs if
 * `n < json_string_capacity(string)`.
 *
 * Errors:
 * - `JSON_ERRC_NOT_ENOUGH_MEMORY`
 *
 * @param string
 * @param n The minimum number of characters of to reserve.
 */
enum json_errc json_string_reserve(struct json_string *string, json_size n);

/**
 * Resize string.
 *
 * If `new_size` is less than or equal to current string size, the string is
 * shortened to match `new_size`; otherwise, the string is grown to `new_size`
 * and the new characters of the string are filled with `c`.
 *
 * Errors:
 * - `JSON_ERRC_NOT_ENOUGH_MEMORY`
 *
 * @param string
 * @param new_size New size.
 * @param c Character to fill empty space.
 */
enum json_errc json_string_resize(
    struct json_string *string, json_size new_size, char c);

/**
 * Reduce memory usage by freeing unused memory.
 *
 * All unused memory will be freed, reducing the capacity of the string.
 *
 * Errors:
 * - `JSON_ERRC_NOT_ENOUGH_MEMORY`
 *
 * @param string
 * @return enum json_errc
 */
enum json_errc json_string_shrink_to_fit(struct json_string *string);

/**
 * Get a pointer to the first character of a string.
 *
 * The behavior of this operation is undefined if the string is empty.
 *
 * @param string
 */
char *json_string_front(struct json_string *string);

/**
 * Get a pointer to the last character of a string.
 *
 * The behavior of this operation is undefined if the string is empty
 *
 * @param string
 */
char *json_string_back(struct json_string *string);

/**
 * Get a pointer to a character in the string.
 *
 * The behavior of this operation is undefined if `pos` is not less that
 * the string size.
 *
 * @param string
 * @param pos Index fo character to retrieve.
 */
char *json_string_at(struct json_string *string, json_size pos);

/**
 * Get the data pointer of a string.
 *
 * @param string
 */
char *json_string_data(struct json_string *string);

/**
 * Swaps the contents of two strings.
 *
 * The behavior of this operation is undefined if the two strings do not have
 * equal allocators.
 *
 * @param a
 * @param b
 */
void json_string_swap(struct json_string *a, struct json_string *b);

/**
 * Compares two strings.
 *
 * Returns `-1` if `a` precedes `b` lexicographically, `1` if `a` succeeds
 * `b` lexigraphically, and `0` if the two strings are equal.
 *
 * @param a
 * @param b
 */
int json_string_compare(
    const struct json_string *a, const struct json_string *b);

/**
 * Copy the characters of a string to a memory region.
 *
 * `dest` may not point to a location within the string. If `start + count` is
 * greater that the string size, the behavior of the operation is undefiend.
 *
 * @param string Source string.
 * @param start Position to start copying characters.
 * @param count Number of characters to copy.
 * @param dest Destination buffer.
 */
void json_string_copy(const struct json_string *string, json_size start,
                      json_size count, char *dest);

/**
 * Remove the last character of a string.
 *
 * The last character of the string is removed, its size is decreased by `1`.
 * If the string is empty, the behavior of this operation is undefined.
 *
 * @param string
 */
void json_string_pop_back(struct json_string *string);

/**
 * Append a single character to a string.
 *
 * Errors:
 * - `JSON_ERRC_NOT_ENOUGH_MEMORY`
 *
 * @param string
 * @param c
 */
enum json_errc json_string_push_back(struct json_string *string, char c);

/**
 * Append multiple characters to a string.
 *
 * Errors:
 * - `JSON_ERRC_NOT_ENOUGH_MEMORY`
 *
 * @param string
 * @param src
 * @param count
 */
enum json_errc json_string_append(
    struct json_string *string, const char *src, json_size count);

/**
 * Insert multiple characters into a string.
 *
 * Errors:
 * - `JSON_ERRC_NOT_ENOUGH_MEMORY`
 *
 * @param string
 * @param pos
 * @param src
 * @param count
 */
enum json_errc json_string_insert(
    struct json_string *string, json_size pos, const char *src,
    json_size count);

/**
 * Erase multiple characters from a string.
 *
 * @param string
 * @param pos
 * @param count
 */
enum json_errc json_string_erase(
    struct json_string *string, json_size pos, json_size count);

/**
 * Allocates and default constructs a string.
 *
 * If the operation fails, `NULL` is returned.
 *
 * @param alloc Allocator to use.
 */
struct json_string *json_string_new(struct json_allocator *alloc);

/**
 * Allocates and copy constructs a string.
 *
 * If the operation fails, `NULL` is returned.
 *
 * @param string
 * @param alloc
 */
struct json_string *json_string_new_copy(
    const struct json_string *string, struct json_allocator *alloc);

/**
 * Allocates and move constructs a string.
 *
 * If the operation fails, `NULL` is returned.
 *
 * @param string
 * @param alloc
 */
struct json_string *json_string_new_move(
    struct json_string *string, struct json_allocator *alloc);

/**
 * Destructs and deallocates a string.
 *
 * @param string
 */
void json_string_delete(struct json_string *string);

/**
 * @}
 */

#endif

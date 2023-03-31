/**
 * @file libjson/memory.h
 *
 * JSON Memory
 */
#ifndef LIBJSON_MEMORY_H_
#define LIBJSON_MEMORY_H_

#include <stdlib.h>
#include <libjson/fwd.h>

/**
 * @defgroup Memory Memory
 * @{
 */

struct json_allocator_methods {
    void *(*allocate)(struct json_allocator *self, json_size bytes,
                      json_size alignment);

    void (*deallocate)(struct json_allocator *self, void *p, json_size bytes,
                       json_size alignment);

    json_bool (*is_equal)(
        const struct json_allocator *self, const struct json_allocator *other);
};

struct json_allocator {
    /** @private */
    const struct json_allocator_methods *_methods;
};

void json_allocator_construct(
    struct json_allocator *alloc, struct json_allocator_methods *methods);

void *json_allocator_allocate(
    struct json_allocator *alloc, json_size bytes, json_size alignment);

void json_allocator_deallocate(struct json_allocator *alloc, void *p,
                               json_size bytes, json_size alignment);

json_bool json_allocator_is_equal(
    const struct json_allocator *a, const struct json_allocator *b);

const struct json_allocator_methods *json_allocator_get_methods(
    const struct json_allocator *alloc);

struct json_allocator *json_null_allocator(void);

struct json_allocator *json_stdc_allocator(void);

struct json_allocator *json_get_default_allocator(void);

struct json_allocator *json_set_default_allocator(
    struct json_allocator *new_default);

/**
 * @}
 */

#endif

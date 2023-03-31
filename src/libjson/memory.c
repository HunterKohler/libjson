#include <stdatomic.h>
#include <stdlib.h>
#include <libjson/memory.h>

void json_allocator_construct(
    struct json_allocator *alloc, struct json_allocator_methods *methods)
{
    alloc->_methods =
        methods ? methods : json_get_default_allocator()->_methods;
}

void *json_allocator_allocate(
    struct json_allocator *alloc, json_size bytes, json_size alignment)
{
    return alloc->_methods->allocate(alloc, bytes, alignment);
}

void json_allocator_deallocate(
    struct json_allocator *alloc, void *p, json_size bytes, json_size alignment)
{
    alloc->_methods->deallocate(alloc, p, bytes, alignment);
}

json_bool json_allocator_is_equal(
    const struct json_allocator *a, const struct json_allocator *b)
{
    return a->_methods->is_equal(a, b);
}

const struct json_allocator_methods *json_allocator_get_methods(
    const struct json_allocator *alloc)
{
    return alloc->_methods;
}

static void *json_null_allocator_allocate(
    struct json_allocator *, json_size, json_size)
{
    return NULL;
}

static void json_null_allocator_deallocate(
    struct json_allocator *, void *, json_size, json_size)
{}

static json_bool json_null_allocator_is_equal(
    const struct json_allocator *self, const struct json_allocator *other)
{
    return self == other;
}

static const struct json_allocator_methods json_null_allocator_methods = {
    .allocate = json_null_allocator_allocate,
    .deallocate = json_null_allocator_deallocate,
    .is_equal = json_null_allocator_is_equal,
};

static struct json_allocator json_null_allocator_value = {
    &json_null_allocator_methods
};

struct json_allocator *json_null_allocator(void)
{
    return &json_null_allocator_value;
}

static void *json_stdc_allocator_allocate(
    struct json_allocator *, json_size bytes, json_size alignment)
{
    return aligned_alloc(alignment, bytes);
}

static void json_stdc_allocator_deallocate(
    struct json_allocator *, void *p, json_size, json_size)
{
    free(p);
}

static json_bool json_stdc_allocator_is_equal(
    const struct json_allocator *self, const struct json_allocator *other)
{
    return self->_methods == other->_methods;
}

static const struct json_allocator_methods json_stdc_allocator_methods = {
    .allocate = json_stdc_allocator_allocate,
    .deallocate = json_stdc_allocator_deallocate,
    .is_equal = json_stdc_allocator_is_equal,
};

static struct json_allocator json_stdc_allocator_value = {
    ._methods = &json_stdc_allocator_methods
};

struct json_allocator *json_stdc_allocator(void)
{
    return &json_stdc_allocator_value;
}

static struct json_allocator *_Atomic json_default_allocator =
    &json_stdc_allocator_value;

struct json_allocator *json_get_default_allocator(void)
{
    return atomic_load(&json_default_allocator);
}

struct json_allocator *json_set_default_allocator(
    struct json_allocator *new_default)
{
    return atomic_exchange(&json_default_allocator, new_default);
}

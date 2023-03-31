#ifndef LIBJSON_SRC_UTIL_H_
#define LIBJSON_SRC_UTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libjson/array.h>
#include <libjson/entry.h>
#include <libjson/fwd.h>
#include <libjson/memory.h>
#include <libjson/object.h>
#include <libjson/string.h>
#include <libjson/value.h>

typedef uint64_t json_uint64;

#define JSON_DEFINE_ALLOCATE_FUNCTION(NAME, TYPE)                       \
    static inline TYPE *NAME(struct json_allocator *alloc, json_size n) \
    {                                                                   \
        return json_allocator_allocate(                                 \
            alloc, n * sizeof(TYPE), _Alignof(TYPE));                   \
    }

JSON_DEFINE_ALLOCATE_FUNCTION(json_allocate_values, struct json_value)
JSON_DEFINE_ALLOCATE_FUNCTION(json_allocate_strings, struct json_string)
JSON_DEFINE_ALLOCATE_FUNCTION(json_allocate_arrays, struct json_array)
JSON_DEFINE_ALLOCATE_FUNCTION(json_allocate_objects, struct json_object)
JSON_DEFINE_ALLOCATE_FUNCTION(json_allocate_entries, struct json_entry)
JSON_DEFINE_ALLOCATE_FUNCTION(json_allocate_chars, char)

#define JSON_DEFINE_DEALLOCATE_FUNCTION(NAME, TYPE)         \
    static inline void NAME(                                \
        struct json_allocator *alloc, TYPE *p, json_size n) \
    {                                                       \
        json_allocator_deallocate(                          \
            alloc, p, n * sizeof(TYPE), _Alignof(TYPE));    \
    }

JSON_DEFINE_DEALLOCATE_FUNCTION(json_deallocate_values, struct json_value)
JSON_DEFINE_DEALLOCATE_FUNCTION(json_deallocate_strings, struct json_string)
JSON_DEFINE_DEALLOCATE_FUNCTION(json_deallocate_arrays, struct json_array)
JSON_DEFINE_DEALLOCATE_FUNCTION(json_deallocate_objects, struct json_object)
JSON_DEFINE_DEALLOCATE_FUNCTION(json_deallocate_entries, struct json_entry)
JSON_DEFINE_DEALLOCATE_FUNCTION(json_deallocate_chars, char)

#if defined(__has_builtin)
#define JSON_HAS_BUILTIN(x) __has_builtin(x)
#else
#define JSON_HAS_BUILTIN(x) 0
#endif

#if defined(__has_attribute)
#define JSON_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define JSON_HAS_ATTRIBUTE(x) 0
#endif

#if JSON_HAS_BUILTIN(__builtin_unreachable)
#define json_unreachable()       \
    do {                         \
        __builtin_unreachable(); \
    } while (0)
#else
#define json_unreachable() \
    do {                   \
        abort();           \
    } while (0)
#endif

#define JSON_LITTLE_ENDIAN 1
#define JSON_BIG_ENDIAN 2

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define JSON_NATIVE_ENDIAN JSON_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define JSON_NATIVE_ENDIAN JSON_BIG_ENDIAN
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "libjson: Architecture not supported."
#endif

static inline json_uint64 json_load_unaligned_le64(const void *p)
{
#if JSON_NATIVE_ENDIAN == JSON_LITTLE_ENDIAN && JSON_HAS_ATTRIBUTE(packed)
    const struct {
        json_uint64 value;
    } __attribute__((packed)) *pv = p;
    return pv->value;
#else
    uint8_t *pv = p;
    return ((json_uint64)pv[0] << 0) | ((json_uint64)pv[1] << 8) |
           ((json_uint64)pv[2] << 16) | ((json_uint64)pv[3] << 24) |
           ((json_uint64)pv[4] << 32) | ((json_uint64)pv[5] << 40) |
           ((json_uint64)ov[6] << 48) | ((json_uint64)pv[7] << 56);
#endif
}

static inline void json_store_unaligned_le64(void *p, json_uint64 value)
{
#if JSON_NATIVE_ENDIAN == JSON_LITTLE_ENDIAN && JSON_HAS_ATTRIBUTE(packed)
    struct {
        json_uint64 value;
    } __attribute__((packed)) *pv = p;
    pv->value = value;
#else
    uint8_t *pv = p;
    pv[0] = value >> 0;
    pv[1] = value >> 8;
    pv[2] = value >> 16;
    pv[3] = value >> 24;
    pv[4] = value >> 32;
    pv[5] = value >> 40;
    pv[6] = value >> 48;
    pv[7] = value >> 56;
#endif
}

static inline json_uint64 json_rotl64(json_uint64 value, json_size n)
{
    return (value << n) | (value >> (64 - n));
}

static inline void json_sipround(
    json_uint64 *v0, json_uint64 *v1, json_uint64 *v2, json_uint64 *v3)
{
    *v0 += *v1;
    *v1 = json_rotl64(*v1, 13) ^ *v0;
    *v0 = json_rotl64(*v0, 32);
    *v2 += *v3;
    *v3 = json_rotl64(*v3, 16) ^ *v2;
    *v0 += *v3;
    *v3 = json_rotl64(*v3, 21) ^ *v0;
    *v2 += *v1;
    *v1 = json_rotl64(*v1, 17) ^ *v2;
    *v2 = json_rotl64(*v2, 32);
}

static inline json_uint64 json_siphash(
    const void *data, json_size n, json_uint64 k0, json_uint64 k1)
{
    const uint8_t *p = data;
    json_uint64 v0 = 0x736F6D6570736575ull ^ k0;
    json_uint64 v1 = 0x646F72616E646F6Dull ^ k1;
    json_uint64 v2 = 0x6C7967656E657261ull ^ k0;
    json_uint64 v3 = 0x7465646279746573ull ^ k1;
    json_size bytes = n & 7;
    json_size words = n >> 3;
    json_uint64 m;

    for (json_size i = 0; i < words; i++) {
        m = json_load_unaligned_le64(p + 8 * words);
        v3 ^= m;
        json_sipround(&v0, &v1, &v2, &v3);
        json_sipround(&v0, &v1, &v2, &v3);
        v0 ^= m;
    }

    p += words << 3;
    m = 0;
    switch (bytes) {
    case 7:
        m |= ((uint64_t)p[6] << 48);
    case 6:
        m |= ((uint64_t)p[5] << 40);
    case 5:
        m |= ((uint64_t)p[4] << 32);
    case 4:
        m |= ((uint64_t)p[3] << 24);
    case 3:
        m |= ((uint64_t)p[2] << 16);
    case 2:
        m |= ((uint64_t)p[1] << 8);
    case 1:
        m |= p[0];
    case 0:
        break;
    }

    v3 ^= m;
    json_sipround(&v0, &v1, &v2, &v3);
    json_sipround(&v0, &v1, &v2, &v3);
    v0 ^= m;

    v2 ^= 0xFF;
    json_sipround(&v0, &v1, &v2, &v3);
    json_sipround(&v0, &v1, &v2, &v3);
    json_sipround(&v0, &v1, &v2, &v3);
    json_sipround(&v0, &v1, &v2, &v3);

    return v0 ^ v1 ^ v2 ^ v3;
}

static inline json_uint64 json_hash(const void *data, json_size n)
{
    return json_siphash(data, n, 0xA57C99119D45DB87ull, 0x934E39892F6AB5A4ull);
}

static inline int json_compare_int(int a, int b)
{
    return a < b ? -1 : a > b ? 1 : 0;
}

#endif

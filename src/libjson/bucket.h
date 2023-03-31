#ifndef LIBJSON_SRC_BUCKET_H_
#define LIBJSON_SRC_BUCKET_H_

#include <libjson/entry.h>

struct json_bucket {
    struct json_entry *_first;
};

#endif

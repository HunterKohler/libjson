/**
 * @file libjson/visitor.h
 *
 * JSON Visitor
 */
#ifndef LIBJSON_VISITOR_H_
#define LIBJSON_VISITOR_H_

#include <libjson/fwd.h>

/**
 * @defgroup Visitor Visitor
 * JSON Visitor
 * @{
 */

struct json_visitor {
    int (*on_null)(struct json_visitor *self, struct json_value *value);
    int (*on_bool)(struct json_visitor *self, struct json_value *value);
    int (*on_int)(struct json_visitor *self, struct json_value *value);
    int (*on_float)(struct json_visitor *self, struct json_value *value);
    int (*on_string)(struct json_visitor *self, struct json_value *value);
    int (*on_array)(struct json_visitor *self, struct json_value *value);
    int (*on_object)(struct json_visitor *self, struct json_value *value);
};

int json_visit(struct json_visitor *vis, struct json_value *value);

/**
 * @}
 */

#endif

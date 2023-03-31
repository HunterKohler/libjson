#include <libjson/fwd.h>
#include <libjson/type.h>
#include <libjson/value.h>
#include <libjson/visitor.h>
#include "./util.h"

int json_visit(struct json_visitor *vis, struct json_value *value)
{
    switch (value->_type) {
    case JSON_TYPE_NULL:
        return vis->on_null(vis, value);
    case JSON_TYPE_BOOL:
        return vis->on_bool(vis, value);
    case JSON_TYPE_INT:
        return vis->on_int(vis, value);
    case JSON_TYPE_FLOAT:
        return vis->on_float(vis, value);
    case JSON_TYPE_STRING:
        return vis->on_string(vis, value);
    case JSON_TYPE_ARRAY:
        return vis->on_array(vis, value);
    case JSON_TYPE_OBJECT:
        return vis->on_object(vis, value);
    default:
        json_unreachable();
    }
}

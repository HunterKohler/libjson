#include <libjson/errc.h>
#include "./util.h"

const char *json_errc_message(enum json_errc ec)
{
    switch (ec) {
    case JSON_ERRC_OK:
        return "";
    case JSON_ERRC_NOT_ENOUGH_MEMORY:
        return "not enough memory";
    default:
        json_unreachable();
    }
}

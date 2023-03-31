#include <libjson/entry.h>

const struct json_string *json_entry_key(const struct json_entry *entry)
{
    return &entry->_key;
}

struct json_value *json_entry_value(struct json_entry *entry)
{
    return &entry->_value;
}

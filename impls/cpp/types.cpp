#include "types.hpp"

std::string ListValue::inspect() {
    std::string out = "(";
    for (auto *value : m_list) {
        out.append(value->inspect());
        out.append(" ");
    }
    if (m_list.size() > 0) {
        out[out.length() - 1] = ')';
    } else {
        out.append(")");
    }
    return out;
}

std::string VectorValue::inspect() {
    std::string out = "[";
    for (auto *value : m_list) {
        out.append(value->inspect());
        out.append(" ");
    }
    if (m_list.size() > 0) {
        out[out.length() - 1] = ']';
    } else {
        out.append("]");
    }
    return out;
}

std::string HashMapValue::inspect() {
    std::string out = "{";
    for (auto pair : m_map) {
        out.append(pair.first->inspect());
        out.append(" ");
        out.append(pair.second->inspect());
        out.append(" ");
    }
    if (m_map.size() > 0) {
        out[out.length() - 1] = '}';
    } else {
        out.append("}");
    }
    return out;
}

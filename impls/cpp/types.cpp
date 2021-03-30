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

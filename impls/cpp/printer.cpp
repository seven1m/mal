#include "printer.hpp"

std::string pr_str(Value *value) {
    return value->inspect();
}

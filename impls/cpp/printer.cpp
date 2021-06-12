#include "printer.hpp"

std::string pr_str(Value *value, bool print_readably) {
    return value->inspect(print_readably);
}

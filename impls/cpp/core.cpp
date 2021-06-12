#include <iostream>

#include "core.hpp"
#include "printer.hpp"

std::unordered_map<std::string, Function> build_namespace() {
    std::unordered_map<std::string, Function> ns;
    ns["+"] = add;
    ns["-"] = subtract;
    ns["*"] = multiply;
    ns["/"] = divide;
    ns["prn"] = prn;
    ns["pr-str"] = pr_str_function;
    ns["println"] = println;
    ns["str"] = str;
    ns["list"] = list;
    ns["list?"] = list_q;
    ns["empty?"] = empty_q;
    ns["count"] = count;
    ns["="] = eq;
    ns["<"] = lt;
    ns["<="] = lte;
    ns[">"] = gt;
    ns[">="] = gte;
    ns["not"] = not_function;
    return ns;
}

Value *add(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_integer()->to_long() + b->as_integer()->to_long();
    return new IntegerValue { result };
}

Value *subtract(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_integer()->to_long() - b->as_integer()->to_long();
    return new IntegerValue { result };
}

Value *multiply(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_integer()->to_long() * b->as_integer()->to_long();
    return new IntegerValue { result };
}

Value *divide(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_integer()->to_long() / b->as_integer()->to_long();
    return new IntegerValue { result };
}

// prn: call pr_str on the first parameter with print_readably set to true, prints the result to the screen and then return nil. Note that the full version of prn is a deferrable below.
Value *prn(size_t argc, Value **args) {
    if (argc == 0) {
        std::cout << "\n";
        return NilValue::the();
    }
    for (size_t i = 0; i < argc; ++i) {
        std::cout << pr_str(args[i], true);
        if (i < argc - 1)
            std::cout << ' ';
    }
    std::cout << "\n";
    return NilValue::the();
}

Value *pr_str_function(size_t argc, Value **args) {
    if (argc == 0)
        return new StringValue { "" };
    std::string str = "";
    for (size_t i = 0; i < argc; ++i) {
        str += pr_str(args[i], true);
        if (i < argc - 1)
            str += ' ';
    }
    return new StringValue { str };
}

// println: calls pr_str on each argument with print_readably set to false,
// joins the results with " ", prints the string to the screen and then
// returns nil.
Value *println(size_t argc, Value **args) {
    if (argc == 0) {
        std::cout << "\n";
        return NilValue::the();
    }
    std::string str = "";
    for (size_t i = 0; i < argc; ++i) {
        str += pr_str(args[i], false);
        if (i < argc - 1)
            str += ' ';
    }
    std::cout << str << "\n";
    return NilValue::the();
}

Value *str(size_t argc, Value **args) {
    if (argc == 0)
        return new StringValue { "" };
    std::string str = "";
    for (size_t i = 0; i < argc; ++i) {
        str += pr_str(args[i], false);
    }
    return new StringValue { str };
}

// list: take the parameters and return them as a list.
Value *list(size_t argc, Value **args) {
    auto l = new ListValue {};
    for (size_t i = 0; i < argc; ++i)
        l->push(args[i]);
    return l;
}

// list?: return true if the first parameter is a list, false otherwise.
Value *list_q(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_list())
        return TrueValue::the();
    return FalseValue::the();
}

// empty?: treat the first parameter as a list and return true
// if the list is empty and false if it contains any elements.
Value *empty_q(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_listy() && args[0]->as_list()->is_empty())
        return TrueValue::the();
    return FalseValue::the();
}

// count: treat the first parameter as a list and return the
// number of elements that it contains.
Value *count(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_listy())
        return new IntegerValue { static_cast<long>(args[0]->as_list()->size()) };
    return new IntegerValue { 0 };
}

// =: compare the first two parameters and return true if they are the same
// type and contain the same value. In the case of equal length lists,
// each element of the list should be compared for equality and if they
// are the same return true, otherwise false.
Value *eq(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];
    if (*a == b)
        return TrueValue::the();
    return FalseValue::the();
}

// <, <=, >, and >=: treat the first two parameters as numbers and do the corresponding numeric comparison, returning either true or false.
Value *lt(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];
    assert(a->is_integer());
    assert(b->is_integer());
    if (a->as_integer()->to_long() < b->as_integer()->to_long())
        return TrueValue::the();
    return FalseValue::the();
}

Value *lte(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];
    assert(a->is_integer());
    assert(b->is_integer());
    if (a->as_integer()->to_long() <= b->as_integer()->to_long())
        return TrueValue::the();
    return FalseValue::the();
}

Value *gt(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];
    assert(a->is_integer());
    assert(b->is_integer());
    if (a->as_integer()->to_long() > b->as_integer()->to_long())
        return TrueValue::the();
    return FalseValue::the();
}

Value *gte(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];
    assert(a->is_integer());
    assert(b->is_integer());
    if (a->as_integer()->to_long() >= b->as_integer()->to_long())
        return TrueValue::the();
    return FalseValue::the();
}

Value *not_function(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_truthy())
        return FalseValue::the();
    return TrueValue::the();
}

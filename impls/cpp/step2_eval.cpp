#include <iostream>
#include <string>
#include <unordered_map>

#include "linenoise.hpp"
#include "printer.hpp"
#include "reader.hpp"

using Env = std::unordered_map<SymbolValue *, Value *, HashMapHash, HashMapPred>;

Value *READ(std::string input) {
    return read_str(input);
}

Value *eval_ast(Value *ast, Env &env);

Value *EVAL(Value *ast, Env &env) {
    if (ast->type() != Value::Type::List) {
        return eval_ast(ast, env);
    } else if (ast->as_list()->is_empty()) {
        return ast;
    } else {
        auto list = eval_ast(ast, env)->as_list();
        auto fn = list->at(0)->as_fn()->to_fn();
        Value *args[list->size() - 1];
        for (size_t i = 1; i < list->size(); ++i) {
            args[i - 1] = list->at(i);
        }
        return fn(list->size() - 1, args);
    }
}

Value *eval_ast(Value *ast, Env &env) {
    switch (ast->type()) {
    case Value::Type::Symbol: {
        auto search = env.find(ast->as_symbol());
        if (search == env.end())
            throw new ExceptionValue { ast->as_symbol()->str() + " not found" };
        return search->second;
    }
    case Value::Type::List: {
        auto result = new ListValue {};
        for (auto val : *ast->as_list()) {
            result->push(EVAL(val, env));
        }
        return result;
    }
    case Value::Type::Vector: {
        auto result = new VectorValue {};
        for (auto val : *ast->as_vector()) {
            result->push(EVAL(val, env));
        }
        return result;
    }
    case Value::Type::HashMap: {
        auto result = new HashMapValue {};
        for (auto pair : *ast->as_hash_map()) {
            auto val = EVAL(pair.second, env);
            result->set(pair.first, val);
        }
        return result;
    }
    default:
        return ast;
    }
}

std::string PRINT(Value *input) { return pr_str(input, true); }

std::string rep(std::string input, Env &env) {
    try {
        auto ast = READ(input);
        auto result = EVAL(ast, env);
        return PRINT(result);
    } catch (ExceptionValue *exception) {
        std::cerr << exception->message() << std::endl;
        return "";
    }
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

Value *sub(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_integer()->to_long() - b->as_integer()->to_long();
    return new IntegerValue { result };
}

Value *mul(size_t argc, Value **args) {
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

int main() {
    const auto history_path = "history.txt";
    linenoise::LoadHistory(history_path);

    Env env {};
    env[new SymbolValue("+")] = new FnValue { add };
    env[new SymbolValue("-")] = new FnValue { sub };
    env[new SymbolValue("*")] = new FnValue { mul };
    env[new SymbolValue("/")] = new FnValue { divide };

    std::string input;
    for (;;) {
        auto quit = linenoise::Readline("user> ", input);
        if (quit)
            break;
        std::cout << rep(input, env) << std::endl;
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);

    return 0;
}

#include <iostream>
#include <string>
#include <unordered_map>

#include "core.hpp"
#include "env.hpp"
#include "linenoise.hpp"
#include "printer.hpp"
#include "reader.hpp"

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
        auto list = ast->as_list();
        auto first = list->at(0);
        if (first->is_symbol()) {
            auto special = first->as_symbol();
            if (special->matches("def!")) {
                auto key = list->at(1)->as_symbol();
                auto val = EVAL(list->at(2), env);
                env.set(key, val);
                return val;
            } else if (special->matches("let*")) {
                auto new_env = new Env { &env };
                auto bindings = list->at(1)->as_list();
                for (size_t i = 0; i < bindings->size(); i += 2) {
                    auto key = bindings->at(i)->as_symbol();
                    assert(i < bindings->size());
                    auto val = EVAL(bindings->at(i + 1), *new_env);
                    new_env->set(key, val);
                }
                return EVAL(list->at(2), *new_env);
            } else if (special->matches("do")) {
                Value *result = nullptr;
                assert(list->size() > 1);
                for (size_t i = 1; i < list->size(); ++i) {
                    result = EVAL(list->at(i), env);
                }
                return result;
            } else if (special->matches("if")) {
                auto condition = list->at(1);
                auto true_expr = list->at(2);
                auto false_expr = list->size() >= 4 ? list->at(3) : NilValue::the();
                if (EVAL(condition, env)->is_truthy())
                    return EVAL(true_expr, env);
                else
                    return EVAL(false_expr, env);
            } else if (special->matches("fn*")) {
                auto env_ptr = &env;
                auto binds = list->at(1)->as_list();
                auto body = list->at(2);
                auto closure = [env_ptr, binds, body](size_t argc, Value **args) {
                    auto exprs = new ListValue {};
                    for (size_t i = 0; i < argc; ++i)
                        exprs->push(args[i]);
                    auto fn_env = new Env { env_ptr, binds, exprs };
                    return EVAL(body, *fn_env);
                };
                return new FnValue { closure };
            }
        }
        auto eval_list = eval_ast(ast, env)->as_list();
        auto fn = eval_list->at(0)->as_fn()->to_fn();
        Value *args[eval_list->size() - 1];
        for (size_t i = 1; i < eval_list->size(); ++i) {
            args[i - 1] = eval_list->at(i);
        }
        return fn(eval_list->size() - 1, args);
    }
}

Value *eval_ast(Value *ast, Env &env) {
    switch (ast->type()) {
    case Value::Type::Symbol: {
        return env.get(ast->as_symbol());
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

int main() {
    const auto history_path = "history.txt";
    linenoise::LoadHistory(history_path);

    auto env = new Env { nullptr }; // top-level Env
    auto ns = build_namespace();
    for (auto pair : ns)
        env->set(new SymbolValue(pair.first), new FnValue { pair.second });

    std::string input;
    for (;;) {
        auto quit = linenoise::Readline("user> ", input);
        if (quit)
            break;
        std::cout << rep(input, *env) << std::endl;
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);

    return 0;
}

#include <iostream>
#include <string>

#include "linenoise.hpp"
#include "printer.hpp"
#include "reader.hpp"

Value *READ(std::string input) {
    return read_str(input);
}

Value *EVAL(Value *input) { return input; }

std::string PRINT(Value *input) { return pr_str(input, true); }

std::string rep(std::string input) {
    try {
        auto ast = READ(input);
        auto result = EVAL(ast);
        return PRINT(result);
    } catch (ExceptionValue *exception) {
        std::cerr << exception->message() << std::endl;
        return "";
    }
}

int main() {
    const auto history_path = "history.txt";
    linenoise::LoadHistory(history_path);

    std::string input;
    for (;;) {
        auto quit = linenoise::Readline("user> ", input);
        if (quit)
            break;
        std::cout << rep(input) << std::endl;
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);

    return 0;
}

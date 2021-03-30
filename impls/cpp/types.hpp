#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>

class Value {
public:
    virtual std::string inspect() { assert(0); }
};

class ListValue : public Value {
public:
    ListValue() { }

    void push(Value *value) {
        m_list.push_back(value);
    }

    virtual std::string inspect();

private:
    std::vector<Value *> m_list {};
};

class SymbolValue : public Value {
public:
    SymbolValue(std::string_view str)
        : m_str { str } { }

    std::string str() { return m_str; }

    virtual std::string inspect() {
        return str();
    }

private:
    std::string m_str;
};

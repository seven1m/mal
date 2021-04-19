#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <unordered_map>
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

protected:
    std::vector<Value *> m_list {};
};

class VectorValue : public ListValue {
public:
    VectorValue() { }

    virtual std::string inspect();
};

struct HashMapHash {
    std::size_t operator()(Value *key) const noexcept {
        return std::hash<std::string> {}(key->inspect());
    }
};

struct HashMapPred {
    constexpr bool operator()(Value *lhs, Value *rhs) const {
        return lhs == rhs; // FIXME
    }
};

class HashMapValue : public Value {
public:
    HashMapValue() { }

    virtual std::string inspect();

    void set(Value *key, Value *val) {
        m_map[key] = val;
    }

    Value *get(Value *key) {
        auto search = m_map.find(key);
        if (search != m_map.end())
            return search->second;
        return nullptr;
    }

private:
    std::unordered_map<Value *, Value *, HashMapHash, HashMapPred> m_map;
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

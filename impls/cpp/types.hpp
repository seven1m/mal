#pragma once

#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class ExceptionValue;
class FalseValue;
class FnValue;
class HashMapValue;
class IntegerValue;
class KeywordValue;
class ListValue;
class NilValue;
class StringValue;
class SymbolValue;
class TrueValue;
class VectorValue;

class Value {
public:
    enum class Type {
        Exception,
        False,
        Fn,
        HashMap,
        Integer,
        Keyword,
        List,
        Nil,
        String,
        Symbol,
        True,
        Vector,
    };

    virtual Type type() const = 0;
    virtual std::string inspect(bool print_readably = false) const = 0;

    virtual bool is_exception() const { return false; }
    virtual bool is_false() const { return false; }
    virtual bool is_integer() const { return false; }
    virtual bool is_keyword() const { return false; }
    virtual bool is_list() const { return false; }
    virtual bool is_listy() const { return false; }
    virtual bool is_nil() const { return false; }
    virtual bool is_symbol() const { return false; }
    virtual bool is_string() const { return false; }
    virtual bool is_true() const { return false; }
    virtual bool is_truthy() const { return true; }

    virtual bool operator==(const Value *other) const { return this == other; }
    bool operator!=(const Value *other) const { return !(*this == other); }

    ExceptionValue *as_exception();
    FalseValue *as_false();
    FnValue *as_fn();
    HashMapValue *as_hash_map();
    IntegerValue *as_integer();
    KeywordValue *as_keyword();
    ListValue *as_list();
    NilValue *as_nil();
    StringValue *as_string();
    SymbolValue *as_symbol();
    TrueValue *as_true();
    VectorValue *as_vector();

    const ExceptionValue *as_exception() const;
    const FalseValue *as_false() const;
    const FnValue *as_fn() const;
    const HashMapValue *as_hash_map() const;
    const IntegerValue *as_integer() const;
    const KeywordValue *as_keyword() const;
    const ListValue *as_list() const;
    const NilValue *as_nil() const;
    const StringValue *as_string() const;
    const SymbolValue *as_symbol() const;
    const TrueValue *as_true() const;
    const VectorValue *as_vector() const;
};

class ListValue : public Value {
public:
    ListValue() { }

    void push(Value *value) {
        m_list.push_back(value);
    }

    virtual Type type() const override { return Type::List; }
    virtual std::string inspect(bool print_readably = false) const override;
    virtual bool is_list() const override { return true; }
    virtual bool is_listy() const override { return true; }

    virtual bool operator==(const Value *) const override;

    auto begin() { return m_list.begin(); }
    auto end() { return m_list.end(); }

    bool is_empty() const { return m_list.size() == 0; }
    size_t size() const { return m_list.size(); }
    Value *at(size_t index) const { return m_list.at(index); }

protected:
    std::vector<Value *> m_list {};
};

class VectorValue : public ListValue {
public:
    VectorValue() { }

    virtual Type type() const override { return Type::Vector; }
    virtual std::string inspect(bool print_readably = false) const override;
    virtual bool is_list() const override { return false; }
};

struct HashMapHash {
    std::size_t operator()(const Value *key) const noexcept {
        return std::hash<std::string> {}(key->inspect());
    }
};

struct HashMapPred {
    bool operator()(const Value *lhs, const Value *rhs) const {
        return lhs->inspect() == rhs->inspect(); // FIXME
    }
};

class HashMapValue : public Value {
public:
    HashMapValue() { }

    virtual Type type() const override { return Type::HashMap; }
    virtual std::string inspect(bool print_readably = false) const override;

    virtual bool operator==(const Value *) const override;

    void set(Value *key, Value *val) {
        m_map[key] = val;
    }

    Value *get(Value *key) {
        auto search = m_map.find(key);
        if (search != m_map.end())
            return search->second;
        return nullptr;
    }

    auto begin() const { return m_map.begin(); }
    auto end() const { return m_map.end(); }

    size_t size() const { return m_map.size(); }

private:
    std::unordered_map<Value *, Value *, HashMapHash, HashMapPred> m_map;
};

class SymbolValue : public Value {
public:
    SymbolValue(std::string_view str)
        : m_str { str } { }

    std::string str() const { return m_str; }

    bool matches(const char *str) const { return m_str == str; }

    virtual Type type() const override { return Type::Symbol; }

    virtual std::string inspect(bool) const override {
        return str();
    }

    bool operator==(const Value *other) const override {
        return other->is_symbol() && const_cast<Value *>(other)->as_symbol()->m_str == m_str;
    }

    virtual bool is_symbol() const override { return true; }

private:
    std::string m_str;
};

class IntegerValue : public Value {
public:
    IntegerValue(long l)
        : m_long { l } { }

    virtual Type type() const override { return Type::Integer; }

    virtual std::string inspect(bool) const override {
        return std::to_string(m_long);
    }

    virtual bool is_integer() const override { return true; }

    bool operator==(const Value *other) const override {
        return other->is_integer() && const_cast<Value *>(other)->as_integer()->m_long == m_long;
    }

    long to_long() { return m_long; }

private:
    long m_long { 0 };
};

using Function = std::function<Value *(size_t, Value **)>;

class FnValue : public Value {
public:
    FnValue(Function fn)
        : m_fn { fn } { }

    virtual Type type() const override { return Type::Fn; }

    virtual std::string inspect(bool) const override {
        return "#<function>";
    }

    Function to_fn() { return m_fn; }

    bool operator==(const Value *other) const override {
        return other == this;
    }

private:
    Function m_fn { nullptr };
};

class ExceptionValue : public Value {
public:
    ExceptionValue(std::string message)
        : m_message { message } { }

    virtual Type type() const override { return Type::Exception; }

    virtual std::string inspect(bool) const override {
        return "<exception" + m_message + ">";
    }

    virtual bool is_exception() const override { return true; }

    bool operator==(const Value *other) const override {
        return other->is_exception() && const_cast<Value *>(other)->as_exception()->m_message == m_message;
    }

    const std::string &message() { return m_message; }

private:
    std::string m_message;
};

class TrueValue : public Value {
public:
    static TrueValue *the() {
        if (!s_instance)
            s_instance = new TrueValue;
        return s_instance;
    }

    virtual Type type() const override { return Type::True; }
    virtual std::string inspect(bool) const override { return "true"; }
    virtual bool is_true() const override { return true; }

private:
    TrueValue() { }

    static inline TrueValue *s_instance { nullptr };
};

class FalseValue : public Value {
public:
    static FalseValue *the() {
        if (!s_instance)
            s_instance = new FalseValue;
        return s_instance;
    }

    virtual Type type() const override { return Type::False; }
    virtual std::string inspect(bool) const override { return "false"; }
    virtual bool is_false() const override { return true; }
    virtual bool is_truthy() const override { return false; }

private:
    FalseValue() { }

    static inline FalseValue *s_instance { nullptr };
};

class NilValue : public Value {
public:
    static NilValue *the() {
        if (!s_instance)
            s_instance = new NilValue;
        return s_instance;
    }

    virtual Type type() const override { return Type::Nil; }
    virtual std::string inspect(bool) const override { return "nil"; }
    virtual bool is_nil() const override { return true; }
    virtual bool is_truthy() const override { return false; }

private:
    NilValue() { }

    static inline NilValue *s_instance { nullptr };
};

class StringValue : public Value {
public:
    StringValue(std::string_view str)
        : m_str { str } { }

    std::string str() const { return m_str; }

    virtual Type type() const override { return Type::String; }
    virtual bool is_string() const override { return true; }

    bool operator==(const Value *other) const override {
        return other->is_string() && other->as_string()->m_str == m_str;
    }

    virtual std::string inspect(bool print_readably = false) const override;

private:
    std::string m_str;
};

class KeywordValue : public Value {
public:
    KeywordValue(std::string_view str)
        : m_str { str } { }

    std::string str() const { return m_str; }

    virtual Type type() const override { return Type::Keyword; }
    virtual bool is_keyword() const override { return true; }

    bool operator==(const Value *other) const override {
        return other->is_keyword() && other->as_keyword()->m_str == m_str;
    }

    virtual std::string inspect(bool) const override {
        return m_str;
    }

private:
    std::string m_str;
};

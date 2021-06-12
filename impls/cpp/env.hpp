#pragma once

#include <unordered_map>

#include "types.hpp"

class Env {
public:
    Env(Env *outer)
        : m_outer(outer) { }

    Env(Env *outer, ListValue *binds, ListValue *exprs)
        : m_outer(outer) {
        set_binds(binds, exprs);
    }

    void set(SymbolValue *key, Value *val) {
        m_data[key] = val;
    }

    Env *find(const SymbolValue *key) const {
        auto search = m_data.find(key);
        if (search != m_data.end())
            return const_cast<Env *>(this);
        else if (m_outer)
            return m_outer->find(key);
        return nullptr;
    }

    Value *get(const SymbolValue *key) const {
        auto env = find(key);
        if (!env)
            throw new ExceptionValue { key->str() + " not found" };
        return env->m_data[key];
    }

private:
    void set_binds(ListValue *binds, ListValue *exprs) {
        for (size_t i = 0; i < binds->size(); ++i) {
            auto key = binds->at(i)->as_symbol();
            if (key->matches("&")) {
                if (i + 1 >= binds->size())
                    throw new ExceptionValue { "missing symbol after &" };
                key = binds->at(i + 1)->as_symbol();
                set_binds_rest(key, exprs, i);
                return;
            }
            if (i >= exprs->size())
                throw new ExceptionValue { "not enough arguments" };
            auto val = exprs->at(i);
            set(key, val);
        }
    }

    void set_binds_rest(SymbolValue *key, ListValue *exprs, size_t starting_index) {
        auto vals = new ListValue;
        for (size_t i = starting_index; i < exprs->size(); ++i) {
            vals->push(exprs->at(i));
        }
        set(key, vals);
    }

    Env *m_outer { nullptr };
    std::unordered_map<const SymbolValue *, Value *, HashMapHash, HashMapPred> m_data;
};

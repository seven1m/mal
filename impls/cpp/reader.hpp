#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "types.hpp"

class Tokenizer {
public:
    Tokenizer(std::string &input)
        : m_input { input } { }

    std::optional<std::string_view> next() {
        auto view = std::string_view(m_input);

        while (m_index < m_input.length()) {
            char c = m_input.at(m_index);

            switch (c) {
            case ' ':
            case '\t':
            case '\n':
            case ',':
                ++m_index;
                break;
            case '~': {
                ++m_index;
                if (m_index < m_input.length() && m_input.at(m_index) == '@') {
                    ++m_index;
                    return view.substr(m_index - 2, 2);
                }
                return view.substr(m_index - 1, 1);
            }
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case '\'':
            case '`':
            case '^':
            case '@':
                return view.substr(m_index++, 1);
            case '"': {
                size_t start = m_index;
                ++m_index;
                while (m_index < m_input.length()) {
                    c = m_input.at(m_index);
                    switch (c) {
                    case '"':
                        ++m_index;
                        return view.substr(start, m_index - start);
                    case '\\':
                        ++m_index;
                        break;
                    }
                    ++m_index;
                }
                std::cout << "EOF\n";
                return view.substr(start, m_index - start);
            }
            case ';': {
                size_t start = m_index;
                while (m_index < m_input.length()) {
                    c = m_input.at(m_index);
                    if (c == '\n')
                        break;
                    ++m_index;
                }
                return view.substr(start, m_index - start);
            }
            case '-':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                size_t start = m_index;
                bool done = false;
                ++m_index;
                while (!done && m_index < m_input.length()) {
                    c = m_input.at(m_index);
                    switch (c) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        ++m_index;
                        break;
                    default:
                        done = true;
                        break;
                    }
                }
                if (m_index - start == 1 && view.at(start) == '-') {
                    --m_index;
                    return tokenize_symbol();
                }
                return view.substr(start, m_index - start);
            }
            default: {
                return tokenize_symbol();
            }
            }
        }
        return {};
    }

private:
    std::string_view tokenize_symbol() {
        auto view = std::string_view(m_input);
        size_t start = m_index;
        bool done = false;
        char c;
        while (!done && m_index < m_input.length()) {
            c = m_input.at(m_index);
            switch (c) {
            case ' ':
            case '\t':
            case '\n':
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case '\'':
            case '"':
            case '`':
            case ',':
            case ';':
                done = true;
                break;
            default:
                ++m_index;
            }
        }
        return view.substr(start, m_index - start);
    }

    std::string &m_input;
    size_t m_index { 0 };
};

class Reader {
public:
    Reader(std::vector<std::string_view> &tokens)
        : m_tokens { tokens } { }

    std::optional<std::string_view> next() {
        if (m_index < m_tokens.size())
            return m_tokens.at(m_index++);
        return {};
    }

    std::optional<std::string_view> peek() {
        if (m_index < m_tokens.size())
            return m_tokens.at(m_index);
        return {};
    }

private:
    std::vector<std::string_view> &m_tokens;
    size_t m_index { 0 };
};

std::vector<std::string_view> tokenize(std::string &input);

Value *read_str(std::string &input);

Value *read_form(Reader &reader);

Value *read_integer(Reader &reader);

Value *read_quoted_value(Reader &reader);

Value *read_with_meta(Reader &reader);

ListValue *read_list(Reader &reader);

ListValue *read_vector(Reader &reader);

HashMapValue *read_hash_map(Reader &reader);

Value *read_atom(Reader &reader);

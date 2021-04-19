#include "reader.hpp"

std::vector<std::string_view> tokenize(std::string &input) {
    Tokenizer tokenizer { input };
    std::vector<std::string_view> vector;
    while (auto token = tokenizer.next()) {
        vector.push_back(*token);
    }
    return vector;
}

Value *read_str(std::string &input) {
    auto tokens = tokenize(input);
    Reader reader { tokens };
    return read_form(reader);
}

Value *read_form(Reader &reader) {
    auto token = reader.peek();

    if (!token)
        return nullptr;

    switch (token.value()[0]) {
    case '(':
        return read_list(reader);
    case '[':
        return read_vector(reader);
    case '{':
        return read_hash_map(reader);
    case '\'':
    case '`':
    case '~':
    case '@':
        return read_quoted_value(reader);
    case '^':
        return read_with_meta(reader);
    default:
        return read_atom(reader);
    }
}

Value *read_quoted_value(Reader &reader) {
    auto token = reader.peek();
    switch (token.value()[0]) {
    case '\'': {
        reader.next();
        auto list = new ListValue {};
        list->push(new SymbolValue { "quote" });
        list->push(read_form(reader));
        return list;
    }
    case '`': {
        reader.next();
        auto list = new ListValue {};
        list->push(new SymbolValue { "quasiquote" });
        list->push(read_form(reader));
        return list;
    }
    case '~': {
        if (token.value().length() > 1 && token.value()[1] == '@') {
            reader.next();
            auto list = new ListValue {};
            list->push(new SymbolValue { "splice-unquote" });
            list->push(read_form(reader));
            return list;
        } else {
            reader.next();
            auto list = new ListValue {};
            list->push(new SymbolValue { "unquote" });
            list->push(read_form(reader));
            return list;
        }
    }
    case '@': {
        reader.next();
        auto list = new ListValue {};
        list->push(new SymbolValue { "deref" });
        list->push(read_form(reader));
        return list;
    }
    default:
        std::cerr << "bad quote!\n";
        abort();
    }
}

Value *read_with_meta(Reader &reader) {
    reader.next(); // consume '^'

    auto *list = new ListValue {};
    auto meta = read_form(reader);
    auto value = read_form(reader);
    list->push(new SymbolValue { "with-meta" });
    list->push(value);
    list->push(meta);
    return list;
}

ListValue *read_list(Reader &reader) {
    reader.next(); // consume '('
    auto *list = new ListValue {};

    while (auto token = reader.peek()) {
        if (*token == ")") {
            reader.next();
            return list;
        }

        list->push(read_form(reader));
    }

    std::cerr << "EOF\n";
    return list;
}

ListValue *read_vector(Reader &reader) {
    reader.next(); // consume '['
    auto *vec = new VectorValue {};

    while (auto token = reader.peek()) {
        if (*token == "]") {
            reader.next();
            return vec;
        }

        vec->push(read_form(reader));
    }

    std::cerr << "EOF\n";
    return vec;
}

HashMapValue *read_hash_map(Reader &reader) {
    reader.next(); // consume '{'
    auto *map = new HashMapValue {};

    while (auto token = reader.peek()) {
        if (*token == "}") {
            reader.next();
            return map;
        }

        auto key = read_form(reader);

        token = reader.peek();
        if (*token == "}") {
            std::cerr << "hash-map key without value!\n";
            reader.next();
            return map;
        }

        auto val = read_form(reader);
        map->set(key, val);
    }

    std::cerr << "EOF\n";
    return map;
}

Value *read_atom(Reader &reader) {
    return new SymbolValue { *reader.next() };
}

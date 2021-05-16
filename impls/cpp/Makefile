CXXFLAGS ?= -std=c++17 -g -Wall -Wextra -Werror

build: step0_repl step1_read_print step2_eval

clean:
	rm -f step0_repl step1_read_print step2_eval

step0_repl: step0_repl.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

step1_read_print: step1_read_print.cpp reader.cpp printer.cpp types.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

step2_eval: step2_eval.cpp reader.cpp printer.cpp types.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

inc_dir := $(Dyninst_DIR)/include
lib_dir := $(Dyninst_DIR)/lib

.PHONY: all
all:
	g++ -c -Wall -Wextra -O3 -I$(inc_dir) -o parse_function_params.o parse_function_params.cpp
	g++ -L$(lib_dir) -o parse_function_params parse_function_params.o -lsymtabAPI -ldynDwarf -ldynElf -lcommon -lboost_system -ltbb

.PHONY: clean
clean:
	rm -f *.o parse_function_params

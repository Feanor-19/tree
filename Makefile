CC=g++

SAN = -fsanitize=alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr,leak,address
sanitize_banned = leak,address

ifndef RELEASE
CFLAGS  =	-D _DEBUG -ggdb3 -std=c++17 -Wall -Wextra -Weffc++ 									\
			-Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations 				\
			-Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported 				\
			-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal 						\
			-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 				\
			-Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual 		\
			-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow 					\
			-Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 			\
			-Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types 			\
			-Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef 				\
			-Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix 	\
			-Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs 	\
			-Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector 				\
			-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer					\
			-Wstack-usage=8192 -pie -fPIE -Werror=vla $(SAN)
else		
CFLAGS = -D NDEBUG 
endif

OBJ = obj
SRC = src
BIN = bin

LIB_OUT = lib_out

SOURCES 	= $(wildcard $(SRC)/*.cpp)
OBJFILES 	= $(patsubst $(SRC)/%,$(OBJ)/%,$(SOURCES:.cpp=.o))
OUT 		= $(BIN)/prog
DUMP_FOLDER = ./dumps

$(OUT) : $(OBJFILES)
	$(CC) -o $@ $(CFLAGS) $^

$(OBJ)/%.o : $(SRC)/%.cpp
	@$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: run
run:
	$(OUT)

MAIN_OBJ = $(OBJ)/main.o
OBJS_FOR_LIB = $(filter-out $(MAIN_OBJ),$(OBJFILES)) 

.PHONY: make_lib
make_lib:
	ar -cvq $(LIB_OUT)/libtree.a $(OBJS_FOR_LIB)

.PHONY: copy_lib
copy_lib:
	cp $(LIB_OUT)/* /usr/lib/feanor/tree
	cp $(SRC)/*.h /usr/include/feanor/tree

.PHONY: clean
clean:
	rm -f $(OBJFILES) $(OUT)

.PHONY: clean_dumps
clean_dumps:
	rm -r -f $(DUMP_FOLDER)

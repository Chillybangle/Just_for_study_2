
#CXXFLAGS := 
CXXFLAGS:=-D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer  -Wstack-usage=8192 -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

LIB_DIR:=../Libs
LIBS:=-lstackfloatdeb4

ASM_DIR := ./ASM
CPU_DIR := ./CPU
HEADERS_DIR := ./Headers
BUILD_DIR := ./Build

ASM_SOURCES := $(shell find $(ASM_DIR) -name '*.cpp')
CPU_SOURCES := $(shell find $(CPU_DIR) -name '*.cpp')

ASMOBJS := $(ASM_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
CPUOBJS := $(CPU_SOURCES:%.cpp=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/CPU.exe $(BUILD_DIR)/ASM.exe

$(BUILD_DIR)/CPU.exe: $(CPUOBJS)
	g++ $(CPUOBJS) -o $(BUILD_DIR)/CPU.exe $(CXXFLAGS) -L$(LIB_DIR) $(LIBS) -I./../Libs/ -I./Headers
	
$(BUILD_DIR)/ASM.exe: $(ASMOBJS)
	g++ $(ASMOBJS) -o $(BUILD_DIR)/ASM.exe $(CXXFLAGS) -I./Headers

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	g++ -c $< -o $@ $(CXXFLAGS) -L$(LIB_DIR) $(LIBS) -I ./../Libs/ -I./Headers
	
	

CXX                             = g++
CC								= gcc
STANDARD_FLAGS                  = -std=c++0x
WARNING_FLAGS                   = -Wall -Wno-unused-variable -Wno-unknown-pragmas
OPT_FLAGS                       = -O2 -m64
CXXFLAGS                        = $(OPT_FLAGS) $(WARNING_FLAGS) $(STANDARD_FLAGS)
CFLAGS							= $(OPT_FLAGS) $(WARNING_FLAGS) -c

ARFLAGS                         = --target=elf64-x86-64

SRCS = $(shell find . -name '*.cpp') 
INCLUDES = -I../3rdparty/boost_1_63_0/ -I/opt/ibm/platformsymphonyde/de71/7.1/include/

LINPACK_SRCS = $(shell find . -name '*.c')

LIBS = 
OBJS = $(SRCS:.cpp=.o)
LINPACK_OBJS = $(LINPACK_SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

OUT = libaswscommon.a

all: $(OUT)
	@echo $(OUT) has been compiled!

-include $(DEPS)

$(OUT): $(OBJS) $(LINPACK_OBJS)
	ar -r $(OUT) $(OBJS) $(LINPACK_OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.d: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(OBJS) $(LINPACK_OBJS) $(OUT)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS)

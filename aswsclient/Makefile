CXX                             = g++
STANDARD_FLAGS                  = -std=c++0x
WARNING_FLAGS                   = -Wall -Wno-unused-variable -Wno-unknown-pragmas
OPT_FLAGS                       = -O2 -m64 -D NO_WIN32
CXXFLAGS                        = $(OPT_FLAGS) $(WARNING_FLAGS) $(STANDARD_FLAGS)
LDFLAGS							= -L../3rdparty/pugixml-1.4 \
                                  -L../3rdparty/boost_1_63_0/stage/lib \
                                  -L/opt/ibm/spectrumcomputing/symphonyde/de721/7.2.1/linux-x86_64/lib64 \
                                  -L../aswscommon 
				  
ARFLAGS                         = --target=elf64-x86-64

SRCS = $(shell find . -name '*.cpp') 
INCLUDES = -I../3rdparty/boost_1_63_0/ -I/opt/ibm/spectrumcomputing/symphonyde/de721/7.2.1/include/ -I/opt/ibm/spectrumcomputing/symphonyde/de721/7.2.1/src/

LIBS = -lboost_filesystem -lboost_serialization -lboost_program_options -lboost_random -lboost_thread -lboost_system \
	-lpugixml -pthread -lsoambase -laswscommon
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

OUT = awsclient

all: $(OUT)
	@echo $(OUT) has been compiled!

-include $(DEPS)

$(OUT): $(OBJS) libpugixml libaswscommon
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

%.d: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -MM -MT $(@:.d=.o) >$@

.PHONY: libpugixml
libpugixml:
	$(MAKE) -C ../3rdparty/pugixml-1.4

.PHONY: libaswscommon
libaswscommon:
	$(MAKE) -C ../aswscommon

.PHONY: clean
clean:
	rm -f $(OBJS) $(OUT)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS)

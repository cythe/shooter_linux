inc_files := $(wildcard *.h)
c_srcs  := $(wildcard *.c)
cx_srcs := $(wildcard *.cpp)

c_deps 	:= $(patsubst %c, %d, $(c_srcs))
cx_deps := $(patsubst %cpp, %dx, $(cx_srcs))

c_objs  := $(patsubst %c, %o, $(c_srcs))
cx_objs := $(patsubst %cpp, %ox, $(cx_srcs))

-include $(c_deps) $(cx_deps)

%.o: %.c %.d
ifeq ($(TARGET_IS_LIB), 1)
	$(CC) -fPIC $(INC_FLAGS) $(CFLAGS) -c $< -o $@
else
	$(CC) $(INC_FLAGS) $(CFLAGS) -c $< -o $@
endif

%.ox: %.cpp %.dx
ifeq ($(TARGET_IS_LIB), 1)
	$(CX) -fPIC $(INC_FLAGS) $(CXFLAGS) -c $< -o $@
else
	$(CX) $(INC_FLAGS) $(CXFLAGS) -c $< -o $@
endif

%.d: %.c
	$(CC) -M $(INC_FLAGS) $(CFLAGS) $< > $@; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@ > $@.tmp && \
	mv -f $@.tmp $@;

%.dx: %.cpp
	$(CX) -M $(INC_FLAGS) $(CXFLAGS) $< > $@; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@ > $@.tmp && \
	mv -f $@.tmp $@;

all: $(TARGET)

$(TARGET): $(c_objs) $(cx_objs)
ifeq ($(TARGET_IS_LIB), 1)
	$(CX) -shared $(LD_FLAGS) $< -o $@ $(SYS_LIBS) $(EXTRA_LIBDIR) $(EXTRA_LIBS)
else
	$(CX) $(LD_FLAGS) $(c_objs) $(cx_objs) -o $@ $(SYS_LIBS) $(EXTRA_LIBDIR) $(EXTRA_LIBS) -Wl,-rpath,../lib
endif

clean:
	rm -f $(c_deps) $(cx_deps) $(c_objs) $(cx_objs) $(TARGET)

install: $(TARGET)
ifeq ($(TARGET_IS_LIB), 1)
	install -m 775 $(TARGET) $(lib_dir)
	pushd $(lib_dir) && ln -sf $(TARGET) lib$(obj-so).so && popd
	install -m 664 $(inc_files) $(inc_dir)
else
	install -m 775 $(TARGET) $(bin_dir)
endif


CXX ?= g++

source_files := $(shell find src -name "*.cpp")
object_files := $(patsubst src/%.cpp, build/%.o, $(source_files))

libraries := libgcrypt

compiler_flags := $(shell pkg-config --cflags ${libraries}) -I./include -Wall -pipe
linker_flags := $(shell pkg-config --libs ${libraries})

ifneq ($(BUILD), release)
compiler_flags += -O0 -g3 -DDEBUG
else
compiler_flags += -march=native -mtune=native
linker_flags += -flto
endif

binary := rtt

.PHONY: all
all: $(binary)

options:
	@echo "CXX: $(CXX)"
	@echo "compiler_flags: $(compiler_flags)"
	@echo "linker_flags: $(linker_flags)"

$(object_files): build/%.o : ./include/%.h
$(object_files): build/%.o : src/%.cpp include/config.h Makefile
	mkdir -p build
	$(CXX) -c $(patsubst build/%.o, src/%.cpp, $@) $(compiler_flags) -o $@


$(binary): ${object_files}
	$(CXX) -o $(binary) $(object_files) $(linker_flags)

.PHONY: clean
clean:
	rm -f $(object_files)
	rm -f $(binary)

uninstall:
	rm -f $(install_dir)/$(binary)

release:
	BUILD=release $(MAKE)

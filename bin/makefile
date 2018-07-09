# Flagship Engine makefile (windows only at the momemt)

_TARGET := $(addprefix ../,$(TARGET))
CFLAGS += -g -gcodeview

DEPS := ../libs/glfw/build/src/Debug/glfw3.lib
LIBS := -L../libs/glfw/build/src/Debug -lglfw3 -lopengl32 -lshell32 -luser32 -lgdi32 -Wl,-nodefaultlib:libcmt -lmsvcrtd

INCLUDE := -I../libs/glfw/include -I../libs/cglm/include

SRCS := $(wildcard ../src/*.c)
HDRS := $(wildcard ../src/*.h)
OBJS := $(notdir $(SRCS:.c=.o))

.PHONY: clean

$(_TARGET): $(DEPS) $(OBJS)
	@ echo $@
	@ $(CC) -o $@ $(OBJS) $(LIBS)

%.o: ../src/%.c $(HDRS)
	@ echo $(notdir $<)
	@ $(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	@ rm -rfv $(OBJS)

../libs/glfw/build/src/Debug/glfw3.lib:
	@ echo $(notdir $@)
	@ cd ../libs/glfw                                              &&\
		mkdir -p build                                             &&\
		cd build                                                   &&\
		cmake -G"Visual Studio 15 2017 Win64" -T"LLVM-vs2017" ..   &&\
		cmake --build .
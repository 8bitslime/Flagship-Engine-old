# Flagship Engine makefile (windows only at the momemt)

_TARGET := $(addprefix ../,$(TARGET))

DEPS := ../libs/glad/glad.lib ../libs/glfw/build/src/Debug/glfw3.lib
LIBS := -L../libs/glad -lglad -L../libs/glfw/build/src/Debug -lglfw3 -lopengl32 -lshell32 -luser32 -lgdi32 -Wl,-nodefaultlib:libcmt -lmsvcrtd

INCLUDE := -I../libs/glad/include -I../libs/glfw/include -I../libs/cglm/include -I../libs

SRCS := $(wildcard ../src/*.c)
HDRS := $(wildcard ../src/*.h)
OBJS := $(notdir $(SRCS:.c=.o))

.PHONY: clean

$(_TARGET): $(DEPS) $(OBJS)
	@ echo $(notdir $@)
	@ $(CC) -g -o $@ $(OBJS) $(LIBS)

%.o: ../src/%.c $(HDRS)
	@ echo $(notdir $<)
	@ $(CC) $(CFLAGS) -g -gcodeview $(INCLUDE) -c -o $@ $<

clean:
	@ rm -rfv $(OBJS)

../libs/glfw/build/src/Debug/glfw3.lib:
	@ echo $(notdir $@)
	@ cd ../libs/glfw                                              &&\
		mkdir -p build                                             &&\
		cd build                                                   &&\
		cmake -G"Visual Studio 15 2017 Win64" -T"LLVM-vs2017" ..   &&\
		cmake --build .

../libs/glad/glad.lib:
	@ echo $(notdir $@)
	@ cd ../libs/glad                              &&\
		$(CC) -I./include -c -o glad.o src/glad.c  &&\
		$(AR) rcs glad.lib glad.o
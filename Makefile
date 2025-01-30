##################################################
# Compiler and common flags
##################################################
CXX      = clang++
CXXFLAGS = -std=c++20 -g -Wall -Wextra \
           -DSOURCE_DIR=\"$(PWD)\" \
           -isystem external/boost/include \
           -Iexternal/fontawesome -Iexternal/glad/include \
           -Iexternal/GLFW/include -Iexternal/glm/include -Iexternal/imgui \
           -Iexternal/ImGuizmo -Iexternal/json -Iexternal/stb_image \
           -Isrc -Isrc/Engine/Core -Isrc/Engine/Lighting -Isrc/Engine/Physics \
           -Isrc/Engine/Rendering -Isrc/Engine/SceneManagement -Isrc/Engine/Utilities \
           -Isrc/Editor/Core -Isrc/Editor/Utilities

##################################################
# Detect OS
##################################################
ifeq ($(OS),Windows_NT)
    # =========== WINDOWS BUILD ===========
    UNAME_S := Windows

    CXXFLAGS += -fsanitize=address,undefined

    LDFLAGS = -Lexternal/GLFW/lib -lglfw3 -lpthread \
              -lkernel32 -lgdi32 -luser32 -lshell32 -lmingw32 \
              -fsanitize=address,undefined

    LLVM_SYMBOLIZER ?= C:/LLVM/bin/llvm-symbolizer.exe

else ifeq ($(shell uname -s),Linux)
    # =========== LINUX BUILD ===========
    UNAME_S := Linux

    CXXFLAGS += -fsanitize=address,undefined -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls

    LDFLAGS = -Lexternal/GLFW/lib -lglfw3 -lpthread -ldl -lX11 -lXrandr -lXi \
              -lXxf86vm -lXcursor -lGL -fsanitize=address

     LLVM_SYMBOLIZER_PATH := $(shell which llvm-symbolizer-18)

    # If llvm-symbolizer is not found, show error
    ifeq ($(LLVM_SYMBOLIZER_PATH),)
        $(error "llvm-symbolizer not found, please install LLVM with symbolizer support")
    endif

    export ASAN_SYMBOLIZER_PATH=$(LLVM_SYMBOLIZER_PATH)

else
    $(error "Unsupported OS: only Windows_NT and Linux are compatible.")
endif

##################################################
# Build directories and output
##################################################
BUILD_DIR = idkbuild
SRC_DIR   = src
BIN       = $(BUILD_DIR)/idk_engine

##################################################
# Sources
##################################################
SRC_CPP_ONLY = main.cpp \
               $(shell find $(SRC_DIR) -name '*.cpp' -not -path '*/idkbuild/*') \
               external/imgui/imgui.cpp \
               external/imgui/imgui_draw.cpp \
               external/imgui/imgui_tables.cpp \
               external/imgui/imgui_widgets.cpp \
               external/imgui/backends/imgui_impl_glfw.cpp \
               external/imgui/backends/imgui_impl_opengl3.cpp \
               external/ImGuizmo/GraphEditor.cpp \
               external/ImGuizmo/ImCurveEdit.cpp \
               external/ImGuizmo/ImGradient.cpp \
               external/ImGuizmo/ImGuizmo.cpp \
               external/ImGuizmo/ImSequencer.cpp

SRC_C_ONLY   = external/glad/src/glad.c

OBJ_CPP = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC_CPP_ONLY))
OBJ_C   = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_C_ONLY))
OBJ     = $(OBJ_CPP) $(OBJ_C)

##################################################
# Targets
##################################################
all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

run:
	ASAN_OPTIONS=log_path=asan_log:verbosity=1 \
	ASAN_SYMBOLIZER_PATH=$(LLVM_SYMBOLIZER_PATH) \
	./$(BIN)


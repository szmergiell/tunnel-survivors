TARGET_EXEC ?= tunnel-survivors

CC = clang
wasm: CC = emcc

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -g -Wall -Wextra -Wpedantic
wasm: CPPFLAGS := -s WASM=1
release: CPPFLAGS := $(INC_FLAGS) -O3 -DNDEBUG
LDFLAGS ?= -lSDL2 -lSDL2_image -lSDL2_ttf -lm
wasm: LDFLAGS := -lm --bind -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png","jpg"]' -s USE_SDL_TTF=2 --preload-file assets/ --use-preload-plugins -s ALLOW_MEMORY_GROWTH

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

release: $(BUILD_DIR)/$(TARGET_EXEC)

wasm: $(BUILD_DIR)/index.html

$(BUILD_DIR)/index.html:
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(LDFLAGS) $(SRCS) -o $@

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

.PHONY: bear

bear:
	make clean; bear -- make

-include $(DEPS)

MKDIR_P ?= mkdir -p


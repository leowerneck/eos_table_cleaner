# Equation of State Cleaner Project Makefile
# (c) 2025 Leo Werneck
# wernecklr@gmail.com

# Configurable project parameters
PROJECT   = eos_cleaner
BUILD_DIR = build
SRC_DIR   = src
INC_DIRS  = src
MODULES   =
HDF5_INC  = $(shell pkg-config --cflags hdf5)
HDF5_LIB  = $(shell pkg-config --libs hdf5)

# Compilation flags
CFLAGS   ?= -std=c99 -g2 -march=native -Wall -Wextra -pedantic -Werror
INCLUDES  = $(addprefix -I,$(INC_DIRS)) $(HDF5_INC)
LDFLAGS  += $(HDF5_LIB) -lm

# Set compiler (override default c++ to avoid macOS using Apple Clang)
CC ?= gcc
ifeq ($(CC),cc)
  CC = gcc
endif

# Build directories
MODULES_BASEDIR = $(BUILD_DIR)/$(PROJECT)
BUILD_DIRS = $(MODULES_BASEDIR) $(addprefix $(MODULES_BASEDIR)/,$(MODULES))

# Gather source files and generate object/dependency lists
SRC_DIRS := $(SRC_DIR) $(SRC_DIR)/$(PROJECT) $(addprefix $(SRC_DIR)/$(PROJECT)/,$(MODULES))
SRC      := $(wildcard $(addsuffix /*.c,$(SRC_DIRS)))
OBJ      := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP      := $(OBJ:.o=.d)

.PHONY: all debug release clean

# Default target (calls 'debug' build)
all: release

# Build types: Debug and Release
debug:   CFLAGS += -O0
release: CFLAGS += -O2 -DNDEBUG -fopenmp
debug release: $(PROJECT)

$(PROJECT): $(OBJ)
	@echo "Linking $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c | $(BUILD_DIRS)
	@$(CC) $(INCLUDES) -MM -MT "$@ $(@:.d=.o)" -MF "$@" $<

ifeq (,$(filter clean,$(MAKECMDGOALS)))
-include $(DEP)
endif

$(BUILD_DIRS):
	@mkdir -p $@

# Cleanup build files
clean:
	@echo "Cleaning up build files"
	@$(RM) -r $(BUILD_DIR) $(PROJECT)

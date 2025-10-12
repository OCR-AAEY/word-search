##############################
#           NOTES            #
##############################

# >>> TEST_FLAGS
# Adds C flags when the project is compiled for testing with criterion. It defines the macro UNIT_TEST and imports all files from $(MAIN_DIR). Note that it is declared with = and not := because it requires $(MAIN_DIR) which is not yet defined when TEST_FLAGS is declared.
#
# >>> XCFLAGS
# Is by default empty but can be overriden when using make to add C flags without overriding the CFLAGS macro. It is for example used in the build GitHub workflow to add -Werror during the compilation.
#
# >>> $(shell pkg-config --cflags --libs criterion)
# Is used to get the appropriate C flags (-I for instance) and lib flags (-l) to import the given library, here criterion.
#
# >>> $(shell find $(MAIN_DIR) -name '*.c')
# Uses the shell `find` command to recursively get all C files in the given directory, here $(MAIN_DIR).
#
# >>> $(SRC_MAIN:$(MAIN_DIR)/%.c=$(BUILD_DIR)/main/%.o)
# Returns the content of the first macro SRC_MAIN replacing $(MAIN_DIR)/ by $(BUILD_DIR)/main/ at the begining of the file, and .c by .o at the end of it.
#
# >>> $(filter $(BUILD_DIR)/main/solver/%.o,$(OBJ_MAIN))
# Returns all the files in $(OBJ_MAIN) matching the pattern $(BUILD_DIR)/main/solver/%.o, i.e. all the files in $(OBJ_MAIN) located in the solver/ subfolder.
#
# >>> @mkdir -p $(@D)
# Is used to create the target (file before the colon) directory if it does not already exist.

##############################
#           MACROS           #
##############################

# Compiler and flags
CC          := gcc
CFLAGS      := -Wall -Wextra -std=c11
XCFLAGS     :=
TEST_FLAGS   = -DUNIT_TEST -I$(MAIN_DIR)
CRITERION_FLAGS := $(shell pkg-config --cflags --libs criterion)
GTK_FLAGS   := $(shell pkg-config --cflags --libs gtk+-3.0)

# Directories
SRC_DIR     := src
MAIN_DIR    := $(SRC_DIR)/main
TEST_DIR    := $(SRC_DIR)/test
BUILD_DIR   := build

# Source and object files
SRC_MAIN    := $(shell find $(MAIN_DIR) -name '*.c')
SRC_TEST    := $(shell find $(TEST_DIR) -name '*.c')
OBJ_MAIN    := $(SRC_MAIN:$(MAIN_DIR)/%.c=$(BUILD_DIR)/main/%.o)
OBJ_TEST    := $(SRC_TEST:$(TEST_DIR)/%.c=$(BUILD_DIR)/test/%.o)

# Executables
BIN_SOLVER       := $(BUILD_DIR)/solver
BIN_IMAGE_LOADER := $(BUILD_DIR)/image_loader
# BIN_APP          := $(BUILD_DIR)/app
BIN_TEST         := $(BUILD_DIR)/run_tests

##############################
#          TARGETS           #
##############################

# Solver target
$(BIN_SOLVER): $(filter $(BUILD_DIR)/main/solver/%.o,$(OBJ_MAIN))
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@

# Image loader target
$(BIN_IMAGE_LOADER): $(filter $(BUILD_DIR)/main/image_loader/%.o,$(OBJ_MAIN))
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(GTK_FLAGS)

## App target
#$(BIN_APP): $(filter %app/%.o,$(OBJ_MAIN))
#	@mkdir -p $(@D)
#	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(GTK_FLAGS)

# Test binary
$(BIN_TEST): $(OBJ_MAIN) $(OBJ_TEST)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $(TEST_FLAGS) $^ -o $@ $(CRITERION_FLAGS) $(GTK_FLAGS)

# Generic rule to build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) -c $< -o $@

##############################
#           PHONY            #
##############################

all: $(BIN_SOLVER) $(BIN_IMAGE_LOADER) #$(BIN_APP)

#run: $(BIN_APP)
#	@echo "Running app..."
#	@./$<

test: $(BIN_TEST)
	@echo "Running unit tests..."
	@./$< --verbose

clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)

.PHONY: all test clean

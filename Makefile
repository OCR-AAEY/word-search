##############################
#           NOTES            #
##############################

# >>> VERBOSE
# Determines whether make will print the execued gcc compilation commands. By default, it is set to 0 (no print). It can be set using `make VERBOSE=1 <command>`.
#
# >>> -I$(MAIN_DIR)
# Make src/main/ the default directory. Therefore, files have to be included from this path.
#
# >>> TEST_FLAGS
# Adds C flags when the project is compiled for testing with criterion. It defines the macro UNIT_TEST and adds the criterion library.
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

# Whether gcc commands should be displayed during compilation or not.
VERBOSE ?= 0

# Source directory.
SRC_DIR   = src
# Main directory.
MAIN_DIR  = $(SRC_DIR)/main
# Test directory.
TEST_DIR  = $(SRC_DIR)/test
# Build directory.
BUILD_DIR = build
# Main sub-directory in build directory.
BUILD_MAIN_DIR = build/main

# Compiler.
ifeq ($(VERBOSE),0)
CC = @gcc
else
CC = gcc
endif

# C flags.
CFLAGS         = -Wall -Wextra -fsanitize=address -I$(MAIN_DIR)
# Additional C flags.
XCFLAGS        =
# C flags for unit testing.
TEST_FLAGS     = -DUNIT_TEST -I$(TEST_DIR)
# C flags for unit test libraries import.
TEST_LIB_FLAGS = $(shell pkg-config --cflags --libs criterion)
# C flags for libraries import.
LIB_FLAGS      = -lm $(shell pkg-config --cflags --libs gtk+-3.0)

# Source files located in the main directory.
SRC_MAIN = $(shell find $(MAIN_DIR) -name '*.c' -and -not -name '*_main.c')
# Source files located in the test directory.
SRC_TEST = $(shell find $(TEST_DIR) -name '*.c' -and -not -name '*_main.c')

# All object files with respect to the source files of the main directory.
OBJ_MAIN          = $(SRC_MAIN:$(MAIN_DIR)/%.c=$(BUILD_DIR)/main/%.o)
# All object files with respect to the source files of the main directory compiled for unit tests.
OBJ_MAIN_FOR_TEST = $(SRC_MAIN:$(MAIN_DIR)/%.c=$(BUILD_DIR)/main_for_test/%.o)
# All object files with respect to the source files of the test directory compiled for unit tests.
OBJ_TEST_FOR_TEST = $(SRC_TEST:$(TEST_DIR)/%.c=$(BUILD_DIR)/test/%.o)

# Solver executable.
BIN_SOLVER = solver
# OCR neural network training executable.
BIN_OCR    = ocr_train
# Main application executable.
BIN_APP    = app
# Unit tests executable.
BIN_TEST   = run_tests

define import
$(foreach dir,$(1),$(filter $(BUILD_MAIN_DIR)/$(dir)/%.o,$(OBJ_MAIN)))
endef

##############################
#          TARGETS           #
##############################

# Solver target.
$(BIN_SOLVER): $(call import,solver) $(BUILD_MAIN_DIR)/solver/solver_main.o
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo "$(BIN_SOLVER): \033[32mCompilation succeeded\033[0m"

# OCR neural network training target.
$(BIN_OCR): $(call import,ocr matrix utils) $(BUILD_MAIN_DIR)/ocr/ocr_train_main.o
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo "$(BIN_OCR): \033[32mCompilation succeeded\033[0m"

# Main app target.
$(BIN_APP): $(OBJ_MAIN) $(BUILD_MAIN_DIR)/app/app_main.o
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo "$(BIN_APP): \033[32mCompilation succeeded\033[0m"

# Unit tests target.
$(BIN_TEST): $(OBJ_MAIN_FOR_TEST) $(OBJ_TEST_FOR_TEST)
	$(CC) $(CFLAGS) $(XCFLAGS) $(TEST_FLAGS) $^ -o $@ $(LIB_FLAGS) $(TEST_LIB_FLAGS)
	@echo "$(BIN_TEST): \033[32mCompilation succeeded\033[0m"

# ocr_training_data: $(OBJ_MAIN) $(BUILD_MAIN_DIR)/ocr/ocr_training_data.o
# 	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
# 	@echo "$(BIN_OCR): \033[32mCompilation succeeded\033[0m"

##############################
#        PATTERN RULES       #
##############################

# Compile main sources for non-test target.
$(BUILD_DIR)/main/%.o: $(MAIN_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) -c $< -o $@ $(LIB_FLAGS)

# Compile main sources for unit tests target.
$(BUILD_DIR)/main_for_test/%.o: $(MAIN_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $(TEST_FLAGS) -c $< -o $@ $(LIB_FLAGS) $(TEST_LIB_FLAGS)

# Compile test sources for unit tests target.
$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $(TEST_FLAGS) -c $< -o $@ $(LIB_FLAGS) $(TEST_LIB_FLAGS)

##############################
#           PHONY            #
##############################

.PHONY: all run test clean format

all: $(BIN_SOLVER) $(BIN_OCR) $(BIN_APP)

run: $(BIN_APP)
	@echo "Running app..."
	@./$<

test: $(BIN_TEST)
	@echo "Running unit tests..."
	@./$< --verbose

clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@echo "Cleaning executables..."
	@rm -rf $(BIN_SOLVER)
	@rm -rf $(BIN_OCR)
	@rm -rf $(BIN_TEST)
	@echo "Cleaning misc files..."
	@rm -rf extracted/
	@echo "\033[32mClean succeeded\033[0m"

format:
	@echo "Formatting source files..."
	@find . -iname '*.h' -o -iname '*.c' | xargs clang-format -i
	@echo "\033[32mFormatting succeeded\033[0m"

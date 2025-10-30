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

VERBOSE ?= 0

# Directories
SRC_DIR     := src
MAIN_DIR    := $(SRC_DIR)/main
TEST_DIR    := $(SRC_DIR)/test
BUILD_DIR   := build

# Compiler and flags
ifeq ($(VERBOSE),0)
CC          := @gcc
else
CC          := gcc
endif
CFLAGS      := -Wall -Wextra -fsanitize=address -I$(MAIN_DIR)
XCFLAGS     :=
TEST_FLAGS  := -DUNIT_TEST -fsanitize=address $(shell pkg-config --cflags --libs criterion)
GTK_FLAGS   := $(shell pkg-config --cflags --libs gtk+-3.0)
LIB_FLAGS   := -lm $(GTK_FLAGS)

# Source files
SRC_MAIN    := $(shell find $(MAIN_DIR) -name '*.c')
SRC_TEST    := $(shell find $(TEST_DIR) -name '*.c')

# Object files
OBJ_MAIN          := $(SRC_MAIN:$(MAIN_DIR)/%.c=$(BUILD_DIR)/main/%.o)
OBJ_MAIN_FOR_TEST := $(SRC_MAIN:$(MAIN_DIR)/%.c=$(BUILD_DIR)/main_for_test/%.o)
OBJ_TEST          := $(SRC_TEST:$(TEST_DIR)/%.c=$(BUILD_DIR)/test/%.o)

# Executables
BIN_SOLVER       := $(BUILD_DIR)/solver
BIN_IMAGE_LOADER := $(BUILD_DIR)/image_loader
BIN_LOCATION 	 := $(BUILD_DIR)/location
BIN_ROTATION     := $(BUILD_DIR)/rotation
# BIN_APP          := $(BUILD_DIR)/app
BIN_TEST         := $(BUILD_DIR)/run_tests

##############################
#          TARGETS           #
##############################

# Solver target
$(BIN_SOLVER): $(filter $(BUILD_DIR)/main/solver/%.o,$(OBJ_MAIN))
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)

# Image loader target
$(BIN_IMAGE_LOADER): $(filter $(BUILD_DIR)/main/image_loader/%.o,$(OBJ_MAIN))
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)

# Location target
$(BIN_LOCATION): $(filter $(BUILD_DIR)/main/bounding_boxes/%.o,$(OBJ_MAIN)) $(filter $(BUILD_DIR)/main/image_loader/%.o,$(OBJ_MAIN)) $(filter $(BUILD_DIR)/main/matrix/%.o,$(OBJ_MAIN)) $(filter $(BUILD_DIR)/main/extract_char/%.o,$(OBJ_MAIN)) $(filter $(BUILD_DIR)/main/utils/%.o,$(OBJ_MAIN))
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)

$(BIN_ROTATION): $(filter $(BUILD_DIR)/main/image_loader/%.o,$(OBJ_MAIN)) \
                      $(filter $(BUILD_DIR)/main/matrix/%.o,$(OBJ_MAIN)) \
                      $(filter $(BUILD_DIR)/main/bounding_boxes/pretreatment.o,$(OBJ_MAIN)) \
                      src/main/rotation/rotation.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)


## App target
#$(BIN_APP): $(filter %app/%.o,$(OBJ_MAIN))
#	@mkdir -p $(@D)
#	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)

# Test binary
$(BIN_TEST): $(OBJ_MAIN_FOR_TEST) $(OBJ_TEST)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(TEST_FLAGS) $(LIB_FLAGS)

##############################
#        PATTERN RULES       #
##############################

# Compile main sources for normal executables
$(BUILD_DIR)/main/%.o: $(MAIN_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) -c $< -o $@ $(LIB_FLAGS)

# Compile main sources for tests
$(BUILD_DIR)/main_for_test/%.o: $(MAIN_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) -c $< -o $@ $(TEST_FLAGS) $(LIB_FLAGS)

# Compile test sources
$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(XCFLAGS) -c $< -o $@ $(TEST_FLAGS) $(LIB_FLAGS)

##############################
#           PHONY            #
##############################

all: $(BIN_SOLVER) $(BIN_ROTATION) #$(BIN_IMAGE_LOADER) #$(BIN_APP)

#run: $(BIN_APP)
#	@echo "Running app..."
#	@./$<

test: $(BIN_TEST)
	@echo "Running unit tests..."
	@./$< --verbose

clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@rm -rf extracted/

format:
	@echo "Formatting source files..."
	@find . -iname '*.h' -o -iname '*.c' | xargs clang-format -i

.PHONY: all test clean format

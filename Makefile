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

# The number of iteration for the random tests.
TEST_ITERATION ?= 100
ifeq ($(TEST_ITERATION),0)
$(error Macro TEST_ITERATION has been set 0)
endif

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
CFLAGS         = -Wall -Wextra -fsanitize=address,undefined -I$(MAIN_DIR) -O1
# Additional C flags.
XCFLAGS        =
# C flags for unit testing.
TEST_FLAGS     = -DUNIT_TEST -D TEST_ITERATION=$(TEST_ITERATION) -I$(TEST_DIR)
# C flags for unit test libraries import.
TEST_LIB_FLAGS = $(shell pkg-config --cflags --libs criterion)
# C flags for libraries import.
LIB_FLAGS      = -lm $(shell pkg-config --cflags --libs gtk+-3.0)

# SIMD (AVX) flags.
AVX ?= 0
ifeq ($(AVX),2)
CFLAGS := $(CFLAGS) -mavx2 -mfma -DUSE_AVX -DUSE_AVX_2
else ifeq ($(AVX),512)
$(error AVX 512 is not supported yet)
CFLAGS := $(CFLAGS) -mavx512 -mfma -DUSE_AVX -DUSE_AVX_512
else ifneq ($(AVX), 0)
$(error AVX macro has been set to an unrecognized value. Expected '2' or '512' and got $(AVX))
endif

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
BIN_SOLVER           = solver
# A executable file that displays a matrix from a matrix file.
BIN_MAT_DISPLAY      = mat_display
# OCR neural network training executable.
BIN_OCR              = ocr_train
# OCR dataset generation script.
BIN_OCR_DATASET      = ocr_dataset
# Program used to apply OCR on a single image.
BIN_DECODE_IMAGE     = decode_image
# Rotate test executable.
BIN_AUTO_ROTATE      = rotate
#Grid rebuild test executable
BIN_GRID_REBUILD     = grid_rebuild
#Wordlist rebuild test executable
BIN_WORDLIST_REBUILD = wordlist_rebuild
# Locates the elements in the image
BIN_LOCATION         = location
# Main application executable.
BIN_APP              = app
# Unit tests executable.
BIN_TEST             = run_tests

# Use to add a dependency to a main function in a make rule. The parameter is the source file containing the main function. You must not provide the extension '.c'.
define main
$(BUILD_MAIN_DIR)/$(1).o
endef

# Use to add a dependency to a directory in a make rule. The parameter is a space separated list of directories to be included. Their path must be given from the 'src/main' directory.
define import
$(foreach dir,$(1),$(filter $(BUILD_MAIN_DIR)/$(dir)/%.o,$(OBJ_MAIN)))
endef

##############################
#          TARGETS           #
##############################

# Solver target.
$(BIN_SOLVER): $(call import,solver) $(call main,solver/solver_main)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo -e "$@: \033[32mCompilation succeeded\033[0m"

# Matrix display target.
$(BIN_MAT_DISPLAY): $(call import,matrix utils/random) $(call main,matrix/mat_display_main)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo -e "$@: \033[32mCompilation succeeded\033[0m"

# OCR neural network training target.
$(BIN_OCR): $(call import,ocr matrix utils) $(call main,ocr/ocr_train_main)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo -e "$@: \033[32mCompilation succeeded\033[0m"

# # OCR dataset generation target.
# $(BIN_OCR_DATASET): $(call import,matrix image_loader utils pretreatment ocr) $(call main,ocr/ocr_dataset_main)
# 	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
# 	@echo -e "$@: \033[32mCompilation succeeded\033[0m"

# Decode image target.
$(BIN_DECODE_IMAGE): $(call import,matrix image_loader utils pretreatment ocr) $(call main,ocr/decode_letter_main)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo -e "$@: \033[32mCompilation succeeded\033[0m"

# Auto rotation target.
$(BIN_AUTO_ROTATE): $(call import,rotation pretreatment image_loader utils matrix) $(call main,rotation/rotate_main)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo -e "$@: \033[32mCompilation succeeded\033[0m"

# # Grid rebuild Target.
# $(BIN_GRID_REBUILD): $(call import,grid_rebuild image_loader matrix ocr pretreatment utils solver location rotation extract_char) $(call main,grid_rebuild/grid_rebuild_main)
# 	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
# 	@echo -e "$@: \033[32mCompilation succeeded\033[0m"

# # Wordlist rebuild target
# $(BIN_WORDLIST_REBUILD): $(call import,wordlist_rebuild image_loader matrix ocr pretreatment utils solver location rotation extract_char) $(call main,wordlist_rebuild/wordlist_rebuild_main)
# 	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
# 	@echo -e "$@: \033[32mCompilation succeeded\033[0m"


# Location target.
$(BIN_LOCATION): $(filter $(BUILD_MAIN_DIR)/location/%.o,$(OBJ_MAIN)) $(filter $(BUILD_MAIN_DIR)/rotation/%.o,$(OBJ_MAIN)) $(filter $(BUILD_MAIN_DIR)/pretreatment/%.o,$(OBJ_MAIN)) $(filter $(BUILD_MAIN_DIR)/image_loader/%.o,$(OBJ_MAIN)) $(filter $(BUILD_MAIN_DIR)/extract_char/%.o,$(OBJ_MAIN)) $(filter $(BUILD_MAIN_DIR)/utils/%.o,$(OBJ_MAIN)) $(filter $(BUILD_MAIN_DIR)/matrix/%.o,$(OBJ_MAIN)) $(BUILD_MAIN_DIR)/location/location_main.o
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo -e "$(BIN_LOCATION): \033[32mCompilation succeeded\033[0m"

# Main app target.
$(BIN_APP): $(call import,extract_char location ocr rotation utils image_loader matrix pretreatment solver grid_rebuild wordlist_rebuild) $(call main,gui/gui_main)
	$(CC) $(CFLAGS) $(XCFLAGS) $^ -o $@ $(LIB_FLAGS)
	@echo -e "$@: \033[32mCompilation succeeded\033[0m"

# Unit tests target.
$(BIN_TEST): $(OBJ_MAIN_FOR_TEST) $(OBJ_TEST_FOR_TEST)
	$(CC) $(CFLAGS) $(XCFLAGS) $(TEST_FLAGS) $^ -o $@ $(LIB_FLAGS) $(TEST_LIB_FLAGS)
	@echo -e "$@: \033[32mCompilation succeeded\033[0m"

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

all: $(BIN_SOLVER) $(BIN_MAT_DISPLAY) $(BIN_OCR) $(BIN_DECODE_IMAGE) $(BIN_AUTO_ROTATE) $(BIN_LOCATION) $(BIN_APP)

run: $(BIN_APP)
	@echo -e "Running app..."
	@./$<

test: $(BIN_TEST)
	@echo -e "Running unit tests..."
	@./$< --verbose

clean:
	@echo -e "Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@echo -e "Cleaning executables..."
	@rm -rf $(BIN_SOLVER)
	@rm -rf $(BIN_MAT_DISPLAY)
	@rm -rf $(BIN_OCR)
	@rm -rf $(BIN_OCR_DATASET)
	@rm -rf $(BIN_DECODE_IMAGE)
	@rm -rf $(BIN_AUTO_ROTATE)
	@rm -rf $(BIN_GRID_REBUILD)
	@rm -rf $(BIN_WORDLIST_REBUILD)
	@rm -rf $(BIN_LOCATION)
	@rm -rf $(BIN_APP)
	@rm -rf $(BIN_TEST)
	@echo -e "Cleaning test files..."
	@rm -rf save_and_load_random_test.matrix
	@rm -rf save_and_load_test.matrix
	@rm -rf save_and_load_random_test.dataset
	@echo -e "Cleaning misc files..."
	@rm -rf extracted/
	@echo -e "\033[32mClean succeeded\033[0m"

format:
	@echo -e "Formatting source files..."
	@find . -iname '*.h' -o -iname '*.c' | xargs clang-format -i
	@echo -e "\033[32mFormatting succeeded\033[0m"

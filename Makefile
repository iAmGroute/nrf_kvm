
# Command options

CC1     = clang-9 -cc1
CC1AS   = clang-9 -cc1as
LINKER  = /usr/share/segger_embedded_studio_for_arm_4.52c/bin/segger-ld
OBJCOPY = llvm-objcopy-9

TARGET = \
	-triple thumbv6m-none-eabi \
	-target-cpu cortex-m0      \
	-mrelocation-model static

CC1_OPTIONS = \
	-disable-free             \
	-fdata-sections           \
	-ffunction-sections       \
	-fno-builtin              \
	-fno-rtti                 \
	-fno-signed-char          \
	-fshort-enums             \
	-fstrict-enums            \
	-mconstructor-aliases     \
	-nobuiltininc             \
	-nostdsysteminc

LINKER_OPTIONS = \
    --add-region FLASH1=0x00040000@0x00000000 --add-region RAM1=0x00004000@0x20000000 \
    -mcpu=cortex-m0                                                                   \
    --entry=Reset_Handler                                                             \
    -D"__STACKSIZE__=1024" -D"__STACKSIZE_PROCESS__=0" -D"__HEAPSIZE__=0"             \
    --list-all-undefineds --fatal-warnings --verbose                                  \
    -T"lib/Assembly/NRF51822/ses_linker.icf"

# Preprocessor definitions

DEFINES = \
	-D "__ARM_ARCH_6M__"     \
	-D "__SEGGER_LINKER"     \
	-D "__SES_ARM"           \
	-D "__SES_VERSION=45203" \
	-D "__SIZEOF_WCHAR_T=4"  \
	-D "ARM_MATH_CM0"        \
	-D "FLASH_PLACEMENT=1"   \
	-D "__nRF_FAMILY"        \
	-D "NRF51"               \
	-D "NRF51822"            \
	-D "NRF51822_XXAA"       \
	-D "__HEAP_SIZE__=0"     \
	-D "NO_SYSTEM_INIT"

# Include directories

INCLUDES = \
    -isystem "../nrfx_deps/output" \
    -I "."

# Source files

FILES_S   = \
	lib/Assembly/ses_startup_thumb.s               \
	lib/Assembly/NRF51822/ses_startup_nrf_common.s \
	lib/Assembly/NRF51822/ses_startup_nrf51.s

FILES_CPP = \
	nrf_kvm/BuildMe.cpp


### The rest is automatic


.PHONY: default
default: all

# Object files, corresponding to the input files
OBJECTS_S   = $(FILES_S:.s=.s.o)
OBJECTS_CPP = $(FILES_CPP:.cpp=.cpp.o)
OBJECTS     = $(OBJECTS_S) $(OBJECTS_CPP)


# Preprocess .s
%_PP.s: %.s
	mkdir -p "build/$(@D)"
	$(CC1) $(TARGET) $(DEFINES) $(INCLUDES) -E "$<" -o "build/$@"

# Preprocess .cpp
%_PP.cpp: %.cpp
	mkdir -p "build/$(@D)"
	# Output all '#defines' only
	$(CC1) $(TARGET) $(CC1_OPTIONS) -x c++ -std=c++11 $(DEFINES) $(INCLUDES) -E -dM "$<" -o "build/$@.h"
	# Output the preprocessed file
	$(CC1) $(TARGET) $(CC1_OPTIONS) -x c++ -std=c++11 $(DEFINES) $(INCLUDES) -E     "$<" -o "build/$@"

# Compile .cpp
%_PP.cpp.s: %_PP.cpp
	mkdir -p "build/$(@D)"
	$(CC1) $(TARGET) $(CC1_OPTIONS) -x c++ -std=c++11 -Wall -O3 -S "build/$<" -o "build/$@"

# Assemble .s
%.s.o: %_PP.s
	mkdir -p "build/$(@D)"
	$(CC1AS) $(TARGET) "build/$<" -filetype "obj" -o "build/$@"

# Assemble .cpp
%.cpp.o: %_PP.cpp.s
	mkdir -p "build/$(@D)"
	$(CC1AS) $(TARGET) "build/$<" -filetype "obj" -o "build/$@"

# Build an index of all the object files for the linker
build.ind:
	printf '"build/%s"\n' $(OBJECTS) > "build/build.ind"

# Link
build.elf: build.ind $(OBJECTS)
	$(LINKER) $(LINKER_OPTIONS) -Map "build/build.map" -o "build/build.elf" "@build/build.ind"

# Convert to Intel hex format
build.hex: build.elf
	$(OBJCOPY) "build/build.elf" -O ihex   "build/build.hex"

# Convert to plain binary format
build.bin: build.hex
	objcopy -I ihex "build/build.hex" -O binary "build/build.bin"
	# This produces a bin of the entire flash space (512 MiB, padded with zeros)
	# $(OBJCOPY) "build/build.elf" -O binary "build/build.bin"
	# This produces wrong output when the hex file skips addresses
	# $(OBJCOPY) -I ihex "build/build.hex" -O binary "build/build.bin"

# Main target !

.PHONY: all
all: build.elf build.hex build.bin


# Cleanup
.PHONY: clean
clean:
	rm -rf build/*
	mkdir -p build/

# Rebuild
.PHONY: re
re: clean all


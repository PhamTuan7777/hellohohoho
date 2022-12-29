﻿# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = D:\vscodecc++\cmake\bin\cmake.exe

# The command to remove a file.
RM = D:\vscodecc++\cmake\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\vscodecc++\Pico\picoproject\TFT_TEST

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\vscodecc++\Pico\picoproject\TFT_TEST\build

# Include any dependencies generated for this target.
include pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\depend.make
# Include any dependencies generated by the compiler for this target.
include pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\compiler_depend.make

# Include the progress variables for this target.
include pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\progress.make

# Include the compile flags for this target's objects.
include pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\flags.make

pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\compile_time_choice.S.obj: pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\flags.make
pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\compile_time_choice.S.obj: D:\vscodecc++\Pico\picoproject\pico-sdk\src\rp2_common\boot_stage2\compile_time_choice.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\vscodecc++\Pico\picoproject\TFT_TEST\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building ASM object pico-sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/compile_time_choice.S.obj"
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build\pico-sdk\src\rp2_common\boot_stage2
	D:\VSCODE~1\GNU_ARM\102021~1.10\bin\AR19DD~1.EXE $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles\bs2_default.dir\compile_time_choice.S.obj   -c D:\vscodecc++\Pico\picoproject\pico-sdk\src\rp2_common\boot_stage2\compile_time_choice.S
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build

pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\compile_time_choice.S.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing ASM source to CMakeFiles/bs2_default.dir/compile_time_choice.S.i"
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build\pico-sdk\src\rp2_common\boot_stage2
	D:\VSCODE~1\GNU_ARM\102021~1.10\bin\AR19DD~1.EXE $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -E D:\vscodecc++\Pico\picoproject\pico-sdk\src\rp2_common\boot_stage2\compile_time_choice.S > CMakeFiles\bs2_default.dir\compile_time_choice.S.i
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build

pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\compile_time_choice.S.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling ASM source to assembly CMakeFiles/bs2_default.dir/compile_time_choice.S.s"
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build\pico-sdk\src\rp2_common\boot_stage2
	D:\VSCODE~1\GNU_ARM\102021~1.10\bin\AR19DD~1.EXE $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -S D:\vscodecc++\Pico\picoproject\pico-sdk\src\rp2_common\boot_stage2\compile_time_choice.S -o CMakeFiles\bs2_default.dir\compile_time_choice.S.s
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build

# Object files for target bs2_default
bs2_default_OBJECTS = \
"CMakeFiles\bs2_default.dir\compile_time_choice.S.obj"

# External object files for target bs2_default
bs2_default_EXTERNAL_OBJECTS =

pico-sdk\src\rp2_common\boot_stage2\bs2_default.elf: pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\compile_time_choice.S.obj
pico-sdk\src\rp2_common\boot_stage2\bs2_default.elf: pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\build.make
pico-sdk\src\rp2_common\boot_stage2\bs2_default.elf: D:\vscodecc++\Pico\picoproject\pico-sdk\src\rp2_common\boot_stage2\boot_stage2.ld
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\vscodecc++\Pico\picoproject\TFT_TEST\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking ASM executable bs2_default.elf"
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build\pico-sdk\src\rp2_common\boot_stage2
	D:\VSCODE~1\GNU_ARM\102021~1.10\bin\AR19DD~1.EXE -mcpu=cortex-m0plus -mthumb -Og -g -Wl,--build-id=none --specs=nosys.specs -nostartfiles -Wl,--script=D:/vscodecc++/Pico/picoproject/pico-sdk/src/rp2_common/boot_stage2/boot_stage2.ld -Wl,-Map=bs2_default.elf.map $(bs2_default_OBJECTS) $(bs2_default_EXTERNAL_OBJECTS) -o bs2_default.elf 
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build\pico-sdk\src\rp2_common\boot_stage2
	echo >nul && "D:\vscodecc++\GNU_ARM\10 2021.10\bin\arm-none-eabi-objdump.exe" -h D:/vscodecc++/Pico/picoproject/TFT_TEST/build/pico-sdk/src/rp2_common/boot_stage2/bs2_default.elf >bs2_default.dis
	echo >nul && "D:\vscodecc++\GNU_ARM\10 2021.10\bin\arm-none-eabi-objdump.exe" -d D:/vscodecc++/Pico/picoproject/TFT_TEST/build/pico-sdk/src/rp2_common/boot_stage2/bs2_default.elf >>bs2_default.dis
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build

# Rule to build all files generated by this target.
pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\build: pico-sdk\src\rp2_common\boot_stage2\bs2_default.elf
.PHONY : pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\build

pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\clean:
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build\pico-sdk\src\rp2_common\boot_stage2
	$(CMAKE_COMMAND) -P CMakeFiles\bs2_default.dir\cmake_clean.cmake
	cd D:\vscodecc++\Pico\picoproject\TFT_TEST\build
.PHONY : pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\clean

pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" D:\vscodecc++\Pico\picoproject\TFT_TEST D:\vscodecc++\Pico\picoproject\pico-sdk\src\rp2_common\boot_stage2 D:\vscodecc++\Pico\picoproject\TFT_TEST\build D:\vscodecc++\Pico\picoproject\TFT_TEST\build\pico-sdk\src\rp2_common\boot_stage2 D:\vscodecc++\Pico\picoproject\TFT_TEST\build\pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default.dir\depend


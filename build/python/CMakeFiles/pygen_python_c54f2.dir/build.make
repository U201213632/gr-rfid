# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/xin/gr-rfid

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xin/gr-rfid/build

# Utility rule file for pygen_python_c54f2.

# Include the progress variables for this target.
include python/CMakeFiles/pygen_python_c54f2.dir/progress.make

python/CMakeFiles/pygen_python_c54f2: python/__init__.pyc
python/CMakeFiles/pygen_python_c54f2: python/__init__.pyo


python/__init__.pyc: ../python/__init__.py
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xin/gr-rfid/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating __init__.pyc"
	cd /home/xin/gr-rfid/build/python && /usr/bin/python3 /home/xin/gr-rfid/build/python_compile_helper.py /home/xin/gr-rfid/python/__init__.py /home/xin/gr-rfid/build/python/__init__.pyc

python/__init__.pyo: ../python/__init__.py
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xin/gr-rfid/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating __init__.pyo"
	cd /home/xin/gr-rfid/build/python && /usr/bin/python3 -O /home/xin/gr-rfid/build/python_compile_helper.py /home/xin/gr-rfid/python/__init__.py /home/xin/gr-rfid/build/python/__init__.pyo

pygen_python_c54f2: python/CMakeFiles/pygen_python_c54f2
pygen_python_c54f2: python/__init__.pyc
pygen_python_c54f2: python/__init__.pyo
pygen_python_c54f2: python/CMakeFiles/pygen_python_c54f2.dir/build.make

.PHONY : pygen_python_c54f2

# Rule to build all files generated by this target.
python/CMakeFiles/pygen_python_c54f2.dir/build: pygen_python_c54f2

.PHONY : python/CMakeFiles/pygen_python_c54f2.dir/build

python/CMakeFiles/pygen_python_c54f2.dir/clean:
	cd /home/xin/gr-rfid/build/python && $(CMAKE_COMMAND) -P CMakeFiles/pygen_python_c54f2.dir/cmake_clean.cmake
.PHONY : python/CMakeFiles/pygen_python_c54f2.dir/clean

python/CMakeFiles/pygen_python_c54f2.dir/depend:
	cd /home/xin/gr-rfid/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xin/gr-rfid /home/xin/gr-rfid/python /home/xin/gr-rfid/build /home/xin/gr-rfid/build/python /home/xin/gr-rfid/build/python/CMakeFiles/pygen_python_c54f2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : python/CMakeFiles/pygen_python_c54f2.dir/depend

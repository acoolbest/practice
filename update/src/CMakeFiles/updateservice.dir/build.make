# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

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
CMAKE_SOURCE_DIR = /home/charge/share/charge_plus/update

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/charge/share/charge_plus/update

# Include any dependencies generated for this target.
include src/CMakeFiles/updateservice.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/updateservice.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/updateservice.dir/flags.make

src/CMakeFiles/updateservice.dir/UpdateService.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/UpdateService.o: src/UpdateService.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/UpdateService.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/UpdateService.o -c /home/charge/share/charge_plus/update/src/UpdateService.cpp

src/CMakeFiles/updateservice.dir/UpdateService.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/UpdateService.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/UpdateService.cpp > CMakeFiles/updateservice.dir/UpdateService.i

src/CMakeFiles/updateservice.dir/UpdateService.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/UpdateService.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/UpdateService.cpp -o CMakeFiles/updateservice.dir/UpdateService.s

src/CMakeFiles/updateservice.dir/UpdateService.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/UpdateService.o.requires

src/CMakeFiles/updateservice.dir/UpdateService.o.provides: src/CMakeFiles/updateservice.dir/UpdateService.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/UpdateService.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/UpdateService.o.provides

src/CMakeFiles/updateservice.dir/UpdateService.o.provides.build: src/CMakeFiles/updateservice.dir/UpdateService.o

src/CMakeFiles/updateservice.dir/PublicFunction.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/PublicFunction.o: src/PublicFunction.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/PublicFunction.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/PublicFunction.o -c /home/charge/share/charge_plus/update/src/PublicFunction.cpp

src/CMakeFiles/updateservice.dir/PublicFunction.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/PublicFunction.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/PublicFunction.cpp > CMakeFiles/updateservice.dir/PublicFunction.i

src/CMakeFiles/updateservice.dir/PublicFunction.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/PublicFunction.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/PublicFunction.cpp -o CMakeFiles/updateservice.dir/PublicFunction.s

src/CMakeFiles/updateservice.dir/PublicFunction.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/PublicFunction.o.requires

src/CMakeFiles/updateservice.dir/PublicFunction.o.provides: src/CMakeFiles/updateservice.dir/PublicFunction.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/PublicFunction.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/PublicFunction.o.provides

src/CMakeFiles/updateservice.dir/PublicFunction.o.provides.build: src/CMakeFiles/updateservice.dir/PublicFunction.o

src/CMakeFiles/updateservice.dir/JsonWriter.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/JsonWriter.o: src/JsonWriter.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/JsonWriter.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/JsonWriter.o -c /home/charge/share/charge_plus/update/src/JsonWriter.cpp

src/CMakeFiles/updateservice.dir/JsonWriter.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/JsonWriter.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/JsonWriter.cpp > CMakeFiles/updateservice.dir/JsonWriter.i

src/CMakeFiles/updateservice.dir/JsonWriter.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/JsonWriter.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/JsonWriter.cpp -o CMakeFiles/updateservice.dir/JsonWriter.s

src/CMakeFiles/updateservice.dir/JsonWriter.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/JsonWriter.o.requires

src/CMakeFiles/updateservice.dir/JsonWriter.o.provides: src/CMakeFiles/updateservice.dir/JsonWriter.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/JsonWriter.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/JsonWriter.o.provides

src/CMakeFiles/updateservice.dir/JsonWriter.o.provides.build: src/CMakeFiles/updateservice.dir/JsonWriter.o

src/CMakeFiles/updateservice.dir/inifile.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/inifile.o: src/inifile.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/inifile.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/inifile.o -c /home/charge/share/charge_plus/update/src/inifile.cpp

src/CMakeFiles/updateservice.dir/inifile.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/inifile.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/inifile.cpp > CMakeFiles/updateservice.dir/inifile.i

src/CMakeFiles/updateservice.dir/inifile.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/inifile.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/inifile.cpp -o CMakeFiles/updateservice.dir/inifile.s

src/CMakeFiles/updateservice.dir/inifile.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/inifile.o.requires

src/CMakeFiles/updateservice.dir/inifile.o.provides: src/CMakeFiles/updateservice.dir/inifile.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/inifile.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/inifile.o.provides

src/CMakeFiles/updateservice.dir/inifile.o.provides.build: src/CMakeFiles/updateservice.dir/inifile.o

src/CMakeFiles/updateservice.dir/HttpRequestInterface.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/HttpRequestInterface.o: src/HttpRequestInterface.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/HttpRequestInterface.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/HttpRequestInterface.o -c /home/charge/share/charge_plus/update/src/HttpRequestInterface.cpp

src/CMakeFiles/updateservice.dir/HttpRequestInterface.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/HttpRequestInterface.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/HttpRequestInterface.cpp > CMakeFiles/updateservice.dir/HttpRequestInterface.i

src/CMakeFiles/updateservice.dir/HttpRequestInterface.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/HttpRequestInterface.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/HttpRequestInterface.cpp -o CMakeFiles/updateservice.dir/HttpRequestInterface.s

src/CMakeFiles/updateservice.dir/HttpRequestInterface.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/HttpRequestInterface.o.requires

src/CMakeFiles/updateservice.dir/HttpRequestInterface.o.provides: src/CMakeFiles/updateservice.dir/HttpRequestInterface.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/HttpRequestInterface.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/HttpRequestInterface.o.provides

src/CMakeFiles/updateservice.dir/HttpRequestInterface.o.provides.build: src/CMakeFiles/updateservice.dir/HttpRequestInterface.o

src/CMakeFiles/updateservice.dir/HttpRequest.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/HttpRequest.o: src/HttpRequest.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/HttpRequest.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/HttpRequest.o -c /home/charge/share/charge_plus/update/src/HttpRequest.cpp

src/CMakeFiles/updateservice.dir/HttpRequest.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/HttpRequest.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/HttpRequest.cpp > CMakeFiles/updateservice.dir/HttpRequest.i

src/CMakeFiles/updateservice.dir/HttpRequest.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/HttpRequest.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/HttpRequest.cpp -o CMakeFiles/updateservice.dir/HttpRequest.s

src/CMakeFiles/updateservice.dir/HttpRequest.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/HttpRequest.o.requires

src/CMakeFiles/updateservice.dir/HttpRequest.o.provides: src/CMakeFiles/updateservice.dir/HttpRequest.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/HttpRequest.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/HttpRequest.o.provides

src/CMakeFiles/updateservice.dir/HttpRequest.o.provides.build: src/CMakeFiles/updateservice.dir/HttpRequest.o

src/CMakeFiles/updateservice.dir/FileDown.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/FileDown.o: src/FileDown.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/FileDown.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/FileDown.o -c /home/charge/share/charge_plus/update/src/FileDown.cpp

src/CMakeFiles/updateservice.dir/FileDown.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/FileDown.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/FileDown.cpp > CMakeFiles/updateservice.dir/FileDown.i

src/CMakeFiles/updateservice.dir/FileDown.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/FileDown.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/FileDown.cpp -o CMakeFiles/updateservice.dir/FileDown.s

src/CMakeFiles/updateservice.dir/FileDown.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/FileDown.o.requires

src/CMakeFiles/updateservice.dir/FileDown.o.provides: src/CMakeFiles/updateservice.dir/FileDown.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/FileDown.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/FileDown.o.provides

src/CMakeFiles/updateservice.dir/FileDown.o.provides.build: src/CMakeFiles/updateservice.dir/FileDown.o

src/CMakeFiles/updateservice.dir/fileMd5.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/fileMd5.o: src/fileMd5.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/fileMd5.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/fileMd5.o -c /home/charge/share/charge_plus/update/src/fileMd5.cpp

src/CMakeFiles/updateservice.dir/fileMd5.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/fileMd5.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/fileMd5.cpp > CMakeFiles/updateservice.dir/fileMd5.i

src/CMakeFiles/updateservice.dir/fileMd5.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/fileMd5.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/fileMd5.cpp -o CMakeFiles/updateservice.dir/fileMd5.s

src/CMakeFiles/updateservice.dir/fileMd5.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/fileMd5.o.requires

src/CMakeFiles/updateservice.dir/fileMd5.o.provides: src/CMakeFiles/updateservice.dir/fileMd5.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/fileMd5.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/fileMd5.o.provides

src/CMakeFiles/updateservice.dir/fileMd5.o.provides.build: src/CMakeFiles/updateservice.dir/fileMd5.o

src/CMakeFiles/updateservice.dir/MyResultClass.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/MyResultClass.o: src/MyResultClass.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/MyResultClass.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/MyResultClass.o -c /home/charge/share/charge_plus/update/src/MyResultClass.cpp

src/CMakeFiles/updateservice.dir/MyResultClass.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/MyResultClass.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/MyResultClass.cpp > CMakeFiles/updateservice.dir/MyResultClass.i

src/CMakeFiles/updateservice.dir/MyResultClass.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/MyResultClass.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/MyResultClass.cpp -o CMakeFiles/updateservice.dir/MyResultClass.s

src/CMakeFiles/updateservice.dir/MyResultClass.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/MyResultClass.o.requires

src/CMakeFiles/updateservice.dir/MyResultClass.o.provides: src/CMakeFiles/updateservice.dir/MyResultClass.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/MyResultClass.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/MyResultClass.o.provides

src/CMakeFiles/updateservice.dir/MyResultClass.o.provides.build: src/CMakeFiles/updateservice.dir/MyResultClass.o

src/CMakeFiles/updateservice.dir/common.o: src/CMakeFiles/updateservice.dir/flags.make
src/CMakeFiles/updateservice.dir/common.o: src/common.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/charge/share/charge_plus/update/CMakeFiles $(CMAKE_PROGRESS_10)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/updateservice.dir/common.o"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/updateservice.dir/common.o -c /home/charge/share/charge_plus/update/src/common.cpp

src/CMakeFiles/updateservice.dir/common.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/updateservice.dir/common.i"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/charge/share/charge_plus/update/src/common.cpp > CMakeFiles/updateservice.dir/common.i

src/CMakeFiles/updateservice.dir/common.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/updateservice.dir/common.s"
	cd /home/charge/share/charge_plus/update/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/charge/share/charge_plus/update/src/common.cpp -o CMakeFiles/updateservice.dir/common.s

src/CMakeFiles/updateservice.dir/common.o.requires:
.PHONY : src/CMakeFiles/updateservice.dir/common.o.requires

src/CMakeFiles/updateservice.dir/common.o.provides: src/CMakeFiles/updateservice.dir/common.o.requires
	$(MAKE) -f src/CMakeFiles/updateservice.dir/build.make src/CMakeFiles/updateservice.dir/common.o.provides.build
.PHONY : src/CMakeFiles/updateservice.dir/common.o.provides

src/CMakeFiles/updateservice.dir/common.o.provides.build: src/CMakeFiles/updateservice.dir/common.o

# Object files for target updateservice
updateservice_OBJECTS = \
"CMakeFiles/updateservice.dir/UpdateService.o" \
"CMakeFiles/updateservice.dir/PublicFunction.o" \
"CMakeFiles/updateservice.dir/JsonWriter.o" \
"CMakeFiles/updateservice.dir/inifile.o" \
"CMakeFiles/updateservice.dir/HttpRequestInterface.o" \
"CMakeFiles/updateservice.dir/HttpRequest.o" \
"CMakeFiles/updateservice.dir/FileDown.o" \
"CMakeFiles/updateservice.dir/fileMd5.o" \
"CMakeFiles/updateservice.dir/MyResultClass.o" \
"CMakeFiles/updateservice.dir/common.o"

# External object files for target updateservice
updateservice_EXTERNAL_OBJECTS =

/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/UpdateService.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/PublicFunction.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/JsonWriter.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/inifile.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/HttpRequestInterface.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/HttpRequest.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/FileDown.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/fileMd5.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/MyResultClass.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/common.o
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/build.make
/home/charge/share/charge_plus/bin/updateservice: /home/charge/share/charge_plus/bin/liboss_c_sdk.so
/home/charge/share/charge_plus/bin/updateservice: /home/charge/share/charge_plus/bin/libjson.so
/home/charge/share/charge_plus/bin/updateservice: /usr/lib/x86_64-linux-gnu/libaprutil-1.so
/home/charge/share/charge_plus/bin/updateservice: /usr/lib/x86_64-linux-gnu/libapr-1.so
/home/charge/share/charge_plus/bin/updateservice: /usr/lib/libmxml.so
/home/charge/share/charge_plus/bin/updateservice: /usr/lib/x86_64-linux-gnu/libcurl.so
/home/charge/share/charge_plus/bin/updateservice: /usr/lib/x86_64-linux-gnu/libpthread.so
/home/charge/share/charge_plus/bin/updateservice: /usr/lib/x86_64-linux-gnu/libssl.so
/home/charge/share/charge_plus/bin/updateservice: /usr/lib/x86_64-linux-gnu/libcrypto.so
/home/charge/share/charge_plus/bin/updateservice: /usr/lib/x86_64-linux-gnu/librt.so
/home/charge/share/charge_plus/bin/updateservice: src/CMakeFiles/updateservice.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable /home/charge/share/charge_plus/bin/updateservice"
	cd /home/charge/share/charge_plus/update/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/updateservice.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/updateservice.dir/build: /home/charge/share/charge_plus/bin/updateservice
.PHONY : src/CMakeFiles/updateservice.dir/build

src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/UpdateService.o.requires
src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/PublicFunction.o.requires
src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/JsonWriter.o.requires
src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/inifile.o.requires
src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/HttpRequestInterface.o.requires
src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/HttpRequest.o.requires
src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/FileDown.o.requires
src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/fileMd5.o.requires
src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/MyResultClass.o.requires
src/CMakeFiles/updateservice.dir/requires: src/CMakeFiles/updateservice.dir/common.o.requires
.PHONY : src/CMakeFiles/updateservice.dir/requires

src/CMakeFiles/updateservice.dir/clean:
	cd /home/charge/share/charge_plus/update/src && $(CMAKE_COMMAND) -P CMakeFiles/updateservice.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/updateservice.dir/clean

src/CMakeFiles/updateservice.dir/depend:
	cd /home/charge/share/charge_plus/update && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/charge/share/charge_plus/update /home/charge/share/charge_plus/update/src /home/charge/share/charge_plus/update /home/charge/share/charge_plus/update/src /home/charge/share/charge_plus/update/src/CMakeFiles/updateservice.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/updateservice.dir/depend

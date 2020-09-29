# My compatibility libraries

## Structure of directories

* library_name
* library_name/BUILD_QMAKE - makefile for qmake
* library_name/include - include files
* library_name/src - source files
* library_name/src/null - dummy implementation of library
* library_name/src/stdc - implementation using standard C
* library_name/src/win32 - implementation for Win32 platform
* library_name/tests - source files for tests
cmake_minimum_required(VERSION 3.10)
project(tabbit C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(
	tabbit
	src/args.c
	src/fsutils.c
	src/tabbit.c
	src/utils.c
	src/main.c
)

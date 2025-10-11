cmake_minimum_required(VERSION 3.27)
get_filename_component(PROJ_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJ_NAME} LANGUAGES CXX)

# Collect all .c, .cpp, .h, .tpp files from src/ & include/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.cpp include/*.c include/*.h include/*.tpp)
file(GLOB_RECURSE SERVERS CONFIGURE_DEPENDS servers/*.cpp servers/*.c servers/*.h servers/*.tpp)

# Define executable path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${SERVERS}
		src/HandleTcp.cpp
		include/HandleTcp.h)

# Include headers from include/
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR}/servers)

# Set C++ standard
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 26)

# Statically link libraries - No STL dependencies.
target_link_options(${PROJECT_NAME} PRIVATE -static)

# Force .exe
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".exe")
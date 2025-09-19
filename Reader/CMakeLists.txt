cmake_minimum_required(VERSION 3.27)
project(L7_8E2 LANGUAGES CXX)

# Collect all cpp files from src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Collect all h files from include/
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.h)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include headers from include/
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Set C++ standard
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 26)
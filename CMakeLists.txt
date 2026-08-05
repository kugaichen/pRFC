cmake_minimum_required(VERSION 3.16)

project(pRFC VERSION 1.0.0 LANGUAGES CXX)

find_package(Threads REQUIRED)

add_library(prfc_core
    FilterReader.cpp
    Filter_Preprocess.cpp
    Hardware_ImprovedClassifierBuilding.cpp
    Hardware_Improved_statemachine.cpp
    Hardware_RFC_ClassifierBuilding.cpp
    Hardware_state_machine.cpp
    PacketLoad.cpp
)

target_compile_features(prfc_core PUBLIC cxx_std_17)
target_include_directories(prfc_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(prfc_core PUBLIC Threads::Threads)

if(MSVC)
    target_compile_options(prfc_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(prfc_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(prfc main.cpp)
target_link_libraries(prfc PRIVATE prfc_core)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME prfc_help COMMAND prfc --help)
    add_test(NAME prfc_requires_rules_file COMMAND prfc)
    set_tests_properties(prfc_requires_rules_file PROPERTIES WILL_FAIL TRUE)
endif()

install(TARGETS prfc RUNTIME DESTINATION bin)

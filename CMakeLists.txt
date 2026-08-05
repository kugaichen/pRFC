cmake_minimum_required(VERSION 3.16)

project(pRFC VERSION 1.0.0 LANGUAGES CXX)

# The synthesizable RTL lives in HW_source/ and is built with an FPGA/EDA tool.
# CMake provides the build entry point for the offline software implementation.
include(CTest)
add_subdirectory(software)

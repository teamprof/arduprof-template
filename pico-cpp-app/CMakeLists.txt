# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# include FreeRTOS path
include(freertos-path.cmake)
# include(${FREERTOS_KERNEL_PATH}/${freertos_port_path})

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Pull in FreeRTOS
include("${FREERTOS_KERNEL_PATH}/${freertos_port_path}/FreeRTOS_Kernel_import.cmake")
# include(FreeRTOS_Kernel_import.cmake)

project(pico-cpp-app C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

FILE(GLOB app_sources 
    src/*.cpp
    # src/main.cpp
    # src/freertos-util.cpp
    # src/AppContext.cpp
    src/thread/*.cpp
    src/peripheral/*.cpp
    src/util/*.cpp
)

add_executable(app 
    ${app_sources}
)

pico_set_program_name(app "app")
pico_set_program_version(app "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(app 1)
pico_enable_stdio_usb(app 0)
# pico_enable_stdio_uart(app 0)
# pico_enable_stdio_usb(app 1)



add_library(freertos INTERFACE)
target_sources(freertos INTERFACE)
target_include_directories(freertos INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(freertos INTERFACE
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap3
        # FreeRTOS-Kernel-Heap4
)



# Add the standard library to the build
target_link_libraries(app
        pico_stdlib
        pico_stdio_usb
        freertos
)

# Add the standard include files to the build
target_include_directories(app PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(app)


# set FreeRTOS kernel path
set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/FreeRTOS-Kernel)

# FreeRTOS port subdirectory for this platform (relative to $FREERTOS_KERNEL_PATH)
# If it is a RP2350 board the board name needs to be added here to select the right port
if(PICO_BOARD STREQUAL "pico2")
    set(freertos_port_path "portable/ThirdParty/GCC/RP2350_ARM_NTZ")
else()
    set(freertos_port_path "portable/ThirdParty/GCC/RP2040")
endif()

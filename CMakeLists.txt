cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME BKhoot)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Tell LVGL where lv_conf.h resides
set(LV_BUILD_CONF_DIR "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party")

# Add LVGL library
add_subdirectory(Middlewares/Third_Party/lvgl)

target_include_directories(lvgl PRIVATE
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2   # for cmsis_os2.h
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${CMAKE_SOURCE_DIR}/App/example/lvgl_example.c
    ${CMAKE_SOURCE_DIR}/Drivers/LCD/lcd.c
    ${CMAKE_SOURCE_DIR}/Drivers/Touch/touch.c
    ${CMAKE_SOURCE_DIR}/Drivers/EEPROM/at24c.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_SOURCE_DIR}/App/example
    ${CMAKE_SOURCE_DIR}/Drivers/LCD
    ${CMAKE_SOURCE_DIR}/Drivers/Touch
    ${CMAKE_SOURCE_DIR}/Drivers/EEPROM
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    lvgl
)

# =========================================================================
# Custom Build Steps & Targets
# =========================================================================

# Generate .hex and .bin files automatically after building the .elf
# add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
#     COMMAND arm-none-eabi-objcopy -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
#     COMMAND arm-none-eabi-objcopy -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
#     COMMENT "Generating .hex and .bin files for ${CMAKE_PROJECT_NAME}"
# )

# Flash Target using OpenOCD
add_custom_target(Flash
    # The command to run OpenOCD, flash the .elf, verify, reset, and exit
    COMMAND openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf verify reset exit"
    
    # Ensure the project is built BEFORE trying to flash
    DEPENDS ${CMAKE_PROJECT_NAME}
    
    # Message shown in the terminal
    COMMENT "Flashing ${CMAKE_PROJECT_NAME} to STM32F407ZGT6..."
    USES_TERMINAL
)
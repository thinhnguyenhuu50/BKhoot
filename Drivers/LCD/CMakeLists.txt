add_library(LCD STATIC
    lcd.c
)

target_include_directories(LCD PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}   # For lcd.h
    ${CMAKE_SOURCE_DIR}/Core/Inc  # For main.h (or pass via INTERFACE)
)

# Link stm32cubemx to get HAL/CMSIS includes and definitions
target_link_libraries(LCD PRIVATE stm32cubemx)
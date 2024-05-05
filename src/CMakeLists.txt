set(OVERCLOCK_ENABLED 0)    # 1: enable, 0: disable

if(OVERCLOCK_ENABLED)
    message(WARNING "Overclocking is enabled. This may damage your device. Use at your own risk.")
    # Overclocking profiles
    #      SYS_CLK  | FLASH_CLK | Voltage
    #  1  | 266MHz  |  133MHz   |  1.10(V) (default, stable, recommended for most devices)
    #  2  | 240MHz  |  120MHZ   |  1.10(V) (more stable)
    #  3  | 360MHz  |  90MHz    |  1.20(V)
    #  4  | 400MHz  |  100MHz   |  1.25(V)
    set(OVERCLOCK_PROFILE 4)

    if(OVERCLOCK_PROFILE EQUAL 1)
        set(SYS_CLK_KHZ 266000)             # CPU clock speed
        set(PERI_CLK_KHZ ${SYS_CLK_KHZ})    # Peripheral clock speed
    elseif(OVERCLOCK_PROFILE EQUAL 2)
        set(SYS_CLK_KHZ 240000)
        set(PERI_CLK_KHZ ${SYS_CLK_KHZ})
    elseif(OVERCLOCK_PROFILE EQUAL 3)
        set(SYS_CLK_KHZ 360000)
        set(PERI_CLK_KHZ ${SYS_CLK_KHZ})
    elseif(OVERCLOCK_PROFILE EQUAL 4)
        set(SYS_CLK_KHZ 400000)
        set(PERI_CLK_KHZ ${SYS_CLK_KHZ})
    else()
        message(FATAL_ERROR "Invalid overclocking profile")
    endif()
else()
    message(WARNING "Overclocking is disabled.")
    set(SYS_CLK_KHZ 125000) # CPU clock speed
    set(PERI_CLK_KHZ ${SYS_CLK_KHZ})    # Peripheral clock speed
endif()

# LCD Pins for SPI and PIO interface
set(TFT_SPIX      0)    # which spi controller will be used.
set(TFT_SCL_PIN   18)
set(TFT_SDA_PIN   19)
set(TFT_RES_PIN   15)
set(TFT_DC_PIN    14)
set(TFT_CS_PIN    13)
set(TFT_BLK_PIN   12)
set(TFT_HOR_RES   240)
set(TFT_VER_RES   320)
set(TFT_BUS_CLK_KHZ 62500)
set(DISP_OVER_PIO 1)  # 1: PIO, 0: SPI

SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Wl,--print-memory-usage")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--print-memory-usage")

include_directories(. include)

add_subdirectory(lvgl)

add_subdirectory(pio)
target_compile_definitions(pio_spi_tx PUBLIC TFT_DC_PIN=${TFT_DC_PIN})
target_compile_definitions(pio_spi_tx PUBLIC TFT_CS_PIN=${TFT_CS_PIN})
target_compile_definitions(pio_spi_tx PUBLIC TFT_BUS_CLK_KHZ=${TFT_BUS_CLK_KHZ})
target_compile_definitions(pio_spi_tx PUBLIC DEFAULT_PIO_CLK_KHZ=${PERI_CLK_KHZ})

add_executable(${ProjectName}
    main.c
    tft.c
    tft_st7789v.c
    porting/lv_port_disp_template.c
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${ProjectName}
    pico_stdlib
    pico_multicore
    hardware_spi
    hardware_pwm
    hardware_clocks
    pio_spi_tx
    pico_bootsel_via_double_reset
    lvgl lvgl::demos lvgl::examples
    FreeRTOS-Kernel-Heap3
    )
target_compile_definitions(${PROJECT_NAME} PUBLIC DEFAULT_SYS_CLK_KHZ=${SYS_CLK_KHZ})
target_compile_definitions(${PROJECT_NAME} PUBLIC DEFAULT_PERI_CLK_KHZ=${PERI_CLK_KHZ})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_SCL_PIN=${TFT_SCL_PIN})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_SDA_PIN=${TFT_SDA_PIN})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_RES_PIN=${TFT_RES_PIN})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_DC_PIN=${TFT_DC_PIN})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_CS_PIN=${TFT_CS_PIN})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_BLK_PIN=${TFT_BLK_PIN})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_HOR_RES=${TFT_HOR_RES})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_VER_RES=${TFT_VER_RES})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_SPIX=${TFT_SPIX})
target_compile_definitions(${PROJECT_NAME} PUBLIC TFT_BUS_CLK_KHZ=${TFT_BUS_CLK_KHZ})
target_compile_definitions(${PROJECT_NAME} PUBLIC DISP_OVER_PIO=${DISP_OVER_PIO})

# Note: If you are using a NOR flash like "w25q16". Just keep the following content.
# The maximum speed of "w25q16" is 133MHz, However, the clock speed of XIP QSPI is divided from "sys_clk".
# So, when your "sys_clk" is greater than 266MHz and default PICO_FLASH_SPI_CLKDIV=2, It will exceed the
# maximum speed, because PICO_FLASH_SPI_CLKDIV must be even, So 4 is good for most purpose, Otherwise,
# nothing should be done. These things will only useful when you overclocking the rp2040.
if(${SYS_CLK_KHZ} GREATER 266000)
    set(PICO_FLASH_SPI_CLKDIV 4)
else()
    set(PICO_FLASH_SPI_CLKDIV 2)
endif()
math(EXPR FLASH_CLK_KHZ "${SYS_CLK_KHZ} / ${PICO_FLASH_SPI_CLKDIV}")
target_compile_definitions(bs2_default PRIVATE PICO_FLASH_SPI_CLKDIV=${PICO_FLASH_SPI_CLKDIV})
target_compile_definitions(${PROJECT_NAME} PRIVATE FLASH_CLK_KHZ=${FLASH_CLK_KHZ})

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})

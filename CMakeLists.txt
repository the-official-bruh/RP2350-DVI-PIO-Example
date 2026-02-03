# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PROJECT_NAME PICO2_DVI_PIO) 
set(PICO_BOARD pico2 CACHE STRING "Board type")
#set(PICO_PLATFORM rp2350)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(${PROJECT_NAME} C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()
add_compile_definitions(PICO_PIO_USE_GPIO_BASE=1)

add_subdirectory(lib/libdvi)
add_subdirectory(lib/libgui)
add_subdirectory(lib/libsprite)

# Add executable. Default name is the project name, version 0.1
add_executable(${PROJECT_NAME}
	main.c
	common_dvi_pin_configs.h
	fonts/b_dseg7_classic_b_italic_72_font.h
	fonts/b_dseg7_classic_b_italic_72_font.c
)

set(DVI_DEFAULT_SERIAL_CONFIG "pico_sock_cfg" CACHE STRING
	"Select a default pin configuration from common_dvi_pin_configs.h")

#target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

pico_set_program_name(${PROJECT_NAME} "PICO2DVIPIO")
pico_set_program_version(${PROJECT_NAME} "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
	pico_stdlib
	pico_binary_info
	hardware_dma
	hardware_spi
	hardware_irq
	hardware_i2c
	hardware_adc
	hardware_pwm
	hardware_gpio
	hardware_pio
	hardware_clocks
	hardware_watchdog
	hardware_timer
	pico_time
	pico_rand
	hardware_uart
	pico_multicore
	pico_stdlib
	libdvi
	libgui
	libsprite
)

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE
	PICO_DEFAULT_UART_RX_PIN=1
	PICO_DEFAULT_UART_TX_PIN=0
	PICO_DEFAULT_UART=0
	DVI_VERTICAL_REPEAT=1
	DVI_N_TMDS_BUFFERS=3
	PICO_PIO_USE_GPIO_BASE=1
	DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
)
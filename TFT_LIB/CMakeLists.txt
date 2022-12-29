file(GLOB tft_h ILI9431.c ILI9431.h ILI9431_font.c ILI9431_font.h ILI9431_gfx.c ILI9431_gfx.h)
add_library(ILI9431 ${tft_h})


target_link_libraries(ILI9431 pico_stdlib hardware_spi)
target_include_directories(ILI9431 PUBLIC ../include/)

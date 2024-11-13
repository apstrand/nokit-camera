#IDF version is less than IDF5.0

idf_component_register(
    SRCS "esp32s3_wroom_cam.cpp" ${SRC_VER}
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "priv_include"
    REQUIRES driver spiffs spi_flash
    PRIV_REQUIRES fatfs esp_lcd
)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)



file(GLOB_RECURSE THREAD_SRCS "thread/*.cpp")
file(GLOB_RECURSE PERIPHERAL_SRCS "peripheral/*.cpp")

idf_component_register(
    SRCS 
        "main.cpp"
        "AppContext.cpp"
        ${THREAD_SRCS} 
        ${PERIPHERAL_SRCS}
    INCLUDE_DIRS 
        "."
    PRIV_REQUIRES 
        driver
        esp_psram
        esp_timer
        spi_flash
)
idf_component_register(
    SRCS
        "WS281xDriver.cpp"
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    REQUIRES
        driver freertos
)

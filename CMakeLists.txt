project(ws281x-driver-esp32)
    
add_library(${PROJECT_NAME}
    "WS281xDriver.cpp"
)
    
target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC idf::driver idf::freertos
)

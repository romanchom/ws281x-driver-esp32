# project(libLunaEsp32)

set(COMPONENT_SRCS  "WS281xDriver.cpp")

set(COMPONENT_ADD_INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

register_component()

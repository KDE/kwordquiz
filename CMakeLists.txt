project(kwordquiz)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

set(CMAKE_MODULE_PATH ${kwordquiz_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(LibKdeEdu REQUIRED)

add_subdirectory( doc )
add_subdirectory( src ) 

install(FILES kwordquiz.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)

set(TARGET google_drive_proxy)
project(${TARGET})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED QtWebKit)
INCLUDE( ${QT_USE_FILE} )

set(HEADERS
    networkaccessmanagerproxy.h
    networkaccessviewer.h
)


set(SOURCES
main.cpp
networkaccessmanagerproxy.cpp
networkaccessviewer.cpp
)

set(PROXY_UI
networkrequests.ui
)

set(MOC
networkaccessviewer.h
networkaccessmanagerproxy.h
)

set(DEPS
    google_drive
)

set(RESOURCES
)

qt4_wrap_cpp(SOURCES ${MOC})
qt4_wrap_ui(SOURCES ${PROXY_UI})
qt4_add_resources(SOURCES ${RESOURCES})
add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})
#add_executable(${TARGET} WIN32 ${HEADERS} ${SOURCES})
target_link_libraries(${TARGET} ${QT_LIBRARIES} ${DEPS})

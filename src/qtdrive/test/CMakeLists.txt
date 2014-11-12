set(TARGET google_drive_test)
project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED QtCore QtGui QtNetwork QtWebKit)
include(${QT_USE_FILE})
add_subdirectory(networkaccessmanagerproxy)
set(HEADERS
    main_window.h
    options_dialog.h
    options.h
)

set(MOC
    main_window.h
    options_dialog.h
)

set(SOURCES
    main.cpp
    main_window.cpp
    options_dialog.cpp
)

set(IFACES
    main_window.ui
    options_dialog.ui
)

set(RESOURCES
)

set(DEPS
    google_drive
)

qt4_wrap_cpp(SOURCES ${MOC})
qt4_wrap_ui(SOURCES ${IFACES})
qt4_add_resources(SOURCES ${RESOURCES})

add_executable(${TARGET} WIN32 ${HEADERS} ${SOURCES})
target_link_libraries(${TARGET} ${QT_LIBRARIES} ${DEPS})



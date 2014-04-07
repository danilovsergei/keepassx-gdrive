set(TARGET google_drive)

project(${TARGET})

find_package(Qt4 REQUIRED QtCore QtNetwork )
include(${QT_USE_FILE})

FIND_PACKAGE (QJSON REQUIRED)

INCLUDE_DIRECTORIES (
    ${QJSON_INCLUDE_DIR}
)

set(HEADERS
    defines.h
    tools.h
    drive_manager.h
    session.h
    file_info.h
    fields.h
    about_info.h
    command.h
    command_private.h
    authorized_command.h
    command_refresh_token.h
    command_oauth2.h
    command_about.h
    command_get.h
    command_delete.h
    command_file_list.h
    command_download_file.h
    command_upload_file.h
    command_update.h
    command_set_properties.h
    command_get_properties.h
)

set(MOC_HEADERS
    drive_manager.h
    session.h
    command.h
    authorized_command.h
    command_refresh_token.h
    command_oauth2.h
    command_about.h
    command_get.h
    command_delete.h
    command_file_list.h
    command_download_file.h
    command_upload_file.h
    command_update.h
    command_set_properties.h
    command_get_properties.h
)

set(SOURCES
    tools.cpp
    drive_manager.cpp
    session.cpp
    file_info.cpp
    fields.cpp
    about_info.cpp
    command.cpp
    authorized_command.cpp
    command_refresh_token.cpp
    command_oauth2.cpp
    command_about.cpp
    command_get.cpp
    command_delete.cpp
    command_file_list.cpp
    command_download_file.cpp
    command_upload_file.cpp
    command_update.cpp
    command_set_properties.cpp
    command_get_properties.cpp

)

set(DEPS
    qjson
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} ${DEPS} ${QJSON_LIBRARIES})




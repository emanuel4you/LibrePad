cmake_minimum_required(VERSION 3.16)
project(librepad LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "/usr/local/bin")

find_package(Qt6
    REQUIRED COMPONENTS Core Gui Widgets
    OPTIONAL_COMPONENTS PrintSupport
)

qt_add_executable(librepad
    main.cpp
    librepad.cpp librepad.h librepad.ui
)

set_target_properties(librepad PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(librepad PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

# Resources:
set(librepad_resource_files
    "images/info.png"
    "images/new.png"
    "images/open.png"
    "images/paste.png"
    "images/pencil.png"
    "images/print.png"
    "images/save.png"
    "images/save_as.png"
    "images/exit.png"
    "images/font.png"
    "images/copy.png"
    "images/cut.png"
    "images/edit_redo.png"
    "images/edit_undo.png"
    "images/next.png"
    "images/previous.png"
)

qt_add_resources(librepad "librepad"
    PREFIX
        "/"
    FILES
        ${librepad_resource_files}
)

if(TARGET Qt::PrintSupport)
    target_link_libraries(librepad PUBLIC
        Qt::PrintSupport
    )
endif()

install(TARGETS librepad
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

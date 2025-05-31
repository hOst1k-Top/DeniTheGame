cmake_minimum_required(VERSION 3.16)
project(DeniTheGame LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} 
    COMPONENTS 
        Core
        Gui
        Widgets
        SQL
        LinguistTools
)

qt_standard_project_setup()

set(HEADERS_FILES
    include/GameInitScreen.h
    include/CardDealler.h
    include/dbprovider.h
    include/SettingsManager.h
    include/Player.h
    include/GameManager.h
    include/DeniGame.h
    include/ClickableCard.h
    include/MemoryCardItem.h
    include/InteractiveGraphicsScene.h
    include/GameOverlayDialog.h
)

set(PROJECT_SOURCES
    main.cpp
    GameInitScreen.cpp
    CardDealler.cpp
    dbprovider.cpp
    SettingsManager.cpp
    Player.cpp
    GameManager.cpp
    DeniGame.cpp
    ClickableCard.cpp
    MemoryCardItem.cpp
    InteractiveGraphicsScene.cpp
    GameOverlayDialog.cpp
)

set(UI_FILES
    ui/GameInitScreen.ui
    ui/DeniGame.ui
)

set(RESOURCES_FILES
    resource/DeniTheGame.qrc
)

set(TRANSLATION_FILES
    translations/DeniTheGame_ru.ts
)

qt_add_executable(DeniTheGame 
    ${PROJECT_SOURCES} 
    ${HEADERS_FILES} 
    ${RESOURCES_FILES} 
    ${UI_FILES}
)

qt_add_translations(DeniTheGame
    TS_FILES ${TRANSLATION_FILES}
    QM_FILES_OUTPUT_VARIABLE qm_files
    SOURCES ${PROJECT_SOURCES} ${HEADERS_FILES} ${UI_FILES}
    LUPDATE_OPTIONS "-no-obsolete"
)

if(Qt6_FOUND)
    find_program(QT_LUPDATE_EXECUTABLE lupdate HINTS ${Qt6_DIR}/../../../bin)
else()
    find_program(QT_LUPDATE_EXECUTABLE lupdate HINTS ${Qt5_DIR}/../../../bin)
endif()

if(QT_LUPDATE_EXECUTABLE)
    add_custom_target(update_translations
        COMMAND ${QT_LUPDATE_EXECUTABLE} 
            ${PROJECT_SOURCES} ${HEADERS_FILES} ${UI_FILES}
            -ts ${TRANSLATION_FILES}
            -no-obsolete
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Updating translation files"
        VERBATIM
    )
    
    add_dependencies(DeniTheGame update_translations)
endif()

install(FILES ${qm_files} DESTINATION translations)

add_custom_command(TARGET DeniTheGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/translations
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${qm_files} ${CMAKE_BINARY_DIR}/translations/
    COMMENT "Copying translation files to build directory"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE ON
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Sql
)
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
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES ru)

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
)

set(UI_FILES
    ui/GameInitScreen.ui
    ui/DeniGame.ui
)

set(RESOURCES_FILES
    resource/DeniTheGame.qrc
)

set (TRANSLATION_FILES
    translations/DeniTheGame_ru.ts
)

qt_add_executable(DeniTheGame ${PROJECT_SOURCES} ${HEADERS_FILES} ${RESOURCES_FILES} ${UI_FILES})

qt6_add_translations(DeniTheGame
    QM_FILES_OUTPUT_VARIABLE qm_files
    TS_FILES ${TRANSLATION_FILES}
)
install(FILES ${qm_files} DESTINATION ${CMAKE_BINARY_DIR}/translations)



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


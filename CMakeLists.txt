cmake_minimum_required(VERSION 3.28)
project(Oasis)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(Oasis
    src/main.cpp
    src/Window.cpp
    src/Game.cpp
    src/Tower.cpp
    src/HUD.cpp
    src/Map.cpp
    src/Enemy.cpp
    src/EnemySpawner.cpp
    src/Cornucopia.cpp
    src/Soldier.cpp
    src/Bullet.cpp
    src/WaterMine.cpp
    src/AudioManager.cpp
    src/SpriteManager.cpp
    src/Cutscene.cpp
    src/AssetLoader.cpp
)
target_include_directories(Oasis PRIVATE include)
target_compile_features(Oasis PRIVATE cxx_std_17)
target_link_libraries(Oasis PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

add_custom_command(TARGET Oasis POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Oasis>/assets)

if(WIN32)
    add_custom_command(TARGET Oasis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:Oasis>
        $<TARGET_FILE_DIR:Oasis>
        COMMAND_EXPAND_LISTS)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OPENAL_DLL "${sfml_SOURCE_DIR}/extlibs/bin/x64/openal32.dll")
    else()
        set(OPENAL_DLL "${sfml_SOURCE_DIR}/extlibs/bin/x86/openal32.dll")
    endif()

    add_custom_command(TARGET Oasis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OPENAL_DLL}"
        $<TARGET_FILE_DIR:Oasis>)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Oasis)

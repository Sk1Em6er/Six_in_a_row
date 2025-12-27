cmake_minimum_required(VERSION 3.25)
project(SixInARow LANGUAGES C CXX)

# ========== Qt 配置 ==========
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 禁用Qt自动工具（无.ui文件，手动写界面）
set(CMAKE_AUTOMOC ON)    # 仅启用MOC（处理Q_OBJECT）
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)
qt_standard_project_setup()

# ========== 源文件配置 ==========

# 源文件新增menu_widget相关文件
set(SOURCES
    src/main.cpp
    src/ui_widget.cpp
    src/menu_widget.cpp  # 新增
    src/game_logic.c
    src/ai.c
    src/file_operation.c
    src/data_persistence.c
)

set(HEADERS
    include/ui_widget.h
    include/menu_widget.h  # 新增
    include/game_logic.h
    include/ai.h
    include/file_operation.h
    include/data_persistence.h
)

# 纯C文件（游戏逻辑）
set(C_FILES
    src/ai.cpp
    src/game_logic.cpp
    src/file_operation.cpp
    src/data_persistence.cpp
)

# Qt C++文件（界面）
set(CXX_FILES
    src/main.cpp
    src/ui_widget.cpp
)

# 头文件
set(HEAD_FILES
    include/ai.h
    include/game_logic.h
    include/file_operation.h
    include/data_persistence.h
    include/ui_widget.h
)

# 排除C文件的MOC处理
set_source_files_properties(${C_FILES} PROPERTIES SKIP_AUTOMOC ON)

# ========== 编译选项 ==========
add_compile_options(
    -finput-charset=utf-8
    -fexec-charset=utf-8
    -Wall
    -D_CRT_SECURE_NO_WARNINGS
)

# ========== 生成可执行文件 ==========
add_executable(SixInARow
    ${C_FILES}
    ${CXX_FILES}
    ${HEAD_FILES}
    include/menu_widget.h
    src/menu_widget.cpp
    include/history_widget.h
    src/history_widget.cpp
    include/settings_dialog.h
    src/settings_dialog.cpp
    include/audio_manager.h
    src/audio_manager.cpp
    include/PvpSixInARowWidget.h
    src/PvpSixInARowWidget.cpp
)

# ========== 链接库 ==========
target_link_libraries(SixInARow PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
)

# ========== 头文件目录 ==========
target_include_directories(SixInARow PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# ========== 输出路径 ==========
set_target_properties(SixInARow PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/release"
)

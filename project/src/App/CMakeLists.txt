cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(project_name MarkDownTranslator)

project(${project_name})

set(CMAKE_PREFIX_PATH "D:/Qt/5.12.2/msvc2017_64")

find_package(Qt5 REQUIRED Widgets Gui WebEngineWidgets)

set(target_name markdowntranslator)

FILE (GLOB ALL_SOURCES "*.cpp")
FILE (GLOB ALL_INCLUDES "*.h")
FILE (GLOB ALL_UI "*.ui")

SET (ALL_SRCS 
	 ${ALL_SOURCES}
	 ${ALL_INCLUDES}
	 ${ALL_UI}
)

add_executable(${target_name}  ${ALL_SRCS})
target_link_libraries(${target_name} Qt5::Widgets Qt5::Gui Qt5::WebEngineWidgets)

#设置目标关联的*.h, *.cpp 使用 Qt moc进行编译 
set_target_properties(${target_name} PROPERTIES AUTOMOC ON)
#设置目标关联的*.ui 使用 Qt uic进行编译
set_target_properties(${target_name} PROPERTIES AUTOUIC ON)
#打开全局rcc，本示例中没有使用qrc，此句可以去掉
#set(CMAKE_AUTORCC ON)
#设置目标关联的*.qrc 使用 Qt rcc进行编译
set_target_properties(${target_name} PROPERTIES AUTORCC ON)
#跳过不需要使用moc编译的文件。如果觉得麻烦此句可以不写，automoc能根据*.h,*.cpp代码里面的宏(Q_OBJECT;Q_GADGET;Q_NAMESPACE)自动判断是否需要使用moc
set_source_files_properties(main.cpp PROPERTIES SKIP_AUTOMOC ON)

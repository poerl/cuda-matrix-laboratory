add_library(gui STATIC
    include/MainView.h
    include/MatrixWidget.h
    include/InputWidget.h
    include/size_selector.h
    include/performance_chart.h

    src/MainView.cpp
    src/InputWidget.cpp
    src/size_selector.cpp
    src/performance_chart.cpp
    src/MatrixWidget.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

target_include_directories(gui PUBLIC include)

target_link_libraries(gui PUBLIC
    api
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
)
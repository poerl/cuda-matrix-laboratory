add_library(presenter STATIC
    include/Presenter.h

    src/Presenter.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Concurrent)

target_include_directories(presenter PUBLIC include)

target_link_libraries(presenter PUBLIC
    api
    Qt6::Core
    Qt6::Concurrent
)
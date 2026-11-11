add_library(api STATIC
    include/dtos.h
    include/IMainView.h
    include/IModel.h
    include/IArray2D.h
    include/IFileWorker.h
)

target_include_directories(api PUBLIC include)

target_link_libraries(api PUBLIC
    Qt6::Core
)

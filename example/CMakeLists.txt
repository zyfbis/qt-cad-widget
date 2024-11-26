find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(example
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(example PRIVATE
    ViewerLib
    Qt6::Core
    Qt6::Widgets
)

if(MSVC)
    add_definitions(/MP)
    set_target_properties(example PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_MSVC_PATH}/bin;%PATH%"
    )
endif()

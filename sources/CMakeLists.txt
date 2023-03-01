cmake_minimum_required(VERSION 3.20.0)
project(spaceTravelLib CXX)

macro (collect var pattern)
    file(GLOB files ${pattern})
    foreach(file ${files})
        set(${var} ${${var}} ${file})
    endforeach()
endmacro()

collect(HEADERS "*.h")
collect(SOURCES "*.cpp")

add_library(spaceTravelLib OBJECT
    ${HEADERS}
    ${SOURCES}
)


find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

message("GLEW INCLUDE" ${GLEW_INCLUDE_DIRS})
message("GLEW LIBRARIES" ${GLEW_LIBRARIES})



target_include_directories(spaceTravelLib PUBLIC ${GLEW_INCLUDE_DIRS}
                                          PUBLIC ${GLUT_INCLUDE_DIRS}
                                          PUBLIC ${OPENGL_INCLUDE_DIRS}
                                          PUBLIC .
                                          )
target_link_libraries(spaceTravelLib ${GLEW_LIBRARIES} ${GLUT_LIBRARY} ${OPENGL_LIBRARIES})
set_target_properties(spaceTravelLib PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -pedantic -Wuninitialized -fPIC")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src exBaseSrc)
set(libexception ${PROJECT_NAME}_exception "PARENT_SCOPE")
add_library(${PROJECT_NAME}_exception ${exBaseSrc} )



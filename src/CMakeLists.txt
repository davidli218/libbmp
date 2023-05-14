aux_source_directory(. DIR_LIB_SRCS)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(libbmp ${DIR_LIB_SRCS})

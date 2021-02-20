include_directories(include)
aux_source_directory(src SRC)

add_library(
    NEAT ${SRC}
)

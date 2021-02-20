cmake_minimum_required(VERSION 3.13.4)

project(main)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
add_subdirectory(neat)
add_subdirectory(flappy_bird)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC neat flappy_bird)
target_link_libraries(${PROJECT_NAME} NEAT FLAPPY_BIRD)


# target_compile_options (${PROJECT_NAME} PRIVATE -lSDL2_ttf)
# target_compile_options(${PROJECT_NAME} PRIVATE -lSDL2_image)
target_link_libraries(${PROJECT_NAME} SDL2 SDL2_image)
# target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} -I/usr/include/freetype2)

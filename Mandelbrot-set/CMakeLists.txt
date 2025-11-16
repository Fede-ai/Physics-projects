set(SOURCE
	main.cpp
)

add_executable(Mandelbrot-set ${SOURCE})

if (MSVC)
    target_compile_options(Mandelbrot-set PRIVATE /openmp:llvm)
endif()

target_include_directories(Mandelbrot-set PRIVATE ${PATH_SFML}/include)
target_link_directories(Mandelbrot-set PRIVATE ${PATH_SFML}/lib)
target_link_libraries(Mandelbrot-set PRIVATE
	$<$<CONFIG:Release>:sfml-system.lib sfml-graphics.lib sfml-window.lib>
	$<$<CONFIG:Debug>:sfml-system-d.lib sfml-graphics-d.lib sfml-window-d.lib>
)

# copy necessary dll files
add_custom_command(TARGET Mandelbrot-set POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different        	
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-system-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-system-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-graphics-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-graphics-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-window-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-window-d-3.dll>
	$<TARGET_FILE_DIR:Mandelbrot-set>
)
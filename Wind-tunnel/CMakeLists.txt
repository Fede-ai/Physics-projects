set(SOURCE
	main.cpp
	lbm.hpp
	lbm.cpp
	foil.hpp
	foil.cpp
)

add_executable(Wind-tunnel ${SOURCE})

if (MSVC)
    target_compile_options(Wind-tunnel PRIVATE
        /O2
        /openmp:llvm
        /arch:AVX2
    )
else()
    target_compile_options(Wind-tunnel PRIVATE
        -O3
        -fopenmp
        -march=native
    )
endif()

target_include_directories(Wind-tunnel PRIVATE ${PATH_SFML}/include)
target_link_directories(Wind-tunnel PRIVATE ${PATH_SFML}/lib)
target_link_libraries(Wind-tunnel PRIVATE
	$<$<CONFIG:Release>:sfml-system.lib sfml-graphics.lib sfml-window.lib>
	$<$<CONFIG:Debug>:sfml-system-d.lib sfml-graphics-d.lib sfml-window-d.lib>
)

# copy necessary dll files
add_custom_command(TARGET Wind-tunnel POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different        	
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-system-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-system-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-graphics-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-graphics-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-window-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-window-d-3.dll>
	$<TARGET_FILE_DIR:Wind-tunnel>
)
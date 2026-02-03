set(SOURCE
	main.cpp
	engine.hpp engine.cpp
	gravity.hpp gravity.cpp
)

add_executable(Gravitational-potential ${SOURCE})

target_include_directories(Gravitational-potential PRIVATE ${PATH_SFML}/include)
target_link_directories(Gravitational-potential PRIVATE ${PATH_SFML}/lib)
target_link_libraries(Gravitational-potential PRIVATE
	$<$<CONFIG:Release>:sfml-system.lib sfml-graphics.lib sfml-window.lib>
	$<$<CONFIG:Debug>:sfml-system-d.lib sfml-graphics-d.lib sfml-window-d.lib>
)

# copy necessary dll files
add_custom_command(TARGET Gravitational-potential POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different        	
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-system-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-system-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-graphics-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-graphics-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-window-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-window-d-3.dll>
	$<TARGET_FILE_DIR:Gravitational-potential>
)

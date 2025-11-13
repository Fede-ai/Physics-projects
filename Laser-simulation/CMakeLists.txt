set(SOURCE
	simulation.h 
	simulation.cpp main.cpp
	laser.h
	laser.cpp
	circle.h
	circle.cpp
	block.h 
	block.cpp
)

add_executable(Laser-simulation ${SOURCE})

target_include_directories(Laser-simulation PRIVATE ${PATH_SFML}/include)
target_link_directories(Laser-simulation PRIVATE ${PATH_SFML}/lib)
target_link_libraries(Laser-simulation PRIVATE
	$<$<CONFIG:Release>:sfml-system.lib sfml-graphics.lib sfml-window.lib>
	$<$<CONFIG:Debug>:sfml-system-d.lib sfml-graphics-d.lib sfml-window-d.lib>
)

# copy necessary dll files
add_custom_command(TARGET Laser-simulation POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different        	
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-system-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-system-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-graphics-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-graphics-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-window-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-window-d-3.dll>
	$<TARGET_FILE_DIR:Laser-simulation>
)
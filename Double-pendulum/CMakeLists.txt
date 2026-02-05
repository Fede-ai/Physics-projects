set(SOURCE
	main.cpp
	pendulum.cpp
	pendulum.hpp
	glow.frag
)

add_executable(Double-pendulum ${SOURCE})

target_include_directories(Double-pendulum PRIVATE ${PATH_SFML}/include)
target_link_directories(Double-pendulum PRIVATE ${PATH_SFML}/lib)
target_link_libraries(Double-pendulum PRIVATE
	$<$<CONFIG:Release>:sfml-system.lib sfml-graphics.lib sfml-window.lib>
	$<$<CONFIG:Debug>:sfml-system-d.lib sfml-graphics-d.lib sfml-window-d.lib>
)

# copy necessary dll files
add_custom_command(TARGET Double-pendulum POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different        	
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-system-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-system-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-graphics-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-graphics-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-window-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-window-d-3.dll>
	${CMAKE_CURRENT_SOURCE_DIR}/glow.frag
	$<TARGET_FILE_DIR:Double-pendulum>
)

add_custom_command(TARGET Double-pendulum POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different        	
	"${CMAKE_CURRENT_SOURCE_DIR}/glow.frag"
	"$<TARGET_FILE_DIR:Double-pendulum>/../glow.frag"
)
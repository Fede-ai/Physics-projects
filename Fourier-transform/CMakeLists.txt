set(SOURCE
	main.cpp
	transform.hpp
	transform.cpp
	bg.frag
)

add_executable(Fourier-transform ${SOURCE})

target_include_directories(Fourier-transform PRIVATE ${PATH_SFML}/include)
target_link_directories(Fourier-transform PRIVATE ${PATH_SFML}/lib)
target_link_libraries(Fourier-transform PRIVATE
	$<$<CONFIG:Release>:sfml-system.lib sfml-graphics.lib sfml-window.lib>
	$<$<CONFIG:Debug>:sfml-system-d.lib sfml-graphics-d.lib sfml-window-d.lib>
)

# copy necessary dll files
add_custom_command(TARGET Fourier-transform POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different        	
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-system-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-system-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-graphics-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-graphics-d-3.dll>
	$<$<CONFIG:Release>:${PATH_SFML}/bin/sfml-window-3.dll>
	$<$<CONFIG:Debug>:${PATH_SFML}/bin/sfml-window-d-3.dll>
	${CMAKE_CURRENT_SOURCE_DIR}/bg.frag
	$<TARGET_FILE_DIR:Fourier-transform>
)

add_custom_command(TARGET Fourier-transform POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different        	
	"${CMAKE_CURRENT_SOURCE_DIR}/bg.frag"
	"$<TARGET_FILE_DIR:Fourier-transform>/../bg.frag"
)
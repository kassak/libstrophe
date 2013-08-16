cmake_minimum_required (VERSION 2.6)
project (libstrophe-examples)

add_executable (bot bot.c)
add_executable (active active.c)
add_executable (basic basic.c)
add_executable (roster roster.c)

target_link_libraries(bot strophe)
target_link_libraries(active strophe)
target_link_libraries(basic strophe)
target_link_libraries(roster strophe)

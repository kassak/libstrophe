cmake_minimum_required (VERSION 2.6)
project (libstrophe-examples)

link_directories (../)

add_executable (bot bot.c)
add_executable (active active.c)
add_executable (basic basic.c)
add_executable (roster roster.c)

target_link_libraries(bot libstrophe.so)
target_link_libraries(active libstrophe.so)
target_link_libraries(basic libstrophe.so)
target_link_libraries(roster libstrophe.so)

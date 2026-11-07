idf_component_register(SRCS "main.c" "starting_screen.c" "game.c" "draw.c" "player.c" "map.c" "../map/map_list.c" "meanie.c"
                       INCLUDE_DIRS "." "../map"
                       PRIV_REQUIRES esp_timer config lcd pin joy)
# PRIV_REQUIRES ...
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

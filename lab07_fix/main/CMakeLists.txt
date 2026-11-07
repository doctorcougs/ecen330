# New .c files in these folders are picked up on the next cmake run
# (idf.py reconfigure, or delete build/ ) since GLOB is evaluated at configure time.
file(GLOB MAIN_SRCS   "*.c")
file(GLOB MAP_SRCS    "../map/*.c")
file(GLOB SPRITE_SRCS "../sprites/*.c")

idf_component_register(SRCS ${MAIN_SRCS} ${MAP_SRCS} ${SPRITE_SRCS}
                       INCLUDE_DIRS "." "../map" "../sprites"
                       PRIV_REQUIRES esp_timer config lcd pin joy)
# PRIV_REQUIRES ...
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

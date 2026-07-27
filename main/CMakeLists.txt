file(GLOB_RECURSE SRC_FILES
    "*.c"
)

idf_component_register(
    SRCS ${SRC_FILES}
    INCLUDE_DIRS
        "."
        "ui"
        "ui/screens"
        "ui/components"
        "ui/fonts"
        "ui/images"
    REQUIRES
        lvgl
        bt
        nvs_flash
        littlefs 
        bt
        
)

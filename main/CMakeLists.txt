idf_component_register(
    SRCS 
        "main.c"
        "pcf8574.c"
        "ui/ui.c"
        "ui/ui_helpers.c"
        "ui/screens/ui_Screen1.c"
        "ui/components/ui_comp_hook.c"
    INCLUDE_DIRS 
        "."
        "ui"
        "ui/screens"
        "ui/components"
    REQUIRES lvgl)
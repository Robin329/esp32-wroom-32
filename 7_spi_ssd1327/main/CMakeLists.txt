set(srcs "display_ssd1327_main.c"
         "display_ssd1327.c"
         "gui/gui_paint.c"
         "fonts/font8.c"
         "fonts/font12.c"
         "fonts/font12CN.c"
         "fonts/font16.c"
         "fonts/font20.c"
         "fonts/font24.c"
         "fonts/font24CN.c"
         "fonts/font24.c"
        )

idf_component_register(SRCS ${srcs}
    INCLUDE_DIRS "." "../../common/" "fonts/" "gui/"
    REQUIRES unity driver i2c_tools console lettershell
    WHOLE_ARCHIVE)

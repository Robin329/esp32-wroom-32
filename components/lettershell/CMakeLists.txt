idf_build_get_property(target IDF_TARGET)
set(srcs "./src/shell.c"
         "./src/shell_cmd_list.c"
         "./src/shell_companion.c"
         "./src/shell_ext.c"
         "shell_port.c"
)

idf_component_register(SRCS ${srcs}
    INCLUDE_DIRS "." "../../common/" "./" "./src"
    LDFRAGMENTS "shell.lf"
    REQUIRES driver)
    
target_compile_options(${COMPONENT_LIB} PRIVATE -DSHELL_CFG_USER="shell_cfg_user.h")
idf_build_set_property(COMPILE_OPTIONS "-DSHELL_CFG_USER=\"shell_cfg_user.h\"" APPEND)
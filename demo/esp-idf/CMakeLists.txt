idf_component_register(
    SRCS "src/shell.c"
         "src/shell_ext.c"
         "shell_port.c"

    INCLUDE_DIRS "./"
                 "./src"

    REQUIRES
        driver

    LDFRAGMENTS "shell.lf"
)

target_compile_options(${COMPONENT_LIB} PRIVATE -DSHELL_CFG_USER="shell_cfg_user.h")

idf_component_register(SRCS "cmd_i2ctools.c"
                            "i2ctools.c"
                       INCLUDE_DIRS "." ""
                       REQUIRES driver console fatfs vfs spi_flash cmd_system
                       )

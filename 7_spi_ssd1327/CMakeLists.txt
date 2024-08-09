# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    "$ENV{IDF_PATH}/examples/system/console/advanced/components"
    "/data/workspace/esp32/esp32-wroom-32/components/i2c_tools"
    "/data/workspace/esp32/esp32-wroom-32/components/lettershell"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(display_ssd1327)

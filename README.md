- [1. esp32-wroom-32 devkit v1](#1-esp32-wroom-32-devkit-v1)
  - [1.1. examples supports](#11-examples-supports)
  - [1.2. Hardware doc](#12-hardware-doc)
- [2. Developing With ESP-IDF](#2-developing-with-esp-idf)
  - [2.1. Setting Up ESP-IDF](#21-setting-up-esp-idf)
    - [2.1.1. Non-GitHub forks](#211-non-github-forks)
  - [2.2. Finding a Project](#22-finding-a-project)
- [3. Quick Reference](#3-quick-reference)
  - [3.1. Setup Build Environment](#31-setup-build-environment)
  - [3.2. Configuring the Project](#32-configuring-the-project)
  - [3.3. Compiling the Project](#33-compiling-the-project)
  - [3.4. Flashing the Project](#34-flashing-the-project)
  - [3.5. Viewing Serial Output](#35-viewing-serial-output)
  - [3.6. Compiling \& Flashing Only the App](#36-compiling--flashing-only-the-app)
  - [3.7. Erasing Flash](#37-erasing-flash)
- [4. Resources](#4-resources)


# 1. esp32-wroom-32 devkit v1
Some example examples of esp32-wroom-32 are based on FreeRTOS.

## 1.1. examples supports
|name                 |          support             |                             position                                    |
|:--------------------| :---------------------------:| :-------------------------------------------------------------- |
|1.hello_world        | ![alt text][supported]       |     [1_hello_world](./1_hello_world/main/hello_world_main.c)    |
|2.blink_led          | ![alt text][supported]     |       [2_blink_led](./2_blink_led/main/blink_example_main.c)   |
|3.i2c_tools          | ![alt text][supported]     |       [3_i2c_tools](./3_i2c_tools/main/cmd_i2ctools.c)   |
|4.i2c_at24cxx          | ![alt text][not support]     |     [4_i2c_at24cxx](./4_i2c_at24cxx/main/at24cxx_main.c)   |
|5.i2c_at24c          | ![alt text][supported]     |       [5_i2c_at24c](./5_i2c_at24c/main/main.c)   |

[supported]: https://img.shields.io/badge/-%E6%94%AF%E6%8C%81-green "supported"
[preview]: https://img.shields.io/badge/-%E9%A2%84%E8%A7%88-orange "preview"
[not support]: https://img.shields.io/badge/not%20supported-orange "not support"

## 1.2. Hardware doc
[esp32-wroom-32-devkit-v1](https://github.com/Robin329/esp32-doc.git)


# 2. Developing With ESP-IDF

## 2.1. Setting Up ESP-IDF

See https://idf.espressif.com/ for links to detailed instructions on how to set up the ESP-IDF depending on chip you use.

**Note:** Each SoC series and each ESP-IDF release has its own documentation. Please see Section [Versions](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/versions.html) on how to find documentation and how to checkout specific release of ESP-IDF.

### 2.1.1. Non-GitHub forks

ESP-IDF uses relative locations as its submodules URLs ([.gitmodules](.gitmodules)). So they link to GitHub. If ESP-IDF is forked to a Git repository which is not on GitHub, you will need to run the script [tools/set-submodules-to-github.sh](tools/set-submodules-to-github.sh) after git clone.

The script sets absolute URLs for all submodules, allowing `git submodule update --init --recursive` to complete. If cloning ESP-IDF from GitHub, this step is not needed.

## 2.2. Finding a Project

As well as the [esp-idf-template](https://github.com/espressif/esp-idf-template) project mentioned in Getting Started, ESP-IDF comes with some example projects in the [examples](examples) directory.

Once you've found the project you want to work with, change to its directory and you can configure and build it.

To start your own project based on an example, copy the example project directory outside of the ESP-IDF directory.

# 3. Quick Reference

See the Getting Started guide links above for a detailed setup guide. This is a quick reference for common commands when working with ESP-IDF projects:

## 3.1. Setup Build Environment

(See the Getting Started guide listed above for a full list of required steps with more details.)

* Install host build dependencies mentioned in the Getting Started guide.
* Run the install script to set up the build environment. The options include `install.bat` or `install.ps1` for Windows, and `install.sh` or `install.fish` for Unix shells.
* Run the export script on Windows (`export.bat`) or source it on Unix (`source export.sh`) in every shell environment before using ESP-IDF.

## 3.2. Configuring the Project

* `idf.py set-target <chip_name>` sets the target of the project to `<chip_name>`. Run `idf.py set-target` without any arguments to see a list of supported targets.
* `idf.py menuconfig` opens a text-based configuration menu where you can configure the project.

## 3.3. Compiling the Project

`idf.py build`

... will compile app, bootloader and generate a partition table based on the config.

## 3.4. Flashing the Project

When the build finishes, it will print a command line to use esptool.py to flash the chip. However you can also do this automatically by running:

`idf.py -p PORT flash`

Replace PORT with the name of your serial port (like `COM3` on Windows, `/dev/ttyUSB0` on Linux, or `/dev/cu.usbserial-X` on MacOS. If the `-p` option is left out, `idf.py flash` will try to flash the first available serial port.

This will flash the entire project (app, bootloader and partition table) to a new chip. The settings for serial port flashing can be configured with `idf.py menuconfig`.

You don't need to run `idf.py build` before running `idf.py flash`, `idf.py flash` will automatically rebuild anything which needs it.

## 3.5. Viewing Serial Output

The `idf.py monitor` target uses the [esp-idf-monitor tool](https://github.com/espressif/esp-idf-monitor) to display serial output from Espressif SoCs. esp-idf-monitor also has a range of features to decode crash output and interact with the device. [Check the documentation page for details](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html).

Exit the monitor by typing Ctrl-].

To build, flash and monitor output in one pass, you can run:

`idf.py flash monitor`

## 3.6. Compiling & Flashing Only the App

After the initial flash, you may just want to build and flash just your app, not the bootloader and partition table:

* `idf.py app` - build just the app.
* `idf.py app-flash` - flash just the app.

`idf.py app-flash` will automatically rebuild the app if any source files have changed.

(In normal development there's no downside to reflashing the bootloader and partition table each time, if they haven't changed.)

## 3.7. Erasing Flash

The `idf.py flash` target does not erase the entire flash contents. However it is sometimes useful to set the device back to a totally erased state, particularly when making partition table changes or OTA app updates. To erase the entire flash, run `idf.py erase-flash`.

This can be combined with other targets, ie `idf.py -p PORT erase-flash flash` will erase everything and then re-flash the new app, bootloader and partition table.

# 4. Resources

* Documentation for the latest version: https://docs.espressif.com/projects/esp-idf/. This documentation is built from the [docs directory](docs) of this repository.

* The [esp32.com forum](https://esp32.com/) is a place to ask questions and find community resources.

* [Check the Issues section on github](https://github.com/espressif/esp-idf/issues) if you find a bug or have a feature request. Please check existing Issues before opening a new one.

* If you're interested in contributing to ESP-IDF, please check the [Contributions Guide](https://docs.espressif.com/projects/esp-idf/en/latest/contribute/index.html).

#!/bin/bash
export LC_ALL=C.UTF-8
get_idf
idf.py build
cp build/compile_commands.json /data/workspace/esp32/
idf.py flash monitor

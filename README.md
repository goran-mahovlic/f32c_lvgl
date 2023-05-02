# f32c_lvgl

Flash f32c bitstream

MAKEFILES=PATH_TO_F32C/f32c/src/conf/f32c.mk make clean

MAKEFILES=PATH_TO_F32C/f32c/src/conf/f32c.mk make

python3 f32cup.py hello.bin

sudo screen /dev/ttyUSB0 115200
# F32C LVGL experiment

Flash f32c bitstream to your FPGA board

### Clone this repo with lvgl v8.3.0 as submodule 

git clone --recurse-submodules https://github.com/goran-mahovlic/f32c_lvgl.git

### Fixing one line log bug in lvgl v 8.3.0

cp lv_draw_sw_letter.c lvgl/src/draw/sw/lv_draw_sw_letter.c

### You will need f32c with all libraries build

MAKEFILES=PATH_TO_F32C/f32c/src/conf/f32c.mk make clean

MAKEFILES=PATH_TO_F32C/f32c/src/conf/f32c.mk make

### Load bin file to f32c

python3 f32cup.py hello.bin

### Check serial output 

sudo screen /dev/ttyUSB0 115200
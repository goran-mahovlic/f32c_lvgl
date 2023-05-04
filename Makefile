PROG = hello

LVGL_PATH ?= ${shell pwd}/lvgl
C2_PATH ?= ${shell pwd}/Compositing
#LVGL_PATH ?= ${shell pwd}
CFLAGS += "-I${shell pwd}"
CFLAGS += "-I$(LVGL_PATH)"
CFLAGS += "-std=c99"
CFLAGS += "-Wno-error"
#CFLAGS += -DLV_LVGL_H_INCLUDE_SIMPLE
#CFLAGS += "-fno-builtin-ffs"
#CFLAGS += "-G128000"

CXXFLAGS += "-I$(C2_PATH)"

CFILES += $(shell find $(LVGL_PATH)/src -type f -name '*.c')
#CFILES += $(shell find $(LVGL_PATH)/demos -type f -name '*.c')
#CFILES += $(shell find $(LVGL_PATH)/examples -type f -name '*.c')

CXXFILES += hello_lvgl.cpp

CXXFILES += $(shell find $(C2_PATH) -type f -name '*.cpp')

LIBS += -lulx2s
#LIBS += -lm

include ${POST_MK}

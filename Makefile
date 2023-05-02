PROG = hello

LVGL_PATH ?= ${shell pwd}/lvgl
#LVGL_PATH ?= ${shell pwd}

CFLAGS += "-I$(LVGL_PATH)"
CFLAGS += "-std=c99"
CFLAGS += "-Wno-error"
CFLAGS += -DLV_LVGL_H_INCLUDE_SIMPLE
#CFLAGS += "-fno-builtin-ffs"
#CFLAGS += "-G128000"

CFILES += $(shell find $(LVGL_PATH)/src -type f -name '*.c')
CFILES += $(shell find $(LVGL_PATH)/demos -type f -name '*.c')
CFILES += $(shell find $(LVGL_PATH)/examples -type f -name '*.c')

CFILES += hello_lvgl.c

LIBS += -lulx2s
#LIBS += -lm

include ${POST_MK}

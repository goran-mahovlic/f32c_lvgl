/*
 * LVGL test
 */

#include "hello_lvgl.h"
#include <demos/lv_demos.h>
//#include <examples/lv_examples.h>

#define F_CPU 100000000
#define NODEMO

volatile static lv_disp_draw_buf_t draw_buf;

volatile static lv_color_t *buf1 = NULL;
volatile static lv_color_t *buf2 = NULL;

static uint32_t tsc_hi, tsc_lo;
int tmp, freq_khz;

#ifdef __mips__
static const char *arch = "mips";
#elif defined(__riscv)
static const char *arch = "riscv";
#else
static const char *arch = "unknown";
#endif

void
delay(uint32_t ms)
{
    int32_t t, target;
    RDTSC(target);
    while (ms--)
    {
        target += F_CPU/1000;
        do
            RDTSC(t);
        while((target-t) > 0);
    }
}

static void
update_tsc(void)
{
    uint32_t tsc;

    RDTSC(tsc);

    /* check for 32-bit overflow */
    if (tsc < tsc_lo)
        tsc_hi++;
    tsc_lo = tsc;
}


uint32_t
millis(void)
{
    uint64_t tsc64;

    update_tsc();
    tsc64 = tsc_hi;
    tsc64 <<= 32;
    tsc64 += tsc_lo;
    
    return((tsc64 * 1000) / F_CPU);
}


void disp_driver_rounder(lv_disp_drv_t * disp_drv, lv_area_t * area)
{
  /* Update the areas as needed. Can be only larger.
   * For example to always have lines 8 px height:*/
   area->y1 = area->y1 & 0x07;
   area->y2 = (area->y2 & 0x07) + 8;
}

/*
void disp_driver_rounder(lv_disp_drv_t * disp_drv, lv_area_t * area)
{
    uint8_t hor_max = disp_drv->hor_res;
    uint8_t ver_max = disp_drv->ver_res;

    area->x1 = 0;
    area->y1 = 0;
    area->x2 = hor_max - 1;
    area->y2 = ver_max - 1;
}
*/

void disp_driver_set_px(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
    lv_color_t * color_p, lv_opa_t opa) 
{
    fb_plot(x, y, (int)color_p);
}

void disp_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    int32_t x, y;
    //int color = (int)color_p;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            fb_plot(x, y, (int)color_p);
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

void
main(void)
{

    printf("Hello, f32c/%s world!\n", arch);

    fb_set_mode(0);
    // Test f32c text to framebuffer
    fb_text(100, 100, "No LVGL FB test", 0x0000, 0xffff, 3);
    delay(2000);

    lv_init();

    buf1 = malloc(512 * 288 * 16);
    buf2 = malloc(512 * 288 * 16);

    lv_log_register_print_cb( printf ); /* register print function for debugging */

    /* static lv_color_t * */

    lv_disp_draw_buf_init( &draw_buf, buf1, buf2, 512 * 288 * 16);
    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;

    lv_disp_drv_init( &disp_drv );

    disp_drv.flush_cb = disp_flush_cb;

    /*Set the resolution of the display*/
    disp_drv.hor_res = 512;
    disp_drv.ver_res = 288;

//    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb  = disp_driver_set_px;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = true;

    lv_disp_drv_register( &disp_drv );

#ifdef NODEMO
    /*Change the active screen's background color*/
    delay(1000);
    printf("Setting background color\n");
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xffff), LV_PART_MAIN);
    delay(1000);
    /*Create a white label, set its text and align it to the center*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    delay(1000);
    printf("Creating label\n");    
    lv_label_set_text(label, "Hello world");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0x0000), LV_PART_MAIN);
    delay(1000);
    printf("Align label\n");    
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
#else
    delay(1000);
    printf("Setting up demo widgets\n");    
    lv_demo_widgets();
    delay(1000);
    printf("Widgets setup done\n");
#endif

    while(1){
        delay(500);
        printf("In the loop\n");
        printf("Millis: %i\n", millis());
        lv_timer_handler(); /* let the GUI do its work */
    }
}
/*
 * LVGL test
 */

extern "C" {
#include <math.h>
#include <stdlib.h>
#include "hello_lvgl.h"
#include <demos/lv_demos.h>
//#include <examples/lv_examples.h>
}

#include <Compositing.h>
#define SPRITE_MAX 10
Compositing c2;

//Define Visuals
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define ANZCOL 256
#define emu_set_pix(x,y,c) bitmap[x + SCREEN_WIDTH*y] = color_map[c]

// crude malloc()
pixel_t *bitmap = (pixel_t *)0x80080000;
pixel_t color_map[ANZCOL];

#define F_CPU 100000000
#define NODEMO
//#define SPINNER

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1 = (lv_color_t *)0x80080000;

#define F32C_VIDEOWIDTH SCREEN_WIDTH
#define F32C_VIDEOHEIGHT SCREEN_HEIGHT
//#define BLACK 0x0000

// uncomment if you are using LOG
//#define DEBUG

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

    // check for 32-bit overflow
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
}

void disp_driver_set_px(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
    lv_color_t color_p, lv_opa_t opa) 
{

  if ((x < 0) || (x >= F32C_VIDEOWIDTH) || (y < 0) || (y >= F32C_VIDEOHEIGHT))
    return;
    emu_set_pix(x,y,(int)&color_p); // Needs to be removed
}

void disp_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    int32_t x, y;

  // set color map
  //for(int i = 0; i < ANZCOL; i++)
  //  color_map[i] = RGB2PIXEL((int)&color_p);

  for(int i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; i++) {
        //bitmap[i] = 0x00;  // works - sets screen to black
        //emu_set_pix(x,y,(int)&color_p);
        emu_set_pix(x,y,uint16_t(RGB2PIXEL((int)&color_p)));
        color_p++;
    }
   // c2.sprite_refresh();
    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

void
main(void)
{

  delay(1000);
  printf("Hello, f32c/%s world!\n", arch);

  buf1 = (lv_color_t *) malloc(F32C_VIDEOWIDTH*F32C_VIDEOHEIGHT*sizeof(lv_color_t));
  bitmap = (pixel_t *) buf1; // alloc memory for video
  c2.init();
  c2.alloc_sprites(SPRITE_MAX);
  //bitmap = (pixel_t *) malloc(SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(pixel_t)); // alloc memory for video
  c2.sprite_from_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT, bitmap); // create c2 sprite
//  c2.sprite_refresh(); // show it on screen
  *c2.cntrl_reg = 0b11000000; // vgatextmode: enable video, yes bitmap, no text mode, no cursor
   c2.sprite_refresh();

  for(int i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(lv_color_t); i++) {
        bitmap[i] = 0x00;  // set screen to white
    }
  //delay(2000);

    lv_init();

    #ifdef DEBUG
        lv_log_register_print_cb( printf ); /* register print function for debugging */
    #endif

    /* static lv_color_t * */

    lv_disp_draw_buf_init( &draw_buf, buf1, NULL, F32C_VIDEOWIDTH * F32C_VIDEOHEIGHT * sizeof(lv_color_t));
    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;

    lv_disp_drv_init( &disp_drv );

    disp_drv.flush_cb = disp_flush_cb;

    /*Set the resolution of the display*/
    disp_drv.hor_res = F32C_VIDEOWIDTH;
    disp_drv.ver_res = F32C_VIDEOHEIGHT;

//    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb  = disp_driver_set_px;
    disp_drv.draw_buf = &draw_buf;
    //disp_drv.full_refresh = true;

    lv_disp_drv_register( &disp_drv );

#ifdef NODEMO
    /*Change the active screen's background color*/
    //delay(200);
    printf("Setting background color\n");
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x00), LV_PART_MAIN);
    //delay(200);
    /*Create a white label, set its text and align it to the center*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    //delay(200);
    printf("Creating label\n");    
    lv_label_set_text(label, "LVGL LABEL TEXT >> disp_flush_cb needs to be fixed");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffff), LV_PART_MAIN);
    //delay(200);
    printf("Align label\n");
    lv_obj_set_x(label, 100) ;
    lv_obj_set_y(label, 100) ;
    #ifdef SPINNER
        printf("Create spinner\n");    
        /*Create a spinner*/
        lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
        lv_obj_set_size(spinner, 100, 100);
        printf("Center spinner\n");    
        lv_obj_center(spinner);
        printf("Set spinner X Y \n");      
        lv_obj_set_x(spinner, 200) ;
        lv_obj_set_y(spinner, 200) ;
        printf("Init done!\n"); 
    #endif
    //lv_obj_align(label, LV_ALIGN_CENTER, 0, 100);
#else
    delay(200);
    printf("Setting up demo widgets\n");    
    lv_demo_widgets();
    delay(200);
    printf("Widgets setup done\n");
#endif
    while(1){
        delay(100);
        //printf("In the loop\n");
        //printf("Millis: %i\n", millis());
        lv_timer_handler(); /* let the GUI do its work */
    }
}

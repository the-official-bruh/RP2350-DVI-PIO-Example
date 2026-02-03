#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "hardware/timer.h"
#include "hardware/dma.h"
#include <math.h>
#include <string.h>
#include "pico/rand.h"
#include "hardware/uart.h"
#include "pico/multicore.h"

#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"
#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"
#include "tmds_encode.h" 
#include <stdarg.h>
#include "GUI_Paint.h"

#include "fonts/b_dseg7_classic_b_italic_72_font.h"

/*
#define UART_ID uart0
#define BAUD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE
#define UART_TX_PIN 16
#define UART_RX_PIN 17

static volatile bool charReady = false;
*/

// DVDD 1.2V (1.1V seems ok too)
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define VREG_VSEL VREG_VOLTAGE_1_10
#define DVI_TIMING dvi_timing_640x480p_60hz

// RGB111 bitplaned framebuffer
#define COLOR_DEPTH 3
#define PLANE_SIZE_BYTES (FRAME_WIDTH * FRAME_HEIGHT / 8)
#define Scale3_RED 0x4
#define Scale3_GREEN 0x2
#define Scale3_BLUE 0x1
#define Scale3_BLACK 0x0
#define Scale3_WHITE 0x7
uint8_t framebuf[COLOR_DEPTH * PLANE_SIZE_BYTES];

struct dvi_inst dvi0;

void core1_main()
{
	dvi_register_irqs_this_core(&dvi0, DMA_IRQ_0);
	dvi_start(&dvi0);
	while(true)
	{
		for (uint y = 0; y < FRAME_HEIGHT; ++y)
		{
			uint32_t *tmdsbuf=0;
			queue_remove_blocking_u32(&dvi0.q_tmds_free, &tmdsbuf);
			for (uint component = 0; component < 3; ++component)
			{
				tmds_encode_1bpp(
					(const uint32_t *)&framebuf[y * FRAME_WIDTH / 8 + component * PLANE_SIZE_BYTES],
					tmdsbuf + component * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD,
					FRAME_WIDTH);
			}
			queue_add_blocking_u32(&dvi0.q_tmds_valid, &tmdsbuf);
		}
	}
}

int main() {

    stdio_init_all();

    sleep_ms(2000);

    printf("Refrence Clock %2d\n", clock_get_hz(clk_ref));
    printf("System Clock %2d\n", clock_get_hz(clk_sys));

    //set_sys_clock_khz(215000, true);

    vreg_set_voltage(VREG_VSEL);
	sleep_ms(10);
	set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);


    printf("Refrence Clock %2d\n", clock_get_hz(clk_ref));
    printf("System Clock %2d\n", clock_get_hz(clk_sys));

    pio_set_gpio_base(DVI_DEFAULT_SERIAL_CONFIG.pio, 0);

	dvi0.timing = &DVI_TIMING;
	dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
	dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

    multicore_launch_core1(core1_main);

	Paint_NewImage(framebuf, FRAME_WIDTH, FRAME_HEIGHT, 0, Scale3_WHITE);
	Paint_SetScale(3);
	
	Paint_Clear(Scale3_BLACK);
	sleep_ms(500);

    char text[24] = "\0";
    char text2[24] = "\0";

    char lastrect[100] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
    char rect[100] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};

    char *ptext = &text[0];
    char *ptext2 = &text2[0];

    int32_t count = 0;

    int x = 50;
    int y = 50;
    int dirx = 0;
    int diry = 0;

    // This is a DVD logo bounce example, you will need an rgb565 image h/c file
    /*
    while(1){
        Paint_Clear(0x0000);
        Paint_DrawImage(<INSERT IMAGE HERE>, x, y, 320, 240);
        if (dirx == 0){
            x += 4;
        }
        else if (dirx == 1){
            x -= 4;
        }
        
        if (diry == 0){
            y += 4;
        }
        else if (diry == 1){
            y -= 4;
        }

        if (x > 320){
            dirx = 1;
        }
        else if (x <= 0){
            dirx = 0;
        }

        if (y > 240){
            diry = 1;
        }
        else if (y <= 0){
            diry = 0;
        }
        sleep_ms(100);
    }
    */
    
    int8_t dir = 1;
    size_t len;

	while(1){
        //count = 1;//(count + 50) % 8000;
        //Paint_Clear(0x0000);
        snprintf(text, 24, "%d", count);
        //len = strlen(text);
        //snprintf(text2, 24, "%0*d", len);
        /*
        for (int i = 0; i < strlen(text); i++){
            text2[i] = '8';
            text2[i+1] = '\0';
        }
        */

        count = count + (250*dir);

        if (count > 8000){
            dir = -1;
            //count = 0;
        }
        if (count < 0){
            dir = 1;
        }

        for (int i = 99; i >= 0; i--){
            if ((count / 80) > i){
                rect[i] = 1;
            }
            else{
                rect[i] = 0;
            }

            if (lastrect[i] != rect[i]){
                if (i > 80){
                    Paint_DrawRectangle(0 + 6*i, 0, 6 + 6*i, 100, Scale3_RED*rect[i], 1, true);
                }
                else{
                    Paint_DrawRectangle(0 + 6*i, 0, 6 + 6*i, 100, Scale3_GREEN*rect[i], 1, true);
                }   
            }
            lastrect[i] = rect[i];
        }

        //Paint_DrawString_EN(FRAME_WIDTH/2-50, FRAME_HEIGHT/2-10, ptext, &DSEG7, 0xFFFF, 0x0000);
        Paint_DrawString_EN(FRAME_WIDTH/2-250, FRAME_HEIGHT/2-50, ptext, &Font20, Scale3_WHITE, Scale3_BLACK);
        //Draw_String(FRAME_WIDTH/2-50, FRAME_HEIGHT/2-30, ptext2, b_dseg7_classic_b_italic_72_font_lookup, b_dseg7_classic_b_italic_72_font_pixels, Scale3_BLACK, Scale3_BLACK, true);
        Draw_String(FRAME_WIDTH/2-250, FRAME_HEIGHT/2-30, ptext, b_dseg7_classic_b_italic_72_font_lookup, b_dseg7_classic_b_italic_72_font_pixels, Scale3_WHITE, Scale3_BLACK);
        sleep_ms(15);
    }
};
/* 
 *  Target: pico_mezw65c
 *  Written by Akihito Honda (Aki.h @akih_san)
 *  https://x.com/akih_san
 *  https://github.com/akih-san
 *
 * Date: 2026.03.16
; Copyright (c) 2026 Akihito Honda
;
; Released under the MIT license
;
; Permission is hereby granted, free of charge, to any person obtaining a copy of this
; software and associated documentation files (the “Software”), to deal in the Software
; without restriction, including without limitation the rights to use, copy, modify, merge,
; publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
; to whom the Software is furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all copies or
; substantial portions of the Software.
; 
; THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS
; OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
; NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
; BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
; ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
; CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.
*/

#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "pico_w65.h"
#include "iodev.h"

static unsigned char rx_buf[URT_BUF_SIZE];
unsigned int rx_wp, rx_rp, rx_cnt;

static repeating_timer_t timer;

void port_init(void) {
	
	uint i;
	// <set GPIO direction>
	//output
	// W65_CLK  GP26	: pwm
	// W65_DCK  GP21
	// W65_BE   GP20
	// W65_IRQ  GP22	: PIO
	// W65_RW   GP27
	// W65_NMI  GP9
	// W65_RES  GP8
	// DBUS(0 - 7) GP0 - GP7
	//input
	// W65_RDY  GP28
	// IRQ is controled by PIO
	
	// pull up
//	gpio_pull_up(W65_BE);
//	gpio_pull_up(W65_RDY);

	gpio_pull_up(W65_DCK);
	gpio_pull_up(W65_IRQ);
	gpio_pull_up(W65_RW);
	gpio_pull_up(W65_NMI);
	
	gpio_set_dir(FF_CK, GPIO_OUT);
	gpio_set_dir(FF_OE, GPIO_OUT);
	gpio_set_dir(W65_RDY, GPIO_IN);
	gpio_set_dir(W65_RW, GPIO_OUT);
	gpio_set_dir(W65_DCK, GPIO_OUT);
	gpio_set_dir(W65_BE, GPIO_OUT);
	gpio_set_dir(W65_NMI, GPIO_OUT);
	gpio_set_dir(W65_RES, GPIO_OUT);
	gpio_set_dir_masked(W65_DTBS, DBUS_DIR_OUT);

	// set SIO 
	gpio_set_function(FF_CK, GPIO_FUNC_SIO);
	gpio_set_function(FF_OE, GPIO_FUNC_SIO);
	gpio_set_function(W65_DCK, GPIO_FUNC_SIO);
	gpio_set_function(W65_RDY, GPIO_FUNC_SIO);
	gpio_set_function(W65_BE, GPIO_FUNC_SIO);
	gpio_set_function(W65_RW, GPIO_FUNC_SIO);
	gpio_set_function(W65_NMI, GPIO_FUNC_SIO);
	gpio_set_function(W65_RES, GPIO_FUNC_SIO);
	for(i=W65_DBUS; i<W65_DBUS+DBUS_WIDTH; i++) gpio_set_function(i, GPIO_FUNC_SIO);

	// reset CPU
	gpio_put(W65_BE, 0);
	gpio_put(W65_RES, 0);				// RES=0 : CPU reset

	// set default value
	gpio_put(FF_OE, 0);		// active output enable
	gpio_put(FF_CK, 0);
	gpio_put(W65_DCK, 0);
	gpio_put(W65_RW, 1);
	gpio_put(W65_NMI, 1);

	gpio_put_masked(W65_DTBS, 0x00);		// set data bus = 0

	ctlq_ev = 0;

	// initialize address FF
	set_address( 0 );				// initial A0-A15 = 0
}

static int slice_num;

// initial clock 2MHz
void clk_init(void)
{
	gpio_set_function(W65_CLK,GPIO_FUNC_PWM );

	slice_num = pwm_gpio_to_slice_num(W65_CLK);		//get slice number of W65_CLK(GPIO 20 PIN)
	// set division rate
	pwm_set_clkdiv(slice_num, W65_CLK_DIV_2);
	clk_fs = W65_CLK_DIV_2;		// set default value for 2MHz
	// set wrap cout
	pwm_set_wrap(slice_num, WRAP_COUNT);
	// set pwm count
	pwm_set_chan_level(slice_num, PWM_CHAN_A, WRAP_COUNT);
	// start pwm
	pwm_set_enabled(slice_num, true);
	printf("Assert %2.2fMHz for TEST CLOCK\r\n", clock_get_hz(clk_sys)/clk_fs/(WRAP_COUNT+1)/1000000);
}

// Set cpu clock. clk_fs is set from PICO02.CFG/PICO16.CFG setting file
void reset_clk(void)
{
	// stop pwm
	pwm_set_enabled(slice_num, false);

	// set division rate
	pwm_set_clkdiv(slice_num, clk_fs);
	// set wrap cout
	pwm_set_wrap(slice_num, WRAP_COUNT);
	// set pwm count
	pwm_set_chan_level(slice_num, PWM_CHAN_A, WRAP_COUNT);
	// start pwm
	pwm_set_enabled(slice_num, true);
	
}

////////////////////////////////////////////
//
// 5ms intervel timer handler
//
////////////////////////////////////////////
bool timer_callback( repeating_timer_t *rt )
{
	int rx_data;

	// get key
	rx_data =  stdio_getchar_timeout_us (0);
	if (rx_data != PICO_ERROR_TIMEOUT) {
		// check NMI
		if ( ctlq_ev == CTL_Q && rx_data == CTL_Q) {
			ctlq_ev = 0;
			nmi_sig = 1;
			return(true);
		}
		else ctlq_ev = (uint8_t)rx_data;
		if (rx_cnt < URT_BUF_SIZE) {
			rx_buf[rx_wp] = rx_data;
			rx_wp = (rx_wp + 1) & (URT_BUF_SIZE - 1);
			rx_cnt++;
		}
	}
	if (maskTimer) return(true);
	if (!irqMask) irq_flg = 1;
	return (true);
}

// Stop 5ms intervel timer handler
//void stopIntervalTimer(void) {
//	cancel_repeating_timer(&timer);
//	timerAction_off();
//	maskTimer = 1;
//}

// Start 5ms intervel timer handler
void startIntervalTimer(void) {
	maskTimer = 1;
	irq_flg = 0;
	nmi_sig = 0;
	ctlq_ev = 0;
	add_repeating_timer_ms( -5, &timer_callback, NULL, &timer );
}

static uint32_t status;

// UART3 Recive( user side )
int u_getch(void) {
	char c;

	while(!rx_cnt);
	status = save_and_disable_interrupts();
	c = rx_buf[rx_rp];
	rx_rp = (rx_rp + 1) & ( URT_BUF_SIZE - 1);
	rx_cnt--;
	restore_interrupts(status);

	return c;               // Read data
}

unsigned int get_str(char *buf, uint8_t cnt) {
	unsigned int c, i;
	
	i = ( (unsigned int)cnt > rx_cnt ) ? rx_cnt : (unsigned int)cnt;
	c = i;
	status = save_and_disable_interrupts();
	while(i--) {
		*buf++ = rx_buf[rx_rp];
		rx_rp = (rx_rp + 1) & ( URT_BUF_SIZE - 1);
		rx_cnt--;
	}
	restore_interrupts(status);

	return c;
}


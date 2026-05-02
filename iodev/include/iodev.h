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

#ifndef IODEV_DEF
#define IODEV_DEF 1

// define GPIO
#define W65_DBUS (uint)0
#define W65_CLK  (uint)26
#define W65_DCK  (uint)21		//pull up
#define W65_RDY  (uint)28		//pull up
#define W65_BE   (uint)20		//pull down
//#define W65_BE   (uint)8		//pull down
#define W65_IRQ  (uint)22		//pull up
#define W65_RW   (uint)27		//pull up
#define W65_NMI  (uint)9		//pull up
#define W65_RES  (uint)8		//none pulldown (pull down by MEZW65C02_RAM)
//#define W65_RES  (uint)20		//none pulldown (pull down by MEZW65C02_RAM)

#define W65_CLK_MASK  ((uint32_t)1 << W65_CLK)
#define W65_DCK_MASK  ((uint32_t)1 << W65_DCK)
#define W65_RDY_MASK  ((uint32_t)1 << W65_RDY)
#define W65_BE_MASK   ((uint32_t)1 << W65_BE)
#define W65_IRQ_MASK  ((uint32_t)1 << W65_IRQ)
#define W65_RW_MASK   ((uint32_t)1 << W65_RW)
#define W65_NMI_MASK  ((uint32_t)1 << W65_NMI)
#define W65_RES_MASK  ((uint32_t)1 << W65_RES)

#define ADR_WIDTH 16
#define DBUS_WIDTH 8

#define W65_DTBS 0xff

#define DBUS_DIR_OUT 0xff
#define DBUS_DIR_IN 0

#define FF_CK 10		// GPIO10
#define FF_OE 11		// GPIO11

extern file_header fh;
extern float clk_fs;

extern uint8_t	cpu_flg;	// CPU flg 0:W65C02 1:W65C816
extern uint8_t	wup_flg;
extern uint8_t	nmi_sig;
extern uint8_t irq_flg;
extern uint8_t ctlq_ev;		// Ctrl+Q flag;
extern uint8_t irqMask;
extern int maskTimer;

extern void bus_master_operation(void);
extern void make_irq(void);
extern void util_addrdump(const char *, uint32_t , const void *, unsigned int);
extern void stop_fw(int);

// read/write memory
extern void write_sram(uint32_t, const uint8_t*, unsigned int);
extern void read_sram(uint32_t , uint8_t *, unsigned int);
extern void set_address( uint32_t);

#endif  //IODEV_DEF

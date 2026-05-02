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
//#include <stdlib.h>
//#include <assert.h>
#include <string.h>
#include "pico/stdlib.h"

#include "pico_w65.h"
#include "iodev.h"

#define MEM_CHECK_UNIT	BUF_SIZE * 16		// 4 KB
#define MAX_MEM_SIZE	0x00080000			// 512KB

extern uint8_t	cpu_flg;				// CPU flg 0:W65C02 1:W65C816
extern uint8_t buffer[];

extern void stop_fw(int);

// Address Bus
union address_bus_u {
    uint32_t w;             // 32 bits Address
    struct {
        uint16_t sw;       // 16bit Address
        uint8_t hl;        // Address H low
        uint8_t hh;        // Address H high
    };
};

#if 0
//bugfix
static const uint8_t bugfix[16] = {
	0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
	0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf };

#endif

void set_address( uint32_t addr)
{
	uint32_t a;
	
	gpio_put(W65_DCK, 0);	// sram OE=1 sram data bus Hi-z

	a = (addr & 0xff00) >> DBUS_WIDTH;
	gpio_put(FF_CK, 0);
	gpio_put_masked(W65_DTBS, a );	// set A15-A8
	gpio_put(FF_CK, 1);				// set FF data high

//bugfix
//	a = (addr & 0xf0) | bugfix[addr & 0xf];
	a = addr & 0xff;
	gpio_put(FF_CK, 0);
	gpio_put_masked(W65_DTBS, a );	// set A7-A0
	gpio_put(FF_CK, 1);				// set FF data high
}

void write_sram(uint32_t addr, const uint8_t *buf, unsigned int len)
{
    unsigned int i;
	uint32_t ad;

	i=0;
	while( i < len ) {
		// wite operation
		set_address( addr );								// set 16bit address
		gpio_put(W65_RW, 0);								// activate /WE & dis-active sram oe
		ad = (cpu_flg) ? (addr & 0x70000) >> 16 : 0;        // bank 0 to 7 (Max 512kb)
		gpio_put_masked(W65_DTBS, ad);						// set bank address to data bas
		gpio_put(W65_DCK, 1);								// assert bank address
		addr++;
		gpio_put(W65_DCK, 0);
		gpio_put_masked(W65_DTBS, (uint32_t)buf[i] );		// set write data
		i++;
		gpio_put(W65_RW, 1);								// deactivate /WE
	}
}

void read_sram(uint32_t addr, uint8_t *buf, unsigned int len)
{
    unsigned int i;
    unsigned int j;		// use hold and accsesss time control
	uint32_t ad;

	i = j = 0;
	while( i < len ) {
		set_address( addr );								// set 16bit address & dis-active sram OE
		ad = (cpu_flg) ? (addr & 0x70000) >> 16 : 0;        // bank 0 to 7 (Max 512kb)
		gpio_put_masked(W65_DTBS, ad);						// set bank address to data bas
		gpio_put(W65_DCK, 1);								// assert bank address & active sram OE
		gpio_set_dir_masked(W65_DTBS, DBUS_DIR_IN);			// DBUS set as input
		j = i++;
		addr++;
		buf[j] = (uint8_t)(gpio_get_all() & 0xff);			// read data
		gpio_put(W65_DCK, 0);								// dis-active sram OE
       	gpio_set_dir_masked(W65_DTBS, DBUS_DIR_OUT);		// DBUS set as output
	}
}

void util_hexdump(const char *header, const void *addr, unsigned int size)
{
    char chars[17];
    const uint8_t *buf = addr;
    size = ((size + 15) & ~0xfU);
    for (int i = 0; i < size; i++) {
        if ((i % 16) == 0) printf("%s%04x:", header, i);
        printf(" %02x", buf[i]);
        if (0x20 <= buf[i] && buf[i] <= 0x7e) {
            chars[i % 16] = buf[i];
        } else {
            chars[i % 16] = '.';
        }
        if ((i % 16) == 15) {
            chars[16] = '\0';
            printf(" %s\n\r", chars);
        }
    }
}

void util_addrdump(const char *header, uint32_t addr_offs, const void *addr, unsigned int size)
{
    char chars[17];
    const uint8_t *buf = addr;
    uint len;
    len = ((size + 15) & ~0xfU);

    for (unsigned int i = 0; i < len; i++) {
        if ((i % 16) == 0) printf("%s%06lx:", header, addr_offs + i);
        if (i < size) {
            printf(" %02x", buf[i]);
            if (0x20 <= buf[i] && buf[i] <= 0x7e) {
                chars[i % 16] = buf[i];
            } else {
                chars[i % 16] = '.';
            }
        }
        else {
            printf("   ");
            chars[i % 16] = 0x20;
        }
        if ((i % 16) == 15) {
            chars[16] = '\0';
            printf(" %s\n\r", chars);
        }
    }
}

void util_hexdump_sum(const char *header, const void *addr, unsigned int size)
{
    util_hexdump(header, addr, size);

    uint8_t sum = 0;
    const uint8_t *p = addr;
    for (int i = 0; i < size; i++) sum += *p++;
    printf("%s%53s CHECKSUM: %02x\n\r", header, "", sum);
}

uint32_t check_ram(void)
{
    unsigned int i;
    uint32_t addr;
	uint8_t *wr_dat;
	uint8_t *rd_dat;

	wr_dat = &buffer[0];
	rd_dat = &buffer[256];
	
    // RAM check
    for (i = 0; i < BUF_SIZE; i += 2) {
        wr_dat[i + 0] = 0xa5;
        wr_dat[i + 1] = 0x5a;
    }
    for (addr = 0; addr < MAX_MEM_SIZE; addr += MEM_CHECK_UNIT) {
        printf("Memory $000000 - $%06lX\r", addr);
        wr_dat[0] = (addr >>  0) & 0xff;
        wr_dat[1] = (addr >>  8) & 0xff;
        wr_dat[2] = (addr >> 16) & 0xff;
    	write_sram(addr, &wr_dat[0], BUF_SIZE);
        read_sram(addr, &rd_dat[0], BUF_SIZE);

    	if (memcmp(&wr_dat[0], &rd_dat[0], BUF_SIZE) != 0) {
            printf("\nMemory error at $%06lX\n\r", addr);
            util_addrdump("WR: ", addr, &wr_dat[0], BUF_SIZE);
            util_addrdump("RD: ", addr, &rd_dat[0], BUF_SIZE);
			stop_fw(CHECK_RAM_ERROR);		// stop
        }
        if (addr == 0) continue;

    	read_sram(0, &rd_dat[0], BUF_SIZE);
    	if (memcmp(&wr_dat[0], &rd_dat[0], BUF_SIZE) == 0) {
            // if the page at addr is the same as the first page,
			// then addr reachs end of memory
			printf("\nMemory wrap around.\n\r");
        	break;
        }
    }
	printf("Memory $000000 - $%06lX %d KB OK\r\n", addr-1, (int)(addr / 1024));
	return( addr );
}


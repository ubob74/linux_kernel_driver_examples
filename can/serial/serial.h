/*
 * Copyright (C) 2012, Texas Instruments, Inc.
 * Texas Instruments, <www.ti.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define CONFIG_OMAP5EVM

#define CONFIG_BOARD_MACH_TYPE		3777
#define CONFIG_IS_OMAP5

#define OMAP54XX_ROM_CODE_VERSION	0x43FFC
#define CLK32K_COUNTER_REGISTER		0x4AE04030
#define OMAP54XX_L4_CORE_BASE		0x4A000000
#define OMAP54XX_L4_WKUP_BASE		0x4Ae00000
#define OMAP54XX_L4_PER_BASE		0x48000000

#define OMAP54XX_L4_PER				OMAP54XX_L4_PER_BASE

/* UART */
#define OMAP54XX_UART1			(OMAP54XX_L4_PER+0x6a000)
#define OMAP54XX_UART2			(OMAP54XX_L4_PER+0x6c000)
#define OMAP54XX_UART3			(OMAP54XX_L4_PER+0x20000)
#define OMAP54XX_UART4			(OMAP54XX_L4_PER+0x6e000)
#define OMAP54XX_UART5			(OMAP54XX_L4_PER+0x66000)
#define OMAP54XX_UART6			(OMAP54XX_L4_PER+0x68000)

#define CONFIG_BAUDRATE			115200

#define CONFIG_SERIAL_BASE (OMAP54XX_UART3 + 0x60000000UL)
#define CONFIG_SERIAL_CLK_HZ		48000000

#define OFF_RBR		0x00
#define OFF_THR		0x00
#define OFF_DLL		0x00
#define OFF_IER		0x04
#define OFF_DLM		0x04
#define OFF_FCR		0x08
#define OFF_IIR		0x08
#define OFF_LCR		0x0C
#define OFF_MCR		0x10
#define OFF_LSR		0x14
#define OFF_MSR		0x18
#define OFF_SCR		0x1C
#define OFF_MDR1	0x20

#define WR(val, addr) writeb(val, CONFIG_SERIAL_BASE + OFF_##addr)
#define RD(addr) readb(CONFIG_SERIAL_BASE + OFF_##addr)


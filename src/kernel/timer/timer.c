/**========================================================================
 *                             TIMER.C
 *  This source file provides a simple implementation of the Programmable
 *  Interval Timer (PIT) for generating timer interrupts.
 *========================================================================**/
#include "timer.h"
#include "tty.h"
#include "keyboard.h"

extern void ioport_out(unsigned short port, unsigned char value);
extern unsigned char ioport_in(unsigned short port);

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40

static volatile uint64_t ticks = 0;
static uint32_t pit_frequency = 1000; // default 1kHz -> 1 ms per tick

void init_pit(uint32_t frequency)
{
    pit_frequency = frequency;
    uint32_t divisor = 1193180 / frequency;
    ioport_out(PIT_COMMAND_PORT, 0x36); // channel 0, low/high, mode 3, binary
    ioport_out(PIT_CHANNEL0_PORT, divisor & 0xFF);
    ioport_out(PIT_CHANNEL0_PORT, (divisor >> 8) & 0xFF);
}

uint64_t get_timer_ticks(void)
{
    return ticks;
}

void handle_timer_interrupt(void)
{
    // send EOI to PIC
    ioport_out(PIC1_COMMAND_PORT, 0x20);
    ticks++;
}

void sleep_ms(uint64_t ms)
{
    uint64_t target = get_timer_ticks() + ms; // if pit is 1kHz, ms == ticks
    while (get_timer_ticks() < target) {
        asm volatile ("hlt");
    }
}

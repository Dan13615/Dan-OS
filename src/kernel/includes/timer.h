/**========================================================================
 *                             TIMER.H
 *  This header file provides an interface for the Programmable Interval Timer (PIT)
 *  used for generating timer interrupts.
 *========================================================================**/

#ifndef TIMER_H
    #define TIMER_H

    #include <stdint.h>

    /**========================================================================
     *                             FUNCTIONS
     * This section declares the functions used to initialize and manage the PIT timer.
     *========================================================================**/
    void init_pit(uint32_t frequency);
    uint64_t get_timer_ticks(void);
    void sleep_ms(uint64_t ms);
    void handle_timer_interrupt(void);

#endif // TIMER_H

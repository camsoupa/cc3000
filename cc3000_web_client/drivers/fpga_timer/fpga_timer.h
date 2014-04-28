#ifndef MYTIMER_H_  // Only define once
#define MYTIMER_H_  // Only define once

#include "../../CMSIS/a2fxxxm3.h"
#include <stdint.h>

// The technique of using a structure declaration
// to describe the device register layout and names is
// very common practice. Notice that there aren't actually
// any objects of that type defined, so the declaration
// simply indicates the structure without using up any store.
typedef struct
{
    uint32_t overflow; // Offset 0x0
    uint32_t counter; // Offset 0x4
    uint32_t control; // Offset 0x8
    uint32_t compare;
    uint32_t status;
} mytimer_t;

/*
 * Bit 0: Timer Enable bit (timerEn)
 * Bit 1: All Interrupts Enable bit (interruptEn)
 * Bit 2: Compare Interrupt Enable bit (compareEn)
 * Bit 3: Overflow Interrupt Enable bit (overflowEn)
 */
#define TIMER_ENABLE_MASK    0x00000001UL
#define INTERRUPTS_ENABLE_MASK 0x00000002UL
#define COMPARE_ENABLE_MASK    0x00000004UL
#define OVERFLOW_ENABLE_MASK   0x00000008UL

#define TIMER_CAPACITY 20

uint8_t cnt_timers;

// The timers
mytimer_t * timers[TIMER_CAPACITY];

typedef void (*timer_handler_t)(void);


uint8_t add_timer(mytimer_t * timer);

/**
 * Initialize the MYTIMER
 */
void timer_init();

/**
 * Start MYTIMER
 */
void timer_enable(uint8_t timer);

/**
 * Stop MYTIMER
 */
void timer_disable(uint8_t timer);

/**
 * Set the limit to which the timer counts.
 */
void timer_setOverflowVal(uint8_t timer, uint32_t value);

/**
 * Read the counter value of the timer.
 */
uint32_t timer_getCounterVal(uint8_t timer);


/**
 * Enable all interrupts
 */
void timer_enable_allInterrupts(uint8_t timer);

/**
 * Disable all interrupts
 */
void timer_disable_allInterrupts(uint8_t timer);

/**
 * Enable compare interrupt
 */
void timer_enable_compareInt(uint8_t timer);

/**
 * Disable compare interrupt
 */
void timer_disable_compareInt(uint8_t timer);

/**
 * Set Compare value
 */
void timer_setCompareVal(uint8_t timer, uint32_t compare);

/**
 * Enable overflow interrupt
 */
void timer_enable_overflowInt(uint8_t timer);

/**
 * Disable overflow interrupt
 */
void timer_disable_overflowInt(uint8_t timer);

 /**
  * Interrupt status
  */
uint32_t timer_getInterrupt_status(uint8_t timer);

#endif /* timer_H_ */

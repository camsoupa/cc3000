#ifndef MYTIMER_H_  // Only define once
#define MYTIMER_H_  // Only define once

#include "../../CMSIS/a2fxxxm3.h"
#include <stdint.h>

#define MYTIMER_1_BASE (0x40050100)
#define MYTIMER_2_BASE (0x40050200)
#define MYTIMER_3_BASE (0x40050300)
#define MYTIMER_4_BASE (0x40050400)

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
#define MYTIMER_ENABLE_MASK    0x00000001UL
#define INTERRUPTS_ENABLE_MASK 0x00000002UL
#define COMPARE_ENABLE_MASK    0x00000004UL
#define OVERFLOW_ENABLE_MASK   0x00000008UL

// Using the mytimer_t structure we can make the
// compiler do the offset mapping for us.
// To access the device registers, an appropriately
// cast constant is used as if it were pointing to
// such a structure, but of course it points to memory addresses instead.
// Look at at mytimer.c
// Look at the the functions's disassembly
// in .lst file under the Debug folder
mytimer_t * timers[4] = {
 ((mytimer_t *) MYTIMER_1_BASE),
 ((mytimer_t *) MYTIMER_2_BASE),
 ((mytimer_t *) MYTIMER_3_BASE),
 ((mytimer_t *) MYTIMER_4_BASE)
};
/**
 * Initialize the MYTIMER
 */
void MYTIMER_init();

/**
 * Start MYTIMER
 */
void MYTIMER_enable(uint8_t timer);

/**
 * Stop MYTIMER
 */
void MYTIMER_disable(uint8_t timer);

/**
 * Set the limit to which the timer counts.
 */
void MYTIMER_setOverflowVal(uint8_t timer, uint32_t value);

/**
 * Read the counter value of the timer.
 */
uint32_t MYTIMER_getCounterVal(uint8_t timer);


/**
 * Enable all interrupts
 */
void MYTIMER_enable_allInterrupts(uint8_t timer);

/**
 * Disable all interrupts
 */
void MYTIMER_disable_allInterrupts(uint8_t timer);

/**
 * Enable compare interrupt
 */
void MYTIMER_enable_compareInt(uint8_t timer);

/**
 * Disable compare interrupt
 */
void MYTIMER_disable_compareInt(uint8_t timer);

/**
 * Set Compare value
 */
void MYTIMER_setCompareVal(uint8_t timer, uint32_t compare);

/**
 * Enable overflow interrupt
 */
void MYTIMER_enable_overflowInt(uint8_t timer);

/**
 * Disable overflow interrupt
 */
void MYTIMER_disable_overflowInt(uint8_t timer);

 /**
  * Interrupt status
  */
uint32_t MYTIMER_getInterrupt_status(uint8_t timer);

#endif /* MYTIMER_H_ */

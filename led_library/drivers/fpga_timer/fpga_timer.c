#include "fpga_timer.h"

void MYTIMER_init()
{
    // we don't have to do anything.
}

void MYTIMER_enable(uint8_t timer)
{
    (timers[timer])->control |= MYTIMER_ENABLE_MASK;
}

void MYTIMER_disable(uint8_t timer)
{
    (timers[timer])->control &= ~MYTIMER_ENABLE_MASK;
}

void MYTIMER_setOverflowVal(uint8_t timer, uint32_t value)
{
    timers[timer]->overflow = value;
}

uint32_t MYTIMER_getCounterVal(uint8_t timer)
{
	return timer[timers]->counter;
}

/**
 * Enable all interrupts
 */
void MYTIMER_enable_allInterrupts(uint8_t timer)
{
	timers[timer]->control |= INTERRUPTS_ENABLE_MASK;
}

/**
 * Disable all interrupts
 */
void MYTIMER_disable_allInterrupts(uint8_t timer)
{
	timers[timer]->control &= ~INTERRUPTS_ENABLE_MASK;
}

/**
 * Enable compare interrupt
 */
void MYTIMER_enable_compareInt(uint8_t timer)
{
	timers[timer]->control |= COMPARE_ENABLE_MASK;
}

/**
 * Disable compare interrupt
 */
void MYTIMER_disable_compareInt(uint8_t timer)
{
	timers[timer]->control &= ~COMPARE_ENABLE_MASK;
}

/**
 * Set Compare value
 */
void MYTIMER_setCompareVal(uint8_t timer, uint32_t compare)
{
    timers[timer]->compare = compare;
}

/**
 * Enable overflow interrupt
 */
void MYTIMER_enable_overflowInt(uint8_t timer)
{
	timers[timer]->control |= OVERFLOW_ENABLE_MASK;
}

/**
 * Disable overflow interrupt
 */
void MYTIMER_disable_overflowInt(uint8_t timer)
{
	timers[timer]->control &= ~OVERFLOW_ENABLE_MASK;
}

 /**
  * Interrupt status
  */
uint32_t MYTIMER_getInterrupt_status(uint8_t timer)
{
    return timers[timer]->status;
}

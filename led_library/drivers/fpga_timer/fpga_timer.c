#include "fpga_timer.h"

void timer_init()
{
    cnt_timers = 0;
}

uint8_t add_timer(mytimer_t * timer)
{
	uint8_t timer_index = cnt_timers;
	if(cnt_timers < TIMER_CAPACITY)
	  timers[cnt_timers++] = timer;
	  return timer_index;

	return TIMER_CAPACITY;
}

void timer_enable(uint8_t timer)
{
    (timers[timer])->control |= TIMER_ENABLE_MASK;
}

void timer_disable(uint8_t timer)
{
    (timers[timer])->control &= ~TIMER_ENABLE_MASK;
}

void timer_setOverflowVal(uint8_t timer, uint32_t value)
{
    timers[timer]->overflow = value;
}

uint32_t timer_getCounterVal(uint8_t timer)
{
	return timer[timers]->counter;
}

/**
 * Enable all interrupts
 */
void timer_enable_allInterrupts(uint8_t timer)
{
	timers[timer]->control |= INTERRUPTS_ENABLE_MASK;
}

/**
 * Disable all interrupts
 */
void timer_disable_allInterrupts(uint8_t timer)
{
	timers[timer]->control &= ~INTERRUPTS_ENABLE_MASK;
}

/**
 * Enable compare interrupt
 */
void timer_enable_compareInt(uint8_t timer)
{
	timers[timer]->control |= COMPARE_ENABLE_MASK;
}

/**
 * Disable compare interrupt
 */
void timer_disable_compareInt(uint8_t timer)
{
	timers[timer]->control &= ~COMPARE_ENABLE_MASK;
}

/**
 * Set Compare value
 */
void timer_setCompareVal(uint8_t timer, uint32_t compare)
{
    timers[timer]->compare = compare;
}

/**
 * Enable overflow interrupt
 */
void timer_enable_overflowInt(uint8_t timer)
{
	timers[timer]->control |= OVERFLOW_ENABLE_MASK;
}

/**
 * Disable overflow interrupt
 */
void timer_disable_overflowInt(uint8_t timer)
{
	timers[timer]->control &= ~OVERFLOW_ENABLE_MASK;
}

 /**
  * Interrupt status
  */
uint32_t timer_getInterrupt_status(uint8_t timer)
{
    return timers[timer]->status;
}

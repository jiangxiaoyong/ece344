#include <types.h>
#include <lib.h>
#include <machine/spl.h>
#include <thread.h>
#include <clock.h>
#include <page_replacement.h>

/* 
 * The address of lbolt has thread_wakeup called on it once a second.
 */
int lbolt;

static int lbolt_counter;
static int ref_counter;

/*
 * This is called HZ times a second by the timer device setup.
 */

void
hardclock(void)
{
	/*
	 * Collect statistics here as desired.
	 */
	ref_counter++;
	if(ref_counter >= (HZ/10))
	{
		ref_counter = 0;	
		ref_bit_counter();
	}


	lbolt_counter++;
	if (lbolt_counter >= HZ) {
		lbolt_counter = 0;
		thread_wakeup(&lbolt);
	}

	thread_yield();
}

/*
 * Suspend execution for n seconds.
 */
void
clocksleep(int num_secs)
{
	int s;

	s = splhigh();
	while (num_secs > 0) {
		thread_sleep(&lbolt);
		num_secs--;
	}
	splx(s);
}

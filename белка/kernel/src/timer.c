#ifndef TIMER_INCLUDED
#define TIMER_INCLUDED


void (*on_timer_tick)() = 0;


void timer_interrupt_handler(Byte* data)
{
	if(on_timer_tick)
		on_timer_tick();
}


void initialize_timer()
{
	Number16 time_divider = 1000;

	asm("cli");
	out_8(0x43, 00110100b); //channel 0, lobyte/hibyte, rate generator
	out_8(0x40, time_divider);
	out_8(0x40, time_divider >> 8);
	asm("sti");

	add_interrupt_handler(32, &timer_interrupt_handler, 0);
}


#endif//TIMER_INCLUDED
#ifndef ERRORS_INCLUDED
#define ERRORS_INCLUDED


asm (
	"pusha \n"
	"call division_by_zero_interrupt_handler\n"
	"popa \n"
	"iret"
);
void division_by_zero_interrupt_handler()
{
	print("division by zero\n");
	out_8(0x20, 0x20);
}


void add_division_by_zero_interrupt_handler()
{
	add_interrupt_handler(0, (Number32)&division_by_zero_interrupt_handler - 8);
}


#endif//ERRORS_INCLUDED
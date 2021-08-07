#ifndef INTERRUPTS_INCLUDED
#define INTERRUPTS_INCLUDED


#include "IO.c"


typedef enum
{
	TASK_GATE_32      = 0b0101,
	INTERRUPT_GATE_16 = 0b0110,
	TRAP_GATE_16      = 0b0111,
	INTERRUPT_GATE_32 = 0b1110,
	TRAP_GATE_32      = 0b1111
}
Interrupt_Type;


typedef struct
{
	Number16 handler_address_low;
	Number16 selector;
	Number8  zero;
	Number8  attributes;
	Number16 handler_address_high;
}
IDT_Node;


IDT_Node idt_nodes[256];


void add_interrupt_handler(Number8 interrupt_number, Bit32 handler)
{
	//IDT_Node* node = 0;
	asm("cli");
	idt_nodes[interrupt_number].handler_address_low = handler;
	idt_nodes[interrupt_number].handler_address_high = handler >> 16;
	idt_nodes[interrupt_number].selector = 8;
	idt_nodes[interrupt_number].zero = 0;
	idt_nodes[interrupt_number].attributes = 0b10000000 | INTERRUPT_GATE_32;
	asm("sti");
}


typedef struct
{
	Number16 size;
	Number32 address;
}
IDT_Table_Address;


asm(
	"pusha \n"
	"call default_interrupt_handler\n"
	"popa \n"
	"iret"
);
void default_interrupt_handler()
{
	out_8(0x20, 0x20);
}


void initialize_interrupts()
{
	IDT_Table_Address idtr;
	Number            i;

	idtr.size    = sizeof(idt_nodes);//256 * 8;
	idtr.address = idt_nodes;

	asm(
		"cli\n"
		"lidt (%0)"
		:
		: "a"(&idtr)
	);

	for(i = 0; i < 256; ++i)
		add_interrupt_handler(i, (Number32)&default_interrupt_handler - 8);

	//настройка PIC
	out_8(0x20, 0x11);
	out_8(0x21, 32);
	out_8(0x21, 0x04);
	out_8(0x21, 0x01);
	out_8(0x21, 0x0);

	out_8(0xA0, 0x11);
	out_8(0xA1, 40);
	out_8(0xA1, 0x02);
	out_8(0xA1, 0x01);
	out_8(0xA1, 0x0);

	asm("sti");
}


#endif//INTERRUPTS_INCLUDED
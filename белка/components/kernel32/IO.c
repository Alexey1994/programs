#ifndef IO_INCLUDED
#define IO_INCLUDED


Number8 in_8(Number16 port)
{
	Number8 data;

	asm(
		"inb %1, %0"
		: "=a"(data)
		: "Nd"(port)
	);

	return data;
}

Number16 in_16(Number16 port)
{
	Number16 data;

	asm(
		"inw %1, %0"
		: "=a"(data)
		: "Nd"(port)
	);

	return data;
}

Number32 in_32(Number16 port)
{
	Number32 data;

	asm(
		"inl %1, %0"
		: "=a"(data)
		: "Nd"(port)
	);

	return data;
}


void out_8(Number16 port, Number8 data)
{
	asm(
		"outb %0, %1"
		:
		: "a"(data), "Nd"(port)
	);
}

void out_16(Number16 port, Number16 data)
{
	asm(
		"outw %0, %1"
		:
		: "a"(data), "Nd"(port)
	);
}

void out_32(Number16 port, Number32 data)
{
	asm(
		"outl %0, %1"
		:
		: "a"(data), "Nd"(port)
	);
}


#endif//IO_INCLUDED
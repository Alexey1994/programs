SECTIONS
{
	. = 0x20000;
	. = ALIGN(4);
	.text : {
		INCLUDE "kernel16.hex";
		* (.text);
	}
}
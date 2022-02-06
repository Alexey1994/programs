SECTIONS
{
	. = 0x20000;
	. = ALIGN(4);
	.text : {
		INCLUDE "bin/components/kernel16.hex";
		* (.text);
	}
	.data : {
		* (.data);
	}
}
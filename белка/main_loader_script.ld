SECTIONS
{
	. = 0x20000;
	. = ALIGN(4);
	.text : {
		INCLUDE "bin/components/main-loader16.hex";
		* (.text);
	}
	.data : {
		* (.data);
	}
}
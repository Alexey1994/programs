SECTIONS
{
	. = 0x00000;
	. = ALIGN(4);
	.text : {
		* (.text);
	}
	.data : {
		* (.data);
	}
}
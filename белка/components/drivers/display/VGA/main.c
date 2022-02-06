#include <kernel.h>


typedef enum
{
	VGA_AC_INDEX        = 0x3C0,
	VGA_AC_WRITE        = 0x3C0,
	VGA_AC_READ         = 0x3C1,
	VGA_MISC_WRITE      = 0x3C2,
	VGA_SEQ_INDEX       = 0x3C4,
	VGA_SEQ_DATA        = 0x3C5,
	VGA_DAC_READ_INDEX  = 0x3C7,
	VGA_DAC_WRITE_INDEX = 0x3C8,
	VGA_DAC_DATA        = 0x3C9,
	VGA_MISC_READ       = 0x3CC,
	VGA_GC_INDEX        = 0x3CE,
	VGA_GC_DATA         = 0x3CF,
	// COLOR emulation MONO emulation
	VGA_CRTC_INDEX      = 0x3D4, // 0x3B4
	VGA_CRTC_DATA       = 0x3D5, // 0x3B5
	VGA_INSTAT_READ     = 0x3DA
}
VGA_Register;


typedef struct
{
	Byte misc_io_address_select:1;   // 0 - CRT register is 0x03Bx, 1 - 0x03Dx
	Byte misc_enable_RAM:1;          // 0 - disable, 1 - enable
	Byte misc_clock:2;               // 00 - 25mHz(320/640), 01 - 28mHz(360/720)
	Byte misc_zero:1;
	Byte misc_page_odd_even:1;       // 0 - select low page, 1 - high
	Byte misc_horizontal_polarity:1; // 0 - positive
	Byte misc_vertical_polarity:1;   // 0 - positive

	struct {
		Byte reset; // 0b01 - synchronous, 0b10 - asynchronous
		
		Byte clocking_dot9_or_8:1; // 0 - 9 dots per character or 720 in graphics, 1 - 8 dots or 640
		Byte clocking_zero:1;
		Byte clocking_shift_load_rate:1; // maybe 0
		Byte clocking_dot_clock_rate:1; // when 1 then master clock divided by 2 for 320 and 360 modes
		Byte clocking_shift_4_enable:1; // maybe 0
		Byte clocking_screen_disable:1; // 0 - enable, 1 - disable screen

		Byte map_mask; // 0b1111 for use all 4 65536 planes
		Byte character_map; // 0 if no use character map

		Byte memory_mode_zero:1;
		Byte memory_mode_extended_memory:1; // set 1 to enable the video memory from 64kb to 256kb
		Byte memory_mode_odd_even:1; // 0 for maps 0 and 2, 1 for 1 and 3
		Byte memory_mode_chain_4:1; // 0 for one map, 1 for 4 chain
	}
	sequencer;

	struct {
		Byte horizontal_total;
		Byte end_horizontal_display;
		Byte start_horizontal_blanking;

		Byte end_horizontal_blanking:5;
		Byte end_horizontal_blanking_enable_skew:2; // maybe 0
		Byte end_horizontal_blanking_enable_vertical_retrace:1;

		Byte start_horizontal_retrace;

		Byte end_horizontal_retrace:5;
		Byte end_horizontal_retrace_enable_skew:2; // maybe 0
		Byte end_horizontal_blanking_bit5:1;

		Byte vertical_total;

		Byte vertical_total_bit8:1;
		Byte vertical_display_end_bit8:1;
		Byte vertical_retrace_start_bit8:1;
		Byte start_vertical_blanking_bit8:1;
		Byte line_compare_bit8:1;
		Byte vertical_total_bit9:1;
		Byte vertical_display_end_bit9:1;
		Byte vertical_retrace_start_bit9:1;

		Byte preset_row_scan:5;
		Byte byte_panning:2;
		Byte zero1:1;

		Byte maximum_scan_line:5;
		Byte start_vertical_blanking_bit9:1;
		Byte line_compare_bit9:1;
		Byte scan_doubling:1;

		Byte cursor_scan_line_start:5;
		Byte cursor_disable:1;
		Byte zero2:2;

		Byte cursor_scan_line_end:5;
		Byte cursor_scew:2;
		Byte zero3:1;

		Byte start_address_high;
		Byte start_address_low;

		Byte cursor_location_high;
		Byte cursor_location_low;

		Byte vertical_retrace_start;

		Byte vertical_retrace_end:4;
		Byte zero4:2;
		Byte vertical_retrace_end_bandwidth:1;
		Byte vertical_retrace_end_protect:1;

		Byte vertical_display_end;
		Byte offset;

		Byte underline_location:5;
		Byte divide_4:1;
		Byte double_word_addressing:1;
		Byte zero5:1;

		Byte start_vertical_blanking;
		Byte end_vertical_blanking;

		Byte mode_map13:1;
		Byte mode_map14:1;
		Byte divide_scanline_clock_by_2:1;
		Byte divide_memory_address_clock_by_2:1;
		Byte address_wrap_select:1;
		Byte word_or_byte_method:1; // 0 - word, 1 - byte
		Byte enable_sync:1;

		Byte line_compare;
	}
	CRT;

	struct {
		Byte set_reset:4; // planes
		Byte zero1:4;

		Byte enable_set_reset:4; // planes
		Byte zero2:4;

		Byte color_compare:4; // planes
		Byte zero3:4;

		Byte data_rotate_count:3;
		Byte data_rotate_logical_operation:2; // 0b00 - same, 0b01 - AND with latch, 0b10 - OR, 0b11 - XOR
		Byte zero4:3;

		Byte read_map_select:2;
		Byte zero5:6;

		Byte write_mode:2; // 0b00 - Write Mode 0, 0b01 - Write Mode 1, 0b10 - Write Mode 3
		Byte zero6:1;
		Byte read_mode:1;
		Byte host_even_odd:1;
		Byte shift_register_interleave:1;
		Byte shift_256:1;
		Byte zero7:1;

		Byte disable_alphanumeric_mode:1;
		Byte chain_even_odd:1;
		Byte memory_map_select:2; // 0b00 - A0000h-BFFFF, 0b01 - A0000h-AFFFF, 0b10 - B0000h-B7FFF, 0b11 - B8000h-BFFFF
		Byte zero8:4;

		Byte color_dont_care:4; // planes
		Byte zero9:4;

		Byte color_mask;
	}
	graphics;

	struct {
		Byte address:5;
		Byte palette_address_source:1;
		Byte zero1:2;

		Byte internal_palette_index:6;
		Byte zero2:2;

		//...
	}
	attribute;

	//struct {

	//}
	//color;
}
VGA_Mode;


void test_VGA();


void start_module(Kernel* kernel, Number module_address)
{
	test_VGA();
}


#include "IO.c"


void write_regs(Byte *regs)
{
	Number i;

	// write MISCELLANEOUS reg
	out_8(VGA_MISC_WRITE, *regs);
	++regs;

	// write SEQUENCER regs
	for(i = 0; i < 5; i++)
	{
		out_8(VGA_SEQ_INDEX, i);
		out_8(VGA_SEQ_DATA, *regs);
		++regs;
	}

	// unlock CRTC registers
	out_8(VGA_CRTC_INDEX, 0x03);
	out_8(VGA_CRTC_DATA, in_8(VGA_CRTC_DATA) | 0x80);
	out_8(VGA_CRTC_INDEX, 0x11);
	out_8(VGA_CRTC_DATA, in_8(VGA_CRTC_DATA) & ~0x80);
	
	// make sure they remain unlocked
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;
	
	// write CRTC regs
	for(i = 0; i < 25; ++i)
	{
		out_8(VGA_CRTC_INDEX, i);
		out_8(VGA_CRTC_DATA, *regs);
		++regs;
	}
	
	// write GRAPHICS CONTROLLER regs
	for(i = 0; i < 9; ++i)
	{
		out_8(VGA_GC_INDEX, i);
		out_8(VGA_GC_DATA, *regs);
		++regs;
	}
	
	// write ATTRIBUTE CONTROLLER regs
	for(i = 0; i < 21; ++i)
	{
		in_8(VGA_INSTAT_READ);
		out_8(VGA_AC_INDEX, i);
		out_8(VGA_AC_WRITE, *regs);
		++regs;
	}
	
	// lock 16-color palette and unblank display
	in_8(VGA_INSTAT_READ);
	out_8(VGA_AC_INDEX, 0x20);
}

void test_VGA()
{
	Byte g_640x480x16[] =
	{
		// MISC
		0b00000011,//0xE3,
		// SEQ
		0b01, 0b000001, 0b1111, 0x00, 0b10, //0x03, 0x01, 0x08, 0x00, 0x06,
		// CRTC
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
		0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
		0xFF,
		// GC
		0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
		0xFF,
		// AC
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x01, 0x00, 0x0F, 0x00, 0x00
	};

	/*Byte g_320x200x256[] =
	{
		// MISC
		0x63,
		// SEQ
		0x03, 0x01, 0x0F, 0x00, 0x0E,
		// CRTC
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
		0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
		0xFF,
		// GC
		0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
		0xFF,
		// AC
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x41, 0x00, 0x0F, 0x00,	0x00
	};*/

	Byte*  screen;
	Number i;

	screen = 0xA0000;

	//write_regs(g_320x200x256);
	write_regs(g_640x480x16);

	for(i = 0; i < 65536 * 4; ++i)
		screen[i] = 0b00000000;
}
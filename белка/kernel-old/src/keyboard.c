#ifndef KEYBOARD_INCLUDED
#define KEYBOARD_INCLUDED


typedef enum
{
	KEY_ESCAPE               = 1,
	KEY_1                    = 2,
	KEY_2                    = 3,
	KEY_3                    = 4,
	KEY_4                    = 5,
	KEY_5                    = 6,
	KEY_6                    = 7,
	KEY_7                    = 8,
	KEY_8                    = 9,
	KEY_9                    = 10,
	KEY_0                    = 11,
	KEY_MINUS                = 12,
	KEY_EQUAL                = 13,
	KEY_BACKSPACE            = 14,
	KEY_TAB                  = 15,
	KEY_Q                    = 16,
	KEY_W                    = 17,
	KEY_E                    = 18,
	KEY_R                    = 19,
	KEY_T                    = 20,
	KEY_Y                    = 21,
	KEY_U                    = 22,
	KEY_I                    = 23,
	KEY_O                    = 24,
	KEY_P                    = 25,
	KEY_SQUARE_BRACKET_OPEN  = 26,
	KEY_SQUARE_BRACKET_CLOSE = 27,
	KEY_ENTER                = 28,
	KEY_CONTROL              = 29,
	KEY_A                    = 30,
	KEY_S                    = 31,
	KEY_D                    = 32,
	KEY_F                    = 33,
	KEY_G                    = 34,
	KEY_H                    = 35,
	KEY_J                    = 36,
	KEY_K                    = 37,
	KEY_L                    = 38,
	KEY_SEMICOLON            = 39,
	KEY_APOSTROPHE           = 40,
	KEY_BACK_APOSTROPHE      = 41,
	KEY_LEFT_SHIFT           = 42,
	KEY_BACKSLASH            = 43,
	KEY_Z                    = 44,
	KEY_X                    = 45,
	KEY_C                    = 46,
	KEY_V                    = 47,
	KEY_B                    = 48,
	KEY_N                    = 49,
	KEY_M                    = 50,
	KEY_COMMA                = 51,
	KEY_DOT                  = 52,
	KEY_SLASH                = 53,
	KEY_RIGHT_SHIFT          = 54,
	KEY_NUMPAD_MULTIPLY      = 55,
	KEY_ALT                  = 56,
	KEY_SPACE                = 57,
	KEY_CAPS_LOCK            = 58,
	KEY_F1                   = 59,
	KEY_F2                   = 60,
	KEY_F3                   = 61,
	KEY_F4                   = 62,
	KEY_F5                   = 63,
	KEY_F6                   = 64,
	KEY_F7                   = 65,
	KEY_F8                   = 66,
	KEY_F9                   = 67,
	KEY_F10                  = 68,
	KEY_NUMPAD_LOCK          = 69,
	KEY_SCROLL_LOCK          = 70,
	KEY_NUMPAD_7             = 71,
	KEY_NUMPAD_8             = 72,
	KEY_NUMPAD_9             = 73,
	KEY_NUMPAD_MINUS         = 74,
	KEY_NUMPAD_4             = 75,//KEY_LEFT
	KEY_NUMPAD_5             = 76,
	KEY_NUMPAD_6             = 77,//KEY_RIGHT
	KEY_NUMPAD_PLUS          = 78,
	KEY_NUMPAD_1             = 79,
	KEY_NUMPAD_2             = 80,
	KEY_NUMPAD_3             = 81,
	KEY_NUMPAD_0             = 82,
	KEY_NUMPAD_DOT           = 83,

	KEY_F11                  = 87,
	KEY_F12	                 = 88,

	KEY_WINDOWS              = 91,
	KEY_MENU                 = 93
}
Key_Code;


Boolean pressed_special_key = 0;


void(*on_key_down)(Key_Code key_code) = 0;
void(*on_key_up)(Key_Code key_code) = 0;


asm (
	"pusha \n"
	"call keyboard_interrupt_handler\n"
	"popa \n"
	"iret"
);
void keyboard_interrupt_handler()
{
	Byte key_code;

	key_code = in_8(0x60);

	if(key_code == 0xE0)
	{
		pressed_special_key = 1;
	}
	else
	{
		if(key_code & 10000000b)
		{
			if(on_key_up)
				on_key_up(key_code);// & 01111111b);
		}
		else
		{
			if(on_key_down)
				on_key_down(key_code);
		}

		pressed_special_key = 0;
	}

	out_8(0x20, 0x20);
}


void initialize_keyboard()
{
	add_interrupt_handler(33, (Number32)&keyboard_interrupt_handler - 8);
}


#endif//KEYBOARD_INCLUDED
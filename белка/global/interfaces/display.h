typedef struct
{
	Number                  module_address;
	struct Display_Context* next;

	Boolean (*set_mode) (Number width, Number height, Number bits_per_pixel, Number frequency);
}
Display_Context;
typedef struct Console_Context
{
	Number                  module_address;
	struct Console_Context* next;

	void (*print)(struct Kernel* kernel, Byte* parameters, ...);
	void (*clear)(struct Kernel* kernel);

	Bit16*   address;
	Number32 x_coord;
	Number32 y_coord;
	Number32 width;
	Number32 height;

	Writer   writer;
}
Console_Context;
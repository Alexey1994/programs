#ifndef GRAPHICS_RASTERIZER_INCLUDED
#define GRAPHICS_RASTERIZER_INCLUDED


#include <types.c>


Number calculate_sqrt(Number a)
{
	Number Xn;
	Number Xn_next;

	Xn = a;

	for(;;)
	{
		Xn_next = (Xn + a / Xn) >> 1;

		if(Xn_next == Xn)
			return Xn;

		Xn = Xn_next;

		print("iterate\n");
	}
}


#endif//GRAPHICS_RASTERIZER_INCLUDED



#include <system/graphics.c>


void main()
{
	Graphics graphics;
	Number32* canvas = malloc(1440 * 900 * 4);

	initialize_graphics(&graphics, 1440, 900);

	print(_Number, calculate_sqrt(117));

	for(;;)
	{
		Number i;
		for(i = 0; i < graphics.width * graphics.height; ++i)
			canvas[i] = (128 << 24) + 128;

		glClear(GL_COLOR_BUFFER_BIT);
		glRasterPos2f(-1, -1);
		glDrawPixels(1440, 900, GL_RGBA, GL_UNSIGNED_BYTE, canvas);

		update_messages(&graphics);
		draw_graphics(&graphics);
	}
}
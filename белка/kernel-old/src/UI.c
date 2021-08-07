#ifndef UI_INCLUDED
#define UI_INCLUDED


#define MAX_NUMBER_OF_ELEMENTS 128


typedef struct
{
	void (*draw)(struct View* view, struct View_Element* element);
	void(*on_enter)(struct View_Element* element);
}
View_Element;


typedef struct
{
	View_Element* elements[MAX_NUMBER_OF_ELEMENTS];
	Number        number_of_elements;

	Number busy_top;
	Number busy_left;
}
View;


View*         active_view = 0;
View_Element* active_element = 0;


void initialize_view(View* view)
{
	view->number_of_elements = 0;
}


void deinitialize_view(View* view)
{

}


void draw_view(View* view)
{
	Number i;

	clear_text_screen();

	if(!active_element && view->number_of_elements)
		active_element = view->elements[0];

	view->busy_top = 0;
	view->busy_left = 0;

	for(i = 0; i < view->number_of_elements; ++i)
		view->elements[i]->draw(view, view->elements[i]);

	active_view = view;
}


typedef struct
{
	void(*draw_button)(View* view, struct Button* button);
	void(*on_enter)(struct Button* button);

	Byte* name;
	Byte* context;
}
Button;


void draw_button(View* view, Button* button)
{
	Number i;
	Number x = view->busy_left;
	Number y = view->busy_top;
	Byte background_color;

	if(button == active_element)
		background_color = CONSOLE_GREEN_DARK;
	else
		background_color = CONSOLE_GRAY_DARK;

	set_text_screen_character(x + 1, y + 1, ' ', background_color, CONSOLE_WHITE);
	for(i = 0; button->name[i]; ++i)
		set_text_screen_character(x + i + 2, y + 1, button->name[i], background_color, CONSOLE_WHITE);
	set_text_screen_character(x + i + 2, y + 1, ' ', background_color, CONSOLE_WHITE);

	view->busy_left += i + 2;
}


void add_button(View* view, Button* button)
{
	button->draw_button = &draw_button;
	view->elements[view->number_of_elements] = button;
	++view->number_of_elements;
}


void handle_UI_key_down(Key_Code key_code)
{
	Number i;

	if(!active_view)
		return;

	switch(key_code)
	{
		case KEY_ENTER:
			if(active_element)
				active_element->on_enter(active_element);
			break;

		case KEY_NUMPAD_4:
			for(i = 0; i < active_view->number_of_elements; ++i)
				if(active_view->elements[i] == active_element)
					break;

			if(i < active_view->number_of_elements)
			{
				if(i == 0)
					active_element = active_view->elements[active_view->number_of_elements - 1];
				else
					active_element = active_view->elements[i - 1];
			}
			else
				active_element = 0;

			draw_view(active_view);
			break;

		case KEY_NUMPAD_6:
			for(i = 0; i < active_view->number_of_elements; ++i)
				if(active_view->elements[i] == active_element)
					break;

			if(i < active_view->number_of_elements)
			{
				if(i == active_view->number_of_elements)
					active_element = active_view->elements[0];
				else
					active_element = active_view->elements[i + 1];
			}
			else
				active_element = 0;

			draw_view(active_view);
			break;
	}
}


#endif//UI_INCLUDED


#include "snake.c"


View view;


void b1_action()
{
	//print("Hi!", 0);
	start_snake();
}

Button b1 = {
	.name = "Snake",
	.on_enter = &b1_action
};


void b2_action()
{
	print("Hi!!", 0);
}

Button b2 = {
	.name = "No",
	.on_enter = &b2_action
};


void b3_action()
{
	print("Kysh!!!", 0);
}

Button b3 = {
	.name = "Napugatj kur",
	.on_enter = &b3_action
};


void draw_program()
{
	initialize_view(&view);

	add_button(&view, &b1);
	add_button(&view, &b2);
	add_button(&view, &b3);
	draw_view(&view);

	on_key_down = &handle_UI_key_down;
}
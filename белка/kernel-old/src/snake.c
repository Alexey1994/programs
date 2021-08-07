Number get_number_of_ticks()
{
	return ticks;
}

#include <number/random.c>


typedef enum
{
	SNAKE_LEFT,
	SNAKE_RIGHT,
	SNAKE_UP,
	SNAKE_DOWN
}
Snake_Vector;


typedef struct
{
	Number x;
	Number y;
}
Snake_Coord;


Boolean snake_in_game_state = 0;
Snake_Vector snake_vector = SNAKE_RIGHT;

Snake_Coord snake[80 * 25];
Number snake_length = 3;


void snake_start_screen()
{
	print("\n\n\n   YF;VBNT GHJ,TK LKZ YFXFKF\n", 0);//нажмите пробел для начала
	snake_in_game_state = 0;
}


void draw_snake_pixel(Number x, Number y)
{
	set_text_screen_character(x, y + 1, '8', 0, 15);
	//default_text_screen.address[default_text_screen.width * y + x] = 0xDB + (15 << 8);
}


void clean_snake_pixel(Number x, Number y)
{
	set_text_screen_character(x, y + 1, ' ', 0, 0);
	//default_text_screen.address[default_text_screen.width * y + x] = 0;
}


Snake_Coord snake_apple_coords;


void generate_apple()
{
	Number i;

repeat_generation:
	snake_apple_coords.x = random_number() % default_text_screen.width;
	snake_apple_coords.y = random_number() % default_text_screen.height;

	for(i = 0; i < snake_length; ++i)
		if(snake_apple_coords.x == snake[i].x && snake_apple_coords.y == snake[i].y)
			goto repeat_generation;

	set_text_screen_character(snake_apple_coords.x, snake_apple_coords.y + 1, '7', 0, 15);
	//default_text_screen.address[default_text_screen.width * snake_apple_coords.y + snake_apple_coords.x] = 0xDB + (13 << 8);
}


void draw_snake()
{
	Number i;
	for(i = 0; i < snake_length; ++i)
		draw_snake_pixel(snake[i].x, snake[i].y);

	generate_apple();
}


void draw_next_snake_position()
{
	Number i;

	switch(snake_vector)
	{
		case SNAKE_RIGHT:
			snake[snake_length].x = snake[snake_length - 1].x + 1;
			snake[snake_length].y = snake[snake_length - 1].y;
			break;

		case SNAKE_LEFT:
			snake[snake_length].x = snake[snake_length - 1].x - 1;
			snake[snake_length].y = snake[snake_length - 1].y;
			break;

		case SNAKE_UP:
			snake[snake_length].x = snake[snake_length - 1].x;
			snake[snake_length].y = snake[snake_length - 1].y - 1;
			break;

		case SNAKE_DOWN:
			snake[snake_length].x = snake[snake_length - 1].x;
			snake[snake_length].y = snake[snake_length - 1].y + 1;
			break;
	}

	if(snake[snake_length].x == snake_apple_coords.x && snake[snake_length].y == snake_apple_coords.y)
	{
		generate_apple();
		++snake_length;
	}
	else
	{
		clean_snake_pixel(snake->x, snake->y);

		for(i = 0; i < snake_length; ++i)
		{
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;
		}
	}

	for(i = 0; i < snake_length - 1; ++i)
	{
		if(snake[i].x == snake[snake_length - 1].x && snake[i].y == snake[snake_length - 1].y)
		{
			snake_in_game_state = 0;
			return;
		}
	}

	if(snake[snake_length - 1].x < default_text_screen.width && snake[snake_length - 1].y < default_text_screen.height)
		draw_snake_pixel(snake[snake_length - 1].x, snake[snake_length - 1].y);
	else
		snake_in_game_state = 0;
}


void start_snake_game()
{
	clear_text_screen();
	snake_vector = SNAKE_RIGHT;
	snake[0].x = 0;
	snake[0].y = 0;
	snake[1].x = 1;
	snake[1].y = 0;
	snake[2].x = 2;
	snake[2].y = 0;
	snake_length = 3;
	draw_snake();
	snake_in_game_state = 1;
}


void handle_user_input(Key_Code key_code)
{
	switch(key_code)
	{
		case KEY_SPACE:
			if(!snake_in_game_state)
				start_snake_game();
			break;

		case KEY_NUMPAD_4:
			if(snake_in_game_state)
			{
				if(snake_vector != SNAKE_RIGHT)
					snake_vector = SNAKE_LEFT;
			}
			break;

		case KEY_NUMPAD_6:
			if(snake_in_game_state)
			{
				if(snake_vector != SNAKE_LEFT)
					snake_vector = SNAKE_RIGHT;
			}
			break;

		case KEY_NUMPAD_8:
			if(snake_in_game_state)
			{
				if(snake_vector != SNAKE_DOWN)
					snake_vector = SNAKE_UP;
			}
			break;

		case KEY_NUMPAD_2:
			if(snake_in_game_state)
			{
				if(snake_vector != SNAKE_UP)
					snake_vector = SNAKE_DOWN;
			}
			break;

		case KEY_ESCAPE:
			start_commander();
			break;
	}
}


Number ticks = 0;
void handle_timer_tick()
{
	if(ticks >= 100)
	{
		if(snake_in_game_state)
			draw_next_snake_position();
		
		ticks = 0;
	}
	else
		++ticks;
}


void start_snake()
{
	initialize_default_text_screen();
	initialize_interrupts();
	initialize_timer();
	initialize_keyboard();

	clear_text_screen();
	hide_text_screen_cursor();

	on_key_down = handle_user_input;
	on_timer_tick = handle_timer_tick;

	snake_start_screen();
}
Number cursor_position = 0;
Byte command[80] = {0};


void write_command_character(Byte character)
{
	//set_text_screen_character(1 + cursor_position, 24, character, CONSOLE_BLACK, CONSOLE_WHITE);

	draw_character(cursor_position + 1, 0, character);

	command[cursor_position] = character;
	++cursor_position;
}


Boolean is_command(Byte* command_name, Byte** argument)
{
	Byte* in;

	in = command;

	for(;;)
	{
		if(*in == ' ')
			break;

		if(*in != *command_name)
			return 0;

		if(!*in || !*command_name)
			break;

		++in;
		++command_name;
	}

	if(argument)
	{
		while(*in && *in == ' ')
			++in;

		*argument = in;
	}

	return 1;
}


void start_snake();
void handle_commander_keydown(Key_Code key_code)
{
	Number i;
	Byte*  argument;

	switch(key_code)
	{
		case KEY_BACK_APOSTROPHE:
			write_command_character('`');
			break;

		case KEY_SEMICOLON:
			write_command_character(';');
			break;

		case KEY_APOSTROPHE:
			write_command_character('\'');
			break;

		case KEY_A:
			write_command_character('A');
			break;

		case KEY_B:
			write_command_character('B');
			break;

		case KEY_C:
			write_command_character('C');
			break;

		case KEY_D:
			write_command_character('D');
			break;

		case KEY_E:
			write_command_character('E');
			break;

		case KEY_F:
			write_command_character('F');
			break;

		case KEY_G:
			write_command_character('G');
			break;

		case KEY_H:
			write_command_character('H');
			break;

		case KEY_I:
			write_command_character('I');
			break;

		case KEY_J:
			write_command_character('J');
			break;

		case KEY_K:
			write_command_character('K');
			break;

		case KEY_L:
			write_command_character('L');
			break;

		case KEY_M:
			write_command_character('M');
			break;

		case KEY_N:
			write_command_character('N');
			break;

		case KEY_O:
			write_command_character('O');
			break;

		case KEY_P:
			write_command_character('P');
			break;

		case KEY_Q:
			write_command_character('Q');
			break;

		case KEY_R:
			write_command_character('R');
			break;

		case KEY_S:
			write_command_character('S');
			break;

		case KEY_T:
			write_command_character('T');
			break;

		case KEY_U:
			write_command_character('U');
			break;

		case KEY_V:
			write_command_character('V');
			break;

		case KEY_W:
			write_command_character('W');
			break;

		case KEY_X:
			write_command_character('X');
			break;

		case KEY_Y:
			write_command_character('Y');
			break;

		case KEY_Z:
			write_command_character('Z');
			break;

		case KEY_SQUARE_BRACKET_OPEN:
			write_command_character('[');
			break;

		case KEY_SQUARE_BRACKET_CLOSE:
			write_command_character(']');
			break;

		case KEY_COMMA:
			write_command_character(',');
			break;

		case KEY_DOT:
			write_command_character('.');
			break;

		case KEY_SPACE:
			write_command_character(' ');
			break;

		case KEY_ENTER:
			if(is_command("PVTQRF", 0))//змейка
			{
				start_snake();
			}
			else if(is_command("CGBCJR", 0))//список
			{
				clear_text_screen_buffer();

				FAT_File_System file_system;

				open_FAT_File_System(&file_system, 0, &read_ATA_PIO_sector, &write_ATA_PIO_sector);
				list_FAT_directory(&file_system);
			}
			else if(is_command("CVJNHTNM", &argument))//смотреть
			{
				clear_text_screen_buffer();

				FAT_File_System file_system;

				open_FAT_File_System(&file_system, 0, &read_ATA_PIO_sector, &write_ATA_PIO_sector);

				Number16 file_name[12];

				i = 0;
				while(*argument)
				{
					file_name[i] = *argument;
					++argument;
					++i;
				}
				file_name[i] = '\0';

				FAT_Data file;
				if(open_FAT_file(&file_system, file_name, &file))
				{
					Byte sector[513];
					clear_bytes(sector, 513);
					read_FAT_sector(&file_system, file.cluster_number_low + (file.cluster_number_high << 16), sector, 0);
					sector[512] = '\0';
					print(sector);
				}
				else
				{
					print("AFQK YT YFQLTY");//файл не найден
				}
			}
			else if(is_command("CJPLFNM", &argument))//создать
			{
				clear_text_screen_buffer();

				FAT_File_System file_system;

				open_FAT_File_System(&file_system, 0, &read_ATA_PIO_sector, &write_ATA_PIO_sector);

				Number16 file_name[12];

				i = 0;
				while(*argument)
				{
					file_name[i] = *argument;
					++argument;
					++i;
				}
				file_name[i] = '\0';

				if(!create_FAT_file(&file_system, file_name, 0, 0))
					print("AFQK CEOTCNDETN");//файл существует
			}

			//for(i = 1; i < 80; ++i)
			//	set_text_screen_character(i, 24, '\0', CONSOLE_BLACK, CONSOLE_BLACK);
			
			//clear_text_screen_buffer();
			VESA_Mode_Info* video_mode = 0x7D00;
			draw_VESA_rectangle(video_mode, CHARACTER_WIDTH, 0, video_mode->width - CHARACTER_WIDTH, CHARACTER_HEIGHT, CONSOLE_BLACK);

			cursor_position = 0;

			clear_bytes(command, sizeof(command));
			break;
	}
}


void start_commander()
{
	initialize_default_text_screen();
	clear_text_screen();
	initialize_interrupts();
	initialize_keyboard();

	on_key_down = handle_commander_keydown;

	cursor_position = 0;
	clear_bytes(command, sizeof(command));

	//set_text_screen_character(0, 24, '>', CONSOLE_BLACK, CONSOLE_GREEN);
	draw_character(0, 0, '>');
}
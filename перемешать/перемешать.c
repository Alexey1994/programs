#include <system/file.c>
#include <system/memory.c>
#include <number/random.c>


Boolean is_current_script_name(Number16* name)
{
	Number16 current_script_name[] = {
		1087, //п
		1077, //е
		1088, //р
		1077, //е
		1084, //м
		1077, //е
		1096, //ш
		1072, //а
		1090, //т
		1100, //ь
		'.', //.
		'\0'
	};
	Number16* script_name = current_script_name;

	while(*name && *script_name)
	{
		if(*name != *script_name)
			return 0;

		++name;
		++script_name;
	}

	return 1;
}


Number number_of_files = 0;

void increment_number_of_files(Byte* context, Bit16* name, Boolean is_directory, Number size)
{
	if(is_directory || is_current_script_name(name))
		return;

	++number_of_files;
}

void calculate_number_of_files()
{
	find_files(L"./*", &increment_number_of_files, 0);
}


Number16** old_names;
Number16** names;

void set_number(Number16* name, Number number)
{
	Number32 number_copy;
	Number32 right_selector;

	number_copy    = number;
	right_selector = 1;

	for(;;)
	{
		number_copy /= 10;

		if(!number_copy)
			break;

		right_selector *= 10;
	}

	do
	{
		*name = number / right_selector % 10 + '0';
		++name;

		right_selector /= 10;
	}
	while(right_selector);

	*name = '\0';
}

void generate_random_names()
{
	Number     i;
	Number     j;
	Number16*  name;
	//Number16** names_copy;

	names = allocate_memory(number_of_files * sizeof(Number16*));
	//names_copy = allocate_memory(number_of_files * sizeof(Number16*));

	for(i = 0; i < number_of_files; ++i)
	{
		name = allocate_memory(256 * sizeof(Number16));
		set_number(name, i);
		names[i] = name;
		//names_copy[i] = name;
	}

	//for(;;)
	{
		for(i = 0; i < number_of_files; ++i)
		{
			for(j = 0; j < number_of_files - 1; ++j)
			{
				if(random_number() & 1)
				{
					Number16* tmp = names[j];
					names[j] = names[j + 1];
					names[j + 1] = tmp;
				}
			}
		}

		//for(i = 0; i < number_of_files; ++i)
		//	if(names[i] != names_copy[i])
		//		goto stop_generation;
	}
	//stop_generation:
	//free_memory(names_copy);

	//for(i = 0; i < number_of_files; ++i)
	//	print(_null_terminated_words, names[i], "\n");
}


void set_extension(Number16* old_name, Number16* new_name)
{
	Number16* extension;

	extension = old_name;
	
	while(*extension)
		++extension;

	for(;;)
	{
		if(extension == old_name)
			return;

		if(*extension == '.')
			break;

		--extension;
	}

	while(*new_name)
		++new_name;

	while(*extension)
	{
		*new_name = *extension;
		++new_name;
		++extension;
	}
	*new_name = '\0';
}

void copy_name(Number16* copy, Number16* name)
{
	for(;;)
	{
		*copy = *name;

		if(!*name)
			break;

		++name;
		++copy;
	}
}

Number file_index = 0;
void rename_next_file(Byte* context, Bit16* name, Boolean is_directory, Number size)
{
	if(is_directory || is_current_script_name(name))
		return;

	old_names[file_index] = allocate_memory(256 * sizeof(Number16));
	copy_name(old_names[file_index], name);

	set_extension(name, names[file_index]);
	
	++file_index;
}

void rename_files()
{
	calculate_number_of_files();
	generate_random_names();
	file_index = 0;
	old_names = allocate_memory(number_of_files * sizeof(Number16*));
	find_files(L"./*", &rename_next_file, 0);

	Number i;
	for(i = 0; i < number_of_files; ++i)
		MoveFileExW(old_names[i], names[i], MOVE_FILE_COPY_ALLOWED);
}


void main()
{
	rename_files();
}
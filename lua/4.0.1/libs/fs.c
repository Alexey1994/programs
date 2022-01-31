Number lua_get_drives(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 0)
	{
		lua_pop(lua_gettop(state), number_of_arguments);
		return 0;
	}

	lua_newtable(state);

	Number32 drives;
	Number   i;
	Number   current_disk_number;
	Byte     path[3];

	drives = GetLogicalDrives();
	path[1] = ':';
	path[2] = '\0';

	current_disk_number = 0;
	for(i = 0; i < sizeof(Number32) * 8; ++i)
	{
		if((drives >> i) & 1)
		{
			path[0] = 'A' + i;

			++current_disk_number;
			lua_pushnumber(state, current_disk_number);
			lua_pushstring(state, path);
			lua_settable(state, 1);
		}
	}

	return 1;
}


Number lua_get_files(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(lua_gettop(state), number_of_arguments);
		return 0;
	}

	Number16 search_path[256];

	to_wide_string(lua_tostring(state, 1), search_path, 256);
	lua_pop(state, 1);

	lua_newtable(state);

	Find_File_Information file_information;
	Byte*                 file_finder;
	Number                current_file_number;

	Byte utf8_string[512];

	current_file_number = 0;
	file_finder = FindFirstFileW(search_path, &file_information);
	if(file_finder)
	{
		++current_file_number;
		lua_pushnumber(state, current_file_number);
		lua_newtable(state);
		{
			lua_pushstring(state, "name");
			to_utf8_string(file_information.file_name, utf8_string);
			lua_pushstring(state, utf8_string);
			lua_settable(state, 3);

			lua_pushstring(state, "is_folder");
			if(file_information.attributes & DIRECTORY_FILE_ATTRIBUTE)
				lua_pushnumber(state, 1);
			else
				lua_pushnil(state);
			lua_settable(state, 3);
		}
		lua_settable(state, 1);

		//on_find(context, file_information.file_name, file_information.attributes & DIRECTORY_FILE_ATTRIBUTE, file_information.file_size_low);

		while(FindNextFileW(file_finder, &file_information))
		{
			//on_find(context, file_information.file_name, file_information.attributes & DIRECTORY_FILE_ATTRIBUTE, file_information.file_size_low);
		
			++current_file_number;
			lua_pushnumber(state, current_file_number);
			lua_newtable(state);
			{
				lua_pushstring(state, "name");
				to_utf8_string(file_information.file_name, utf8_string);
				lua_pushstring(state, utf8_string);
				lua_settable(state, 3);

				lua_pushstring(state, "is_folder");
				if(file_information.attributes & DIRECTORY_FILE_ATTRIBUTE)
					lua_pushnumber(state, 1);
				else
					lua_pushnil(state);
				lua_settable(state, 3);
			}
			lua_settable(state, 1);
		}
		FindClose(file_finder);
	}

	return 1;
}


void register_lua_fs(Lua_State* state)
{
	lua_register(state, "get_drives", &lua_get_drives);
	lua_register(state, "get_files", &lua_get_files);
}
#ifndef SQLITE_INCLUDED
#define SQLITE_INCLUDED


#include <types.c>


typedef enum
{
	SQLITE_OK  = 0
}
SQLite_Status;


import SQLite_Status sqlite3_open(Byte* path, Byte** data_base);
import SQLite_Status sqlite3_close(Byte* data_base);
import SQLite_Status sqlite3_exec(
	Byte* data_base,
	Byte* query,
	SQLite_Status (*result)(
		Byte* argument,
		Number number_of_values,
		Byte** values,
		Byte** columns
	),
	Byte* argument,
	Byte** error_message
);


#endif//SQLITE_INCLUDED


Number print_query_data(void* argument, Number number_of_values, Byte** values, Byte** columns)
{
	Number i;

	for(i = 0; i < number_of_values; ++i)	
		printf("%s = %s\n", columns[i], values[i]);

	return 0;
}


void main()
{
	Byte*         data_base;
	SQLite_Status status;

	status = sqlite3_open("a.db", &data_base);
	if(status)
	{
		printf("open database error\n");
		goto error;
	}

	//sqlite3_exec(data_base, "CREATE TABLE User(id INT, name TEXT)", &print_query_data, 0, 0);
	//sqlite3_exec(data_base, "INSERT INTO User(id, name) VALUES(1, 'Alexey')", &print_query_data, 0, 0);
	sqlite3_exec(data_base, "SELECT id, name FROM User", &print_query_data, 0, 0);

	sqlite3_close(data_base);

	return;

error:
	sqlite3_close(data_base);
}
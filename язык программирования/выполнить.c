#define DEBUG
#include "interpreter2.h"


N_32 main()
{
	Parser parser;

	BUFFER(main_type_name, 12)
	BUFFER(types, 100)
		add_bytes_in_buffer_end(&main_type_name, L"source", 12);
		if(parse_type(&main_type_name, &types))
		{
			printf("generate...\n");
			interprete(((Type**)types.data)[0]);
		}
		else
			printf("\n\nerror\n");
	END_BUFFER(types)
	END_BUFFER(main_type_name)
	printf("done\n");

	return 0;
}

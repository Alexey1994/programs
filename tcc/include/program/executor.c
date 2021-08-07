#ifndef PROGRAM_EXECUTOR_INCLUDED
#define PROGRAM_EXECUTOR_INCLUDED


#include <types.c>
#include "commands.c"


#define read_Byte_instruction() (program[instruction_index++])

#define read_Number_instruction() ((Number)read_Byte_instruction() +\
		(Number)(read_Byte_instruction() << 8) +\
		(Number)(read_Byte_instruction() << 16) +\
		(Number)(read_Byte_instruction() << 24))

#define push_data(number_of_bytes, data)\
{\
	Number i;\
	Number data_copy = (data);\
\
	for(i = 0; i < number_of_bytes; ++i)\
		((Byte*)local_data_index + *(Number*)stack_index)[i] = ((Byte*)&data_copy)[i];\
\
	*(Number*)(stack_index) += number_of_bytes;\
}

#define pop_data(number_of_bytes)\
{\
	Number i;\
\
	*(Number*)(stack_index) -= number_of_bytes;\
\
	data = 0;\
	for(i = 0; i < number_of_bytes; ++i)\
		((Byte*)&data)[i] = ((Byte*)local_data_index + *(Number*)(stack_index))[i];\
}


void execute(Byte* program, Number local_data_index, Number stack_index)
{
	Number instruction_index;

	Byte   instruction;
	Number number_of_bytes;
	Number size;
	Number data;
	Number address;
	Number i;
	Number a;

	Byte*  immediate_data;

	stack_index -= 4;
	*(Number*)(stack_index) = 0;
	instruction_index = 0;

	for(;;)
	{
		instruction = read_Byte_instruction();

		if(instruction == STOP_COMMAND)
			break;

		if(instruction < PUSH_COMMAND)
		{
			switch(instruction)
			{
				case STOP_COMMAND:
					return;

				case IMMEDIATE_DATA_COMMAND:
					goto copy_immediate;
					start_immediate:
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
					break;

					copy_immediate:
					asm("movl $start_immediate, %0" : "=a"(immediate_data) :);

					for(i = 0; i < 256; ++i)
						immediate_data[i] = 0x90;

					size = read_Byte_instruction();
					for(i = 0; i < size; ++i)
						immediate_data[i] = read_Byte_instruction();

					goto start_immediate;
					break;


				case JUMP_COMMAND:
					instruction_index = read_Number_instruction();
					break;


				case CALL_IMMEDIATE_COMMAND:
					size = *(Number*)(stack_index);

					stack_index -= 4;
					*(Number*)(stack_index) = local_data_index;

					local_data_index += size;

					stack_index -= 4;
					size = read_Number_instruction();
					*(Number*)(stack_index) = instruction_index;

					stack_index -= 4;
					*(Number*)(stack_index) = 0;

					instruction_index = size;
					break;

				case CALL_COMMAND:
					size = *(Number*)(stack_index);

					stack_index -= 4;
					*(Number*)(stack_index) = local_data_index;

					local_data_index += size;

					stack_index -= 4;
					*(Number*)(stack_index) = instruction_index;

					stack_index -= 4;
					*(Number*)(stack_index) = 0;

					pop_data(sizeof(Number));
					instruction_index = data;
					break;

				case BEGIN_SUBPROGRAM_COMMAND:
					break;

				case RETURN_COMMAND:
					size = *(Number*)(stack_index);
					stack_index += 4;

					instruction_index = *(Number*)(stack_index);
					stack_index += 4;

					local_data_index = *(Number*)(stack_index);
					stack_index += 4;
					break;

				/*case UNION_RETURN_COMMAND:
					size = *(Number*)(stack_index);
					stack_index += 4;

					instruction_index = *(Number*)(stack_index);
					stack_index += 4;

					local_data_index = *(Number*)(stack_index);
					stack_index += 4;

					*(Number*)(stack_index) += size;
					break;*/

				case SAVE_RETURNED_CONTEXT_COMMAND:
					*(Number*)(stack_index) += size;
					break;

				case SET_LOCAL_CONTEXT_COMMAND:
					break;

				case GET_LOCAL_CONTEXT_COMMAND:
					break;

				case SET_GLOBAL_CONTEXT_COMMAND:
					break;

				case GET_GLOBAL_CONTEXT_COMMAND:
					break;
			}
		}
		else
		{
			number_of_bytes = 1 << ((instruction - PUSH_COMMAND) / (GREATER_OR_EQUAL_COMMAND - PUSH_COMMAND));
			instruction = (instruction - PUSH_COMMAND) % (GREATER_OR_EQUAL_COMMAND - PUSH_COMMAND) + PUSH_COMMAND;

			switch(instruction)
			{
				case PUSH_COMMAND:
					data = 0;
					for(i = 0; i < number_of_bytes; ++i)
						((Byte*)&data)[i] = read_Byte_instruction();

					push_data(number_of_bytes, data);
					break;

				case POP_COMMAND:
					pop_data(number_of_bytes);
					break;

				case SET_LOCAL_COMMAND:
					address = read_Number_instruction();
					pop_data(number_of_bytes);

					for(i = 0; i < number_of_bytes; ++i)
						((Byte*)local_data_index + address)[i] = ((Byte*)&data)[i];
					break;

				case GET_LOCAL_COMMAND:
					push_data(number_of_bytes, *(Number*)(local_data_index + read_Number_instruction()));
					break;

				case LOAD_COMMAND:
					pop_data(sizeof(Number));
					push_data(number_of_bytes, *(Number*)(data));
					break;

				case STORE_COMMAND:
					pop_data(sizeof(Number));
					address = data;
					pop_data(number_of_bytes);

					for(i = 0; i < number_of_bytes; ++i)
						((Byte*)address)[i] = ((Byte*)&data)[i];
					break;


				case JUMP_IF_ZERO_COMMAND:
					address = read_Number_instruction();
					pop_data(number_of_bytes);
					if(!data)
						instruction_index = address;
					break;

				case JUMP_IF_NOT_ZERO_COMMAND:
					address = read_Number_instruction();
					pop_data(number_of_bytes);
					if(data)
						instruction_index = address;
					break;


				case NOT_COMMAND:
					pop_data(number_of_bytes);
					push_data(number_of_bytes, ~data);
					break;

				case NEGATE_COMMAND:
					pop_data(number_of_bytes);
					if(number_of_bytes == 1)
					{
						push_data(number_of_bytes, -(Number8)data);
					}
					else if(number_of_bytes == 2)
					{
						push_data(number_of_bytes, -(Number16)data);
					}
					else if(number_of_bytes == 4)
					{
						push_data(number_of_bytes, -(Number32)data);
					}
					else if(number_of_bytes == 8)
					{
						push_data(number_of_bytes, -(Number64)data);
					}
					break;


				case OR_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data | a);
					break;

				case AND_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data & a);
					break;

				case XOR_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data ^ a);
					break;


				case SHIFT_LEFT_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data << a);
					break;

				case SHIFT_RIGHT_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data >> a);
					break;


				case ADD_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data + a);
					break;

				case SUBTRACT_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data - a);
					break;

				case MULTIPLY_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data * a);
					break;

				case DIVIDE_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data / a);
					break;

				case MODULO_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data % a);
					break;


				case EQUAL_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data == a);
					break;

				case NOT_EQUAL_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data != a);
					break;

				case LESSER_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data < a);
					break;

				case LESSER_OR_EQUAL_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data <= a);
					break;

				case GREATER_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data > a);
					break;

				case GREATER_OR_EQUAL_COMMAND:
					pop_data(number_of_bytes);
					a = data;
					pop_data(number_of_bytes);
					push_data(number_of_bytes, data >= a);
					break;
			}
		}
	}
}


#endif//PROGRAM_EXECUTOR_INCLUDED

/*
#include <memory.c>


#define MEMORY_SIZE 60

Byte   memory[MEMORY_SIZE];
Number instruction_index;


#define WRITE_BYTE(command) {memory[instruction_index] = command; ++instruction_index;}
#define WRITE_INDEX(index) {WRITE_BYTE((index) % 256); WRITE_BYTE(((index) >> 8) % 256); WRITE_BYTE(((index) >> 16) % 256); WRITE_BYTE(((index) >> 24) % 256);}


#define COMMAND32(command) (PUSH_COMMAND + ((command) - PUSH_COMMAND) + (GREATER_OR_EQUAL_COMMAND - PUSH_COMMAND) * 2)
#define COMMAND16(command) (PUSH_COMMAND + ((command) - PUSH_COMMAND) + (GREATER_OR_EQUAL_COMMAND - PUSH_COMMAND) * 1)


#define STOP                    {WRITE_BYTE(STOP_COMMAND);}

#define IMMEDIATE_DATA(...)     {WRITE_BYTE(IMMEDIATE_DATA_COMMAND); Byte data[] = {__VA_ARGS__}; WRITE_BYTE(sizeof(data)); Number i; for(i = 0; i < sizeof(data); ++i) WRITE_BYTE(data[i]);}

#define JUMP(index)             {WRITE_BYTE(JUMP_COMMAND); WRITE_INDEX(index);}

#define CALL_IMMEDIATE(index)   {WRITE_BYTE(CALL_IMMEDIATE_COMMAND); WRITE_INDEX(index);}
#define CALL                    {WRITE_BYTE(CALL_COMMAND);}
#define BEGIN_SUBPROGRAM        {WRITE_BYTE(BEGIN_SUBPROGRAM_COMMAND);}
#define RETURN                  {WRITE_BYTE(RETURN_COMMAND);}
#define UNION_RETURN            {WRITE_BYTE(UNION_RETURN_COMMAND);}

#define SET_LOCAL_CONTEXT       {WRITE_BYTE(SET_LOCAL_CONTEXT_COMMAND);}
#define GET_LOCAL_CONTEXT       {WRITE_BYTE(GET_LOCAL_CONTEXT_COMMAND);}

#define SET_GLOBAL_CONTEXT      {WRITE_BYTE(SET_GLOBAL_CONTEXT_COMMAND);}
#define GET_GLOBAL_CONTEXT      {WRITE_BYTE(GET_GLOBAL_CONTEXT_COMMAND);}


#define PUSH(data)              {WRITE_BYTE(COMMAND32(PUSH_COMMAND)); WRITE_BYTE(data); WRITE_BYTE(data / 256); WRITE_BYTE(data / (256 * 256)); WRITE_BYTE(data / (256 * 256 * 256));}
#define POP                     {WRITE_BYTE(COMMAND32(POP_COMMAND));}

#define SET_LOCAL(index)        {WRITE_BYTE(COMMAND32(SET_LOCAL_COMMAND)); WRITE_INDEX(index);}
#define GET_LOCAL(index)        {WRITE_BYTE(COMMAND32(GET_LOCAL_COMMAND)); WRITE_INDEX(index);}

#define LOAD                    {WRITE_BYTE(COMMAND32(LOAD_COMMAND));}
#define STORE                   {WRITE_BYTE(COMMAND32(STORE_COMMAND));}

#define JUMP_IF_ZERO(index)     {WRITE_BYTE(COMMAND32(JUMP_IF_ZERO_COMMAND)); WRITE_INDEX(index);}
#define JUMP_IF_NOT_ZERO(index) {WRITE_BYTE(COMMAND32(JUMP_IF_NOT_ZERO_COMMAND)); WRITE_INDEX(index);}

#define NOT                     {WRITE_BYTE(COMMAND32(NOT_COMMAND));}
#define NEGATE                  {WRITE_BYTE(COMMAND32(NEGATE_COMMAND));}

#define OR                      {WRITE_BYTE(COMMAND32(OR_COMMAND));}
#define AND                     {WRITE_BYTE(COMMAND32(AND_COMMAND));}
#define XOR                     {WRITE_BYTE(COMMAND32(XOR_COMMAND));}

#define SHIFT_LEFT              {WRITE_BYTE(COMMAND32(SHIFT_LEFT_COMMAND));}
#define SHIFT_RIGHT             {WRITE_BYTE(COMMAND32(SHIFT_RIGHT_COMMAND));}

#define ADD                     {WRITE_BYTE(COMMAND32(ADD_COMMAND));}
#define SUBTRACT                {WRITE_BYTE(COMMAND32(SUBTRACT_COMMAND));}
#define MULTIPLY                {WRITE_BYTE(COMMAND32(MULTIPLY_COMMAND));}
#define DIVIDE                  {WRITE_BYTE(COMMAND32(DIVIDE_COMMAND));}
#define MODULO                  {WRITE_BYTE(COMMAND32(MODULO_COMMAND));}

#define EQUAL                   {WRITE_BYTE(COMMAND32(EQUAL_COMMAND));}
#define NOT_EQUAL               {WRITE_BYTE(COMMAND32(NOT_EQUAL_COMMAND));}
#define LESSER                  {WRITE_BYTE(COMMAND32(LESSER_COMMAND));}
#define LESSER_OR_EQUAL         {WRITE_BYTE(COMMAND32(LESSER_OR_EQUAL_COMMAND));}
#define GREATER                 {WRITE_BYTE(COMMAND32(GREATER_COMMAND));}
#define GREATER_OR_EQUAL        {WRITE_BYTE(COMMAND32(GREATER_OR_EQUAL_COMMAND));}


#define PUSH16(data)              {WRITE_BYTE(COMMAND16(PUSH_COMMAND)); WRITE_BYTE(data); WRITE_BYTE(data / 256);}
#define POP16                     {WRITE_BYTE(COMMAND16(POP_COMMAND));}

#define SET_LOCAL16(index)        {WRITE_BYTE(COMMAND16(SET_LOCAL_COMMAND)); WRITE_INDEX(index);}
#define GET_LOCAL16(index)        {WRITE_BYTE(COMMAND16(GET_LOCAL_COMMAND)); WRITE_INDEX(index);}

#define LOAD16                    {WRITE_BYTE(COMMAND16(LOAD_COMMAND));}
#define STORE16                   {WRITE_BYTE(COMMAND16(STORE_COMMAND));}

#define JUMP_IF_ZERO16(index)     {WRITE_BYTE(COMMAND16(JUMP_IF_ZERO_COMMAND)); WRITE_INDEX(index);}
#define JUMP_IF_NOT_ZERO16(index) {WRITE_BYTE(COMMAND16(JUMP_IF_NOT_ZERO_COMMAND)); WRITE_INDEX(index);}

#define NOT16                     {WRITE_BYTE(COMMAND16(NOT_COMMAND));}
#define NEGATE16                  {WRITE_BYTE(COMMAND16(NEGATE_COMMAND));}

#define OR16                      {WRITE_BYTE(COMMAND16(OR_COMMAND));}
#define AND16                     {WRITE_BYTE(COMMAND16(AND_COMMAND));}
#define XOR16                     {WRITE_BYTE(COMMAND16(XOR_COMMAND));}

#define SHIFT_LEFT16              {WRITE_BYTE(COMMAND16(SHIFT_LEFT_COMMAND));}
#define SHIFT_RIGHT16             {WRITE_BYTE(COMMAND16(SHIFT_RIGHT_COMMAND));}

#define ADD16                     {WRITE_BYTE(COMMAND16(ADD_COMMAND));}
#define SUBTRACT16                {WRITE_BYTE(COMMAND16(SUBTRACT_COMMAND));}
#define MULTIPLY16                {WRITE_BYTE(COMMAND16(MULTIPLY_COMMAND));}
#define DIVIDE16                  {WRITE_BYTE(COMMAND16(DIVIDE_COMMAND));}
#define MODULO16                  {WRITE_BYTE(COMMAND16(MODULO_COMMAND));}

#define EQUAL16                   {WRITE_BYTE(COMMAND16(EQUAL_COMMAND));}
#define NOT_EQUAL16               {WRITE_BYTE(COMMAND16(NOT_EQUAL_COMMAND));}
#define LESSER16                  {WRITE_BYTE(COMMAND16(LESSER_COMMAND));}
#define LESSER_OR_EQUAL16         {WRITE_BYTE(COMMAND16(LESSER_OR_EQUAL_COMMAND));}
#define GREATER16                 {WRITE_BYTE(COMMAND16(GREATER_COMMAND));}
#define GREATER_OR_EQUAL16        {WRITE_BYTE(COMMAND16(GREATER_OR_EQUAL_COMMAND));}


#define PUSH8(data)              {WRITE_BYTE(PUSH_COMMAND); WRITE_BYTE(data);}
#define POP8                     {WRITE_BYTE(POP_COMMAND);}

#define SET_LOCAL8(index)        {WRITE_BYTE(SET_LOCAL_COMMAND); WRITE_INDEX(index);}
#define GET_LOCAL8(index)        {WRITE_BYTE(GET_LOCAL_COMMAND); WRITE_INDEX(index);}

#define LOAD8                    {WRITE_BYTE(LOAD_COMMAND);}
#define STORE8                   {WRITE_BYTE(STORE_COMMAND);}

#define JUMP_IF_ZERO8(index)     {WRITE_BYTE(JUMP_IF_ZERO_COMMAND); WRITE_INDEX(index);}
#define JUMP_IF_NOT_ZERO8(index) {WRITE_BYTE(JUMP_IF_NOT_ZERO_COMMAND); WRITE_INDEX(index);}

#define NOT8                     {WRITE_BYTE(NOT_COMMAND);}
#define NEGATE8                  {WRITE_BYTE(NEGATE_COMMAND);}

#define OR8                      {WRITE_BYTE(OR_COMMAND);}
#define AND8                     {WRITE_BYTE(AND_COMMAND);}
#define XOR8                     {WRITE_BYTE(XOR_COMMAND);}

#define SHIFT_LEFT8              {WRITE_BYTE(SHIFT_LEFT_COMMAND);}
#define SHIFT_RIGHT8             {WRITE_BYTE(SHIFT_RIGHT_COMMAND);}

#define ADD8                     {WRITE_BYTE(ADD_COMMAND);}
#define SUBTRACT8                {WRITE_BYTE(SUBTRACT_COMMAND);}
#define MULTIPLY8                {WRITE_BYTE(MULTIPLY_COMMAND);}
#define DIVIDE8                  {WRITE_BYTE(DIVIDE_COMMAND);}
#define MODULO8                  {WRITE_BYTE(MODULO_COMMAND);}

#define EQUAL8                   {WRITE_BYTE(EQUAL_COMMAND);}
#define NOT_EQUAL8               {WRITE_BYTE(NOT_EQUAL_COMMAND);}
#define LESSER8                  {WRITE_BYTE(LESSER_COMMAND);}
#define LESSER_OR_EQUAL8         {WRITE_BYTE(LESSER_OR_EQUAL_COMMAND);}
#define GREATER8                 {WRITE_BYTE(GREATER_COMMAND);}
#define GREATER_OR_EQUAL8        {WRITE_BYTE(GREATER_OR_EQUAL_COMMAND);}
*/
/*
void write_program()
{

	//PUSH(3)
	//PUSH(7)
	//GET_LOCAL(0) PUSH(1) SUBTRACT SET_LOCAL(0)
	//GET_LOCAL(0)
	//JUMP_IF_NOT_ZERO(5)

	//PUSH(1)
	//LOAD

	//IMMEDIATE_DATA(0xC9)
	//JUMP(6)
	//CALL_IMMEDIATE(0)

	//PUSH(3)
	//CALL_IMMEDIATE(11)
	//STOP
	//BEGIN_SUBPROGRAM
	//GET_LOCAL(-4)
	//RETURN


	//PUSH(6)
	//CALL	
	//BEGIN_SUBPROGRAM
	//RETURN

	//PUSH(1)
	//PUSH(2)
	//STORE

	//IMMEDIATE_DATA(0xEB, 0xFE);
	//IMMEDIATE_DATA(0x90);

	//out_8(0x3D4, 0x0A);
	//mov edx,0x3d4
	//mov al,0xa
	//out dx,al
	//IMMEDIATE_DATA(0xBA, 0xD4, 0x03, 0x00, 0x00, 0xB0, 0x0A, 0xEE)
	//out_8(0x3D5, 0x20);
	//mov edx,0x3d5
	//mov al,0x20
	//out dx,al
	//IMMEDIATE_DATA(0xBA, 0xD5, 0x03, 0x00, 0x00, 0xB0, 0x20, 0xEE)

	STOP
}


void print_program(Number program_size)
{
	Number i;\
	for(i = 0; i < program_size; ++i)
		print(_Number, memory[i], "\n", 0);
}

void print_memory(Number program_size)
{
	Number i;
	for(i = program_size; i + 4 <= MEMORY_SIZE; i += 4)
		print(_Number, *((Number*)(memory + i)), "\n", 0);
}


void main()
{
	Number program_size;

	clear_bytes(memory, MEMORY_SIZE);
	
	instruction_index = 0;
	write_program();

	if(instruction_index % 4)
		program_size = (instruction_index / 4 + 1) * 4;
	else
		program_size = instruction_index;

	execute(memory, memory + program_size, memory + MEMORY_SIZE);

	//print_program(program_size);
	//print("\n", 0);
	print_memory(program_size);

	print("\nprogram size: ", _Number, program_size, " bytes\n", 0);
}*/
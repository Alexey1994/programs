#ifndef PROGRAM_COMPILER_INCLUDED
#define PROGRAM_COMPILER_INCLUDED


#include <types.c>
#include <data-structures/stack.c>
#include "../commands.c"


#define add_x86_byte(byte)\
{\
	Byte out_byte = byte;\
	add_bytes_in_stack(x86_program, &out_byte, 1);\
}


#define pop_x86_4_bytes_in_a()\
{\
	/*mov ebx, [esp]*/\
	add_x86_byte(0x8B);\
	add_x86_byte(0x1C);\
	add_x86_byte(0x24);\
	/*mov eax, [ebp + ebx + 0]*/\
	add_x86_byte(0x8B);\
	add_x86_byte(0x44);\
	add_x86_byte(0x1D);\
	add_x86_byte(0);\
	/*sub [esp], 4*/\
	add_x86_byte(0x83);\
	add_x86_byte(0x2C);\
	add_x86_byte(0x24);\
	add_x86_byte(4);\
}

#define pop_x86_4_bytes_in_b()\
{\
	/*mov ebx, [esp]*/\
	add_x86_byte(0x8B);\
	add_x86_byte(0x1C);\
	add_x86_byte(0x24);\
	/*mov ecx, [ebp + ebx + 0]*/\
	add_x86_byte(0x8B);\
	add_x86_byte(0x4C);\
	add_x86_byte(0x1D);\
	add_x86_byte(0);\
	/*sub [esp], 4*/\
	add_x86_byte(0x83);\
	add_x86_byte(0x2C);\
	add_x86_byte(0x24);\
	add_x86_byte(4);\
}

#define push_x86_4_bytes()\
{\
	/*mov ebx, [esp]*/\
	add_x86_byte(0x8B);\
	add_x86_byte(0x1C);\
	add_x86_byte(0x24);\
	/*mov [ebp + ebx + 0], eax*/\
	add_x86_byte(0x89);\
	add_x86_byte(0x44);\
	add_x86_byte(0x1D);\
	add_x86_byte(0);\
	/*add [esp], 4*/\
	add_x86_byte(0x83);\
	add_x86_byte(0x04);\
	add_x86_byte(0x24);\
	add_x86_byte(4);\
}


void compile_in_x86_32(Stack* x86_program, Byte* program, Number program_size)
{
	Byte   instruction;
	Number number_of_bytes;
	Number size;
	Number i;

	initialize_stack(x86_program, 128);

	for(i = 0; i < program_size;)
	{
		instruction = program[i];
		++i;

		if(instruction == STOP_COMMAND)
			continue;

		if(instruction < PUSH_COMMAND)
		{
			switch(instruction)
			{
				case IMMEDIATE_DATA_COMMAND:
					size = program[i];
					++i;

					add_bytes_in_stack(x86_program, program + i, size);
					i += size;
					break;


				case JUMP_COMMAND:
					add_x86_byte(0xE9);
					
					size = *(Number*)(program + i);
					i += 4;
					add_bytes_in_stack(x86_program, &size, sizeof(size));
					break;


				case CALL_IMMEDIATE_COMMAND:
					//call size
					add_x86_byte(0xE8);
					size = *(Number*)(program + i);
					i += 4;
					add_bytes_in_stack(x86_program, &size, sizeof(size));
					break;

				case CALL_COMMAND:
					pop_x86_4_bytes_in_a();

					//call eax
					add_x86_byte(0xFF);
					add_x86_byte(0xD0);
					break;

				case BEGIN_SUBPROGRAM_COMMAND:
					//push ebp
					add_x86_byte(0x55);
					//add ebp, [esp + 8]
					add_x86_byte(0x03);
					add_x86_byte(0x6C);
					add_x86_byte(0x24);
					add_x86_byte(8);
					//push 0
					add_x86_byte(0x68);
					add_x86_byte(0);
					add_x86_byte(0);
					add_x86_byte(0);
					add_x86_byte(0);
					break;

				case RETURN_COMMAND:
					//pop eax
					add_x86_byte(0x58);
					//pop ebp
					add_x86_byte(0x5D);
					//ret
					add_x86_byte(0xC3);
					break;

				/*case UNION_RETURN_COMMAND:
					//pop eax
					add_x86_byte(0x58);
					//pop ebp
					add_x86_byte(0x5D);
					//add [esp + 4], eax
					add_x86_byte(0x01);
					add_x86_byte(0x44);
					add_x86_byte(0x24);
					add_x86_byte(4);
					//ret
					add_x86_byte(0xC3);
					break;*/

				case SAVE_RETURNED_CONTEXT_COMMAND:
					//add ebp, eax
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
					if(number_of_bytes == 4)
					{
						//mov ebx, [esp]
						add_x86_byte(0x8B);
						add_x86_byte(0x1C);
						add_x86_byte(0x24);
						//mov [ebp + ebx + 0], size
						add_x86_byte(0xC7);
						add_x86_byte(0x44);
						add_x86_byte(0x1D);
						add_x86_byte(0);
						size = *(Number*)(program + i);
						i += 4;
						add_bytes_in_stack(x86_program, &size, sizeof(size));
						//add [esp], 4
						add_x86_byte(0x83);
						add_x86_byte(0x04);
						add_x86_byte(0x24);
						add_x86_byte(4);
					}

					break;

				case POP_COMMAND:
					if(number_of_bytes == 4)
					{
						//sub [esp], 4
						add_x86_byte(0x83);
						add_x86_byte(0x2C);
						add_x86_byte(0x24);
						add_x86_byte(4);
					}
					break;

				case SET_LOCAL_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();

						/*mov ebx, [esp]*/
						add_x86_byte(0x8B);
						add_x86_byte(0x1C);
						add_x86_byte(0x24);

						/*mov [ebp + ebx + size], ecx*/
						add_x86_byte(0x89);
						add_x86_byte(0x8C);
						add_x86_byte(0x1D);
						size = *(Number*)(program + i);
						i += 4;
						add_bytes_in_stack(x86_program, &size, sizeof(size));
					}
					break;

				case GET_LOCAL_COMMAND:
					if(number_of_bytes == 4)
					{
						/*mov ebx, [esp]*/
						add_x86_byte(0x8B);
						add_x86_byte(0x1C);
						add_x86_byte(0x24);

						/*mov eax, [ebp + ebx + size]*/
						add_x86_byte(0x8B);
						add_x86_byte(0x84);
						add_x86_byte(0x1D);
						size = *(Number*)(program + i);
						i += 4;
						add_bytes_in_stack(x86_program, &size, sizeof(size));

						push_x86_4_bytes();
					}
					break;

				case LOAD_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_a();
						
						//mov eax, [eax]
						add_x86_byte(0x8B);
						add_x86_byte(0x00);

						push_x86_4_bytes();
					}
					break;

				case STORE_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();
						
						//mov [ecx], eax
						add_x86_byte(0x89);
						add_x86_byte(0x01);

						push_x86_4_bytes();
					}
					break;


				case JUMP_IF_ZERO_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_a();
						
						//cmp eax, 0
						add_x86_byte(0x3D);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						//jz size
						add_x86_byte(0x0F);
						add_x86_byte(0x84);
						size = *(Number*)(program + i);
						i += 4;
						add_bytes_in_stack(x86_program, &size, sizeof(size));
					}
					break;

				case JUMP_IF_NOT_ZERO_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_a();
						
						//cmp eax, 0
						add_x86_byte(0x3D);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						//jnz size
						add_x86_byte(0x0F);
						add_x86_byte(0x85);
						size = *(Number*)(program + i);
						i += 4;
						add_bytes_in_stack(x86_program, &size, sizeof(size));
					}
					break;


				case NOT_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_a();
						
						//not eax
						add_x86_byte(0xF7);
						add_x86_byte(0xD0);

						push_x86_4_bytes();
					}
					break;

				case NEGATE_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_a();
						
						//neg eax
						add_x86_byte(0xF7);
						add_x86_byte(0xD8);

						push_x86_4_bytes();
					}
					break;


				case OR_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();
						
						//or eax, ecx
						add_x86_byte(0x09);
						add_x86_byte(0xC8);

						push_x86_4_bytes();
					}
					break;

				case AND_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();
						
						//and eax, ecx
						add_x86_byte(0x21);
						add_x86_byte(0xC8);

						push_x86_4_bytes();
					}
					break;

				case XOR_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();
						
						//xor eax, ecx
						add_x86_byte(0x31);
						add_x86_byte(0xC8);

						push_x86_4_bytes();
					}
					break;


				case SHIFT_LEFT_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();
						
						//shl eax, cl
						add_x86_byte(0xD3);
						add_x86_byte(0xE0);

						push_x86_4_bytes();
					}
					break;

				case SHIFT_RIGHT_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();
						
						//shr eax, cl
						add_x86_byte(0xD3);
						add_x86_byte(0xE8);

						push_x86_4_bytes();
					}
					break;


				case ADD_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();
						
						//add eax, ecx
						add_x86_byte(0x01);
						add_x86_byte(0xC8);

						push_x86_4_bytes();
					}
					break;

				case SUBTRACT_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();

						//sub eax, ecx
						add_x86_byte(0x29);
						add_x86_byte(0xC8);

						push_x86_4_bytes();
					}
					break;

				case MULTIPLY_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();

						//mul ecx
						add_x86_byte(0xF7);
						add_x86_byte(0xE1);

						push_x86_4_bytes();
					}
					break;

				case DIVIDE_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();
						
						//div ecx
						add_x86_byte(0xF7);
						add_x86_byte(0xF1);

						push_x86_4_bytes();
					}
					break;

				case MODULO_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();
						
						//div ecx
						add_x86_byte(0xF7);
						add_x86_byte(0xF1);
						//mov eax, edx
						add_x86_byte(0x89);
						add_x86_byte(0xD0);

						push_x86_4_bytes();
					}
					break;


				case EQUAL_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();

						//cmp eax, ecx
						add_x86_byte(0x39);
						add_x86_byte(0xC8);
						//je a
						add_x86_byte(0x74);
						add_x86_byte(0x04);
						//xor eax, eax
						add_x86_byte(0x31);
						add_x86_byte(0xC0);
						//jmp b
						add_x86_byte(0xEB);
						add_x86_byte(0x05);
						//a: mov eax, 1
						add_x86_byte(0xB8);
						add_x86_byte(0x01);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						//b:

						push_x86_4_bytes();
					}
					break;

				case NOT_EQUAL_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();

						//cmp eax, ecx
						add_x86_byte(0x39);
						add_x86_byte(0xC8);
						//jne a
						add_x86_byte(0x75);
						add_x86_byte(0x04);
						//xor eax, eax
						add_x86_byte(0x31);
						add_x86_byte(0xC0);
						//jmp b
						add_x86_byte(0xEB);
						add_x86_byte(0x05);
						//a: mov eax, 1
						add_x86_byte(0xB8);
						add_x86_byte(0x01);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						//b:

						push_x86_4_bytes();
					}
					break;

				case LESSER_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();

						//cmp eax, ecx
						add_x86_byte(0x39);
						add_x86_byte(0xC8);
						//jl a
						add_x86_byte(0x7C);
						add_x86_byte(0x04);
						//xor eax, eax
						add_x86_byte(0x31);
						add_x86_byte(0xC0);
						//jmp b
						add_x86_byte(0xEB);
						add_x86_byte(0x05);
						//a: mov eax, 1
						add_x86_byte(0xB8);
						add_x86_byte(0x01);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						//b:

						push_x86_4_bytes();
					}
					break;

				case LESSER_OR_EQUAL_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();

						//cmp eax, ecx
						add_x86_byte(0x39);
						add_x86_byte(0xC8);
						//jle a
						add_x86_byte(0x7E);
						add_x86_byte(0x04);
						//xor eax, eax
						add_x86_byte(0x31);
						add_x86_byte(0xC0);
						//jmp b
						add_x86_byte(0xEB);
						add_x86_byte(0x05);
						//a: mov eax, 1
						add_x86_byte(0xB8);
						add_x86_byte(0x01);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						//b:

						push_x86_4_bytes();
					}
					break;

				case GREATER_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();

						//cmp eax, ecx
						add_x86_byte(0x39);
						add_x86_byte(0xC8);
						//jg a
						add_x86_byte(0x7F);
						add_x86_byte(0x04);
						//xor eax, eax
						add_x86_byte(0x31);
						add_x86_byte(0xC0);
						//jmp b
						add_x86_byte(0xEB);
						add_x86_byte(0x05);
						//a: mov eax, 1
						add_x86_byte(0xB8);
						add_x86_byte(0x01);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						//b:

						push_x86_4_bytes();
					}
					break;

				case GREATER_OR_EQUAL_COMMAND:
					if(number_of_bytes == 4)
					{
						pop_x86_4_bytes_in_b();
						pop_x86_4_bytes_in_a();

						//cmp eax, ecx
						add_x86_byte(0x39);
						add_x86_byte(0xC8);
						//jge a
						add_x86_byte(0x7D);
						add_x86_byte(0x04);
						//xor eax, eax
						add_x86_byte(0x31);
						add_x86_byte(0xC0);
						//jmp b
						add_x86_byte(0xEB);
						add_x86_byte(0x05);
						//a: mov eax, 1
						add_x86_byte(0xB8);
						add_x86_byte(0x01);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						add_x86_byte(0x00);
						//b:

						push_x86_4_bytes();
					}
					break;
			}
		}
	}
}


#endif//PROGRAM_COMPILER_INCLUDED


#include "../executor.c"
#include <system/file.c>


void main()
{
	Number program_size;
	Stack  x86_program;

	clear_bytes(memory, MEMORY_SIZE);
	
	instruction_index = 0;
	write_program();

	if(instruction_index % 4)
		program_size = (instruction_index / 4 + 1) * 4;
	else
		program_size = instruction_index;

	compile_in_x86_32(&x86_program, memory, program_size);
	printf("\nprogram size: %d bytes\n", program_size);
	printf("x86 program size: %d bytes\n\n", x86_program.size);

	create_file(L"program", x86_program.data, x86_program.size);
	system("ndisasm -b 32 program");
}

#include <system.c>
#include <system/file/file.h>


/*
http://sparksandflames.com/files/x86InstructionChart.html
// E [data register]
// G data register
// O [number]
// I number
// S segment register
// M number16: memory address
// A number16: far address
// X number16: DS:SI address
// Y number16: ES:DI address
// F flags register
// J number8: short jump address


http://www.avprog.narod.ru/progs/emu8086/8086_instruction_set.html#:~:text=%D0%A2%D0%B8%D0%BF%D1%8B%20%D0%BE%D0%BF%D0%B5%D1%80%D0%B0%D0%BD%D0%B4%D0%BE%D0%B2%3A,%D1%82%D0%BE%D0%BB%D1%8C%D0%BA%D0%BE%20%D0%BA%D0%B0%D0%BA%20%D0%B2%D1%82%D0%BE%D1%80%D0%BE%D0%B9%20%D0%BE%D0%BF%D0%B5%D1%80%D0%B0%D0%BD%D0%B4%3A%20CS.
*/

typedef enum
{
	BITS_8,
	BITS_16,
	BITS_32
}
Bit_Depth;


typedef struct
{
	//Number32 AX; 0
	//Number32 CX; 1
	//Number32 DX; 2
	//Number32 BX; 3
	//Number32 SP; 4
	//Number32 BP; 5
	//Number32 SI; 6
	//Number32 DI; 7
	Bit32 data_registers[8];
	//Number32 ES; 0
	//Number32 CS; 1
	//Number32 SS; 2
	//Number32 DS; 3
	//Number32 FS; 4
	//Number32 GS; 5
	Bit16 segment_registers[8];
	Bit32 IP;
	Bit32 CF;
	Byte* memory;
}
Processor;


Byte read_next_processor_byte(Processor* processor)
{
	Byte byte;
	byte = processor->memory[processor->IP];
	++processor->IP;
	return byte;
}


Number8 read_next_processor_Number8(Processor* processor)
{
	return read_next_processor_byte(processor);
}


Number16 read_next_processor_Number16(Processor* processor)
{
	Number16 number;
	number = read_next_processor_byte(processor);
	number += read_next_processor_byte(processor) << 8;
	return number;
}


Number32 execute_getting_G_operand(Processor* processor, Byte register_number, Bit_Depth depth)
{
	switch(depth)
	{
		case BITS_8:
			switch(register_number)
			{
				case 0: printf("AL"); return processor->data_registers;
				case 1: printf("CL"); return processor->data_registers + 1;
				case 2: printf("DL"); return processor->data_registers + 2;
				case 3: printf("BL"); return processor->data_registers + 3;
				case 4: printf("AH"); return (Byte*)processor->data_registers + 1;
				case 5: printf("CH"); return (Byte*)(processor->data_registers + 1) + 1;
				case 6: printf("DH"); return (Byte*)(processor->data_registers + 2) + 1;
				case 7: printf("BH"); return (Byte*)(processor->data_registers + 3) + 1;
			}
			break;

		case BITS_16:
			switch(register_number)
			{
				case 0: printf("AX"); return processor->data_registers;
				case 1: printf("CX"); return processor->data_registers + 1;
				case 2: printf("DX"); return processor->data_registers + 2;
				case 3: printf("BX"); return processor->data_registers + 3;
				case 4: printf("SP"); return processor->data_registers + 4;
				case 5: printf("BP"); return processor->data_registers + 5;
				case 6: printf("SI"); return processor->data_registers + 6;
				case 7: printf("DI"); return processor->data_registers + 7;
			}
			break;

		case BITS_32:
			switch(register_number)
			{
				case 0: printf("EAX"); return processor->data_registers;
				case 1: printf("ECX"); return processor->data_registers + 1;
				case 2: printf("EDX"); return processor->data_registers + 2;
				case 3: printf("EBX"); return processor->data_registers + 3;
				case 4: printf("ESP"); return processor->data_registers + 4;
				case 5: printf("EBP"); return processor->data_registers + 5;
				case 6: printf("ESI"); return processor->data_registers + 6;
				case 7: printf("EDI"); return processor->data_registers + 7;
			}
			break;
	}
}


Number32 execute_getting_E_operand(Processor* processor, Byte config, Bit_Depth depth)
{
	Byte     mod;
	Byte     opcode;
	Number16 offset;

	mod = (config >> 6) & 0b00000011;
	opcode = config & 0b00000111;

	switch(mod)
	{
		case 0:
			switch(opcode)
			{
				case 0: printf("[BX+SI]"); return processor->memory + processor->data_registers[3] + processor->data_registers[6];
				case 1: printf("[BX+DI]"); return processor->memory + processor->data_registers[3] + processor->data_registers[7];
				case 2: printf("[BP+SI]"); return processor->memory + processor->data_registers[5] + processor->data_registers[6];
				case 3: printf("[BP+DI]"); return processor->memory + processor->data_registers[5] + processor->data_registers[7];
				case 4: printf("[SI]"); return processor->memory + processor->data_registers[6];
				case 5: printf("[DI]"); return processor->memory + processor->data_registers[7];
				case 6: 
					offset = read_next_processor_Number16(processor);
					printf("[%d]", offset);
					return processor->memory + offset;
				case 7: printf("[BX]"); return processor->memory + processor->data_registers[3];
			}
			break;

		case 1:
			offset = read_next_processor_byte(processor);

			switch(opcode)
			{
				case 0: printf("[BX+SI+%d]", offset); return processor->memory + processor->data_registers[3] + processor->data_registers[6] + offset;
				case 1: printf("[BX+DI+%d]", offset); return processor->memory + processor->data_registers[3] + processor->data_registers[7] + offset;
				case 2: printf("[BP+SI+%d]", offset); return processor->memory + processor->data_registers[5] + processor->data_registers[6] + offset;
				case 3: printf("[BP+DI+%d]", offset); return processor->memory + processor->data_registers[5] + processor->data_registers[7] + offset;
				case 4: printf("[SI+%d]", offset); return processor->memory + processor->data_registers[6] + offset;
				case 5: printf("[DI+%d]", offset); return processor->memory + processor->data_registers[7] + offset;
				case 6: printf("[BP+%d]", offset); return processor->memory + processor->data_registers[5] + offset;
				case 7: printf("[BX+%d]", offset); return processor->memory + processor->data_registers[3] + offset;
			}
			break;

		case 2:
			offset = read_next_processor_Number16(processor);

			switch(opcode)
			{
				case 0: printf("[BX+SI+%d]", offset); return processor->memory + processor->data_registers[3] + processor->data_registers[6] + offset;
				case 1: printf("[BX+DI+%d]", offset); return processor->memory + processor->data_registers[3] + processor->data_registers[7] + offset;
				case 2: printf("[BP+SI+%d]", offset); return processor->memory + processor->data_registers[5] + processor->data_registers[6] + offset;
				case 3: printf("[BP+DI+%d]", offset); return processor->memory + processor->data_registers[5] + processor->data_registers[7] + offset;
				case 4: printf("[SI+%d]", offset); return processor->memory + processor->data_registers[6] + offset;
				case 5: printf("[DI+%d]", offset); return processor->memory + processor->data_registers[7] + offset;
				case 6: printf("[BP+%d]", offset); return processor->memory + processor->data_registers[5] + offset;
				case 7: printf("[BX+%d]", offset); return processor->memory + processor->data_registers[3] + offset;
			}
			break;

		case 3:
			return execute_getting_G_operand(processor, opcode, depth);
	}
}


void execute_operation_00(Processor* processor)
{
	Byte     config;
	Number32 source;
	Number32 destination;

	printf("ADD ");
	config = read_next_processor_byte(processor);
	destination = execute_getting_E_operand(processor, config, BITS_8);
	printf(", ");
	source = execute_getting_G_operand(processor, (config >> 3) & 0b00000111, BITS_8);
	*((Number8*)destination) += *((Number8*)source);
}
void execute_operation_01(Processor* processor)
{
	Byte     config;
	Number32 source;
	Number32 destination;

	printf("ADD ");
	config = read_next_processor_byte(processor);
	destination = execute_getting_E_operand(processor, config, BITS_16);
	printf(", ");
	source = execute_getting_G_operand(processor, (config >> 3) & 0b00000111, BITS_16);
	*((Number16*)destination) += *((Number16*)source);
}
void execute_operation_02(Processor* processor)
{
	Byte     config;
	Number32 source;
	Number32 destination;

	printf("ADD ");
	config = read_next_processor_byte(processor);
	destination = execute_getting_G_operand(processor, (config >> 3) & 0b00000111, BITS_8);
	printf(", ");
	source = execute_getting_E_operand(processor, config, BITS_8);
	*((Number8*)destination) += *((Number8*)source);
}
void execute_operation_03(Processor* processor)
{
	Byte     config;
	Number32 source;
	Number32 destination;

	printf("ADD ");
	config = read_next_processor_byte(processor);
	destination = execute_getting_G_operand(processor, (config >> 3) & 0b00000111, BITS_16);
	printf(", ");
	source = execute_getting_E_operand(processor, config, BITS_16);
	*((Number16*)destination) += *((Number16*)source);
}
void execute_operation_04(Processor* processor)
{
	Number8 number;

	number = read_next_processor_Number8(processor);
	printf("ADD AL, %d", number);
	number += (Number8)processor->data_registers[0];
	*((Number8*)processor->data_registers) = number;
}
void execute_operation_05(Processor* processor)
{
	Number16 number;

	number = read_next_processor_Number16(processor);
	printf("ADD AX, %d", number);
	number += (Number16)processor->data_registers[0];
	*((Number16*)processor->data_registers) = number;
}

void execute_operation_06(Processor* processor)
{
	Number32 esp;

	printf("PUSH ES");

	esp = *((Number16*)processor->data_registers + 4);
	*((Number16*)processor->data_registers + 4) = esp - 2;
	*((Number16*)(processor->memory + esp)) = processor->segment_registers[0];
}
void execute_operation_07(Processor* processor)
{
	Number32 esp;

	printf("POP ES");

	esp = *((Number16*)processor->data_registers + 4);
	*((Number16*)processor->data_registers + 4) = esp + 2;
	processor->segment_registers[0] = *((Number16*)(processor->memory + esp));
}


static void(*operation_executors[256])(Processor* processor) = {
	&execute_operation_00,
	&execute_operation_01,
	&execute_operation_02,
	&execute_operation_03,
	&execute_operation_04,
	&execute_operation_05,
	&execute_operation_06,
	&execute_operation_07
};


void emulate(Byte* code, Number32 code_length)
{
	void(*execute_operation)(Processor* processor);
	Processor processor;

	processor.memory = code;
	processor.IP = 0;

	Number32 i;
	for(i = 0; i < code_length; ++i)
	{
		execute_operation = operation_executors[processor.memory[processor.IP]];
		++processor.IP;
		execute_operation(&processor);
		printf("\n");
	}
}

/*
void test_disasm()
{
	Byte code[512];
	File file = open_file(L"c");
	read_from_file(file, 0, code, 512);
	close_file(file);
	emulate_x86(code, 100);
}


void test()
{
	Byte code[] = {
		0x33, 0xDB
	};
	emulate_x86(code, 1);
}*/


void test2()
{
	Byte code[] = {
		0x05, 1, 2
	};
	emulate(code, 1);
}


Number32 main()
{
	//test_disasm();
	//test();
	test2();
	return 0;
}
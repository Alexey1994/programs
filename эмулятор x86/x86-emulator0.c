#include <system.c>
#include <system/file.c>


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
	//Number32 AX;
	//Number32 CX;
	//Number32 DX;
	//Number32 BX;
	//Number32 SP;
	//Number32 BP;
	//Number32 SI;
	//Number32 DI;
	Bit32 data_registers[8];
	//Number32 ES;
	//Number32 CS;
	//Number32 SS;
	//Number32 DS;
	//Number32 FS;
	//Number32 GS;
	Bit32 segment_registers[8];
	Bit32 IP;
	Bit32 CF;
	Byte* memory;
}
Processor;


void print_data_register_name(Byte register_number, Bit_Depth depth)
{
	switch(depth)
	{
		case BITS_8:
			switch(register_number)
			{
				case 0: printf("AL"); break;
				case 1: printf("CL"); break;
				case 2: printf("DL"); break;
				case 3: printf("BL"); break;
				case 4: printf("AH"); break;
				case 5: printf("CH"); break;
				case 6: printf("DH"); break;
				case 7: printf("BH"); break;
			}
			break;

		case BITS_16:
			switch(register_number)
			{
				case 0: printf("AX"); break;
				case 1: printf("CX"); break;
				case 2: printf("DX"); break;
				case 3: printf("BX"); break;
				case 4: printf("SP"); break;
				case 5: printf("BP"); break;
				case 6: printf("SI"); break;
				case 7: printf("DI"); break;
			}
			break;

		case BITS_32:
			switch(register_number)
			{
				case 0: printf("EAX"); break;
				case 1: printf("ECX"); break;
				case 2: printf("EDX"); break;
				case 3: printf("EBX"); break;
				case 4: printf("ESP"); break;
				case 5: printf("EBP"); break;
				case 6: printf("ESI"); break;
				case 7: printf("EDI"); break;
			}
			break;
	}
}


void print_segment_register_name(Byte register_number)
{
	switch(register_number)
	{
		case 0: printf("ES"); break;
		case 1: printf("CS"); break;
		case 2: printf("SS"); break;
		case 3: printf("DS"); break;
		case 4: printf("FS"); break;
		case 5: printf("GS"); break;
	}
}


void print_memory16_command(Byte** code, Byte config, Bit_Depth depth)
{
	Byte     mod;
	Byte     opcode;
	Number16 offset;

	mod = (config >> 6) & 0b00000011;
	opcode = config & 0b00000111;

	switch(mod)
	{
		case 0:
			printf("[");
			switch(opcode)
			{
				case 0: printf("BX+SI"); break;
				case 1: printf("BX+DI"); break;
				case 2: printf("BP+SI"); break;
				case 3: printf("BP+DI"); break;
				case 4: printf("SI"); break;
				case 5: printf("DI"); break;
				case 6: 
					offset = **code; ++*code;
					offset += **code << 8; ++*code;
					printf("%d", offset);
					break;
				case 7: printf("BX"); break;
			}
			printf("]");
			break;

		case 1:
			offset = **code; ++*code;
			printf("[");
			switch(opcode)
			{
				case 0: printf("BX+SI+%d", offset); break;
				case 1: printf("BX+DI+%d", offset); break;
				case 2: printf("BP+SI+%d", offset); break;
				case 3: printf("BP+DI+%d", offset); break;
				case 4: printf("SI+%d", offset); break;
				case 5: printf("DI+%d", offset); break;
				case 6: printf("BP+%d", offset); break;
				case 7: printf("BX+%d", offset); break;
			}
			printf("]");
			break;

		case 2:
			offset = **code; ++*code;
			offset += **code << 8; ++*code;
			printf("[");
			switch(opcode)
			{
				case 0: printf("BX+SI+%d", offset); break;
				case 1: printf("BX+DI+%d", offset); break;
				case 2: printf("BP+SI+%d", offset); break;
				case 3: printf("BP+DI+%d", offset); break;
				case 4: printf("SI+%d", offset); break;
				case 5: printf("DI+%d", offset); break;
				case 6: printf("BP+%d", offset); break;
				case 7: printf("BX+%d", offset); break;
			}
			printf("]");
			break;

		case 3:
			print_data_register_name(opcode, depth);
			break;
	}
}


typedef enum
{
	X86_OPERAND_DATA_REGISTER_BYTE = 0,
	X86_OPERAND_DATA_REGISTER_WORD = 1,
	X86_OPERAND_REGISTER_DATA_BYTE = 2,
	X86_OPERAND_REGISTER_DATA_WORD = 3,
	X86_OPERAND_REGISTER_NUMBER_BYTE = 4,
	X86_OPERAND_REGISTER_NUMBER_WORD = 5,
	X86_OPERAND_DATA_NUMBER_BYTE = 6,
	X86_OPERAND_DATA_NUMBER_WORD = 7,
	X86_OPERAND_DATA_SEGMENT = 8,
	X86_OPERAND_SEGMENT_DATA = 9
}
X86_Operand_Index;


void execute_binary_operation1(Byte** code, X86_Operand_Index operand_index)
{
	Byte     data;
	Byte     register_number;
	Number16 offset;

	if(operand_index < X86_OPERAND_REGISTER_NUMBER_BYTE)
	{
		data = **code; ++*code;
		register_number = (data >> 3) & 0b00000111;

		switch(operand_index)
		{
			case X86_OPERAND_DATA_REGISTER_BYTE:
				
				print_memory16_command(code, data, BITS_8);
				printf(", ");
				print_data_register_name(register_number, BITS_8);
				break;

			case X86_OPERAND_DATA_REGISTER_WORD:
				print_memory16_command(code, data, BITS_16);
				printf(", ");
				print_data_register_name(register_number, BITS_16);
				break;

			case X86_OPERAND_REGISTER_DATA_BYTE:
				print_data_register_name(register_number, BITS_8);
				printf(", ");
				print_memory16_command(code, data, BITS_8);
				break;

			case X86_OPERAND_REGISTER_DATA_WORD:
				print_data_register_name(register_number, BITS_16);
				printf(", ");
				print_memory16_command(code, data, BITS_16);
				break;
		}
	}
	else if(operand_index == X86_OPERAND_REGISTER_NUMBER_BYTE)
	{
		offset = **code; ++*code;
		printf("AL, %d", offset);
	}
	else if(operand_index == X86_OPERAND_REGISTER_NUMBER_WORD)
	{
		offset = **code; ++*code;
		offset += **code << 8; ++*code;
		printf("AX, %d", offset);
	}
	else if(operand_index == X86_OPERAND_DATA_NUMBER_BYTE)
	{
		data = **code; ++*code;
		print_memory16_command(code, data, BITS_8);
		offset = **code; ++*code;
		printf(", %d", offset);
	}
	else if(operand_index == X86_OPERAND_DATA_NUMBER_WORD)
	{
		data = **code; ++*code;
		print_memory16_command(code, data, BITS_16);
		offset = **code; ++*code;
		offset += **code << 8; ++*code;
		printf(", %d", offset);
	}
	else if(operand_index == X86_OPERAND_DATA_SEGMENT)
	{
		data = **code; ++*code;
		register_number = (data >> 3) & 0b00000111;
		print_memory16_command(code, data, BITS_16);
		printf(", ");
		print_segment_register_name(register_number);
	}
	else if(operand_index == X86_OPERAND_SEGMENT_DATA)
	{
		data = **code; ++*code;
		register_number = (data >> 3) & 0b00000111;
		print_segment_register_name(register_number);
		printf(", ");
		print_memory16_command(code, data, BITS_16);
	}
}


void emulate_x86(Byte* code, Number32 code_length)
{
	Byte     operation;
	Byte     suboperation;
	Byte     data;
	Byte     register_number;
	Byte     register_number1;
	Byte     register_number2;
	Byte     memory_mod;
	Byte     memory_opcode;
	Number16 offset;
	Number16 segment;
	X86_Operand_Index operand_index;

	Number32 i;
	for(i = 0; i < code_length; ++i)
	{
		operation = *code; ++code;

		if(operation >= 0 && operation <= 0x05)
		{
			printf("ADD ");
			execute_binary_operation1(&code, operation);
		}
		else if(operation >= 0x08 && operation <= 0x0D)
		{
			printf("OR ");
			execute_binary_operation1(&code, operation - 0x08);
		}
		else if(operation >= 0x10 && operation <= 0x15)
		{
			printf("ADC ");
			execute_binary_operation1(&code, operation - 0x10);
		}
		else if(operation >= 0x18 && operation <= 0x1D)
		{
			printf("SBB ");
			execute_binary_operation1(&code, operation - 0x18);
		}
		else if(operation >= 0x20 && operation <= 0x25)
		{
			printf("AND ");
			execute_binary_operation1(&code, operation - 0x20);
		}
		else if(operation >= 0x28 && operation <= 0x2D)
		{
			printf("SUB ");
			execute_binary_operation1(&code, operation - 0x28);
		}
		else if(operation >= 0x30 && operation <= 0x35)
		{
			printf("XOR ");
			execute_binary_operation1(&code, operation - 0x30);
		}
		else if(operation >= 0x38 && operation <= 0x3D)
		{
			printf("CMP ");
			execute_binary_operation1(&code, operation - 0x38);
		}
		else if(operation >= 0x40 && operation <= 0x47)
		{
			printf("INC ");
			print_data_register_name(operation & 0b00000111, BITS_16);
		}
		else if(operation >= 0x48 && operation <= 0x4F)
		{
			printf("DEC ");
			print_data_register_name(operation & 0b00000111, BITS_16);
		}
		else if(operation >= 0x50 && operation <= 0x57)
		{
			printf("PUSH ");
			print_data_register_name(operation & 0b00000111, BITS_16);
		}
		else if(operation >= 0x58 && operation <= 0x5F)
		{
			printf("POP ");
			print_data_register_name(operation & 0b00000111, BITS_16);
		}
		else if(operation >= 0x70 && operation <= 0x7F)
		{
			offset = *code; ++code;
			switch(operation)
			{
				case 0x70: printf("JO"); break;
				case 0x71: printf("JNO"); break;
				case 0x72: printf("JB"); break;
				case 0x73: printf("JNB"); break;
				case 0x74: printf("JZ"); break;
				case 0x75: printf("JNZ"); break;
				case 0x76: printf("JBE"); break;
				case 0x77: printf("JA"); break;
				case 0x78: printf("JS"); break;
				case 0x79: printf("JNS"); break;
				case 0x7A: printf("JP"); break;
				case 0x7B: printf("JNP"); break;
				case 0x7C: printf("JL"); break;
				case 0x7D: printf("JNL"); break;
				case 0x7E: printf("JLE"); break;
				case 0x7F: printf("JNLE"); break;
			}
			printf(" %d", offset);
		}
		else if(operation >= 0x80 && operation <= 0x83)
		{
			switch(operation)
			{
				case 0x80: operand_index = X86_OPERAND_DATA_NUMBER_BYTE; break;
				case 0x81: operand_index = X86_OPERAND_DATA_NUMBER_WORD; break;
				case 0x82: operand_index = X86_OPERAND_DATA_NUMBER_BYTE; break;
				case 0x83: operand_index = X86_OPERAND_DATA_NUMBER_BYTE; break;
			}

			suboperation = (*code >> 3) & 0b00000111;

			switch(suboperation)
			{
				case 0: //ADD [] Number
					printf("ADD ");
					break;

				case 1: //OR [] Number
					printf("OR ");
					break;

				case 2: //ADC [] Number
					printf("ADC ");
					break;

				case 3: //SBB [] Number
					printf("SBB ");
					break;

				case 4: //AND [] Number
					printf("AND ");
					break;

				case 5: //SUB [] Number
					printf("SUB ");
					break;

				case 6: //XOR [] Number
					printf("XOR ");
					break;

				case 7: //CMP [] Number
					printf("CMP ");
					break;
			}

			execute_binary_operation1(&code, operand_index);
		}
		else if(operation >= 0x90 && operation <= 0x97)
		{
			printf("XCHG ");
			print_data_register_name(operation & 0b00000111, BITS_16);
		}
		else if(operation >= 0xB0 && operation <= 0xB7)
		{
			offset = *code; ++code;
			printf("MOV ");
			print_data_register_name(operation & 0b00000111, BITS_8);
			printf(", %d", offset);
		}
		else if(operation >= 0xB8 && operation <= 0xBF)
		{
			offset = *code; ++code;
			offset += *code << 8; ++code;
			printf("MOV ");
			print_data_register_name(operation & 0b00000111, BITS_16);
			printf(", %d", offset);
		}
		else if(operation >= 0xD8 && operation <= 0xDF)
		{
			printf("floating point operation");
		}
		else
		{
			switch(operation)
			{
				case 0x06: //PUSH ES
					printf("PUSH ES");
					break;

				case 0x07: //POP ES
					printf("POP ES");
					break;

				case 0x0E: //PUSH CS
					printf("PUSH CS");
					break;

				case 0x0F: //TWO BYTES(MMX, SSE etc.)
					printf("extended operation");
					++code;
					break;

				case 0x16: //PUSH SS
					printf("PUSH SS");
					break;

				case 0x17: //POP SS
					printf("POP SS");
					break;

				case 0x1E: //PUSH DS
					printf("PUSH DS");
					break;

				case 0x1F: //POP DS
					printf("POP DS");
					break;

				case 0x26: //ES:
					printf("ES:");
					break;

				case 0x27: //DAA
					printf("DAA");
					break;

				case 0x2E: //CS:
					printf("CS:");
					break;

				case 0x2F: //DAS
					printf("DAS");
					break;

				case 0x36: //SS
					printf("SS");
					break;

				case 0x37: //AAA
					printf("AAA");
					break;

				case 0x3E: //DS
					printf("DS");
					break;

				case 0x3F: //AAS
					printf("AAS");
					break;

				case 0x60: //PUSHA
					printf("PUSHA");
					break;

				case 0x61: //POPA
					printf("POPA");
					break;

				case 0x62: //BOUND
					printf("BOUND ");
					execute_binary_operation1(&code, X86_OPERAND_REGISTER_DATA_WORD);
					break;

				case 0x63: //ARPL [] reg 16
					printf("ARPL ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_REGISTER_WORD);
					break;

				case 0x64: //FS:
					printf("FS:");
					break;

				case 0x65: //GS:
					printf("GS:");
					break;

				case 0x66: //OPSIZE::
					printf("OPSIZE:");
					break;

				case 0x67: //ADSIZE:
					printf("ADSIZE:");
					break;

				case 0x68: //PUSH Number 16
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("PUSH %d", offset);
					break;

				case 0x69: //IMUL Number 16
					printf("IMUL ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_REGISTER_WORD);
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf(", %d", offset);
					break;

				case 0x6A: //PUSH Number 8
					offset = *code; ++code;
					printf("PUSH %d", offset);
					break;

				case 0x6B: //IMUL Number 8
					printf("IMUL ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_REGISTER_WORD);
					offset = *code; ++code;
					printf(", %d", offset);
					break;

				case 0x6C: //INSB Number 8
					printf("INSB");
					break;

				case 0x6D: //INSW Number 8
					printf("INSW");
					break;

				case 0x6E: //OUTSB Number 8
					printf("OUTSB");
					break;

				case 0x6F: //OUTSW Number 8
					printf("OUTSW");
					break;

				case 0x84: //TEST [] reg 8
					printf("TEST ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_REGISTER_BYTE);
					break;

				case 0x85: //TEST [] reg 16
					printf("TEST ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_REGISTER_WORD);
					break;

				case 0x86: //XCHG [] reg 8
					printf("XCHG ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_REGISTER_BYTE);
					break;

				case 0x87: //XCHG [] reg 16
					printf("XCHG ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_REGISTER_WORD);
					break;

				case 0x88: //MOV [] reg 8
					printf("MOV ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_REGISTER_BYTE);
					break;

				case 0x89: //MOV [] reg 16
					printf("MOV ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_REGISTER_WORD);
					break;

				case 0x8A: //MOV reg [] 8
					printf("MOV ");
					execute_binary_operation1(&code, X86_OPERAND_REGISTER_DATA_BYTE);
					break;

				case 0x8B: //MOV reg [] 16
					printf("MOV ");
					execute_binary_operation1(&code, X86_OPERAND_REGISTER_DATA_WORD);
					break;

				case 0x8C: //MOV [] segment
					printf("MOV ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_SEGMENT);
					break;

				case 0x8D: //LEA 16 reg []
					printf("LEA ");
					execute_binary_operation1(&code, X86_OPERAND_REGISTER_DATA_WORD);
					break;

				case 0x8E: //MOV segment []
					printf("MOV ");
					execute_binary_operation1(&code, X86_OPERAND_SEGMENT_DATA);
					break;

				case 0x8F: //POP [] 16
					printf("POP word ");
					data = *code; ++code;
					print_memory16_command(&code, data, BITS_16);
					break;

				case 0x98: //CBW
					printf("CBW");
					break;

				case 0x99: //CWD
					printf("CWD");
					break;

				case 0x9A: //far CALL address
					offset = *code; ++code;
					offset += *code << 8; ++code;
					segment = *code; ++code;
					segment += *code << 8; ++code;
					printf("far CALL %d:%d", segment, offset);
					break;

				case 0x9B: //WAIT
					printf("CWD");
					break;

				case 0x9C: //PUSHF
					printf("PUSHF");
					break;

				case 0x9D: //POPF
					printf("POPF");
					break;

				case 0x9E: //SAHF
					printf("SAHF");
					break;

				case 0x9F: //LAHF
					printf("LAHF");
					break;

				case 0xA0: //MOV AL [address]
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("MOV AL, [%d]", offset);
					break;

				case 0xA1: //MOV AX [address]
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("MOV AX, [%d]", offset);
					break;

				case 0xA2: //MOV [address] AL
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("MOV [%d], AL", offset);
					break;

				case 0xA3: //MOV [address] AX
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("MOV [%d], AX", offset);
					break;

				case 0xA4: //MOVSB
					printf("MOVSB");
					break;

				case 0xA5: //MOVSW
					printf("MOVSW");
					break;

				case 0xA6: //CMPSB
					printf("CMPSB");
					break;

				case 0xA7: //CMPSW
					printf("CMPSW");
					break;

				case 0xA8: //TEST AL number8
					offset = *code; ++code;
					printf("TEST AL, %d", offset);
					break;

				case 0xA9: //TEST AX number16
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("TEST AX, %d", offset);
					break;

				case 0xAA: //STOSB
					printf("STOSB");
					break;

				case 0xAB: //STOSW
					printf("STOSW");
					break;

				case 0xAC: //LODSB
					printf("LODSB");
					break;

				case 0xAD: //LODSW
					printf("LODSW");
					break;

				case 0xAE: //SCASB
					printf("SCASB");
					break;

				case 0xAF: //SCASW
					printf("SCASW");
					break;

				case 0xC0:
					suboperation = (*code >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //ROL [register], number8
							printf("ROL");
							break;

						case 1: //ROR [register], number8
							printf("ROR");
							break;

						case 2: //RCL [register], number8
							printf("RCL");
							break;

						case 3: //RCR [register], number8
							printf("RCR");
							break;

						case 4: //SHL [register], number8
							printf("SHL");
							break;

						case 5: //SHR [register], number8
							printf("SHR");
							break;

						case 6: //SHL [register], number8
							printf("SHL");
							break;

						case 7: //SHR [register], number8
							printf("SHR");
							break;
					}

					data = *code; ++code;
					printf(" byte ");
					print_memory16_command(&code, data, BITS_8);
					offset = *code; ++code;
					printf(", byte %d", offset);
					break;

				case 0xC1:
					suboperation = (*code >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //ROL [register], number16
							printf("ROL");
							break;

						case 1: //ROR [register], number16
							printf("ROR");
							break;

						case 2: //RCL [register], number16
							printf("RCL");
							break;

						case 3: //RCR [register], number16
							printf("RCR");
							break;

						case 4: //SHL [register], number16
							printf("SHL");
							break;

						case 5: //SHR [register], number16
							printf("SHR");
							break;

						case 6: //SHL [register], number16
							printf("SHL");
							break;

						case 7: //SHR [register], number16
							printf("SHR");
							break;
					}

					data = *code; ++code;
					printf(" word ");
					print_memory16_command(&code, data, BITS_16);
					offset = *code; ++code;
					printf(", byte %d", offset);
					break;

				case 0xC2: //RET number16
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("RET %d", offset);
					break;

				case 0xC3: //RET
					printf("RET");
					break;

				case 0xC4: //LES register [register]
					printf("LES ");
					execute_binary_operation1(&code, X86_OPERAND_REGISTER_DATA_WORD);
					break;

				case 0xC5: //LDS register [register]
					printf("LDS ");
					execute_binary_operation1(&code, X86_OPERAND_REGISTER_DATA_WORD);
					break;

				case 0xC6: //MOV [register] number8
					data = *code; ++code;
					printf("MOV byte ");
					print_memory16_command(&code, data, BITS_8);
					offset = *code; ++code;
					printf(", %d", offset);
					break;

				case 0xC7: //MOV [register] number16
					data = *code; ++code;
					printf("MOV word ");
					print_memory16_command(&code, data, BITS_8);
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf(", %d", offset);
					break;

				case 0xC8: //ENTER number16 number8
					offset = *code; ++code;
					offset += *code << 8; ++code;
					data = *code; ++code;
					printf("ENTER %d, %d", offset, data);
					break;

				case 0xC9: //LEAVE
					printf("LEAVE");
					break;

				case 0xCA: //RETF number16
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("RETF %d", offset);
					break;

				case 0xCB: //RETF
					printf("RETF");
					break;

				case 0xCC: //INT3
					printf("INT3");
					break;

				case 0xCD: //INT number8
					offset = *code; ++code;
					printf("INT %d", offset);
					break;

				case 0xCE: //INT0
					printf("INT0");
					break;

				case 0xCF: //IRET
					printf("IRET");
					break;

				case 0xD0: //ROL byte [register] 1
					data = *code; ++code;
					suboperation = (data >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //ROL byte [register], 1
							printf("ROL");
							break;

						case 1: //ROR byte [register], 1
							printf("ROR");
							break;

						case 2: //RCL byte [register], 1
							printf("RCL");
							break;

						case 3: //RCR byte [register], 1
							printf("RCR");
							break;

						case 4: //SHL byte [register], 1
							printf("SHL");
							break;

						case 5: //SHR byte [register], 1
							printf("SHR");
							break;

						case 6: //SHL byte [register], 1
							printf("SHL");
							break;

						case 7: //SHR byte [register], 1
							printf("SHR");
							break;
					}

					printf(" byte ");
					print_memory16_command(&code, data, BITS_8);
					printf(", 1");
					break;

				case 0xD1:
					data = *code; ++code;
					suboperation = (data >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //ROL word [register], 1
							printf("ROL");
							break;

						case 1: //ROR word [register], 1
							printf("ROR");
							break;

						case 2: //RCL word [register], 1
							printf("RCL");
							break;

						case 3: //RCR word [register], 1
							printf("RCR");
							break;

						case 4: //SHL word [register], 1
							printf("SHL");
							break;

						case 5: //SHR word [register], 1
							printf("SHR");
							break;

						case 6: //SHL word [register], 1
							printf("SHL");
							break;

						case 7: //SHR word [register], 1
							printf("SHR");
							break;
					}

					printf(" word ");
					print_memory16_command(&code, data, BITS_16);
					printf(", 1");
					break;

				case 0xD2:
					data = *code; ++code;
					suboperation = (data >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //ROL byte [register], CL
							printf("ROL");
							break;

						case 1: //ROR byte [register], CL
							printf("ROR");
							break;

						case 2: //RCL byte [register], CL
							printf("RCL");
							break;

						case 3: //RCR byte [register], CL
							printf("RCR");
							break;

						case 4: //SHL byte [register], CL
							printf("SHL");
							break;

						case 5: //SHR byte [register], CL
							printf("SHR");
							break;

						case 6: //SHL byte [register], CL
							printf("SHL");
							break;

						case 7: //SHR byte [register], CL
							printf("SHR");
							break;
					}

					printf(" byte ");
					print_memory16_command(&code, data, BITS_8);
					printf(", CL");
					break;

				case 0xD3: //ROL word [register] 1
					data = *code; ++code;
					suboperation = (data >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //ROL word [register], CL
							printf("ROL");
							break;

						case 1: //ROR word [register], CL
							printf("ROR");
							break;

						case 2: //RCL word [register], CL
							printf("RCL");
							break;

						case 3: //RCR word [register], CL
							printf("RCR");
							break;

						case 4: //SHL word [register], CL
							printf("SHL");
							break;

						case 5: //SHR word [register], CL
							printf("SHR");
							break;

						case 6: //SHL word [register], CL
							printf("SHL");
							break;

						case 7: //SHR word [register], CL
							printf("SHR");
							break;
					}

					printf(" word ");
					print_memory16_command(&code, data, BITS_16);
					printf(", CL");
					break;

				case 0xD4: //AAM number8
					offset = *code; ++code;
					printf("AAM %d", offset);
					break;

				case 0xD5: //AAD number8
					offset = *code; ++code;
					printf("AAD %d", offset);
					break;

				case 0xD6: //SALC
					printf("SALC");
					break;

				case 0xD7: //XLAT
					printf("XLAT");
					break;

				case 0xE0: //LOOPNZ number8
					offset = *code; ++code;
					printf("LOOPNZ %d", offset);
					break;

				case 0xE1: //LOOPZ number8
					offset = *code; ++code;
					printf("LOOPZ %d", offset);
					break;

				case 0xE2: //LOOP number8
					offset = *code; ++code;
					printf("LOOP %d", offset);
					break;

				case 0xE3: //JCXZ number8
					offset = *code; ++code;
					printf("JCXZ %d", offset);
					break;

				case 0xE4: //IN AL, number8
					offset = *code; ++code;
					printf("IN AL, %d", offset);
					break;

				case 0xE5: //IN AX, number8
					offset = *code; ++code;
					printf("IN AX, %d", offset);
					break;

				case 0xE6: //OUT number8, AL
					offset = *code; ++code;
					printf("OUT %d, AL", offset);
					break;

				case 0xE7: //OUT number8, AX
					offset = *code; ++code;
					printf("OUT %d, AX", offset);
					break;

				case 0xE8: //CALL number16
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("CALL %d", offset);
					break;

				case 0xE9: //JMP number16
					offset = *code; ++code;
					offset += *code << 8; ++code;
					printf("JMP %d", offset);
					break;

				case 0xEA: //JMP number16:number16
					offset = *code; ++code;
					offset += *code << 8; ++code;
					segment = *code; ++code;
					segment += *code << 8; ++code;
					printf("JMP %d:%d", segment, offset);
					break;

				case 0xEB: //JMP number8
					offset = *code; ++code;
					printf("JMP %d", offset);
					break;

				case 0xEC: //IN AL, DX
					printf("IN AL, DX");
					break;

				case 0xED: //IN AX, DX
					printf("IN AX, DX");
					break;

				case 0xEE: //OUT DX, AL
					printf("OUT DX, AL");
					break;

				case 0xEF: //OUT DX, AX
					printf("OUT DX, AX");
					break;

				case 0xF0: //LOCK:
					printf("LOCK:");
					break;

				case 0xF1: //INT1
					printf("INT1");
					break;

				case 0xF2: //REPNE:
					printf("REPNE:");
					break;

				case 0xF3: //REP:
					printf("REP:");
					break;

				case 0xF4: //HLT
					printf("HLT");
					break;

				case 0xF5: //CMC
					printf("CMC");
					break;

				case 0xF6:
					suboperation = (*code >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //TEST byte [register] number8
							printf("TEST");
							break;

						case 1: //TEST byte [register] number8
							printf("TEST");
							break;

						case 2: //NOT byte [register] number8
							printf("NOT");
							break;

						case 3: //NEG byte [register] number8
							printf("NEG");
							break;

						case 4: //MUL byte [register] number8
							printf("MUL");
							break;

						case 5: //IMUL byte [register] number8
							printf("IMUL");
							break;

						case 6: //DIV byte [register] number8
							printf("DIV");
							break;

						case 7: //IDIV byte [register] number8
							printf("IDIV");
							break;
					}

					printf(" byte ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_NUMBER_BYTE);
					break;

				case 0xF7: //TEST word [register] number16
					suboperation = (*code >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //TEST byte [register] number16
							printf("TEST");
							break;

						case 1: //TEST byte [register] number16
							printf("TEST");
							break;

						case 2: //NOT byte [register] number16
							printf("NOT");
							break;

						case 3: //NEG byte [register] number16
							printf("NEG");
							break;

						case 4: //MUL byte [register] number16
							printf("MUL");
							break;

						case 5: //IMUL byte [register] number16
							printf("IMUL");
							break;

						case 6: //DIV byte [register] number16
							printf("DIV");
							break;

						case 7: //IDIV byte [register] number16
							printf("IDIV");
							break;
					}

					printf(" word ");
					execute_binary_operation1(&code, X86_OPERAND_DATA_NUMBER_WORD);
					break;

				case 0xF8: //CLC
					printf("CLC");
					break;

				case 0xF9: //STC
					printf("STC");
					break;

				case 0xFA: //CLI
					printf("CLI");
					break;

				case 0xFB: //STI
					printf("STI");
					break;

				case 0xFC: //CLD
					printf("CLD");
					break;

				case 0xFD: //STD
					printf("STD");
					break;

				case 0xFE: //INC [register] 8
					data = *code; ++code;
					suboperation = (data >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //INC [register] 8
							printf("INC");
							break;

						case 1: //DEC [register] 8
							printf("DEC");
							break;
					}

					printf("INC byte ");
					print_memory16_command(&code, data, BITS_8);
					break;

				case 0xFF: //INC [register] 16
					data = *code; ++code;
					suboperation = (data >> 3) & 0b00000111;

					switch(suboperation)
					{
						case 0: //INC [register] 16
							printf("INC");
							break;

						case 1: //DEC [register] 16
							printf("DEC");
							break;

						case 2: //CALL [register] 16
							printf("CALL");
							break;

						case 3: //CALL far [register] 16
							printf("CALL far");
							break;

						case 4: //JMP [register] 16
							printf("JMP");
							break;

						case 5: //JMP far [register] 16
							printf("JMP far");
							break;

						case 6: //PUSH [register] 16
							printf("PUSH");
							break;
					}

					printf(" word ");
					print_memory16_command(&code, data, BITS_16);
					break;
			}
		}

		printf("\n");
	}
}



//Number32 AX; 0
//Number32 CX; 1
//Number32 DX; 2
//Number32 BX; 3
//Number32 SP; 4
//Number32 BP; 5
//Number32 SI; 6
//Number32 DI; 7

//Number32 ES; 0
//Number32 CS; 1
//Number32 SS; 2
//Number32 DS; 3
//Number32 FS; 4
//Number32 GS; 5


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
					offset = processor->memory[processor->IP]; ++processor->IP;
					offset += processor->memory[processor->IP] << 8; ++processor->IP;
					printf("[%d]", offset);
					return processor->memory + offset;
				case 7: printf("[BX]"); return processor->memory + processor->data_registers[3];
			}
			break;

		case 1:
			offset = processor->memory[processor->IP]; ++processor->IP;

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
			offset = processor->memory[processor->IP]; ++processor->IP;
			offset += processor->memory[processor->IP] << 8; ++processor->IP;

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
	printf("here");
}


static void(*operation_executors[256])(Processor* processor) = {
	&execute_operation_00
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
	}
}


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
}


void test2()
{
	Byte code[] = {
		0x00
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
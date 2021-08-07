#include <memory/memory.h>


static N_32 get_operand_data_size(Operand_Type type)
{
	switch(type)
	{
		case NUMBER:
			return sizeof(Number_Operand);
		case NAME_OPERAND:
			return sizeof(Name_Operand);
		case VARIABLE:
			return sizeof(Variable_Operand);
		case LABEL:
			return sizeof(Label_Operand);
		case CALL:
			return sizeof(Call_Operand);

		case NUMBER_LINK:
			return sizeof(Number_Operand*);
		case VARIABLE_LINK:
			return sizeof(Variable_Operand*);
		case LABEL_LINK:
			return sizeof(Label_Operand*);

		case LABEL_ADDRESS:
			return sizeof(N_32);

		case DATA_BLOCK:
			return sizeof(Data_Block_Operand);

		default:
			return 0;
	}
}


Operand* create_operand(Operand_Type type, Byte* data)
{
	N_32     operand_size;
	Operand* operand;

	operand_size = get_operand_data_size(type);
	operand = allocate_memory(sizeof(Operand) + operand_size);
	operand->type = type;
	copy_memory(&operand->data, data, operand_size);

	return operand;
}


Operand* create_number_operand(N_32 value)
{
	Number_Operand operand;

	operand.value = value;
	
	return create_operand(NUMBER, &operand);
}


Operand* create_name_operand(Buffer* name)
{
	Name_Operand operand;

	initialize_buffer(&operand.name, 10);
	add_buffer_in_buffer_end(&operand.name, name);
	
	return create_operand(NAME_OPERAND, &operand);
}


Operand* create_label_operand(Buffer* label_name, N_32 expression_index)
{
	Label_Operand operand;

	initialize_buffer(&operand.name, 10);
	add_buffer_in_buffer_end(&operand.name, label_name);

	operand.expression_index = expression_index;
	
	return create_operand(LABEL, &operand);
}


Operand* create_label_link_operand(Label_Operand* operand)
{
	return create_operand(LABEL_LINK, &operand);
}


Operand* create_variable_operand(Buffer* variable_name, N_32 variable_index, Type* type)
{
	Variable_Operand operand;

	initialize_buffer(&operand.name, 10);
	add_buffer_in_buffer_end(&operand.name, variable_name);

	operand.variable_index = variable_index;
	operand.type = type;
	
	return create_operand(VARIABLE, &operand);
}


Operand* create_variable_link_operand(Variable_Operand* operand)
{
	return create_operand(VARIABLE_LINK, &operand);
}


Operand* create_static_string_operand(Buffer* data)
{
	Static_String_Operand operand;

	initialize_buffer(&operand.data, 10);
	add_buffer_in_buffer_end(&operand.data, data);
	
	return create_operand(STATIC_STRING, &operand);
}


Operand* create_data_block_operand(Buffer* expressions)
{
	Data_Block_Operand operand;

	operand.expressions = expressions;

	return create_operand(DATA_BLOCK, &operand);
}


Operand* create_call_operand(Type* type)
{
	Call_Operand operand;

	operand.type = type;
	initialize_buffer(&operand.expressions, 100);

	return create_operand(CALL, &operand);
}


void add_operand_in_stack(Buffer* stack, Operand_Type type, Byte* operand_data)
{
	add_bytes_in_buffer_end(stack, &type, sizeof(Operand_Type));
	add_bytes_in_buffer_end(stack, operand_data, get_operand_data_size(type));
	add_bytes_in_buffer_end(stack, &type, sizeof(Operand_Type));
}


Operand_Type get_operand_type_from_stack(Buffer* stack)
{
	if(!stack->length)
	{
		printf("error: not found next operand\n");
		return 0;
	}

	return stack->data[stack->length - sizeof(N_32)];
}


Operand* remove_operand_from_stack(Buffer* stack)
{
	N_32 operand_type;

	operand_type = get_operand_type_from_stack(stack);
	stack->length -= sizeof(Operand) + sizeof(Operand) + get_operand_data_size(operand_type);

	return stack->data + stack->length;
}
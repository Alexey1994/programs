#include <types.c>


Byte* stream;
Byte head;


#define next() {head = *stream; if(*stream){++stream;}}


void print_operation(Byte operation)
{
	Byte s[2];

	s[0] = operation;
	s[1] = '\0';
	print("s", s);
}


Number get_prior(Byte operation)
{
	switch(operation)
	{
		case '+':
			return 2;

		case '*':
			return 3;

		case '(':
			return 1;

		case 0:
			return 0;
	}

	print("s", "неизвестная операция ");
	print_operation(operation);
	print("sns", ":", operation, "\n");
}


Boolean parse(Boolean require_operand, Byte* previouse_operation);


Boolean add_operation(Byte operation, Byte* previouse_operation)
{
	next();

	if(operation != '(' && get_prior(operation) <= get_prior(*previouse_operation))
	{
		print_operation(*previouse_operation);
		*previouse_operation = 0;
	}

	if(parse(1, &operation))
	{
		if(operation == '(')
			return parse(0, &operation);

		if(operation)
			print_operation(operation);

		return 1;
	}

	if(operation == '(')
	{
		print("s", "отсутствует )");
	}
	else if(operation)
		print_operation(operation);

	return 0;
}


Boolean parse(Boolean require_operand, Byte* previouse_operation)
{
	if(head == '(')
	{
		if(!require_operand)
			return 0;

		return add_operation('(', previouse_operation);
	}
	else
	{
		if(require_operand)
		{
			if(head == ')')
			{
				print("s", ") не может быть здесь");
				return 0;
			}

			print_operation(head);
			next();
		}

		if(head == '+')
			return add_operation('+', previouse_operation);
		else if(head == '*')
			return add_operation('*', previouse_operation);
		else if(head == ')')
		{
			next();

			if(*previouse_operation == 0)
			{
				print("s", "отсутствует )");
			}

			return 1;
		}
	}

	return 0;
}


void test(Byte* s)
{
	Byte current_operation;

	print("ss", s, " -> ");

	stream = s;

	next();
	current_operation = 0;
	parse(1, &current_operation);
	print("s", "\n");
}


void main()
{
	test("1");
	test("1+2");
	test("1+2+3");
	test("1*2");
	test("1*2*3");
	test("1*2+3");
	test("1+2*3");
	test("(1)");
	test("(1+2)");
	test("(1+2+3)");
	test("(1+2)+3");
	test("1+(2+3)");
	test("(1*2)");
	test("(1+2)*3");
	test("1*(2+3)");
	test("1*(2+3*(4+5))*(6+7)");

	print("s", "\n");

	test("(");
	test("(1");
	test("(1+2");
	test("(1+2+3");
	test("(1+(2+3)");
	test("()");
	test(")");
	test("1)");
	test("1+)");
}
#ifndef CRYPTOGRAPHY_INCLUDED
#define CRYPTOGRAPHY_INCLUDED


#include <types.c>


#define BIG_NUMBER_SIZE 4


typedef Number8 Big_Number[BIG_NUMBER_SIZE];


void print_Big_Number(Big_Number a)
{
	Number i;

	for(i = 0; i < BIG_NUMBER_SIZE; ++i)
	{
		printf("%d ", a[i]);
	}
}


Integer_Number compare_numbers(Big_Number a, Big_Number b)
{
	Number         i;
	Integer_Number result;

	for(i = BIG_NUMBER_SIZE; i; --i)
	{
		result = a[i - 1] - b[i - 1];
		
		if(result)
			return result;
	}

	return 0;
}


void initialize_null(Big_Number a)
{
	Number i;
	for(i = 0; i < BIG_NUMBER_SIZE; ++i)
		a[i] = 0;
}


Boolean is_null(Big_Number a)
{
	Number i;

	for(i = 0; i < BIG_NUMBER_SIZE; ++i)
		if(a[i])
			return 0;
	return 1;
}


void copy(Big_Number source, Big_Number destination)
{
	Number i;

	for(i = 0; i < BIG_NUMBER_SIZE; ++i)
		destination[i] = source[i];
}


void shift_left(Big_Number a, Big_Number result)
{
	/*
	Number i;
	Number remind;

	remind = 0;
	for(i = 0; i < BIG_NUMBER_SIZE; ++i)
	{
		result[i] = a[i] << 1;
		if(remind)
			result[i] |= 0b00000001;
		remind = a[i] & 0b10000000;
	}*/

	Number i;
	for(i = BIG_NUMBER_SIZE - 1; i; --i)
		result[i] = a[i - 1];
	result[0] = 0;
}


void shift_right(Big_Number a, Big_Number result)
{
	/*
	Number i;
	Number remind;

	remind = 0;
	for(i = BIG_NUMBER_SIZE; i; --i)
	{
		result[i - 1] = a[i - 1] >> 1;
		if(remind)
			result[i - 1] |= 0b10000000;
		remind = a[i - 1] & 0b00000001;
	}*/

	Number i;
	for(i = 1; i < BIG_NUMBER_SIZE; ++i)
		result[i - 1] = a[i];
	result[BIG_NUMBER_SIZE - 1] = 0;
}


void add(Big_Number a, Big_Number b, Big_Number result)
{
	Number i;
	Number remind;

	remind = 0;
	for(i = 0; i < BIG_NUMBER_SIZE; ++i)
	{
		remind += (Number)a[i] + (Number)b[i];
		result[i] = remind; //для десятичныхremind % 10;
		remind >>= 8;       //для десятичных /= 10;
	}
}


void negate(Big_Number a, Big_Number result)
{
	Number     i;
	Big_Number one = {1};

	for(i = 0; i < BIG_NUMBER_SIZE; ++i)
		result[i] = ~a[i];
	add(result, one, result);
}


void subtract(Big_Number a, Big_Number b, Big_Number result)
{
	Big_Number negated;
	
	negate(b, negated);
	add(a, negated, result);
}


void multiply(Big_Number a, Big_Number b, Big_Number result)
{
	Number     i;
	Number     j;
	Number     local_result;
	Big_Number result_string;

	initialize_null(result);

	for(i = 0; i < BIG_NUMBER_SIZE; ++i)
	{
		initialize_null(result_string);
		local_result = 0;

		for(j = 0; j < BIG_NUMBER_SIZE; ++j)
		{
			local_result += b[i] * a[j];
			if(i + j < BIG_NUMBER_SIZE)
				result_string[i + j] = local_result;
			local_result >>= 8;
		}

		add(result, result_string, result);
	}
}


void divide(Big_Number a, Big_Number b, Big_Number result)
{
	Big_Number c;
	Number     i;

	initialize_null(result);
	initialize_null(c);

	if(!compare_numbers(b, c))
	{
		printf("division by zero\n");
		return;
	}

	for(i = BIG_NUMBER_SIZE; i; --i)
	{
		shift_left(c, c);
		c[0] = a[i - 1];

		while(compare_numbers(c, b) >= 0)
		{
			subtract(c, b, c);
			++result[i - 1];
		}
	}
}


void fast_power(Big_Number a, Big_Number power, Big_Number result)
{

}


#endif//CRYPTOGRAPHY_INCLUDED


void main()
{
	Big_Number a = {1, 2, 4};
	Big_Number b = {0, 2};
	Big_Number result = {0};

	//negate(a, result);
	//shift_left(a, result);
	//shift_right(a, result);
	//add(a, b, result);
	//subtract(a, b, result);
	//multiply(a, b, result);
	//divide(a, b, result);
	//fast_power(a, b, result);
	print_Big_Number(result);
}
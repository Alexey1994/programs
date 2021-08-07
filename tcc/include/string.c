#ifndef UTF_8_INCLUDED
#define UTF_8_INCLUDED


#include <types.c>


Number32 read_next_UTF_8_character_from_string(Number8** string)
{
	Number8  current_byte;
	Number   number_of_bytes;
	Number32 result;
	Number   i;

	current_byte = **string;
	++*string;

	if(!(current_byte & 0b10000000))
		return current_byte;

	if((current_byte & 0b11110000) == 0b11110000)
	{
		number_of_bytes = 4;
		result = (current_byte & 0b00001111) << 18;
	}
	else if((current_byte & 0b11100000) == 0b11100000)
	{
		number_of_bytes = 3;
		result = (current_byte & 0b00011111) << 12;
	}
	else if((current_byte & 0b11000000) == 0b11000000)
	{
		number_of_bytes = 2;
		result = (current_byte & 0b00111111) << 6;
	}
	else
		goto error;

	for(i = 0; i < number_of_bytes - 1; ++i)
	{
		current_byte = **string;
		++*string;
		result |= (current_byte & 0b00111111) << ((number_of_bytes - 2 - i) * 6);
	}

	return result;

error:
	return 1;
}


void write_UTF8_character(Number32 character, Byte** string)
{
	if(character < 0b1111111)
	{
		**string = character;
		++*string;
	}
	else if(character < 0x7FF)
	{
		**string = 0b11000000 | ((character >> 6) & 0b00011111);
		++*string;

		**string = 0b10000000 | ((character) & 0b00111111);
		++*string;
	}
}


Integer_Number compare_UTF_8_strings(Byte* string1, Byte* string2)
{
	Integer_Number character1;
	Integer_Number character2;
	Integer_Number result;

	for(;;)
	{
		character1 = read_next_UTF_8_character_from_string(&string1);
		character2 = read_next_UTF_8_character_from_string(&string2);
		result = character1 - character2;

		if(result || !(character1 && character2))
			return result;
	}
}


Integer_Number compare_long_string_and_UTF_8_string(Number16* string1, Number number_of_characters_in_string1, Byte* string2)
{
	Integer_Number character1;
	Integer_Number character2;
	Integer_Number result;

	for(; number_of_characters_in_string1; --number_of_characters_in_string1, ++string1)
	{
		character1 = *string1;
		character2 = read_next_UTF_8_character_from_string(&string2);
		result = character1 - character2;

		if(result || !(number_of_characters_in_string1 && character2))
			return result;
	}

	if(number_of_characters_in_string1)
		return *string1;
	else
	{
		character2 = read_next_UTF_8_character_from_string(&string2);
		return -character2;
	}
}


Integer_Number compare_long_strings(Number16* string1, Number number_of_characters_in_string1, Number16* string2, Number number_of_characters_in_string2)
{
	Integer_Number character1;
	Integer_Number character2;
	Integer_Number result;

	for(; number_of_characters_in_string1 && number_of_characters_in_string2; --number_of_characters_in_string1, ++string1, --number_of_characters_in_string2, ++string2)
	{
		result = *string1 - *string2;

		if(result || !(number_of_characters_in_string1 && number_of_characters_in_string2))
			return result;
	}

	if(number_of_characters_in_string1)
		return *string1;
	else if(number_of_characters_in_string2)
	{
		character2 = *string2;
		return -character2;
	}
	else
		return 0;
}


#endif//UTF_8_INCLUDED

/*
void main()
{
	printf("%d\n", compare_UTF_8_strings("aa", "aa"));
	printf("%d\n", compare_long_string_and_UTF_8_string(L"ab", 2, "aa"));
	printf("%d\n", compare_long_strings(L"ab", 2, L"ac", 2));
}*/
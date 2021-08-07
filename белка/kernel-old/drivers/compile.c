#include <system/file.c>
#include <program/executor.c>


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

#define SET_CONTEXT             {WRITE_BYTE(SET_CONTEXT_COMMAND);}
#define GET_CONTEXT             {WRITE_BYTE(GET_CONTEXT_COMMAND);}


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


void main()
{
	//IMMEDIATE_DATA(0xEB, 0xFE)
	
	PUSH8(1)
	PUSH(0xB8000)
	STORE8

	PUSH8(15)
	PUSH(0xB8001)
	STORE8

	STOP

	create_file(L"OUT", memory, instruction_index);
}
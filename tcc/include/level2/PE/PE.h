#ifndef PE_H_INCLUDED
#define PE_H_INCLUDED


#include <system/system.h>
#include <input/input.h>
#include <output/output.h>
#include <data-structures/buffer/buffer.h>


typedef enum
{
    I386_MACHINE = 0x014C
}
Machine;

typedef enum
{
    APPLICATION_32 = 0x010B
}
PE_Type;


typedef enum
{
    EXECUTABLE_CODE_PE_SECTION       = 0x00000020,
    INITIALIZED_DATA_PE_SECTION      = 0x00000040,
    UNINITIALIZED_DATA_PE_SECTION    = 0x00000080,
    ENABLE_EXECUTE_MEMORY_PE_SECTION = 0x20000000,
    ENABLE_READ_MEMORY_PE_SECTION    = 0x40000000,
    ENABLE_WRITE_MEMORY_PE_SECTION   = 0x80000000
}
PE_Section_Type;

typedef struct
{
    Byte            name[8];
    N_32            address;
    PE_Section_Type type;
    Buffer          data;
}
PE_Section;

typedef struct
{
    Buffer  name;
    Buffer* functions;
}
PE_Import_Library;

typedef struct
{
    Machine machine;

    struct
    {
        N_32 address;
        N_32 size;
    }
    data_directories[16];
/*
    struct
    {
        Buffer libraries;
    }
    import_section;
*/
    Buffer import_libraries;

    struct
    {
        N_32   address;
        Buffer section;
        Output section_output;
        Input  section_input;
    }
    export_section;

    Buffer  sections;
    N_32    section_address;
    Output* output;
    Boolean is_first_library_import;
    N_32    import_function_address;
    N_32    entry_point_address;
}
PE;


void begin_PE (PE* pe, Output* output);
    N_32 add_section_in_PE (PE* pe, Byte* name, PE_Section_Type type, Input* input);
    
    void add_PE_library_import(PE* pe, Byte* library_name);
    N_32 add_PE_library_function_import(PE* pe, Byte* function_name);
    void add_PE_import_section(PE* pe);

    void add_PE_code_section(PE* pe, Input* input);
void end_PE (PE* pe);


#include "PE.c"
#endif //PE_H_INCLUDED

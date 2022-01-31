#define DEBUG
#include <log/log.h>
#include <file/file.h>
#include "PE.h"


void define_function(Output* output, Byte* function_name, ...)
{
    N_32** argument = &function_name + 1;
    printf(function_name);

    while(*argument)
    {
        printf("%d ", argument);
        ++argument;
    }
}


Boolean end_define_function(Byte* function_name)
{
    printf("%s\n", function_name);
    return 0;
}

#define def(function_name, ...)\
    N_32 _count_##function_name;\
    printf(#__VA_ARGS__);\
    define_function(&output, (#function_name), ##__VA_ARGS__, 0);\
    for(_count_##function_name = 0; _count_##function_name < 1 || end_define_function("end function"); ++_count_##function_name)

#define function_call(address, )


N_32 main()
{
    PE   pe;
    N_32 pe_data_address;
    N_32 Sleep;

    FILE_OUTPUT("out.exe")
        begin_PE(&pe, &output);
            add_PE_library_import(&pe, "msvcrt.dll");
            add_PE_library_function_import(&pe, "printf");
            add_PE_library_function_import(&pe, "scanf");

            add_PE_library_import(&pe, "kernel32.dll");
            Sleep = add_PE_library_function_import(&pe, "Sleep");
            add_PE_import_section(&pe);

            BUFFER_IO()
                write_null_terminated_byte_array(&output, "Hi");
                write_byte(&output, '\0');
                flush_output(&output);

                pe_data_address = add_section_in_PE(
                    &pe,
                    "Data",
                    INITIALIZED_DATA_PE_SECTION | ENABLE_READ_MEMORY_PE_SECTION,
                    &input
                );
            END_BUFFER_IO

            BUFFER_IO()
                //scanf 0x00400000 + 0x1000
                //mov eax, 0x00400000 + 0x1000 + 70 - scanf string in import section
                write_byte(&output, 0xB8);
                N_32 data_address = 0x00400000 + 0x1000;// + left_offset;// + 70;
                write_byte_array(&output, &data_address, sizeof(data_address));

                //push eax
                write_byte(&output, 0x50);

                //mov eax, [0x00400000 + 0x1000 + 48]
                write_byte(&output, 0xA1);
                N_32 function_address = 0x00400000 + 0x1000 + 4;// + 48;
                write_byte_array(&output, &function_address, sizeof(function_address));

                //call eax
                write_byte(&output, 0xFF);
                write_byte(&output, 0xD0);


                //Sleep 1000
                //mov eax, 0x00400000 + 0x1000 + 70 - Sleep string in import section
                write_byte(&output, 0xB8);
                N_32 data_address = 1000;
                write_byte_array(&output, &data_address, sizeof(data_address));

                //push eax
                write_byte(&output, 0x50);

                //mov eax, [0x00400000 + 0x1000 + 48]
                write_byte(&output, 0xA1);
                write_byte_array(&output, &Sleep, sizeof(function_address));

                //call eax
                write_byte(&output, 0xFF);
                write_byte(&output, 0xD0);


                //printf 0x00400000 + 0x1000
                //mov eax, 0x00400000 + 0x1000 + 70 - printf string in import section
                write_byte(&output, 0xB8);
                N_32 data_address = pe_data_address;//0x00400000 + 0x1000;// + left_offset;// + 70;
                write_byte_array(&output, &data_address, sizeof(data_address));

                //push eax
                write_byte(&output, 0x50);

                //mov eax, [0x00400000 + 0x1000 + 48]
                write_byte(&output, 0xA1);
                N_32 function_address = 0x00400000 + 0x1000;// + 48;
                write_byte_array(&output, &function_address, sizeof(function_address));

                //call eax
                write_byte(&output, 0xFF);
                write_byte(&output, 0xD0);


                /*
                // call 0x0001500 - 0x2000
                write_byte(&output, 0xE8);
                N_32 function_address = -5 - 0x500;//0x1500;
                write_byte_array(&output, &function_address, sizeof(function_address));
                */

                //jump $
                write_byte(&output, 0xEB);
                write_byte(&output, 0xFE);

                /*
                def(func, "a", "b") {

                }*/

                flush_output(&output);

                add_PE_code_section(&pe, &input);
            END_BUFFER_IO
        end_PE(&pe);
    END_OUTPUT

    return 0;
}

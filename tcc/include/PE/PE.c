#include "PE.h"

#define PE_IMAGE_BASE 0x00400000


static N_32 get_aligned_size(N_32 size, N_32 align)
{
    if(size % align)
        size = (size / align + 1) * align;

    return size;
}


void begin_PE(PE* pe, Output* output)
{
    pe->is_first_library_import = 1;
    pe->section_address     = 4096;
    pe->entry_point_address = 0;
    pe->output              = output;
    initialize_buffer(&pe->sections, 1);
    initialize_buffer(&pe->import_libraries, 100);

    cycle(0, 16, 1)
        pe->data_directories[i].address = 0;
        pe->data_directories[i].size = 0;
    end
}


static N_32 get_aligned_headers_size(PE* pe)
{
    return get_aligned_size(312 + pe->sections.length / sizeof(PE_Section) * 40, 512);
}


static void write_PE_header(PE* pe)
{
    write_byte_array(pe->output, "MZ", 2);

    cycle(2, 0x3C, 1)
        write_byte(pe->output, 0);
    end

    write_binary_N_32(pe->output, 0x40);

    write_byte_array(pe->output, "PE\0\0", 4);
    write_binary_N_16(pe->output, I386_MACHINE);
    write_binary_N_16(pe->output, pe->sections.length / sizeof(PE_Section));
    write_binary_N_32(pe->output, 0);
    write_binary_N_32(pe->output, 0);
    write_binary_N_32(pe->output, 0);

    write_binary_N_16(pe->output, 0xE0);
    write_binary_N_16(pe->output, 0x030F); //charachteristics(flags)
}


static N_32 calculate_PE_image_size(PE* pe)
{
    N_32 image_size;

    image_size = 4096;//get_aligned_headers_size(pe);

    for_each_buffer_element(&pe->sections, PE_Section, section)
    {
        image_size += get_aligned_size(section.data.length, 4096);
    }
    end_for_each_buffer_element

    return image_size;//get_aligned_size(image_size, 8192);
}


static void write_optional_PE_header(PE* pe)
{
    write_binary_N_16(pe->output, APPLICATION_32);               //application type
    write_binary_N_16(pe->output, 0);                            //version

    write_binary_N_32(pe->output, 0);                            //size of code
    write_binary_N_32(pe->output, 0);                            //size of initialized data
    write_binary_N_32(pe->output, 0);                            //size of uninitialized data
    write_binary_N_32(pe->output, pe->entry_point_address);      //address of entry point
    write_binary_N_32(pe->output, 0x00000000);                   //base of code
    write_binary_N_32(pe->output, 0x00000000);                   //base of data
    write_binary_N_32(pe->output, PE_IMAGE_BASE);                //image base

    write_binary_N_32(pe->output, 4096);                         //section alignment
    write_binary_N_32(pe->output, 512);                          //file alignment

    write_binary_N_32(pe->output, 0x04);                         //OS version
    write_binary_N_32(pe->output, 0);                            //Image version
    write_binary_N_32(pe->output, 0x04);                         //Subsystem version
    write_binary_N_32(pe->output, 0);                            //Win32 version

    write_binary_N_32(pe->output, calculate_PE_image_size(pe));  //size of image
    write_binary_N_32(pe->output, get_aligned_headers_size(pe)); //size of headers

    write_binary_N_32(pe->output, 0);                            //checksum
    write_binary_N_16(pe->output, 3);                            //subsystem(console)
    write_binary_N_16(pe->output, 0);                            //dll charachteristics

    write_binary_N_32(pe->output, 0x00001000);                   //stack reserve
    write_binary_N_32(pe->output, 0x00001000);                   //stack commit

    write_binary_N_32(pe->output, 0x00001000);                   //heap reserve
    write_binary_N_32(pe->output, 0x00001000);                   //heap commit

    write_binary_N_32(pe->output, 0);
}


static void write_PE_data_directories_header(PE* pe)
{
    write_binary_N_32(pe->output, 16);

    cycle(0, 16, 1)
        write_binary_N_32(pe->output, pe->data_directories[i].address);
        write_binary_N_32(pe->output, pe->data_directories[i].size);
    end
}


static void write_PE_sections(PE* pe)
{
    N_32 headers_size;
    N_32 aligned_headers_size;
    N_32 section_position;

    headers_size = 312 + pe->sections.length / sizeof(PE_Section) * 40;
    aligned_headers_size = get_aligned_size(headers_size, 512);
    section_position = 0;

    for_each_buffer_element(&pe->sections, PE_Section, section)
    {
        write_byte_array(pe->output, section.name, 8);
        write_binary_N_32(pe->output, section.data.length);
        write_binary_N_32(pe->output, section.address);
        write_binary_N_32(pe->output, get_aligned_size(section.data.length, 512));

        write_binary_N_32(pe->output, section_position + aligned_headers_size);
        write_binary_N_32(pe->output, 0);
        write_binary_N_32(pe->output, 0);
        write_binary_N_32(pe->output, 0);
        write_binary_N_32(pe->output, section.type);

        section_position += get_aligned_size(section.data.length, 512);
    }
    end_for_each_buffer_element

    while(headers_size % 512)
    {
        write_byte(pe->output, 0);
        ++headers_size;
    }

    for_each_buffer_element(&pe->sections, PE_Section, section)
    {
        write_byte_array(pe->output, section.data.data, section.data.length);

        N_32 section_data_size = section.data.length;
        while(section_data_size % 512)
        {
            write_byte(pe->output, 0);
            ++section_data_size;
        }
    }
    end_for_each_buffer_element
}


void end_PE(PE* pe)
{
    write_PE_header(pe);
    write_optional_PE_header(pe);
    write_PE_data_directories_header(pe);
    write_PE_sections(pe);

    deinitialize_buffer(&pe->sections);

    for_each_buffer_element(&pe->import_libraries, PE_Import_Library, import_library)
    {
        deinitialize_buffer(&import_library.name);

        for_each_buffer_element(import_library.functions, Buffer, function_name)
        {
            deinitialize_buffer(&function_name);
        }
        end_for_each_buffer_element

        deinitialize_buffer(import_library.functions);
        free(import_library.functions);
    }
    end_for_each_buffer_element
    deinitialize_buffer(&pe->import_libraries);
}


static void initialize_PE_section_name(Byte* section_name, Byte* name)
{
    cycle(0, 8, 1)
        section_name[i] = *name;

        if(*name)
            ++name;
    end
}


N_32 add_section_in_PE (PE* pe, Byte* name, PE_Section_Type type, Input* input)
{
    PE_Section section;

    initialize_PE_section_name(section.name, name);
    section.address = pe->section_address;
    section.type    = type;

    Byte byte;
    N_32 size;

    initialize_buffer(&section.data, 100);
    
    while(!end_of_input(input))
    {
        byte = input_byte(input);
        next_input_byte(input);
        add_in_buffer_end(&section.data, Byte, byte);
    }

    pe->section_address += get_aligned_size(section.data.length, 4096);
    add_in_buffer_end(&pe->sections, PE_Section, section);

    return PE_IMAGE_BASE + section.address;
}


void add_PE_library_import(PE* pe, Byte* library_name)
{
    PE_Import_Library library;

    if(pe->is_first_library_import)
    {
        pe->import_function_address = pe->section_address + PE_IMAGE_BASE;
        pe->is_first_library_import = 0;
    }
    else
    {
        pe->import_function_address += 4;
    }

    initialize_buffer(&library.name, 10);
    add_bytes_in_buffer_end(&library.name, library_name, strlen(library_name));
    library.functions = malloc(sizeof(Buffer));
    initialize_buffer(library.functions, 10);
    add_in_buffer_end(&pe->import_libraries, PE_Import_Library, library);
}


N_32 add_PE_library_function_import(PE* pe, Byte* function_name)
{
    PE_Import_Library library;
    N_32              function_address;

    get_buffer_bytes(&pe->import_libraries, pe->import_libraries.length - sizeof(PE_Import_Library), &library, sizeof(PE_Import_Library));
    
    Buffer function_name_buffer;
    initialize_buffer(&function_name_buffer, 10);
    add_bytes_in_buffer_end(&function_name_buffer, function_name, strlen(function_name));

    add_in_buffer_end(library.functions, Buffer, function_name_buffer);
    function_address = pe->import_function_address;
    pe->import_function_address += 4;

    return function_address;
}


typedef struct
{
    N_32 lookup_table_address;
    N_32 time;
    N_32 first_forwarder_index;
    N_32 name_address;
    N_32 import_address_table;
}
PE_Import_Directory;


/*
function_addresses:
{
    0x00000000,
    0x00000000
}

{
    function1_names,
    0x00000000,
    0x00000000,
    library1_name,
    function_addresses
}

{
    function2_names,
    0x00000000,
    0x00000000,
    library2_name,
    function_addresses + 4
}

library1_name:
{"msvcrt.dll"}

function1_names:
{function_1_1}
{0x00000000}

function_1_1:
{"printf"}


library2_name:
{"opengl32.dll"}

function2_names:
{function_2_1}
{0x00000000}

function_2_1:
{"printf"}
*/

void add_PE_import_section(PE* pe)
{
    N_32 functions_length = 0;
    for_each_buffer_element(&pe->import_libraries, PE_Import_Library, import_library)
    {
        functions_length += import_library.functions->length / sizeof(Buffer);
    }
    end_for_each_buffer_element

    N_32 libraries_length = pe->import_libraries.length / sizeof(PE_Import_Library);

    N_32 start_offset = pe->section_address;//0x1000;
    N_32 libraries_offset = start_offset + functions_length * sizeof(N_32) + libraries_length * sizeof(N_32);
    N_32 library_name_offset = libraries_offset + (pe->import_libraries.length / sizeof(PE_Import_Library) + 1) * sizeof(PE_Import_Directory);

    BUFFER_IO()
        //function addresses data - zeros
        for_each_buffer_element(&pe->import_libraries, PE_Import_Library, import_library)
        {
            for_each_buffer_element(import_library.functions, Buffer, function_name)
            {
                N_32 default_function_address = 1;
                write_byte_array(&output, &default_function_address, sizeof(default_function_address));
            }
            end_for_each_buffer_element
            N_32 zero = 0;
            write_byte_array(&output, &zero, sizeof(zero));
        }
        end_for_each_buffer_element

        N_32 library_index = 0;
        N_32 function_index = 0;
        for_each_buffer_element(&pe->import_libraries, PE_Import_Library, import_library)
        {
            N_32 function_name_pointers_offset = library_name_offset + get_aligned_size(import_library.name.length + 1, 4);
            PE_Import_Directory import_directory = {
                function_name_pointers_offset,    //function names pointer
                0x00000000,
                0x00000000,
                library_name_offset,              //library name
                start_offset + function_index * 4 //function addresses
            };
            write_byte_array(&output, &import_directory, sizeof(PE_Import_Directory));

            library_name_offset += get_aligned_size(import_library.name.length + 1, 4);
            library_name_offset += (import_library.functions->length / sizeof(Buffer) + 1) * 4;

            for_each_buffer_element(import_library.functions, Buffer, function_name)
            {
                library_name_offset += 2 + get_aligned_size(function_name.length + 1, 4);
                ++function_index;
            }
            end_for_each_buffer_element
            ++function_index; //для нуль терминирования в таблице адресов

            ++library_index;
        }
        end_for_each_buffer_element

        PE_Import_Directory import_directory = {
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000
        };
        write_byte_array(&output, &import_directory, sizeof(import_directory));

        N_32 function_name_offset = libraries_offset + (pe->import_libraries.length / sizeof(PE_Import_Library) + 1) * sizeof(PE_Import_Directory);
        for_each_buffer_element(&pe->import_libraries, PE_Import_Library, import_library)
        {
            //library name data
            for_each_buffer_element(&import_library.name, Byte, charachter)
            {
                write_byte(&output, charachter);
            }
            end_for_each_buffer_element
            cycle(import_library.name.length, get_aligned_size(import_library.name.length + 1, 4), 1)
                write_byte(&output, '\0');
            end
            function_name_offset += get_aligned_size(import_library.name.length + 1, 4);

            //function name pointers data
            function_name_offset += (import_library.functions->length / sizeof(Buffer) + 1) * 4;
            for_each_buffer_element(import_library.functions, Buffer, function_name)
            {
                N_32 zero = function_name_offset;//start_offset;
                write_byte_array(&output, &zero, sizeof(zero));
                function_name_offset += 2 + get_aligned_size(function_name.length + 1, 4);
            }
            end_for_each_buffer_element
            N_32 zero = 0;
            write_byte_array(&output, &zero, sizeof(zero));

            //function names data
            for_each_buffer_element(import_library.functions, Buffer, function_name)
            {
                N_16 function_hint = 0;
                write_byte_array(&output, &function_hint, sizeof(function_hint));

                for_each_buffer_element(&function_name, Byte, charachter)
                {
                    write_byte(&output, charachter);
                }
                end_for_each_buffer_element
                cycle(function_name.length, get_aligned_size(function_name.length + 1, 4), 1)
                    write_byte(&output, '\0');
                end
            }
            end_for_each_buffer_element
        }
        end_for_each_buffer_element

        flush_output(&output);

        add_section_in_PE(
            pe,
            "Import",
            INITIALIZED_DATA_PE_SECTION | ENABLE_READ_MEMORY_PE_SECTION | ENABLE_WRITE_MEMORY_PE_SECTION,
            &input
        );
    END_BUFFER_IO

    PE_Section import_section;
    get_buffer_bytes(&pe->sections, pe->sections.length - sizeof(PE_Section), &import_section, sizeof(PE_Section));

    pe->data_directories[1].address = libraries_offset;
    pe->data_directories[1].size = import_section.data.length - functions_length * 4;
}


void add_PE_code_section(PE* pe, Input* input)
{
    pe->entry_point_address = add_section_in_PE(
        pe,
        "Code",
        EXECUTABLE_CODE_PE_SECTION | INITIALIZED_DATA_PE_SECTION | ENABLE_EXECUTE_MEMORY_PE_SECTION | ENABLE_EXECUTE_MEMORY_PE_SECTION | ENABLE_READ_MEMORY_PE_SECTION,
        input
    ) - PE_IMAGE_BASE;
}
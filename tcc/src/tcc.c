	//s
/*
#include "tcc.h"
#if ONE_SOURCE
# include "libtcc.c"
#endif
#include "tcctools.c"*/


#include <types.c>


typedef struct
{

}
addr_t;


typedef struct
{

}
DLLReference;


typedef struct
{

}
jmp_buf;


typedef struct
{

}
FILE;


typedef struct
{

}
BufferedFile;


typedef struct
{
	Boolean verbose;       // if true, display some information during compilation
    Boolean nostdinc;      // if true, no standard headers are added
    Boolean nostdlib;      // if true, no standard libraries are added
    Boolean nocommon;      // if true, do not use common symbols for .bss data
    Boolean static_link;   // if true, static linking is performed
    Boolean rdynamic;      // if true, all symbols are exported
    Boolean symbolic;      // if true, resolve symbols in the current module first
    Boolean alacarte_link; // if true, only link in referenced objects from archive

    Byte*   tcc_lib_path;     // CONFIG_TCCDIR or -B option
    Byte*   soname;           // as specified on the command line (-soname)
    Byte*   rpath;            // as specified on the command line (-Wl,-rpath=)
    Boolean enable_new_dtags; // ditto, (-Wl,--enable-new-dtags)

    
    Number output_type;   // output type, see TCC_OUTPUT_XXX
    Number output_format; // output format, see TCC_OUTPUT_FORMAT_xxx

    // C language options
    Boolean char_is_unsigned;
    Boolean leading_underscore;
    Boolean ms_extensions;	        // allow nested named struct w/o identifier behave like unnamed
    Boolean dollars_in_identifiers;	// allows '$' char in identifiers
    Boolean ms_bitfields;           // if true, emulate MS algorithm for aligning bitfields

    // warning switches
    Boolean warn_write_strings;
    Boolean warn_unsupported;
    Boolean warn_error;
    Boolean warn_none;
    Boolean warn_implicit_function_declaration;
    Boolean warn_gcc_compat;
    
    Boolean do_debug; // compile with debug symbol (and use them if error during execution)
#ifdef CONFIG_TCC_BCHECK
    Boolean do_bounds_check; // compile with built-in memory and bounds checker
#endif
#ifdef TCC_TARGET_ARM
    enum float_abi float_abi; // float ABI of the generated code*/
#endif
    Boolean run_test; // nth test to run with -dt -run */

    addr_t text_addr; // address of text section */
    Boolean has_text_addr;

    Number section_align; // section alignment */

    Byte* init_symbol; // symbols to call at load-time (not used currently)
    Byte* fini_symbol; // symbols to call at unload-time (not used currently)

#ifdef TCC_TARGET_I386
    Number seg_size; // 32. Can be 16 with i386 assembler (.code16)
#endif
#ifdef TCC_TARGET_X86_64
    Boolean nosse; // For -mno-sse support.
#endif

    DLLReference** loaded_dlls; // array of all loaded dlls (including those referenced by loaded dlls)
    Number nb_loaded_dlls;

    Byte** include_paths;
    Number nb_include_paths;

    Byte** sysinclude_paths;
    Number nb_sysinclude_paths;

    Byte** library_paths;
    Number nb_library_paths;

    // crt?.o object path
    Byte** crt_paths;
    Number nb_crt_paths;

    Byte** cmd_include_files;
    Number nb_cmd_include_files;

    // error handling
    void *error_opaque;
    void (*error_func)(void* opaque, Byte* msg);
    Boolean error_set_jmp_enabled;
    jmp_buf error_jmp_buf;
    Number nb_errors;

    // output file for preprocessing (-E)
    FILE *ppfp;
    enum {
	LINE_MACRO_OUTPUT_FORMAT_GCC,
	LINE_MACRO_OUTPUT_FORMAT_NONE,
	LINE_MACRO_OUTPUT_FORMAT_STD,
    LINE_MACRO_OUTPUT_FORMAT_P10 = 11
    } Pflag; /* -P switch */
    char dflag; /* -dX value */

    // for -MD/-MF: collected dependencies for this compilation
    char **target_deps;
    int nb_target_deps;
/*
    // compilation
    BufferedFile *include_stack[INCLUDE_STACK_SIZE];
    BufferedFile **include_stack_ptr;

    int ifdef_stack[IFDEF_STACK_SIZE];
    int *ifdef_stack_ptr;

    // included files enclosed with #ifndef MACRO
    int cached_includes_hash[CACHED_INCLUDES_HASH_SIZE];
    CachedInclude **cached_includes;
    int nb_cached_includes;

    // #pragma pack stack
    int pack_stack[PACK_STACK_SIZE];
    int *pack_stack_ptr;
    char **pragma_libs;
    int nb_pragma_libs;

    // inline functions are stored as token lists and compiled last only if referenced
    struct InlineFunc **inline_fns;
    int nb_inline_fns;

    Section **sections;
    int nb_sections;

    Section **priv_sections;
    int nb_priv_sections;

    Section *got;
    Section *plt;

    // temporary dynamic symbol sections (for dll loading)
    Section *dynsymtab_section;
    // exported dynamic symbol section
    Section *dynsym;
    // copy of the global symtab_section variable
    Section *symtab;
    // extra attributes (eg. GOT/PLT value) for symtab symbols
    struct sym_attr *sym_attrs;
    int nb_sym_attrs;

#ifdef TCC_TARGET_PE
    // PE info
    int pe_subsystem;
    unsigned pe_characteristics;
    unsigned pe_file_align;
    unsigned pe_stack_size;
    addr_t pe_imagebase;
# ifdef TCC_TARGET_X86_64
    Section *uw_pdata;
    int uw_sym;
    unsigned uw_offs;
# endif
#endif

#ifdef TCC_IS_NATIVE
    const char *runtime_main;
    void **runtime_mem;
    int nb_runtime_mem;
#endif
*/
    // used by main and tcc_parse_args only
    struct filespec** files; // files seen on command line
    Number  nb_files;
    Number  nb_libraries;
    Number  filetype;
    Byte*   outfile;
    Boolean option_r;       // option -r
    Boolean do_bench;       // option -bench
    Boolean gen_deps;       // option -MD
    Byte*   deps_outfile;   // option -MF
    Boolean option_pthread; // -pthread option
    Number  argc;
    Byte**  argv;
}
TCC_State;


#include "parser.c"


Boolean tcc_run(TCC_State* state, Number number_of_arguments, Byte** arguments)
{
    Number (*prog_main)(Number number_of_arguments, Byte** arguments);

    return prog_main;
/*
    state->runtime_main = "main";
    if((state->dflag & 16) && !find_elf_sym(state->symtab, state->runtime_main))
        return 0;
    if(tcc_relocate(s1, TCC_RELOCATE_AUTO) < 0)
        return -1;
    prog_main = tcc_get_symbol_err(s1, s1->runtime_main);

#ifdef CONFIG_TCC_BACKTRACE
    if (s1->do_debug) {
        set_exception_handler();
        rt_prog_main = prog_main;
    }
#endif

    errno = 0; // clean errno value

#ifdef CONFIG_TCC_BCHECK
    if (s1->do_bounds_check) {
        void (*bound_init)(void);
        void (*bound_exit)(void);
        void (*bound_new_region)(void *p, addr_t size);
        int  (*bound_delete_region)(void *p);
        int i, ret;

        // set error function
        rt_bound_error_msg = tcc_get_symbol_err(s1, "__bound_error_msg");
        // XXX: use .init section so that it also work in binary ?
        bound_init = tcc_get_symbol_err(s1, "__bound_init");
        bound_exit = tcc_get_symbol_err(s1, "__bound_exit");
        bound_new_region = tcc_get_symbol_err(s1, "__bound_new_region");
        bound_delete_region = tcc_get_symbol_err(s1, "__bound_delete_region");

        bound_init();
        // mark argv area as valid
        bound_new_region(argv, argc*sizeof(argv[0]));
        for (i=0; i<argc; ++i)
            bound_new_region(argv[i], strlen(argv[i]) + 1);

        ret = (*prog_main)(argc, argv);

        // unmark argv area
        for (i=0; i<argc; ++i)
            bound_delete_region(argv[i]);
        bound_delete_region(argv);
        bound_exit();
        return ret;
    }
#endif
    return (*prog_main)(argc, argv);*/
}


Number main(Number32 number_of_arguments, Byte** arguments)
{
	TCC_State state;

	//return tcc_run(&state, number_of_arguments, arguments);
	//return tcc_output_file(&state, state.outfile);

	print(tcc_keywords)

	return 0;
}

/*
static const char help[] =
    "Tiny C Compiler "TCC_VERSION" - Copyright (C) 2001-2006 Fabrice Bellard\n"
    "Usage: tcc [options...] [-o outfile] [-c] infile(s)...\n"
    "       tcc [options...] -run infile [arguments...]\n"
    "General options:\n"
    "  -c          compile only - generate an object file\n"
    "  -o outfile  set output filename\n"
    "  -run        run compiled source\n"
    "  -fflag      set or reset (with 'no-' prefix) 'flag' (see tcc -hh)\n"
    "  -Wwarning   set or reset (with 'no-' prefix) 'warning' (see tcc -hh)\n"
    "  -w          disable all warnings\n"
    "  -v -vv      show version, show search paths or loaded files\n"
    "  -h -hh      show this, show more help\n"
    "  -bench      show compilation statistics\n"
    "  -           use stdin pipe as infile\n"
    "  @listfile   read arguments from listfile\n"
    "Preprocessor options:\n"
    "  -Idir       add include path 'dir'\n"
    "  -Dsym[=val] define 'sym' with value 'val'\n"
    "  -Usym       undefine 'sym'\n"
    "  -E          preprocess only\n"
    "Linker options:\n"
    "  -Ldir       add library path 'dir'\n"
    "  -llib       link with dynamic or static library 'lib'\n"
    "  -r          generate (relocatable) object file\n"
    "  -shared     generate a shared library/dll\n"
    "  -rdynamic   export all global symbols to dynamic linker\n"
    "  -soname     set name for shared library to be used at runtime\n"
    "  -Wl,-opt[=val]  set linker option (see tcc -hh)\n"
    "Debugger options:\n"
    "  -g          generate runtime debug info\n"
#ifdef CONFIG_TCC_BCHECK
    "  -b          compile with built-in memory and bounds checker (implies -g)\n"
#endif
#ifdef CONFIG_TCC_BACKTRACE
    "  -bt N       show N callers in stack traces\n"
#endif
    "Misc. options:\n"
    "  -x[c|a|n]   specify type of the next infile\n"
    "  -nostdinc   do not use standard system include paths\n"
    "  -nostdlib   do not link with standard crt and libraries\n"
    "  -Bdir       set tcc's private include/library dir\n"
    "  -MD         generate dependency file for make\n"
    "  -MF file    specify dependency file name\n"
    "  -m32/64     defer to i386/x86_64 cross compiler\n"
    "Tools:\n"
    "  create library  : tcc -ar [rcsv] lib.a files\n"
#ifdef TCC_TARGET_PE
    "  create def file : tcc -impdef lib.dll [-v] [-o lib.def]\n"
#endif
    ;

static const char help2[] =
    "Tiny C Compiler "TCC_VERSION" - More Options\n"
    "Special options:\n"
    "  -P -P1                        with -E: no/alternative #line output\n"
    "  -dD -dM                       with -E: output #define directives\n"
    "  -pthread                      same as -D_REENTRANT and -lpthread\n"
    "  -On                           same as -D__OPTIMIZE__ for n > 0\n"
    "  -Wp,-opt                      same as -opt\n"
    "  -include file                 include 'file' above each input file\n"
    "  -isystem dir                  add 'dir' to system include path\n"
    "  -static                       link to static libraries (not recommended)\n"
    "  -dumpversion                  print version\n"
    "  -print-search-dirs            print search paths\n"
    "  -dt                           with -run/-E: auto-define 'test_...' macros\n"
    "Ignored options:\n"
    "  --param  -pedantic  -pipe  -s  -std  -traditional\n"
    "-W... warnings:\n"
    "  all                           turn on some (*) warnings\n"
    "  error                         stop after first warning\n"
    "  unsupported                   warn about ignored options, pragmas, etc.\n"
    "  write-strings                 strings are const\n"
    "  implicit-function-declaration warn for missing prototype (*)\n"
    "-f[no-]... flags:\n"
    "  unsigned-char                 default char is unsigned\n"
    "  signed-char                   default char is signed\n"
    "  common                        use common section instead of bss\n"
    "  leading-underscore            decorate extern symbols\n"
    "  ms-extensions                 allow anonymous struct in struct\n"
    "  dollars-in-identifiers        allow '$' in C symbols\n"
    "-m... target specific options:\n"
    "  ms-bitfields                  use MSVC bitfield layout\n"
#ifdef TCC_TARGET_ARM
    "  float-abi                     hard/softfp on arm\n"
#endif
#ifdef TCC_TARGET_X86_64
    "  no-sse                        disable floats on x86_64\n"
#endif
    "-Wl,... linker options:\n"
    "  -nostdlib                     do not link with standard crt/libs\n"
    "  -[no-]whole-archive           load lib(s) fully/only as needed\n"
    "  -export-all-symbols           same as -rdynamic\n"
    "  -image-base= -Ttext=          set base address of executable\n"
    "  -section-alignment=           set section alignment in executable\n"
#ifdef TCC_TARGET_PE
    "  -file-alignment=              set PE file alignment\n"
    "  -stack=                       set PE stack reserve\n"
    "  -large-address-aware          set related PE option\n"
    "  -subsystem=[console/windows]  set PE subsystem\n"
    "  -oformat=[pe-* binary]        set executable output format\n"
    "Predefined macros:\n"
    "  tcc -E -dM - < nul\n"
#else
    "  -rpath=                       set dynamic library search path\n"
    "  -enable-new-dtags             set DT_RUNPATH instead of DT_RPATH\n"
    "  -soname=                      set DT_SONAME elf tag\n"
    "  -Bsymbolic                    set DT_SYMBOLIC elf tag\n"
    "  -oformat=[elf32/64-* binary]  set executable output format\n"
    "  -init= -fini= -as-needed -O   (ignored)\n"
    "Predefined macros:\n"
    "  tcc -E -dM - < /dev/null\n"
#endif
    "See also the manual for more details.\n"
    ;

static const char version[] =
    "tcc version "TCC_VERSION" ("
#ifdef TCC_TARGET_I386
        "i386"
#elif defined TCC_TARGET_X86_64
        "x86_64"
#elif defined TCC_TARGET_C67
        "C67"
#elif defined TCC_TARGET_ARM
        "ARM"
#elif defined TCC_TARGET_ARM64
        "AArch64"
#endif
#ifdef TCC_ARM_HARDFLOAT
        " Hard Float"
#endif
#ifdef TCC_TARGET_PE
        " Windows"
#elif defined(TCC_TARGET_MACHO)
        " Darwin"
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
        " FreeBSD"
#else
        " Linux"
#endif
    ")\n"
    ;

static void print_dirs(const char *msg, char **paths, int nb_paths)
{
    int i;
    printf("%s:\n%s", msg, nb_paths ? "" : "  -\n");
    for(i = 0; i < nb_paths; i++)
        printf("  %s\n", paths[i]);
}

static void print_search_dirs(TCCState *s)
{
    printf("install: %s\n", s->tcc_lib_path);
    // print_dirs("programs", NULL, 0);
    print_dirs("include", s->sysinclude_paths, s->nb_sysinclude_paths);
    print_dirs("libraries", s->library_paths, s->nb_library_paths);
    printf("libtcc1:\n  %s/"TCC_LIBTCC1"\n", s->tcc_lib_path);
#ifndef TCC_TARGET_PE
    print_dirs("crt", s->crt_paths, s->nb_crt_paths);
    printf("elfinterp:\n  %s\n",  DEFAULT_ELFINTERP(s));
#endif
}

static void set_environment(TCCState *s)
{
    char * path;

    path = getenv("C_INCLUDE_PATH");
    if(path != NULL) {
        tcc_add_sysinclude_path(s, path);
    }
    path = getenv("CPATH");
    if(path != NULL) {
        tcc_add_include_path(s, path);
    }
    path = getenv("LIBRARY_PATH");
    if(path != NULL) {
        tcc_add_library_path(s, path);
    }
}

static char *default_outputfile(TCCState *s, const char *first_file)
{
    char buf[1024];
    char *ext;
    const char *name = "a";

    if (first_file && strcmp(first_file, "-"))
        name = tcc_basename(first_file);
    snprintf(buf, sizeof(buf), "%s", name);
    ext = tcc_fileextension(buf);
#ifdef TCC_TARGET_PE
    if (s->output_type == TCC_OUTPUT_DLL)
        strcpy(ext, ".dll");
    else
    if (s->output_type == TCC_OUTPUT_EXE)
        strcpy(ext, ".exe");
    else
#endif
    if (s->output_type == TCC_OUTPUT_OBJ && !s->option_r && *ext)
        strcpy(ext, ".o");
    else
        strcpy(buf, "a.out");
    return tcc_strdup(buf);
}

static unsigned getclock_ms(void)
{
#ifdef _WIN32
    return GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + (tv.tv_usec+500)/1000;
#endif
}

int main(int argc0, char **argv0)
{
    TCCState *s;
    int ret, opt, n = 0, t = 0;
    unsigned start_time = 0;
    const char *first_file;
    int argc; char **argv;
    FILE *ppfp = stdout;

redo:
    argc = argc0, argv = argv0;
    s = tcc_new();
    opt = tcc_parse_args(s, &argc, &argv, 1);

    if ((n | t) == 0) {
        if (opt == OPT_HELP)
            return printf(help), 1;
        if (opt == OPT_HELP2)
            return printf(help2), 1;
        if (opt == OPT_M32 || opt == OPT_M64)
            tcc_tool_cross(s, argv, opt); // never returns
        if (s->verbose)
            printf(version);
        if (opt == OPT_AR)
            return tcc_tool_ar(s, argc, argv);
#ifdef TCC_TARGET_PE
        if (opt == OPT_IMPDEF)
            return tcc_tool_impdef(s, argc, argv);
#endif
        if (opt == OPT_V)
            return 0;
        if (opt == OPT_PRINT_DIRS) {
            // initialize search dirs
            set_environment(s);
            tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
            print_search_dirs(s);
            return 0;
        }

        n = s->nb_files;
        if (n == 0)
            tcc_error("no input files\n");

        if (s->output_type == TCC_OUTPUT_PREPROCESS) {
            if (s->outfile) {
                ppfp = fopen(s->outfile, "w");
                if (!ppfp)
                    tcc_error("could not write '%s'", s->outfile);
            }
        } else if (s->output_type == TCC_OUTPUT_OBJ && !s->option_r) {
            if (s->nb_libraries)
                tcc_error("cannot specify libraries with -c");
            if (n > 1 && s->outfile)
                tcc_error("cannot specify output file with -c many files");
        } else {
            if (s->option_pthread)
                tcc_set_options(s, "-lpthread");
        }

        if (s->do_bench)
            start_time = getclock_ms();
    }

    set_environment(s);
    if (s->output_type == 0)
        s->output_type = TCC_OUTPUT_EXE;
    tcc_set_output_type(s, s->output_type);
    s->ppfp = ppfp;

    if ((s->output_type == TCC_OUTPUT_MEMORY
      || s->output_type == TCC_OUTPUT_PREPROCESS) && (s->dflag & 16))
        s->dflag |= t ? 32 : 0, s->run_test = ++t, n = s->nb_files;

    // compile or add each files or library
    for (first_file = NULL, ret = 0;;) {
        struct filespec *f = s->files[s->nb_files - n];
        s->filetype = f->type;
        s->alacarte_link = f->alacarte;
        if (f->type == AFF_TYPE_LIB) {
            if (tcc_add_library_err(s, f->name) < 0)
                ret = 1;
        } else {
            if (1 == s->verbose)
                printf("-> %s\n", f->name);
            if (!first_file)
                first_file = f->name;
            if (tcc_add_file(s, f->name) < 0)
                ret = 1;
        }
        s->filetype = 0;
        s->alacarte_link = 1;
        if (--n == 0 || ret
            || (s->output_type == TCC_OUTPUT_OBJ && !s->option_r))
            break;
    }

    if (s->run_test) {
        t = 0;
    } else if (s->output_type == TCC_OUTPUT_PREPROCESS) {
        ;
    } else if (0 == ret) {
        if (s->output_type == TCC_OUTPUT_MEMORY) {
#ifdef TCC_IS_NATIVE
            ret = tcc_run(s, argc, argv);
#endif
        } else {
            if (!s->outfile)
                s->outfile = default_outputfile(s, first_file);
            if (tcc_output_file(s, s->outfile))
                ret = 1;
            else if (s->gen_deps)
                gen_makedeps(s, s->outfile, s->deps_outfile);
        }
    }

    if (s->do_bench && (n | t | ret) == 0)
        tcc_print_stats(s, getclock_ms() - start_time);
    tcc_delete(s);
    if (ret == 0 && n)
        goto redo; // compile more files with -c
    if (t)
        goto redo; // run more tests with -dt -run
    if (ppfp && ppfp != stdout)
        fclose(ppfp);
    return ret;
}
*/
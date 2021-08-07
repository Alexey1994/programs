#ifndef LZIO_INCLUDED
#define LZIO_INCLUDED


//#include <types.c>
typedef unsigned char          Byte;

typedef unsigned int           Number;
typedef unsigned char          Number8;
typedef unsigned short int     Number16;
typedef unsigned long int      Number32;

typedef int                    Integer_Number;


#define zFopen luaZ_Fopen
#define zsopen luaZ_sopen
#define zmopen luaZ_mopen
#define zread luaZ_read

#define EOZ (-1)


#define ZBSIZE 256

typedef struct
{
	Number n;   // bytes still unread
	Byte*  p;   // current position in buffer
	void*  u;   // additional data
	Byte*  name;
	Byte   buffer[ZBSIZE];
	Number (*filbuf)(struct ZIO* z);
}
ZIO;


#define zgetc(z)  (((z)->n--)>0 ? ((int)*(z)->p++): (z)->filbuf(z))
#define zungetc(z)  (++(z)->n,--(z)->p)
#define zname(z)  ((z)->name)
/*
Byte zungetc(ZIO* z)
{
	++z->n;
	--z->p;
	return z->p;
}*/


Number zread(ZIO* z, Byte* b, Number n)
{
	Number min_size;

	while(n)
	{
		if(z->n == 0)
		{
			if(z->filbuf(z) == EOZ)
				return n; // return number of missing bytes

			zungetc(z); // put result from `filbuf' in the buffer
		}

		if(n <= z->n)
			min_size = n;
		else
			min_size = z->n;

		memcpy(b, z->p, min_size);
		z->n -= min_size;
		z->p += min_size;
		b = b + min_size;
		n -= min_size;
	}

	return 0;
}


// memory buffers

static Number zmfilbuf(ZIO* z)
{
	return EOZ;
}


ZIO* zmopen(ZIO* z, Byte* b, Number size, Byte* name)
{
	if(!b)
		return 0;

	z->n      = size;
	z->p      = b;
	z->filbuf = &zmfilbuf;
	z->u      = 0;
	z->name   = name;

	return z;
}


// strings

ZIO* zsopen(ZIO* z, Byte* s, Byte* name) {
	if(!s)
		return 0;

	return zmopen(z, s, strlen(s), name);
}


// FILEs

static int zffilbuf(ZIO* z) {
	size_t n;

	if(feof((FILE*)z->u))
		return EOZ;

	n = fread(z->buffer, 1, ZBSIZE, (FILE *)z->u);
	
	if(!n)
		return EOZ;
	
	z->n = n-1;
	z->p = z->buffer;
	
	return *(z->p++);
}


ZIO* zFopen(ZIO* z, FILE* f, Byte* name) {
	if(!f)
		return 0;
  
	z->n      = 0;
	z->p      = z->buffer;
	z->filbuf = &zffilbuf;
	z->u      = f;
	z->name   = name;

	return z;
}


#endif//LZIO_INCLUDED
#ifndef LMEM_INCLUDED
#define LMEM_INCLUDED


void luaD_breakrun (lua_State *L, int errcode);

Byte* luaM_realloc(struct lua_State* L, Byte* block, Number size)
{
	if(size == 0)
	{
		free(block);
		return 0;
	}
	else if(size >= MAX_SIZET)
		lua_error(L, "memory allocation error: block too big");
	
	block = realloc(block, size);
	
	if(block == NULL)
	{
		if (L)
			luaD_breakrun(L, LUA_ERRMEM);
		else
			return 0;
	}

	return block;
}


Byte* luaM_growaux(struct lua_State* L, Byte* block, Number nelems, Integer_Number inc, Number size, Byte* errormsg, Number limit) {
	size_t newn = nelems+inc;

	if (nelems >= limit - inc)
		lua_error(L, errormsg);

	if((newn ^ nelems) <= nelems ||  // still the same power-of-2 limit?
		(nelems > 0 && newn < MINPOWER2))  // or block already is MINPOWER2?
		return block;  // do not need to reallocate
	else  // it crossed a power-of-2 boundary; grow to next power
		return luaM_realloc(L, block, luaO_power2(newn)*size);
}


#define luaM_free(L, b)		luaM_realloc(L, (b), 0)
#define luaM_malloc(L, t)	luaM_realloc(L, NULL, (t))
#define luaM_new(L, t)          ((t *)luaM_malloc(L, sizeof(t)))
#define luaM_newvector(L, n,t)  ((t *)luaM_malloc(L, (n)*(Number)sizeof(t)))

#define luaM_growvector(L, v,nelems,inc,t,e,l) \
          ((v)=(t *)luaM_growaux(L, v,nelems,inc,sizeof(t),e,l))

#define luaM_reallocvector(L, v,n,t) \
	((v)=(t *)luaM_realloc(L, v,(n)*(Number)sizeof(t)))


#endif//LMEM_INCLUDED
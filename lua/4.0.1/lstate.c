#ifndef LSTATE_INCLUDED
#define LSTATE_INCLUDED




// marks for Reference array
typedef enum
{
	NONEXT         = -1, // to end the free list
	HOLD           = -2,
	COLLECTED      = -3,
	LOCK           = -4,
}
Lua_Reference;


typedef struct
{
	TObject o;
	int     st; // can be LOCK, HOLD, COLLECTED, or next (for free list)
}
Ref;


struct lua_longjmp;  // defined in ldo.c
struct TM;           // defined in ltm.h


typedef struct stringtable
{
	int       size;
	Number32  nuse;  // number of elements
	TString** hash;
}
stringtable;




static Number errormessage (lua_State* L) {
	Byte* message;

	message = lua_tostring(L, 1);
	
	if (message == NULL)
		message = "(no message)";

	print("error: ", message, "\n");

	return 0;
}


static void f_luaopen (lua_State* L, void* ud) {
	int stacksize;

	stacksize = *(int*)ud;

	if (stacksize == 0)
		stacksize = DEFAULT_STACK_SIZE;
	else
		stacksize += LUA_MINSTACK;
	
	L->gt = luaH_new(L, 10);  // table of globals
	luaD_init(L, stacksize);
	luaS_init(L);
	luaX_init(L);
	luaT_init(L);
	lua_newtable(L);
	lua_ref(L, 1);  // create registry
	lua_register(L, LUA_ERRORMESSAGE, errormessage);
}


LUA_API void lua_close (lua_State* L)
{
	luaC_collect(L, 1);
	luaS_freeall(L);

	if(L->stack)
		L->nblocks -= (L->stack_last - L->stack + 1) * sizeof(TObject);
	
	luaM_free(L, L->stack);
	L->nblocks -= (L->last_tag + 1) * sizeof(struct TM);
	luaM_free(L, L->TMtable);
	L->nblocks -= (L->refSize) * sizeof(Ref);
	luaM_free(L, L->refArray);
	L->nblocks -= (L->Mbuffsize) * sizeof(char);
	luaM_free(L, L->Mbuffer);
	luaM_free(L, L);
}


LUA_API lua_State* lua_open (int stacksize)
{
	lua_State* L;

	L = luaM_new(NULL, lua_State);
	
	if(!L)
		return NULL;

	L->stack       = NULL;
	L->strt.size   = L->udt.size = 0;
	L->strt.nuse   = L->udt.nuse = 0;
	L->strt.hash   = NULL;
	L->udt.hash    = NULL;
	L->Mbuffer     = NULL;
	L->Mbuffsize   = 0;
	L->rootproto   = NULL;
	L->rootcl      = NULL;
	L->roottable   = NULL;
	L->TMtable     = NULL;
	L->last_tag    = -1;
	L->refArray    = NULL;
	L->refSize     = 0;
	L->refFree     = NONEXT;
	L->nblocks     = sizeof(lua_State);
	L->GCthreshold = MAX_INT;  // to avoid GC during pre-definitions
	L->callhook    = NULL;
	L->linehook    = NULL;
	L->allowhooks  = 1;
	L->errorJmp    = NULL;

	if(luaD_runprotected(L, f_luaopen, &stacksize)) {
		lua_close(L);
		return NULL;
	}

	L->GCthreshold = L->nblocks * 2;

	return L;
}


#endif//LSTATE_INCLUDED
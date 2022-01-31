typedef enum
{
	CA_DISCONNECTED,
	CA_ACTIVE,
	CA_CINEMATIC,
	CA_CONNECTING,
	CA_CHALLENGING,
	CA_CONNECTED,
	CA_LOADING,
	CA_PRIMED
}
Client_State;

typedef struct
{
	Client_State state;
	Number       framecount;
}
Client;


Client static_client;


#include "game.c"
#include "screen.c"


void CL_Init(Number time)
{
	static_client.state = CA_DISCONNECTED;

	SCR_Init();
}


void CL_Frame(Number time)
{
	/*
	// see if we need to update any userinfo
	CL_CheckUserinfo();

	// if we haven't gotten a packet in a long time,
	// drop the connection
	CL_CheckTimeout();

	// send intentions now
	CL_SendCmd();

	// resend a connection request if necessary
	CL_CheckForResend();

	// decide on the serverTime to render
	CL_SetCGameTime();*/

	// update the screen
	SCR_UpdateScreen();

	// update audio
	/*S_Update();

	// advance local effects for next frame
	SCR_RunCinematic();

	Con_RunConsole();*/

	static_client.framecount++;
}
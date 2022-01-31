void SCR_Init()
{

}


void SCR_UpdateScreen()
{
	switch(static_client.state)
	{
		case CA_ACTIVE:
			//CL_CGameRendering();
			CG_DrawActiveFrame();
			break;

		case CA_DISCONNECTED:
			break;

		default:
			print("SCR_UpdateScreen: Неизвестное состояние клиента ", _Number, static_client.state, "\n");
	}
}
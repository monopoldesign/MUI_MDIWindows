/******************************************************************************
* MUI-Custom-Class
* (C)2022 M.Volkel (mario.volkel@outlook.com)
*******************************************************************************/

// Comment templates

/******************************************************************************
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
-
------------------------------------------------------------------------------*/

/*********************ß*********************************************************
* Header-Files
*******************************************************************************/
#include "main.h"
#include "slaveW.h"
#include "help.h"

/******************************************************************************
* Class-Methods
*******************************************************************************/

/*-----------------------------------------------------------------------------
- OM_NEW
------------------------------------------------------------------------------*/
ULONG slaveW_New(struct IClass *cl, Object *obj, struct opSet *msg)
{
	struct slaveW_Data tmp = {0};

	if (obj = (Object *)DoSuperNew(cl, obj,
		MUIA_Window_Title,			"MUI_MDI_Slave",
		MUIA_Window_ID,				MAKE_ID('M', 'A', 'I', 'N'),
		WindowContents,				tmp.BT_Close = SimpleButton("Close"),
		MUIA_Window_Screen,			myScreen,
		TAG_MORE, msg->ops_AttrList))
	{
		struct slaveW_Data *data = INST_DATA(cl, obj);
		*data = tmp;

		DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 2, MUIM_slaveW_Finish, 0);
		DoMethod(data->BT_Close, MUIM_Notify, MUIA_Pressed, FALSE, obj, 2, MUIM_slaveW_Finish, 0);

		return (ULONG)obj;
	}
	return 0;
}

ULONG slaveW_Finish(struct IClass *cl, Object *obj, struct MUIP_slaveW_Finish *msg)
{
	struct slaveW_Data *data = INST_DATA(cl, obj);

	// Close Window
	set(obj, MUIA_Window_Open, FALSE);

	// Remove Window from ApplicationObject
	DoMethod((Object *)xget(obj, MUIA_ApplicationObject), OM_REMMEMBER, obj);

	// Delete Window
	MUI_DisposeObject(obj);
	win2 = NULL;

	return 0;
}

/*-----------------------------------------------------------------------------
- Dispatcher
------------------------------------------------------------------------------*/
DISPATCHER(slaveW_Dispatcher)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return slaveW_New(cl, obj, (APTR)msg);
		case MUIM_slaveW_Finish:
			return slaveW_Finish(cl, obj, (APTR)msg);
	}

	return DoSuperMethodA(cl, obj, msg);
}

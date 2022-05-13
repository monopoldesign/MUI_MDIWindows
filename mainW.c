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
#include "mainW.h"
#include "help.h"

/******************************************************************************
* Class-Methods
*******************************************************************************/

/*-----------------------------------------------------------------------------
- OM_NEW
------------------------------------------------------------------------------*/
ULONG mainW_New(struct IClass *cl, Object *obj, struct opSet *msg)
{
	struct mainW_Data tmp = {0};

	if (obj = (Object *)DoSuperNew(cl, obj,
		MUIA_Window_Title,			"MUI_MDI",
		MUIA_Window_ID,				MAKE_ID('M', 'A', 'I', 'N'),
		WindowContents,				tmp.BT_New = SimpleButton("New Window"),
		MUIA_Window_Screen,			myScreen,
		TAG_MORE, msg->ops_AttrList))
	{
		struct mainW_Data *data = INST_DATA(cl, obj);
		*data = tmp;

		DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 2, MUIM_mainW_Finish, 0);
		DoMethod(data->BT_New, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_mainW_newWindow);

		return (ULONG)obj;
	}
	return 0;
}

ULONG mainW_Finish(struct IClass *cl, Object *obj, struct MUIP_mainW_Finish *msg)
{
	struct mainW_Data *data = INST_DATA(cl, obj);

	DoMethod((Object *)xget(obj, MUIA_ApplicationObject), MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	return 0;
}

ULONG mainW_newWindow(struct IClass *cl, Object *obj, Msg msg)
{
	struct mainW_Data *data = INST_DATA(cl, obj);

	set((Object *)xget(obj, MUIA_ApplicationObject), MUIA_Application_Sleep, TRUE);

	if (!win2)
	{
		if (win2 = (Object *)NewObject(CL_mainW->mcc_Class, NULL, TAG_DONE))
		{
			DoMethod((Object *)xget(obj, MUIA_ApplicationObject), OM_ADDMEMBER, win2);
			set(win2, MUIA_Window_Open, TRUE);
		}
	}

	set((Object *)xget(obj, MUIA_ApplicationObject), MUIA_Application_Sleep, FALSE);
	return 0;
}

/*-----------------------------------------------------------------------------
- Dispatcher
------------------------------------------------------------------------------*/
DISPATCHER(mainW_Dispatcher)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return mainW_New(cl, obj, (APTR)msg);
		case MUIM_mainW_Finish:
			return mainW_Finish(cl, obj, (APTR)msg);
		case MUIM_mainW_newWindow:
			return mainW_newWindow(cl, obj, (APTR)msg);
	}

	return DoSuperMethodA(cl, obj, msg);
}

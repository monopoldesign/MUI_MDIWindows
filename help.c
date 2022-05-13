/******************************************************************************
* MUI_MDIWindows
* (C)2022 M.Volkel (mario.volkel@outlook.com)
*******************************************************************************/

// Comment templates

/******************************************************************************
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
-
------------------------------------------------------------------------------*/

/******************************************************************************
* Header-Files
*******************************************************************************/
#include "main.h"
#include "mainW.h"
#include "help.h"

/******************************************************************************
* Help-Functions
*******************************************************************************/
ULONG DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1, ...)
{
	return (DoSuperMethod(cl, obj, OM_NEW, &tag1, NULL));
}

LONG xget(Object *obj, ULONG attribute)
{
	LONG x;
	get(obj, attribute, &x);
	return(x);
}

/*-----------------------------------------------------------------------------
- init()
------------------------------------------------------------------------------*/
BOOL initLibs(void)
{
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37);
	MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN);

	if (IntuitionBase && MUIMasterBase)
		return TRUE;

	exitLibs();
	return FALSE;
}

/*-----------------------------------------------------------------------------
- end()
------------------------------------------------------------------------------*/
void exitLibs(void)
{
	if (MUIMasterBase)
		CloseLibrary((struct Library *)MUIMasterBase);

	if (IntuitionBase)
		CloseLibrary((struct Library *)IntuitionBase);

	exit(0);
}

/*-----------------------------------------------------------------------------
- DisposeApp()
------------------------------------------------------------------------------*/
BOOL initClasses(void)
{
	CL_mainW = MUI_CreateCustomClass(NULL, MUIC_Window, NULL, sizeof(struct mainW_Data), mainW_Dispatcher);

	if (CL_mainW)
		return TRUE;

	exitClasses();
	return FALSE;
}

/*-----------------------------------------------------------------------------
- DisposeApp()
------------------------------------------------------------------------------*/
void exitClasses(void)
{
	if (CL_mainW)
		MUI_DeleteCustomClass(CL_mainW);
}

/*-----------------------------------------------------------------------------
- DisposeApp()
------------------------------------------------------------------------------*/
BOOL SetupScreen(void)
{
	if (!(myScreen = OpenScreenTags(NULL, 
		SA_LikeWorkbench, TRUE,
		SA_Type, PUBLICSCREEN,
		SA_PubName, screenName,
		SA_Title, "MUI_MDIWindows V0.1, (C)2022 M.Volkel",
		SA_ShowTitle, TRUE,
		TAG_DONE)))
	{
		return FALSE;
	}

	while (PubScreenStatus(myScreen, NULL) == 0);

	LockPubScreen(screenName);
	return TRUE;
}

/*-----------------------------------------------------------------------------
- DisposeApp()
------------------------------------------------------------------------------*/
void CloseDownScreen(void)
{
	UnlockPubScreen(NULL, myScreen);
	if (myScreen)
	{
		CloseScreen(myScreen);
		myScreen = NULL;
	}
}
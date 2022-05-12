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
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

#include <exec/memory.h>
#include <libraries/mui.h>
#include <proto/exec.h>

#include <pragma/gadtools_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/muimaster_lib.h>

/******************************************************************************
* Macros
*******************************************************************************/
#define HOOKPROTONH(name, ret, obj, param) ret name(register __a2 obj, register __a1 param)
#define MakeHook(hookName, hookFunc) struct Hook hookName = {{NULL, NULL}, (HOOKFUNC)hookFunc, NULL, NULL}
#define DISPATCHER(name) LONG name(register __a0 Class *cl, register __a2 Object *obj, register __a1 Msg msg)

/******************************************************************************
* Prototypes
*******************************************************************************/
BOOL initLibs(void);
void exitLibs(void);
BOOL initClasses(void);
void exitClasses(void);
BOOL SetupScreen(void);
void CloseDownScreen(void);

/******************************************************************************
* Definitions
*******************************************************************************/
#define MAKE_ID(a, b, c, d) ((ULONG)(a) << 24 | (ULONG)(b) << 16 | (ULONG)(c) << 8 | (ULONG)(d))

/******************************************************************************
* Global Variables
*******************************************************************************/
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;

char buffer[40];
struct ObjApp *App = NULL;

struct Screen *myScreen;
UBYTE *screenName = "MDISCREEN";

struct MUI_CustomClass *CL_mainW;
Object *win2 = NULL;

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

/******************************************************************************
* MUI-Custom-Class
*******************************************************************************/
/*-----------------------------------------------------------------------------
- Definitions/Variables
------------------------------------------------------------------------------*/
#define TAGBASE_CLASS (TAG_USER | 0x80420000)
#define MUIM_mainW_Finish		TAGBASE_CLASS + 1
#define MUIM_mainW_newWindow	TAGBASE_CLASS + 2

struct mainW_Data
{
	Object *BT_New;
};

struct MUIP_mainW_Finish
{
	ULONG MethodID;
	LONG level;
};

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

/******************************************************************************
* Main-Program
*******************************************************************************/

/*-----------------------------------------------------------------------------
- main()
------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	Object *app, *win;
	BOOL running = TRUE;
	ULONG signal;

	if (initLibs())
	{
		if (SetupScreen())
		{
			if (initClasses())
			{
				app = ApplicationObject,
					MUIA_Application_Author,		"M.Volkel",
					MUIA_Application_Base,			"MUIMDI",
					MUIA_Application_Title,			"MUI_MDIWindows",
					MUIA_Application_Version,		"$VER: MUI_MDIWindows V0.1",
					MUIA_Application_Copyright,		"(C)2022 M.Volkel",
					MUIA_Application_Description,	"MUI-MDIWindows",
					MUIA_Application_Window,		win = NewObject(CL_mainW->mcc_Class, NULL, TAG_DONE),
				End;

				if (app)
				{
					set(win, MUIA_Window_Open, TRUE);

					while (running)
					{
						switch (DoMethod(app, MUIM_Application_NewInput, &signal))
						{
							// Window close
							case MUIV_Application_ReturnID_Quit:
								if ((MUI_RequestA(app, win, 0, "Quit?", "_Yes|_No", "\33cAre you sure?", 0)) == 1)
									running = FALSE;
							break;

							default:
								break;
						}

						if (running && signal)
							Wait(signal);
					}
					set(win, MUIA_Window_Open, FALSE);
					MUI_DisposeObject(app);
				}
				exitClasses();
			}
		}
		CloseDownScreen();
		exitLibs();
	}
	exit(0);
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

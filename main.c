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

/******************************************************************************
* Prototypes
*******************************************************************************/
void init(void);
void end(void);
struct ObjApp * CreateApp(void);
void DisposeApp(struct ObjApp * ObjectApp);

ULONG SetupScreen(void);
void CloseDownScreen(void);

/******************************************************************************
* Definitions
*******************************************************************************/
#define MAKE_ID(a, b, c, d) ((ULONG)(a) << 24 | (ULONG)(b) << 16 | (ULONG)(c) << 8 | (ULONG)(d))

struct ObjApp
{
	APTR	App;
	APTR	WI_label_0;
	APTR	BT_label_0;
};

/******************************************************************************
* Global Variables
*******************************************************************************/
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;

char buffer[40];
struct ObjApp *App = NULL;

struct Screen *myScreen;

/******************************************************************************
* Main-Program
*******************************************************************************/

/*-----------------------------------------------------------------------------
- main()
------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	BOOL running = TRUE;
	ULONG signal;

	init();

	if (SetupScreen() != 0)
	{
		printf("Can't Open Screen\n");
		end();
	}

	if (!(App = CreateApp()))
	{
		printf("Can't Create App\n");
		end();
	}

	while (running)
	{
		switch (DoMethod(App->App, MUIM_Application_NewInput, &signal))
		{
			// Window close
			case MUIV_Application_ReturnID_Quit:
				if ((MUI_RequestA(App->App, App->WI_label_0, 0, "Quit?", "_Yes|_No", "\33cAre you sure?", 0)) == 1)
					running = FALSE;
			break;

			default:
				break;
		}

		if (running && signal)
			Wait(signal);
	}

	DisposeApp(App);
	CloseDownScreen();
	end();
}

/*-----------------------------------------------------------------------------
- init()
------------------------------------------------------------------------------*/
void init(void)
{
	if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37)))
	{
		printf("Can't Open Intuition Library\n");
		exit(20);
	}

	if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN)))
	{
		printf("Can't Open MUIMaster Library\n");
		CloseLibrary((struct Library *)IntuitionBase);
		exit(20);
	}
}

/*-----------------------------------------------------------------------------
- end()
------------------------------------------------------------------------------*/
void end(void)
{
	CloseLibrary((struct Library *)MUIMasterBase);
	CloseLibrary((struct Library *)IntuitionBase);
	exit(0);
}

/*-----------------------------------------------------------------------------
- CreateApp()
------------------------------------------------------------------------------*/
struct ObjApp * CreateApp(void)
{
	struct ObjApp * ObjectApp;

	APTR	GROUP_ROOT_0;

	if (!(ObjectApp = AllocVec(sizeof(struct ObjApp), MEMF_CLEAR)))
		return(NULL);

	ObjectApp->BT_label_0 = SimpleButton("Exit");

	GROUP_ROOT_0 = GroupObject,
		MUIA_Group_Columns,		1,
		MUIA_Group_SameSize,	TRUE,
		Child,					ObjectApp->BT_label_0,
	End;

	ObjectApp->WI_label_0 = WindowObject,
		MUIA_Window_Title,			"MUI_MDI",
		MUIA_Window_ID,				MAKE_ID('0', 'W', 'I', 'N'),
		WindowContents,				GROUP_ROOT_0,
		MUIA_Window_LeftEdge,		100,
		MUIA_Window_TopEdge,		100,
		MUIA_Window_Width,			100,
		MUIA_Window_Height,			100,
		MUIA_Window_Screen,			myScreen,
	End;

	ObjectApp->App = ApplicationObject,
		MUIA_Application_Author,		"M.Volkel",
		MUIA_Application_Base,			"MUIMDI",
		MUIA_Application_Title,			"MUI_MDIWindows",
		MUIA_Application_Version,		"$VER: MUI_MDIWindows V0.1",
		MUIA_Application_Copyright,		"(C)2022 M.Volkel",
		MUIA_Application_Description,	"MUI-MDIWindows",
		SubWindow,						ObjectApp->WI_label_0,
	End;


	if (!ObjectApp->App)
	{
		FreeVec(ObjectApp);
		return(NULL);
	}

	// Window-Close-Method
	DoMethod(ObjectApp->WI_label_0, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, ObjectApp->App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	// Exit-Button
	DoMethod(ObjectApp->BT_label_0, MUIM_Notify, MUIA_Pressed, MUIV_EveryTime, ObjectApp->App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	// Open Window
	set(ObjectApp->WI_label_0, MUIA_Window_Open, TRUE);

	return(ObjectApp);
}

/*-----------------------------------------------------------------------------
- DisposeApp()
------------------------------------------------------------------------------*/
void DisposeApp(struct ObjApp * ObjectApp)
{
	MUI_DisposeObject(ObjectApp->App);
	FreeVec(ObjectApp);
}

/*-----------------------------------------------------------------------------
- DisposeApp()
------------------------------------------------------------------------------*/
ULONG SetupScreen(void)
{
	if (!(myScreen = OpenScreenTags(NULL, 
		SA_LikeWorkbench, TRUE,
		SA_Type, PUBLICSCREEN,
		SA_PubName, "MDISCREEN",
		SA_Title, "MUI_MDIWindows V0.1, (C)2022 M.Volkel",
		SA_ShowTitle, TRUE,
		TAG_DONE)))
	{
		return 1;
	}

	while (PubScreenStatus(myScreen, NULL) == 0);

	LockPubScreen("MDISCREEN");
	return 0;
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

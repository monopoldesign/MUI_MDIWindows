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

int SetupScreen(void);
ULONG CloneScreen(void);
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

struct Screen *myScreen = NULL;
struct TextFont *openedFont;
UBYTE *fontName;
STRPTR myPubScreen = "MUI_MDI";

APTR VisualInfo = NULL;
struct TextAttr topaz8 = {(STRPTR)"topaz.font", 8, 0x00, 0x01 };

struct ColorSpec ScreenColors[] = {
	 0, 0x09, 0x09, 0x09,
	 1, 0x00, 0x00, 0x00,
	 2, 0x0F, 0x0F, 0x0F,
	 3, 0x05, 0x07, 0x0A,
	 4, 0x08, 0x08, 0x08,
	 5, 0x0A, 0x0A, 0x0A,
	 6, 0x0A, 0x09, 0x08,
	 7, 0x0E, 0x0A, 0x09,
	~0, 0x00, 0x00, 0x00};

UWORD DriPens[] = {~0};

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

	if (CloneScreen() != 0)
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
				if ((MUI_RequestA(App->App, 0, 0, "Quit?", "_Yes|_No", "\33cAre you sure?", 0)) == 1)
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
		//MUIA_Window_Screen,			&myScreen,
		MUIA_Window_ScreenTitle,	myPubScreen,
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
ULONG CloneScreen(void)
{
	STRPTR pubScreenName = "Workbench";

	struct Screen *pubScreen = NULL;
	struct DrawInfo *screenDrawInfo = NULL;
	struct TextAttr pubScreenFont;

	ULONG screenModeID;
	UBYTE *pubScreenFontName;
	ULONG fontNameSize;

	pubScreen = LockPubScreen(pubScreenName);
	if (pubScreen != NULL)
	{
		screenDrawInfo = GetScreenDrawInfo(pubScreen);
		if (screenDrawInfo != NULL)
		{
			screenModeID = GetVPModeID(&(pubScreen->ViewPort));
			if (screenModeID != INVALID_ID)
			{
				pubScreenFontName = screenDrawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
				fontNameSize = 1 + strlen(pubScreenFontName);
				fontName = AllocVec(fontNameSize, MEMF_CLEAR);

				if (fontName != NULL)
				{
					strcpy(fontName, pubScreenFontName);
					pubScreenFont.ta_Name = fontName;
					pubScreenFont.ta_YSize = screenDrawInfo->dri_Font->tf_YSize;
					pubScreenFont.ta_Style = screenDrawInfo->dri_Font->tf_Style;
					pubScreenFont.ta_Flags = screenDrawInfo->dri_Font->tf_Flags;

					openedFont = OpenFont(&pubScreenFont);

					if (openedFont != NULL)
					{
						if (!(myScreen = OpenScreenTags(NULL,
										SA_Left,		0,
										SA_Top,			0,
										SA_Width,		pubScreen->Width,
										SA_Height,		pubScreen->Height,
										SA_Depth,		screenDrawInfo->dri_Depth,
										SA_Overscan,	OSCAN_TEXT,
										SA_Pens,		screenDrawInfo->dri_Pens,
										SA_Font,		&pubScreenFont,
										SA_DisplayID,	screenModeID,
										SA_Title,		"MUI_MDIWindows V0.1, (C)2022 M.Volkel",
										SA_PubName,		myPubScreen,
										SA_Type,		PUBLICSCREEN,
										//SA_Colors,	&ScreenColors[0],
										//SA_Type,		CUSTOMSCREEN,
										TAG_DONE)))
						{
							return 1;
						}

						FreeScreenDrawInfo(pubScreen, screenDrawInfo);
						UnlockPubScreen(pubScreenName, pubScreen);
						pubScreen = NULL;
						return 0;
					}
				}
				FreeVec(fontName);
			}
		}
	}
	return 2;
}

/*-----------------------------------------------------------------------------
- DisposeApp()
------------------------------------------------------------------------------*/
void CloseDownScreen(void)
{
	if (myScreen)
	{
		CloseScreen(myScreen);
		myScreen = NULL;

		if (openedFont)
			CloseFont(openedFont);

		if (fontName)
			FreeVec(fontName);
	}
}

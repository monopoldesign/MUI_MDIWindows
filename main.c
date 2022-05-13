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
#include "help.h"

/******************************************************************************
* Global Variables
*******************************************************************************/
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;

char buffer[40];

struct Screen *myScreen;
UBYTE *screenName = "MDISCREEN";

struct MUI_CustomClass *CL_mainW;
Object *win2 = NULL;

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

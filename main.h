#ifndef _MAIN_H
#define _MAIN_H

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

/******************************************************************************
* Definitions
*******************************************************************************/
#define MAKE_ID(a, b, c, d) ((ULONG)(a) << 24 | (ULONG)(b) << 16 | (ULONG)(c) << 8 | (ULONG)(d))

/******************************************************************************
* Global Variables
*******************************************************************************/
extern struct IntuitionBase *IntuitionBase;
extern struct Library *MUIMasterBase;

extern struct Screen *myScreen;
extern UBYTE *screenName;

extern struct MUI_CustomClass *CL_mainW;
extern Object *win2;

#endif

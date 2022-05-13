#ifndef _MAINW_H
#define _MAINW_H

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

/******************************************************************************
* Header-Files
*******************************************************************************/
#include "main.h"

/******************************************************************************
* Prototypes
*******************************************************************************/
ULONG mainW_New(struct IClass *cl, Object *obj, struct opSet *msg);
ULONG mainW_Finish(struct IClass *cl, Object *obj, struct MUIP_mainW_Finish *msg);
ULONG mainW_newWindow(struct IClass *cl, Object *obj, Msg msg);
DISPATCHER(mainW_Dispatcher);

/******************************************************************************
* Definitions
*******************************************************************************/
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

#endif

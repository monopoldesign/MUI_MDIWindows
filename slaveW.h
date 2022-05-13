#ifndef _SLAVEW_H
#define _SLAVEW_H

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
ULONG slaveW_New(struct IClass *cl, Object *obj, struct opSet *msg);
ULONG slaveW_Finish(struct IClass *cl, Object *obj, struct MUIP_slaveW_Finish *msg);
DISPATCHER(slaveW_Dispatcher);

/******************************************************************************
* Definitions
*******************************************************************************/
#define TAGBASE_CLASS (TAG_USER | 0x80420000)
#define MUIM_slaveW_Finish		TAGBASE_CLASS + 1
#define MUIM_slaveW_newWindow	TAGBASE_CLASS + 2

struct slaveW_Data
{
	Object *BT_Close;
};

struct MUIP_slaveW_Finish
{
	ULONG MethodID;
	LONG level;
};

#endif
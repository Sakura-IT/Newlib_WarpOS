/*
 * $Id$
 *
 * Remake attempt of the startup file for WOS as the source
 * of the original was lost. By Dennis van der Boon in 2020.
 */


//****************************************************************************

#pragma pack(push,2)
#include <exec/types.h>
#include <powerpc/powerpc.h>
#include <proto/exec.h>
#include <powerpc/tasksPPC.h>
#include <exec/tasks.h>

//****************************************************************************

ULONG __SaveLR, __SaveSP, _WBenchMsg, __commandlen, __commandline; 
LONG __argc, __Saveerror;
char **__argv;

struct DosLibrary* DOSBase;
struct ExecBase* SysBase;
struct Library* PowerPCBase;

//****************************************************************************

struct CallFunction {
APTR cf_Function;
LONG cf_Priority;
};

struct TaskObject {
  struct Node n;                /* contains task name and priority */
  UBYTE flags;
  UBYTE id;                     /* used to check a TaskObject pointer */
  ULONG reserved1[6];
  struct Task *m68kParent;      /* parent 68k process, for sync. tasks */
  APTR start_addr;              /* ptr to the task's first instruction */
  ULONG stacksize;
  ULONG args[8];                /* r3 - r10 */
  struct Library *PPCBase;      /* our library base */
  BPTR inputhandle;             /* the standard I/O handles */
  BPTR outputhandle;
  BPTR errorhandle;
  BPTR progdirLock;             /* PROGDIR: lock from parent */
  BPTR curdirLock;              /* current directory lock from parent */
  ULONG reserved2;
  void *msg_data;               /* only relevant, if we really have a */
  ULONG msg_length;             /*  startup msg (PPCTASKTAG_STARTUP_MSG) */
  ULONG msg_id;
  char reserved3[32];
/* PPC-only data below */
  struct TaskPPC *this;
  APTR initialstack;            /* ptr to task's initial V.4 stack frame */
  APTR exit_addr;               /* function to call on PPCFinishTask() */
  ULONG reserved4;
  ULONG Ext_UserData;
  char reserved5[32];
};

#define TskObj_idMagic 0xa5

#pragma pack(pop)

//****************************************************************************

void  _exit(LONG rval);
void __start (char* commandline, struct PPCBase* PowerPCBase, ULONG wbenchmsg, ULONG commandlen, struct TaskObject* taskobj);
void callfuncs(APTR list, ULONG flag);

extern APTR __initlist, __exitlist, __ctrslist, __dtrslist;
extern LONG main(LONG, char**);

//****************************************************************************

	__asm(".section .init\n"
	".globl __initlist\n"
	".type __initlist,@function\n"
	"__initlist:\n"
	".section .fini\n"
	".globl __exitlist\n"
	".type __exitlist,@function\n"
	"__exitlist:\n"
	".section .ctors\n"
	".globl __ctrslist\n"
	"__ctrslist:\n"
	".section .dtors\n"
	".globl __dtrslist\n"
	"__dtrslist:\n"
	".globl __start\n"
	".globl _exit\n"
	".globl __nocommandline\n"
	".section .text\n"
	);

//****************************************************************************

	__asm("_start:\tnop\n\t"
	"lis 11,__SaveSP@ha\n\t"
	"stw 1,__SaveSP@l(11)\n\t"
	"lis 11,__SaveLR@ha\n\t"
	"mflr 0\n\t"
	"stw 0,__SaveLR@l(11)\n\t"
	"mr 7,2\n\t"
	"b __start\n\t"
	".string \"  Compiled using MOS2WOS - Startup version 1.0  \""
	);

//****************************************************************************

void _exit(LONG rval)
{
	__Saveerror = rval;
	
	ULONG* funcpointer = (ULONG*)&__dtrslist;	
	while (funcpointer[0])
	{
		void (*dtorfunc)(void) = (APTR)funcpointer[0];
		dtorfunc();
		funcpointer++;
	}

	callfuncs((APTR)&__exitlist, 0);
	
	__asm("\tlis 11,__Saveerror@ha\n\t"
	"lwz 3,__Saveerror@l(11)\n\t"
	"lis 11,__SaveSP@ha\n\t"	
	"lwz 1,__SaveSP@l(11)\n\t"
	"lis 11,__SaveLR@ha\n\t"
	"lwz 0,__SaveLR@l(11)\n\t"
	"mtlr 0\n\t"
	"blr\n\t"
	);
}

//****************************************************************************

void callfuncs(APTR list, ULONG flag)
{
	struct CallFunction* cf;
	ULONG nextHigh, pri;
		
	ULONG highPri = -1;
	
	while(1)
	{
		cf = (struct CallFunction*)list;
		nextHigh = 0;
		while (cf->cf_Function)
		{
			pri = (cf->cf_Priority + 1) ^ flag;
		
			if (highPri == pri)
			{
				void (*func)(void) = (APTR)cf->cf_Function;
				func();
			}
			else if ((pri < highPri) && (pri > nextHigh))
			{
				nextHigh = pri;
			}
			cf++;
		}
		if (!(nextHigh))
		{
			break;
		}
		highPri = nextHigh;
	}				
	return;
}

//**************************************************************************** 

void  __start (char* commandline, struct PPCBase* myPowerPCBase, ULONG wbenchmsg, ULONG commandlen, struct TaskObject* taskobj)
{
	if ((taskobj) && (taskobj->id == TskObj_idMagic))
	{
		if (!(taskobj->args[0]))
		{
			wbenchmsg = taskobj->args[1];
		}
		else
		{
			wbenchmsg = 0;
			while (commandline[0])
			{
				if ((commandline[0] == '"') && (commandline[1] == ' '))
				{
					commandline += 2;
					break;
				}
				commandline++;
			}

			commandlen = 1;
			
			char* endline = commandline;
			
			while (endline[0])
			{
				commandlen++;
				endline++;
			}
			endline[0] = '\n';
			endline[1] = 0;  //buffer overflow?			
		}
		myPowerPCBase = (struct PPCBase*)taskobj->this->tp_PowerPCBase;
	}

	PowerPCBase = (struct Library*)myPowerPCBase;			
	SysBase = (struct ExecBase*)myPowerPCBase->PPC_SysLib;
	DOSBase = (struct DosLibrary*)myPowerPCBase->PPC_DosLib;

	__commandlen = commandlen;
	__commandline = (ULONG)commandline;
	_WBenchMsg = wbenchmsg;
 
	callfuncs((APTR)&__initlist, -1);
	
	ULONG* funcpointer = (ULONG*)&__ctrslist;	
	while (funcpointer[0])
	{
		void (*ctorfunc)(void) = (APTR)funcpointer[0];
		ctorfunc();
		funcpointer++;
	}
			
	_exit(main(__argc, __argv));
}

//****************************************************************************

/*****************************************************************************

This code serves as a basis for writing a library-based MUI custom class
(xyz.mcc) and its preferences editor (xyz.mcp).

You need to define a few things in your main source file, then include
this file and then continue with your classes methods. The order is
important, mccheader.c must be the first code-generating module.

Things to be defined before mccheader.c is included:

(1)  UserLibID     - version string for your class. must follow normal $VER: string conventions.
(2)  VERSION       - version number of the class. must match the one given in the $VER: string.
(3)  REVISION      - revision number of the class. must match the one given in the $VER: string.
(4)  CLASS         - Name of your class, ".mcc" or ".mcp" must always be appended.

(5)  SUPERCLASS    - superclass of your class.
(6)  struct Data   - instance data structure.
(7)  _Dispatcher   - your dispatcher function.

(8)  SUPERCLASSP   - Superclass of the preferences class, must be MUIC_Mccprefs or a subclass.
(9)  struct DataP  - instance data structure of preferences class.
(10) _DispatcherP  - dispatcher for the preferences class.

(11) USEDCLASSES   - Other custom classes required by this class (NULL terminated string array)
(12) USEDCLASSESP  - Preferences classes (.mcp) required by this class (NULL terminated string array)
(13) SHORTHELP     - .mcp help text for prefs program's listview.

Items (1) to (4) must always be defined. If you create a stand-alone
custom class (*.mcc) without preferences editor, also define (5), (6)
and (7). Name your class and the resulting ouptut file "Myclass.mcc".

If you create a preferences class (*.mcp) for a custom class, define
(8), (9) and (10) instead of (5), (6) and (7). Name your class and the
resulting output file "Myclass.mcp".

If you create a custom class with included preferences editor, define
all the above. Note that in this case, the name of your class and the
resulting output file is always "Myclass.mcc". MUI will automatically
recognize that there's also a "Myclass.mcp" included. Having a builtin
preferences class reduces the need for a second file but increases the
size and memory consuption of the class.

(11) If your class requires other mcc custom classes, list them in the
static array USEDCLASSES like this:
#define USEDCLASSES used_classes
const STRPTR used_classes[] = { "Busy.mcc", "Listtree.mcc", NULL };

(12) If your class has one (or more) preferences classes, list them in
the array USEDCLASSESP like this:
#define USEDCLASSESP used_classesP
const STRPTR used_classesP[] = { "Myclass.mcp", "Popxxx.mcp", NULL };

(13) If you want MUI to display additional help text (besides name,
version and copyright) when the mouse pointer is over your mcp entry
in the prefs listview:
#define SHORTHELP "ANSI display for terminal programs."

If your class needs custom initialization (e.g. opening other
libraries), you can define
  ClassInit
  ClassExit
to point to custom functions. These functions need to have the prototypes
  BOOL ClassInitFunc(struct Library *base);
  VOID ClassExitFunc(struct Library *base);
and will be called right after the class has been created and right
before the class is being deleted. If your init func returns FALSE,
the custom class will be unloaded immediately.

Define the minimum version of muimaster.libray in MASTERVERSION. If you
don't define MASTERVERSION, it will default to MUIMASTER_VMIN from the
mui.h include file.

---
Items (1) to (4) must always be defined. If you create a stand-alone
custom class (*.mcc) without preferences editor, also define (5), (6)
and (7). Name your class and the resulting ouptut file "Myclass.mcc".

If you create a preferences class (*.mcp) for a custom class, define
(8), (9) and (10) instead of (5), (6) and (7). Name your class and the
resulting output file "Myclass.mcp".

If you create a custom class with included preferences editor, define
all the above. Note that in this case, the name of your class and the
resulting output file is always "Myclass.mcc". MUI will automatically
recognize that there's also a "Myclass.mcp" included. Having a builtin
preferences class reduces the need for a second file but increases the
size and memory consuption of the class.

If your class needs custom initialization (e.g. opening other
libraries), you can define
  PreClassInit
  PostClassExit
  ClassInit
  ClassExit
to point to custom functions. These functions need to have the prototypes
  BOOL ClassInitFunc(struct Library *base);
  VOID ClassExitFunc(struct Library *base);
and will be called right after the class has been created and right
before the class is being deleted. If your init func returns FALSE,
the custom class will be unloaded immediately.

  BOOL PreClassInitFunc(void);
  VOID PostClassExitFunc(void);

These functions will be called BEFORE the class is created and AFTER the
class is deleted, if something depends on it for example. MUIMasterBase
is open then.

Define the minimum version of muimaster.libray in MASTERVERSION. If you
don't define MASTERVERSION, it will default to MUIMASTER_VMIN from the
mui.h include file.

This code automatically defines and initializes the following variables:
  struct Library *MUIMasterBase;
  struct Library *SysBase;
  struct Library *UtilityBase;
  struct DosLibrary *DOSBase;
  struct GfxBase *GfxBase;
  struct IntuitionBase *IntuitionBase;
  struct Library *MUIClassBase;       // your classes library base
  struct MUI_CustomClass *ThisClass;  // your custom class
  struct MUI_CustomClass *ThisClassP; // your preferences class

Example: Myclass.c
  #define CLASS      MUIC_Myclass // name of class, e.g. "Myclass.mcc"
  #define SUPERCLASS MUIC_Area    // name of superclass
  struct Data
  {
    LONG          MyData;
    struct Foobar MyData2;
    // ...
  };
  #define UserLibID "$VER: Myclass.mcc 17.53 (11.11.96)"
  #define VERSION   17
  #define REVISION  53
  #include "mccheader.c"
  ULONG ASM SAVEDS _Dispatcher(REG(a0) struct IClass *cl GNUCREG(a0),
                               REG(a2) Object *obj GNUCREG(a2),
                               REG(a1) Msg msg GNUCREG(a1) )
  {
    // ...
  }

Compiling and linking with SAS-C can look like this:
  Myclass.mcc: Myclass.c
    sc $(CFLAGS) $*.c OBJNAME $*.o
    slink to $@ from $*.o lib $(LINKERLIBS) $(LINKERFLAGS)

Note well that we don't use SAS library creation feature here, it simply
sucks too much. It's not much more complicated to do the library
initialziation ourselves and we have better control over everything.

Make sure to read the whole source to get some interesting comments
and some understanding on how libraries are created!

*****************************************************************************/

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

/* MorphOS relevant includes... */
#ifdef __MORPHOS__
#include <emul/emulinterface.h>
#include <emul/emulregs.h>
#endif

/* a few other includes... */

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

/* The name of the class will also become the name of the library. */
/* We need a pointer to this string in our ROMTag (see below). */

static const char UserLibName[] = CLASS;

/* Here's our global data, described above. */

#if defined(__amigaos4__)
struct Library *MUIMasterBase = NULL;
struct Library *SysBase       = NULL;
struct Library *UtilityBase   = NULL;
struct Library *DOSBase       = NULL;
struct Library *GfxBase       = NULL;
struct Library *IntuitionBase = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct ExecIFace *IExec           = NULL;
struct UtilityIFace *IUtility     = NULL;
struct DOSIFace *IDOS             = NULL;
struct GraphicsIFace *IGraphics   = NULL;
struct IntuitionIFace *IIntuition = NULL;
#if defined(__NEWLIB__)
struct Library *NewlibBase = NULL;
struct NewlibIFace* INewlib = NULL;
#endif
#else
struct Library        *MUIMasterBase = NULL;
struct ExecBase       *SysBase       = NULL;
struct Library        *UtilityBase   = NULL;
struct DosLibrary     *DOSBase       = NULL;
struct GfxBase        *GfxBase       = NULL;
struct IntuitionBase  *IntuitionBase = NULL;
#endif

#ifdef SUPERCLASS
static struct MUI_CustomClass *ThisClass = NULL;
#endif

#ifdef SUPERCLASSP
static struct MUI_CustomClass *ThisClassP = NULL;
#endif

#ifdef __GNUC__
  #if defined(USE_UTILITYBASE) && !defined(__NEWLIB__)
  struct Library *__UtilityBase = NULL; // required by libnix & clib2
  #endif
  #ifdef __libnix__
    /* these one are needed copies for libnix.a */
    #ifdef USE_MATHIEEEDOUBBASBASE
    struct Library *__MathIeeeDoubBasBase = NULL;
    #endif
    #ifdef USE_MATHIEEEDOUBTRANSBASE
    struct Library *__MathIeeeDoubTransBase = NULL;
    #endif
  #endif
#endif /* __GNUC__ */

/* Our library structure, consisting of a struct Library, a segment pointer */
/* and a semaphore. We need the semaphore to protect init/exit stuff in our */
/* open/close functions */

struct LibraryHeader
{
  struct Library         lh_Library;
  UWORD                  lh_Pad1;
  BPTR                   lh_Segment;
  struct SignalSemaphore lh_Semaphore;
  UWORD                  lh_Pad2;
  #if defined(CLASS_STACKSWAP)
  struct StackSwapStruct *lh_Stack;
  #endif
};

/******************************************************************************/
/* External references                                                        */
/******************************************************************************/

//static BOOL LIBFUNC UserLibInit   (struct Library *base);
//static BOOL LIBFUNC UserLibExpunge(struct Library *base);
static BOOL LIBFUNC UserLibOpen   (struct Library *base);
static BOOL LIBFUNC UserLibClose  (struct Library *base);

/******************************************************************************/
/* Local Structures & Prototypes                                              */
/******************************************************************************/

#if defined(__amigaos4__)

static struct LibraryHeader * LIBFUNC LibInit    (struct LibraryHeader *base, BPTR librarySegment, struct ExecIFace *pIExec);
static BPTR                   LIBFUNC LibExpunge (struct LibraryManagerInterface *Self);
static struct LibraryHeader * LIBFUNC LibOpen    (struct LibraryManagerInterface *Self, ULONG version);
static BPTR                   LIBFUNC LibClose   (struct LibraryManagerInterface *Self);
static ULONG                  LIBFUNC MCC_Query  (UNUSED struct Interface *self, REG(d0, LONG which));

#elif defined(__MORPHOS__)

static struct LibraryHeader * LIBFUNC LibInit    (struct LibraryHeader *base, BPTR librarySegment, struct ExecBase *sb);
static BPTR                   LIBFUNC LibExpunge (void);
static struct LibraryHeader * LIBFUNC LibOpen    (void);
static BPTR                   LIBFUNC LibClose   (void);
static LONG                   LIBFUNC LibNull    (void);
static ULONG                  LIBFUNC MCC_Query  (void);

#else

static struct LibraryHeader * LIBFUNC LibInit    (REG(d0, struct LibraryHeader *base), REG(a0, BPTR librarySegment), REG(a6, struct ExecBase *sb));
static BPTR                   LIBFUNC LibExpunge (REG(a6, struct LibraryHeader *base));
static struct LibraryHeader * LIBFUNC LibOpen    (REG(a6, struct LibraryHeader *base));
static BPTR                   LIBFUNC LibClose   (REG(a6, struct LibraryHeader *base));
static LONG                   LIBFUNC LibNull    (void);
static ULONG                  LIBFUNC MCC_Query  (REG(d0, LONG which));

#endif

/******************************************************************************/
/* Dummy entry point and LibNull() function all in one                        */
/******************************************************************************/

#if defined(__amigaos4__)
int _start(void)
#else
int Main(void)
#endif
{
  return RETURN_FAIL;
}

#if !defined(__amigaos4__)
static LONG LIBFUNC LibNull(VOID)
{
  return(0);
}
#endif

/******************************************************************************/
/* Local data structures                                                      */
/******************************************************************************/

#if defined(__amigaos4__)
/* ------------------- OS4 Manager Interface ------------------------ */
STATIC ULONG LibObtain(struct LibraryManagerInterface *Self)
{
   return(Self->Data.RefCount++);
}

STATIC ULONG LibRelease(struct LibraryManagerInterface *Self)
{
   return(Self->Data.RefCount--);
}

STATIC CONST APTR LibManagerVectors[] =
{
   (APTR)LibObtain,
   (APTR)LibRelease,
   (APTR)NULL,
   (APTR)NULL,
   (APTR)LibOpen,
   (APTR)LibClose,
   (APTR)LibExpunge,
   (APTR)NULL,
   (APTR)-1
};

STATIC CONST struct TagItem LibManagerTags[] =
{
   {MIT_Name,             (ULONG)"__library"},
   {MIT_VectorTable,      (ULONG)LibManagerVectors},
   {MIT_Version,          1},
   {TAG_DONE,             0}
};

/* ------------------- Library Interface(s) ------------------------ */

STATIC CONST APTR LibVectors[] =
{
   (APTR)LibObtain,
   (APTR)LibRelease,
   (APTR)NULL,
   (APTR)NULL,
   (APTR)MCC_Query,
   (APTR)-1
};

STATIC CONST struct TagItem MainTags[] =
{
   {MIT_Name,        (ULONG)"main"},
   {MIT_VectorTable, (ULONG)LibVectors},
   {MIT_Version,     1},
   {TAG_DONE,        0}
};

STATIC CONST ULONG LibInterfaces[] =
{
   (ULONG)LibManagerTags,
   (ULONG)MainTags,
   (ULONG)0
};

// Out libraries always have to carry a 68k jump table with it, so
// lets define it here as extern, as we are going to link it to
// our binary here.
#ifndef NO_VECTABLE68K
extern const APTR VecTable68K[];
#endif

STATIC CONST struct TagItem LibCreateTags[] =
{
   {CLT_DataSize,   (ULONG)(sizeof(struct LibraryHeader))},
   {CLT_InitFunc,   (ULONG)LibInit},
   {CLT_Interfaces, (ULONG)LibInterfaces},
   #ifndef NO_VECTABLE68K
   {CLT_Vector68K,  (ULONG)VecTable68K},
   #endif
   {TAG_DONE,       0}
};

#else

static const APTR LibVectors[] =
{
  #ifdef __MORPHOS__
  (APTR)FUNCARRAY_32BIT_NATIVE,
  #endif
  (APTR)LibOpen,
  (APTR)LibClose,
  (APTR)LibExpunge,
  (APTR)LibNull,
  (APTR)MCC_Query,
  (APTR)-1
};

static const ULONG LibInitTab[] =
{
  sizeof(struct LibraryHeader),
  (ULONG)LibVectors,
  (ULONG)NULL,
  (ULONG)LibInit
};

#endif

/* ------------------- ROM Tag ------------------------ */
static const USED_VAR struct Resident ROMTag =
{
  RTC_MATCHWORD,
  (struct Resident *)&ROMTag,
  (struct Resident *)&ROMTag + 1,
  #if defined(__amigaos4__)
  RTF_AUTOINIT|RTF_NATIVE,      // The Library should be set up according to the given table.
  #elif defined(__MORPHOS__)
  RTF_AUTOINIT|RTF_PPC,
  #else
  RTF_AUTOINIT,
  #endif
  VERSION,
  NT_LIBRARY,
  0,
  (char *)UserLibName,
  (char *)UserLibID+6,
  #if defined(__amigaos4__)
  (APTR)LibCreateTags,           // This table is for initializing the Library.
  #else
  (APTR)LibInitTab,
  #endif
  #if defined(__MORPHOS__)
  REVISION,
  0
  #endif
};

#if defined(__MORPHOS__)
/*
 * To tell the loader that this is a new emulppc elf and not
 * one for the ppc.library.
 * ** IMPORTANT **
 */
const USED_VAR ULONG __abox__ = 1;

#endif /* __MORPHOS__ */

/******************************************************************************/
/* Standard Library Functions, all of them are called in Forbid() state.      */
/******************************************************************************/

#if defined(__amigaos4__)
static struct LibraryHeader * LibInit(struct LibraryHeader *base, BPTR librarySegment, struct ExecIFace *pIExec)
{
  struct ExecBase *sb = (struct ExecBase *)pIExec->Data.LibBase;
  IExec = pIExec;
#elif defined(__MORPHOS__)
static struct LibraryHeader * LibInit(struct LibraryHeader *base, BPTR librarySegment, struct ExecBase *sb)
{
#else
static struct LibraryHeader * LIBFUNC LibInit(REG(d0, struct LibraryHeader *base), REG(a0, BPTR librarySegment), REG(a6, struct ExecBase *sb))
{
#endif
  struct LibraryHeader *ret_base = NULL;
  #if defined(CLASS_STACKSWAP)
  static struct StackSwapStruct *stack;
  #endif

  SysBase = (APTR)sb;

  #if defined(__amigaos4__) && defined(__NEWLIB__)
  if((NewlibBase = OpenLibrary("newlib.library", 3)) &&
     GETINTERFACE(INewlib, NewlibBase))
  #endif
  {
    D(DBF_STARTUP, "LibInit()");

    // make sure that this is really a 68020+ machine if optimized for 020+
    #if _M68060 || _M68040 || _M68030 || _M68020 || __mc68020 || __mc68030 || __mc68040 || __mc68060
    if(!(SysBase->AttnFlags & AFF_68020))
      return(NULL);
    #endif

    #if defined(CLASS_STACKSWAP)
    if(!(stack = AllocMem(sizeof(struct StackSwapStruct)+8192, MEMF_PUBLIC|MEMF_CLEAR)))
      return( NULL );

    stack->stk_Lower   = (APTR)((ULONG)stack + sizeof(struct StackSwapStruct));
    stack->stk_Upper   = (ULONG)((ULONG)stack->stk_Lower+8192);
    stack->stk_Pointer = (APTR)stack->stk_Upper;

    D(DBF_STARTUP, "Before StackSwap()");
    StackSwap( stack );
    #endif

    // cleanup the library header structure beginning with the
    // library base, even if that is done automcatically, we explicitly
    // do it here for consistency reasons.
    base->lh_Library.lib_Node.ln_Type = NT_LIBRARY;
    base->lh_Library.lib_Node.ln_Pri  = 0;
    base->lh_Library.lib_Node.ln_Name = (char *)UserLibName;
    base->lh_Library.lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
    base->lh_Library.lib_Version      = VERSION;
    base->lh_Library.lib_Revision     = REVISION;
    base->lh_Library.lib_IdString     = (char *)UserLibID; // here without +6 or otherwise MUI didn't identify it.

    base->lh_Segment = librarySegment;

    InitSemaphore(&base->lh_Semaphore);

    #if defined(CLASS_STACKSWAP)
    base->lh_Stack = stack;
    StackSwap(base->lh_Stack);
    FreeMem(base->lh_Stack, sizeof(struct StackSwapStruct) + 8192);
    D(DBF_STARTUP, "After second StackSwap()");
    #endif

    ret_base = base;
  }

  return(ret_base);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#ifndef __amigaos4__
#define DeleteLibrary(LIB) \
  FreeMem((STRPTR)(LIB)-(LIB)->lib_NegSize, (ULONG)((LIB)->lib_NegSize+(LIB)->lib_PosSize))
#endif

#if defined(__amigaos4__)
static BPTR LibExpunge(struct LibraryManagerInterface *Self)
{
  struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif defined(__MORPHOS__)
static BPTR LibExpunge(void)
{
  struct LibraryHeader *base = (struct LibraryHeader *)REG_A6;
#else
static BPTR LIBFUNC LibExpunge(REG(a6, struct LibraryHeader *base))
{
#endif
  BPTR rc;

  D(DBF_STARTUP, "LibExpunge()");

  // in case our open counter is still > 0, we have
  // to set the late expunge flag and return immediately
  if(base->lh_Library.lib_OpenCnt > 0)
  {
    base->lh_Library.lib_Flags |= LIBF_DELEXP;
    rc = 0;
  }
  else
  {
    #if defined(__amigaos4__) && defined(__NEWLIB__)
    if(NewlibBase)
    {
      DROPINTERFACE(INewlib);
      CloseLibrary(NewlibBase);
      NewlibBase = NULL;
    }
    #endif

    Remove((struct Node *)base);
    rc = base->lh_Segment;
    DeleteLibrary(&base->lh_Library);
  }

  return(rc);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#if defined(__amigaos4__)
static struct LibraryHeader *LibOpen(struct LibraryManagerInterface *Self, ULONG version UNUSED)
{
  struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif defined(__MORPHOS__)
static struct LibraryHeader *LibOpen(void)
{
  struct LibraryHeader *base = (struct LibraryHeader *)REG_A6;
#else
static struct LibraryHeader * LIBFUNC LibOpen(REG(a6, struct LibraryHeader *base))
{
#endif
  struct LibraryHeader *res;

  D(DBF_STARTUP, "LibOpen()");

  // !!! WARNING !!!
  // LibOpen(), LibClose() and LibExpunge() are called while the system is in
  // Forbid() state. That means that these functions should be quick and should
  // not break this Forbid()!! Therefore the open counter should be increased
  // as the very first instruction during LibOpen(), because a UserLibOpen()
  // which breaks a Forbid() and another task calling LibExpunge() will cause
  // to expunge this library while it is not yet fully initialized. A crash
  // is unavoidable then. Even the semaphore does not guarantee 100% protection
  // against such a race condition, because waiting for the semaphore to be
  // obtained will effectively break the Forbid()!

  // increase the open counter ahead of anything else
  base->lh_Library.lib_OpenCnt++;
  // delete the late expung flag
  base->lh_Library.lib_Flags &= ~LIBF_DELEXP;

  ObtainSemaphore(&base->lh_Semaphore);

  if(UserLibOpen(&base->lh_Library))
  {
    #ifdef CLASS_VERSIONFAKE
    base->lh_Library.lib_Version  = MUIMasterBase->lib_Version;
    base->lh_Library.lib_Revision = MUIMasterBase->lib_Revision;
    #endif

    res = base;
  }
  else
  {
    D(DBF_STARTUP, "UserLibOpen() failed");
    // decrease the open counter again
    base->lh_Library.lib_OpenCnt--;
    res = NULL;
  }

  ReleaseSemaphore(&base->lh_Semaphore);

  return res;
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#if defined(__amigaos4__)
static BPTR LibClose(struct LibraryManagerInterface *Self)
{
  struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif defined(__MORPHOS__)
static BPTR LibClose(void)
{
  struct LibraryHeader *base = (struct LibraryHeader *)REG_A6;
#else
static BPTR LIBFUNC LibClose(REG(a6, struct LibraryHeader *base))
{
#endif
  BPTR rc = 0;

  D(DBF_STARTUP, "LibClose()");

  ObtainSemaphore(&base->lh_Semaphore);

  UserLibClose(&base->lh_Library);

  ReleaseSemaphore(&base->lh_Semaphore);

  // decrease the open counter
  base->lh_Library.lib_OpenCnt--;

  // in case the opern counter is <= 0 we can
  // make sure that we free everything
  if(base->lh_Library.lib_OpenCnt <= 0)
  {
    // in case the late expunge flag is set we go and
    // expunge the library base right now
    if(base->lh_Library.lib_Flags & LIBF_DELEXP)
    {
      #if defined(__amigaos4__)
      rc = LibExpunge(Self);
      #elif defined(__MORPHOS__)
      rc = LibExpunge();
      #else
      rc = LibExpunge(base);
      #endif
    }
  }

  return rc;
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#ifdef SUPERCLASS
  DISPATCHERPROTO(_Dispatcher);
#endif

#ifdef SUPERCLASSP
  DISPATCHERPROTO(_DispatcherP);
#endif

static BOOL UserLibOpen(struct Library *base)
{
  BOOL PreClassInitFunc(void);
  BOOL ClassInitFunc(struct Library *base);

  D(DBF_STARTUP, "OpenCount = %ld", base->lib_OpenCnt);

  if(base->lib_OpenCnt != 0)
    return(TRUE);

  #ifndef MASTERVERSION
  #define MASTERVERSION MUIMASTER_VMIN
  #endif

  if((MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MASTERVERSION)) &&
     GETINTERFACE(IMUIMaster, MUIMasterBase))
  {
    #ifdef PreClassInit
    if (!PreClassInitFunc())
    {
      return FALSE;
    }
    #endif


    #ifdef SUPERCLASS
    ThisClass = MUI_CreateCustomClass(base, SUPERCLASS, NULL, sizeof(struct INSTDATA), ENTRY(_Dispatcher));
    if(ThisClass)
    #endif
    {
      #ifdef SUPERCLASSP
      if((ThisClassP = MUI_CreateCustomClass(base, SUPERCLASSP, NULL, sizeof(struct INSTDATAP), ENTRY(_DispatcherP))))
      #endif
      {
        #ifdef SUPERCLASS
        #define THISCLASS ThisClass
        #else
        #define THISCLASS ThisClassP
        #endif

        UtilityBase   = (APTR)THISCLASS->mcc_UtilityBase;
        DOSBase       = (APTR)THISCLASS->mcc_DOSBase;
        GfxBase       = (APTR)THISCLASS->mcc_GfxBase;
        IntuitionBase = (APTR)THISCLASS->mcc_IntuitionBase;

        #if defined(USE_UTILITYBASE) && !defined(__NEWLIB__)
        __UtilityBase = (APTR)UtilityBase;
        #endif

        if(UtilityBase && DOSBase && GfxBase && IntuitionBase &&
           GETINTERFACE(IUtility, UtilityBase) &&
           GETINTERFACE(IDOS, DOSBase) &&
           GETINTERFACE(IGraphics, GfxBase) &&
           GETINTERFACE(IIntuition, IntuitionBase))
        {
          #if defined(DEBUG)
          SetupDebug();
          #endif

          #ifndef ClassInit
          return(TRUE);
          #else
          if(ClassInitFunc(base))
          {
            return(TRUE);
          }

          #ifdef SUPERCLASSP
          MUI_DeleteCustomClass(ThisClassP);
          ThisClassP = NULL;
          #endif
          #endif
        }

        DROPINTERFACE(IIntuition);
        DROPINTERFACE(IGraphics);
        DROPINTERFACE(IDOS);
        DROPINTERFACE(IUtility);
      }

      #if defined(SUPERCLASSP) && defined(SUPERCLASS)
      MUI_DeleteCustomClass(ThisClass);
      ThisClass = NULL;
      #endif
    }

    DROPINTERFACE(IMUIMaster);
    CloseLibrary(MUIMasterBase);
    MUIMasterBase = NULL;
  }

  D(DBF_STARTUP, "fail.: %08lx %s",base,base->lib_Node.ln_Name);

  return(FALSE);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

static BOOL UserLibClose(struct Library *base)
{
  VOID PostClassExitFunc(void);
  VOID ClassExitFunc(struct Library *base);

  D(DBF_STARTUP, "OpenCount = %ld", base->lib_OpenCnt);

  if(base->lib_OpenCnt == 1)
  {
    #ifdef ClassExit
    ClassExitFunc(base);
    #endif

    #ifdef SUPERCLASSP
    if (ThisClassP)
    {
      MUI_DeleteCustomClass(ThisClassP);
      ThisClassP = NULL;
    }
    #endif

    #ifdef SUPERCLASS
    if (ThisClass)
    {
      MUI_DeleteCustomClass(ThisClass);
      ThisClass = NULL;
    }
    #endif

    #ifdef PostClassExit
    PostClassExitFunc();
    #endif

    DROPINTERFACE(IIntuition);
    DROPINTERFACE(IGraphics);
    DROPINTERFACE(IDOS);
    DROPINTERFACE(IUtility);

    if (MUIMasterBase)
    {
      DROPINTERFACE(IMUIMaster);
      CloseLibrary(MUIMasterBase);
      MUIMasterBase = NULL;
    }
  }

  return(TRUE);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#if defined(__amigaos4__)
static ULONG LIBFUNC MCC_Query(UNUSED struct Interface *self, REG(d0, LONG which))
{
#elif defined(__MORPHOS__)
static ULONG MCC_Query(void)
{
  LONG which = (LONG)REG_D0;
#else
static ULONG LIBFUNC MCC_Query(REG(d0, LONG which))
{
#endif

  D(DBF_STARTUP, "MCC_Query: %d", which);

  switch (which)
  {
    #ifdef SUPERCLASS
    case 0: return((ULONG)ThisClass);
    #endif

    #ifdef SUPERCLASSP
    case 1: return((ULONG)ThisClassP);
    #endif

    #ifdef PREFSIMAGEOBJECT
    case 2:
    {
      Object *obj = PREFSIMAGEOBJECT;
      return((ULONG)obj);
    }
    #endif

    #ifdef ONLYGLOBAL
    case 3:
    {
      return(TRUE);
    }
    #endif

    #ifdef INFOCLASS
    case 4:
    {
      return(TRUE);
    }
    #endif

    #ifdef USEDCLASSES
    case 5:
    {
      return((ULONG)USEDCLASSES);
    }
    #endif

    #ifdef USEDCLASSESP
    case 6:
    {
      return((ULONG)USEDCLASSESP);
    }
    #endif

    #ifdef SHORTHELP
    case 7:
    {
      return((ULONG)SHORTHELP);
    }
    #endif
  }

  return(0);
}

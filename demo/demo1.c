
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <mui/TheBar_mcc.h>
#include <string.h>
#include <stdio.h>

/***********************************************************************/

long __stack = 8192;
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;

/***********************************************************************/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

/***********************************************************************/

struct MUIS_TheBar_Button buttons[] =
{
    {0, 0, "_Pred", "Pread mail."},
    {1, 1, "_Next", "Next mail."},
    {2, 2, "_Move", "Move somewhere."},
    {MUIV_TheBar_BarSpacer, 3},
    {3, 4, "_Forw", "Forward somewhere."},
    {4, 5, "F_ind", "Find something."},
    {5, 6, "_Save", "Save mail."},
    {MUIV_TheBar_End},
};

/***********************************************************************/

char *appareances[] = {"Images and text","Images","Text",NULL};
char *labelPoss[] = {"Bottom","Top","Right","Left",NULL};

int
main(int argc,char **argv)
{
    int res;

    if (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",39))
    {
        if (MUIMasterBase = OpenLibrary("muimaster.library",19))
        {
            Object *app, *win, *sb, *appareance, *labelPos, *borderless, *sunny, *raised, *scaled, *update;

            if (app = ApplicationObject,
                    MUIA_Application_Title,         "TheBar Demo1",
                    MUIA_Application_Version,       "$VER: TheBarDemo1 1.0 (24.6.2003)",
                    MUIA_Application_Copyright,     "Copyright 2003 by Alfonso Ranieri",
                    MUIA_Application_Author,        "Alfonso Ranieri <alforan@tin.it>",
                    MUIA_Application_Description,  "TheBar example",
                    MUIA_Application_Base,         "THEBAREXAMPLE",

                    SubWindow, win = WindowObject,
                        MUIA_Window_ID,             MAKE_ID('M','A','I','N'),
                        MUIA_Window_Title,          "TheBar Demo1",
                        WindowContents, VGroup,
                            Child, sb = TheBarObject,
                                MUIA_Group_Horiz,       TRUE,
                                MUIA_TheBar_EnableKeys, TRUE,
                                MUIA_TheBar_Buttons,    buttons,
                                MUIA_TheBar_PicsDrawer, "PROGDIR:Pics",
                                MUIA_TheBar_Strip,      "Read.toolbar",
                                MUIA_TheBar_SelStrip,   "Read_S.toolbar",
                                MUIA_TheBar_DisStrip,   "Read_G.toolbar",
                                MUIA_TheBar_StripCols,  11,
                            End,
                            Child, VGroup,
                                GroupFrameT("Settings"),
                                Child, HGroup,
                                    Child, Label2("Appareance"),
                                    Child, appareance = MUI_MakeObject(MUIO_Cycle,NULL,appareances),
                                    Child, Label2("Label pos"),
                                    Child, labelPos = MUI_MakeObject(MUIO_Cycle,NULL,labelPoss),
                                End,
                                Child, HGroup,
                                    Child, HSpace(0),
                                    Child, Label1("Borderless"),
                                    Child, borderless = MUI_MakeObject(MUIO_Checkmark,NULL),
                                    Child, HSpace(0),
                                    Child, Label1("Sunny"),
                                    Child, sunny = MUI_MakeObject(MUIO_Checkmark,NULL),
                                    Child, HSpace(0),
                                    Child, Label1("Raised"),
                                    Child, raised = MUI_MakeObject(MUIO_Checkmark,NULL),
                                    Child, HSpace(0),
                                    Child, Label1("Scaled"),
                                    Child, scaled = MUI_MakeObject(MUIO_Checkmark,NULL),
                                    Child, HSpace(0),
                                End,
                            End,
                            Child, update = MUI_MakeObject(MUIO_Button,"_Update"),
                        End,
                    End,
                End)
            {
                ULONG sigs = 0, id;

                DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
                DoMethod(update,MUIM_Notify,MUIA_Pressed,FALSE,app,2,MUIM_Application_ReturnID,TAG_USER);

                set(win,MUIA_Window_Open,TRUE);

                while ((id = DoMethod(app,MUIM_Application_NewInput,&sigs))!=MUIV_Application_ReturnID_Quit)
                {
                    if (id==TAG_USER)
                    {
                        ULONG appareanceV, labelPosV, borderlessV, sunnyV, raisedV, scaledV;

                        get(appareance,MUIA_Cycle_Active,&appareanceV);
                        get(labelPos,MUIA_Cycle_Active,&labelPosV);
                        get(borderless,MUIA_Selected,&borderlessV);
                        get(sunny,MUIA_Selected,&sunnyV);
                        get(raised,MUIA_Selected,&raisedV);
                        get(scaled,MUIA_Selected,&scaledV);

                        SetAttrs(sb,MUIA_TheBar_ViewMode,   appareanceV,
                                    MUIA_TheBar_LabelPos,   labelPosV,
                                    MUIA_TheBar_Borderless, borderlessV,
                                    MUIA_TheBar_Sunny,      sunnyV,
                                    MUIA_TheBar_Raised,     raisedV,
                                    MUIA_TheBar_Scaled,     scaledV,
                                    TAG_DONE);
                    }

                    if (sigs)
                    {
                        sigs = Wait(sigs | SIGBREAKF_CTRL_C);
                        if (sigs & SIGBREAKF_CTRL_C) break;
                    }
                }

                MUI_DisposeObject(app);

                res = RETURN_OK;
            }
            else
            {
                printf("%s: can't create application\n",argv[0]);
                res = RETURN_FAIL;
            }

            CloseLibrary(MUIMasterBase);
        }
        else
        {
            printf("%s: Can't open muimaster.library ver 19 or higher\n",argv[0]);
            res = RETURN_ERROR;
        }
    }
    else
    {
        printf("%s: Can't open intuition.library ver 39 or higher\n",argv[0]);
        res = RETURN_ERROR;
    }

    return res;
}

/***********************************************************************/

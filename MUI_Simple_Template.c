#include <libraries/mui.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <exec/types.h>
#include <clib/alib_protos.h>


#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

#define IPTR ULONG

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *MUIMasterBase;

BOOL Open_Libs(void)
{
  if ( !(IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",39)) )
    return(0);

  if ( !(GfxBase=(struct GfxBase *) OpenLibrary("graphics.library",0)) )
  {
    CloseLibrary((struct Library *)IntuitionBase);
    return(0);
  }

  if ( !(MUIMasterBase=OpenLibrary(MUIMASTER_NAME,19)) )
  {
    CloseLibrary((struct Library *)GfxBase);
    CloseLibrary((struct Library *)IntuitionBase);
    return(0);
  }

  return(1);
}

void Close_Libs(void)
{
  if (IntuitionBase)
    CloseLibrary((struct Library *)IntuitionBase);

  if (GfxBase)
    CloseLibrary((struct Library *)GfxBase);

  if (MUIMasterBase)
    CloseLibrary(MUIMasterBase);
}

//Buradan yukarýsý ana kütüphane ve minimum tanýmlarý içerir

// MUI Objects
Object *app, *win1, *bQuit;

int main(int argc,char *argv[])
{
	APTR	GROUP_ROOT_0;
	ULONG signals;
	BOOL running = TRUE;	
	
	if (!Open_Libs())
	{
		printf("Cannot open libs\n");
		return(0);
	}
	
	GROUP_ROOT_0 = GroupObject,	
		Child, CLabel2("Merhaba Dünya!"),
		Child, MUI_MakeObject(MUIO_HBar,4),
		
		Child, bQuit= SimpleButton("Quit"),
	End;
	
	MUIA_Application_Window, win1 = WindowObject,
			MUIA_Window_Title, "Pencere Adý",
			MUIA_Window_ID   , MAKE_ID('S','R','K','N'),
			MUIA_Window_Width, 400,
			WindowContents, GROUP_ROOT_0,
	End;
	
	app = ApplicationObject,
		MUIA_Application_Title      , "Project",
		MUIA_Application_Version    , "$VER: Project X.X (XX.XX.XX)",
		MUIA_Application_Copyright  , " ",
		MUIA_Application_Author     , " ",
		MUIA_Application_Description, " ", 
		MUIA_Application_Base       , " ",
		SubWindow, win1,
		
	End;

	if (!app)
	{
		printf("Cannot create application.\n");
		return(0);
	}
		/* Notifies */
		
		// Closing Application
        DoMethod(win1, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
          app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	
	DoMethod(bQuit, MUIM_Notify, MUIA_Pressed, FALSE,
          app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);


		
		
    	set(win1,MUIA_Window_Open,TRUE);// open window
    
	while(running)
	{
		ULONG id = DoMethod(app,MUIM_Application_Input,&signals);

		switch(id)
		{
				case MUIV_Application_ReturnID_Quit:
					//if((MUI_RequestA(app, win1, 0, "Quit?", "_Yes|_No", "\33cAre you sure?", 0)) == 1)
							running = FALSE;
					break;	
		}
		if(running && signals) Wait(signals);
	}

	set(win1,MUIA_Window_Open,FALSE);

    if(app) MUI_DisposeObject(app);
	Close_Libs();
	exit(TRUE);
}

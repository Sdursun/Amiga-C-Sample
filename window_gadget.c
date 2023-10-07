/**********************************************************************************
;
; Copyright 2023 ing. E. Th. van den Oosterkamp
;
; Example software for the book "Classic AmigaOS Programming" (ISBN  9781690195153)
;
; Permission is hereby granted, free of charge, to any person obtaining a copy 
; of this software and associated files (the "Software"), to deal in the Software 
; without restriction, including without limitation the rights to use, copy,
; modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
; and to permit persons to whom the Software is furnished to do so,
; subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in 
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
; INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
; PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
; HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
; OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
; SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
;
***********************************************************************************/


#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <intuition/intuitionbase.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <stdio.h>


// ======================================
// Internally used commands
// ======================================

#define CMD_NONE	0
#define CMD_ABOUT	0x10
#define CMD_OPTIONS	0x20
#define CMD_OPTHIGH	(CMD_OPTIONS+0)
#define CMD_OPTMID	(CMD_OPTIONS+1)
#define CMD_OPTLOW	(CMD_OPTIONS+2)
#define CMD_QUIT	0x100

#define SRC_MASK	0x3000
#define SRC_GADGET	0x1000
#define SRC_MENU	0x2000



// ======================================
// Menubar
// ======================================

struct Menu * CreateMenu( void * pVI )
{
	// GadTools will create the menubar from the following array of NewMenu structs
	struct NewMenu nm[] = {
					{ NM_TITLE, "Project",   0, 0, 0, 0 },
					{ NM_ITEM,	"About...",  "A", 0, 0, (APTR)CMD_ABOUT },
					{ NM_ITEM,	NM_BARLABEL, 0, 0, 0, 0 },
					{ NM_ITEM,	"Quit",      "Q", 0, 0, (APTR)CMD_QUIT },
					{ NM_TITLE, "Options",   0, 0, 0, 0 },
					{ NM_ITEM,	"High",      0, CHECKIT|MENUTOGGLE|CHECKED, 0x6, (APTR)CMD_OPTHIGH },
					{ NM_ITEM,	"Mid",       0, CHECKIT|MENUTOGGLE, 0x5, (APTR)CMD_OPTMID },
					{ NM_ITEM,	"Low",       0, CHECKIT|MENUTOGGLE, 0x3, (APTR)CMD_OPTLOW },
					{ NM_END, 0, 0, 0, 0, 0 }
						  };

	// Convert the above array into an Intuition menubar
	struct Menu * pMenu = CreateMenusA( nm, 0 );
	if ( !pMenu )
	{
		return NULL;
	}

	// Calculate the position and size of each menu and each item.
	LayoutMenusA( pMenu, pVI, 0 );

	return pMenu;
}

// --------------------------------------

int ProcessMenuPick( struct Menu * pMenu, struct IntuiMessage * pIMsg )
{
	// Check if the user selected anything.
	if ( ITEMNUM( pIMsg->Code ) == NOITEM )
	{
		return CMD_NONE;
	}

	// Get the address of the MenuItem struct related 
	// to the item the user selected
	char * pItem = (char *)ItemAddress( pMenu, pIMsg->Code );
	if ( !pItem )
	{
		return CMD_NONE;
	}

	// GadTools will have placed the UserData directly
	// after the MenuItem struct.
	pItem += sizeof( struct MenuItem );

	// Retrieve the UserData as an int.
	int Command = *(int *)pItem;

	// Add the source of the command
	return Command | SRC_MENU;
}


// ======================================
// Gadgets
// ======================================

struct Gadget * CreateGadgets( struct Window * pWin, 
								void * pVI, 
								struct Gadget * pGadgets[] )
{
	#define NUM_GADGETS	3

	// The labels and tags for the cycle gadget
	static char * cycLabels[] = { "High", "Mid", "Low", 0 };
	ULONG cycTags[] = { GTCY_Labels, (ULONG)cycLabels, TAG_END };

	// The arrays used for the creation of the gadgets	 
	int ggKind[ NUM_GADGETS ] = { BUTTON_KIND, BUTTON_KIND, CYCLE_KIND };
	ULONG * ggTags[ NUM_GADGETS ] = { NULL, NULL, cycTags };
	struct NewGadget ggNew[ NUM_GADGETS ] = {
			{ 100,78,85,14, "Quit", 0,1,0,pVI, (APTR)CMD_QUIT },
			{ 100,20,85,14, "About..", 0,2,0,pVI, (APTR)CMD_ABOUT },
			{ 100,35,85,14, "Options", 0,3,0,pVI, (APTR)CMD_OPTIONS },
								};
							
	// Create the context that GadTools will link the gadget list from
	struct Gadget * pGad;
	struct Gadget * pContext = CreateContext( &pGad );
	if ( !pContext )
	{
		return NULL;
	}
	
	for ( int Index=0; Index < NUM_GADGETS; ++Index )
	{
		pGad = CreateGadgetA( ggKind[ Index ], pGad, &ggNew[ Index ], 
								(struct TagItem *)ggTags[ Index ] );
		pGadgets[ Index ] = pGad;
	}

	// Add the list of gadgets to the window.
	AddGList( pWin, pContext, 0, -1, 0 );

	// Refresh the gadgets so they're shown correctly.
	RefreshGList( pContext, pWin, 0, -1 );
	

	return pContext;
}

// --------------------------------------

int ProcessGadgetUp( struct IntuiMessage * pIMsg )
{
	// Get the address of the GadGet struct related to the
	// gadget the user interacted with.
	struct Gadget * pGadget = (struct Gadget *)pIMsg->IAddress;
	if ( !pGadget )
	{
		return CMD_NONE;	
	}

	// The UserData field will contain the command.		
	int Command = (int)pGadget->UserData;

	// Is it the cycle gadget?	
	if ( Command == CMD_OPTIONS )
	{
		// For cycle gadgets the Code field of the message
		// will indicate the newly selected option.
		Command += pIMsg->Code;
	}
	
	return Command | SRC_GADGET;
}


// ======================================
// Keyboard action
// ======================================

int ProcessRawKey( struct IntuiMessage * pIMsg )
{
	// Nothing to do if the code is not "[Esc] released"
	if ( pIMsg->Code != 0xc5 )
	{
		return CMD_NONE;
	}
	
	return CMD_QUIT;
}


// ======================================
// Supporting functions
// ======================================

void ShowMessage( struct Window * pWin, const char * pMsg )
{
	const char * pGadgets = "Close";
	struct EasyStruct ES = {	sizeof( struct EasyStruct ),
								0,
								NULL,
								pMsg,
								pGadgets,
							};
	ULONG IDCMP=0;
	EasyRequestArgs( pWin, &ES, &IDCMP, NULL );
}

// --------------------------------------

void UpdateOptions( int Command, 
					struct Window * pWin, 
					struct Gadget * pGadList[] )
{
	// Mask out the source of the command
	int Source = Command & SRC_MASK;

	// Remove the command portion but keep the option number
	int Option = Command & 0x0F;

	// Do not update the gadget if it was the source
	if ( (Source != SRC_GADGET) && pGadList && pGadList[2] )
	{
		// The cycle gadget is updated via a taglist
		ULONG cycTags[] = { GTCY_Active, (ULONG)Option, TAG_END };
		GT_SetGadgetAttrsA( pGadList[2], pWin, NULL, 
								(struct TagItem *)&cycTags );
	}

	// Do not update the menu if it was the source
	if ( Source != SRC_MENU )
	{
		struct Menu * pMenu = pWin->MenuStrip;
		if ( pMenu )
		{
			// Remove the strp before making changes.
			ClearMenuStrip( pWin );

			// Get the address of the 1st item of the 2nd menu.
			UWORD MenuNumber = FULLMENUNUM( 1, 0, 0 );
			struct MenuItem * pItem = ItemAddress( pMenu, MenuNumber );

			// Process all three items
			for ( int Index=0; Index < 3; ++Index )
			{
				if ( pItem )
				{
					// Set the checkmark on the correct item,
					// otherwise remove it.
					if ( Index == Option )
					{
						pItem->Flags |= CHECKED; 
					}
					else
					{
						pItem->Flags &= ~CHECKED; 
					}

					// Advance to the next item.
					pItem = pItem->NextItem;
				}
			}

			// Place the modified strip back on the window.
			ResetMenuStrip( pWin, pMenu );
		}
	}
}


// ======================================
// The main loop
// ======================================

int main()
{
	char  Title[] = "Hello World!";
	ULONG WinTags[] = {
                        WA_Width,    	200,
                        WA_Height,   	100,
                        WA_Title,    	(ULONG)&Title,
                        WA_IDCMP,		IDCMP_CLOSEWINDOW|
				                        IDCMP_MENUPICK|
				                        IDCMP_RAWKEY|
				                        IDCMP_REFRESHWINDOW|
				                        BUTTONIDCMP|
				                        CYCLEIDCMP,
					    WA_DragBar,		1,	// Allow the window to be moved
					    WA_DepthGadget,	1,	// Add gadget to move front/back
                        WA_Activate, 	1,	// Make window active immediately
                        WA_CloseGadget,	1,	// Add gadget to close the window
                        WA_NewLookMenus,1,	// Use KS3.0 look
                        TAG_END
					  };

	struct Window * pWin = OpenWindowTagList( NULL, (struct TagItem *)&WinTags );
	if ( !pWin )
	{
		return -1;
	}

	void * pVI = GetVisualInfo( pWin->WScreen, NULL );

	struct Menu * pMenu = CreateMenu( pVI );
	if ( pMenu )
	{
		SetMenuStrip( pWin, pMenu );
	}

	struct Gadget * MyGadgets[ NUM_GADGETS ];

	struct Gadget * pGadgets = CreateGadgets( pWin, pVI, MyGadgets );

	int Command = CMD_NONE;
	while ( Command != CMD_QUIT )
	{
		// Put the task to sleep until an IDCMP message appears
		WaitPort( pWin->UserPort );

		// There could be multiple messages, process them all.
		struct IntuiMessage * pIMsg;
		while ( pIMsg = GT_GetIMsg( pWin->UserPort ) )
		{
			// The Class field indicates what caused the message
			switch ( pIMsg->Class )
			{
				case IDCMP_CLOSEWINDOW :
						Command = CMD_QUIT;
						break;

				case IDCMP_MENUPICK :
						Command = ProcessMenuPick( pMenu, pIMsg );
						printf( "MenuPick: %x\n", Command );
						break;
				
				case IDCMP_GADGETUP :
						Command = ProcessGadgetUp( pIMsg );
						printf( "GadgetUp: %x\n", Command );
						break;
				case IDCMP_RAWKEY :
						Command = ProcessRawKey( pIMsg );
						break;
			}

			// Reply to the message to indicate we're done with it.
			GT_ReplyIMsg( pIMsg );

			// Check if the user wants us to do something.
			switch ( Command & 0x0FF0 )
			{
				case CMD_ABOUT :
					ShowMessage( pWin, "This is the About message!" );
					Command = CMD_NONE;
					break;

				case CMD_OPTIONS :
					UpdateOptions( Command, pWin, MyGadgets );
					break;
			}
		}
	}

	// First close the window.
	CloseWindow( pWin );

	// Now it is safe to free the menu and gadets.
	FreeMenus( pMenu );
	FreeGadgets( pGadgets );

	// After menu and gadets the VisualInfo can be freed,
	FreeVisualInfo( pVI );


	return 0;
}

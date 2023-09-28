/* CheckBox Example
sc win.c LINK LIB lib:reaction.lib NOSTACKCHECK
quit
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <graphics/gfxmacros.h>
#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#include <gadgets/checkbox.h>
#include <images/label.h>
#include <gadgets/layout.h>
#include <classes/window.h>

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/wb.h>
#include <proto/icon.h>
#include <clib/alib_protos.h>

#include <proto/button.h>
#include <proto/label.h>
#include <proto/layout.h>
#include <proto/window.h>



enum
{
	GID_MAIN=0,
	GID_QUIT,
	GID_LAST
};

enum
{
	WID_MAIN=0,
	WID_LAST
};

enum
{
	OID_MAIN=0,
	OID_LAST
};

int main(void)
{
	struct MsgPort *AppPort;

	struct Window *windows[WID_LAST];

	struct Gadget *gadgets[GID_LAST];

	Object *objects[OID_LAST];

	/* make sure our classes opened... */
	if (!ButtonBase || !WindowBase || !LayoutBase)
		return(30);
	else if ( AppPort = CreateMsgPort() )
	{
		/* Create the window object.
		 */
		objects[OID_MAIN] = WindowObject,
			WA_ScreenTitle, "ReAction", //Workbench barında görünen adı
			WA_Title, "ReAction CheckBox Example", // Pencere başlığındaki adı
			WA_Activate, TRUE, //Açıldığında aktif pencere olsun
			WA_DepthGadget, TRUE,
			WA_DragBar, TRUE, //
			WA_CloseGadget, TRUE, //Kapatma düğmesi
			WA_SizeGadget, TRUE, //Boyutlandırma düğmesi
			WINDOW_IconifyGadget, TRUE, //Simge durumu düğmesi
			WINDOW_IconTitle, "CheckBox", //Simge durumunda görünen adı
			WINDOW_AppPort, AppPort,
			WINDOW_Position, WPOS_CENTERMOUSE, //Pencere açıldığıda ortası fare altında olsun
			WINDOW_ParentGroup, gadgets[GID_MAIN] = VGroupObject,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_DeferLayout, TRUE,

				

				LAYOUT_AddChild, HGroupObject,
					LAYOUT_BackFill, NULL,
					LAYOUT_SpaceOuter, TRUE,
					LAYOUT_VertAlignment, LALIGN_CENTER,
					LAYOUT_HorizAlignment, LALIGN_CENTER,
					LAYOUT_BevelStyle, BVS_FIELD,

					LAYOUT_AddImage, LabelObject,
						LABEL_Text, "The checkbox may have its label placed\n",
						LABEL_Text, "either on the left or right side.\n",
						LABEL_Text, " \n",
						LABEL_Text, "You may click the label text as well\n",
						LABEL_Text, "as the check box itself.\n",
					LabelEnd,
				LayoutEnd,
				//2 tane quit butonu oluştur
				LAYOUT_AddChild, ButtonObject,
					GA_ID, GID_QUIT,
					GA_RelVerify, TRUE,
					GA_Text,"_Quit",
				ButtonEnd,
				//CHILD_WeightedHeight, 0, //bu verilmez ise buton boyutlanır
				LAYOUT_AddChild, ButtonObject,
					GA_ID, GID_QUIT, //ID GID_QUIT
					GA_RelVerify, TRUE,
					GA_Text,"_Quit",
				ButtonEnd,
				CHILD_WeightedHeight, 0, //bunun ile buton yüksekliği sabitlenir
			EndGroup,
		EndWindow;

	 	/*  Object creation sucessful?
	 	 */
		if (objects[OID_MAIN])
		{
			/*  Open the window.
			 */
			if (windows[WID_MAIN] = (struct Window *) RA_OpenWindow(objects[OID_MAIN]))
			{
				ULONG wait, signal, app = (1L << AppPort->mp_SigBit);
				ULONG done = FALSE;
				ULONG result;
				UWORD code;

			 	/* Obtain the window wait signal mask.
				 */
				GetAttr(WINDOW_SigMask, objects[OID_MAIN], &signal);

				/* Input Event Loop
				 */
				while (!done)
				{
					wait = Wait( signal | SIGBREAKF_CTRL_C | app );

					if ( wait & SIGBREAKF_CTRL_C )
					{
						done = TRUE;
					}
					else
					{
						while ( (result = RA_HandleInput(objects[OID_MAIN], &code) ) != WMHI_LASTMSG )
						{
							switch (result & WMHI_CLASSMASK)
							{
								case WMHI_CLOSEWINDOW:
									windows[WID_MAIN] = NULL;
									done = TRUE;
									break;

								case WMHI_GADGETUP:
									switch (result & WMHI_GADGETMASK)
									{
										
										case GID_QUIT:
											done = TRUE;
											break;
									}
									break;

								case WMHI_ICONIFY:
									RA_Iconify(objects[OID_MAIN]);
									windows[WID_MAIN] = NULL;
									break;

								case WMHI_UNICONIFY:
									windows[WID_MAIN] = (struct Window *) RA_OpenWindow(objects[OID_MAIN]);

									if (windows[WID_MAIN])
									{
										GetAttr(WINDOW_SigMask, objects[OID_MAIN], &signal);
									}
									else
									{
										done = TRUE;	// error re-opening window!
									}
								 	break;
							}
						}
					}
				}
			}

			/* Disposing of the window object will also close the window if it is
			 * already opened, and it will dispose of the layout object attached to it.
			 */
			DisposeObject(objects[OID_MAIN]);
		}

		DeleteMsgPort(AppPort);
	}

	return(0);
}

#include <exec/types.h>
#include <intuition/intuition.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <stdio.h>

int main(void) {
    struct Window *window;
    struct IntuitionBase *IntuitionBase;
    
    BOOL running = TRUE;

    // Baþlangýçta Intuition kütüphanesini aç
    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37);
    if (!IntuitionBase) {
        printf("Intuition kütüphanesi açýlamadý.\n");
        return 1;
    }

    // Pencereyi aç
    window = OpenWindowTags(NULL,
        WA_Title, (ULONG)"Amiga NDK 3.2 Pencere Örneði",
        WA_Width, 400,
        WA_Height, 200,
        WA_CloseGadget, TRUE,
        WA_DragBar, TRUE,
        WA_DepthGadget, TRUE,
        WA_Activate, TRUE,
        TAG_END);

    if (!window) {
        printf("Pencere oluþturulamadý.\n");
        CloseLibrary((struct Library *)IntuitionBase);
        return 1;
    }

    // Pencerenin kapatýlmasýný bekleyin
    
    while (running) {
        ULONG signals = Wait(1L << window->UserPort->mp_SigBit);
        if (signals & (1L << window->UserPort->mp_SigBit)) {
            struct IntuiMessage *msg;
            while (msg = (struct IntuiMessage *)GetMsg(window->UserPort)) {
                if (msg->Class == IDCMP_CLOSEWINDOW) {
                    running = FALSE;
                }
                ReplyMsg((struct Message *)msg);
            }
        }
    }

    // Pencereyi kapat
    CloseWindow(window);

    // Intuition kütüphanesini kapat
    CloseLibrary((struct Library *)IntuitionBase);

    return 0;
}

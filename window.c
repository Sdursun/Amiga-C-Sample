//vc -c99 -lauto test.c -o Test
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <intuition/intuitionbase.h>
#include <intuition/intuition.h>
 
int main()
{
	char  Title[] = "Hello World!";
	ULONG WinTags[] = {
                        WA_Width,    	200,
                        WA_Height,   	100,
                        WA_Title,    	(ULONG)&Title,
                        WA_Activate, 	1,
			TAG_END
                     };
 
	struct Window * pWin = OpenWindowTagList( NULL, (struct TagItem *)&WinTags );
	if ( !pWin )
	{
		return -1;
	}
 
	Delay( 500 );
 
	CloseWindow( pWin );
 
	return 0;
}

//vc -c99 -lauto filename.c filename
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <intuition/intuitionbase.h>
#include <intuition/intuition.h>

int main()
{	
	struct Window * pWin;
	char  Title[] = "Hello World!";
	
	//Pencere özellikleri bu alanda tanımlanır
	ULONG WinTags[] = {
                        WA_Width,    	200,
                        WA_Height,   	100,
                        WA_Title,    	(ULONG)&Title, //Pencere başlığı burada oluşturulur
                        WA_Activate, 	1,
			TAG_END
                     };
	//Pencere açılır eğer hata oluşur ise -1 döner
	pWin = OpenWindowTagList( NULL, (struct TagItem *)&WinTags );
	if ( !pWin )
	{
		return -1;
	}
	
	Delay( 500 );
	CloseWindow( pWin ); //Pencere kapatılır
	return 0;
}

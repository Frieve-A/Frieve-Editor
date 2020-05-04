//---------------------------------------------------------------------------

#ifndef inetH
#define inetH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <WinInet.hpp>
bool IsInternetConnected();
//bool GetHtmlSourceF(AnsiString FileName, AnsiString string);
void GetHtml(UnicodeString Url, UnicodeString &html);
//---------------------------------------------------------------------------
#endif

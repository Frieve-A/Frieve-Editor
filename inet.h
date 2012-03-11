//---------------------------------------------------------------------------

#ifndef inetH
#define inetH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <WinInet.hpp>
bool IsInternetConnected();
//bool GetHtmlSourceF(AnsiString FileName, AnsiString string);
void GetHtml(AnsiString Url, AnsiString &html);
//---------------------------------------------------------------------------
#endif

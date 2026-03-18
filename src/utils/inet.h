// ---------------------------------------------------------------------------

#ifndef inetH
#define inetH
// ---------------------------------------------------------------------------
#include <WinInet.hpp>
#include <vcl.h>
bool IsInternetConnected();
// bool GetHtmlSourceF(AnsiString FileName, AnsiString string);
void GetHtml(UnicodeString Url, UnicodeString &html);
// ---------------------------------------------------------------------------
#endif

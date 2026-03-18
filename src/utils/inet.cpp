// ---------------------------------------------------------------------------

#include "inet.h"
#include "document.h"

// ---------------------------------------------------------------------------
bool IsInternetConnected() {
  DWORD ConnectType = INTERNET_CONNECTION_MODEM + INTERNET_CONNECTION_LAN +
                      INTERNET_CONNECTION_PROXY;
  return InternetGetConnectedState(&ConnectType, 0);
}

// ---------------------------------------------------------------------------
void GetHtml(UnicodeString Url, UnicodeString &html) {
  HINTERNET hSession;
  hSession = InternetOpen(UnicodeString(AppTitle).c_str(),
                          INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
  if (hSession) {
    HINTERNET hService;
    hService = InternetOpenUrl(hSession, Url.c_str(), NULL, 0, 0, 0);
    if (hService) {
      while (1) {
        char lpBuffer[1024 + 1]; // Bytes to read at once
        DWORD dwBytesRead;       // Bytes read
        InternetReadFile(hService, lpBuffer, 1024, &dwBytesRead);
        if (dwBytesRead == 0) {
          break; // Exit loop when bytes read is 0
        }
        lpBuffer[dwBytesRead] = 0; // Null-terminate for string
        html += lpBuffer;          // Append to html
      }
    }
    InternetCloseHandle(hService);
  }
  InternetCloseHandle(hSession);
}
// ---------------------------------------------------------------------------
#pragma package(smart_init)

// ---------------------------------------------------------------------------
// Export (BMP, text, hierarchical) (split from fomain.cpp)

#include <vcl.h>
#pragma hdrstop

#include <clipbrd.hpp>
#include "fomain.h"
#include "fomain_export.h"
#include "setting.h"

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void TFo_Main::ExportBMP(int operation, UnicodeString Text) {
  // Save browser image or copy (0=BMP, 1=JPEG, 2=Copy)
  PC_Client->ActivePage = TS_Browser;

  // Background buffer creation
  Graphics::TBitmap *BMP = new Graphics::TBitmap();
  m_nBrowserWidth = PB_Browser->Width;
  m_nBrowserHeight = PB_Browser->Height;
  BMP->Width = m_nBrowserWidth;
  BMP->Height = m_nBrowserHeight;

  m_fFontZoom = 1.0f;
  m_nBGColor = SettingView.m_nBackgroundColor;
  m_nFGColor = SettingView.m_nFourgroundColor;

  if (SettingView.m_bAntiAlias) {
    int countshift = 1;
    while ((1 << countshift) < SettingView.m_nAntiAliasX) {
      countshift++;
    }
    int count = SettingView.m_nAntiAliasX;
    int countshift2 = countshift + countshift;

    BMP->PixelFormat = pf32bit;

    m_nBrowserWidth = m_nBrowserWidth * count;
    m_nBrowserHeight = m_nBrowserHeight * count;

    // Create bitmap of multiple size
    Graphics::TBitmap *BMP2 = new Graphics::TBitmap();
    BMP2->Width = m_nBrowserWidth * SettingView.m_nAntiAliasX;
    BMP2->Height = m_nBrowserHeight * SettingView.m_nAntiAliasX;
    BMP2->PixelFormat = pf32bit;
    // Draw to BMP
    m_fFontZoom = SettingView.m_nAntiAliasX;
    PaintSub(BMP2->Canvas);

    // Anti-aliasing
    int **P2 = new int *[BMP2->Height];
    for (int iy = 0; iy < BMP2->Height; iy++) {
      P2[iy] = (int *)BMP2->ScanLine[iy];
    }
    for (int iy = 0; iy < BMP->Height; iy++) {
      int *PW = (int *)BMP->ScanLine[iy];
      for (int ix = 0; ix < BMP->Width; ix++) {
        int r = 0, g = 0, b = 0;
        for (int iy2 = 0; iy2 < count; iy2++) {
          int *PR = P2[(iy << countshift) + iy2] + (ix << countshift);
          int *PRE = PR + count;
          while (PR < PRE) {
            int c = *PR++;
            r += c & 0xff;
            g += c & 0xff00;
            b += c & 0xff0000;
          }
        }
        r = r >> countshift2;
        g = (g >> countshift2) & 0xff00;
        b = (b >> countshift2) & 0xff0000;
        *PW++ = r | g | b;
      }
    }
    delete[] P2;

    delete BMP2;

    m_nBrowserWidth = BMP->Width;
    m_nBrowserHeight = BMP->Height;

    float x = 1.0f / count;
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      m_CardX[i] *= x;
      m_CardY[i] *= x;
      m_CardWidth[i] *= x;
      m_CardHeight[i] *= x;
    }

    m_nFontHeight /= count;

    m_fFontZoom = 1.0f;
  } else {
    // Draw to BMP
    PaintSub(BMP->Canvas);
  }

  /*
   // Background buffer creation
   Graphics::TBitmap *BMP = new Graphics::TBitmap();
   m_nBrowserWidth = PB_Browser->Width;
   m_nBrowserHeight = PB_Browser->Height;
   BMP->Width = m_nBrowserWidth;
   BMP->Height = m_nBrowserHeight;

   // Draw to BMP
   m_fFontZoom = 1.0f;
   m_nBGColor = SettingView.m_nBackgroundColor;
   m_nFGColor = SettingView.m_nFourgroundColor;
   PaintSub(BMP->Canvas);
   */
  switch (operation) {
  case 0:
    BMP->SaveToFile(Text);
    break;
  case 1: {
    TJPEGImage *JI = new TJPEGImage();
    JI->Assign(BMP);
    JI->SaveToFile(Text);
    delete JI;
  } break;
  case 2:
    // Copy to clipboard
    Clipboard()->Assign(BMP);
    break;
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::ExportCardText(int operation, bool btitle, UnicodeString Text) {
  TStringList *SL = new TStringList();

  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    if (btitle) {
      SL->Add(Card->m_Title);
    } else {
      SL->Add(Card->m_Lines->Text);
    }
  }

  switch (operation) {
  case 0:
    try {
      SL->SaveToFile(Text);
    } catch (...) {
      ShowMessage(MLText.FailedToSave + SD_Txt->FileName + UnicodeString("."));
    }
    break;
  case 1:
    Clipboard()->SetTextBuf(SL->Text.c_str());
    break;
  }
}

void TFo_Main::AddTxtsInFolder(UnicodeString TopDir, TCard *Parent) {
  TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
  Card->m_Title = ExtractFileName(TopDir);
  if (Parent) {
    TLink *Link = m_Document->NewLink();
    Link->m_nFromID = Parent->m_nID;
    Link->m_nDestID = Card->m_nID;
  } else {
    Card->m_bTop = true;
  }

  void *Handle;
  TWin32FindData Data;
  Handle = FindFirstFile((TopDir + "\\*.*").c_str(), &Data);
  if (Handle == INVALID_HANDLE_VALUE)
    return;

  if (!((UnicodeString(Data.cFileName) == ".") ||
        (UnicodeString(Data.cFileName) == ".."))) {
    if (ExtractFileExt(Data.cFileName).UpperCase() == ".TXT") {
      TCard *Card2 = m_Document->NewCard(m_Document->m_Cards->Count);
      Card2->m_Title = ExtractFileNameOnly(Data.cFileName);
      Card2->m_Lines->LoadFromFile(TopDir + "\\" + Data.cFileName);

      TLink *Link = m_Document->NewLink();
      Link->m_nFromID = Card->m_nID;
      Link->m_nDestID = Card2->m_nID;
    }

    if (Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      AddTxtsInFolder(TopDir + "\\" + Data.cFileName, Card);
    }
  }

  while (FindNextFile(Handle, &Data)) {
    if (!((UnicodeString(Data.cFileName) == ".") ||
          (UnicodeString(Data.cFileName) == ".."))) {
      if (ExtractFileExt(Data.cFileName).UpperCase() == ".TXT") {
        TCard *Card2 = m_Document->NewCard(m_Document->m_Cards->Count);
        Card2->m_Title = ExtractFileNameOnly(Data.cFileName);
        Card2->m_Lines->LoadFromFile(TopDir + "\\" + Data.cFileName);

        TLink *Link = m_Document->NewLink();
        Link->m_nFromID = Card->m_nID;
        Link->m_nDestID = Card2->m_nID;
      }

      if (Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        AddTxtsInFolder(TopDir + "\\" + Data.cFileName, Card);
      }
    }
  }

  FindClose(Handle);
}

// ---------------------------------------------------------------------------
void TFo_Main::ExportHierarchicalText(TStringList *SL, int CurrentLevel,
                                      UnicodeString HChar,
                                      TCard *CurrentParent) {
  // Subroutine for hierarchical text export; output cards under CurrentParent;
  // recursively called
  for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
    TCard *Card = m_Document->GetCardByIndex(ic);
    if (Card) {
      if (Card->m_nParentID == CurrentParent->m_nID) {
        // Card under current parent

        // Output card title
        UnicodeString S;
        for (int ih = 0; ih < CurrentLevel; ih++) {
          S += HChar;
        }
        SL->Add(S + Card->m_Title);

        // Output card content
        for (int il = 0; il < Card->m_Lines->Count; il++) {
          SL->Add(Card->m_Lines->Strings[il]);
        }

        // Output child card content
        ExportHierarchicalText(SL, CurrentLevel + 1, HChar, Card);
      }
    }
  }
}

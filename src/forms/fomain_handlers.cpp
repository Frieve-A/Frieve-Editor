// ---------------------------------------------------------------------------
// Main form event handlers (split from fomain.cpp)
// Arrange, View, Import/Export, Statistics, Drawing, GlobalSearch, GPT

#include <vcl.h>
#pragma hdrstop

#include <math.h>
#include <DateUtils.hpp>
#include <Vcl.filectrl.hpp>
#include <clipbrd.hpp>
#include <printers.hpp>

#include "fomain.h"
#include "fomain_file.h"
#include "fomain_export.h"
#include "foedittext.h"
#include "fofullscreen.h"
#include "foshape.h"
#include "fosearch.h"
#include "foselectcard.h"
#include "foview.h"
#include "setting.h"
#include "utils.h"
#include "textproc.h"
#include <System.Classes.hpp>
#include <System.IOUtils.hpp>
#include <System.JSON.hpp>
#include <System.Net.HttpClient.hpp>
#include <System.Net.HttpClientComponent.hpp>
#include <System.Net.URLClient.hpp>

#pragma package(smart_init)

namespace {

bool IsMouseOnControl(TControl *Control, const TPoint &MousePos) {
  if (!Control || !Control->Visible) {
    return false;
  }

  TPoint P = Control->ScreenToClient(MousePos);
  return P.x >= 0 && P.y >= 0 && P.x < Control->Width && P.y < Control->Height;
}

int ClampValue(int value, int minValue, int maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}

int ConsumeWheelSteps(int &remainder, int wheelDelta) {
  remainder += wheelDelta;
  int steps = remainder / WHEEL_DELTA;
  remainder %= WHEEL_DELTA;
  return steps;
}

UnicodeString ResolveLocalizedHelpPath() {
  UnicodeString exeDir = ExtractFilePath(ParamStr(0));
  UnicodeString lang = Trim(SettingView.m_Language);
  if (lang == "" || lang == "English") {
    return exeDir + "help.fip";
  }

  UnicodeString suffix = LowerCase(lang);
  UnicodeString localized = exeDir + "help\\help_" + suffix + ".fip";
  if (FileExists(localized)) {
    return localized;
  }
  return exeDir + "help.fip";
}

} // namespace

bool TFo_Main::ShouldHandleBrowserWheel(const TPoint &MousePos) {
  if (PC_Client->ActivePage != TS_Browser || !TB_Zoom->Visible) {
    return false;
  }

  return IsMouseOnControl(PB_Browser, MousePos) ||
         IsMouseOnControl(Sc_X, MousePos) || IsMouseOnControl(Sc_Y, MousePos);
}

void TFo_Main::ScrollBrowserWheel(TScrollBar *ScrollBar, int &Remainder,
                                  int WheelDelta, bool positiveMovesForward) {
  if (!ScrollBar) {
    return;
  }

  int steps = ConsumeWheelSteps(Remainder, WheelDelta);
  if (steps == 0) {
    return;
  }

  int stepSize = ScrollBar->SmallChange;
  if (stepSize < 1) {
    stepSize = 1;
  }

  int pos = ScrollBar->Position;
  if (positiveMovesForward) {
    pos += stepSize * steps;
  } else {
    pos -= stepSize * steps;
  }

  ScrollBar->Position = ClampValue(pos, ScrollBar->Min, ScrollBar->Max);
}

void TFo_Main::ZoomBrowserWheel(int WheelDelta) {
  int steps = ConsumeWheelSteps(m_nBrowserZoomWheelRemainder, WheelDelta);
  if (steps == 0) {
    return;
  }

  int stepSize = (TB_Zoom->Max - TB_Zoom->Min) / 50;
  if (stepSize < 1) {
    stepSize = 1;
  }

  int pos = TB_Zoom->Position + stepSize * steps;
  TB_Zoom->Position = ClampValue(pos, TB_Zoom->Min, TB_Zoom->Max);
}

void TFo_Main::ResizeFontWheel(int WheelDelta) {
  int steps = ConsumeWheelSteps(m_nBrowserFontWheelRemainder, WheelDelta);
  while (steps > 0) {
    MVF_MagnifyClick(this);
    steps--;
  }
  while (steps < 0) {
    MVF_ReduceClick(this);
    steps++;
  }
}

void TFo_Main::ResetBrowserZoom() {
  TB_Zoom->Position = ClampValue(0, TB_Zoom->Min, TB_Zoom->Max);
}


void __fastcall TFo_Main::SB_ArrangeRefreshClick(TObject *Sender) {
  RefreshSimMatrix();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_ArrangeClick(TObject *Sender) {
  PrepareArrange();
  if (SB_Arrange->Down) {
    BackupSub(MLText.ArrangeCards);
  } else {
    // Was displaying Tree
    bool tree = Bu_ArrangeType->Tag >= 1000 && Bu_ArrangeType->Tag <= 1999;
    if (tree) {
      m_Document->RefreshList();
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PAT_ArrangeTypeClick(TObject *Sender) {
  bool tree = Bu_ArrangeType->Tag >= 1000 && Bu_ArrangeType->Tag <= 1999;
  Bu_ArrangeType->Tag = ((TMenuItem *)Sender)->Tag;
  if (SB_Arrange->Down &&
      tree != (Bu_ArrangeType->Tag >= 1000 && Bu_ArrangeType->Tag <= 1999)) {
    m_Document->RefreshList();
  }
  PrepareArrange();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_ArrangeTypeClick(TObject *Sender) {
  TPoint P;
  P.x = ((TControl *)Sender)->Left;
  P.y = ((TControl *)Sender)->Top;
  P = Pa_BrowserTop->ClientToScreen(P);
  PM_ArrangeType->Popup(P.x, P.y);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_FixClick(TObject *Sender) {
  if (m_nTargetCard >= 0) {
    BackupSub(MLText.FixCardPosition);
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bSelected) {
        Card->m_bFixed = SB_Fix->Down;
      }
    }
    /*
     TCard *Card = m_Document->GetCard(m_nTargetCard);
     Card->m_bFixed = SB_Fix->Down;
     */
    m_Document->RefreshList();
  }
}
// ---------------------------------------------------------------------------

float TFo_Main::MoveDistance(float sx, float sy, float dx, float dy,
                             int direction) {
  // direction: right, down, left, up
  switch (direction) {
  case 0: // Right
    if (sx > dx) {
      return (sx - dx) * (sx - dx) + (dy - sy) * (dy - sy) * 9.0f;
    } else {
      return -1.0f;
    }
  case 1: // Down
    if (sy > dy) {
      return (sy - dy) * (sy - dy) + (dx - sx) * (dx - sx) * 9.0f;
    } else {
      return -1.0f;
    }
  case 2: // Left
    if (sx < dx) {
      return (dx - sx) * (dx - sx) + (dy - sy) * (dy - sy) * 9.0f;
    } else {
      return -1.0f;
    }
  case 3: // Up
    if (sy < dy) {
      return (dy - sy) * (dy - sy) + (dx - sx) * (dx - sx) * 9.0f;
    } else {
      return -1.0f;
    }
  }
  return -1.0f;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_OverviewClick(TObject *Sender) {
  SettingView.m_bOverview = !SettingView.m_bOverview;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_AutoScrollClick(TObject *Sender) {
  m_nScrollTargetX = -65536;
  m_nScrollTargetY = -65536;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MV_BrowserClick(TObject *Sender) {
  PC_Client->ActivePage = TS_Browser;
  PC_ClientChange(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MH_ContentsClick(TObject *Sender) {
  ExitFullScreen();

  UnicodeString S =
      UnicodeString(ExtractFilePath(ParamStr(0))) + UnicodeString("feditor.exe");
  UnicodeString S2 = UnicodeString("\"") + ResolveLocalizedHelpPath() +
                     UnicodeString("\"");
  ShellExecute(Handle, NULL, S.c_str(), S2.c_str(), NULL, SW_SHOW);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MH_FrieveSiteClick(TObject *Sender) {
  ExitFullScreen();

  ShellExecute(Handle, NULL, UnicodeString(AppURL).c_str(), NULL, NULL,
               SW_SHOW);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MH_FIPSiteClick(TObject *Sender) {
  ExitFullScreen();

  ShellExecute(Handle, NULL, UnicodeString(SoftURL).c_str(), NULL, NULL,
               SW_SHOW);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_ViewClick(TObject *Sender) {
  Fo_View->m_Document = m_Document;

  TPoint P;
  P.x = SB_View->Left;
  P.y = SB_View->Top;
  P = Pa_BrowserTop->ClientToScreen(P);
  Fo_View->Left = P.x;
  Fo_View->Top = P.y;
  Fo_View->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MV_EditorClick(TObject *Sender) {
  ExitFullScreen();
  CloseEditBox();
  PC_Client->ActivePage = TS_Editor;
  PC_ClientChange(Sender);
  if (Pa_Editor->Visible) {
    RE_Edit->SetFocus();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVC_VideoClick(TObject *Sender) {
  SettingView.m_bVideoVisible = !SettingView.m_bVideoVisible;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVL_LinkHemmingClick(TObject *Sender) {
  SettingView.m_bLinkHemming = !SettingView.m_bLinkHemming;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVL_LinkShadowClick(TObject *Sender) {
  SettingView.m_bLinkShadow = !SettingView.m_bLinkShadow;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVC_CardShadowClick(TObject *Sender) {
  SettingView.m_bCardShadow = !SettingView.m_bCardShadow;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVC_TickerVisibleClick(TObject *Sender) {
  SettingView.m_bTickerVisible = !SettingView.m_bTickerVisible;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVC_TickerLineClick(TObject *Sender) {
  SettingView.m_nTickerLines = ((TMenuItem *)Sender)->Tag;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVC_CardGradationClick(TObject *Sender) {
  SettingView.m_bCardGradation = !SettingView.m_bCardGradation;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PBN_NewCardClick(TObject *Sender) {
  BrowserNewCard();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PBL_DeleteLinkClick(TObject *Sender) {
  BackupSub(PI_DeleteLink->Caption);

  m_Document->DeleteLinkByIndex(m_nTargetLink);
  m_nTargetLink = -1;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PBL_InsertCardClick(TObject *Sender) {
  BrowserInsertCardToLink();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PBC_NewChildCardClick(TObject *Sender) {
  BrowserNewChildCard(false);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PBC_NewBrotherCardClick(TObject *Sender) {
  BrowserNewBrotherCard();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PBC_SetAsDefaultClick(TObject *Sender) {
  TCard *Card = m_Document->GetCard(m_nTargetCard);
  if (Card) {
    SettingView.m_nCardDefaultShape = Card->m_nShape;
    SettingView.m_nCardDefaultSize = Card->m_nSize;
    SettingView.m_bCardDefaultFixed = Card->m_bFixed;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_LinkShapeClick(TObject *Sender) {
  TPoint P;
  P.x = SB_LinkShape->Left;
  P.y = SB_LinkShape->Top;
  P = Pa_Card->ClientToScreen(P);

  TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
  if (Link) {
    Fo_Shape->IL_Shape = IL_LinkShape;
    Fo_Shape->m_nMode = 1;
    Fo_Shape->m_nItemIndex = Link->m_nShape;
    Fo_Shape->m_bIgnoreChange = true;
    Fo_Shape->Left = P.x;
    Fo_Shape->Top = P.y;
    Fo_Shape->Show();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PBL_SetAsDefaultClick(TObject *Sender) {
  TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
  if (Link) {
    SettingView.m_nLinkDefaultShape = Link->m_nShape;
    // SettingView.m_nLinkDefaultSize = Link->m_nSize;
    // SettingView.m_bLinkDefaultFixed = Link->m_bFixed;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MEC_AllCardsClick(TObject *Sender) {
  TPoint P;
  GetCursorPos(&P);

  Fo_Shape->IL_Shape = IL_Shape;
  Fo_Shape->m_nMode = 2;
  Fo_Shape->m_nItemIndex = SettingView.m_nCardDefaultShape;
  Fo_Shape->m_bIgnoreChange = true;
  Fo_Shape->Left = P.x;
  Fo_Shape->Top = P.y;
  Fo_Shape->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MEC_AllLinksClick(TObject *Sender) {
  TPoint P;
  GetCursorPos(&P);

  Fo_Shape->IL_Shape = IL_LinkShape;
  Fo_Shape->m_nMode = 3;
  Fo_Shape->m_nItemIndex = SettingView.m_nLinkDefaultShape;
  Fo_Shape->m_bIgnoreChange = true;
  Fo_Shape->Left = P.x;
  Fo_Shape->Top = P.y;
  Fo_Shape->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFI_TxtFileClick(TObject *Sender) {
  if (OD_Txt->Execute()) {
    BackupSub(MF_Import->Caption + UnicodeString(" ") + MFI_TxtFile->Caption);

    for (int i = 0; i < OD_Txt->Files->Count; i++) {
      TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
      Card->m_Title = ExtractFileNameOnly(OD_Txt->Files->Strings[i]);
      Card->m_Lines->LoadFromFile(OD_Txt->Files->Strings[i]);
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_TxtFileClick(TObject *Sender) {
  if (SD_Txt->Execute()) {
    TFileStream *FS = new TFileStream(SD_Txt->FileName, fmCreate);

    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      Card->m_Lines->SaveToStream(FS);
    }

    delete FS;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_TxtFilesClick(TObject *Sender) {
  if (m_Document->m_Cards->Count == 0) {
    return;
  }
  SD_Txt->FileName =
      ForFileName(m_Document->GetCardByIndex(0)->m_Title) + ".txt";
  if (SD_Txt->Execute()) {
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      TFileStream *FS = new TFileStream(ExtractFilePath(SD_Txt->FileName) +
                                            ForFileName(Card->m_Title) + ".txt",
                                        fmCreate);
      Card->m_Lines->SaveToStream(FS);
      delete FS;
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_HtmlFilesClick(TObject *Sender) {
  if (m_Document->m_Cards->Count == 0) {
    return;
  }
  UnicodeString DocumentTitle;
  if (m_Document->m_FN != "") {
    DocumentTitle = ExtractFileNameOnly(m_Document->m_FN);
  }
  SD_Html->FileName = "index.html";
  if (SD_Html->Execute()) {
    TStringList *SL = new TStringList();
    SL->Add("<html>");
    SL->Add("<head>");
    SL->Add(UnicodeString("<title>") + DocumentTitle +
            UnicodeString("</title>"));
    SL->Add("</head>");
    SL->Add("<frameset cols=\"25%,*\">");
    SL->Add("<frame src=\"cards.html\">");
    SL->Add("<frame src=\"./1.html\" name=\"card\">");
    SL->Add("<noframes>");
    SL->Add("<body></body>");
    SL->Add("</noframes>");
    SL->Add("</frameset>");
    SL->Add("</html>");
    try {
      SL->SaveToFile(SD_Html->FileName);
    } catch (...) {
      ShowMessage(MLText.FailedToSave + SD_Html->FileName +
                  UnicodeString("."));
    }
    delete SL;

    SL = new TStringList();
    SL->Add("<html>");
    SL->Add("<head>");
    SL->Add(UnicodeString("<title>") + DocumentTitle + "</title>");
    SL->Add("<link rel=StyleSheet href=\"./style.css\">");
    SL->Add("</head>");
    SL->Add("<body>");
    SL->Add(UnicodeString("<font size=+2>") + DocumentTitle + "</font>");
    SL->Add("<hr>");
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      SL->Add(UnicodeString("<a href=\"./") + IntToStr(i + 1) +
              ".html\" target=\"card\">" + Card->m_Title + "</a><br>");
    }
    SL->Add("<hr>");
    try {
      SL->SaveToFile(ExtractFilePath(SD_Html->FileName) + "cards.html");
    } catch (...) {
      ShowMessage(MLText.FailedToSave + ExtractFilePath(SD_Html->FileName) +
                  "cards.html" + UnicodeString("."));
    }
    delete SL;

    if (m_Document->m_FN != "") {
      DocumentTitle = ExtractFileNameOnly(m_Document->m_FN) + " - ";
    }
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      TStringList *SL2 = new TStringList();
      SL2->Add("<html>");
      SL2->Add("<head>");
      SL2->Add(UnicodeString("<title>") + DocumentTitle + Card->m_Title +
               "</title>");
      SL2->Add("<link rel=StyleSheet href=\"./style.css\">");
      SL2->Add("</head>");
      SL2->Add("<body>");
      SL2->Add(UnicodeString("<font size=+2>") + DocumentTitle + Card->m_Title +
               "</font>");
      SL2->Add("<hr>");
      SL2->Add(UnicodeString("<a href=\"./1.html\">[Top]</a> "));
      if (i > 0) {
        SL2->Add(UnicodeString("<a href=\"./") + IntToStr(i - 1 + 1) +
                 ".html\">[Prev]</a> ");
      } else {
        SL2->Add("[Prev] ");
      }
      if (i < m_Document->m_Cards->Count - 1) {
        SL2->Add(UnicodeString("<a href=\"./") + IntToStr(i + 1 + 1) +
                 ".html\">[Next]</a> ");
      } else {
        SL2->Add("[Next] ");
      }
      SL2->Add(UnicodeString("<a href=\"./") +
               IntToStr(m_Document->m_Cards->Count) +
               UnicodeString(".html\">[End]</a> "));
      SL2->Add(UnicodeString("(") + IntToStr(i + 1) + UnicodeString(" / ") +
               IntToStr(m_Document->m_Cards->Count) + UnicodeString(")"));
      SL2->Add("<hr>");
      for (int il = 0; il < m_Document->m_Links->Count; il++) {
        TLink *Link = m_Document->GetLinkByIndex(il);
        int dest = -1;
        if (Link->m_nFromID == Card->m_nID) {
          dest = Link->m_nDestID;
        } else if (Link->m_nDestID == Card->m_nID) {
          dest = Link->m_nFromID;
        }
        if (dest >= 0) {
          int card2index = m_Document->SearchCardIndex(dest);
          TCard *Card2 = m_Document->GetCardByIndex(card2index);
          SL2->Add(UnicodeString("<a href=\"./") + IntToStr(card2index + 1) +
                   ".html\">[" + Card2->m_Title + "]</a> ");
        }
      }
      SL2->Add("<hr>");
      SL2->Add("<br>");
      for (int il = 0; il < Card->m_Lines->Count; il++) {
        if (IsFileNameOrURL(Card->m_Lines->Strings[il])) {
          SL2->Add(UnicodeString("<a href=\"") + Card->m_Lines->Strings[il] +
                   UnicodeString("\">") + Card->m_Lines->Strings[il] +
                   UnicodeString("</a><br>"));
        } else {
          SL2->Add(Card->m_Lines->Strings[il] + "<br>");
        }
      }
      SL2->Add("<br>");
      int count = 0;
      for (int il = 0; il < m_Document->m_Links->Count; il++) {
        TLink *Link = m_Document->GetLinkByIndex(il);
        int dest = -1;
        if (Link->m_nFromID == Card->m_nID) {
          dest = Link->m_nDestID;
        } else if (Link->m_nDestID == Card->m_nID) {
          dest = Link->m_nFromID;
        }
        if (dest >= 0) {
          if (count++ == 0) {
            SL2->Add("<hr>");
          }
          int card2index = m_Document->SearchCardIndex(dest);
          TCard *Card2 = m_Document->GetCardByIndex(card2index);
          SL2->Add(UnicodeString("<a href=\"./") + IntToStr(card2index + 1) +
                   ".html\">[" + Card2->m_Title + "]</a> ");
        }
      }
      SL2->Add("<hr>");
      SL2->Add("</body>");
      SL2->Add("</html>");
      try {
        SL2->SaveToFile(ExtractFilePath(SD_Html->FileName) + IntToStr(i + 1) +
                       ".html");
      } catch (...) {
        ShowMessage(MLText.FailedToSave + ExtractFilePath(SD_Html->FileName) +
                    IntToStr(i + 1) + ".html" + UnicodeString("."));
      }
      delete SL2;
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_TxtFileCardTitleClick(TObject *Sender) {
  if (SD_Txt->Execute()) {
    ExportCardText(0, true, SD_Txt->FileName);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_ClipboardBMPClick(TObject *Sender) {
  ExportBMP(2, "");
  /*
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

   if (SettingView.m_bAntiAlias){
   int countshift = 1;
   while ((1 << countshift) < SettingView.m_nAntiAliasX){
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
   int **P2 = new int*[BMP2->Height];
   for (int iy = 0 ; iy < BMP2->Height ; iy++){
   P2[iy] = (int*)BMP2->ScanLine[iy];
   }
   for (int iy = 0 ; iy < BMP->Height ; iy++){
   int *PW = (int*)BMP->ScanLine[iy];
   for (int ix = 0 ; ix < BMP->Width ; ix++){
   int r = 0, g = 0, b = 0;
   for (int iy2 = 0 ; iy2 < count ; iy2++){
   int *PR = P2[(iy << countshift) + iy2] + (ix << countshift);
   int *PRE = PR + count;
   while (PR < PRE){
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
   for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
   m_CardX[i] *= x;
   m_CardY[i] *= x;
   m_CardWidth[i] *= x;
   m_CardHeight[i] *= x;
   }

   m_nFontHeight /= count;

   m_fFontZoom = 1.0f;
   }else{
   // Draw to BMP
   PaintSub(BMP->Canvas);
   }

   // Copy to clipboard
   Clipboard()->Assign(BMP);

   // Discard BMP
   delete BMP;
   */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_ClipboardCardTitleClick(TObject *Sender) {
  ExportCardText(1, true, "");
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_ClipboardCardTextClick(TObject *Sender) {
  ExportCardText(1, false, "");
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_BMPFileClick(TObject *Sender) {
  if (SD_BMP->Execute()) {
    ExportBMP(0, SD_BMP->FileName);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_JPEGFileClick(TObject *Sender) {
  if (SD_JPEG->Execute()) {
    ExportBMP(1, SD_JPEG->FileName);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFI_HierarchicalTextFileClick(TObject *Sender) {
  // Hierarchical text load
  if (OD_Txt->Execute()) {
    BackupSub(MF_Import->Caption + UnicodeString(" ") +
              MFI_HierarchicalTextFile->Caption);

    TStringList *SL = new TStringList();
    TStringList *SLK = new TStringList();
    for (int i = 0; i < OD_Txt->Files->Count; i++) {
      SL->LoadFromFile(OD_Txt->Files->Strings[i]);

      // Hierarchy symbol
      for (int il = 0; il < SL->Count; il++)
        if (SL->Strings[il] != "") {
          WideString W = SL->Strings[il];
          SLK->Add(W.SubString(1, 1));
        }
      SLK->Sort();

      UnicodeString K; // Hierarchy symbol
      if (SLK->Count) {
        UnicodeString Last; // Hierarchy symbol
        int count = 1;
        int max = 0;
        Last = SLK->Strings[0];
        for (int i2 = 1; i2 < SLK->Count; i2++) {
          if (SLK->Strings[i2] == Last) {
            count++;
            if (count >= max) {
              max = count;
              K = SLK->Strings[i2];
            }
          } else {
            count = 1;
            Last = SLK->Strings[i2];
          }
        }
      }
      SLK->Clear();

      TList *HList = new TList(); // Store each current hierarchy

      // Top node
      TCard *Top = m_Document->NewCard(m_Document->m_Cards->Count);
      Top->m_bTop = true;
      HList->Add(Top);
      Top->m_Title = ExtractFileNameOnly(OD_Txt->Files->Strings[i]);

      // Line loop
      for (int il = 0; il < SL->Count; il++)
        if (Trim(SL->Strings[il]) != "") {
          TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
          int level = 1;
          WideString W = SL->Strings[il];
          while (true) {
            if (W.SubString(1, 1) == K) {
              W = W.SubString(2, W.Length());
              level++;
            } else {
              break;
            }
          }
          Card->m_Title = W;

          // Hierarchy management
          // Delete levels at or above self
          for (int ih = HList->Count - 1; ih >= level; ih--) {
            HList->Delete(ih);
          }
          // If level skipped (e.g. 0->2), fill gaps with NULL
          for (int ih = HList->Count; ih < level; ih++) {
            HList->Add(NULL);
          }
          // Write self to current level
          HList->Add(Card);

          // Find parent card
          TCard *Parent = NULL;
          int plevel = level - 1;
          while (plevel >= 0 && Parent == NULL) {
            if (plevel < HList->Count) {
              Parent = (TCard *)HList->Items[plevel];
            }
            if (!Parent) {
              plevel--;
            }
          }

          // Link from parent card
          if (Parent) {
            TLink *Link = m_Document->NewLink();
            Link->m_nFromID = Parent->m_nID;
            Link->m_nDestID = Card->m_nID;
          }
        }
      delete HList;

      /*
       TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
       Card->m_Title = ExtractFileNameOnly(OD_Txt->Files->Strings[i]);
       Card->m_Lines->LoadFromFile(OD_Txt->Files->Strings[i]);
       */
    }
    delete SLK;
    delete SL;

    m_Document->RefreshList();
    m_Document->RefreshLink();
  }
}
// ---------------------------------------------------------------------------
/*
 UnicodeString SelectDirectory(UnicodeString stTitle)
 {
 HResult hres;
 int *ip0, ip1;
 Shlobj::TBrowseInfo bi;
 int *lpBuffer;
 UnicodeString rs;

 hres  = SHGetSpecialFolderLocation(Application.Handle, CSIDL_DESKTOP, ip0);
 if (FAILED(hres) )  {
 Exit;
 }
 SetLength(rs, MAX_PATH);
 bi.hwndOwner     = Handle;
 bi.pidlRoot      = ip0;
 bi.lpszTitle     = stTitle;
 bi.ulFlags       = 0;
 bi.lpfn          = NULL;
 bi.lParam        = 0;
 ip1 = SHBrowseForFolder(bi);
 if (!(ip1 == NULL) )
 {
 lpBuffer  = Marshal.StringToHGlobalAnsi(rs);
 if (SHGetPathFromIDList(ip1,lpBuffer) )
 {
 if (!(lpBuffer == NULL) )
 Result  = Marshal.PtrToStringUni(lpBuffer);
 }
 }
 }
 */

void __fastcall TFo_Main::MFI_TxtFilesinaFolderClick(TObject *Sender) {
  UnicodeString Dir;
  if (SelectDirectory(L"", L"", Dir)) {
    BackupSub(MF_Import->Caption + UnicodeString(" ") +
              MFI_TxtFilesinaFolder->Caption);

    AddTxtsInFolder(Dir, NULL);
    /*
     // File enumeration
     TStringList *Files = new TStringList();
     FileListCreator(Dir, Files, ".txt");
     Files->Sort();

     // Folder enumeration
     TStringList *Dirs = new TStringList();
     for (int i = 0 ; i < Files->Count ; i++){
     UnicodeString Dir = ExtractFileDir(Files->Strings[i]);
     if (Dirs->Count == 0){
     UnicodeString TopDir = ExtractFileDir(Dir);
     if (Dir == ""){
     "No Title";
     }
     Dirs->Add(TopDir);
     Dirs->Add(Dir);
     }else if (Dirs->Strings[Dirs->Count - 1] != Dir){
     Dirs->Add(Dir);
     }
     }

     // Create folder cards
     TList *DirCards = new TList();
     for (int i = 0 ; i < Files->Count ; i++){
     TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
     Card->m_Title = Dirs->Strings[i];

     // Find folder containing this Dir
     int parentindex = -1;
     int max = 0;
     for (int i2 = 0 ; i2 < i ; i2++){
     if (Dirs->Strings[i].Pos(Dirs->Strings[i2] + "\\") == 1){
     if (Dirs->Strings[i2].Length() > max){
     max = Dirs->Strings[i2].Length();
     parentindex = i2;
     }
     }
     }

     if (parentindex >= 0){
     // Create link
     TCard *Parent = (TCard*)DirCards->Items[parentindex];
     TLink *Link = m_Document->NewLink();
     Link->m_nFromID = Parent->m_nID;
     Link->m_nDestID = Card->m_nID;
     }

     DirCards->Add(Card);
     }


     TList *Cards = new TList();

     for (int i = 0 ; i < Files->Count ; i++){
     TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
     Card->m_Title = ExtractFileNameOnly(Files->Strings[i]);
     Card->m_Lines->LoadFromFile(Files->Strings[i]);

     // Find folder containing this Dir
     int parentindex = -1;
     int max = 0;
     for (int i2 = 0 ; i2 < Dirs->Count ; i2++){
     if (Files->Strings[i].Pos(Dirs->Strings[i2] + "\\") == 1){
     if (Dirs->Strings[i2].Length() > max){
     max = Dirs->Strings[i2].Length();
     parentindex = i2;
     }
     }
     }

     if (parentindex >= 0){
     // Create link
     TCard *Parent = (TCard*)DirCards->Items[parentindex];
     TLink *Link = m_Document->NewLink();
     Link->m_nFromID = Parent->m_nID;
     Link->m_nDestID = Card->m_nID;
     }

     Cards->Add(Card);
     }

     delete Cards;
     delete DirCards;
     delete Files;
     delete Dirs;
     */

    m_Document->RefreshList();
    m_Document->RefreshLink();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_AntiAliasingClick(TObject *Sender) {
  SettingView.m_bAntiAlias = !SettingView.m_bAntiAlias;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_AAClick(TObject *Sender) {
  SettingView.m_nAntiAliasX = ((TMenuItem *)Sender)->Tag;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_BrowserTestClick(TObject *Sender) {
  // Undo Redo test
  UnicodeString str;
  bool b = m_UndoRedo->GetCanUndo(str);
  if (b) {
    ShowMessage(str);
    m_UndoRedo->Undo(m_Document, m_nCurrentCard, RE_Edit->SelStart,
                     RE_Edit->SelLength, &m_nNextCardID, &m_nNextSelStart,
                     &m_nNextSelLength);
  } else {
    // b = m_UndoRedo->GetCanRedo(&str);
    m_UndoRedo->Redo(m_Document, m_nCurrentCard, RE_Edit->SelStart,
                     RE_Edit->SelLength, &m_nNextCardID, &m_nNextSelStart,
                     &m_nNextSelLength);
  }

  /*
   TDocument *NewDoc = new TDocument(*m_Document);
   delete m_Document;
   m_Document = NewDoc;
   */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_BGAnimationClick(TObject *Sender) {
  SettingView.m_bBGAnimation = !SettingView.m_bBGAnimation;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVOB_BGAnimationTypeClick(TObject *Sender) {
  SettingView.m_nBGAnimationType = ((TMenuItem *)Sender)->Tag;
  memset(m_BGAnimationBuf, 0, sizeof(m_BGAnimationBuf));
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_RedoClick(TObject *Sender) {
  UnicodeString undostr;
  if (!m_UndoRedo->GetCanRedo(undostr)) {
    return;
  }
  m_UndoRedo->Redo(m_Document, m_nCurrentCard, RE_Edit->SelStart,
                   RE_Edit->SelLength, &m_nNextCardID, &m_nNextSelStart,
                   &m_nNextSelLength);
}

// ---------------------------------------------------------------------------
void TFo_Main::MoveToSelectedAndRecentCard() {
  // Move to most recently touched card
  m_nTargetCard = -1;
  double max = 0.0f;
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card2 = m_Document->GetCardByIndex(i);
    if (Card2->m_bSelected && Card2->m_fViewed > max) {
      m_nTargetCard = Card2->m_nID;
      max = Card2->m_fViewed;
    }
  }
}

// ---------------------------------------------------------------------------

void __fastcall TFo_Main::LB_ListClick(TObject *Sender) {
  if (LB_List->ItemIndex >= 0) {
    // selectionsetting
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      Card->m_bSelected = LB_List->Selected[i];
    }

    TCard *Card = m_Document->GetCardByIndex(LB_List->ItemIndex);
    if (Card) {
      if (Card->m_bSelected) {
        m_nTargetCard = Card->m_nID;
        m_nTargetLink = -1;
      } else {
        MoveToSelectedAndRecentCard();
      }
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::LB_ListMouseDown(TObject *Sender, TMouseButton Button,
                                           TShiftState Shift, int X, int Y) {
  if (Button == mbLeft) {
    Ti_Check->Enabled = false;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::LB_ListMouseUp(TObject *Sender, TMouseButton Button,
                                         TShiftState Shift, int X, int Y) {
  if (Button == mbLeft) {
    Ti_Check->Enabled = true;
  }
  m_nLastTimeOut = GetTickCount();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::RE_EditExit(TObject *Sender) {
  if (PC_Client->ActivePage != TS_Editor &&
      (!SettingView.m_bEditInBrowserAlways || Fo_FullScreen->Visible)) {
    CloseTextEditBox();
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::CloseTextEditBox() {
  RE_Edit->Parent = Pa_Editor;
  RE_Edit->Align = alClient;
  Sp_BrowserRight->Visible = false;
  Sp_BrowserBottom->Visible = false;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::RE_EditKeyDown(TObject *Sender, WORD &Key,
                                         TShiftState Shift) {
  if (Key == 27) {
    RE_EditExit(Sender);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVT_BrowserPosClick(TObject *Sender) {
  SettingView.m_nEditInBrowserPos = ((TMenuItem *)Sender)->Tag;
  CloseTextEditBox();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVT_AlwaysShowEditorClick(TObject *Sender) {
  SettingView.m_bEditInBrowserAlways = !SettingView.m_bEditInBrowserAlways;
  CloseTextEditBox();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVT_EditInBrowserClick(TObject *Sender) {
  SettingView.m_bEditInBrowser = !SettingView.m_bEditInBrowser;
  CloseTextEditBox();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MV_EditInBrowserClick(TObject *Sender) {
  if (PC_Client->ActivePage == TS_Browser) {
    m_bTextEditRequested = true;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MV_EditTitleClick(TObject *Sender) {
  if (PC_Client->ActivePage == TS_Editor) {
    if (Ed_Title->Enabled) {
      Ed_Title->SetFocus();
    }
  } else if (PC_Client->ActivePage == TS_Browser) {
    if (m_nTargetCard >= 0) {
      m_bTitleEditRequested = true;
    } else if (m_nTargetLink >= 0 && Ed_LinkTitle->Enabled) {
      Ed_LinkTitle->SetFocus();
      Ed_LinkTitle->SelectAll();
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PBN_SelectClick(TObject *Sender) {
  SB_Cursor->Down = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PBN_LinkClick(TObject *Sender) {
  SB_Line->Down = true;
}

// ---------------------------------------------------------------------------
void TFo_Main::SetCardAssign() {
  // Called when Label Fold needs to be considered outside of Paint
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    m_CardAssign[i] = i;
  }

  // Card loop
  // First check fold state of all cards
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    Card->m_bLabelIsFold = m_Document->LabelIsFold(Card);
  }

  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    if (Card->m_bLabelIsFold) {
      // All folded

      if (m_CardAssign[i] == i) {
        // Representative card
        // Card loop
        // Assign cards with same label to card indicated by i
        for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
          if (i != i2) {
            TCard *Card2 = m_Document->GetCardByIndex(i2);

            if (m_Document->LabelIsSame(Card, Card2)) {
              // Same label structure card
              // Assign coords to first found; non-selected cards assigned to
              // backmost
              m_CardAssign[i2] = i;
            }
          }
      } /* else{
       // Neither selected nor representative

       int idxbak = m_CardAssign[i];

       // Card loop
       // Make this card representative
       for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if
       (m_CardAssign[i2] == idxbak){ m_CardAssign[i2] = i;
       }
       }
       */
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_CursorAnimationClick(TObject *Sender) {
  SettingView.m_bFocusCursorAnimation = !SettingView.m_bFocusCursorAnimation;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MEC_AllLinksDirectionClick(TObject *Sender) {
  BackupSub(ME_BatchConversion->Caption);
  for (int i = 0; i < m_Document->m_Links->Count; i++) {
    TLink *Link = m_Document->GetLinkByIndex(i);
    int bak = Link->m_nFromID;
    Link->m_nFromID = Link->m_nDestID;
    Link->m_nDestID = bak;
  }
  m_Document->RefreshList();
  m_Document->RefreshLink();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_TopClick(TObject *Sender) {
  if (m_nTargetCard >= 0) {
    BackupSub(MLText.SetAsRootCard);
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bSelected) {
        Card->m_bTop = SB_Top->Down;
      }
    }
    m_Document->RefreshList();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_CardListClick(TObject *Sender) {
  SettingView.m_bCardList = !SettingView.m_bCardList;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_FileListClick(TObject *Sender) {
  SettingView.m_bFileList = !SettingView.m_bFileList;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_EditorRelatedClick(TObject *Sender) {
  SettingView.m_bEditorRelatedVisible = SB_EditorRelated->Down;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_CloseGlobalSearchClick(TObject *Sender) {
  Sp_GlobalSearch->Visible = false;
  Pa_GlobalSearch->Visible = false;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_GlobalSearchPaint(TObject *Sender) {
  // Background buffer creation
  Graphics::TBitmap *BMP = new Graphics::TBitmap();
  BMP->Width = PB_GlobalSearch->Width;
  BMP->Height = PB_GlobalSearch->Height;

  TCanvas *C = BMP->Canvas;
  TBrush *B = C->Brush;
  TPen *P = C->Pen;
  TFont *F = C->Font;
  F->Name = SettingView.m_Font->Name;
  F->Charset = SettingView.m_Font->Charset;
  F->Size = SettingView.m_Font->Size;
  F->Color = TColor(m_nFGColor);

  int textheight = C->TextHeight(" ");
  m_GlobalSearchItemHeight = textheight * 3;

  // Background fill
  B->Color = TColor(m_nBGColor);
  C->FillRect(Rect(0, 0, BMP->Width, BMP->Height));

  int itemcount = BMP->Height / m_GlobalSearchItemHeight;
  int largechange = itemcount;
  if (largechange < 1) {
    largechange = 1;
  }
  Sc_GlobalSearch->LargeChange = largechange;
  Sc_GlobalSearch->SmallChange = 1;

  // Get current index
  m_GlobalSearchCursorIndex = -1;
  for (int i = 0; i < m_GlobalSearchResult->Count; i++) {
    TCard *Card = m_Document->GetCard((int)(intptr_t)m_GlobalSearchResult->Items[i]);
    if (Card) {
      if (m_nTargetCard == Card->m_nID) {
        m_GlobalSearchCursorIndex = i;
      }
    }
  }

  for (int i = 0; i < itemcount + 1; i++) {
    int Index = i + Sc_GlobalSearch->Position;

    if (Index < m_GlobalSearchResult->Count) {
      // In range
      TCard *Card =
          m_Document->GetCard((int)(intptr_t)m_GlobalSearchResult->Items[Index]);
      if (Card) {
        // Target card exists

        // Draw frame
        float sizex = 1.0f;
        TColor c = GetCardColor(Card, sizex);
        B->Style = bsSolid;
        // if (m_nTargetCard == Card->m_nID){
        if (Index == m_GlobalSearchCursorIndex) {
          P->Color = HalfColor(c, m_nBGColor, 0.33f);
          B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
          P->Width = 3;
        } else {
          P->Color = HalfColor(c, m_nBGColor, 0.75f);
          B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
          P->Width = 1;
        }

        int top = m_GlobalSearchItemHeight * i;
        C->RoundRect(
            0, top + 2, BMP->Width - 3, top + m_GlobalSearchItemHeight - 2,
            m_GlobalSearchItemHeight / 4, m_GlobalSearchItemHeight / 4);

        HRGN MyRgn;
        MyRgn = ::CreateRectRgn(textheight / 2 - 2, 0,
                                BMP->Width - textheight / 2, BMP->Height);
        ::SelectClipRgn(C->Handle, MyRgn);

        B->Style = bsClear;
        // Draw card title
        {
          int foundat = 0;
          WideString Title = DecodeES(Card->m_Title, " ");
          if (m_GlobalSearchOption & 0x1) {
            // Title search
            if (m_GlobalSearchOption & 0x10) {
              // Search from title
              WideString Target = WideLowerCase(Title);
              foundat = Target.Pos(WideLowerCase(m_GlobalSearchKeyword));
            } else {
              // Case-sensitive
              foundat = Title.Pos(m_GlobalSearchKeyword);
            }
          }
          if (foundat == 0) {
            // Simple title display
            C->TextOut(textheight / 2 - 2, top + textheight / 2, Title);
          } else {
            // Emphasize found portion

            WideString S1;

            // Display left side
            S1 = Title.SubString(1, foundat - 1);
            int widthsum = 0;
            C->TextOut(textheight / 2 - 2, top + textheight / 2, S1);
            widthsum += C->TextWidth(S1);

            // Display keyword
            S1 = Title.SubString(foundat, m_GlobalSearchKeyword.Length());
            F->Color = clRed;
            F->Style = TFontStyles() << fsBold;
            C->TextOut(textheight / 2 - 2 + widthsum, top + textheight / 2, S1);
            widthsum += C->TextWidth(S1);
            F->Color = TColor(m_nFGColor);
            F->Style = TFontStyles();

            // Display right side
            S1 = Title.SubString(foundat + m_GlobalSearchKeyword.Length(),
                                 Title.Length());
            C->TextOut(textheight / 2 - 2 + widthsum, top + textheight / 2, S1);
          }
        }

        {
          int foundat = 0;
          WideString Text = Card->m_Lines->Text;
          if (m_GlobalSearchOption & 0x2) {
            // Body search
            if (m_GlobalSearchOption & 0x10) {
              WideString Target = WideLowerCase(Text);
              foundat = Target.Pos(WideLowerCase(m_GlobalSearchKeyword));
            } else {
              foundat = Text.Pos(m_GlobalSearchKeyword);
            }
          }

          int maxlen = BMP->Width / C->TextWidth(" ");
          if (foundat == 0) {
            // Simple body display
            Text = Text.SubString(1, maxlen);
            C->TextOut(textheight / 2 - 2, top + textheight + textheight / 2,
                       Text);
          } else {
            int start = foundat - (maxlen - m_GlobalSearchKeyword.Length()) / 2;
            if (start < 1) {
              start = 1;
            }
            // bool endleader = start - 1 + maxlen < Text.Length();
            Text = Text.SubString(start, maxlen);
            foundat -= start - 1;
            /*
             if (start != 1){
             Text = WideString("...") + Text;
             }
             if (endleader){
             Text += WideString("...");
             }
             */
            while (C->TextWidth(Text) > BMP->Width - textheight / 2) {
              int leftlen = foundat - 1;
              int rightlen = Text.Length() - m_GlobalSearchKeyword.Length() -
                             (foundat - 1);
              int delpos;
              if (leftlen < rightlen || foundat <= 1) {
                delpos = 1; // Delete from end
              } else {
                delpos = 2; // Delete from head
              }
              Text = Text.SubString(delpos, Text.Length() - 1);
              if (delpos == 2) {
                foundat--;
              }
            }
            // C->TextOut(textheight / 2 - 2, top + textheight + textheight / 2,
            // Text);
            WideString S1;

            // UpdateDisplay
            S1 = Text.SubString(1, foundat - 1);
            int widthsum = 0;
            C->TextOut(textheight / 2 - 2, top + textheight + textheight / 2,
                       S1);
            widthsum += C->TextWidth(S1);

            // Display keyword
            S1 = Text.SubString(foundat, m_GlobalSearchKeyword.Length());
            F->Color = clRed;
            F->Style = TFontStyles() << fsBold;
            C->TextOut(textheight / 2 - 2 + widthsum,
                       top + textheight + textheight / 2, S1);
            widthsum += C->TextWidth(S1);
            F->Color = TColor(m_nFGColor);
            F->Style = TFontStyles();

            // Display right side
            S1 = Text.SubString(foundat + m_GlobalSearchKeyword.Length(),
                                Text.Length());
            C->TextOut(textheight / 2 - 2 + widthsum,
                       top + textheight + textheight / 2, S1);
          }
        }
        ::SelectClipRgn(C->Handle, NULL);
        ::DeleteObject(MyRgn);
      }
    }
  }

  // Draw to foreground
  PB_GlobalSearch->Canvas->Draw(0, 0, BMP);
  delete BMP;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sc_GlobalSearchChange(TObject *Sender) {
  if (Sc_GlobalSearch->Tag == 0) {
    PB_GlobalSearchPaint(Sender);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_GlobalSearchMouseDown(TObject *Sender,
                                                   TMouseButton Button,
                                                   TShiftState Shift, int X,
                                                   int Y) {
  Sc_GlobalSearch->SetFocus();
  int Index = Y / m_GlobalSearchItemHeight + Sc_GlobalSearch->Position;
  if (Index >= 0 && Index < m_GlobalSearchResult->Count) {
    TCard *Card = m_Document->GetCard((int)(intptr_t)m_GlobalSearchResult->Items[Index]);
    if (Card) {
      m_Document->ClearCardSelection();
      m_nTargetCard = Card->m_nID;
      Card->m_bSelected = true;
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sc_GlobalSearchKeyDown(TObject *Sender, WORD &Key,
                                                 TShiftState Shift) {
  // La_GlobalSearchTop->Caption = Key;//36,33,38,40,34,35
  int targetindex = m_GlobalSearchCursorIndex;
  if (targetindex < 0) {
    targetindex = 0;
  }

  int itemcount = 1;
  if (m_GlobalSearchItemHeight) {
    itemcount = PB_GlobalSearch->Height / m_GlobalSearchItemHeight;
  }

  switch (Key) {
  case 36: // Home
    targetindex = 0;
    break;
  case 33: // Page Up
    targetindex -= itemcount;
    break;
  case 38: // Up
    targetindex--;
    break;
  case 40: // Down
    targetindex++;
    break;
  case 34: // Page Down
    targetindex += itemcount;
    break;
  case 35: // End
    targetindex = m_GlobalSearchResult->Count - 1;
    break;
  }

  if (targetindex < 0) {
    targetindex = 0;
  }
  if (targetindex >= m_GlobalSearchResult->Count) {
    targetindex = m_GlobalSearchResult->Count - 1;
  }

  if (targetindex != m_GlobalSearchCursorIndex) {
    Key = 0;
    TCard *Card =
        m_Document->GetCard((int)(intptr_t)m_GlobalSearchResult->Items[targetindex]);
    m_Document->ClearCardSelection();
    Card->m_bSelected = true;
    m_nTargetCard = Card->m_nID;

    Sc_GlobalSearch->Tag = 1;
    if (targetindex < Sc_GlobalSearch->Position) {
      Sc_GlobalSearch->Position = targetindex;
    } else if (targetindex >= Sc_GlobalSearch->Position + itemcount - 1) {
      Sc_GlobalSearch->Position = targetindex - itemcount + 1;
    }
    Sc_GlobalSearch->Tag = 0;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PE_CutToNewCardTitleWithLinkClick(TObject *Sender) {
  BackupSub(PE_CutToNewCardTitleWithLink->Caption);

  m_bDoNotBackup = true;

  // Cut
  ME_CutClick(Sender);

  TMemo *Me = new TMemo(this);
  Me->Parent = this;
  Me->WordWrap = false;
  Me->PasteFromClipboard();

  int from = m_nTargetCard;

  for (int i = 0; i < Me->Lines->Count; i++) {
    UnicodeString S = Trim(Me->Lines->Strings[i]);
    if (S != "") {
      // Create new card
      MI_NewCardClick(Sender);

      // Refresh card display
      Ti_CheckTimer(Sender);

      // Paste
      Ed_Title->SelectAll();
      Ed_Title->SetFocus();
      Ed_Title->Text = S;

      // Link
      if (m_nTargetCard >= 0) {
        TLink *Link = m_Document->NewLink();
        Link->m_nFromID = from;
        Link->m_nDestID = m_nTargetCard;
      }
    }
  }

  delete Me;

  m_bDoNotBackup = false;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_NoScrollLagClick(TObject *Sender) {
  SettingView.m_bNoScrollLag = !SettingView.m_bNoScrollLag;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFI_HierarchicalTextFile2Click(TObject *Sender) {
  // Hierarchical text load
  if (OD_Txt->Execute()) {
    BackupSub(MF_Import->Caption + UnicodeString(" ") +
              MFI_HierarchicalTextFile2->Caption);

    TStringList *SL = new TStringList();
    for (int i = 0; i < OD_Txt->Files->Count; i++) {
      SL->LoadFromFile(OD_Txt->Files->Strings[i]);
      UnicodeString K;
      if (SL->Count) {
        K = SL->Strings[0].SubString(1, 1);
      }

      TList *HList = new TList(); // Store each current hierarchy

      // Top node
      TCard *Top = m_Document->NewCard(m_Document->m_Cards->Count);
      Top->m_bTop = true;
      HList->Add(Top);
      Top->m_Title = ExtractFileNameOnly(OD_Txt->Files->Strings[i]);

      // Line loop
      for (int il = 0; il < SL->Count; il++)
        if (Trim(SL->Strings[il]) != "") {
          TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
          int level = 1;
          WideString W = SL->Strings[il];
          while (true) {
            if (W.SubString(1, 1) == K) {
              W = W.SubString(2, W.Length());
              level++;
            } else {
              break;
            }
          }
          Card->m_Title = W;

          // Hierarchy management
          // Delete levels at or above self
          for (int ih = HList->Count - 1; ih >= level; ih--) {
            HList->Delete(ih);
          }
          // If level skipped (e.g. 0->2), fill gaps with NULL
          for (int ih = HList->Count; ih < level; ih++) {
            HList->Add(NULL);
          }
          // Write self to current level
          HList->Add(Card);

          // Find parent card
          TCard *Parent = NULL;
          int plevel = level - 1;
          while (plevel >= 0 && Parent == NULL) {
            if (plevel < HList->Count) {
              Parent = (TCard *)HList->Items[plevel];
            }
            if (!Parent) {
              plevel--;
            }
          }

          // Link from parent card
          if (Parent) {
            TLink *Link = m_Document->NewLink();
            Link->m_nFromID = Parent->m_nID;
            Link->m_nDestID = Card->m_nID;
          }

          // Bodysetting
          while (il + 1 < SL->Count) {
            if (SL->Strings[il + 1].SubString(1, 1) != K) {
              Card->m_Lines->Add(SL->Strings[il + 1]);
              il++;
            } else {
              break;
            }
          }
        }
      delete HList;

      /*
       TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
       Card->m_Title = ExtractFileNameOnly(OD_Txt->Files->Strings[i]);
       Card->m_Lines->LoadFromFile(OD_Txt->Files->Strings[i]);
       */
    }
    delete SL;

    m_Document->RefreshList();
    m_Document->RefreshLink();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::TB_ZoomKeyDown(TObject *Sender, WORD &Key,
                                         TShiftState Shift) {
  // PgUp,Downzoom by
  switch (Key) {
  case 33: // PgUp
  {
    int pos = TB_Zoom->Position;
    pos -= (TB_Zoom->Max - TB_Zoom->Min) / 50;
    if (pos < TB_Zoom->Min) {
      pos = TB_Zoom->Min;
    }
    TB_Zoom->Position = pos;
  }
    Key = 0;
    break;
  case 34: // PgDn
  {
    int pos = TB_Zoom->Position;
    pos += (TB_Zoom->Max - TB_Zoom->Min) / 50;
    if (pos > TB_Zoom->Max) {
      pos = TB_Zoom->Max;
    }
    TB_Zoom->Position = pos;
  }
    Key = 0;
    break;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sp_BrowserRightMoved(TObject *Sender) {
  SettingView.m_nEditorWidthInBrowser = RE_Edit->Width;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sp_BrowserBottomMoved(TObject *Sender) {
  SettingView.m_nEditorHeightInBrowser = RE_Edit->Height;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::LB_LinkClick(TObject *Sender) {
  // Move to related text
  if (SB_EditorRelated->Down && LB_Link->ItemIndex >= 0) {
    TLink *Link = m_Document->GetLinkByIndex(
        (int)(intptr_t)m_LinkIndexes->Items[LB_Link->ItemIndex]);
    if (Link) {
      int id;
      if (Link->m_nFromID == m_nCurrentCard) {
        id = Link->m_nDestID;
      } else if (Link->m_nDestID == m_nCurrentCard) {
        id = Link->m_nFromID;
      }
      TCard *Card = m_Document->GetCard(id);
      WideString Target = Me_EditorRelated->Text;
      WideString Key = "Title : " + Card->m_Title;
      int foundat = Target.Pos(Key);
      if (foundat >= 1) {
        // Update?
        Me_EditorRelated->HideSelection = false;
        Me_EditorRelated->SelStart = Target.Length() - 1;
        Me_EditorRelated->SelLength = 1;
        Me_EditorRelated->SelStart = foundat - 1;
        Me_EditorRelated->SelLength = Key.Length();
        // UnicodeString(SearchKeyword).Length();
      } else {
        Me_EditorRelated->HideSelection = true;
      }
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::RE_EditEnter(TObject *Sender) {
  Me_EditorRelated->HideSelection = true;
}

// ---------------------------------------------------------------------------
class TSRectToCard {
public:
  UnicodeString m_Name;
  TRect m_Rect;
  TList *m_CardIDList;

  TSRectToCard();
  virtual ~TSRectToCard();

private:
  TSRectToCard(const TSRectToCard &source);
  TSRectToCard &operator=(const TSRectToCard &source);
};

// ---------------------------------------------------------------------------
TSRectToCard::TSRectToCard() {
  m_Rect = Rect(0, 0, 0, 0);
  m_CardIDList = new TList();
}

// ---------------------------------------------------------------------------
TSRectToCard::~TSRectToCard() { delete m_CardIDList; }

// ---------------------------------------------------------------------------
void TFo_Main::ClearStatisticsRectToCard() {
  for (int i = 0; i < m_StatisticsRectToCard->Count; i++) {
    delete (TSRectToCard *)m_StatisticsRectToCard->Items[i];
  }
  m_StatisticsRectToCard->Clear();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_StatisticsPaint(TObject *Sender) {
  // Background buffer creation
  Graphics::TBitmap *BMP = new Graphics::TBitmap();
  BMP->Width = PB_Statistics->Width;
  BMP->Height = PB_Statistics->Height;

  // Prepare and initialize
  TCanvas *C = BMP->Canvas;
  TBrush *B = C->Brush;
  TPen *P = C->Pen;
  TFont *F = C->Font;
  F->Name = RE_Edit->Font->Name;
  F->Charset = RE_Edit->Font->Charset;
  F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);
  m_nFontHeight = C->TextHeight(" ");

  // Background fill
  B->Color = TColor(m_nBGColor);
  B->Style = bsSolid;
  C->FillRect(Rect(0, 0, BMP->Width, BMP->Height));

  ClearStatisticsRectToCard();

  {
    // Bar graph

    // Name and value
    TStringList *Name = new TStringList();
    TList *Value = new TList();
    TList *Color = new TList();

    const int topmargin = m_nFontHeight * 2.5, leftmargin = m_nFontHeight / 2;

    switch (Bu_StatisticsKey->Tag) {
    case 0: {
      // Label count
      const int labeltype = 0;
      for (int i = 0; i < m_Document->m_Labels[labeltype]->Count; i++) {
        TCardLabel *Label = m_Document->GetLabelByIndex(labeltype, i);
        if (Label->m_bEnable) {
          TSRectToCard *SRTC = new TSRectToCard();
          Name->Add(Label->m_Name);
          float count = 0.0f;
          for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
            TCard *Card = m_Document->GetCardByIndex(ic);
            if (Card->m_Labels->Contain(i + 1)) {
              count += 1.0f;
              SRTC->m_CardIDList->Add((void *)(intptr_t)Card->m_nID);
            }
          }
          int icount = *(int *)&count;
          Value->Add((void *)(intptr_t)icount);
          Color->Add((void *)(intptr_t)Label->m_nColor);
          m_StatisticsRectToCard->Add(SRTC);
        }
      }
    } break;
    case 10000: // Total
    case 10100: // Source
    case 10200: // Destination
    {
      // Number of links
      int *NumLinks = new int[m_Document->m_Cards->Count];
      memset(NumLinks, 0, sizeof(int) * m_Document->m_Cards->Count);

      // Link loop
      for (int i = 0; i < m_Document->m_Links->Count; i++) {
        TLink *Link = m_Document->GetLinkByIndex(i);

        if (Bu_StatisticsKey->Tag == 10000 || Bu_StatisticsKey->Tag == 10100) {
          // Source side
          int idx = m_Document->SearchCardIndex(Link->m_nFromID);
          NumLinks[idx]++;
        }

        if (Bu_StatisticsKey->Tag == 10000 || Bu_StatisticsKey->Tag == 10200) {
          // Link line
          int idx = m_Document->SearchCardIndex(Link->m_nDestID);
          NumLinks[idx]++;
        }
      }

      // Link destination
      int maxlinks = 0;
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        if (maxlinks < NumLinks[i]) {
          maxlinks = NumLinks[i];
        }
      }

      // Create link
      for (int i = 0; i <= maxlinks; i++) {
        TSRectToCard *SRTC = new TSRectToCard();
        Name->Add(IntToStr(i) + " Links");
        float count = 0.0f;
        for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
          if (NumLinks[ic] == i) {
            TCard *Card = m_Document->GetCardByIndex(ic);
            SRTC->m_CardIDList->Add((void *)(intptr_t)Card->m_nID);
            count += 1.0f;
          }
        }
        int icount = *(int *)&count;
        Value->Add((void *)(intptr_t)icount);
        Color->Add((void *)(intptr_t)GetColor(i * 1.0f / (maxlinks + 1), 0, 255));
        m_StatisticsRectToCard->Add(SRTC);
      }

      delete[] NumLinks;
    } break;
    case 20000:
    case 20100:
    case 20200:
    case 20300:
    case 20400:
    case 30000:
    case 30100:
    case 30200:
    case 30300:
    case 30400:
    case 40000:
    case 40100:
    case 40200:
    case 40300:
    case 40400: {
      int type1 = Bu_StatisticsKey->Tag / 10000;
      int type2 = (Bu_StatisticsKey->Tag / 100) % 100;

      // Sort
      switch (type2) {
      case 0:
      case 1:
      case 2: {
        // Card under current parent
        double *dt = new double[m_Document->m_Cards->Count];
        for (int i = 0; i < m_Document->m_Cards->Count; i++) {
          TCard *Card = m_Document->GetCardByIndex(i);
          double date = 0.0;
          switch (type1) {
          case 2:
            date = Card->m_fCreated;
            break;
          case 3:
            date = Card->m_fUpdated;
            break;
          case 4:
            date = Card->m_fViewed;
            break;
          }
          dt[i] = date;
        }
        // Output child card content
        for (int i = 1; i < m_Document->m_Cards->Count; i++) {
          for (int i2 = i; i2 > 0; i2--) {
            if (dt[i2] > dt[i2 - 1]) {
              double t = dt[i2];
              dt[i2] = dt[i2 - 1];
              dt[i2 - 1] = t;
            } else {
              break;
            }
          }
        }

        UnicodeString Last;
        for (int i = 0; i < m_Document->m_Cards->Count; i++) {
          UnicodeString S;
          switch (type2) {
          case 0:
            S = IntToStr(YearOf(dt[i]));
            break;
          case 1:
            S = IntToStr(MonthOf(dt[i])) + "/" + IntToStr(YearOf(dt[i]));
            break;
          case 2:
            S = DateToStr(dt[i]);
            break;
          }

          if (S != Last) {
            Name->Add(S);
            Last = S;
          }
        }

        delete[] dt;
      } break;
      case 3:
        Name->Add("MON");
        Name->Add("TUE");
        Name->Add("WED");
        Name->Add("THU");
        Name->Add("FRI");
        Name->Add("SAT");
        Name->Add("SUN");
        break;
      case 4:
        for (int i = 0; i < 24; i++) {
          Name->Add(IntToStr(i));
        }
        break;
      }

      for (int i = 0; i < Name->Count; i++) {
        Color->Add((void *)(intptr_t)GetColor(i * 1.0f / Name->Count, 0, 255));
        Value->Add((void *)(intptr_t)0);
        TSRectToCard *SRTC = new TSRectToCard();
        m_StatisticsRectToCard->Add(SRTC);
      }

      // Update?
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        TCard *Card = m_Document->GetCardByIndex(i);
        double date = 0.0;
        switch (type1) {
        case 2:
          date = Card->m_fCreated;
          break;
        case 3:
          date = Card->m_fUpdated;
          break;
        case 4:
          date = Card->m_fViewed;
          break;
        }
        int index = 0;
        switch (type2) {
        case 0:
        case 1:
        case 2: {
          UnicodeString S;
          switch (type2) {
          case 0:
            S = IntToStr(YearOf(date));
            break;
          case 1:
            S = IntToStr(MonthOf(date)) + "/" + IntToStr(YearOf(date));
            break;
          case 2:
            S = DateToStr(date);
            break;
          }
          for (int i2 = 0; i2 < Name->Count; i2++) {
            if (Name->Strings[i2] == S) {
              index = i2;
              break;
            }
          }
        } break;
        case 3: // Week
          index = DayOfTheWeek(date) - 1;
          break;
        case 4: // Hour
          index = HourOf(date);
          break;
        }

        int l1 = (int)(intptr_t)Value->Items[index];
        float v1 = *(float *)&l1;
        v1 += 1.0f;
        int iv1 = *(int *)&v1;
        Value->Items[index] = (void *)(intptr_t)iv1;

        TSRectToCard *SRTC =
            (TSRectToCard *)m_StatisticsRectToCard->Items[index];
        SRTC->m_CardIDList->Add((void *)(intptr_t)Card->m_nID);
      }
    } break;
    }

    // Break
    if (SB_StatisticsSort->Down) {
      // Insert break
      for (int i = 1; i < Value->Count; i++) {
        for (int i2 = i; i2 > 0; i2--) {
          int l1 = (int)(intptr_t)Value->Items[i2];
          float v1 = *(float *)&l1;
          int l2 = (int)(intptr_t)Value->Items[i2 - 1];
          float v2 = *(float *)&l2;

          if (v1 > v2) {
            Value->Items[i2] = (void *)(intptr_t)l2;
            Value->Items[i2 - 1] = (void *)(intptr_t)l1;

            UnicodeString S = Name->Strings[i2];
            Name->Strings[i2] = Name->Strings[i2 - 1];
            Name->Strings[i2 - 1] = S;

            void *c = Color->Items[i2];
            Color->Items[i2] = Color->Items[i2 - 1];
            Color->Items[i2 - 1] = c;

            void *srtc = m_StatisticsRectToCard->Items[i2];
            m_StatisticsRectToCard->Items[i2] =
                m_StatisticsRectToCard->Items[i2 - 1];
            m_StatisticsRectToCard->Items[i2 - 1] = srtc;
          } else {
            break;
          }
        }
      }
    }

    // Bar graph drawing

    // Determine label panel size
    float max = 1.0f;
    int textwidth = 0;
    for (int i = 0; i < Name->Count; i++) {
      int tw = C->TextWidth(Name->Strings[i]);
      if (tw > textwidth) {
        textwidth = tw;
      }

      int l = (int)(intptr_t)Value->Items[i];
      float v = *(float *)&l;
      if (v > max) {
        max = v;
      }
    }
    int lineheignt = m_nFontHeight * 2;   // Line height
    int hsize = Name->Count * lineheignt; // Total line height for display
    float hvisible = (BMP->Height - topmargin - leftmargin) / lineheignt;
    // First visible line index
    if (hvisible * 2 >= Name->Count || Name->Count == 0) {
      Sc_StatisticsY->LargeChange = 10000;
    } else {
      Sc_StatisticsY->LargeChange =
          (10000 * hvisible) / (Name->Count - hvisible);
    }
    Sc_StatisticsY->SmallChange = Sc_StatisticsY->LargeChange / 10;
    float hinvisible = Name->Count - hvisible; // Invisible line count
    int hoffset = hinvisible * Sc_StatisticsY->Position * 0.0001 * lineheignt;
    // Scrollbar position
    if (hoffset < 0) {
      hoffset = 0;
    }
    int hend = hsize - hoffset; // Effective bar height
    if (hend > BMP->Height - topmargin - leftmargin) {
      hend = BMP->Height - topmargin - leftmargin;
    }
    if (hend < -leftmargin) {
      hend = -leftmargin;
    }
    int barwidth = BMP->Width - (textwidth + leftmargin * 5); // Bar width
    if (barwidth < 0) {
      barwidth = 0;
    }
    int shadowoffset = m_nFontHeight / 3; // Shadow offset

    // String drawing
    HRGN MyRgn, RgnBak;
    int rgnexist;
    MyRgn = ::CreateRectRgn(0, topmargin, BMP->Width, BMP->Height - leftmargin);
    RgnBak = ::CreateRectRgn(0, 0, 0, 0);
    rgnexist = ::GetClipRgn(C->Handle, RgnBak);
    if (rgnexist) {
      HRGN CmbRgn;
      CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
      ::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
      ::SelectClipRgn(C->Handle, CmbRgn);
      ::DeleteObject(CmbRgn);
    } else {
      ::SelectClipRgn(C->Handle, MyRgn);
    }
    B->Color = TColor(HalfColor(m_nFGColor, m_nBGColor, 0.95f));
    // Loop
    for (int i = 1; i < Name->Count; i += 2) {
      int y = i * m_nFontHeight * 2 + topmargin - hoffset;
      B->Style = bsSolid;
      C->FillRect(Rect(leftmargin - 2, y - m_nFontHeight / 4,
                       BMP->Width - leftmargin,
                       y + m_nFontHeight * 1.5 + m_nFontHeight / 4));
    }
    if (rgnexist) {
      ::SelectClipRgn(C->Handle, RgnBak);
    } else {
      ::SelectClipRgn(C->Handle, NULL);
    }
    ::DeleteObject(RgnBak);
    ::DeleteObject(MyRgn);

    // Background animation
    if (SettingView.m_bBGAnimation) {
      BGAnimation(C);
    }

    // Axis
    P->Color = TColor(m_nFGColor);
    P->Style = psSolid;
    P->Width = 3;
    C->MoveTo(BMP->Width - leftmargin, topmargin - leftmargin);
    C->LineTo(textwidth + leftmargin * 2, topmargin - leftmargin);
    C->LineTo(textwidth + leftmargin * 2, topmargin + hend);

    // Statistics key
    if (m_fStatisticsPos >= 1.0f) {
      P->Width = 1;
      P->Color = HalfColor(m_nFGColor, m_nBGColor, 0.5f);
      B->Style = bsClear;
      int order = 1;
      if (barwidth > 0) {
        while (barwidth / (max * 1.0 / order) < leftmargin * 8) {
          order *= 10;
        }
      } else {
        order = 100000;
      }
      F->Color = TColor(m_nFGColor);
      for (int i = 0; i <= max; i += order) {
        UnicodeString v = IntToStr(i);
        int x = leftmargin + textwidth + leftmargin * 2 + barwidth * i / max;
        C->TextOut(x - C->TextWidth(v) / 2, leftmargin, v);
        if (i > 0) {
          C->MoveTo(x, topmargin + 1);
          C->LineTo(x, topmargin + hend);
        }
      }
    }

    // Update bar graph setting
    // HRGN MyRgn, RgnBak;
    // int rgnexist;
    MyRgn = ::CreateRectRgn(0, topmargin, BMP->Width, BMP->Height - leftmargin);
    RgnBak = ::CreateRectRgn(0, 0, 0, 0);
    rgnexist = ::GetClipRgn(C->Handle, RgnBak);
    if (rgnexist) {
      HRGN CmbRgn;
      CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
      ::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
      ::SelectClipRgn(C->Handle, CmbRgn);
      ::DeleteObject(CmbRgn);
    } else {
      ::SelectClipRgn(C->Handle, MyRgn);
    }
    // Loop
    for (int i = 0; i < Name->Count; i++) {
      int y = i * m_nFontHeight * 2 + topmargin - hoffset;
      if (y < BMP->Height - leftmargin && y + m_nFontHeight * 2 >= topmargin) {
        // Click possible position setting
        TSRectToCard *SRTC = (TSRectToCard *)m_StatisticsRectToCard->Items[i];
        SRTC->m_Rect = Rect(0, y - m_nFontHeight / 4, BMP->Width,
                            y + m_nFontHeight * 1.5 + m_nFontHeight / 4);
        SRTC->m_Name = Name->Strings[i];

        // Update display
        B->Style = bsClear;
        F->Color = TColor(m_nFGColor);
        C->TextOut(leftmargin, y + m_nFontHeight / 4, Name->Strings[i]);

        int l = (int)(intptr_t)Value->Items[i];
        float v = *(float *)&l;
        int x = leftmargin + textwidth + leftmargin * 2 +
                barwidth * v / max * m_fStatisticsPos;

        // Shadow
        B->Style = bsSolid;
        B->Color =
            HalfColor(HalfColor(m_nFGColor, m_nBGColor, 0.5f), 0x0, 0.33f);
        C->FillRect(Rect(leftmargin + textwidth + leftmargin * 2 + shadowoffset,
                         y + shadowoffset, x + shadowoffset,
                         y + m_nFontHeight * 1.5 + shadowoffset));

        // Bar setting
        B->Style = bsSolid;
        B->Color = TColor((int)(intptr_t)Color->Items[i]);
        C->FillRect(Rect(leftmargin + textwidth + leftmargin * 2, y, x,
                         y + m_nFontHeight * 1.5));

        if (m_fStatisticsPos >= 1.0f) {
          // Label
          UnicodeString VS = FloatToStr(v);
          int vw = C->TextWidth(VS);
          int vx = x + leftmargin;
          if (vx + vw >= BMP->Width - leftmargin * 2) {
            vx = x - leftmargin - vw;
          }
          if (vx < leftmargin + textwidth + leftmargin * 3) {
            vx = leftmargin + textwidth + leftmargin * 3;
          }
          B->Style = bsClear;
          C->TextOut(vx, y + m_nFontHeight / 4, VS);
        }
      }
    }
    if (rgnexist) {
      ::SelectClipRgn(C->Handle, RgnBak);
    } else {
      ::SelectClipRgn(C->Handle, NULL);
    }
    ::DeleteObject(RgnBak);
    ::DeleteObject(MyRgn);

    delete Color;
    delete Value;
    delete Name;
  }

  // File/folder drawing
  PB_Statistics->Canvas->Draw(0, 0, BMP);

  // Discard BMP
  delete BMP;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sc_StatisticsYChange(TObject *Sender) {
  m_bRedrawRequested = true;
}

// ---------------------------------------------------------------------------
void TFo_Main::LinktoAllCardswithDesignatedLabel(TList *IDs) {
  // Get labelID from card at click; reflect to card label

  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);

    if (Card->m_nID != m_nTargetCard) {
      // Update label
      bool contain = false;
      for (int i2 = 0; i2 < Fo_Select->m_IDs->Count && !contain; i2++) {
        contain |= Card->m_Labels->Contain((int)(intptr_t)IDs->Items[i2]);
      }
      if (contain) {
        // Card has label

        // Check if link exists
        bool linked = false;
        for (int i2 = 0; i2 < m_Document->m_Links->Count && !linked; i2++) {
          TLink *Link = m_Document->GetLinkByIndex(i2);
          linked |= Link->m_nFromID == m_nTargetCard &&
                    Link->m_nDestID == Card->m_nID;
        }

        if (!linked) {
          // Create new link if none
          TLink *Link = m_Document->NewLink();
          Link->m_nFromID = m_nTargetCard;
          Link->m_nDestID = Card->m_nID;
        }
      }
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_LinktoAllCardswithDesignatedLabelClick(
    TObject *Sender) {
  Fo_Select = new TFo_Select(this);
  Fo_Select->m_Document = m_Document;
  Fo_Select->Caption = MLText.SelectLabelOfDestination;
  Fo_Select->m_nType = 1;
  Fo_Select->m_bMultiSelect = true;
  Fo_Select->ShowModal();
  if (Fo_Select->ModalResult == mrOk) {
    BackupSub(MI_LinktoAllCardswithDesignatedLabel->Caption);

    LinktoAllCardswithDesignatedLabel(Fo_Select->m_IDs);
  }
  Fo_Select->Release();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_NewCardLinkstoAllCardswithDesignatedLabelClick(
    TObject *Sender) {
  Fo_Select = new TFo_Select(this);
  Fo_Select->m_Document = m_Document;
  Fo_Select->Caption = MLText.SelectLabelOfDestination;
  Fo_Select->m_nType = 1;
  Fo_Select->m_bMultiSelect = true;
  Fo_Select->ShowModal();
  if (Fo_Select->ModalResult == mrOk) {
    BackupSub(MI_NewCardLinkstoAllCardswithDesignatedLabel->Caption);

    TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
    Card->m_bSelected = true;
    Card->m_Title = "";
    // cardTitle and label
    for (int i = 0; i < Fo_Select->m_IDs->Count; i++) {
      TCardLabel *Label =
          m_Document->GetLabelByIndex(0, (int)(intptr_t)Fo_Select->m_IDs->Items[i] - 1);
      if (i > 0) {
        Card->m_Title += ", ";
      }
      Card->m_Title += Label->m_Name;
    }
    m_nTargetCard = Card->m_nID;
    m_nTargetLink = -1;

    // All cards with same label
    float x = 0.0f, y = 0.0f;
    int count = 0;
    // Card loop
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);

      if (Card->m_nID != m_nTargetCard) {
        // Update label
        bool contain = false;
        for (int i2 = 0; i2 < Fo_Select->m_IDs->Count && !contain; i2++) {
          contain |= Card->m_Labels->Contain((int)(intptr_t)Fo_Select->m_IDs->Items[i2]);
        }
        if (contain) {
          x += Card->m_fX;
          y += Card->m_fY;
          count++;
        }
      }
    }
    if (count) {
      Card->m_fX = x / count;
      Card->m_fY = y / count;
    }

    LinktoAllCardswithDesignatedLabel(Fo_Select->m_IDs);
  }
  Fo_Select->Release();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_AddDesignatedLabeltoAllDestinationCardsClick(
    TObject *Sender) {
  Fo_Select = new TFo_Select(this);
  Fo_Select->m_Document = m_Document;
  Fo_Select->Caption = MLText.SelectLabel;
  Fo_Select->m_nType = 1;
  Fo_Select->m_bMultiSelect = true;
  Fo_Select->ShowModal();
  if (Fo_Select->ModalResult == mrOk) {
    BackupSub(MI_AddDesignatedLabeltoAllDestinationCards->Caption);

    // Link loop
    for (int i = 0; i < m_Document->m_Links->Count; i++) {
      TLink *Link = m_Document->GetLinkByIndex(i);
      if (Link->m_nFromID == m_nTargetCard) {
        TCard *Card = m_Document->GetCard(Link->m_nDestID);
        for (int i2 = 0; i2 < Fo_Select->m_IDs->Count; i2++) {
          Card->m_Labels->AddLabel((int)(intptr_t)Fo_Select->m_IDs->Items[i2]);
        }
      }
    }
  }
  Fo_Select->Release();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_StatisticsSortClick(TObject *Sender) {
  m_fStatisticsPos = 0.5f; // Statistics position default 0.5
  PB_StatisticsPaint(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_StatisticsMouseDown(TObject *Sender,
                                                 TMouseButton Button,
                                                 TShiftState Shift, int X,
                                                 int Y) {
  // Statistics click -> card display
  for (int i = 0; i < m_StatisticsRectToCard->Count; i++) {
    TSRectToCard *SRTC = (TSRectToCard *)m_StatisticsRectToCard->Items[i];
    if (SRTC->m_Rect.Left <= X && SRTC->m_Rect.Right > X &&
        SRTC->m_Rect.Top <= Y && SRTC->m_Rect.Bottom > Y) {
      La_SearchResultKeyword->Caption = SRTC->m_Name;
      m_GlobalSearchResult->Clear();
      for (int i2 = 0; i2 < SRTC->m_CardIDList->Count; i2++) {
        m_GlobalSearchResult->Add(SRTC->m_CardIDList->Items[i2]);
      }
      Sc_GlobalSearch->Tag = 1;
      Sc_GlobalSearch->Position = 0;
      Sc_GlobalSearch->Tag = 0;
      m_GlobalSearchCursorIndex = -1;
      La_GlobalSearchCount->Caption =
          IntToStr(m_GlobalSearchResult->Count) + UnicodeString(" Hits");
      if (m_GlobalSearchResult->Count) {
        ExitFullScreen();
        Fo_Search->Visible = false;
        Sc_GlobalSearch->Max = m_GlobalSearchResult->Count - 1;
        if (!Pa_GlobalSearch->Visible) {
          Pa_Client->Align = alNone;
          Pa_GlobalSearch->Visible = true;
          Sp_GlobalSearch->Visible = true;
          Pa_Client->Align = alClient;
        } else {
          PB_GlobalSearchPaint(Sender);
        }
        Sc_GlobalSearch->SetFocus();
      }
      break;
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_StatisticsKeyClick(TObject *Sender) {
  TPoint P;
  P.x = ((TControl *)Sender)->Left;
  P.y = ((TControl *)Sender)->Top;
  P = Pa_BrowserTop->ClientToScreen(P);
  PM_StatisticsKey->Popup(P.x, P.y);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PM_StatisticKeyClick(TObject *Sender) {
  Bu_StatisticsKey->Tag = ((TMenuItem *)Sender)->Tag;

  m_fStatisticsPos = 0.5f; // Statistics position default 0.5
  PB_StatisticsPaint(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_DrawingPaint(TObject *Sender) {
  // Background buffer creation
  Graphics::TBitmap *BMP = new Graphics::TBitmap();
  BMP->Width = PB_Drawing->Width;
  BMP->Height = PB_Drawing->Height;

  // Prepare and initialize
  TCanvas *C = BMP->Canvas;
  TBrush *B = C->Brush;
  TPen *P = C->Pen;
  TFont *F = C->Font;
  F->Name = RE_Edit->Font->Name;
  F->Charset = RE_Edit->Font->Charset;
  F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);
  m_nFontHeight = C->TextHeight(" ");

  // Background fill
  B->Color = TColor(m_nBGColor);
  B->Style = bsSolid;
  C->FillRect(Rect(0, 0, BMP->Width, BMP->Height));

  // Square drawing
  int sqrsize = BMP->Width; // Use smaller of Width and Height
  int leftspace = 0;
  int topspace = 0;
  if (sqrsize > BMP->Height) {
    sqrsize = BMP->Height;
    leftspace = (BMP->Width - sqrsize) / 2;
  } else {
    topspace = (BMP->Height - sqrsize) / 2;
  }
  float zoom = TB_DZoom->Position * 0.0001f;
  m_DrawingRect = Rect(leftspace + sqrsize / 2 - sqrsize * zoom * 0.5,
                       topspace + sqrsize / 2 - sqrsize * zoom * 0.5,
                       leftspace + sqrsize / 2 + sqrsize * zoom * 0.5,
                       topspace + sqrsize / 2 + sqrsize * zoom * 0.5);
  P->Width = 1;
  P->Style = psDot;
  P->Color = TColor(m_nFGColor);
  B->Style = bsClear;
  C->Rectangle(leftspace + sqrsize / 2 - sqrsize * zoom * 0.5,
               topspace + sqrsize / 2 - sqrsize * zoom * 0.375,
               leftspace + sqrsize / 2 + sqrsize * zoom * 0.5,
               topspace + sqrsize / 2 + sqrsize * zoom * 0.375);

  // Frame drawing
  P->Color = TColor(m_nFGColor);
  B->Color = TColor(m_nFGColor);
  P->Width = 3;
  m_Drawing->Draw(C, m_DrawingRect);

  // Card preview display
  /*
   DrawCard only considers browser drawing; m_CardX etc. are updated in
   PaintSub. Would need to rebuild per preview.

   int idx = m_Document->SearchCardIndex(m_nTargetCard);
   if (idx >= 0) {
   TCard *Card = m_Document->GetCardByIndex(idx);
   if (Card) {
   TColor HMColor = TColor(m_nBGColor);
   float SizeX = Card->m_nSize;
   if (m_Document->CountEnableLabel(Card)){
   // Label exists: use label color
   TColor c = GetCardColor(Card, SizeX);
   P->Color = HalfColor(c, m_nBGColor, 0.33f);
   B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
   }else{
   // No label
   P->Color = HalfColor(m_nFGColor, m_nBGColor, 0.5f);
   //B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor, 0.75f);
   B->Color = HalfColor(P->Color, m_nBGColor, 0.875f);
   }

   Card->m_Color = P->Color;
   DrawCard(C, Card, (int)SizeX, idx, HMColor, 0);
   }
   }
   */

  // Copy to Drawing
  PB_Drawing->Canvas->Draw(0, 0, BMP);

  // Discard BMP
  delete BMP;

  m_Drawing->m_bDrawRequest = false;
}

// ---------------------------------------------------------------------------
void TFo_Main::ApplyDrawing() {
  TCard *Card = m_Document->GetCard(m_nCurrentCard);
  if (Card && m_Drawing->m_bModified) {
    m_Drawing->m_bModified = false;
    BackupSub(MLText.EditDrawing);
    delete Card->m_Drawing;
    Card->m_Drawing = new TDrawing(*m_Drawing);
  }
}

void __fastcall TFo_Main::PB_DrawingMouseDown(TObject *Sender,
                                              TMouseButton Button,
                                              TShiftState Shift, int X, int Y) {
  TB_DZoom->SetFocus();
  if (Button == mbLeft) {
    m_Drawing->m_nPenColor = TColor(0x7fffffff);
    if (Sh_DColor->Pen->Width > 1) {
      m_Drawing->m_nPenColor = Sh_DColor->Brush->Color;
    }
    m_Drawing->SetTool(m_DrawingTool);
    m_Drawing->MouseDown((X - m_DrawingRect.left) * 1.0 /
                             (m_DrawingRect.right - m_DrawingRect.left),
                         (Y - m_DrawingRect.top) * 1.0 /
                             (m_DrawingRect.bottom - m_DrawingRect.top),
                         m_DrawingRect.right - m_DrawingRect.left, Shift);
    PB_DrawingPaint(Sender);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_DrawingMouseMove(TObject *Sender,
                                              TShiftState Shift, int X, int Y) {
  m_Drawing->MouseMove((X - m_DrawingRect.left) * 1.0 /
                           (m_DrawingRect.right - m_DrawingRect.left),
                       (Y - m_DrawingRect.top) * 1.0 /
                           (m_DrawingRect.bottom - m_DrawingRect.top));
  PB_DrawingPaint(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_DrawingMouseUp(TObject *Sender,
                                            TMouseButton Button,
                                            TShiftState Shift, int X, int Y) {
  if (Button == mbLeft) {
    m_Drawing->MouseUp((X - m_DrawingRect.left) * 1.0 /
                           (m_DrawingRect.right - m_DrawingRect.left),
                       (Y - m_DrawingRect.top) * 1.0 /
                           (m_DrawingRect.bottom - m_DrawingRect.top));
    PB_DrawingPaint(Sender);
    ApplyDrawing();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_DrawingToolClick(TObject *Sender) {
  m_DrawingTool = ((TSpeedButton *)Sender)->Tag;
  m_Drawing->ClearSelection();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_DColorClick(TObject *Sender) {
  CD_Color->Color = Sh_DColor->Brush->Color;
  if (CD_Color->Execute()) {
    Sh_DColor->Brush->Color = CD_Color->Color;
    Sh_DColorMouseDown(Sender, mbLeft, TShiftState(), 0, 0);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sh_DColorDefaultMouseDown(TObject *Sender,
                                                    TMouseButton Button,
                                                    TShiftState Shift, int X,
                                                    int Y) {
  Sh_DColorDefault->Pen->Width = 3;
  Sh_DColor->Pen->Width = 1;
  m_Drawing->ChangePenColor(TColor(0x7fffffff));
  PB_DrawingPaint(Sender);
  ApplyDrawing();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sh_DColorMouseDown(TObject *Sender,
                                             TMouseButton Button,
                                             TShiftState Shift, int X, int Y) {
  Sh_DColorDefault->Pen->Width = 1;
  Sh_DColor->Pen->Width = 3;
  m_Drawing->ChangePenColor(Sh_DColor->Brush->Color);
  PB_DrawingPaint(Sender);
  ApplyDrawing();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVC_DrawingClick(TObject *Sender) {
  SettingView.m_bDrawingVisible = !SettingView.m_bDrawingVisible;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_HierarchicalTextFileClick(TObject *Sender) {
  TCard *Card = m_Document->GetCard(m_nTargetCard);
  if (Card) {
    // Export card with default card name as Title
    SD_Txt->FileName = Card->m_Title;
    if (SD_Txt->Execute()) {
      TStringList *SL = new TStringList();
      ExportHierarchicalText(SL, 1, ".", Card);
      try {
        SL->SaveToFile(SD_Txt->FileName);
      } catch (...) {
        ShowMessage(MLText.FailedToSave + SD_Txt->FileName +
                    UnicodeString("."));
      }
      delete SL;
    }
  }
}

// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MS_ResetAllDatesClick(TObject *Sender) {
  // Create for write; all positions 0
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    m_Document->GetCardByIndex(i)->m_fCreated = 0.0;
    m_Document->GetCardByIndex(i)->m_fUpdated = 0.0;
    m_Document->GetCardByIndex(i)->m_fViewed = 0.0;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MS_OutputWordNgramClick(TObject *Sender) {
  if (!SD_CSV->Execute()) {
    return;
  }

  int doccount = m_Document->m_Cards->Count;

  // Decompose all text to N-gram
  TWideStringList *WS = new TWideStringList();
  for (int i = 0; i < doccount; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    WS->Add(Card->m_Lines->Text);
  }

  float fdummy;
  bool bdummy = false;
  const int n = 16; // up to 16-gram
  TTextDecomposer *TD =
      new TTextDecomposer(WS, n, 0, 0.0f, 0.0f, fdummy, bdummy, 0x1);

  // Output char N-gram to file
  FILE *F = fopen(AnsiString(SD_CSV->FileName).c_str(), "wt");
  fprintf(F, "String,Freq\n");

  int currentindex[n];
  for (int i = 0; i < TD->m_nMaxCombi; i++) {
    TD->m_Gram[i]->SortValue();
    currentindex[i] = TD->m_Gram[i]->Count - 1;
  }

  while (true) {
    // Check paper size
    int max = 0;
    int maxindex = 0;
    for (int in = n - 1; in >= 0; in--)
      if (currentindex[in] >= 0) {
        int v = TD->m_Gram[in]->Values(currentindex[in]);
        if (v > max) {
          max = v;
          maxindex = in;
        }
      }
    if (!max) {
      // Print complete
      break;
    }

    // Output to paper
    while (currentindex[maxindex] >= 0) {
      if (TD->m_Gram[maxindex]->Enabled(currentindex[maxindex])) {
        int v = TD->m_Gram[maxindex]->Values(currentindex[maxindex]);
        if (v == max) {
          AnsiString S = TD->m_Gram[maxindex]->Strings(currentindex[maxindex]);
          if (!(S.Pos("\"") || S.Pos("\n") || S.Pos("\r") || S.Pos("\t"))) {
            // Display left side
            fprintf(F, "%s,%d\n", S.c_str(), v);
          }
          currentindex[maxindex]--;
        } else {
          // Paper size check complete
          break;
        }
      } else {
        currentindex[maxindex]--;
      }
    }
  }

  fclose(F);

  delete TD;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MS_TextAnalysisTestClick(TObject *Sender) {
  PC_Client->ActivePage = TS_Editor;
  Bu_TestClick(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MS_ImportCSVClick(TObject *Sender) {
  if (!OD_CSV->Execute()) {
    return;
  }

  BackupSub(MS_ImportCSV->Caption);

  TStringList *SL = new TStringList();
  SL->LoadFromFile(OD_CSV->FileName);

  WideString S = SL->Strings[0];
  SplitStrBy(S, ",");

  TStringList *Tags = new TStringList();
  int FirstLabel = m_Document->m_Labels[0]->Count + 1;
  while (S != "") {
    WideString S2 = SplitStrBy(S, ",");
    Tags->Add(S2);
    TCardLabel *Label = m_Document->NewLabel(0);
    Label->m_Name = S2;
  }

  for (int i = 1; i < SL->Count; i++) {
    TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
    WideString S = SL->Strings[i];
    WideString S2;
    if (S.SubString(1, 1) == "\"") {
      S = S.SubString(2, S.Length());
      while (true) {
        int p1 = S.Pos("\"\"");
        int p2 = S.Pos("\",");
        if (p1 && (p1 < p2)) {
          S2 += S.SubString(1, p1);
          S = S.SubString(p1 + 2, S.Length());
        } else {
          break;
        }
      }
      S2 += SplitStrBy(S, "\",");
    } else {
      S2 = SplitStrBy(S, ",");
    }
    Card->m_Lines->Text = S2;

    for (int it = 0; it < Tags->Count; it++) {
      if (SplitStrBy(S, ",") == "1") {
        Card->m_Labels->AddLabel(FirstLabel + it);
      }
    }
  }

  delete Tags;
  delete SL;
}
// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormMouseWheel(TObject *Sender, TShiftState Shift,
                                         int WheelDelta, TPoint &MousePos,
                                         bool &Handled) {
  bool browserTarget = ShouldHandleBrowserWheel(MousePos);

  if (browserTarget && Shift.Contains(ssCtrl) && Shift.Contains(ssShift)) {
    ResizeFontWheel(WheelDelta);
    Handled = true;
  } else if (browserTarget && Shift.Contains(ssCtrl)) {
    ZoomBrowserWheel(WheelDelta);
    Handled = true;
  } else if (
      Pa_GlobalSearch->Visible &&
      (Sc_GlobalSearch->Focused() || IsMouseOnControl(PB_GlobalSearch, MousePos) ||
       IsMouseOnControl(Sc_GlobalSearch, MousePos))) {
    int step = Sc_GlobalSearch->SmallChange;
    if (step < 1) {
      step = 1;
    }

    int delta = (step * abs(WheelDelta) + 119) / 120;
    if (delta < 1) {
      delta = 1;
    }

    int pos = Sc_GlobalSearch->Position;
    if (WheelDelta > 0) {
      pos -= delta;
    } else {
      pos += delta;
    }

    if (pos < 0) {
      pos = 0;
    }
    if (pos > Sc_GlobalSearch->Max) {
      pos = Sc_GlobalSearch->Max;
    }

    Sc_GlobalSearch->Position = pos;
    Handled = true;
  } else if (browserTarget && Shift.Contains(ssShift)) {
    ScrollBrowserWheel(Sc_X, m_nBrowserWheelRemainderX, WheelDelta, false);
    Handled = true;
  } else if (browserTarget) {
    ScrollBrowserWheel(Sc_Y, m_nBrowserWheelRemainderY, WheelDelta, false);
    Handled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFo_Main::WMMouseHWheel(TMessage &msg) {
  TPoint mousePos((int)(short)LOWORD(msg.LParam),
                  (int)(short)HIWORD(msg.LParam));
  if (ShouldHandleBrowserWheel(mousePos)) {
    ScrollBrowserWheel(Sc_X, m_nBrowserWheelRemainderX,
                       GET_WHEEL_DELTA_WPARAM(msg.WParam), true);
    msg.Result = 1;
    return;
  }

  msg.Result = DefWindowProc(Handle, WM_MOUSEHWHEEL, msg.WParam, msg.LParam);
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::WMGesture(TMessage &msg) {
  GESTUREINFO gestureInfo;
  memset(&gestureInfo, 0, sizeof(gestureInfo));
  gestureInfo.cbSize = sizeof(gestureInfo);

  ::HGESTUREINFO gestureHandle =
      reinterpret_cast<::HGESTUREINFO>(msg.LParam);
  if (!::GetGestureInfo(gestureHandle, &gestureInfo)) {
    msg.Result = DefWindowProc(Handle, WM_GESTURE, msg.WParam, msg.LParam);
    return;
  }

  bool handled = false;
  if (gestureInfo.dwID == GID_ZOOM) {
    TPoint mousePos(gestureInfo.ptsLocation.x, gestureInfo.ptsLocation.y);
    if (ShouldHandleBrowserWheel(mousePos)) {
      if ((gestureInfo.dwFlags & GF_BEGIN) != 0) {
        m_uGestureZoomBeginDistance = gestureInfo.ullArguments;
        m_nGestureZoomBeginPos = TB_Zoom->Position;
        handled = true;
      } else if (m_uGestureZoomBeginDistance > 0 &&
                 gestureInfo.ullArguments > 0) {
        double scale =
            (double)gestureInfo.ullArguments / m_uGestureZoomBeginDistance;
        double zoomOffset = 2000.0 * log(scale) / log(2.0);
        int delta =
            zoomOffset >= 0.0 ? (int)(zoomOffset + 0.5) : (int)(zoomOffset - 0.5);
        int pos = m_nGestureZoomBeginPos + delta;
        TB_Zoom->Position = ClampValue(pos, TB_Zoom->Min, TB_Zoom->Max);
        handled = true;
      }

      if ((gestureInfo.dwFlags & GF_END) != 0) {
        m_uGestureZoomBeginDistance = 0;
      }
    }
  }

  if (handled) {
    ::CloseGestureInfoHandle(gestureHandle);
    msg.Result = 1;
    return;
  }

  msg.Result = DefWindowProc(Handle, WM_GESTURE, msg.WParam, msg.LParam);
}

//---------------------------------------------------------------------------
void __fastcall TFo_Main::ME_GPTClick(TObject *Sender) {
  int tag = ((TMenuItem *)Sender)->Tag;
  if (tag < 0) {
    // Show GPT Menu
    TPoint P;
    GetCursorPos(&P);
    PM_GPT->Popup(P.x, P.y);
  } else if (tag < Setting2Function.m_GPT->Count) {
    if (SettingFile.m_GPTAPIKey == "") {
      MS_GPTAPIKeyClick(Sender);
      if (SettingFile.m_GPTAPIKey == "") {
        return;
      }
    }

    UnicodeString menu_name = Setting2Function.m_GPT->Names[tag];
    UnicodeString command = Setting2Function.m_GPT->Strings[tag];
    int pos = command.Pos("=");
    command = command.SubString(pos + 1, command.Length() - pos);
    pos = command.Pos(",");
    UnicodeString prompt =
        command.SubString(pos + 1, command.Length() - pos); // Prompt
    command =
        command.SubString(1, pos - 1); // Insert, InsertAfter, Replace or Title

    if (prompt == "(Input)") {
      Fo_EditText = new TFo_EditText(this);
      Fo_EditText->La_Text->Caption = "Prompt:";
      Fo_EditText->Caption = menu_name;
      Fo_EditText->ShowModal();
      if (Fo_EditText->ModalResult == mrOk &&
          Fo_EditText->Ed_Text->Text != "") {
        prompt = Fo_EditText->Ed_Text->Text;
        if (RE_Edit->SelLength > 0) {
          prompt =
              RE_Edit->Text.SubString(RE_Edit->SelStart, RE_Edit->SelLength) +
              "\n---\n" + prompt;
        }
      } else {
        prompt = "";
      }
      Fo_EditText->Release();
    } else if (command == "Replace" or command == "Title") {
      prompt += "\n---\n" +
                RE_Edit->Text.SubString(RE_Edit->SelStart, RE_Edit->SelLength);
    } else if (command == "InsertAfter") {
      prompt += "\n---\n" + RE_Edit->Text.SubString(1, RE_Edit->SelStart);
    }

    if (prompt != "") {
      BackupSub(menu_name);

      // API key and endpoint setting
      String apiUrl = "https://api.openai.com/v1/chat/completions";

      // HTTP client creation
      TNetHTTPClient *httpClient = new TNetHTTPClient(NULL);
      TNetHTTPRequest *httpRequest = new TNetHTTPRequest(NULL);
      httpRequest->Client = httpClient;

      // Header setting
      httpClient->CustomHeaders["Content-Type"] = "application/json";
      httpClient->CustomHeaders["Authorization"] =
          "Bearer " + SettingFile.m_GPTAPIKey;

      // Request body creation
      TJSONObject *requestBodyObj = new TJSONObject();
      requestBodyObj->AddPair("model", Setting2Function.m_GPTModel);
      TJSONObject *jsonObj = new TJSONObject();
      jsonObj->AddPair("role", "user");
      jsonObj->AddPair("content", prompt);
      TJSONObject *jsonObj2 = new TJSONObject();
      jsonObj2->AddPair("role", "system");
      jsonObj2->AddPair("content", Setting2Function.m_GPTSystemPrompt);
      TJSONArray *jsonArray = new TJSONArray();
      jsonArray->Add(jsonObj2);
      jsonArray->Add(jsonObj);
      requestBodyObj->AddPair("messages", jsonArray);
      String requestBody = requestBodyObj->ToString();
      // ShowMessage(requestBody);
      delete requestBodyObj;

      UnicodeString result =
          "(An error occured. Please check that you are properly connected to "
          "the Internet and that the correct API Key is specified in the API "
          "Key of the Settings menu.)";

      // POST request send
      TMemoryStream *responseStream = new TMemoryStream();
      TBytes requestBodyBytes = TEncoding::UTF8->GetBytes(requestBody);
      TBytesStream *requestStream = new TBytesStream(requestBodyBytes);
      TCursor crbak = Screen->Cursor;
      Screen->Cursor = crHourGlass;
      try {
        httpRequest->Post(apiUrl, requestStream, responseStream);
      } catch (Exception &e) {
      }
      Screen->Cursor = crbak;
      responseStream->Position = 0;
      TStreamReader *reader =
          new TStreamReader(responseStream, TEncoding::UTF8);
      String response = reader->ReadToEnd();
      // ShowMessage(response);
      TJSONObject *jsonResponse =
          (TJSONObject *)TJSONObject::ParseJSONValue(response);
      if (jsonResponse) {
        try {
          TJSONArray *choices = (TJSONArray *)jsonResponse->GetValue("choices");
          if (choices && choices->Count > 0) {
            TJSONObject *choice = (TJSONObject *)choices->Items[0];
            result = NormalizeLineBreaks(
                ((TJSONObject *)choice->Get("message")->JsonValue)
                    ->GetValue("content")
                    ->Value());
          }
        } catch (Exception &e) {
        }
        delete jsonResponse;
      }

      delete reader;

      delete responseStream;
      delete httpRequest;
      delete httpClient;

      int selstart = RE_Edit->SelStart;
      int selend = RE_Edit->SelStart + RE_Edit->SelLength;
      RE_Edit->Tag = -1;
      if (command == "Insert" or command == "InsertAfter") {
        RE_Edit->Text = RE_Edit->Text.SubString(1, selend) + result +
                        RE_Edit->Text.SubString(
                            selend + 1, RE_Edit->Text.Length() - selend);
        RE_Edit->SelStart = selend;
        RE_Edit->SelLength = result.Length();
      } else if (command == "Replace") {
        RE_Edit->Text = RE_Edit->Text.SubString(1, selstart) + result +
                        RE_Edit->Text.SubString(
                            selend + 1, RE_Edit->Text.Length() - selend);
        RE_Edit->SelStart = selstart;
        RE_Edit->SelLength = result.Length();
      } else if (command == "Title") {
        if (Pa_Card->Visible) {
          Ed_Title->Text = result;
          Ed_Title->SelectAll();
        }
      }
      RE_Edit->Tag = 0;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TFo_Main::MS_GPTAPIKeyClick(TObject *Sender) {
  Fo_EditText = new TFo_EditText(this);
  Fo_EditText->La_Text->Caption = "API Key:";
  Fo_EditText->Caption = "GPT API Key";
  Fo_EditText->Ed_Text->Text = SettingFile.m_GPTAPIKey;
  Fo_EditText->ShowModal();
  if (Fo_EditText->ModalResult == mrOk && Fo_EditText->Ed_Text->Text != "") {
    SettingFile.m_GPTAPIKey = Fo_EditText->Ed_Text->Text;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFo_Main::MF_AutoSaveClick(TObject *Sender) {
  if (!m_Document || !MF_AutoSave) {
    return;
  }

  bool nextChecked = (m_Document->m_nAutoSave == 0);
  m_Document->m_nAutoSave = nextChecked ? 1 : 0; // Store as an explicit override.
  UpdateAutoSaveReloadMenuStates();
}

void __fastcall TFo_Main::MF_AutoReloadClick(TObject *Sender) {
  if (!m_Document || !MF_AutoReload) {
    return;
  }

  bool nextChecked = (m_Document->m_nAutoReload == 0);
  m_Document->m_nAutoReload = nextChecked ? 1 : 0; // Store as an explicit override.
  UpdateAutoSaveReloadMenuStates();
}

//---------------------------------------------------------------------------

void __fastcall TFo_Main::MS_AutoSaveDefaultClick(TObject *Sender) {
  if (!MS_AutoSaveDefault) {
    return;
  }

  SettingFile.m_bAutoSaveDefault = !SettingFile.m_bAutoSaveDefault;
  UpdateAutoSaveReloadMenuStates();
}

void __fastcall TFo_Main::MS_AutoReloadDefaultClick(TObject *Sender) {
  if (!MS_AutoReloadDefault) {
    return;
  }

  SettingFile.m_bAutoReloadDefault = !SettingFile.m_bAutoReloadDefault;
  UpdateAutoSaveReloadMenuStates();
}

//---------------------------------------------------------------------------

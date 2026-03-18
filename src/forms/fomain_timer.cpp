// ---------------------------------------------------------------------------
// Main form timer (Ti_CheckTimer) - split from fomain.cpp

#include <vcl.h>
#pragma hdrstop

#include <clipbrd.hpp>

#include "fomain.h"
#include "fomain_file.h"
#include "fomain_label.h"
#include "fomain_paint.h"
#include "fomain_search.h"
#include "fomain_animation.h"
#include "fomain_arrange.h"
#include "fomain_browser.h"
#include "fofullscreen.h"
#include "forandomanimation.h"
#include "fosearch.h"
#include "foshape.h"
#include "foview.h"
#include "setting.h"
#include "utils.h"
#include "foprogress.h"

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::Ti_CheckTimer(TObject *Sender) {
  unsigned int tgt = GetTickCount();

  // Continuous Load
  if (m_bContinuousLoad) {
    int newage = FileAge(m_Document->m_FN);
    if (newage != m_nCLFileAge) {
      LoadSub(m_Document->m_FN, true);
    }
  }

  // Plugin timeout
  if (SettingFile.fepTimeOut) {
    SettingFile.fepTimeOut(m_Document);
  }

  // DefaultView
  if (m_Document->m_nDefaultView >= 0) {
    switch (m_Document->m_nDefaultView) {
    case 0:
      PC_Client->ActivePage = TS_Browser;
      break;
    case 1:
      PC_Client->ActivePage = TS_Editor;
      break;
    }
    m_Document->m_nDefaultView = -1;
  }

  // Operation request
  if (fReqZoom >= -999.0f) {
    TB_Zoom->Position = (int)(fReqZoom * 2000);
    fReqZoom = -1000.0f;
  }
  if (fReqX >= -999.0f) {
    if (PC_Client->ActivePage != TS_Browser) {
      Sc_X->Position = (int)(fReqX * 10000);
    } else {
      m_fBrowserScrollRatio = 0.0f;
      m_nScrollTargetX = fReqX * 10000;
    }
    fReqX = -1000.0f;
  }
  if (fReqY >= -999.0f) {
    if (PC_Client->ActivePage != TS_Browser) {
      Sc_Y->Position = (int)(fReqY * 10000);
    } else {
      m_fBrowserScrollRatio = 0.0f;
      m_nScrollTargetY = fReqY * 10000;
    }
    fReqY = -1000.0f;
  }
  if (bReqArrange != -1) {
    SB_Arrange->Down = bReqArrange;
    bReqArrange = -1;
  }
  if (nReqArrangeMode != -1) {
    Bu_ArrangeType->Tag = nReqArrangeMode;
    nReqArrangeMode = -1;
  }
  if (bReqAutoScroll != -1) {
    SB_AutoScroll->Down = bReqAutoScroll;
    bReqAutoScroll = -1;
  }
  if (bReqAutoZoom != -1) {
    SB_AutoZoom->Down = bReqAutoZoom;
    bReqAutoZoom = -1;
  }
  if (bReqFullScreen != -1) {
    if (bReqFullScreen != Fo_FullScreen->Visible) {
      MV_FullScreenClick(Sender);
    }
    bReqFullScreen = -1;
  }
  if (bReqExit != -1) {
    Close();
    return;
  }
  if (nReqTargetCard != -1000) {
    m_nTargetCard = nReqTargetCard;
    nReqTargetCard = -1000;
  }
  if (bReqSizeLimitation != -1) {
    SettingView.m_bSizeLimitation = bReqSizeLimitation;
    bReqSizeLimitation = -1;
  }
  if (bReqLinkLimitation != -1) {
    SettingView.m_bLinkLimitation = bReqLinkLimitation;
    bReqLinkLimitation = -1;
  }
  if (bReqDateLimitation != -1) {
    SettingView.m_bDateLimitation = bReqDateLimitation;
    bReqDateLimitation = -1;
  }
  if (nReqSizeLimitation != -1) {
    SettingView.m_nSizeLimitation = nReqSizeLimitation;
    nReqSizeLimitation = -1;
  }
  if (nReqLinkLimitation != -1) {
    SettingView.m_nLinkLimitation = nReqLinkLimitation;
    nReqLinkLimitation = -1;
  }
  if (bReqLinkDirection != -1) {
    SettingView.m_bLinkDirection = bReqLinkDirection;
    bReqLinkDirection = -1;
  }
  if (bReqLinkBackward != -1) {
    SettingView.m_bLinkBackward = bReqLinkBackward;
    bReqLinkBackward = -1;
  }
  if (nReqLinkTarget != -2) {
    SettingView.m_nLinkTarget = nReqLinkTarget;
    nReqLinkTarget = -2;
  }
  if (nReqDateLimitation != -1) {
    SettingView.m_nDateLimitation = nReqDateLimitation;
    nReqDateLimitation = -1;
  }
  if (ReqDateLimitationDateType != -1) {
    SettingView.m_DateLimitationDateType = ReqDateLimitationDateType;
    ReqDateLimitationDateType = -1;
  }
  if (ReqDateLimitationType != -1) {
    SettingView.m_DateLimitationType = ReqDateLimitationType;
    ReqDateLimitationType = -1;
  }

  if (nReqKeyDown != -1) {
    unsigned short Key = (unsigned short)nReqKeyDown;
    FormKeyDown(Sender, Key, TShiftState());
    nReqKeyDown = -1;
  }

  // Taskbar display
  {
    UnicodeString S;
    if (m_Document->m_FN != "") {
      S = ExtractFileNameOnly(m_Document->m_FN);
    } else {
      S = AppTitle;
    }

    if (Application->Title != S) {
      Application->Title = S;
    }
  }

  // Caption
  {
    UnicodeString S;
    if (SettingView.m_nSpecialPaint) {
      S = SettingView.m_SpecialCaption;
    } else {
      S = UnicodeString(AppTitle) + " " + m_Document->m_FN;
      if (m_Document->m_bChanged) {
        S += " *";
      }
    }
    if (Caption != S) {
      Caption = S;
    }
  }

  // File list
  {
    TPoint pos, listpos;
    GetCursorPos(&pos);
    listpos.x = Pa_Files->Left;
    listpos.y = Pa_Files->Top;
    listpos = ClientToScreen(listpos);
    if (pos.x >= Left && pos.x < Left + Width && pos.y >= Top &&
        pos.y < Top + Height) {
      if ((pos.x < Left + 16 && pos.y >= listpos.y &&
           pos.y < listpos.y + Pa_List->Height) &&
          !Pa_Files->Visible && m_nAnimation == 0 && Application->Active &&
          !m_bMDownBrowser && SettingView.m_bFileList) {
        CloseEditBox();
        // Show file list without flicker
        PB_Browser->Tag = 1;

        Pa_Client->Visible = false;
        Sp_Left2->Visible = false;
        Pa_List->Visible = false;
        Pa_Client->Align = alNone;
        Sp_Left2->Align = alNone;
        Pa_List->Align = alNone;

        Pa_Files->Visible = true;
        Sp_Left->Left = Pa_Files->Width;
        Sp_Left->Visible = true;

        Ti_Check->Enabled = false;
        Application->ProcessMessages(); // Let components resize
        Ti_Check->Enabled = true;

        Pa_List->Left = Pa_Files->Width + Sp_Left->Width;
        Pa_List->Align = alLeft;
        Sp_Left2->Left = Pa_List->Left + Pa_List->Width;
        Sp_Left2->Align = alLeft;
        Pa_Client->Left = Sp_Left2->Left + Sp_Left2->Width;
        Pa_Client->Width = ClientWidth - Pa_Client->Left;
        Pa_Client->Align = alClient;
        Pa_List->Visible = true;
        Sp_Left2->Visible = true;
        Pa_Client->Visible = true;

        Ti_Check->Enabled = false;
        Application->ProcessMessages(); // Let components resize
        Ti_Check->Enabled = true;

        PB_Browser->Tag = 0;
      } else if ((pos.x >= Left + Pa_List->Left + 32 || pos.y < listpos.y ||
                  pos.y > listpos.y + Pa_Files->Height ||
                  !Application->Active) &&
                 Pa_Files->Visible) {
        // Hide file list without flicker
        PB_Browser->Tag = 1;

        Pa_Client->Visible = false;
        Sp_Left2->Visible = false;
        Pa_List->Visible = false;
        Pa_Client->Align = alNone;
        Sp_Left2->Align = alNone;
        Pa_List->Align = alNone;

        Pa_Files->Visible = false;
        Sp_Left->Visible = false;

        Pa_List->Left = 0;
        Pa_List->Align = alLeft;
        Sp_Left2->Left = Pa_List->Left + Pa_List->Width;
        Sp_Left2->Align = alLeft;
        Pa_Client->Left = Sp_Left2->Left + Sp_Left2->Width;
        Pa_Client->Width = ClientWidth - Pa_Client->Left;
        Pa_Client->Align = alClient;
        Pa_List->Visible = true;
        Sp_Left2->Visible = true;
        Pa_Client->Visible = true;

        Ti_Check->Enabled = false;
        Application->ProcessMessages(); // Let components resize
        Ti_Check->Enabled = true;

        PB_Browser->Tag = 0;
      }
    }

    TColor c = (TColor)SettingView.m_nBackgroundColor;
    if (m_Document->m_bChanged) {
      c = clBtnFace;
    }
    if (LB_FileList->Color != c) {
      LB_FileList->Color = c;
    }
  }

  // Card list
  if (Pa_List->Visible != SettingView.m_bCardList) {
    Pa_Client->Align = alNone;
    Pa_List->Visible = SettingView.m_bCardList;
    Sp_Left2->Left = Pa_List->Left + Pa_List->Width;
    Sp_Left2->Visible = SettingView.m_bCardList;
    Pa_Client->Align = alClient;
  }

  // Related text
  bool RelatedTextRefreshRequest = false; // Related text refresh request
  {
    if (SB_EditorRelated->Down != SettingView.m_bEditorRelatedVisible) {
      SB_EditorRelated->Down = SettingView.m_bEditorRelatedVisible;
    }
    if (SettingView.m_bEditorRelatedVisible != Pa_EditorRelated->Visible) {
      Pa_Editor->Align = alNone;
      Pa_EditorRelated->Visible = SettingView.m_bEditorRelatedVisible;
      Sp_EditorRelated->Visible = SettingView.m_bEditorRelatedVisible;
      Pa_Editor->Align = alClient;
      RelatedTextRefreshRequest = Pa_EditorRelated->Visible;
      // Update requested when made visible
    }
  }

  bool drawrequest = false; // Browser draw request (data update)
  bool drawrequest2 =
      m_bRedrawRequested; // Browser draw request (animation display)
  bool drawrequest_s = m_bRedrawRequested; // Statistics draw request
  m_bRedrawRequested = false;

  // Animation
  switch (m_nAnimation) {
  case 1:
    // RandomCard
    Animation_RandomCard();
    break;
  case 2:
    // RandomCard2
    Animation_RandomCard2();
    drawrequest2 = true;
    break;
  case 4:
    // RandomJump
    Animation_RandomJump();
    drawrequest2 = true;
    break;
  case 5:
    // RandomMap
    Animation_RandomMap();
    break;
  case 3:
    // Random Trace
    Animation_RandomTrace();
    break;
  }

  // Fullscreen during animation
  if (Fo_RandomAnimation->m_bFullScreenRequest) {
    Fo_RandomAnimation->m_bFullScreenRequest = false;
    MV_FullScreenClick(Sender);
  }

  // Zoom during animation
  if (Fo_RandomAnimation->Visible) {
    if (TB_Zoom->Position != Fo_RandomAnimation->TB_Zoom->Position) {
      if (Fo_RandomAnimation->m_bZoomChanged) {
        // User performed zoom operation
        PB_Browser->Tag = 1; // Drawing disabled
        TB_Zoom->Position = Fo_RandomAnimation->TB_Zoom->Position;
        PB_Browser->Tag = 0; // Drawing resumed
        drawrequest2 = true;
        SB_AutoZoom->Down = false;
      } else {
        // Zoom by Auto Zoom
        Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
      }
      Fo_RandomAnimation->m_bZoomChanged = false;
    }
  }

  // Card list

  // Process when current card not selected or selected card exists but no
  // TargetCard
  bool SelectionChanged = false;
  bool SelectedExist = true;
  if (LB_List->Count == m_Document->m_Cards->Count) {
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (!SettingView.m_bFocusCursorAnimation) {
        if (Card->m_bSelected) {
          Card->m_nSelected = 100;
        } else {
          Card->m_nSelected = 0;
        }
      }
      SelectionChanged |= Card->m_bSelected != LB_List->Selected[i];
      SelectedExist |= Card->m_bSelected;
    }
  }
  {
    bool b;
    TCard *Card = m_Document->GetCard(m_nTargetCard);

    if (Card) {
      b = !Card->m_bSelected;
    } else {
      b = SelectedExist;
    }
    if (b) {
      // No card selected
      // Move to most recently touched card among selected
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
  }

  SelectionChanged |=
      m_nTargetCard >= 0 &&
      m_Document->SearchCardIndex(m_nTargetCard) != LB_List->ItemIndex;
  SelectionChanged |= m_nCurrentCard != m_nTargetCard;
  bool ListChanged = LB_List->Count != m_Document->m_Cards->Count ||
                     m_nRefreshListCount != m_Document->m_nRefreshListCount;
  if (SelectionChanged || ListChanged || Fo_View->m_bRefreshRequest ||
      m_UndoRedo->m_bChanged) {
    m_UndoRedo->m_bChanged = false;
    RelatedTextRefreshRequest = true; // Related text refresh request

    // Some list-related edit occurred
    m_nRefreshListCount = m_Document->m_nRefreshListCount;
    m_fZoomSD = 0.21f;
    drawrequest = true;

    if (TreeMode()) {
      SetCardVisible(false);
      m_Document->RefreshCardLevel();
      SetCardVisible(true);
    }

    // Card list display update
    int lasttopindex = LB_List->TopIndex;
    LB_List->Items->BeginUpdate();
    if (ListChanged) {
      LB_List->Items->Clear();
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        TCard *Card = m_Document->GetCardByIndex(i);
        UnicodeString S;
        if (Card->m_bTop) {
          S = ">";
        } else {
          // S = "  ";
        }
        if (TreeMode()) {
          for (int i2 = 0; i2 < Card->m_nLevel; i2++) {
            S += "  ";
          }
        }
        LB_List->Items->Add(S + DecodeES(Card->m_Title, " "));
        if (LB_List->Selected[i] != Card->m_bSelected) {
          LB_List->Selected[i] = Card->m_bSelected;
        }
      }
    } else {
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        TCard *Card = m_Document->GetCardByIndex(i);
        if (LB_List->Selected[i] != Card->m_bSelected) {
          LB_List->Selected[i] = Card->m_bSelected;
        }
      }
    }
    int index = m_Document->SearchCardIndex(m_nTargetCard);
    if (LB_List->ItemIndex != index) {
      LB_List->ItemIndex = index;
    }

    // Reflect card body changes (needed for Undo)
    if (m_nCurrentCard >= 0 && m_nCurrentCard == m_nTargetCard) {
      TCard *Card = m_Document->GetCard(m_nCurrentCard);
      if (Card) {
        UnicodeString Text =
            Card->m_Lines->Text.SubString(1, Card->m_Lines->Text.Length() - 2);
        if (Text != RE_Edit->Text && Card->m_Lines->Text != RE_Edit->Text) {
          RE_Edit->Tag = 1;
          RE_Edit->Lines->BeginUpdate();
          int line = RE_Edit->Perform(EM_GETFIRSTVISIBLELINE, 0, (NativeInt)0);
          RE_Edit->Text = Text;
          if (m_nNextCardID == m_nCurrentCard) {
            int line2 =
                RE_Edit->Perform(EM_GETFIRSTVISIBLELINE, 0, (NativeInt)0);
            if (line2 > line) {
              while (line2-- > line) {
                RE_Edit->Perform(EM_SCROLL, SB_LINEUP, (NativeInt)0);
              }
            } else {
              while (line2++ < line) {
                RE_Edit->Perform(EM_SCROLL, SB_LINEDOWN, (NativeInt)0);
              }
            }
            RE_Edit->Lines->EndUpdate();

            RE_Edit->SelStart = m_nNextSelStart;
            RE_Edit->SelLength = 0;
            RE_Edit->SelLength = m_nNextSelLength;
          } else {
            RE_Edit->Lines->EndUpdate();
          }
          RE_Edit->Tag = 0;
        }
        if (Ed_Title->Text != Card->m_Title) {
          Ed_Title->Tag = 1;
          Ed_Title->Text = Card->m_Title;
          Ed_Title->Tag = 0;
        }
        UnicodeString TB = DecodeES(Card->m_Title, "\r\n");
        if (Ed_TitleB->Text != TB) {
          Ed_Title->Tag = 1;
          Ed_TitleB->Text = TB;
          Ed_Title->Tag = 0;
        }

        delete m_Drawing;
        m_Drawing = new TDrawing(*Card->m_Drawing);
      }
    }

    // Restore selection index
    LB_List->TopIndex = lasttopindex;
    if (LB_List->ItemIndex >= 0) {
      while (LB_List->ItemRect(LB_List->ItemIndex).top < 0) {
        LB_List->TopIndex = LB_List->TopIndex - 1;
      }
      while (LB_List->ItemRect(LB_List->ItemIndex).bottom > LB_List->Height) {
        LB_List->TopIndex = LB_List->TopIndex + 1;
      }
    }

    LB_List->Items->EndUpdate();
  }

  // Process when non-existent card selected (e.g. after Undo)
  if (m_nCurrentCard == m_nTargetCard) {
    TCard *Card = m_Document->GetCard(m_nCurrentCard);
    if (Card == NULL) {
      RelatedTextRefreshRequest = true; // Related text refresh request
      m_nTargetCard = -1;
    }
  }

  // Search
  TCard *Card = m_Document->GetCard(m_nTargetCard);
  if (m_nCurrentCard != m_nTargetCard) {
    RelatedTextRefreshRequest = true; // Related text refresh request
    TCard *LastCard = m_Document->GetCard(m_nCurrentCard);
    m_nLastTarget = -1;
    if (LastCard && Card) {
      if (LastCard->m_nSelected > 0 && !LastCard->m_bSelected &&
          Card->m_nSelected < 100) {
        m_nLastTarget = m_nCurrentCard;
      }
    }
    if (SettingView.m_bFocusCursorAnimation) {
      if (Card) {
        if (Card->m_nSelected < 100) {
          m_nFocusCursorPos = 0;
        }
      } else {
        m_nFocusCursorPos = 0;
      }
    } else {
      m_nFocusCursorPos = 100;
    }

    m_nCurrentCard = m_nTargetCard;
    m_Document->m_nCardID = m_nTargetCard;

    CloseEditBox();

    m_fBrowserScrollRatio = 0.0f;
    // SetCardVisible();
    drawrequest = true;

    int nDrawingTool = m_Drawing->m_nTool;

    if (m_nCurrentCard == -1) {
      Pa_Editor->Visible = false;
      Ed_Title->Tag = 1;
      Ed_Title->Text = "";
      Ed_Title->Tag = 0;
      La_Created->Caption = "-";
      La_Edited->Caption = "-";
      La_Viewed->Caption = "-";
      Ed_Title->Enabled = false;
      Ed_CardSize->Enabled = false;
      UD_CardSize->Enabled = false;
      Ed_CardSize->Text = "";
      RE_Edit->Tag = 1;
      RE_Edit->Text = "";
      RE_Edit->Tag = 0;
      RE_Edit->Enabled = false;

      // Clear drawing
      delete m_Drawing;
      m_Drawing = new TDrawing();

      PB_Drawing->Enabled = false;
      RefreshLabel();
    } else {
      Pa_Editor->Visible = true;
      RE_Edit->Enabled = true;
      Ed_Title->Enabled = true;
      Ed_CardSize->Enabled = true;
      UD_CardSize->Enabled = true;
      if (Card) {
        // Body
        RE_Edit->Tag = 1;
        // RE_Edit->Lines->Assign(Card->m_Lines);
        // RE_Edit->Text = Card->m_Lines->Text.SubString(1,
        // Card->m_Lines->Text.Length() - 2);
        RE_Edit->Text = Card->m_Lines->Text;
        Card->m_fViewed = Now();
        RE_Edit->SelStart = 0;
        RE_Edit->Tag = 0;
        // Other than body
        // Title
        if (Ed_Title->Text != Card->m_Title) {
          Ed_Title->Tag = 1;
          Ed_Title->Text = Card->m_Title;

          Ed_TitleB->Text = DecodeES(Card->m_Title);
          if (CountStr(Card->m_Title, "\\n")) {
            Ed_TitleB->WantReturns = true;
          } else {
            Ed_TitleB->WantReturns = false;
          }

          Ed_Title->Tag = 0;
        }
        // Label update
        RefreshLabel();

        // Drawing
        delete m_Drawing;
        m_Drawing = new TDrawing(*Card->m_Drawing);

        // Date
        La_Created->Caption = DateTimeToStr(Card->m_fCreated);
        La_Edited->Caption = DateTimeToStr(Card->m_fUpdated);
        La_Viewed->Caption = DateTimeToStr(Card->m_fViewed);
        m_nRefreshLinkCount = m_Document->m_nRefreshLinkCount;
        RefreshLinks();

        // Media player
        if (Card->m_VideoFN != "") {
          TCardVideo *Video = SearchVideo(Card->m_VideoFN);
          if (Video) {
            // Video->m_Video->SetPosition(Video->m_Video->m_fLoopStartPos);
          }
        }
      } else {
        delete m_Drawing;
        m_Drawing = new TDrawing();
      }

      m_Drawing->m_nTool = nDrawingTool;

      // Drawing
      PB_Drawing->Enabled = true;
    }
    RefreshLaStatus();
  }

  // Related text update
  if (RelatedTextRefreshRequest && Pa_EditorRelated->Visible) {
    TStringList *NewStr = new TStringList();
    if (m_nTargetCard >= 0) {
      TCard *Card = m_Document->GetCard(m_nTargetCard);
      TList *Str = new TList();   // Text list
      TList *Order = new TList(); // Display order list
      // Link loop
      for (int il = 0; il < m_Document->m_Links->Count; il++) {
        TLink *Link = m_Document->GetLinkByIndex(il);
        TCard *Card2 = NULL;
        if (Link->m_nFromID == Card->m_nID && Link->m_nDestID != Card->m_nID) {
          Card2 = m_Document->GetCard(Link->m_nDestID);
        } else if (Link->m_nDestID == Card->m_nID &&
                   Link->m_nFromID != Card->m_nID) {
          Card2 = m_Document->GetCard(Link->m_nFromID);
        }

        if (Card2)
          if (Card2->m_Lines->Count > 0) {
            if (Card2->m_bVisible) {
              Str->Add(new UnicodeString(
                  UnicodeString("Title : ") + DecodeES(Card2->m_Title, " ") +
                  UnicodeString("\r\n\r\n") + Card2->m_Lines->Text +
                  UnicodeString("\r\n\r\n")));
            } else {
              Str->Add(new UnicodeString(
                  UnicodeString("(Title : ") + DecodeES(Card2->m_Title, " ") +
                  UnicodeString(")\r\n\r\n") + Card2->m_Lines->Text +
                  UnicodeString("\r\n\r\n")));
            }
            int order = Card2->m_nViewedOrder;
            if (!Card2->m_bVisible) {
              order += m_Document->m_Cards->Count;
              // Push hidden card text to end
            }
            Order->Add((void *)(intptr_t)order);
          }
      }

      while (Str->Count) {
        int minorder = (int)(intptr_t)Order->Items[0];
        int minindex = 0;
        for (int io = 1; io < Order->Count; io++) {
          int o = (int)(intptr_t)Order->Items[io];
          if (minorder > o) {
            minorder = o;
            minindex = io;
          }
        }

        NewStr->Add(*(UnicodeString *)Str->Items[minindex]);
        delete (UnicodeString *)Str->Items[minindex];
        Str->Delete(minindex);
        Order->Delete(minindex);
      }

      delete Str;
      delete Order;
    }

    if (Me_EditorRelated->Lines->Text != NewStr->Text) {
      Me_EditorRelated->Lines->BeginUpdate();
      Me_EditorRelated->Lines->Text = NewStr->Text;
      Me_EditorRelated->Lines->EndUpdate();
    }
    delete NewStr;
  }

  // Keep selection visible when focus moves to search window
  if (RE_Edit->HideSelection != !Fo_Search->Visible) {
    {
      int selstartbak = RE_Edit->SelStart;
      int sellengthbak = RE_Edit->SelLength;
      RE_Edit->HideSelection = !Fo_Search->Visible;
      RE_Edit->SelStart = selstartbak;
      RE_Edit->SelLength = sellengthbak;
    }
    {
      int selstartbak = Ed_Title->SelStart;
      int sellengthbak = Ed_Title->SelLength;
      Ed_Title->HideSelection = !Fo_Search->Visible;
      Ed_Title->SelStart = selstartbak;
      Ed_Title->SelLength = sellengthbak;
    }
  }

  // Process when non-existent link selected (e.g. after Undo)
  if (m_nCurrentLink == m_nTargetLink) {
    TLink *Link = m_Document->GetLinkByIndex(m_nCurrentLink);
    if (Link == NULL) {
      m_nTargetLink = -1;
    }
  }

  // Link list
  if (m_nRefreshLinkCount != m_Document->m_nRefreshLinkCount) {
    m_nRefreshLinkCount = m_Document->m_nRefreshLinkCount;
    RefreshLinks();

    // Reflect link title change
    if (m_nCurrentLink >= 0 && m_nCurrentLink == m_nTargetLink) {
      TLink *Link = m_Document->GetLinkByIndex(m_nCurrentLink);
      if (Link) {
        if (Ed_LinkTitle->Text != Link->m_Name) {
          Ed_LinkTitle->Tag = 1;
          Ed_LinkTitle->Text = Link->m_Name;
          Ed_LinkTitle->Tag = 0;
        }
      }
    }

    drawrequest = true;
  }

  // Link
  TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
  if (m_nCurrentLink != m_nTargetLink) {
    m_nCurrentLink = m_nTargetLink;

    Ed_LinkTitle->Tag = 1;
    // SetCardVisible();

    if (Link) {
      // Link selected
      Pa_Card->Visible = false;
      Pa_Link->Visible = true;

      Ed_LinkTitle->Text = Link->m_Name;
      Ch_LinkDirection->Checked = Link->m_bDirection;
    } else {
      Ed_LinkTitle->Text = "";
      Ch_LinkDirection->Checked = false;

      // Link deselected
      Pa_Link->Visible = false;
      Pa_Card->Visible = true;
    }

    Ed_LinkTitle->Tag = 0;

    // Label update
    RefreshLabel();
    drawrequest = true;
  }

  // Card/link count
  {
    UnicodeString S = IntToStr(m_Document->m_Cards->Count) +
                      UnicodeString(" ") + MLText.Cards + UnicodeString(", ") +
                      IntToStr(m_Document->m_Links->Count) +
                      UnicodeString(" ") + MLText.Links;
    if (La_Count->Caption != S) {
      La_Count->Caption = S;
    }
  }

  // Size
  if (m_nTargetCard >= 0 && Card) {
    // Display size
    int size = (int)(log(Card->m_nSize / 100.0) * (8 / log(4)) + 100.5) -
               100; // 8 to 4
    if (size > 8) {
      size = 8;
    } else if (size < -8) {
      size = -8;
    }

    if (UD_CardSize->Position != size || Ed_CardSize->Text == "") {
      Ed_CardSize->Tag = 1;
      UD_CardSize->Position = size;
      Ed_CardSize->Text = IntToStr(UD_CardSize->Position);
      Ed_CardSize->Tag = 0;
    }
  }

  // Shape
  if (SB_Shape->Visible != (m_nTargetCard >= 0)) {
    SB_Shape->Visible = (m_nTargetCard >= 0);
  }

  if (Fo_Shape->m_bSelected) {
    BackupSub(ME_BatchConversion->Caption);
    switch (Fo_Shape->m_nMode) {
    case 0:
      // Card
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bSelected) {
          Card->m_nShape = Fo_Shape->m_nItemIndex;
        }
      }
      m_Document->RefreshList();
      break;
    case 1:
      // Link
      if (m_nTargetLink >= 0 && Link) {
        // Reflect selection
        Link->m_nShape = Fo_Shape->m_nItemIndex;
        m_Document->RefreshLink();
      }
      break;
    case 2:
      // All cards
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        m_Document->GetCardByIndex(i)->m_nShape = Fo_Shape->m_nItemIndex;
      }
      m_Document->RefreshList();
      break;
    case 3:
      // All links
      for (int i = 0; i < m_Document->m_Links->Count; i++) {
        // Reflect selection
        m_Document->GetLinkByIndex(i)->m_nShape = Fo_Shape->m_nItemIndex;
      }
      m_Document->RefreshLink();
      break;
    }
    Fo_Shape->m_bSelected = false;
  }
  if (m_nTargetCard >= 0 && Card) {
    // Reflect selection to canvas
    if (Card->m_nShape != SB_Shape->Tag) {
      SB_Shape->Tag = Card->m_nShape;
      if (Card->m_nShape < IL_Shape->Count) {
        SB_Shape->Glyph->Canvas->FillRect(Rect(0, 0, 31, 13));
        IL_Shape->GetBitmap(Card->m_nShape, SB_Shape->Glyph);
      }
    }
  }

  if (m_nTargetLink >= 0 && Link) {
    // Reflect selection to canvas
    if (Link->m_nShape != SB_Shape->Tag) {
      SB_Shape->Tag = Link->m_nShape;
      if (Link->m_nShape < IL_LinkShape->Count) {
        SB_LinkShape->Glyph->Canvas->FillRect(Rect(0, 0, 31, 13));
        IL_LinkShape->GetBitmap(Link->m_nShape, SB_LinkShape->Glyph);
      }
    }
  }

  // Fix
  if (SB_Fix->Visible != (m_nTargetCard >= 0)) {
    SB_Fix->Visible = (m_nTargetCard >= 0);
  }

  if (m_nTargetCard >= 0 && Card) {
    if (SB_Fix->Down != Card->m_bFixed) {
      SB_Fix->Down = Card->m_bFixed;
    }
  }

  // Top card
  if (SB_Top->Visible != (m_nTargetCard >= 0)) {
    SB_Top->Visible = (m_nTargetCard >= 0);
  }

  if (m_nTargetCard >= 0 && Card) {
    if (SB_Top->Down != Card->m_bTop) {
      SB_Top->Down = Card->m_bTop;
    }
  }

  // Refresh request
  if (Fo_View->m_bRefreshRequest) {
    Fo_View->m_bRefreshRequest = false;
    RefreshLabel();
    // SetCardVisible();
    drawrequest = true;
  }

  // Label update
  if (m_nRefreshLabelCount != m_Document->m_nRefreshLabelCount) {
    m_nRefreshLabelCount = m_Document->m_nRefreshLabelCount;
    FreeMILabels();

    for (int i = 0; i < m_Document->m_Labels[0]->Count; i++) {
      TMenuItem *MI = new TMenuItem(PL_NoLabel->Owner);
      TCardLabel *Label = m_Document->GetLabelByIndex(0, i);
      MI->Caption = Label->m_Name;
      MI->GroupIndex = PL_NoLabel->GroupIndex;
      MI->RadioItem = PL_NoLabel->RadioItem;
      MI->OnClick = PL_NoLabel->OnClick;
      MI->Tag = i + 1;
      MI_Labels->Add(MI);
      Po_Label->Items->Add(MI);
    }

    for (int i = 0; i < m_Document->m_Labels[1]->Count; i++) {
      TMenuItem *MI = new TMenuItem(PL_NoLinkLabel->Owner);
      TCardLabel *Label = m_Document->GetLabelByIndex(1, i);
      MI->Caption = Label->m_Name;
      MI->GroupIndex = PL_NoLinkLabel->GroupIndex;
      MI->RadioItem = PL_NoLinkLabel->RadioItem;
      MI->OnClick = PL_NoLinkLabel->OnClick;
      MI->Tag = i + 1;
      MI_LinkLabels->Add(MI);
      Po_LinkLabel->Items->Add(MI);
    }

    RefreshLabel();
    if (!SB_Arrange->Down) {
      PB_BrowserPaint(Sender);
      BrowserArrangeByFold();
    }
    // SetCardVisible();
    drawrequest = true;
  }

  if (drawrequest && PC_Client->ActivePage == TS_Browser) {
    // Draw on data update; keep draw data as before Arrange
    PB_BrowserPaint(Sender);
    RefreshLaStatus();
  }

  if (drawrequest && Pa_GlobalSearch->Visible) {
    PB_GlobalSearchPaint(Sender);
  }

  // Update Bu_ArrangeType caption
  {
    UnicodeString S;
    switch (Bu_ArrangeType->Tag) {
      // 0-99=Hard, 100-199=Soft, 200-299=Matrix
    case 0:
      S = PAT_Normalize->Caption;
      break;
    case 1:
      S = PAT_Repulsion->Caption;
      break;
    case 2:
      S = PAT_Link->Caption;
      break;
    case 3:
      S = PAT_Label->Caption;
      break;
    case 4:
      S = PAT_Index->Caption;
      break;
    case 102:
      S = PAT_SoftLink->Caption;
      break;
    case 103:
      S = PAT_SoftLabel->Caption;
      break;
    case 104:
      S = PAT_SoftIndex->Caption;
      break;
    case 200:
      S = PAT_Matrix->Caption;
      break;
      // case 202:S = "Link(Matrix)->Caption;break;
      // case 203:S = "Label(Matrix)->Caption;break;
    case 204:
      S = PAT_MatrixIndex->Caption;
      break;

      // 500-999=Similarity
    case 500:
      S = PAT_Similarity->Caption;
      break;
    case 600:
      S = PAT_SimilaritySoft->Caption;
      break;

      // 1000-1999=Tree
    case 1000:
      S = PAT_TreeRadial->Caption;
      break;
    }
    S = DeleteActionKey(S);
    if (Bu_ArrangeType->Caption != S) {
      Bu_ArrangeType->Caption = S;
    }
  }
  if (SB_ArrangeRefresh->Enabled !=
      (SB_Arrange->Down && Bu_ArrangeType->Tag >= 500 &&
       Bu_ArrangeType->Tag <= 999)) {
    SB_ArrangeRefresh->Enabled =
        (SB_Arrange->Down && Bu_ArrangeType->Tag >= 500 &&
         Bu_ArrangeType->Tag <= 999);
  }

  // Browser draw
  m_fTickerSpeed = (tgt - m_nLastTimeOut) * (0.1f / 120);
  // 120ms to 0.1 units (TickerSpeed)
  m_fBGAnimationSpeed = (tgt - m_nLastTimeOut) * 0.001f;
  if (PC_Client->ActivePage == TS_Browser &&
      (Application->Active ||
       SettingView.m_bRedrawInBackground) /* && !m_bMDownBrowser */) {
    bool b = true;
    if (SettingFile.fepOperation) {
      if (SettingFile.fepOperation(
              m_Document, UnicodeString("Browser_RedrawByTimer").c_str())) {
        b = false;
      }
    }
    if (b) {
      if (SB_Arrange->Down) {
        switch (Bu_ArrangeType->Tag) {
        case 0:
          // Normalize
          BrowserArrangeByNormalize();
          drawrequest2 = true;
          break;
        case 1:
          // Repulsion
          BrowserArrangeByRepulsion();
          drawrequest2 = true;
          break;
        case 2:
          // Link
          BrowserArrangeByLink();
          drawrequest2 = true;
          break;
        case 3:
          // Label
          BrowserArrangeByLabel();
          drawrequest2 = true;
          break;
        case 4:
          // Index
          BrowserArrangeByIndex();
          drawrequest2 = true;
          break;
        case 102:
          // Link(Soft)
          BrowserArrangeByLink(0.33f);
          drawrequest2 = true;
          break;
        case 103:
          // Label(Soft)
          BrowserArrangeByLabel(0.33f);
          drawrequest2 = true;
          break;
        case 104:
          // Index(Soft)
          BrowserArrangeByIndex(0.33f);
          drawrequest2 = true;
          break;
        case 200:
        case 202:
        case 203:
        case 204:
          // Matrix
          BrowserArrangeByMatrix(Bu_ArrangeType->Tag);
          drawrequest2 = true;
          break;
        case 500:
          // Similarity
          BrowserArrangeBySimilarity();
          drawrequest2 = true;
          break;
        case 600:
          // Similarity(Soft)
          BrowserArrangeBySimilarity(0.33f);
          drawrequest2 = true;
          break;
        case 1000:
          BrowserArrangeByTree(Bu_ArrangeType->Tag);
          drawrequest2 = true;
          break;
        }
        // BrowserArrangeByFold();
      }

      // Score, Ticker, redraw by background animation
      drawrequest2 |= SettingView.m_bScore || SettingView.m_bTickerVisible ||
                      SettingView.m_bBGAnimation;

      // Redraw by auto scroll
      if (!(m_bMDownBrowser > 0 && m_bMDownBrowser < 4) &&
          m_bMDownBrowser != 5) { // 4=AutoScroll when specifying display
                                  // position from overview
        if ((SB_AutoScroll->Down || SB_AutoZoom->Down ||
             m_nScrollTargetX > -65536) &&
            GetTickCount() >= m_uMDownBrowserLast + 500) {
          drawrequest2 |= BrowserAutoScroll();
        }
      }

      // Redraw by thumbnail, video update
      TCursor crbak = Screen->Cursor;
      bool imageupdated = UpdateImageList() || UpdateVideoList();
      drawrequest2 |= imageupdated || m_VideoList->Count > 0;

      if (imageupdated) {
        Screen->Cursor = crbak;
      }

      // Redraw by focus cursor
      if (SettingView.m_bFocusCursor) {
        if (SettingView.m_bFocusCursorAnimation) {
          int speed = (tgt - m_nLastTimeOut) / 5; // Animation over 0.5 sec;
          bool last = m_nFocusCursorPos % 100 < 50;
          if (m_nFocusCursorPos < 100) {
            m_nFocusCursorPos += speed;
            if (m_nFocusCursorPos > 100) {
              m_nFocusCursorPos = 100; // Prevent sudden blink
            }
          } else {
            // Blink (clear but distracting)
            m_nFocusCursorPos += (tgt - m_nLastTimeOut) / 10;
          }
          drawrequest2 |=
              m_nFocusCursorPos < 100 || (m_nFocusCursorPos % 100 < 50) != last;

          // Normal card selection
          for (int i = 0; i < m_Document->m_Cards->Count; i++) {
            TCard *Card = m_Document->GetCardByIndex(i);
            int sel = Card->m_bSelected;
            sel *= 100;
            if (sel != Card->m_nSelected) {
              if (Card->m_nSelected < sel) {
                Card->m_nSelected += speed;
                if (Card->m_nSelected > 100) {
                  Card->m_nSelected = 100;
                }
              } else {
                Card->m_nSelected -= speed;
                if (Card->m_nSelected < 0) {
                  Card->m_nSelected = 0;
                }
              }
              drawrequest2 = true;
            }
          }
        }
      }

      if (drawrequest2) {
        PB_BrowserPaint(Sender);
      }
    }
  }

  // Drawing
  if (PC_Client->ActivePage == TS_Drawing) {
    if (m_Drawing->m_bDrawRequest) {
      PB_DrawingPaint(Sender);
    }
  }

  // Search
  if (PC_Client->ActivePage == TS_Statistics) {
    // Statistics redraw
    if (m_fStatisticsPos < 1.0f) {
      m_fStatisticsPos = 1.03f - (1.03f - m_fStatisticsPos) *
                                     pow(0.5,
                                         (tgt - m_nLastTimeOut) /
                                             150.0); // 150ms halves toward 1.0
      if (m_fStatisticsPos > 1.0f) {
        m_fStatisticsPos = 1.0f;
      }
      drawrequest_s = true;
    }

    // Background animation redraw
    drawrequest_s |= SettingView.m_bBGAnimation;

    if (drawrequest_s) {
      PB_StatisticsPaint(Sender);
    }
  }

  // Ed_TitleB position
  if (PC_Client->ActivePage == TS_Browser) {
    if (m_nTargetCard >= 0) {
      if (Ed_TitleB->Visible) {
        SetEdTitleBPos();
      }
    }
    if (SettingView.m_nEditInBrowserPos == 0) {
      SettingView.m_bEditInBrowserAlways = false;
    }
    if (RE_Edit->Parent != Pa_Editor || SettingView.m_bEditInBrowserAlways) {
      SetTextEditPos();
    }
  }

  // Search
  if (bSearchRequest) {
    int srbak = bSearchRequest;
    bSearchRequest = 0;
    if (SearchKeyword != WideString("")) {
      // Save keyword history
      for (int i = 0; i < Fo_Search->Co_Keyword->Items->Count; i++) {
        if (SearchKeyword == Fo_Search->Co_Keyword->Items->Strings[i]) {
          Fo_Search->Co_Keyword->Items->Delete(i);
          break;
        }
      }
      while (Fo_Search->Co_Keyword->Items->Count >= 100) {
        Fo_Search->Co_Keyword->Items->Delete(
            Fo_Search->Co_Keyword->Items->Count - 1);
      }
      Fo_Search->Co_Keyword->Items->Insert(0, SearchKeyword);

      if (srbak & 0x40) {
        // Global search
        if (!(srbak & 0x100)) {
          La_SearchResultKeyword->Caption =
              UnicodeString("\"") + SearchKeyword + UnicodeString("\"");

          GlobalSearch(srbak);
          m_GlobalSearchOption = srbak;
          m_GlobalSearchKeyword = SearchKeyword;
          // Not replace

          // Display global search result
          Sc_GlobalSearch->Tag = 1;
          Sc_GlobalSearch->Position = 0;
          Sc_GlobalSearch->Tag = 0;
          m_GlobalSearchCursorIndex = -1;
          La_GlobalSearchCount->Caption =
              IntToStr(m_GlobalSearchResult->Count) + UnicodeString(" Hits");
          if (m_GlobalSearchResult->Count) {
            // Found
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
          } else {
            // Not found
            Sc_GlobalSearch->Max = 0;
            PB_GlobalSearchPaint(Sender);
            ShowMessage(UnicodeString("\"") + SearchKeyword +
                        UnicodeString("\"") + MLText.NotFound);
          }
        } else {
          // Replace
          int replaced = ReplaceAll(srbak);

          if (replaced) {
            // Replace succeeded
            Fo_Search->Visible = false;
          } else {
            // Not found
            ShowMessage(UnicodeString("\"") + SearchKeyword +
                        UnicodeString("\"") + MLText.NotFound);
          }
        }
      } else {
        Search(srbak);
        if (srbak & 0x8) {
          Fo_Search->SetFocus();
        } else if (srbak & 0x4) {
          Ed_FindCard->SetFocus();
        }
      }
    }
  }
  if (Ed_FindCard->Text != SearchKeyword) {
    Ed_FindCard->Text = SearchKeyword;
  }

  // Menu items

  if (Fo_FullScreen->Visible && PC_Client->ActivePage != TS_Browser) {
    ExitFullScreen();
  }

  if (m_nMDownTargetY < 16 && Fo_FullScreen->Visible) {
    if (Fo_FullScreen->Menu != MM_Menu) {
      Menu = NULL;
      Fo_FullScreen->Menu = MM_Menu;
      Pa_BrowserTop->Parent = Fo_FullScreen;
      Pa_BrowserTop->ParentColor = true;
      Pa_Card->Parent = Fo_FullScreen;
    }
  } else {
    if (Menu != MM_Menu) {
      Pa_BrowserTop->Parent = TS_Browser;
      Pa_BrowserTop->ParentColor = true;
      Pa_Card->Parent = Pa_Client;
      Fo_FullScreen->Menu = NULL;
      Menu = MM_Menu;
    }
  }

  // File
  if (MF_Save->Enabled != !m_Document->m_bReadOnly) {
    MF_Save->Enabled = !m_Document->m_bReadOnly;
    MF_SaveAs->Enabled = !m_Document->m_bReadOnly;
  }
  {
    bool CanExportH = Card != NULL && TreeMode();
    if (CanExportH) {
      CanExportH &= Card->m_bTop == true;
    }
    if (MFE_HierarchicalTextFile->Enabled != CanExportH) {
      MFE_HierarchicalTextFile->Enabled = CanExportH;
    }
  }

  // Undo
  {
    UnicodeString UndoName;
    bool CanUndo =
        (m_UndoRedo->GetCanUndo(UndoName) || Ed_FindCard->Focused()) &&
        (m_nAnimation == 0);
    if (ME_Undo->Enabled != CanUndo) {
      ME_Undo->Enabled = CanUndo;
      PE_Undo->Enabled = CanUndo;
      PD_Undo->Enabled = CanUndo;
      PBN_Undo->Enabled = CanUndo;
      PBC_Undo->Enabled = CanUndo;
      PBL_Undo->Enabled = CanUndo;
    }

    UnicodeString S = ME_Undo->Caption;
    int p = S.Pos(" - ");
    if (p) {
      S = S.SubString(1, p - 1);
    }
    if (UndoName != "") {
      S += UnicodeString(" - ") + UnicodeString(UndoName);
    }
    if (ME_Undo->Caption != S) {
      ME_Undo->Caption = S;
      PE_Undo->Caption = S;
      PD_Undo->Caption = S;
      PBN_Undo->Caption = S;
      PBC_Undo->Caption = S;
      PBL_Undo->Caption = S;
    }
  }

  // Redo
  {
    UnicodeString RedoName;
    bool CanRedo = m_UndoRedo->GetCanRedo(RedoName) && (m_nAnimation == 0);
    if (ME_Redo->Enabled != CanRedo) {
      ME_Redo->Enabled = CanRedo;
      PE_Redo->Enabled = CanRedo;
      PD_Redo->Enabled = CanRedo;
      PBN_Redo->Enabled = CanRedo;
      PBC_Redo->Enabled = CanRedo;
      PBL_Redo->Enabled = CanRedo;
    }
    UnicodeString S = ME_Redo->Caption;

    int p = S.Pos(" - ");
    if (p) {
      S = S.SubString(1, p - 1);
    }
    if (RedoName != "") {
      S += UnicodeString(" - ") + UnicodeString(RedoName);
    }
    if (ME_Redo->Caption != S) {
      ME_Redo->Caption = S;
      PE_Redo->Caption = S;
      PD_Redo->Caption = S;
      PBN_Redo->Caption = S;
      PBC_Redo->Caption = S;
      PBL_Redo->Caption = S;
    }
  }

  // Edit
  {
    // Cut (Enable if text is selected or card is selected)
    bool editenabled =
        (RE_Edit->Focused() && RE_Edit->SelLength > 0) ||
        (Ed_Title->Focused() && Ed_Title->SelLength > 0) ||
        (Ed_TitleB->Focused() && Ed_TitleB->SelLength > 0) ||
        (Ed_LinkTitle->Focused() && Ed_LinkTitle->SelLength > 0) ||
        (Ed_FindCard->Focused() && Ed_FindCard->SelLength > 0) ||
        (Ed_FindCard->Focused() && Ed_FindCard->SelLength > 0) ||
        (TB_DZoom->Focused() && m_Drawing->Selected() > 0) ||
        (!RE_Edit->Focused() && !Me_EditorRelated->Focused() &&
         !Ed_Title->Focused() && !Ed_TitleB->Focused() &&
         !Ed_LinkTitle->Focused() && !Ed_FindCard->Focused() &&
         !TB_DZoom->Focused() && m_nTargetCard >= 0);
    if (ME_Cut->Enabled != editenabled) {
      ME_Cut->Enabled = editenabled;
      PE_Cut->Enabled = editenabled;
      PD_Cut->Enabled = editenabled;
      PBC_Cut->Enabled = editenabled;
    }
  }
  {
    // Copy (Enable if text is selected or card is selected)
    bool editenabled =
        (RE_Edit->Focused() && RE_Edit->SelLength > 0) ||
        (Me_EditorRelated->Focused() && Me_EditorRelated->SelLength > 0) ||
        (Ed_Title->Focused() && Ed_Title->SelLength > 0) ||
        (Ed_TitleB->Focused() && Ed_TitleB->SelLength > 0) ||
        (Ed_LinkTitle->Focused() && Ed_LinkTitle->SelLength > 0) ||
        (Ed_FindCard->Focused() && Ed_FindCard->SelLength > 0) ||
        (TB_DZoom->Focused() && m_Drawing->Selected() > 0) ||
        (!RE_Edit->Focused() && !Me_EditorRelated->Focused() &&
         !Ed_Title->Focused() && !Ed_TitleB->Focused() &&
         !Ed_LinkTitle->Focused() && !Ed_FindCard->Focused() &&
         !TB_DZoom->Focused() && m_nTargetCard >= 0);
    if (ME_Copy->Enabled != editenabled) {
      ME_Copy->Enabled = editenabled;
      PE_Copy->Enabled = editenabled;
      PD_Copy->Enabled = editenabled;
      PBC_Copy->Enabled = editenabled;
    }
  }

  {
    // Delete (Enable if editing text or card is selected)
    bool editenabled = RE_Edit->Focused() || Ed_Title->Focused() ||
                       Ed_TitleB->Focused() || Ed_LinkTitle->Focused() ||
                       Ed_FindCard->Focused() ||
                       (TB_DZoom->Focused() && m_Drawing->Selected() > 0) ||
                       (!RE_Edit->Focused() && !Me_EditorRelated->Focused() &&
                        !Ed_Title->Focused() && !Ed_TitleB->Focused() &&
                        !Ed_LinkTitle->Focused() && !Ed_FindCard->Focused() &&
                        !TB_DZoom->Focused() && m_nTargetCard >= 0);
    if (ME_Delete->Enabled != editenabled) {
      ME_Delete->Enabled = editenabled;
      PE_Delete->Enabled = editenabled;
      PD_Delete->Enabled = editenabled;
    }
  }

  {
    // Paste
    bool editenabled =
        Clipboard()->HasFormat(CF_TEXT) &&
        (RE_Edit->Focused() || Ed_Title->Focused() || Ed_TitleB->Focused() ||
         Ed_LinkTitle->Focused() || Ed_FindCard->Focused() ||
         TB_DZoom->Focused() || PC_Client->ActivePage == TS_Browser);
    // Text paste possible
    editenabled &= !Me_EditorRelated->Focused(); // Related text not editable
    if (ME_Paste->Enabled != editenabled) {
      ME_Paste->Enabled = editenabled;
      PE_Paste->Enabled = editenabled;
      PD_Paste->Enabled = editenabled;
      PBN_Paste->Enabled = editenabled;
      PBC_Paste->Enabled = editenabled;
    }
  }
  {
    // Cut and create card
    bool editenabled = RE_Edit->SelLength > 0;
    if (PE_CutToNewCard->Enabled != editenabled) {
      PE_CutToNewCard->Enabled = editenabled;
      PE_CutToNewCardTitle->Enabled = editenabled;
      PE_CutToNewCardTitleWithLink->Enabled = editenabled;
    }
  }

  {
    bool gptenabled = RE_Edit->Focused();
    if (ME_GPT->Enabled != gptenabled) {
      ME_GPT->Enabled = ME_GPT->Enabled;
    }
  }

  {
    // WebSearch
    static WideString LastSelText;
    WideString SelText = Trim(GetSelText());
    bool websearchenabled = SelText.Length() > 0 && SelText.Pos("\n") == 0;
    if (SelText.Length() > 32) {
      SelText = SelText.SubString(1, 32) + "...";
    }
    if (ME_WebSearch->Enabled != websearchenabled || LastSelText != SelText) {
      ME_WebSearch->Enabled = websearchenabled;
      PBC_WebSearch->Enabled = websearchenabled;
      PBL_WebSearch->Enabled = websearchenabled;
      for (int i = 0; i < MI_WebSearch->Count; i++) {
        TMenuItem *MI = (TMenuItem *)MI_WebSearch->Items[i];
        if (websearchenabled) {
          MI->Caption = Setting2Function.m_WebSearch
                            ->Names[i % Setting2Function.m_WebSearch->Count] +
                        UnicodeString(" \"") + SelText + UnicodeString("\"");
        } else {
          MI->Caption = Setting2Function.m_WebSearch
                            ->Names[i % Setting2Function.m_WebSearch->Count];
        }
        MI->Enabled = websearchenabled;
      }
    }
    LastSelText = SelText;
  }

  // Insert
  if (MI_NewLink->Enabled != m_nTargetCard >= 0) {
    bool b = m_nTargetCard >= 0;
    MI_NewLink->Enabled = b;
    MI_NewLabel->Enabled = b;
    MI_NewExtLink->Enabled = b;
    MI_LinktoAllCardswithDesignatedLabel->Enabled = b;
    MI_AddDesignatedLabeltoAllDestinationCards->Enabled = b;
  }
  if (MI_NewLinkLabel->Enabled != m_nTargetLink >= 0) {
    MI_NewLinkLabel->Enabled = m_nTargetLink >= 0;
  }

  // View
  if (MV_FullScreen->Enabled != (PC_Client->ActivePage == TS_Browser)) {
    MV_FullScreen->Enabled = (PC_Client->ActivePage == TS_Browser);
  }

  // View->Card
  if (MVC_CardShadow->Checked != SettingView.m_bCardShadow) {
    MVC_CardShadow->Checked = SettingView.m_bCardShadow;
  }
  if (MVC_CardGradation->Checked != SettingView.m_bCardGradation) {
    MVC_CardGradation->Checked = SettingView.m_bCardGradation;
  }
  if (MVC_TickerVisible->Checked != SettingView.m_bTickerVisible) {
    MVC_TickerVisible->Checked = SettingView.m_bTickerVisible;
  }
  if (MVC_Image->Checked != SettingView.m_bImageVisible) {
    MVC_Image->Checked = SettingView.m_bImageVisible;
  }
  {
    TMenuItem *MI = NULL;
    switch (SettingView.m_nTickerLines) {
    case 1:
      MI = MVC_Ticker1Line;
      break;
    case 2:
      MI = MVC_Ticker2Lines;
      break;
    }
    if (MI) {
      if (!MI->Checked) {
        MI->Checked = true;
      }
    }
  }
  if (MVC_Video->Checked != SettingView.m_bVideoVisible) {
    MVC_Video->Checked = SettingView.m_bVideoVisible;
  }
  if (MVC_Drawing->Checked != SettingView.m_bDrawingVisible) {
    MVC_Drawing->Checked = SettingView.m_bDrawingVisible;
  }
  {
    TMenuItem *MI = NULL;
    switch (SettingView.m_nImageLimitation) {
    case 32:
      MI = MVI_32;
      break;
    case 40:
      MI = MVI_40;
      break;
    case 64:
      MI = MVI_64;
      break;
    case 80:
      MI = MVI_80;
      break;
    case 120:
      MI = MVI_120;
      break;
    case 160:
      MI = MVI_160;
      break;
    case 240:
      MI = MVI_240;
      break;
    case 320:
      MI = MVI_320;
      break;
    }
    if (MI) {
      if (!MI->Checked) {
        MI->Checked = true;
      }
    }
  }

  // View->Link
  if (MVL_Link->Checked != SettingView.m_bLinkVisible) {
    MVL_Link->Checked = SettingView.m_bLinkVisible;
  }
  if (MVL_LinkHemming->Checked != SettingView.m_bLinkHemming) {
    MVL_LinkHemming->Checked = SettingView.m_bLinkHemming;
  }
  if (MVL_LinkShadow->Checked != SettingView.m_bLinkShadow) {
    MVL_LinkShadow->Checked = SettingView.m_bLinkShadow;
  }
  if (MVL_LinkDirection->Checked != SettingView.m_bLinkDirectionVisible) {
    MVL_LinkDirection->Checked = SettingView.m_bLinkDirectionVisible;
  }
  if (MVL_LinkName->Checked != SettingView.m_bLinkNameVisible) {
    MVL_LinkName->Checked = SettingView.m_bLinkNameVisible;
  }

  // View->Label
  if (MVL_LabelCircle->Checked != SettingView.m_bLabelCircleVisible) {
    MVL_LabelCircle->Checked = SettingView.m_bLabelCircleVisible;
  }
  if (MVL_LabelRectangle->Checked != SettingView.m_bLabelRectangleVisible) {
    MVL_LabelRectangle->Checked = SettingView.m_bLabelRectangleVisible;
  }
  if (MVL_LabelName->Checked != SettingView.m_bLabelNameVisible) {
    MVL_LabelName->Checked = SettingView.m_bLabelNameVisible;
  }

  // View->Text
  if (MVT_Text->Checked != SettingView.m_bTextVisible) {
    MVT_Text->Checked = SettingView.m_bTextVisible;
  }
  if (MVT_Centering->Checked != SettingView.m_bTextCentering) {
    MVT_Centering->Checked = SettingView.m_bTextCentering;
  }
  if (MVT_WordWrap->Checked != SettingView.m_bTextWordWrap) {
    MVT_WordWrap->Checked = SettingView.m_bTextWordWrap;
    RE_Edit->WordWrap = SettingView.m_bTextWordWrap;
    if (SettingView.m_bTextWordWrap) {
      // Wrap at right edge
      RE_Edit->ScrollBars = ssVertical;
    } else {
      // No wrap
      RE_Edit->ScrollBars = ssBoth;
    }
    if (PC_Client->ActivePage == TS_Browser) {
      m_bRedrawRequested = true;
    }
  }
  if (MVT_EditInBrowser->Checked != SettingView.m_bEditInBrowser) {
    MVT_EditInBrowser->Checked = SettingView.m_bEditInBrowser;
  }
  if (MVT_AlwaysShowEditor->Checked != SettingView.m_bEditInBrowserAlways) {
    MVT_AlwaysShowEditor->Checked = SettingView.m_bEditInBrowserAlways;
  }
  {
    TMenuItem *MI = NULL;
    switch (SettingView.m_nEditInBrowserPos) {
    case 0:
      MI = MVT_UnderTheCard;
      break;
    case 1:
      MI = MVT_BrowserRight;
      break;
    case 2:
      MI = MVT_BrowserBottom;
      break;
    }
    if (MI) {
      if (!MI->Checked) {
        MI->Checked = true;
      }
    }
  }

  // Card popup
  if (PL_DeleteCard->Enabled != (m_nTargetCard >= 0)) {
    PL_DeleteCard->Enabled = (m_nTargetCard >= 0);
    PL_CardProperty->Enabled = (m_nTargetCard >= 0);
  }

  // View->Score
  if (MVS_Score->Checked != SettingView.m_bScore) {
    MVS_Score->Checked = SettingView.m_bScore;
  }
  if (MDS_Score->Enabled != SettingView.m_bScore) {
    MDS_Score->Enabled = SettingView.m_bScore;
    MDS_Score_I->Enabled = SettingView.m_bScore;
  }
  if (MVS_Authenticity->Checked != (SettingView.m_ScoreType == 0)) {
    MVS_Authenticity->Checked = (SettingView.m_ScoreType == 0);
  }
  if (MVS_StartingPoint->Checked != (SettingView.m_ScoreType == 1)) {
    MVS_StartingPoint->Checked = (SettingView.m_ScoreType == 1);
  }
  if (MVS_Destination->Checked != (SettingView.m_ScoreType == 2)) {
    MVS_Destination->Checked = (SettingView.m_ScoreType == 2);
  }
  if (MVS_Links_Out->Checked != (SettingView.m_ScoreType == 3)) {
    MVS_Links_Out->Checked = (SettingView.m_ScoreType == 3);
  }
  if (MVS_Links_In->Checked != (SettingView.m_ScoreType == 4)) {
    MVS_Links_In->Checked = (SettingView.m_ScoreType == 4);
  }
  if (MVS_Links_Total->Checked != (SettingView.m_ScoreType == 5)) {
    MVS_Links_Total->Checked = (SettingView.m_ScoreType == 5);
  }
  if (MVS_Links_InOut->Checked != (SettingView.m_ScoreType == 6)) {
    MVS_Links_InOut->Checked = (SettingView.m_ScoreType == 6);
  }
  if (MVS_TextLength->Checked != (SettingView.m_ScoreType == 7)) {
    MVS_TextLength->Checked = (SettingView.m_ScoreType == 7);
  }

  // View->Others
  if (MVO_CardList->Checked != SettingView.m_bCardList) {
    MVO_CardList->Checked = SettingView.m_bCardList;
  }
  if (MVO_FileList->Checked != SettingView.m_bFileList) {
    MVO_FileList->Checked = SettingView.m_bFileList;
  }

  if (MVO_Overview->Checked != SettingView.m_bOverview) {
    MVO_Overview->Checked = SettingView.m_bOverview;
  }

  // Popup menu
  TPopupMenu *PM = PM_BNoSelect;
  if (m_nTargetCard >= 0) {
    PM = PM_BCardSelect;
  }
  if (m_nTargetLink >= 0) {
    PM = PM_BLinkSelect;
  }
  if (PB_Browser->PopupMenu != PM) {
    PB_Browser->PopupMenu = PM;
  }
  if (Fo_FullScreen->PopupMenu != PM) {
    Fo_FullScreen->PopupMenu = PM;
  }

  if (PM == PM_BCardSelect) {
    bool b = m_Document->SearchParent(m_nTargetCard, false, false) >= 0;
    if (PBC_NewBrotherCard->Visible != b) {
      PBC_NewBrotherCard->Visible = b;
    }
  }

  if (PBN_Select->Checked != SB_Cursor->Down) {
    PBN_Select->Checked = SB_Cursor->Down;
    PBC_Select->Checked = SB_Cursor->Down;
    PBL_Select->Checked = SB_Cursor->Down;
  }
  if (PBN_Link->Checked != SB_Line->Down) {
    PBN_Link->Checked = SB_Line->Down;
    PBC_Link->Checked = SB_Line->Down;
    PBL_Link->Checked = SB_Line->Down;
  }

  // Display color
  if (LB_List->Font->Color != (TColor)SettingView.m_nFourgroundColor) {
    LB_List->Font->Color = (TColor)SettingView.m_nFourgroundColor;
    LB_FileList->Font->Color = (TColor)SettingView.m_nFourgroundColor;
    RE_Edit->Font->Color = (TColor)SettingView.m_nFourgroundColor;
    Me_EditorRelated->Font->Color = (TColor)SettingView.m_nFourgroundColor;
    LB_Link->Font->Color = (TColor)SettingView.m_nFourgroundColor;
  }
  if (LB_List->Color != (TColor)SettingView.m_nBackgroundColor) {
    LB_List->Color = (TColor)SettingView.m_nBackgroundColor;
    LB_FileList->Color = (TColor)SettingView.m_nBackgroundColor;
    RE_Edit->Color = (TColor)SettingView.m_nBackgroundColor;
    Me_EditorRelated->Color = (TColor)SettingView.m_nBackgroundColor;
    LB_Link->Color = (TColor)SettingView.m_nBackgroundColor;
    Color = (TColor)SettingView.m_nBackgroundColor;
    Pa_Files->Color = (TColor)SettingView.m_nBackgroundColor;
    Pa_List->Color = (TColor)SettingView.m_nBackgroundColor;
    Pa_Client->Color = (TColor)SettingView.m_nBackgroundColor;
    Pa_Browser->Color = (TColor)SettingView.m_nBackgroundColor;
    Pa_Editor->Color = (TColor)SettingView.m_nBackgroundColor;
    Pa_EditorRelated->Color = (TColor)SettingView.m_nBackgroundColor;
    Pa_EditorRelatedTop->Color = (TColor)SettingView.m_nBackgroundColor;
    // Pa_EditorTop->Color = (TColor)SettingView.m_nBackgroundColor;
    // Pa_BrowserTop->Color = (TColor)SettingView.m_nBackgroundColor;
    // Pa_DrawingTop->Color = (TColor)SettingView.m_nBackgroundColor;
    // Pa_StatisticsTop->Color = (TColor)SettingView.m_nBackgroundColor;
    bool focused = TB_Zoom->Focused();
    TB_Zoom->Visible = false;
    TB_Zoom->Visible = true;
    if (focused) {
      TB_Zoom->SetFocus();
    } // TB_Zoom loses focus when Background color changes
  }

  // Search
  if (MVO_WallPaper->Checked != (SettingView.m_WallPaper != "")) {
    MVO_WallPaper->Checked = (SettingView.m_WallPaper != "");
  }
  if (MVO_FixWallPaper->Checked != SettingView.m_bFixWallPaper) {
    MVO_FixWallPaper->Checked = SettingView.m_bFixWallPaper;
  }
  if (MVO_TileWallPaper->Checked != SettingView.m_bTileWallPaper) {
    MVO_TileWallPaper->Checked = SettingView.m_bTileWallPaper;
  }

  // Background animation
  if (MVO_BGAnimation->Checked != SettingView.m_bBGAnimation) {
    MVO_BGAnimation->Checked = SettingView.m_bBGAnimation;
  }
  {
    TMenuItem *MI;
    switch (SettingView.m_nBGAnimationType) {
    case 0:
      MI = MVOB_MovingLine;
      break;
    case 1:
      MI = MVOB_Bubble;
      break;
    case 2:
      MI = MVOB_Snow;
      break;
    case 3:
      MI = MVOB_CherryBlossom;
      break;
    }

    if (!MI->Checked) {
      MI->Checked = true;
    }
  }
  // CursorAnimation
  if (MVO_CursorAnimation->Checked != SettingView.m_bFocusCursorAnimation) {
    MVO_CursorAnimation->Checked = SettingView.m_bFocusCursorAnimation;
  }
  // Cursor animation
  if (MVO_NoScrollLag->Checked != SettingView.m_bNoScrollLag) {
    MVO_NoScrollLag->Checked = SettingView.m_bNoScrollLag;
  }

  // Anti-alias
  if (MVO_AntiAliasing->Checked != SettingView.m_bAntiAlias) {
    MVO_AntiAliasing->Checked = SettingView.m_bAntiAlias;
  }
  if (MVO_AA2x->Checked != (SettingView.m_nAntiAliasX == 2)) {
    MVO_AA2x->Checked = (SettingView.m_nAntiAliasX == 2);
  }
  if (MVO_AA4x->Checked != (SettingView.m_nAntiAliasX == 4)) {
    MVO_AA4x->Checked = (SettingView.m_nAntiAliasX == 4);
  }

  // StatusBar
  if (MV_StatusBar->Checked != SettingView.m_bStatusBar) {
    MV_StatusBar->Checked = SettingView.m_bStatusBar;
  }
  if (SB_Status->Visible != SettingView.m_bStatusBar) {
    SB_Status->Visible = SettingView.m_bStatusBar;
  }

  // Statistics key
  {
    UnicodeString S;

    int level1 = Bu_StatisticsKey->Tag / 10000;
    int level2 = (Bu_StatisticsKey->Tag / 100) % 100;
    switch (level1) {
    case 0: // Label
      S = PMSK_Label->Caption;
      break;
    case 1: // Number of Link
      S = PMSK_NumberOfLink->Caption;
      break;
    case 2: // Created
      S = PMSK_CreatedDate->Caption;
      break;
    case 3: // Edited
      S = PMSK_EditedDate->Caption;
      break;
    case 4: // Viewed
      S = PMSK_ViewedDate->Caption;
      break;
    }
    S = DeleteActionKey(S);
    switch (level1) {
    case 1:
      S += " ";
      switch (level2) {
      case 0: // Total
        S += PMSKL_Total->Caption;
        break;
      case 1: // Source
        S += PMSKL_Source->Caption;
        break;
      case 2: // Dest.
        S += PMSKL_Destination->Caption;
        break;
      }
      break;
    case 2:
    case 3:
    case 4:
      S += " ";
      switch (level2) {
      case 0: // Year
        S += PMSKC_Year->Caption;
        break;
      case 1: // Month
        S += PMSKC_Month->Caption;
        break;
      case 2: // Day
        S += PMSKC_Day->Caption;
        break;
      case 3: // Week
        S += PMSKC_Week->Caption;
        break;
      case 4: // Hour
        S += PMSKC_Hour->Caption;
        break;
      }
      break;
    }
    S = DeleteActionKey(S);
    Bu_StatisticsKey->Caption = S;
  }

  // Text edit box
  if (m_bTextEditRequested) {
    if (PC_Client->ActivePage == TS_Browser && m_nTargetCard >= 0) {
      SetTextEditPos();
      RE_Edit->SetFocus();
    }
    m_bTextEditRequested = false;
    m_bTitleEditRequested = false;
  } else if (m_bTitleEditRequested) {
    if (PC_Client->ActivePage == TS_Browser && m_nTargetCard >= 0) {
      SetEdTitleBPos();
      Ed_TitleB->Visible = true;
      Ed_TitleB->SetFocus();
    }
    m_bTitleEditRequested = false;
  }

  // CheckCount increment
  m_Document->m_nCheckCount++;

  m_nLastTimeOut = tgt;
  m_fTickerSpeed = 0.0;
  m_fBGAnimationSpeed = 0.0;
}

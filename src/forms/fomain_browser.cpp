// ---------------------------------------------------------------------------
// Browser card operations (split from fomain.cpp)

#include <vcl.h>
#pragma hdrstop

#include <math.h>

#include "fomain.h"
#include "fomain_browser.h"
#include "fomain_paint.h"
#include "fofullscreen.h"
#include "setting.h"

#pragma package(smart_init)

#define OVERVIEWSIZE 0.75f

static double DistanceToLine(int X, int Y, double X1, double Y1, double X2,
                             double Y2) {
  double K1, K2;
  if (X2 != X1) {
    K1 = (Y2 - Y1) / (X2 - X1);
  } else {
    K1 = 100000000000.0;
  }
  if (Y2 != Y1) {
    K2 = (X2 - X1) / (Y2 - Y1);
  } else {
    K2 = 100000000000.0;
  }
  double LX = (K1 * X1 + K2 * X - Y1 + Y) / (K1 + K2);
  double LY = K1 * ((K1 * X1 + K2 * X - Y1 + Y) / (K1 + K2) - X1) + Y1;

  double S = 0.5;
  if (fabs(X1 - X2) > 0.0 || fabs(Y1 - Y2) > 0.0) {
    if (fabs(X1 - X2) > fabs(Y1 - Y2)) {
      S = (LX - X1) / (X2 - X1);
    } else {
      S = (LY - Y1) / (Y2 - Y1);
    }
  }

  if (S < 0.0) {
    S = 0.0;
  }
  if (S > 1.0) {
    S = 1.0;
  }

  LX = X1 * (1 - S) + X2 * S;
  LY = Y1 * (1 - S) + Y2 * S;

  S = sqrt((LX - X) * (LX - X) + (LY - Y) * (LY - Y));

  return S;
}


void TFo_Main::BrowserNewCard() {
  BackupSub(MI_NewCard->Caption);

  // New card
  m_Document->ClearCardSelection();
  TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
  Card->m_bSelected = true;
  float cx, cy;
  DispPosToCardPos(m_nMDownTargetX, m_nMDownTargetY, cx, cy);
  Card->m_fX = cx;
  Card->m_fY = cy;

  if (SB_ToolLabel->Down && m_nToolLabel > 0) {
    // Label exists
    Card->m_Labels->AddLabel(m_nToolLabel);
  }

  m_nTargetCard = Card->m_nID;
  m_nTargetLink = -1;
  m_bDblClicked = true;

  // Edit box move
  Ti_CheckTimer(this);
  /*
   if (Fo_FullScreen->Visible){
   Ed_TitleB->Parent = Fo_FullScreen;
   }else{
   Ed_TitleB->Parent = TS_Browser;
   }
   */
  SetEdTitleBPos();

  Ed_TitleB->Visible = true;
  Ed_TitleB->SetFocus();
  // TB_Zoom->SetFocus();
}

// ---------------------------------------------------------------------------
void TFo_Main::CardPosToDispPos(float cx, float cy, float &dx, float &dy) {
  float Zoom = pow(2, TB_Zoom->Position / 2000.0f);
  float addx =
      (Zoom * (0.05f - Sc_X->Position * 0.0001f) + 0.5f) * m_nBrowserWidth;
  float addy =
      (Zoom * (0.05f - Sc_Y->Position * 0.0001f) + 0.5f) * m_nBrowserHeight;
  float mpyx = Zoom * 0.9f * m_nBrowserWidth;
  float mpyy = Zoom * 0.9f * m_nBrowserHeight;
  dx = mpyx * cx + addx;
  dy = mpyy * cy + addy;
}

// ---------------------------------------------------------------------------
void TFo_Main::DispPosToCardPos(float dx, float dy, float &cx, float &cy) {
  // Card drag
  cx = dx;
  cy = dy;

  // Client to 0-1
  cx /= m_nBrowserWidth;
  cy /= m_nBrowserHeight;

  // DeZoom
  float Zoom = pow(2, TB_Zoom->Position / 2000.0f);
  cx = (cx - 0.5f) / Zoom + 0.5f;
  cy = (cy - 0.5f) / Zoom + 0.5f;

  // DeAddress
  cx += (Sc_X->Position - 5000.0f) / 10000;
  cy += (Sc_Y->Position - 5000.0f) / 10000;

  // DePutiZoom
  cx = (cx - 0.05f) / 0.9f;
  cy = (cy - 0.05f) / 0.9f;
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserNewChildCard(bool binverse) {
  BackupSub(PBC_NewChildCard->Caption);

  // New card create from card or link
  int lastcard = m_nTargetCard;
  int lastindex = m_Document->SearchCardIndex(lastcard);
  int insindex = lastindex + 1;
  TCard *LastCard = m_Document->GetCardByIndex(lastindex);

  if (!binverse) {
    // Insert card at last linked card position
    for (int il = 0; il < m_Document->m_Links->Count; il++) {
      TLink *Link = m_Document->GetLinkByIndex(il);
      if (Link->m_nFromID == lastcard) {
        int index = m_Document->SearchCardIndex(Link->m_nDestID) + 1;
        if (index > insindex) {
          insindex = index;
        }
      }
    }
  } else {
    // Default link label

    // Insert card between linked cards
    for (int il = 0; il < m_Document->m_Links->Count; il++) {
      TLink *Link = m_Document->GetLinkByIndex(il);
      if (Link->m_nDestID == lastcard) {
        int index = m_Document->SearchCardIndex(Link->m_nFromID) + 1;
        if (index > insindex) {
          insindex = index;
        }
      }
    }
  }

  m_Document->ClearCardSelection();
  TCard *NewCard = m_Document->NewCard(insindex);
  NewCard->m_bSelected = true;
  if (SB_Arrange->Down) {
    // Insert at same position as parent when auto-arranging
    NewCard->m_fX = LastCard->m_fX;
    NewCard->m_fY = LastCard->m_fY;
  } else {
    // When not auto-arranging, insert near source card
    // Get average length of linked cards
    int count = 0;
    float r = 0.0f;
    for (int i = 0; i < m_Document->m_Links->Count; i++) {
      TLink *Link = m_Document->GetLinkByIndex(i);
      if (Link->m_nDestID == lastcard || Link->m_nFromID == lastcard) {
        TCard *Card;
        if (Link->m_nDestID == lastcard) {
          Card = m_Document->GetCard(Link->m_nFromID);
        } else {
          Card = m_Document->GetCard(Link->m_nDestID);
        }
        r +=
            sqrt((Card->m_fX - LastCard->m_fX) * (Card->m_fX - LastCard->m_fX) +
                 (Card->m_fY - LastCard->m_fY) * (Card->m_fY - LastCard->m_fY));
        count++;
      }
    }
    if (count) {
      r /= count;
    }
    // 64 dots on display
    float cx1, cx2, dummy;
    DispPosToCardPos(0, 0, cx1, dummy);
    DispPosToCardPos(64, 0, cx2, dummy);
    float r2 = fabs(cx2 - cx1);
    if (r < r2) {
      r = r2;
    } else if (r > r2 * 3) {
      r = r2 * 3;
    }
    float rbak = r; // Min distance for card insertion
    int itarcount = 0;
    while (itarcount++ < 100) {
      float t = (rand() % 10001) * 0.0002f * 3.1415926f;
      NewCard->m_fX = LastCard->m_fX + sin(t) * r;
      NewCard->m_fY = LastCard->m_fY - cos(t) * r;
      if (NewCard->m_fX < -1.0f) {
        NewCard->m_fX = -1.0f;
      }
      if (NewCard->m_fY < -1.0f) {
        NewCard->m_fY = -1.0f;
      }
      if (NewCard->m_fX > 2.0f) {
        NewCard->m_fX = 2.0f;
      }
      if (NewCard->m_fY > 2.0f) {
        NewCard->m_fY = 2.0f;
      }
      bool found = false;
      for (int i = 0; i < m_Document->m_Cards->Count && !found; i++) {
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card != NewCard &&
            (Card->m_fX - NewCard->m_fX) * (Card->m_fX - NewCard->m_fX) +
                    (Card->m_fY - NewCard->m_fY) *
                        (Card->m_fY - NewCard->m_fY) <
                (rbak * rbak / 4.0f)) {
          found = true;
        }
      }
      if (!found) {
        break;
      } else {
        // r *= 1.1f;
      }
    }
  }
  for (int il = 0; il < LastCard->m_Labels->Count; il++) {
    NewCard->m_Labels->AddLabel(LastCard->m_Labels->GetLabel(il));
  }
  NewCard->m_nShape = LastCard->m_nShape;
  m_nTargetCard = NewCard->m_nID;
  m_nTargetLink = -1;
  TLink *Link = m_Document->NewLink();
  if (!binverse) {
    // Child node
    Link->m_nFromID = lastcard;
    Link->m_nDestID = m_nTargetCard;
  } else {
    // Default link label
    Link->m_nFromID = m_nTargetCard;
    Link->m_nDestID = lastcard;
  }

  Ti_CheckTimer(this);
  CloseEditBox();
  CloseTextEditBox();
  if (PC_Client->ActivePage == TS_Browser) {
    SetEdTitleBPos();
    Ed_TitleB->Visible = true;
    Ed_TitleB->SetFocus();
  } else {
    Ed_Title->SelectAll();
    Ed_Title->SetFocus();
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserNewBrotherCard() {
  int ParentID = m_Document->SearchParent(m_nTargetCard, false, false);

  if (ParentID >= 0) {
    BackupSub(PBC_NewBrotherCard->Caption);

    // Child card creation
    // New card create from child or link
    int lastindex = m_Document->SearchCardIndex(m_nTargetCard);
    int insindex = lastindex + 1;
    TCard *LastCard = m_Document->GetCardByIndex(lastindex);

    m_Document->ClearCardSelection();
    TCard *NewCard = m_Document->NewCard(insindex);
    NewCard->m_bSelected = true;
    m_nTargetCard = NewCard->m_nID;
    if (SB_Arrange->Down) {
      NewCard->m_fX = LastCard->m_fX;
      NewCard->m_fY = LastCard->m_fY;
    } else {
      // When not auto-arranging, insert near parent card
      TCard *ParentCard = m_Document->GetCard(ParentID);
      // Get average length of linked cards
      int count = 0;
      float r = 0.0f;
      for (int i = 0; i < m_Document->m_Links->Count; i++) {
        TLink *Link = m_Document->GetLinkByIndex(i);
        if (Link->m_nDestID == ParentID || Link->m_nFromID == ParentID) {
          TCard *Card;
          if (Link->m_nDestID == ParentID) {
            Card = m_Document->GetCard(Link->m_nFromID);
          } else {
            Card = m_Document->GetCard(Link->m_nDestID);
          }
          r += sqrt((Card->m_fX - ParentCard->m_fX) *
                        (Card->m_fX - ParentCard->m_fX) +
                    (Card->m_fY - ParentCard->m_fY) *
                        (Card->m_fY - ParentCard->m_fY));
          count++;
        }
      }
      if (count) {
        r /= count;
      }
      // 64 dots on display
      float cx1, cx2, dummy;
      DispPosToCardPos(0, 0, cx1, dummy);
      DispPosToCardPos(64, 0, cx2, dummy);
      float r2 = fabs(cx2 - cx1);
      if (r < r2) {
        r = r2;
      }
      float rbak = r; // Min distance for card insertion
      int itarcount = 0;
      while (itarcount++ < 100) {
        float t = (rand() % 10000) * 0.0002f * 3.1415926f;
        float r3 = r; // (rand() % 10001) * 0.0001f * r;
        NewCard->m_fX = ParentCard->m_fX + sin(t) * r3;
        NewCard->m_fY = ParentCard->m_fY - cos(t) * r3;

        if (NewCard->m_fX < -1.0f) {
          NewCard->m_fX = -1.0f;
        }
        if (NewCard->m_fY < -1.0f) {
          NewCard->m_fY = -1.0f;
        }
        if (NewCard->m_fX > 2.0f) {
          NewCard->m_fX = 2.0f;
        }
        if (NewCard->m_fY > 2.0f) {
          NewCard->m_fY = 2.0f;
        }
        bool found = false;
        for (int i = 0; i < m_Document->m_Cards->Count && !found; i++) {
          TCard *Card = m_Document->GetCardByIndex(i);
          if (Card != NewCard &&
              (Card->m_fX - NewCard->m_fX) * (Card->m_fX - NewCard->m_fX) +
                      (Card->m_fY - NewCard->m_fY) *
                          (Card->m_fY - NewCard->m_fY) <
                  (rbak * rbak / 4.0f)) {
            found = true;
          }
        }
        if (!found) {
          break;
        } else {
          r += rbak * 0.1f / itarcount;
        }
      }
    }
    for (int il = 0; il < LastCard->m_Labels->Count; il++) {
      NewCard->m_Labels->AddLabel(LastCard->m_Labels->GetLabel(il));
    }
    NewCard->m_nShape = LastCard->m_nShape;
    m_nTargetLink = -1;
    TLink *Link = m_Document->NewLink();
    Link->m_nFromID = ParentID;
    Link->m_nDestID = m_nTargetCard;

    Ti_CheckTimer(this);
    CloseEditBox();
    CloseTextEditBox();
    if (PC_Client->ActivePage == TS_Browser) {
      SetEdTitleBPos();
      Ed_TitleB->Visible = true;
      Ed_TitleB->SetFocus();
    } else {
      Ed_Title->SelectAll();
      Ed_Title->SetFocus();
    }
  } else {
    // Child card creation complete

    // Card create

    /*
     // New card create
     MI_NewCardClick(Sender);
     */
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserInsertCardToLink() {
  BackupSub(PBL_InsertCard->Caption);

  // Insert new card to link
  TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
  int DestID = Link->m_nDestID; // Link Dest ID
  int lastcard = Link->m_nFromID;
  int lastindex = m_Document->SearchCardIndex(Link->m_nFromID);
  int insindex = lastindex + 1;
  TCard *DestCard = m_Document->GetCard(DestID);           // Destination card
  TCard *LastCard = m_Document->GetCardByIndex(lastindex); // Last linked card

  // Insert card between link source and dest
  for (int il = 0; il < m_Document->m_Links->Count; il++)
    if (il != m_nTargetLink) {
      TLink *Link = m_Document->GetLinkByIndex(il);
      if (Link->m_nFromID == lastcard) {
        int index = m_Document->SearchCardIndex(Link->m_nDestID) + 1;
        if (index > insindex) {
          insindex = index;
        }
      }
    }

  m_Document->ClearCardSelection();
  TCard *NewCard = m_Document->NewCard(insindex);
  NewCard->m_bSelected = true;

  // New card position and link update
  NewCard->m_fX = (LastCard->m_fX + DestCard->m_fX) * 0.5f;
  NewCard->m_fY = (LastCard->m_fY + DestCard->m_fY) * 0.5f;

  // Change link Dest to new card
  Link->m_nDestID = NewCard->m_nID;

  m_nTargetCard = NewCard->m_nID;
  m_nTargetLink = -1;

  // New link
  TLink *Link2 = m_Document->NewLink();
  Link2->m_nFromID = NewCard->m_nID;
  Link2->m_nDestID = DestID;
  for (int il = 0; il < Link->m_Labels->Count; il++) { // Label loop
    Link2->m_Labels->AddLabel(Link->m_Labels->GetLabel(il));
  }

  Link2->m_bDirection = Link->m_bDirection;

  Ti_CheckTimer(this);
  CloseEditBox();
  CloseTextEditBox();
  if (PC_Client->ActivePage == TS_Browser) {
    SetEdTitleBPos();
    Ed_TitleB->Visible = true;
    Ed_TitleB->SetFocus();
  } else {
    Ed_Title->SelectAll();
    Ed_Title->SetFocus();
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormKeyDown(TObject *Sender, WORD &Key,
                                      TShiftState Shift) {
  // M_File->Caption = Key;
  // M_File->Caption = (int)Active;
  if (PC_Client->ActivePage == TS_Browser && Shift.Contains(ssCtrl) &&
      !Shift.Contains(ssAlt) && !RE_Edit->Focused() &&
      !Ed_LinkTitle->Focused() && !Ed_TitleB->Visible &&
      !m_bTitleEditRequested) {
    bool fontShortcut = Shift.Contains(ssShift);
    switch (Key) {
    case VK_PRIOR:
      if (fontShortcut) {
        MVF_MagnifyClick(Sender);
      } else {
        ZoomBrowserWheel(WHEEL_DELTA);
      }
      Key = 0;
      return;
    case VK_NEXT:
      if (fontShortcut) {
        MVF_ReduceClick(Sender);
      } else {
        ZoomBrowserWheel(-WHEEL_DELTA);
      }
      Key = 0;
      return;
    case '0':
    case VK_NUMPAD0:
      if (fontShortcut) {
        MVF_DefaultSizeClick(Sender);
      } else {
        ResetBrowserZoom();
      }
      Key = 0;
      return;
    }
  }

  if (Key >= 37 && Key <= 40 && PC_Client->ActivePage == TS_Browser &&
      m_CardVisible && TB_Zoom->Focused()) {
    // if (Key >= 37 && Key <= 40 && PC_Client->ActivePage == TS_Browser &&
    // m_CardVisible && (TB_Zoom->Focused() || !Active ||
    // Fo_FullScreen->Visible)){
    m_bSkipAutoZoom = true; // Disable AutoZoom for cursor
                            // Cursor to card

    if (!Shift.Contains(ssCtrl)) {
      // Ctrl held (move linked cards)

      float sx = 0.5f * m_nBrowserWidth, sy = 0.5f * m_nBrowserHeight;
      if (m_nTargetCard >= 0) {
        // cardSelected
        int index = m_Document->SearchCardIndex(m_nTargetCard);
        if (index >= 0) {
          sx = m_CardX[index];
          sy = m_CardY[index];
        }
      }

      float mindist = -1.0f;
      int minindex = -1;
      if (!Shift.Contains(ssShift) || m_nTargetCard < 0) {
        // Link destination search
        for (int i = 0; i < m_Document->m_Cards->Count; i++)
          if (m_CardVisible[i]) {
            TCard *Card = m_Document->GetCardByIndex(i);
            if (Card->m_bGetFocus) {
              float md = MoveDistance(sx, sy, m_CardX[i], m_CardY[i], Key - 37);
              if (md > 0.0f && (mindist < 0.0f || md < mindist)) {
                mindist = md;
                minindex = i;
              }
            }
          }
      } else {
        // Link source card search
        for (int i = 0; i < m_Document->m_Links->Count; i++)
          if (m_LinkVisible[i]) {
            TLink *Link = m_Document->GetLinkByIndex(i);
            int index = -1;
            if (Link->m_nFromID == m_nTargetCard) {
              index = m_Document->SearchCardIndex(Link->m_nDestID);
            }
            if (Link->m_nDestID == m_nTargetCard) {
              index = m_Document->SearchCardIndex(Link->m_nFromID);
            }
            if (index >= 0) {
              TCard *Card = m_Document->GetCardByIndex(index);
              if (Card->m_bGetFocus) {
                float md = MoveDistance(sx, sy, m_CardX[index], m_CardY[index],
                                        Key - 37);
                if (md > 0.0f && (mindist < 0.0f || md < mindist)) {
                  mindist = md;
                  minindex = index;
                }
              }
            }
          }
      }

      if (minindex >= 0) {
        m_Document->ClearCardSelection();
        TCard *Card = m_Document->GetCardByIndex(minindex);
        m_nTargetCard = Card->m_nID;
        Card->m_bSelected = true;
      }
    } else {
      // Ctrl held

      // Child node key
      int parent = m_Document->SearchParent(m_nTargetCard, Key == 40, true);

      if (parent >= 0)
        switch (Key) {
        case 38: // Up (child node, BS equiv)
        case 40: // Down (sibling node)
        {
          m_nTargetCard = parent;
          m_Document->ClearCardSelection();
          TCard *Card = m_Document->GetCardByIndex(parent);
          Card->m_bSelected = true;
        } break;
        case 37: // Left (prev sibling key)
        case 39: // Right (next sibling key)
        {
          int card = m_Document->SearchBrother(m_nTargetCard, parent, Key == 39,
                                               false, true);
          if (card >= 0) {
            m_nTargetCard = card;
            m_Document->ClearCardSelection();
            TCard *Card = m_Document->GetCardByIndex(card);
            Card->m_bSelected = true;
          }
        }
        }
    }

    Key = 0;
  }

  if (Key == 8 && PC_Client->ActivePage == TS_Browser && m_CardVisible &&
      TB_Zoom->Focused()) {
    // if (Key == 8 && PC_Client->ActivePage == TS_Browser && m_CardVisible &&
    // (TB_Zoom->Focused() || !Active)){
    /*
     // Card not in Display range
     // Return to previously viewed card (Viewed updates during traversal)
     int last = m_Document->SearchLast(m_nTargetCard);
     if (last > 0){
     m_nTargetCard = last;
     }
     */

    // Child node create
    int parent = m_Document->SearchParent(m_nTargetCard, false, true);
    if (parent >= 0) {
      m_nTargetCard = parent;
      m_Document->ClearCardSelection();
      TCard *Card = m_Document->GetCardByIndex(parent);
      Card->m_bSelected = true;
    }
    Key = 0;
  }

  if (PC_Client->ActivePage == TS_Browser) {
    // Double-click
    if (Key == 46 && TB_Zoom->Focused()) {
      // Delete
      if (m_nTargetCard >= 0) {
        // Delete card
        PL_DeleteCardClick(Sender);
        /*
         m_Document->DeleteCard(m_nTargetCard);
         m_nTargetCard = -1;
         m_nTargetCard2 = -1;
         */
      } else if (m_nTargetLink >= 0) {
        // Link linedelete
        BackupSub(PI_DeleteLink->Caption);
        m_Document->DeleteLinkByIndex(m_nTargetLink);
        m_nTargetLink = -1;
      }
      PB_BrowserMouseUp(Sender, mbLeft, TShiftState(), 0, 0);
      Key = 0;
    } else if (Key == 32) {
      // Space
      if (!m_bShowRecent) { // Key release = stop Recent Display
        m_bShowRecent = true;

        // Stop Recent Display
        m_nLastTarget = -1;
        m_nFocusCursorPos = 0;
        m_bRedrawRequested = true;
      }
    }
  }
  if (Key == 46 && LB_List->Focused()) {
    // Delete
    if (m_nTargetCard >= 0) {
      // Delete card
      PL_DeleteCardClick(Sender);
    }
    Key = 0;
  }

  if (Key == 27 && Fo_FullScreen->Visible) { // ESC
    ExitFullScreen();
    Key = 0;
  }

  if (Key == 45 && PC_Client->ActivePage == TS_Browser) {
    // Browser Insert key (child node create)

    if (m_nTargetCard >= 0) {
      // cardSelected

      // Child (or sibling) node create
      BrowserNewChildCard(Shift.Contains(ssShift));
      Key = 0;
    } else if (m_nTargetLink >= 0) {
      // linkSelected

      // Insert new card to link
      BrowserInsertCardToLink();
      Key = 0;
    } else {
      // Card selected or link selected

      // Card create

      /*
       // New card create
       MI_NewCardClick(Sender);
       */
    }
  }

  // M_File->Caption = Key;
  if (PC_Client->ActivePage == TS_Browser && TB_Zoom->Focused() &&
      ((Key >= 48 && Key <= 90) || (Key >= 96 && Key <= 111) ||
       (Key >= 186 && Key <= 226)) &&
      !Ed_TitleB->Visible && !m_bTitleEditRequested && !Shift.Contains(ssAlt) &&
      !Shift.Contains(ssCtrl)
      // !Shift.Contains(ssShift) &&
  ) {
    if (m_nTargetLink >= 0 && !Ed_LinkTitle->Focused()) {
      Ed_LinkTitle->SelectAll();
      Ed_LinkTitle->SetFocus();
      keybd_event(Key, 0, 0, 0);
      keybd_event(Key, 0, KEYEVENTF_KEYUP, 0);
    } else {
      m_bTitleEditRequested = true;
      Ti_CheckTimer(Sender);
      if (Ed_TitleB->Visible) {
        keybd_event(Key, 0, 0, 0);
        keybd_event(Key, 0, KEYEVENTF_KEYUP, 0);
      }
    }
    Key = 0;
  }

  if (PC_Client->ActivePage == TS_Drawing) {
    // End Drawing
    if (Key == 46) {
      m_Drawing->DDelete();
      ApplyDrawing();
      Key = 0;
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVF_DefaultSizeClick(TObject *Sender) {
  UD_Size->Position = SettingView.m_Font->Size;
  Ed_Size->Text = UD_Size->Position;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVF_MagnifyClick(TObject *Sender) {
  int last = UD_Size->Position;
  UD_Size->Position = UD_Size->Position * 1.5;
  if (UD_Size->Position > SettingView.m_Font->Size &&
      last < SettingView.m_Font->Size) {
    UD_Size->Position = SettingView.m_Font->Size;
  }
  if (UD_Size->Position > UD_Size->Max) {
    UD_Size->Position = UD_Size->Max;
  }
  Ed_Size->Text = UD_Size->Position;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVF_ReduceClick(TObject *Sender) {
  int last = UD_Size->Position;
  UD_Size->Position = UD_Size->Position * 0.67;
  if (UD_Size->Position < SettingView.m_Font->Size &&
      last > SettingView.m_Font->Size) {
    UD_Size->Position = SettingView.m_Font->Size;
  }
  if (UD_Size->Position < UD_Size->Min) {
    UD_Size->Position = UD_Size->Min;
  }
  Ed_Size->Text = UD_Size->Position;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVL_LinkClick(TObject *Sender) {
  SettingView.m_bLinkVisible = !SettingView.m_bLinkVisible;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVL_LinkDirectionClick(TObject *Sender) {
  SettingView.m_bLinkDirectionVisible = !SettingView.m_bLinkDirectionVisible;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVL_LinkNameClick(TObject *Sender) {
  SettingView.m_bLinkNameVisible = !SettingView.m_bLinkNameVisible;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_NewCardClick(TObject *Sender) {
  BackupSub(MI_NewCard->Caption);

  int insindex = m_Document->m_Cards->Count;
  if (m_nTargetCard >= 0) {
    // cardSelected
    insindex = m_Document->SearchCardIndex(m_nTargetCard);

    if (Sender != PL_NewCard) {
      // Card list popup menu insert
      insindex++;
    }
  }

  m_Document->ClearCardSelection();
  TCard *Card = m_Document->NewCard(insindex);
  Card->m_bSelected = true;
  m_nTargetCard = Card->m_nID;
  m_nTargetLink = -1;

  Ti_CheckTimer(Sender);
  CloseEditBox();
  CloseTextEditBox();
  if (PC_Client->ActivePage == TS_Browser) {
    SetEdTitleBPos();
    Ed_TitleB->Visible = true;
    Ed_TitleB->SetFocus();
  } else {
    Ed_Title->SelectAll();
    Ed_Title->SetFocus();
  }
}
// ---------------------------------------------------------------------------

bool TFo_Main::CheckTreeButton(int index, int X, int Y) {
  if (TreeMode()) {
    int i = index;
    TCard *Card = m_Document->GetCardByIndex(i);
    if (Card->m_bHasChild) {
      int fleft = m_CardX[i] + m_CardWidth[i] / 2 - m_nFontHeight;
      int ftop = m_CardY[i] + m_CardHeight[i] / 2 - m_nFontHeight / 2;
      int fsize = (m_nFontHeight * 2) / 3;
      fsize = fsize / 2 * 2 + 1;
      if (fsize < 13) {
        fsize = 13;
      }
      int fright = m_CardX[i] + m_CardWidth[i] / 2 - m_nFontHeight + fsize;
      int fbottom =
          m_CardY[i] + m_CardHeight[i] / 2 - m_nFontHeight / 2 + fsize;
      if (X >= fleft && X < fright && Y >= ftop && Y < fbottom) {
        Card->m_bFold = !Card->m_bFold;
        m_bMDownBrowser = 0;
        m_Document->RefreshList();
        return true;
      }
    }
  }
  return false;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_BrowserMouseDown(TObject *Sender,
                                              TMouseButton Button,
                                              TShiftState Shift, int X, int Y) {
  m_nMDownBrowserX = X;
  m_nMDownBrowserY = Y;
  m_nMDownTargetX = X;
  m_nMDownTargetY = Y;

  if (PC_Client->ActivePage != TS_Browser || m_bDblClicked ||
      m_CardVisible == NULL) {
    return;
  }

  if (Button == mbLeft) {
    m_bMDownBrowser = 1;
  }

  bool LastEd_TitleBVisible = Ed_TitleB->Visible;
  RE_EditExit(Sender);
  TB_Zoom->SetFocus();
  m_bTitleEditRequested = false;

  if (SettingView.m_bOverview && Button == mbLeft) {
    if (X >= m_nOVXOffset && X < m_nOVXOffset + m_nOVWidth &&
        Y >= m_nOVYOffset && Y < m_nOVYOffset + m_nOVHeight) {
      m_bMDownBrowser = 4;
      m_fBrowserScrollRatio = 0.0f;
      PB_BrowserMouseMove(Sender, Shift, X, Y);
      return;
    }
  }

  m_nScrollTargetX = -65536;
  m_nScrollTargetY = -65536;

  int lastcard = m_nTargetCard;
  m_nTargetCard = -1;
  m_nTargetLink = -1;
  m_nTargetCard2 = -1;
  int aindex = -1;

  if (lastcard >= 0) {
    int i = m_Document->SearchCardIndex(lastcard);
    if (i >= 0)
      if (m_CardVisible[i] && m_CardAssign[i] == i) {
        if (CheckTreeButton(i, X, Y)) {
          return;
        } else if (X >= m_CardX[i] - m_CardWidth[i] / 2 &&
                   X < m_CardX[i] + m_CardWidth[i] / 2 &&
                   Y >= m_CardY[i] - m_CardHeight[i] / 2 &&
                   Y < m_CardY[i] + m_CardHeight[i] / 2) {
          aindex = i;
        }
      }
  }

  int *draworder = new int[m_Document->m_Cards->Count];
  memset(draworder, 0, sizeof(int) * m_Document->m_Cards->Count);
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    int vo = m_Document->GetCardByIndex(i)->m_nViewedOrder;
    draworder[vo] = i;
  }

  for (int ic = m_Document->m_Cards->Count - 1; ic >= 0 && aindex == -1; ic--) {
    int i = draworder[ic];
    if (m_CardVisible[i] && m_CardRelated[i] && m_CardAssign[i] == i) {
      if (CheckTreeButton(i, X, Y)) {
        delete[] draworder;
        return;
      } else if (X >= m_CardX[i] - m_CardWidth[i] / 2 &&
                 X < m_CardX[i] + m_CardWidth[i] / 2 &&
                 Y >= m_CardY[i] - m_CardHeight[i] / 2 &&
                 Y < m_CardY[i] + m_CardHeight[i] / 2) {
        aindex = i;
      }
    }
  }

  for (int ic = m_Document->m_Cards->Count - 1; ic >= 0 && aindex == -1; ic--) {
    int i = draworder[ic];
    if (m_CardVisible[i] && m_CardAssign[i] == i) {
      if (CheckTreeButton(i, X, Y)) {
        delete[] draworder;
        return;
      } else if (X >= m_CardX[i] - m_CardWidth[i] / 2 &&
                 X < m_CardX[i] + m_CardWidth[i] / 2 &&
                 Y >= m_CardY[i] - m_CardHeight[i] / 2 &&
                 Y < m_CardY[i] + m_CardHeight[i] / 2) {
        aindex = i;
      }
    }
  }
  delete[] draworder;

  if (Shift.Contains(ssAlt) && Button == mbLeft) {
    if (aindex >= 0) {
      int i = aindex;
      TCard *Card = m_Document->GetCardByIndex(i);
      SettingView.m_nLinkTarget = Card->m_nID;
    } else {
      SettingView.m_nLinkTarget = -1;
    }
    m_nTargetCard = lastcard;
    m_bMDownBrowser = 0;
    return;
  } else if (aindex >= 0) {
    int i = aindex;
    TCard *Card = m_Document->GetCardByIndex(i);
    m_nTargetCard = Card->m_nID;

    if (!Shift.Contains(ssCtrl) && Button != mbRight && !Card->m_bSelected) {
      m_Document->ClearCardSelection();
    }

    if (Button == mbLeft && Shift.Contains(ssCtrl) &&
        !Shift.Contains(ssShift)) {
      Card->m_bSelected = !Card->m_bSelected;
    } else {
      Card->m_bSelected = true;
    }
    m_nMDownBrowserOffsetX = m_CardX[i] - X;
    m_nMDownBrowserOffsetY = m_CardY[i] - Y;

    if (Button == mbLeft) {
      if (!Card->m_bLabelIsFold) {
        if (SB_ToolLabel->Down) {
          if (m_nToolLabel > 0) {
            BackupSub(MLText.AddLabel);
            Card->m_Labels->AddLabel(m_nToolLabel);
          } else {
            BackupSub(MLText.DeleteLabel);
            Card->m_Labels->Clear();
          }
          m_Document->RefreshLabel();
        }

        if (Shift.Contains(ssShift) && Shift.Contains(ssCtrl)) {
          TStringList *SL = new TStringList();
          for (int i2 = 0; i2 < Card->m_Lines->Count; i2++) {
            if (IsFileNameOrURL(Card->m_Lines->Strings[i2])) {
              SL->Add(Card->m_Lines->Strings[i2]);
            }
          }

          if (SL->Count == 1) {
            OpenExtLink(SL->Strings[0]);
            m_bMDownBrowser = 0;
            delete SL;
            return;
          } else if (SL->Count > 1) {
            FreeMIExtLink();
            MI_ExtLink = new TList();
            for (int i = 0; i < SL->Count; i++) {
              TMenuItem *MI = new TMenuItem(Po_ExtLink);
              MI->Caption = SL->Strings[i];
              MI->OnClick = PM_ExtLinkClick;
              MI_ExtLink->Add(MI);
              Po_ExtLink->Items->Add(MI);
            }
            TPoint P;
            P.x = ((TControl *)Sender)->Left + X;
            P.y = ((TControl *)Sender)->Top + Y;
            P = ((TControl *)Sender)->ClientToScreen(P);
            Po_ExtLink->Popup(P.x, P.y);
            m_bMDownBrowser = 0;

            delete SL;
            return;
          }

          delete SL;
        } else if (!Card->m_bSelected) {
          m_bMDownBrowser = 0;
        } else if (lastcard == m_nTargetCard && !LastEd_TitleBVisible) {
          bool b = true;
          if (SettingFile.fepOperation) {
            b = !SettingFile.fepOperation(
                m_Document, UnicodeString("Card_EditTitle").c_str());
          }

          if (RE_Edit->Parent != Pa_Editor &&
              !SettingView.m_bEditInBrowserAlways) {
            CloseTextEditBox();
          }

          if (b) {
            m_bTitleEditRequested = true;
          }
        } else if (Shift.Contains(ssShift) && !Shift.Contains(ssCtrl)) {
          if (lastcard >= 0) {
            BackupSub(MI_NewLink->Caption);
            TLink *Link = m_Document->NewLink();
            Link->m_nFromID = lastcard;
            Link->m_nDestID = m_nTargetCard;

            if (SB_ToolLinkLabel->Down && m_nToolLinkLabel > 0) {
              Link->m_Labels->AddLabel(m_nToolLinkLabel);
            }
          }
        }
      } else {
        for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
          if (m_CardVisible[i2] && i != i2) {
            TCard *Card2 = m_Document->GetCardByIndex(i2);
            if (m_Document->LabelIsSame(Card, Card2)) {
              Card2->m_bSelected = Card->m_bSelected;
            }
          }
      }
    }
  } else {
    if (Shift.Contains(ssCtrl)) {
      m_nTargetCard = lastcard;
      if (!Shift.Contains(ssShift)) {
        m_bMDownBrowser = 0;
        return;
      }
    } else {
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        TCard *Card = m_Document->GetCardByIndex(i);
        Card->m_bSelected = false;
      }
    }

    if (Shift.Contains(ssShift)) {
      m_bMDownBrowser = 5;
      return;
    }
  }

  if (m_nTargetCard == -1 && m_Document->m_Links->Count > 0) {
    double mind = 0.0;
    int min = 0;
    bool editdest = false;
    int count = 0;

    for (int i = 0; i < m_Document->m_Links->Count; i++)
      if (m_LinkVisible[i]) {
        TLink *Link = m_Document->GetLinkByIndex(i);
        int card1index = m_Document->SearchCardIndex(Link->m_nFromID);
        int card2index = m_Document->SearchCardIndex(Link->m_nDestID);
        if (m_CardVisible[card1index] && m_CardVisible[card2index]) {
          double X1 = m_CardX[card1index];
          double Y1 = m_CardY[card1index];
          double X2 = m_CardX[card2index];
          double Y2 = m_CardY[card2index];

          double S = 100.0f;
          if (Link->m_nShape < 6) {
            S = DistanceToLine(X, Y, X1, Y1, X2, Y2);
          } else {
            float xd = X2 - X1;
            float yd = Y2 - Y1;
            float len = sqrt(xd * xd + yd * yd) * sqrt(2);
            if (len > 0.0f) {
              float radstep = M_PI / len;

              float pos = radstep;
              float lastx = 0.0f, lasty = 0.0f;
              while (pos < M_PI) {
                float x0, y0;
                CurvePosToXY(pos, x0, y0);
                double S1 = DistanceToLine(X, Y, X1 + lastx, Y1 + lasty,
                                           X1 + xd * x0, Y1 + yd * y0);
                lastx = xd * x0;
                lasty = yd * y0;
                pos += radstep;
                if (S1 < S) {
                  S = S1;
                }
              }
            }
          }
          if (S < mind || count++ == 0) {
            mind = S;
            min = i;
            editdest = (X - X1) * (X - X1) + (Y - Y1) * (Y - Y1) >
                       (X - X2) * (X - X2) + (Y - Y2) * (Y - Y2);
          }
        }
      }
    if (mind < 4 && count) {
      m_nTargetLink = min;

      if (Button == mbLeft) {
        if (SB_ToolLinkLabel->Down) {
          TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
          if (m_nToolLinkLabel > 0) {
            BackupSub(MLText.AddLabel);
            Link->m_Labels->AddLabel(m_nToolLinkLabel);
          } else {
            BackupSub(MLText.DeleteLabel);
            Link->m_Labels->Clear();
          }
          m_Document->RefreshLabel();
        }

        if (editdest) {
          m_bMDownBrowser = 2;
        } else {
          m_bMDownBrowser = 3;
        }
      }
    }
  }

  m_nMDownBrowserScrollX = Sc_X->Position;
  m_nMDownBrowserScrollY = Sc_Y->Position;
  if (!SettingView.m_bNoScrollLag) {
    m_uMDownBrowserLast = GetTickCount();
  }
  m_bMDownBrowserMoved = false;

  Ti_CheckTimer(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_BrowserDblClick(TObject *Sender) {
  CloseEditBox();

  if (SettingFile.fepOperation) {
    if (SettingFile.fepOperation(m_Document,
                                 UnicodeString("Card_DblClick").c_str())) {
      return;
    }
  }

  if (m_nTargetCard >= 0) {
    TCard *Card = m_Document->GetCard(m_nTargetCard);
    if (Card->m_bLabelIsFold) {
      for (int il = 0; il < Card->m_Labels->Count; il++) {
        m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1)
            ->m_bFold = false;
      }
    } else {
      if (SettingView.m_bEditInBrowser) {
        m_bTextEditRequested = true;
      } else {
        MV_EditorClick(Sender);
      }
    }
  } else {
    BrowserNewCard();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_BrowserMouseUp(TObject *Sender,
                                            TMouseButton Button,
                                            TShiftState Shift, int X, int Y) {
  int MDownBrowserBak = m_bMDownBrowser;
  m_bMDownBrowser = 0;
  if (m_nTargetCard2 >= 0) {
    switch (MDownBrowserBak) {
    case 1: {
      TCard *Card = m_Document->GetCard(m_nTargetCard);
      if (Card) {
        BackupSub(MI_NewLink->Caption);
        TLink *Link = m_Document->NewLink();
        Link->m_nFromID = m_nTargetCard;
        Link->m_nDestID = m_nTargetCard2;

        if (SB_ToolLinkLabel->Down && m_nToolLinkLabel > 0) {
          Link->m_Labels->AddLabel(m_nToolLinkLabel);
        }

        m_nTargetCard = -1;
        m_nTargetLink = m_Document->m_Links->Count - 1;
      }
    } break;
    case 2:
      {
        BackupSub(MLText.EditLink);
        TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
        Link->m_nDestID = m_nTargetCard2;
        m_Document->RefreshList();
        m_Document->RefreshLink();
      }
      break;
    case 3:
      {
        BackupSub(MLText.EditLink);
        TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
        Link->m_nFromID = m_nTargetCard2;
        m_Document->RefreshList();
        m_Document->RefreshLink();
      }
      break;
    }

    CloseEditBox();
    CloseTextEditBox();
  }

  m_nTargetCard2 = -1;

  m_bRedrawRequested = true;

  m_bDblClicked = false;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_BrowserMouseMove(TObject *Sender,
                                              TShiftState Shift, int X, int Y) {
  m_nMDownTargetX = X;
  m_nMDownTargetY = Y;
  m_bTitleEditRequested = false;

  if (m_bMDownBrowser && !m_bMDownBrowserMoved) {
    m_bMDownBrowserMoved = m_nMDownBrowserX != X || m_nMDownBrowserY != Y;
  }

  switch (m_bMDownBrowser) {
  case 1:
    if (m_nMDownTargetX != m_nMDownBrowserX ||
        m_nMDownTargetY != m_nMDownBrowserY) {
      CloseEditBox();
    }
    if (m_nTargetCard >= 0) {
      if (SB_Cursor->Down) {
        float CardX;
        float CardY;
        DispPosToCardPos(X + m_nMDownBrowserOffsetX, Y + m_nMDownBrowserOffsetY,
                         CardX, CardY);

        TCard *Card = m_Document->GetCard(m_nTargetCard);
        if (Card) {
          TList *moved = new TList();
          float ax = CardX - Card->m_fX;
          float ay = CardY - Card->m_fY;
          Card->m_fX = CardX;
          Card->m_fY = CardY;
          moved->Add(Card);
          bool fold = Card->m_bLabelIsFold;
          if (Shift.Contains(ssCtrl)) {
            for (int i = 0; i < m_Document->m_Cards->Count; i++) {
              TCard *Card2 = m_Document->GetCardByIndex(i);
              if (Card->m_nID != Card2->m_nID) {
                if (m_Document->LabelIsSame(Card, Card2)) {
                  if (!fold) {
                    Card2->m_fX += ax;
                    Card2->m_fY += ay;
                    if (Card2->m_fX < -1.5f) {
                      Card2->m_fX = -1.5f;
                    }
                    if (Card2->m_fX > 2.5f) {
                      Card2->m_fX = 2.5f;
                    }
                    if (Card2->m_fY < -1.5f) {
                      Card2->m_fY = -1.5f;
                    }
                    if (Card2->m_fY > 2.5f) {
                      Card2->m_fY = 2.5f;
                    }
                  } else {
                    Card2->m_fX = CardX;
                    Card2->m_fY = CardY;
                  }
                  moved->Add(Card2);
                }
              }
            }
          } else {
            for (int i = 0; i < m_Document->m_Cards->Count; i++) {
              TCard *Card2 = m_Document->GetCardByIndex(i);
              if (Card->m_nID != Card2->m_nID && Card2->m_bSelected) {
                Card2->m_fX += ax;
                Card2->m_fY += ay;
                if (Card2->m_fX < -1.5f) {
                  Card2->m_fX = -1.5f;
                }
                if (Card2->m_fX > 2.5f) {
                  Card2->m_fX = 2.5f;
                }
                if (Card2->m_fY < -1.5f) {
                  Card2->m_fY = -1.5f;
                }
                if (Card2->m_fY > 2.5f) {
                  Card2->m_fY = 2.5f;
                }
                moved->Add(Card2);
              }
            }
          }

          if (TreeMode()) {
            for (int i = 0; i < moved->Count; i++) {
              TCard *Card = (TCard *)moved->Items[i];

              float x = 0.0f;
              bool first = true;
              for (int ic = 0; ic < m_Document->m_Cards->Count; ic++)
                if (m_CardVisible[ic]) {
                  TCard *Card2 = m_Document->GetCardByIndex(ic);
                  if (Card != Card2 &&
                      Card2->m_nParentID == Card->m_nParentID) {
                    if (first) {
                      x = Card2->m_fX;
                      first = false;
                    } else if (fabs(Card2->m_fX - Card->m_fX) <
                               fabs(x - Card->m_fX)) {
                      x = Card2->m_fX;
                    }
                  }
                }

              TList *cards = new TList();
              for (int ic = 0; ic < m_Document->m_Cards->Count; ic++)
                if (m_CardVisible[ic]) {
                  TCard *Card2 = m_Document->GetCardByIndex(ic);
                  if (Card != Card2 &&
                      Card2->m_nParentID == Card->m_nParentID) {
                    if (fabs(Card2->m_fX - x) < 1.0e-5) {
                      cards->Add(Card2);
                    }
                  }
                }

              for (int ic = 0; ic < cards->Count; ic++) {
                TCard *Card2 = (TCard *)cards->Items[ic];
                int index1 = m_Document->SearchCardIndex(Card->m_nID);
                int index2 = m_Document->SearchCardIndex(Card2->m_nID);
                if ((index1 < index2 && Card->m_fY > Card2->m_fY) ||
                    (index1 > index2 && Card->m_fY < Card2->m_fY)) {
                  m_Document->SwapCard(index1, index2);
                }
              }

              delete cards;
            }
          }

          delete moved;

          m_bRedrawRequested = true;
        }
      } else {
        m_nTargetCard2 = -1;
        for (int i = m_Document->m_Cards->Count - 1; i >= 0; i--)
          if (m_Document->GetCardByIndex(i)->m_nID != m_nTargetCard &&
              m_CardVisible[i]) {
            if (X >= m_CardX[i] - m_CardWidth[i] / 2 &&
                X < m_CardX[i] + m_CardWidth[i] / 2 &&
                Y >= m_CardY[i] - m_CardHeight[i] / 2 &&
                Y < m_CardY[i] + m_CardHeight[i] / 2) {

              m_nTargetCard2 = m_Document->GetCardByIndex(i)->m_nID;
              m_nMDownBrowserOffsetX = m_CardX[i] - X;
              m_nMDownBrowserOffsetY = m_CardY[i] - Y;
            }
          }
        m_bRedrawRequested = true;
      }
    } else {
      float ShiftX = X - m_nMDownBrowserX;
      float ShiftY = Y - m_nMDownBrowserY;

      ShiftX /= m_nBrowserWidth;
      ShiftY /= m_nBrowserHeight;

      float Zoom = pow(2, TB_Zoom->Position / 2000.0f);
      ShiftX /= Zoom;
      ShiftY /= Zoom;

      PB_Browser->Tag = 1;
      Sc_X->Position = m_nMDownBrowserScrollX - ShiftX * 10000;
      Sc_Y->Position = m_nMDownBrowserScrollY - ShiftY * 10000;
      PB_Browser->Tag = 0;

      m_bRedrawRequested = true;
    }

    SetEdTitleBPos();
    break;
  case 2:
    if (m_nTargetLink >= 0) {
      TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);

      m_nTargetCard2 = -1;
      for (int i = m_Document->m_Cards->Count - 1; i >= 0; i--)
        if (m_Document->GetCardByIndex(i)->m_nID != Link->m_nFromID &&
            m_CardVisible[i]) {
          if (X >= m_CardX[i] - m_CardWidth[i] / 2 &&
              X < m_CardX[i] + m_CardWidth[i] / 2 &&
              Y >= m_CardY[i] - m_CardHeight[i] / 2 &&
              Y < m_CardY[i] + m_CardHeight[i] / 2) {

            m_nTargetCard2 = m_Document->GetCardByIndex(i)->m_nID;
            m_nMDownBrowserOffsetX = m_CardX[i] - X;
            m_nMDownBrowserOffsetY = m_CardY[i] - Y;
          }
        }
      m_bRedrawRequested = true;
    }
    break;
  case 3:
    if (m_nTargetLink >= 0) {
      TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);

      m_nTargetCard2 = -1;
      for (int i = m_Document->m_Cards->Count - 1; i >= 0; i--)
        if (m_Document->GetCardByIndex(i)->m_nID != Link->m_nDestID &&
            m_CardVisible[i]) {
          if (X >= m_CardX[i] - m_CardWidth[i] / 2 &&
              X < m_CardX[i] + m_CardWidth[i] / 2 &&
              Y >= m_CardY[i] - m_CardHeight[i] / 2 &&
              Y < m_CardY[i] + m_CardHeight[i] / 2) {

            m_nTargetCard2 = m_Document->GetCardByIndex(i)->m_nID;
            m_nMDownBrowserOffsetX = m_CardX[i] - X;
            m_nMDownBrowserOffsetY = m_CardY[i] - Y;
          }
        }
      m_bRedrawRequested = true;
    }
    break;
  case 4:
    {
      float cx =
          (((X * 1.0f - m_nOVXOffset) / m_nOVWidth - 0.5f) / OVERVIEWSIZE +
           0.5f);
      float cy =
          (((Y * 1.0f - m_nOVYOffset) / m_nOVHeight - 0.5f) / OVERVIEWSIZE +
           0.5f);

      float AX = 0.5f - cx;
      float AY = 0.5f - cy;

      AX *= 0.9f;
      AY *= 0.9f;

      AX *= 10000;
      AY *= 10000;

      PB_Browser->Tag = 1;
      if (m_nTargetCard >= 0 || m_nTargetLink >= 0) {
        SB_AutoScroll->Down = false;
      }
      m_fBrowserScrollRatio = 0.0f;
      m_nScrollTargetX = 5000 - AX;
      m_nScrollTargetY = 5000 - AY;
      PB_Browser->Tag = 0;
      BrowserAutoScroll();
    }
    break;
  case 5:
    int leftx, rightx, topy, bottomy;
    if (m_nMDownBrowserX < m_nMDownTargetX) {
      leftx = m_nMDownBrowserX;
      rightx = m_nMDownTargetX;
    } else {
      rightx = m_nMDownBrowserX;
      leftx = m_nMDownTargetX;
    }
    if (m_nMDownBrowserY < m_nMDownTargetY) {
      topy = m_nMDownBrowserY;
      bottomy = m_nMDownTargetY;
    } else {
      bottomy = m_nMDownBrowserY;
      topy = m_nMDownTargetY;
    }
    if (m_CardVisible)
      for (int i = 0; i < m_Document->m_Cards->Count; i++)
        if (m_CardVisible[i]) {
          if (m_CardX[i] - m_CardWidth[i] / 2 >= leftx &&
              m_CardY[i] - m_CardHeight[i] / 2 >= topy &&
              m_CardX[i] + m_CardWidth[i] / 2 <= rightx &&
              m_CardY[i] + m_CardHeight[i] / 2 <= bottomy) {
            TCard *Card = m_Document->GetCardByIndex(i);
            Card->m_bSelected = true;
            if (m_nTargetCard < 0) {
              m_nFocusCursorPos = 0;
              m_nTargetCard = Card->m_nID;
            }
            if (Card->m_bLabelIsFold) {
              for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
                if (m_CardVisible[i2] && i2 != i) {
                  TCard *Card2 = m_Document->GetCardByIndex(i2);
                  if (m_Document->LabelIsSame(Card, Card2)) {
                    Card2->m_bSelected = true;
                  }
                }
            }
          }
        }
    m_bRedrawRequested = true;
    break;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sc_YChange(TObject *Sender) {
  m_bRedrawRequested = true;
  if (PB_Browser->Tag == 0) {
    SB_AutoScroll->Down = false;
    m_nScrollTargetX = -65536;
    m_nScrollTargetY = -65536;
  }
}
// ---------------------------------------------------------------------------

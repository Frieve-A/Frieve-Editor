// ---------------------------------------------------------------------------
// Card/link filtering and visibility (split from fomain.cpp)

#include <vcl.h>
#pragma hdrstop

#include "fomain.h"
#include "fomain_filter.h"
#include "foedittext.h"
#include "folabeledit.h"
#include "forandomanimation.h"
#include "setting.h"

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void TFo_Main::LinkLimitationSub(int targetcard, int targetlink, int linkdepth,
                                 int *Score, bool linkdirection,
                                 bool linkbackward) {
  int *IFound = new int[m_Document->m_Cards->Count];
  memset(IFound, 0, sizeof(int) * m_Document->m_Cards->Count);

  int startingscore = linkdepth < 11 ? linkdepth + 1 : 0xffff;

  if (targetcard >= 0) {
    // Card start
    int idx = m_Document->SearchCardIndex(targetcard);
    if (idx >= 0) {
      Score[idx] = startingscore;
    }
  } else if (targetlink >= 0) {
    // Link start
    TLink *Link = m_Document->GetLinkByIndex(targetlink);
    int idx;
    if (!linkdirection || linkbackward || !Link->m_bDirection) {
      idx = m_Document->SearchCardIndex(Link->m_nFromID);
      Score[idx] = startingscore;
    }
    if (!SettingView.m_bLinkDirection || !SettingView.m_bLinkBackward ||
        !Link->m_bDirection) {
      idx = m_Document->SearchCardIndex(Link->m_nDestID);
      Score[idx] = startingscore;
    }
  }

  // Link loop
  bool found = true;
  while (found) { // for (int i = 0 ; i < SettingView.m_nLinkLimitation ; i++){
    found = false;

    // Card loop
    for (int ic = 0; ic < m_Document->m_Cards->Count; ic++)
      if (Score[ic] > 1 && !IFound[ic]) {
        IFound[ic] = 1;
        TCard *Card = m_Document->GetCardByIndex(ic);
        // Card->m_bVisible = true;
        int id = Card->m_nID;

        for (int il = 0; il < m_Document->m_Links->Count; il++) {
          TLink *Link = m_Document->GetLinkByIndex(il);
          if (Link->m_bVisible) {
            int targetidx = -1;
            if (Link->m_nFromID == id &&
                (!linkdirection || !linkbackward || !Link->m_bDirection)) {
              targetidx = m_Document->SearchCardIndex(Link->m_nDestID);
            } else if (Link->m_nDestID == id &&
                       (!linkdirection || linkbackward ||
                        !Link->m_bDirection)) {
              targetidx = m_Document->SearchCardIndex(Link->m_nFromID);
            }
            if (targetidx >= 0) {
              // TCard *TargetCard = m_Document->GetCardByIndex(targetidx);
              // TargetCard->m_bVisible = true;
              if (Score[targetidx] < Score[ic] - 1) {
                Score[targetidx] = Score[ic] - 1;
                found = true;
                IFound[ic] = 0;
              }
            }
          }
        }
      }
  }

  for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
    if (Score[ic] > 0) {
      Score[ic] = startingscore + 1 - Score[ic];
    }
  }

  delete[] IFound;
}

// ---------------------------------------------------------------------------
void TFo_Main::SetCardVisible(bool bFoldTree) {
  // Display card

  if (m_nAnimation > 0 && m_nAnimation != 3) {
    // Card Visible setting
    return;
  }

  // All Display
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    Card->m_bVisibleBak = Card->m_bVisible;
    Card->m_bVisible = true;
  }
  for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++) {
    m_Document->GetLinkByIndex(i2)->m_bVisible = true;
  }

  // Label, size, link filter
  FilteringCard();

  // Link filter
  if (SettingView.m_bLinkLimitation &&
      (m_nTargetCard >= 0 || m_nTargetLink >= 0)) {
    // Link exists, card or link is selected
    /*
     // All Display
     for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
     m_Document->GetCardByIndex(i)->m_bVisible = false;
     }
     */

    int *Score = new int[m_Document->m_Cards->Count];
    memset(Score, 0, sizeof(int) * m_Document->m_Cards->Count);
    /*
     int *IFound = new int[m_Document->m_Cards->Count];
     memset(IFound, 0, sizeof(int) * m_Document->m_Cards->Count);

     int startingscore = SettingView.m_nLinkLimitation < 11 ?
     SettingView.m_nLinkLimitation + 1 : 0xffff;

     if (m_nTargetCard >= 0){
     // Card start
     int idx = m_Document->SearchCardIndex(m_nTargetCard);
     Score[idx] = startingscore;
     }else if (m_nTargetLink >= 0){
     // Link start
     TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
     int idx;
     if (!SettingView.m_bLinkDirection || SettingView.m_bLinkBackward ||
     !Link->m_bDirection){ idx = m_Document->SearchCardIndex(Link->m_nFromID);
     Score[idx] = startingscore;
     }
     if (!SettingView.m_bLinkDirection || !SettingView.m_bLinkBackward ||
     !Link->m_bDirection){ idx = m_Document->SearchCardIndex(Link->m_nDestID);
     Score[idx] = startingscore;
     }
     }

     // Link loop
     bool found = true;
     while (found){//for (int i = 0 ; i < SettingView.m_nLinkLimitation ; i++){
     found = false;

     // Card loop
     for (int ic = 0 ; ic < m_Document->m_Cards->Count ; ic++) if (Score[ic] > 1
     && !IFound[ic]){ IFound[ic] = 1; TCard *Card =
     m_Document->GetCardByIndex(ic);
     //Card->m_bVisible = true;
     int id = Card->m_nID;

     for (int il = 0 ; il < m_Document->m_Links->Count ; il++){
     TLink *Link = m_Document->GetLinkByIndex(il);
     if (Link->m_bVisible){
     int targetidx = -1;
     if (Link->m_nFromID == id &&
     (!SettingView.m_bLinkDirection ||
     !SettingView.m_bLinkBackward ||
     !Link->m_bDirection)){
     targetidx = m_Document->SearchCardIndex(Link->m_nDestID);
     }else if (Link->m_nDestID == id &&
     (!SettingView.m_bLinkDirection ||
     SettingView.m_bLinkBackward ||
     !Link->m_bDirection)){
     targetidx = m_Document->SearchCardIndex(Link->m_nFromID);
     }
     if (targetidx >= 0){
     //TCard *TargetCard = m_Document->GetCardByIndex(targetidx);
     //TargetCard->m_bVisible = true;
     if (Score[targetidx] < Score[ic] - 1){
     Score[targetidx] = Score[ic] - 1;
     found = true;
     IFound[ic] = 0;
     }
     }
     }
     }
     }
     }

     delete[] IFound;

     */
    // Display only cards reachable within specified link count from current
    // card
    LinkLimitationSub(
        m_nTargetCard, m_nTargetLink, SettingView.m_nLinkLimitation, Score,
        SettingView.m_bLinkDirection, SettingView.m_bLinkBackward);

    if (SettingView.m_nLinkTarget >= 0) {
      // Display only links between here and TargetCard
      int idx2 = m_Document->SearchCardIndex(SettingView.m_nLinkTarget);
      if (idx2 >= 0) {
        if (Score[idx2] > 0) {
          int *Score2 = new int[m_Document->m_Cards->Count];
          memset(Score2, 0, sizeof(int) * m_Document->m_Cards->Count);
          LinkLimitationSub(SettingView.m_nLinkTarget, -1,
                            SettingView.m_nLinkLimitation, Score2,
                            SettingView.m_bLinkDirection,
                            !SettingView.m_bLinkBackward);
          for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
            /*
             // Reflect for debug in card title
             TCard *Card_ = m_Document->GetCardByIndex(ic);
             Card_->m_Title = IntToStr(Score[ic]) + " : " +
             IntToStr(Score2[ic]);
             */
            if (Score2[ic] == 0) {
              Score[ic] = 0;
            } else if (SettingView.m_nLinkLimitation < 11 &&
                       Score[ic] + Score2[ic] >
                           SettingView.m_nLinkLimitation + 2) {
              Score[ic] = 0;
            }
          }
          delete[] Score2;
        } else {
          // Cannot reach TargetCard within specified link count; hide all
          for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
            Score[ic] = 0;
          }
        }
      } else {
        // TargetCard not found

        // Reset TargetCard
        SettingView.m_nLinkTarget = -1;
      }

      // TODO: Check if route can be traced from filtered cards here?
    }

    for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
      TCard *Card = m_Document->GetCardByIndex(ic);
      Card->m_bVisible &= Score[ic] > 0;
    }

    delete[] Score;

    // Reflect for debug in card title
    // m_Document->RefreshList();

  } /* else{
   // No link limit or no card selected (all displayed)
   for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
   m_Document->GetCardByIndex(i)->m_bVisible = true;
   }
   } */

  // In hierarchy view, hide cards below folded ones
  int changed = true;
  while (bFoldTree && changed) {
    changed = false;
    for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
      TCard *Card = m_Document->GetCardByIndex(ic);
      if (Card->m_bVisible && Card->m_nParentID >= 0) {
        // Recursively hide Display's parent, child, grandchild
        TCard *Card2 = m_Document->GetCard(Card->m_nParentID);
        if (Card2->m_bFold || !Card2->m_bVisible) {
          Card->m_bVisible = false;
          m_CardVisible[ic] = false;
          changed = true;
        }
      }
    }
  }

  // Force display selected card
  if (m_nTargetCard >= 0) {
    TCard *Card = m_Document->GetCard(m_nTargetCard);
    if (Card) {
      Card->m_bVisible = true;
    }
  }

  // Force display both ends of selected link
  if (m_nTargetLink >= 0) {
    TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
    if (Link) {
      m_Document->GetCard(Link->m_nFromID)->m_bVisible = true;
      m_Document->GetCard(Link->m_nDestID)->m_bVisible = true;
    }
  }

  // Restart Ticker animation when Display changes
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    if (Card->m_bVisibleBak != Card->m_bVisible) {
      // Reset Ticker
      Card->m_fTickerPos = 0.0;
    }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::FilteringCard() {
  // Filter cards to display by Label

  // label(Show)
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    if (Card->m_bVisible) {
      if (Card->m_Labels->Count) {
        bool visible = false;
        for (int il = 0; il < Card->m_Labels->Count && !visible; il++) {
          visible |=
              m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1)
                  ->m_bShow;
        }
        Card->m_bVisible &= visible;
      } else {
        Card->m_bVisible &= SettingView.m_bNoLabelVisible;
      }
    }
  }

  // label(Hide)
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    if (Card->m_bVisible) {
      if (Card->m_Labels->Count) {
        for (int il = 0; il < Card->m_Labels->Count && Card->m_bVisible; il++) {
          Card->m_bVisible &=
              !m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1)
                   ->m_bHide;
        }
      }
    }
  }

  // label(Link)
  for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++) {
    TLink *Link = m_Document->GetLinkByIndex(i2);
    if (Link->m_bVisible) {
      bool visible = true;
      if (Link->m_Labels->Count > 0 && m_nTargetLink != i2) {
        visible = false;
        for (int il = 0; il < Link->m_Labels->Count && !visible; il++) {
          TCardLabel *Label =
              m_Document->GetLabelByIndex(1, Link->m_Labels->GetLabel(il) - 1);
          visible |= Label->m_bShow;
        }
        for (int il = 0; il < Link->m_Labels->Count && visible; il++) {
          TCardLabel *Label =
              m_Document->GetLabelByIndex(1, Link->m_Labels->GetLabel(il) - 1);
          visible &= !Label->m_bHide;
        }
      } else if (m_nTargetLink != i2) {
        visible = SettingView.m_bNoLinkLabelVisible;
      }
      Link->m_bVisible = visible;
    }
  }

  // Filter cards to display by size

  // Size
  if (SettingView.m_bSizeLimitation)
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bVisible) {
        float size = Card->m_nSize;
        if (Card->m_Labels->Count) {
          for (int il = 0; il < Card->m_Labels->Count; il++) {
            size *=
                m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1)
                    ->m_nSize *
                0.01f;
          }
        }
        Card->m_bVisible &= size >= SettingView.m_nSizeLimitation - 1;
      }
    }

  // Filter cards to display by date
  m_Document->RefreshDateOrder();
  if (SettingView.m_bDateLimitation) {
    int currentdate = -2;
    int daterange =
        (SettingView.m_nDateLimitation * m_Document->m_Cards->Count) / 100;
    if (m_nTargetCard >= 0) {
      TCard *Card = m_Document->GetCard(m_nTargetCard);
      switch (SettingView.m_DateLimitationDateType) {
      case 0:
        currentdate = Card->m_nCreatedOrder;
        break;
      case 1:
        currentdate = Card->m_nUpdatedOrder;
        break;
      case 2:
        currentdate = Card->m_nViewedOrder;
        break;
      }
    }
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bVisible) {
        float date;
        switch (SettingView.m_DateLimitationDateType) {
        case 0:
          date = Card->m_nCreatedOrder;
          break;
        case 1:
          date = Card->m_nUpdatedOrder;
          break;
        case 2:
          date = Card->m_nViewedOrder;
          break;
        }
        switch (SettingView.m_DateLimitationType) {
        case 0:
          // Old
          Card->m_bVisible &= date <= daterange;
          break;
        case 1:
          // New
          Card->m_bVisible &= date >= daterange;
          break;
        case 2:
          // Near
          if (currentdate > -1.0f) {
            Card->m_bVisible &= fabs(date - currentdate) < daterange;
          }
          break;
        case 3:
          // Older
          if (currentdate > -1.0f) {
            Card->m_bVisible &=
                date <= currentdate && fabs(date - currentdate) < daterange;
          }
          break;
        case 4:
          // Newer
          if (currentdate > -1.0f) {
            Card->m_bVisible &=
                date >= currentdate && fabs(date - currentdate) < daterange;
          }
          break;
        }
      }
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_Label0Click(TObject *Sender) {
  int index = ((TMenuItem *)Sender)->Tag;
  if (m_nTargetCard < 0) {
    // No card selected
    return;
  } else {
    TCard *Card = m_Document->GetCard(m_nTargetCard);
    if (index >= Card->m_Labels->Count) {
      // No label
      PL_NoLabel->Checked = true;
    } else {
      // Has label
      ((TMenuItem *)MI_Labels->Items[Card->m_Labels->GetLabel(index) - 1])
          ->Checked = true;
    }
  }
  Po_Label->Tag = index;

  TPoint P;
  P.x = Bu_Label[index]->Left;
  P.y = Bu_Label[index]->Top;
  P = Pa_Card->ClientToScreen(P);
  Po_Label->Popup(P.x, P.y);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PL_Click(TObject *Sender) {
  int label = ((TMenuItem *)Sender)->Tag;
  int index = Po_Label->Tag;

  if (index < 0) {
    // Tool label specified
    m_nToolLabel = label;
    return;
  }

  if (m_nTargetCard < 0) {
    // No card selected
    return;
  }

  int AddLabel = -1;
  int DelLabel = -1;

  TCard *Card = m_Document->GetCard(m_nTargetCard);
  if (index >= Card->m_Labels->Count) {
    // Add label

    if (label > 0) {
      // Add label
      BackupSub(MLText.AddLabel);
      AddLabel = label;
    }
  } else {
    // Edit existing label
    if (label == 0) {
      // Delete label
      BackupSub(MLText.DeleteLabel);
      DelLabel = Card->m_Labels->GetLabel(index);
    } else {
      // Change label
      BackupSub(MLText.ChangeLabel);
      DelLabel = Card->m_Labels->GetLabel(index);
      AddLabel = label;
    }
  }

  if (AddLabel >= 0 || DelLabel >= 0) {
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bSelected) {
        if (AddLabel >= 0 && DelLabel < 0) {
          // Add
          Card->m_Labels->AddLabel(label);
        } else if (AddLabel >= 0 && DelLabel >= 0) {
          // Change
          if (Card->m_Labels->Contain(DelLabel)) {
            // Have label to delete
            if (Card->m_Labels->Contain(AddLabel)) {
              // Already have add label -> simple delete
              Card->m_Labels->DeleteLabel(DelLabel);
            } else {
              // Normal change
              for (int i2 = 0; i2 < Card->m_Labels->Count; i2++) {
                if (Card->m_Labels->GetLabel(i2) == DelLabel) {
                  Card->m_Labels->SetLabel(i2, AddLabel);
                  break;
                }
              }
            }
          } else {
            // Don't have label to delete -> simple add
            Card->m_Labels->AddLabel(label);
          }
        } else {
          // delete
          Card->m_Labels->DeleteLabel(DelLabel);
        }
      }
    }
  }
  /*
   TCard *Card = m_Document->GetCard(m_nTargetCard);

   if (index >= Card->m_Labels->Count){
   // labeladd

   if (label > 0){
   // labeladd
   BackupSub(MLText.AddLabel);
   Card->m_Labels->AddLabel(label);
   }
   }else{
   // Edit existing label
   if (label == 0){
   // labeldelete
   BackupSub(MLText.DeleteLabel);
   Card->m_Labels->DeleteLabel(Card->m_Labels->GetLabel(index));
   }else{
   // labelchange
   BackupSub(MLText.ChangeLabel);
   Card->m_Labels->SetLabel(index, label);
   }
   }
   */
  m_Document->RefreshLabel();
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_LabelClick(TObject *Sender) {
  if (SB_ToolLabel->Down) {
    SB_Cursor->Down = true;
  }
  Fo_LabelEdit = new TFo_LabelEdit(this);
  Fo_LabelEdit->m_Document = m_Document;
  Fo_LabelEdit->m_UndoRedo = m_UndoRedo;
  Fo_LabelEdit->m_LType = 0;
  Fo_LabelEdit->Caption = DeleteActionKey(ME_Label->Caption);
  Fo_LabelEdit->ShowModal();
  Fo_LabelEdit->Release();
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::TS_BrowserShow(TObject *Sender) {
  TB_Zoom->SetFocus();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ed_LinkTitleChange(TObject *Sender) {
  if (Ed_LinkTitle->Tag == 0) {
    // Edit selected link name
    TextEditBackupSub(MLText.EditLinkTitle);

    TLink *Link = m_Document->GetLinkByIndex(m_nCurrentLink);
    Link->m_Name = Ed_LinkTitle->Text;
    m_Document->RefreshLink();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ch_LinkDirectionClick(TObject *Sender) {
  if (Ed_LinkTitle->Tag == 0) {
    BackupSub(MLText.EditLink);
    // Toggle selected link direction on/off
    TLink *Link = m_Document->GetLinkByIndex(m_nCurrentLink);
    Link->m_bDirection = Ch_LinkDirection->Checked;
    m_Document->RefreshList();
    m_Document->RefreshLink();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_LinkDirectionInverseClick(TObject *Sender) {
  // Reverse selected link direction
  BackupSub(MLText.EditLink);
  TLink *Link = m_Document->GetLinkByIndex(m_nCurrentLink);
  int bak = Link->m_nFromID;
  Link->m_nFromID = Link->m_nDestID;
  Link->m_nDestID = bak;
  m_Document->RefreshList();
  m_Document->RefreshLink();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_LinkLabelClick(TObject *Sender) {
  if (SB_ToolLinkLabel->Down) {
    SB_Cursor->Down = true;
  }

  Fo_LabelEdit = new TFo_LabelEdit(this);
  Fo_LabelEdit->m_Document = m_Document;
  Fo_LabelEdit->m_UndoRedo = m_UndoRedo;
  Fo_LabelEdit->m_LType = 1;
  Fo_LabelEdit->Caption = DeleteActionKey(ME_LinkLabel->Caption);
  Fo_LabelEdit->ShowModal();
  Fo_LabelEdit->Release();
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_LinkLabel0Click(TObject *Sender) {
  int index = ((TMenuItem *)Sender)->Tag;
  if (m_nTargetLink < 0) {
    // No link selected
    return;
  } else {
    TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
    if (index >= Link->m_Labels->Count) {
      // No label
      PL_NoLinkLabel->Checked = true;
    } else {
      // Has label
      ((TMenuItem *)MI_LinkLabels->Items[Link->m_Labels->GetLabel(index) - 1])
          ->Checked = true;
    }
  }
  Po_LinkLabel->Tag = index;

  TPoint P;
  P.x = Bu_LinkLabel[index]->Left;
  P.y = Bu_LinkLabel[index]->Top;
  P = Pa_Link->ClientToScreen(P);
  Po_LinkLabel->Popup(P.x, P.y);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PL_NoLinkLabelClick(TObject *Sender) {
  int label = ((TMenuItem *)Sender)->Tag;
  int index = Po_LinkLabel->Tag;

  if (index < 0) {
    // Tool label specified
    m_nToolLinkLabel = label;
    return;
  }

  if (m_nTargetLink < 0) {
    // No link selected
    return;
  }

  TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);

  if (index >= Link->m_Labels->Count) {
    // Add label

    if (label > 0) {
      // Add label
      BackupSub(MLText.AddLabel);
      Link->m_Labels->AddLabel(label);
    }
  } else {
    // Edit existing label
    if (label == 0) {
      // Delete label
      BackupSub(MLText.DeleteLabel);
      Link->m_Labels->DeleteLabel(Link->m_Labels->GetLabel(index));
    } else {
      // labelchange
      BackupSub(MLText.ChangeLabel);
      Link->m_Labels->SetLabel(index, label);
    }
  }
  m_Document->RefreshLabel();
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_NewLinkLabelClick(TObject *Sender) {
  if (m_nTargetLink >= 0) {
    Fo_EditText = new TFo_EditText(this);
    Fo_EditText->La_Text->Caption = MLText.LabelName + ":";
    Fo_EditText->Caption = DeleteActionKey(MI_NewLinkLabel->Caption);
    Fo_EditText->ShowModal();
    if (Fo_EditText->ModalResult == mrOk && Fo_EditText->Ed_Text->Text != "") {
      BackupSub(MI_NewLinkLabel->Caption);

      TCardLabel *Label = m_Document->NewLabel(1);
      Label->m_Name = Fo_EditText->Ed_Text->Text;
      Label->m_nColor = GetColor(rand() % 10001 / 10000.0, 0, 255);

      TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
      Link->m_Labels->AddLabel(m_Document->m_Labels[1]->Count);
    }
    Fo_EditText->Release();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_NewLabelClick(TObject *Sender) {
  if (m_nTargetCard >= 0) {
    Fo_EditText = new TFo_EditText(this);
    Fo_EditText->La_Text->Caption = MLText.LabelName + ":";
    Fo_EditText->Caption = DeleteActionKey(MI_NewLabel->Caption);
    Fo_EditText->ShowModal();
    if (Fo_EditText->ModalResult == mrOk && Fo_EditText->Ed_Text->Text != "") {
      BackupSub(MI_NewLabel->Caption);

      TCardLabel *Label = m_Document->NewLabel(0);
      Label->m_Name = Fo_EditText->Ed_Text->Text;
      Label->m_nColor = GetColor(rand() % 10001 / 10000.0, 0, 255);

      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        TCard *Card = m_Document->GetCardByIndex(i);
        if (Card->m_bSelected) {
          Card->m_Labels->AddLabel(m_Document->m_Labels[0]->Count);
        }
      }
    }
    Fo_EditText->Release();
  }
}

// ---------------------------------------------------------------------------
// Label and link display (split from fomain.cpp)

#include <vcl.h>
#pragma hdrstop

#include "fomain.h"
#include "fomain_label.h"
#include "setting.h"
#include "utils.h"

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void TFo_Main::FreeMILabels() {
  for (int i = 0; i < MI_Labels->Count; i++) {
    delete (TMenuItem *)(MI_Labels->Items[i]);
  }
  MI_Labels->Clear();

  for (int i = 0; i < MI_LinkLabels->Count; i++) {
    delete (TMenuItem *)(MI_LinkLabels->Items[i]);
  }
  MI_LinkLabels->Clear();
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshLabel() {
  if (m_nTargetCard < 0) {
    for (int i = 0; i < MAXLABELS; i++) {
      Bu_Label[i]->Visible = false;
    }
  } else {
    TCard *Card = m_Document->GetCard(m_nTargetCard);
    if (Card)
      for (int i = 0; i < MAXLABELS; i++) {
        if (i < Card->m_Labels->Count) {
          Bu_Label[i]->Caption =
              m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(i) - 1)
                  ->m_Name;
          Bu_Label[i]->Visible = true;
        } else if (i == Card->m_Labels->Count) {
          Bu_Label[i]->Caption = "";
          Bu_Label[i]->Visible = true;
        } else {
          Bu_Label[i]->Visible = false;
        }
      }
  }

  if (m_nTargetLink < 0) {
    for (int i = 0; i < MAXLABELS; i++) {
      Bu_LinkLabel[i]->Visible = false;
    }
  } else {
    TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
    if (Link)
      for (int i = 0; i < MAXLABELS; i++) {
        if (i < Link->m_Labels->Count) {
          Bu_LinkLabel[i]->Caption =
              m_Document->GetLabelByIndex(1, Link->m_Labels->GetLabel(i) - 1)
                  ->m_Name;
          Bu_LinkLabel[i]->Visible = true;
        } else if (i == Link->m_Labels->Count) {
          Bu_LinkLabel[i]->Caption = "";
          Bu_LinkLabel[i]->Visible = true;
        } else {
          Bu_LinkLabel[i]->Visible = false;
        }
      }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshLaStatus() {
  if (m_nCurrentCard >= 0) {
    TCard *Card = m_Document->GetCard(m_nCurrentCard);

    if (Card) {
      UnicodeString S = "";

      int ccount = 0;
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        if (m_Document->GetCardByIndex(i)->m_bSelected) {
          ccount++;
        }
      }
      if (ccount > 1) {
        S += IntToStr(ccount) + UnicodeString(" ") + MLText.Cards +
             UnicodeString("  ");
      }

      S += IntToStr(Card->m_Labels->Count) + UnicodeString(" ") +
           MLText.Labels + UnicodeString("  ");

      int lcount = 0;
      for (int il = 0; il < m_Document->m_Links->Count; il++) {
        TLink *Link = m_Document->GetLinkByIndex(il);
        if (Link->m_nFromID == Card->m_nID || Link->m_nDestID == Card->m_nID) {
          lcount++;
        }
      }
      S += IntToStr(lcount) + UnicodeString(" ") + MLText.Links +
           UnicodeString("  ");

      int line = RE_Edit->Perform(EM_LINEFROMCHAR, -1, (NativeInt)0);
      S += UnicodeString("X : ") +
           IntToStr(RE_Edit->SelStart -
                    RE_Edit->Perform(EM_LINEINDEX, line, (NativeInt)0) + 1) +
           UnicodeString("  ");

      S += MLText.Line + UnicodeString(" : ") + IntToStr(line + 1) +
           UnicodeString("/") + IntToStr(Card->m_Lines->Count) +
           UnicodeString("  ");
      S += MLText.Len + UnicodeString(" : ") +
           IntToStr(WideString(RE_Edit->Text)
                        .SubString(1, RE_Edit->SelStart + 1)
                        .Length() -
                    line * 2) +
           UnicodeString("/") +
           IntToStr(WideString(Card->m_Lines->Text).Length() -
                    Card->m_Lines->Count * 2) +
           UnicodeString("  ");
      S += MLText.Size + UnicodeString(" : ") +
           SizeToStr(Card->m_Lines->Text.Length()) + UnicodeString("  ");

      SB_Status->Panels->Items[0]->Text = S;

      S = "";
      S += MLText.Created + UnicodeString(" : ") +
           DateTimeToStr(Card->m_fCreated) + UnicodeString("  ");
      S += MLText.Edited + UnicodeString(" : ") +
           DateTimeToStr(Card->m_fUpdated) + UnicodeString("  ");
      S += MLText.Viewed + UnicodeString(" : ") +
           DateTimeToStr(Card->m_fViewed) + UnicodeString("  ");

      SB_Status->Panels->Items[1]->Text = S;
    }
  } else {
    SB_Status->Panels->Items[0]->Text = "";
    SB_Status->Panels->Items[1]->Text = "";
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshLinks() {
  LB_Link->Items->BeginUpdate();
  LB_Link->Clear();
  m_LinkIndexes->Clear();
  TCard *Card = m_Document->GetCard(m_nCurrentCard);
  if (Card) {
    for (int i = 0; i < m_Document->m_Links->Count; i++) {
      TLink *Link = m_Document->GetLinkByIndex(i);
      TCard *Card2 = NULL;
      UnicodeString SDirection;
      if (Link->m_nFromID == m_nCurrentCard) {
        Card2 = m_Document->GetCard(Link->m_nDestID);
        SDirection = "-> ";
      } else if (Link->m_nDestID == m_nCurrentCard) {
        Card2 = m_Document->GetCard(Link->m_nFromID);
        SDirection = "<- ";
      }
      if (Link->m_bDirection == false) {
        SDirection = "<> ";
      }
      if (Card2) {
        UnicodeString S = DecodeES(Card2->m_Title, " ");

        if (Card2->m_Labels->Count) {
          UnicodeString S2 = UnicodeString("[");
          for (int i2 = 0; i2 < Card2->m_Labels->Count; i2++) {
            if (i2 > 0) {
              S2 += ", ";
            }
            S2 += m_Document
                      ->GetLabelByIndex(0, Card2->m_Labels->GetLabel(i2) - 1)
                      ->m_Name;
          }
          S2 += UnicodeString("] ");
          S = S2 + S;
        }

        if (Link->m_bDirection) {
          S = SDirection + S;
        }

        if (Link->m_Name != "") {
          S += UnicodeString(" (") + Link->m_Name + UnicodeString(")");
        }
        LB_Link->Items->Add(S);
        m_LinkIndexes->Add((void *)(intptr_t)i);
      }
    }
  }
  LB_Link->Items->EndUpdate();
}

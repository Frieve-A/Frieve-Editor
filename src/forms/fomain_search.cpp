// ---------------------------------------------------------------------------
// Search and replace (split from fomain.cpp)

#include <vcl.h>
#pragma hdrstop

#include <cwchar>
#include <System.NetEncoding.hpp>
#include "fomain.h"
#include "fomain_search.h"
#include "foabout.h"
#include "foedittext.h"
#include "fosearch.h"
#include "setting.h"

#pragma package(smart_init)

namespace {
UnicodeString NormalizeWebSearchUrl(const UnicodeString &url) {
  UnicodeString normalized = Trim(url);
  if (normalized.IsEmpty()) {
    return normalized;
  }
  if (normalized.Pos("://") == 0) {
    if (normalized.Pos("//") == 1) {
      normalized = "https:" + normalized;
    } else {
      normalized = "https://" + normalized;
    }
  }
  return normalized;
}

int FindForwardMatch(const UnicodeString &target, const UnicodeString &key,
                     int startPos) {
  const int targetLength = target.Length();
  const int keyLength = key.Length();
  if (keyLength <= 0 || startPos < 0 || startPos > targetLength - keyLength) {
    return -1;
  }

  const wchar_t *targetPtr = target.c_str();
  const wchar_t *keyPtr = key.c_str();
  for (int i = startPos; i <= targetLength - keyLength; i++) {
    if (std::wmemcmp(targetPtr + i, keyPtr, keyLength) == 0) {
      return i + 1;
    }
  }

  return -1;
}

int FindBackwardMatch(const UnicodeString &target, const UnicodeString &key,
                      int endPos) {
  const int targetLength = target.Length();
  const int keyLength = key.Length();
  if (keyLength <= 0 || targetLength < keyLength) {
    return -1;
  }

  int lastStart = endPos - keyLength;
  if (lastStart > targetLength - keyLength) {
    lastStart = targetLength - keyLength;
  }
  if (lastStart < 0) {
    return -1;
  }

  const wchar_t *targetPtr = target.c_str();
  const wchar_t *keyPtr = key.c_str();
  for (int i = lastStart; i >= 0; i--) {
    if (std::wmemcmp(targetPtr + i, keyPtr, keyLength) == 0) {
      return i + 1;
    }
  }

  return -1;
}
} // namespace


void TFo_Main::GlobalSearch(int SearchRequest) {
  // Search m_GlobalSearchResult
  m_GlobalSearchResult->Clear();

  if (m_Document->m_Cards->Count == 0) {
    // Card loop
    return;
  }

  Screen->Cursor = crHourGlass;

  for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
    TCard *Card = m_Document->GetCardByIndex(ic);
    int foundat = 0;
    if (!Card->m_bVisible && !(SearchRequest & 0x20)) {
      // Display card search
    } else {
      if (SearchRequest & 0x1) {
        // Title search
        if (SearchRequest & 0x10) {
          // Case-sensitive search
          WideString Target = WideLowerCase(WideString(Card->m_Title));
          foundat = Target.Pos(WideLowerCase(SearchKeyword));
        } else {
          // Case-sensitive search
          foundat = WideString(Card->m_Title).Pos(SearchKeyword);
        }
      }
      if (foundat == 0 && SearchRequest & 0x2) {
        // Body search
        if (SearchRequest & 0x10) {
          WideString Target = WideLowerCase(WideString(Card->m_Lines->Text));
          foundat = Target.Pos(WideLowerCase(SearchKeyword));
        } else {
          foundat = WideString(Card->m_Lines->Text).Pos(SearchKeyword);
        }
      }
    }

    if (foundat > 0) {
      // Found
      m_GlobalSearchResult->Add((void *)(intptr_t)Card->m_nID);
    }
  }

  Screen->Cursor = crDefault;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::ME_FindClick(TObject *Sender) {
  WideString SelText = GetSelText();
  if (SelText.Length()) {
    SearchKeyword = SelText;
  }
  Ed_Title->SelLength = 0;
  RE_Edit->SelLength = 0;
  Fo_Search->bReplace = false;
  Fo_Search->Show();
  Fo_Search->Ch_Backward->Checked = Sender == (TObject *)ME_FindPrevious;
  Fo_Search->Ch_GlobalSearch->Checked =
      Sender != (TObject *)ME_FindNext && Sender != (TObject *)ME_FindPrevious;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_FindNextClick(TObject *Sender) {
  if (SearchKeyword != WideString("")) {
    if (PC_Client->ActivePage == TS_Browser) {
      bSearchRequest = (bLastSearchRequest & 0x30) | 0x01;
    } else {
      bSearchRequest = (bLastSearchRequest & 0x33);
      if (!(bSearchRequest & 0x03)) {
        bSearchRequest |= 0x03;
      }
    }
  } else {
    if (bLastSearchRequest & 0x100) {
      ME_ReplaceClick(Sender);
    } else {
      ME_FindClick(Sender);
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_FindPreviousClick(TObject *Sender) {
  if (SearchKeyword != WideString("")) {
    if (PC_Client->ActivePage == TS_Browser) {
      bSearchRequest = (bLastSearchRequest & 0x30) | 0x81;
    } else {
      bSearchRequest = (bLastSearchRequest & 0x33) | 0x80;
      if (!(bSearchRequest & 0x03)) {
        bSearchRequest |= 0x03;
      }
    }
  } else {
    ME_FindClick(Sender);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_ReplaceClick(TObject *Sender) {
  WideString SelText = GetSelText();
  if (SelText.Length()) {
    SearchKeyword = SelText;
  }
  Ed_Title->SelLength = 0;
  RE_Edit->SelLength = 0;
  Fo_Search->bReplace = true;
  Fo_Search->Show();
  Fo_Search->Ch_Backward->Checked = Sender == (TObject *)ME_FindPrevious;
  Fo_Search->Ch_GlobalSearch->Checked =
      Sender != (TObject *)ME_FindNext && Sender != (TObject *)ME_FindPrevious;
}
// ---------------------------------------------------------------------------

int TFo_Main::ReplaceAll(int SearchRequest) {
  if (m_Document->m_Cards->Count == 0) {
    // No cards
    return false;
  }

  Screen->Cursor = crHourGlass;

  int Replaced = 0;
  bool Backup = false;

  for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
    TCard *Card = m_Document->GetCardByIndex(ic);
    int foundat;
    if (!Card->m_bVisible && !(SearchRequest & 0x20)) {
      // Hidden and hidden cards not in search target
    } else {
      if (SearchRequest & 0x1) {
        int lastpos = 1;
        // Search from title
        WideString S = Card->m_Title;
        while (true) {
          if (SearchRequest & 0x10) {
            // Case-insensitive
            WideString Target = WideLowerCase(S);
            Target = Target.SubString(lastpos, Target.Length());
            foundat = Target.Pos(WideLowerCase(SearchKeyword)) + lastpos - 1;
          } else {
            // Case-sensitive
            WideString Target = S.SubString(lastpos, S.Length());
            foundat = Target.Pos(SearchKeyword) + lastpos - 1;
          }
          if (foundat < lastpos) {
            break;
          } else {
            S = S.SubString(1, foundat - 1) + ReplaceKeyword +
                S.SubString(foundat + SearchKeyword.Length(), S.Length());
            lastpos = foundat + ReplaceKeyword.Length();
            Replaced++;
          }
        }
        if (S != Card->m_Title) {
          if (!Backup) {
            BackupSub(ME_Replace->Caption);
            Backup = true;
          }
          m_Document->SetCardTitle(Card, UnicodeString(S).c_str());
        }
      }
      if (SearchRequest & 0x2) {
        int lastpos = 1;
        // Body search
        WideString S = Card->m_Lines->Text;
        while (true) {
          if (SearchRequest & 0x10) {
            WideString Target = WideLowerCase(S);
            Target = Target.SubString(lastpos, Target.Length());
            foundat = Target.Pos(WideLowerCase(SearchKeyword)) + lastpos - 1;
          } else {
            WideString Target = S.SubString(lastpos, S.Length());
            foundat = Target.Pos(SearchKeyword) + lastpos - 1;
          }
          if (foundat < lastpos) {
            break;
          } else {
            S = S.SubString(1, foundat - 1) + ReplaceKeyword +
                S.SubString(foundat + SearchKeyword.Length(), S.Length());
            lastpos = foundat + ReplaceKeyword.Length();
            Replaced++;
          }
        }
        if (S != Card->m_Lines->Text) {
          if (!Backup) {
            BackupSub(ME_Replace->Caption);
            Backup = true;
          }
          m_Document->SetCardText(Card, UnicodeString(S).c_str());
        }
      }
    }
  }

  if (Replaced) {
    m_Document->RefreshList();
  }
  Screen->Cursor = crDefault;

  return Replaced;
}
// ---------------------------------------------------------------------------
void TFo_Main::Search(int SearchRequest) {
  if (m_Document->m_Cards->Count == 0) {
    // Card loop
    return;
  }

  m_bSearching = true;

  Screen->Cursor = crHourGlass;

  // Search dialog focus
  SetFocus();

  int targetcard = m_nTargetCard;
  if (targetcard < 0) {
    // No card selected

    // Select card
    targetcard = m_Document->GetCardByIndex(0)->m_nID;
  }

  // Search start
  int StartTarget = 0;
  int StartPos = 0;
  if (RE_Edit->Focused()) {
    // Text edit
    StartTarget = 1;
    if ((SearchRequest & 0x100) &&
        ((GetSelText() == SearchKeyword) ||
         ((SearchRequest & 0x10) &&
          (WideLowerCase(GetSelText()) == WideLowerCase(SearchKeyword))))) {
      // Double-click
      BackupSub(ME_Replace->Caption);
      WideString S = RE_Edit->Text;
      int selstartbak = RE_Edit->SelStart;
      RE_Edit->Text =
          S.SubString(1, RE_Edit->SelStart) + ReplaceKeyword +
          S.SubString(1 + RE_Edit->SelStart + RE_Edit->SelLength, S.Length());
      RE_Edit->SelStart = selstartbak;
      RE_Edit->SelLength = ReplaceKeyword.Length();
    }
    if (SearchRequest & 0x80) {
      // Updatesearch
      StartPos = RE_Edit->SelStart;
      /*
       if (RE_Edit->SelLength){
       StartPos = RE_Edit->SelStart;
       }else{
       StartPos = (RE_Edit->Text).Length();
       }
       */
    } else {
      // Start search
      StartPos = RE_Edit->SelStart + RE_Edit->SelLength;
      /*
       if (RE_Edit->SelLength){
       StartPos = RE_Edit->SelStart + RE_Edit->SelLength;
       }else{
       StartPos = 0;
       }
       */
    }
  } else if (m_nTargetCard >= 0) {
    // cardSelected
    StartTarget = 0;
    if ((SearchRequest & 0x100) &&
        ((GetSelText() == SearchKeyword) ||
         ((SearchRequest & 0x10) &&
          (WideLowerCase(GetSelText()) == WideLowerCase(SearchKeyword))))) {
      // Double-click
      BackupSub(ME_Replace->Caption);
      WideString S = Ed_Title->Text;
      int selstartbak = Ed_Title->SelStart;
      Ed_Title->Text =
          S.SubString(1, Ed_Title->SelStart) + ReplaceKeyword +
          S.SubString(1 + Ed_Title->SelStart + Ed_Title->SelLength, S.Length());
      Ed_Title->SelStart = selstartbak;
      Ed_Title->SelLength = ReplaceKeyword.Length();
    }
    if (SearchRequest & 0x80) {
      // Updatesearch
      StartPos = Ed_Title->SelStart;
      /*
       if (Ed_Title->SelLength){
       StartPos = Ed_Title->SelStart;
       }else{
       StartPos = WideString(Ed_Title->Text).Length();
       }
       */
    } else {
      StartPos = Ed_Title->SelStart + Ed_Title->SelLength;
      /*
       // Start search
       if (Ed_Title->SelLength){
       StartPos = Ed_Title->SelStart + Ed_Title->SelLength;
       }else{
       StartPos = 0;
       }
       */
    }
  }

  // Whether found or not
  bool Found = false;
  bool NotFound = false; // Not found after one full round

  // Start card, target, etc.
  int CardIndex = m_Document->SearchCardIndex(targetcard);
  int Target = StartTarget;
  int Pos = StartPos;
  // Search start card
  int StartCard = CardIndex;

  UnicodeString Key;
  if (SearchRequest & 0x10) {
    // Case-insensitive key
    Key = WideLowerCase(SearchKeyword);
  } else {
    // Case-sensitive key
    Key = SearchKeyword;
  }

  bool resetpos = false;

  while (true) {
    TCard *Card = m_Document->GetCardByIndex(CardIndex);
    // Target search
    int foundat = -1;
    if (!Card->m_bVisible && !(SearchRequest & 0x20)) {
      // Display card search
    } else if (Target == 0) {
      if (SearchRequest & 0x1) {
        // Title search
        UnicodeString Target;
        if (SearchRequest & 0x10) {
          // Case-sensitive search
          Target = WideLowerCase(Card->m_Title);
        } else {
          // Case-sensitive search
          Target = Card->m_Title;
        }
        if (SearchRequest & 0x80) {
          // Updatesearch
          if (resetpos) {
            Pos = Target.Length();
          }
          foundat = FindBackwardMatch(Target, Key, Pos);
        } else {
          // Start search
          if (resetpos) {
            Pos = 0;
          }
          foundat = FindForwardMatch(Target, Key, Pos);
        }
        if (foundat >= 0) {
          // Update?
          m_Document->ClearCardSelection();
          m_nTargetCard = Card->m_nID;
          Card->m_bSelected = true;
          m_nTargetLink = -1;
          Ti_CheckTimer(this);

          Ed_Title->SetFocus();
          Ed_Title->SelStart = foundat - 1;
          Ed_Title->SelLength = SearchKeyword.Length();
          RE_Edit->SelLength = 0;
          Found = true;
          break;
        }
      }
    } else {
      if (SearchRequest & 0x2) {
        // Body search
        UnicodeString Target;
        if (SearchRequest & 0x10) {
          // Case-sensitive search
          Target = WideLowerCase(Card->m_Lines->Text);
        } else {
          // Case-sensitive search
          Target = WideString(Card->m_Lines->Text);
        }
        if (SearchRequest & 0x80) {
          // Updatesearch
          if (resetpos) {
            Pos = Target.Length();
          }
          foundat = FindBackwardMatch(Target, Key, Pos);
        } else {
          // Start search
          if (resetpos) {
            Pos = 0;
          }
          foundat = FindForwardMatch(Target, Key, Pos);
        }

        // RichEdit
        // int ToEnd = RE->Text.Length() - Pos;
        // foundat = RE->FindTextA(SearchKeyword, Pos, ToEnd, TSearchTypes() <<
        // stMatchCase);

        if (foundat >= 0) {
          // Update?
          m_Document->ClearCardSelection();
          m_nTargetCard = Card->m_nID;
          Card->m_bSelected = true;
          m_nTargetLink = -1;
          if (PC_Client->ActivePage == TS_Browser) {
            if (RE_Edit->Parent != TS_Browser) {
              PC_Client->ActivePage = TS_Editor;
            }
          }
          Ti_CheckTimer(this);

          RE_Edit->SetFocus();
          RE_Edit->SelStart = foundat - 1;
          RE_Edit->SelLength = SearchKeyword.Length();
          // UnicodeString(SearchKeyword).Length();
          Ed_Title->SelLength = 0;
          Found = true;
          break;
        }
      }
    }

    // Wrap card/target
    resetpos = true;
    Target = 1 - Target;
    if (Target == 1 && (SearchRequest & 0x80)) {
      // Wrap
      // Select card
      CardIndex = (CardIndex + m_Document->m_Cards->Count - 1) %
                  m_Document->m_Cards->Count;
    } else if (Target == 0 && !(SearchRequest & 0x80)) {
      // Wrap
      // Select card
      CardIndex = (CardIndex + 1) % m_Document->m_Cards->Count;
    }

    // All search complete
    if (NotFound) {
      break;
    }
    if (CardIndex == StartCard && Target == StartTarget) {
      // Not found through entire search
      NotFound = true;
    }
  }

  Screen->Cursor = crDefault;

  if (!Found) {
    ShowMessage(UnicodeString("\"") + SearchKeyword + UnicodeString("\"") +
                MLText.NotFound);
  }

  // Update end
  /*
   LB_List->Perform(WM_SETREDRAW, 1, 0);
   Ed_Title->Perform(WM_SETREDRAW, 1, 0);
   Co_Label->Perform(WM_SETREDRAW, 1, 0);
   RE_Edit->Perform(WM_SETREDRAW, 1, 0);
   LB_Link->Perform(WM_SETREDRAW, 1, 0);
   */
  /*
   Perform(WM_SETREDRAW, 1, 0);

   // Drawing
   // Invalidate(); // Redraw
   Ed_Title->Invalidate();
   RefreshLabel();
   if (PC_Client->ActivePage == TS_Editor){
   LB_List->Invalidate();
   RE_Edit->Invalidate();
   LB_Link->Invalidate();
   }else if (PC_Client->ActivePage == TS_Browser){
   PB_Browser->Invalidate();
   }
   */

  m_bSearching = false;
}

// ---------------------------------------------------------------------------
WideString TFo_Main::GetSelText() {
  // Selected text

  WideString Key;
  int SelStart = 0, SelLength = 0;
  if (Ed_FindCard->Focused()) {
    Key = Ed_FindCard->Text;
    SelStart = Ed_FindCard->SelStart;
    SelLength = Ed_FindCard->SelLength;
  } else if (RE_Edit->Focused()) {
    Key = RE_Edit->Text;
    SelStart = RE_Edit->SelStart;
    SelLength = RE_Edit->SelLength;
  } else if (Me_EditorRelated->Focused()) {
    Key = Me_EditorRelated->Text;
    SelStart = Me_EditorRelated->SelStart;
    SelLength = Me_EditorRelated->SelLength;
  } else if (Ed_Title->Focused()) {
    Key = Ed_Title->Text;
    SelStart = Ed_Title->SelStart;
    SelLength = Ed_Title->SelLength;
  } else if (Ed_TitleB->Focused()) {
    Key = Ed_TitleB->Text;
    SelStart = Ed_TitleB->SelStart;
    SelLength = Ed_TitleB->SelLength;
  } else if (Ed_LinkTitle->Focused()) {
    Key = Ed_LinkTitle->Text;
    SelStart = Ed_LinkTitle->SelStart;
    SelLength = Ed_LinkTitle->SelLength;
  } else if (m_nTargetCard >= 0) {
    TCard *Card = m_Document->GetCard(m_nTargetCard);
    if (Card) {
      Key = Card->m_Title;
      SelStart = 0;
      SelLength = Key.Length();
    }
  } else if (m_nTargetLink >= 0) {
    TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
    if (Link) {
      Key = Link->m_Name;
      SelStart = 0;
      SelLength = Key.Length();
    }
  }

  Key = Key.SubString(SelStart + 1, SelLength);

  return Key;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::ME_WebSearchClick(TObject *Sender) {
  ExitFullScreen();

  TMenuItem *MI = (TMenuItem *)Sender;

  WideString Key = Trim(GetSelText());
  UnicodeString SearchName = Setting2Function.m_WebSearch->Names[MI->Tag];

  UnicodeString BaseUrl = Setting2Function.m_WebSearch->Values[SearchName];
  UnicodeString Url = NormalizeWebSearchUrl(BaseUrl);
  if (Url.IsEmpty()) {
    return;
  }
  UnicodeString EncodedKey = TNetEncoding::URL->Encode(UnicodeString(Key));
  Url += EncodedKey;

  UnicodeString Params =
      UnicodeString("url.dll,FileProtocolHandler \"") + Url + UnicodeString("\"");
  ShellExecute(Handle, L"open", L"rundll32.exe", Params.c_str(), NULL, SW_SHOW);

  // Status bar update
  SettingFile.m_WebSearch = SearchName;
  for (int i = 0; i < Setting2Function.m_WebSearch->Count; i++) {
    TMenuItem *MI = (TMenuItem *)MI_WebSearch->Items[i];
    if (SettingFile.m_WebSearch ==
        Setting2Function.m_WebSearch->Names[MI->Tag]) {
      MI->ShortCut = TextToShortCut("Ctrl+W");
    } else {
      MI->ShortCut = 0;
    }
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MH_AboutClick(TObject *Sender) {
  Fo_About = new TFo_About(this);
  Fo_About->ShowModal();
  Fo_About->Release();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MDS_TitleClick(TObject *Sender) {
  BackupSub(MD_SortCard->Caption + UnicodeString(" ") +
            ((TMenuItem *)Sender)->Caption);
  m_Document->SortCards(0, ((TMenuItem *)Sender)->Tag);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MDS_Date_CreatedClick(TObject *Sender) {
  BackupSub(MD_SortCard->Caption + UnicodeString(" ") +
            ((TMenuItem *)Sender)->Caption);
  m_Document->SortCards(1, ((TMenuItem *)Sender)->Tag);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MDS_Date_EditedClick(TObject *Sender) {
  BackupSub(MD_SortCard->Caption + UnicodeString(" ") +
            ((TMenuItem *)Sender)->Caption);
  m_Document->SortCards(2, ((TMenuItem *)Sender)->Tag);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MDS_Date_ViewedClick(TObject *Sender) {
  BackupSub(MD_SortCard->Caption + UnicodeString(" ") +
            ((TMenuItem *)Sender)->Caption);
  m_Document->SortCards(3, ((TMenuItem *)Sender)->Tag);
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// File I/O operations (split from fomain.cpp)

#include <vcl.h>
#pragma hdrstop

#include "fomain.h"
#include "fomain_file.h"
#include "setting.h"
#include "utils.h"

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void TFo_Main::BackupSub(UnicodeString Action) {
  if (!m_bDoNotBackup) {
    m_UndoRedo->Backup(m_Document, DeleteActionKey(Action).c_str());
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshRecent(UnicodeString FN) {
  // History update
  if (FN != "" &&
      FN != (ExtractFilePath(ParamStr(0)) + UnicodeString("help.fip"))) {
    // File name
    for (int i = 0; i < 10; i++) {
      if (SettingFile.m_RecentFiles[i] == FN) {
        // Already in list
        if (i > 0) {
          for (int i2 = i; i2 >= 1; i2--) {
            SettingFile.m_RecentFiles[i2] = SettingFile.m_RecentFiles[i2 - 1];
          }
        }
        SettingFile.m_RecentFiles[0] = FN;
        break;
      }
    }

    if (SettingFile.m_RecentFiles[0] != FN) {
      // No duplicate
      for (int i = 9; i >= 1; i--) {
        SettingFile.m_RecentFiles[i] = SettingFile.m_RecentFiles[i - 1];
      }
      SettingFile.m_RecentFiles[0] = FN;
    }

    FN = ExtractFilePath(FN);

    // Folder name
    for (int i = 0; i < 10; i++) {
      if (SettingFile.m_RecentFolders[i] == FN) {
        // Already in list
        if (i > 0) {
          for (int i2 = i; i2 >= 1; i2--) {
            SettingFile.m_RecentFolders[i2] =
                SettingFile.m_RecentFolders[i2 - 1];
          }
        }
        SettingFile.m_RecentFolders[0] = FN;
        break;
      }
    }

    if (SettingFile.m_RecentFolders[0] != FN) {
      // No duplicate
      for (int i = 9; i >= 1; i--) {
        SettingFile.m_RecentFolders[i] =
            SettingFile.m_RecentFolders[i - 1];
      }
      SettingFile.m_RecentFolders[0] = FN;
    }
  }

  // Menu update
  for (int i = 0; i < 10; i++) {
    if (SettingFile.m_RecentFiles[i] != "") {
      if (i < 9) {
        MI_RecentFiles[i]->Caption = UnicodeString("&") + IntToStr(i + 1) +
                                     " " + SettingFile.m_RecentFiles[i];
      } else {
        MI_RecentFiles[i]->Caption =
            UnicodeString("1&0 ") + SettingFile.m_RecentFiles[i];
      }
      MI_RecentFiles[i]->Visible = true;
    } else {
      MI_RecentFiles[i]->Visible = false;
    }

    if (SettingFile.m_RecentFolders[i] != "") {
      if (i < 9) {
        MI_RecentFolders[i]->Caption = UnicodeString("&") + IntToStr(i + 1) +
                                       " " + SettingFile.m_RecentFolders[i] +
                                       "...";
      } else {
        MI_RecentFolders[i]->Caption =
            UnicodeString("1&0 ") + SettingFile.m_RecentFolders[i] + "...";
      }
      MI_RecentFolders[i]->Visible = true;
    } else {
      MI_RecentFolders[i]->Visible = false;
    }
  }
}

// ---------------------------------------------------------------------------
bool TFo_Main::SaveCheck() {
  if (m_Document->m_bChanged && !m_Document->m_bReadOnly) {
    int result = Application->MessageBox(
        UnicodeString(MLText.SaveCheck).c_str(),
        UnicodeString(AppTitle).c_str(), MB_YESNOCANCEL);
    switch (result) {
    case ID_YES:
      if (Save()) {
        return true;
      } else {
        return false;
      }
    case ID_NO:
      return true;
    case ID_CANCEL:
      return false;
    }
  }
  return true;
}

// ---------------------------------------------------------------------------
bool TFo_Main::Save() {
  if (m_Document->m_bReadOnly) {
    return false;
  }

  if (m_Document->m_FN == "") {
    return SaveAs();
  }

  TCursor crbak = Screen->Cursor;
  Screen->Cursor = crHourGlass;

  m_Document->m_nCardID = m_nTargetCard;
  Ti_Check->Enabled = false;
  Application->ProcessMessages();

  // Apply display settings
  fReqZoom = TB_Zoom->Position / 2000.0;
  fReqX = Sc_X->Position / 10000.0;
  fReqY = Sc_Y->Position / 10000.0;
  bReqArrange = SB_Arrange->Down;
  nReqArrangeMode = Bu_ArrangeType->Tag;
  bReqAutoScroll = SB_AutoScroll->Down;
  bReqAutoZoom = SB_AutoZoom->Down;
  bReqSizeLimitation = SettingView.m_bSizeLimitation;
  bReqLinkLimitation = SettingView.m_bLinkLimitation;
  bReqDateLimitation = SettingView.m_bDateLimitation;
  nReqSizeLimitation = SettingView.m_nSizeLimitation;
  nReqLinkLimitation = SettingView.m_nLinkLimitation;
  bReqLinkDirection = SettingView.m_bLinkDirection;
  bReqLinkBackward = SettingView.m_bLinkBackward;
  nReqLinkTarget = SettingView.m_nLinkTarget;
  nReqDateLimitation = SettingView.m_nDateLimitation;
  ReqDateLimitationDateType = SettingView.m_DateLimitationDateType;
  ReqDateLimitationType = SettingView.m_DateLimitationType;

  bool result = m_Document->Save();

  // Reset request
  fReqZoom = -1000.0f;
  fReqX = -1000.0f;
  fReqY = -1000.0f;
  bReqArrange = -1;
  nReqArrangeMode = -1;
  bReqAutoScroll = -1;
  bReqAutoZoom = -1;
  bReqSizeLimitation = -1;
  bReqLinkLimitation = -1;
  bReqDateLimitation = -1;
  nReqSizeLimitation = -1;
  nReqLinkLimitation = -1;
  bReqLinkDirection = -1;
  bReqLinkBackward = -1;
  nReqLinkTarget = -2;
  nReqDateLimitation = -1;
  ReqDateLimitationDateType = -1;
  ReqDateLimitationType = -1;

  if (!result) {
    ShowMessage(MLText.FailedToSave + m_Document->m_FN + UnicodeString("."));
  }
  Ti_Check->Enabled = true;
  if (result && m_bContinuousLoad) {
    m_nCLFileAge = FileAge(m_Document->m_FN);
  }

  RefreshRecent(m_Document->m_FN);
  RefreshFileList();

  Screen->Cursor = crbak;

  return result;
}

// ---------------------------------------------------------------------------
bool TFo_Main::SaveAs() {
  if (m_Document->m_bReadOnly) {
    return false;
  }

  if (SD->Execute()) {
    m_Document->m_FN = SD->FileName;
    return Save();
  } else {
    return false;
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::LoadSub(UnicodeString FN, bool bSoftLoad, bool bRefreshRecent) {
  SB_CloseGlobalSearchClick(this);

  TCursor crbak = Screen->Cursor;
  Screen->Cursor = crHourGlass;

  if (bRefreshRecent) {
    RefreshRecent(FN);
  }

  bool result = m_Document->Load(FN, bSoftLoad);

  if (result) {
    if (bSoftLoad) {
      m_nCurrentCard = -1;
      m_nTargetLink = -1;
      if (m_Document->m_Cards->Count) {
        if (m_Document->m_nCardID >= 0) {
          m_nTargetCard = m_Document->m_nCardID;
        } else {
          if (m_Document->SearchCardIndex(m_nTargetCard) < 0) {
            m_nTargetCard = -1;
          }
        }
      }
    } else {
      m_UndoRedo->ClearUndos();
      m_UndoRedo->ClearRedos();

      m_nCurrentCard = -2;
      m_nTargetCard = -1;
      SettingView.m_nLinkTarget = -1;
      SettingView.m_bNoLabelVisible = true;
      SettingView.m_bNoLinkLabelVisible = true;
      if (m_Document->m_Cards->Count) {
        if (m_Document->m_nCardID >= 0) {
          m_nTargetCard = m_Document->m_nCardID;
          TCard *Card = m_Document->GetCard(m_nTargetCard);
          if (Card) {
            Card->m_bSelected = true;
          }
        }
        m_nTargetLink = -1;
      }
    }

    SB_Cursor->Down = true;
    SB_Arrange->Down = false;

    m_bContinuousLoad =
        FN == ExtractFilePath(ParamStr(0)) + "_continuousload.fip";
    if (m_bContinuousLoad) {
      m_nCLFileAge = FileAge(FN);
    }

    RefreshFileList();
  }

  Screen->Cursor = crbak;
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshFileList() {
  TStringList *SL = new TStringList();
  FileListCreator(ExtractFileDir(SettingFile.m_RecentFolders[0] + "file"), SL,
                  ".fip", false);

  LB_FileList->Items->BeginUpdate();
  LB_FileList->Clear();
  int index = -1;
  for (int i = 0; i < SL->Count; i++) {
    LB_FileList->Items->Add(ExtractFileNameOnly(SL->Strings[i]));
    if (SL->Strings[i] == m_Document->m_FN) {
      index = i;
    }
  }
  LB_FileList->ItemIndex = index;
  LB_FileList->Items->EndUpdate();

  delete SL;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MF_RecentFilesClick(TObject *Sender) {
  if (SaveCheck()) {
    LoadSub(SettingFile.m_RecentFiles[((TMenuItem *)Sender)->Tag]);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MF_RecentFoldersClick(TObject *Sender) {
  OD->FileName =
      SettingFile.m_RecentFolders[((TMenuItem *)Sender)->Tag] + "*.fip";
  MF_OpenClick(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MF_NewClick(TObject *Sender) {
  if (SaveCheck()) {
    SB_CloseGlobalSearchClick(Sender);

    m_bContinuousLoad = false;
    delete m_Document;
    m_Document = new TDocument();
    m_UndoRedo->ClearUndos();
    m_UndoRedo->ClearRedos();
    m_nCurrentCard = -1;
    m_nTargetCard = 0;
    m_Document->GetCardByIndex(0)->m_bSelected = true;
    m_nTargetLink = -1;
    SettingView.m_nLinkTarget = -1;
    SettingView.m_bNoLabelVisible = true;
    SettingView.m_bNoLinkLabelVisible = true;

    SB_Cursor->Down = true;
    SB_Arrange->Down = false;

    RefreshFileList();
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MF_SaveClick(TObject *Sender) { Save(); }

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MF_SaveAsClick(TObject *Sender) { SaveAs(); }

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MF_OpenClick(TObject *Sender) {
  if (SaveCheck()) {
    OD->InitialDir = ExtractFileDir(m_Document->m_FN);
    if (OD->Execute()) {
      LoadSub(OD->FileName);
    }
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormCloseQuery(TObject *Sender, bool &CanClose) {
  CanClose = SaveCheck();
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MF_ExitClick(TObject *Sender) { Close(); }

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::LB_FileListClick(TObject *Sender) {
  if (!m_Document->m_bChanged) {
    LB_FileListDblClick(Sender);
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::LB_FileListDblClick(TObject *Sender) {
  if (LB_FileList->ItemIndex >= 0) {
    if (SaveCheck()) {
      LoadSub(ExtractFileDir(SettingFile.m_RecentFolders[0] + "file") +
                  UnicodeString("\\") +
                  LB_FileList->Items->Strings[LB_FileList->ItemIndex] +
                  UnicodeString(".fip"),
              false, false);
    }
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::LB_FileListMouseDown(TObject *Sender,
                                              TMouseButton Button,
                                              TShiftState Shift, int X, int Y) {
  m_bFileListDragging = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::LB_FileListMouseUp(TObject *Sender,
                                            TMouseButton Button,
                                            TShiftState Shift, int X, int Y) {
  m_bFileListDragging = false;
}

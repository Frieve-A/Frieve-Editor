// ---------------------------------------------------------------------------
// Settings (language, font, wallpaper) (split from fomain.cpp)

#include <vcl.h>
#pragma hdrstop

#include "fomain.h"
#include "fomain_settings.h"
#include "setting.h"
#include "utils.h"

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void TFo_Main::ApplyLanguageSetting() {
  if (!FileExists(LanguageFileName())) {
    return;
  }
  TFastIni *Ini = new TFastIni(LanguageFileName());

  // Main window
  TS_Browser->Caption = Ini->ReadString("MainWnd", "TS_Browser",
                                        TS_Browser->Caption); // Browser
  TS_Editor->Caption = Ini->ReadString("MainWnd", "TS_Editor",
                                       TS_Editor->Caption); // Editor
  TS_Drawing->Caption = Ini->ReadString("MainWnd", "TS_Drawing",
                                        TS_Drawing->Caption); // Drawing
  TS_Statistics->Caption =
      Ini->ReadString("MainWnd", "TS_Statistics",
                      TS_Statistics->Caption); // Statistics
  La_Files->Caption = Ini->ReadString("MainWnd", "La_Files",
                                      La_Files->Caption); // File
  La_Cards->Caption = Ini->ReadString("MainWnd", "La_Cards",
                                      La_Cards->Caption); // card
  La_Card->Caption = Ini->ReadString("MainWnd", "La_Card",
                                     La_Card->Caption); // card
  La_Title->Caption = Ini->ReadString("MainWnd", "La_Title",
                                      La_Title->Caption); // Name
  La_Size->Caption = Ini->ReadString("MainWnd", "La_Size",
                                     La_Size->Caption); // Size
  La_Label->Caption = Ini->ReadString("MainWnd", "La_Label",
                                      La_Label->Caption); // label
  La_Link->Caption = Ini->ReadString("MainWnd", "La_Link",
                                     La_Link->Caption); // link
  La_LinkTitle->Caption = Ini->ReadString("MainWnd", "La_Title",
                                          La_LinkTitle->Caption); // Name
  Ch_LinkDirection->Caption =
      Ini->ReadString("MainWnd", "Ch_LinkDirection",
                      Ch_LinkDirection->Caption); // Direction
  Bu_LinkDirectionInverse->Caption =
      Ini->ReadString("MainWnd", "Bu_LinkDirectionInverse",
                      Bu_LinkDirectionInverse->Caption); // Inverse
  La_LinkLabel->Caption = Ini->ReadString("MainWnd", "La_Label",
                                          La_LinkLabel->Caption); // label
  La_Arrange->Caption = Ini->ReadString("MainWnd", "La_Arrange",
                                        La_Arrange->Caption); // Auto arrange
  La_Zoom->Caption = Ini->ReadString("MainWnd", "La_Zoom",
                                     La_Zoom->Caption); // Zoom
  Bu_Shuffle->Caption = Ini->ReadString("MainWnd", "Bu_Shuffle",
                                        Bu_Shuffle->Caption); // Shuffle
  SB_AutoScroll->Caption =
      Ini->ReadString("MainWnd", "SB_AutoScroll",
                      SB_AutoScroll->Caption); // Auto scroll
  SB_AutoZoom->Caption = Ini->ReadString("MainWnd", "SB_AutoZoom",
                                         SB_AutoZoom->Caption); // Auto zoom
  Bu_FindCard->Caption = Ini->ReadString("MainWnd", "Bu_FindCard",
                                         Bu_FindCard->Caption); // search
  SB_EditorRelated->Caption =
      Ini->ReadString("MainWnd", "SB_EditorRelated",
                      SB_EditorRelated->Caption); // Related text
  La_PenColor->Caption = Ini->ReadString("MainWnd", "La_PenColor",
                                         La_PenColor->Caption); // Color:
  La_PenColorDefault->Caption =
      Ini->ReadString("MainWnd", "La_PenColorDefault",
                      La_PenColorDefault->Caption); // Auto
  La_DZoom->Caption = Ini->ReadString("MainWnd", "La_Zoom",
                                      La_DZoom->Caption); // Zoom
  La_StatisticsKey->Caption = Ini->ReadString("MainWnd", "La_StatisticsKey",
                                              La_StatisticsKey->Caption); // Key
  SB_StatisticsSort->Caption =
      Ini->ReadString("MainWnd", "SB_StatisticsSort",
                      SB_StatisticsSort->Caption); // Sort
  La_GlobalSearchTop->Caption = Ini->ReadString("MainWnd", "La_GlobalSearchTop",
                                                La_GlobalSearchTop->Caption);

  // Main menu
  M_File->Caption = Ini->ReadString("MainWnd", "M_File", M_File->Caption);
  MF_New->Caption = Ini->ReadString("MainWnd", "MF_New", MF_New->Caption);
  MF_Open->Caption = Ini->ReadString("MainWnd", "MF_Open",
                                     MF_Open->Caption); // Open(&O)...
  MF_Save->Caption = Ini->ReadString("MainWnd", "MF_Save",
                                     MF_Save->Caption); // Save(&S)
  MF_SaveAs->Caption = Ini->ReadString("MainWnd", "MF_SaveAs",
                                       MF_SaveAs->Caption); // Save As(&A)...
  MF_AutoSave->Caption =
      Ini->ReadString("MainWnd", "MF_AutoSave", MF_AutoSave->Caption);
  MF_AutoReload->Caption =
      Ini->ReadString("MainWnd", "MF_AutoReload", MF_AutoReload->Caption);
  MF_RecentFiles->Caption =
      Ini->ReadString("MainWnd", "MF_RecentFiles",
                      MF_RecentFiles->Caption); // Recent File(&R)
  MF_RecentFolders->Caption =
      Ini->ReadString("MainWnd", "MF_RecentFolders",
                      MF_RecentFolders->Caption); // Recent Folder(&C)
  MF_Import->Caption = Ini->ReadString("MainWnd", "MF_Import",
                                       MF_Import->Caption); // Import(&I)
  MFI_TxtFile->Caption =
      Ini->ReadString("MainWnd", "MFI_TxtFile",
                      MFI_TxtFile->Caption); // TextFile(&T)...
  MFI_HierarchicalTextFile->Caption = Ini->ReadString(
      "MainWnd", "MFI_HierarchicalTextFile", MFI_HierarchicalTextFile->Caption);
  MFI_HierarchicalTextFile2->Caption =
      Ini->ReadString("MainWnd", "MFI_HierarchicalTextFile2",
                      MFI_HierarchicalTextFile2->Caption);
  MFI_TxtFilesinaFolder->Caption = Ini->ReadString(
      "MainWnd", "MFI_TxtFilesinaFolder", MFI_TxtFilesinaFolder->Caption);
  MF_Export->Caption = Ini->ReadString("MainWnd", "MF_Export",
                                       MF_Export->Caption); // Export(&E)
  MFE_TxtFileCardTitle->Caption = Ini->ReadString(
      "MainWnd", "MFE_TxtFileCardTitle",
      MFE_TxtFileCardTitle->Caption); // TextFile(cardTitle)(&C)...
  MFE_TxtFile->Caption =
      Ini->ReadString("MainWnd", "MFE_TxtFile",
                      MFE_TxtFile->Caption); // TextFile(&T)...
  MFE_TxtFiles->Caption =
      Ini->ReadString("MainWnd", "MFE_TxtFiles",
                      MFE_TxtFiles->Caption); // All TextFile(&X)...
  MFE_HierarchicalTextFile->Caption = Ini->ReadString(
      "MainWnd", "MFE_HierarchicalTextFile", MFE_HierarchicalTextFile->Caption);
  MFE_HtmlFiles->Caption =
      Ini->ReadString("MainWnd", "MFE_HtmlFiles",
                      MFE_HtmlFiles->Caption); // HtmlFile(&H)...
  MFE_BMPFile->Caption =
      Ini->ReadString("MainWnd", "MFE_BMPFile", MFE_BMPFile->Caption);
  MFE_JPEGFile->Caption =
      Ini->ReadString("MainWnd", "MFE_JPEGFile", MFE_JPEGFile->Caption);
  MFE_ClipboardCardTitle->Caption = Ini->ReadString(
      "MainWnd", "MFE_ClipboardCardTitle", MFE_ClipboardCardTitle->Caption);
  MFE_ClipboardCardText->Caption = Ini->ReadString(
      "MainWnd", "MFE_ClipboardCardText", MFE_ClipboardCardText->Caption);
  MFE_ClipboardBMP->Caption =
      Ini->ReadString("MainWnd", "MFE_ClipboardBMP", MFE_ClipboardBMP->Caption);
  MF_PrintImage->Caption =
      Ini->ReadString("MainWnd", "MF_PrintImage",
                      MF_PrintImage->Caption); // Browser Display(&P)...
  MF_Exit->Caption = Ini->ReadString("MainWnd", "MF_Exit",
                                     MF_Exit->Caption); // Exit(&X)
  M_Edit->Caption = Ini->ReadString("MainWnd", "M_Edit", M_Edit->Caption);
  ME_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
                                     ME_Undo->Caption); // Undo(&U)
  ME_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo", ME_Redo->Caption);
  ME_Cut->Caption = Ini->ReadString("MainWnd", "ME_Cut", ME_Cut->Caption);
  ME_Copy->Caption = Ini->ReadString("MainWnd", "ME_Copy",
                                     ME_Copy->Caption); // Copy(&C)
  ME_Paste->Caption = Ini->ReadString("MainWnd", "ME_Paste",
                                      ME_Paste->Caption); // Paste(&P)
  ME_Delete->Caption = Ini->ReadString("MainWnd", "ME_Delete",
                                       ME_Delete->Caption); // delete(&D)
  ME_SelectAll->Caption =
      Ini->ReadString("MainWnd", "ME_SelectAll",
                      ME_SelectAll->Caption); // Select all(&A)
  ME_Find->Caption = Ini->ReadString("MainWnd", "ME_Find",
                                     ME_Find->Caption); // search(&F)
  ME_Replace->Caption = Ini->ReadString("MainWnd", "ME_Replace",
                                        ME_Replace->Caption); // (&E)
  ME_FindNext->Caption = Ini->ReadString("MainWnd", "ME_FindNext",
                                         ME_FindNext->Caption); // Find next(&N)
  ME_FindPrevious->Caption =
      Ini->ReadString("MainWnd", "ME_FindPrevious",
                      ME_FindPrevious->Caption); // Find previous(&V)
  ME_WebSearch->Caption = Ini->ReadString("MainWnd", "ME_WebSearch",
                                          ME_WebSearch->Caption); // W
  ME_ReadAloud->Caption = Ini->ReadString("MainWnd", "ME_ReadAloud",
                                          ME_ReadAloud->Caption);
  MD_SortCard->Caption = Ini->ReadString("MainWnd", "MD_SortCard",
                                         MD_SortCard->Caption); // Sort card(&S)
  MDS_Title->Caption = Ini->ReadString("MainWnd", "MDS_Title",
                                       MDS_Title->Caption); // Title(&T)
  MDS_Title_I->Caption =
      Ini->ReadString("MainWnd", "MDS_Title_I",
                      MDS_Title_I->Caption); // Title(inv)(&T)
  MDS_Date_Created->Caption =
      Ini->ReadString("MainWnd", "MDS_Date_Created",
                      MDS_Date_Created->Caption); // Created(&C)
  MDS_Date_Created_I->Caption =
      Ini->ReadString("MainWnd", "MDS_Date_Created_I",
                      MDS_Date_Created_I->Caption); // Created(inv)(&R)
  MDS_Date_Edited->Caption =
      Ini->ReadString("MainWnd", "MDS_Date_Edited",
                      MDS_Date_Edited->Caption); // Edited(&E)
  MDS_Date_Edited_I->Caption =
      Ini->ReadString("MainWnd", "MDS_Date_Edited_I",
                      MDS_Date_Edited_I->Caption); // Edited(inv)(&D)
  MDS_Date_Viewed->Caption =
      Ini->ReadString("MainWnd", "MDS_Date_Viewed",
                      MDS_Date_Viewed->Caption); // Viewed(&V)
  MDS_Date_Viewed_I->Caption =
      Ini->ReadString("MainWnd", "MDS_Date_Viewed_I",
                      MDS_Date_Viewed_I->Caption); // Viewed(inv)(&W)
  MDS_Score->Caption = Ini->ReadString("MainWnd", "MDS_Score",
                                       MDS_Score->Caption); // Score(&S)
  MDS_Score_I->Caption =
      Ini->ReadString("MainWnd", "MDS_Score_I",
                      MDS_Score_I->Caption); // Score(inv)(&O)
  MDS_Shuffle->Caption = Ini->ReadString("MainWnd", "MDS_Shuffle",
                                         MDS_Shuffle->Caption); // Shuffle(&H)
  ME_BatchConversion->Caption =
      Ini->ReadString("MainWnd", "ME_BatchConversion",
                      ME_BatchConversion->Caption); // Batch change(&B)
  MEC_AllCards->Caption =
      Ini->ReadString("MainWnd", "MEC_AllCards",
                      MEC_AllCards->Caption); // All Card shape(&C)
  MEC_AllLinks->Caption =
      Ini->ReadString("MainWnd", "MEC_AllLinks",
                      MEC_AllLinks->Caption); // All link label(&L)
  MEC_AllLinksDirection->Caption =
      Ini->ReadString("MainWnd", "MEC_AllLinksDirection",
                      MEC_AllLinksDirection->Caption); // All link direction(&D)
  ME_Label->Caption = Ini->ReadString("MainWnd", "ME_Label",
                                      ME_Label->Caption); // Label edit(&L)...
  ME_LinkLabel->Caption =
      Ini->ReadString("MainWnd", "ME_LinkLabel",
                      ME_LinkLabel->Caption); // Link label edit(&I)...
  M_Insert->Caption = Ini->ReadString("MainWnd", "M_Insert",
                                      M_Insert->Caption); // insert(&I)
  MI_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
                                        MI_NewCard->Caption); // New card(&C)
  MI_NewLink->Caption = Ini->ReadString("MainWnd", "MI_NewLink",
                                        MI_NewLink->Caption); // New link(&L)...
  MI_NewExtLink->Caption =
      Ini->ReadString("MainWnd", "MI_NewExtLink",
                      MI_NewExtLink->Caption); // New external link(&E)...
  MI_NewLabel->Caption =
      Ini->ReadString("MainWnd", "MI_NewLabel",
                      MI_NewLabel->Caption); // New label(&A)...
  MI_NewLinkLabel->Caption =
      Ini->ReadString("MainWnd", "MI_NewLinkLabel",
                      MI_NewLinkLabel->Caption); // New link label(&N)...
  MI_LinktoAllCardswithDesignatedLabel->Caption =
      Ini->ReadString("MainWnd", "MI_LinktoAllCardswithDesignatedLabel",
                      MI_LinktoAllCardswithDesignatedLabel->Caption);
  MI_NewCardLinkstoAllCardswithDesignatedLabel->Caption =
      Ini->ReadString("MainWnd", "MI_NewCardLinkstoAllCardswithDesignatedLabel",
                      MI_NewCardLinkstoAllCardswithDesignatedLabel->Caption);
  MI_AddDesignatedLabeltoAllDestinationCards->Caption =
      Ini->ReadString("MainWnd", "MI_AddDesignatedLabeltoAllDestinationCards",
                      MI_AddDesignatedLabeltoAllDestinationCards->Caption);
  M_Animation->Caption = Ini->ReadString("MainWnd", "M_Animation",
                                         M_Animation->Caption); // Animation(&A)
  MA_RandomFlash->Caption =
      Ini->ReadString("MainWnd", "MA_RandomFlash",
                      MA_RandomFlash->Caption); // Random card(&C)
  MA_RandomScroll->Caption =
      Ini->ReadString("MainWnd", "MA_RandomScroll",
                      MA_RandomScroll->Caption); // Random card2(&2)
  MA_RandomJump->Caption =
      Ini->ReadString("MainWnd", "MA_RandomJump",
                      MA_RandomJump->Caption); // Random card(&C)
  MA_RandomMap->Caption =
      Ini->ReadString("MainWnd", "MA_RandomMap",
                      MA_RandomMap->Caption); // Random card2(&2)
  MA_RandomTrace->Caption =
      Ini->ReadString("MainWnd", "MA_RandomTrace",
                      MA_RandomTrace->Caption); // Random trace(&T)
  M_View->Caption = Ini->ReadString("MainWnd", "M_View", M_View->Caption);
  MV_Font->Caption = Ini->ReadString("MainWnd", "MV_Font",
                                     MV_Font->Caption); // Font(&F)
  MVF_DefaultSize->Caption =
      Ini->ReadString("MainWnd", "MVF_DefaultSize",
                      MVF_DefaultSize->Caption); // Default size(&D)
  MVF_Magnify->Caption = Ini->ReadString("MainWnd", "MVF_Magnify",
                                         MVF_Magnify->Caption); // Magnify(&M)
  MVF_Reduce->Caption = Ini->ReadString("MainWnd", "MVF_Reduce",
                                        MVF_Reduce->Caption); // Reduce(&R)
  MVF_ChangeFont->Caption =
      Ini->ReadString("MainWnd", "MVF_ChangeFont",
                      MVF_ChangeFont->Caption); // Change font(&C)
  MV_Card->Caption = Ini->ReadString("MainWnd", "MV_Card",
                                     MV_Card->Caption); // card(&C)
  MVC_CardShadow->Caption =
      Ini->ReadString("MainWnd", "MVC_CardShadow", MVC_CardShadow->Caption);
  MVC_CardGradation->Caption = Ini->ReadString("MainWnd", "MVC_CardGradation",
                                               MVC_CardGradation->Caption);
  MVC_TickerVisible->Caption = Ini->ReadString("MainWnd", "MVC_TickerVisible",
                                               MVC_TickerVisible->Caption);
  MVC_Ticker1Line->Caption =
      Ini->ReadString("MainWnd", "MVC_Ticker1Line", MVC_Ticker1Line->Caption);
  MVC_Ticker2Lines->Caption =
      Ini->ReadString("MainWnd", "MVC_Ticker2Lines", MVC_Ticker2Lines->Caption);
  MVC_Image->Caption = Ini->ReadString("MainWnd", "MVC_Image",
                                       MVC_Image->Caption); // Image(&I)
  MVC_Video->Caption = Ini->ReadString("MainWnd", "MVC_Video",
                                       MVC_Video->Caption); // Video(&V)
  MVC_Drawing->Caption = Ini->ReadString("MainWnd", "MVC_Drawing",
                                         MVC_Drawing->Caption); // Drawing(&D)
  MV_Link->Caption = Ini->ReadString("MainWnd", "MV_Link",
                                     MV_Link->Caption); // link(&L)
  MVL_Link->Caption = Ini->ReadString("MainWnd", "MVL_Link",
                                      MVL_Link->Caption); // Link(&L)
  MVL_LinkHemming->Caption =
      Ini->ReadString("MainWnd", "MVL_LinkHemming", MVL_LinkHemming->Caption);
  MVL_LinkShadow->Caption =
      Ini->ReadString("MainWnd", "MVL_LinkShadow", MVL_LinkShadow->Caption);
  MVL_LinkDirection->Caption =
      Ini->ReadString("MainWnd", "MVL_LinkDirection",
                      MVL_LinkDirection->Caption); // Link direction(&D)
  MVL_LinkName->Caption =
      Ini->ReadString("MainWnd", "MVL_LinkName",
                      MVL_LinkName->Caption); // Link name(&N)
  MV_Label->Caption = Ini->ReadString("MainWnd", "MV_Label",
                                      MV_Label->Caption); // label(&A)
  MVL_LabelCircle->Caption =
      Ini->ReadString("MainWnd", "MVL_LabelCircle",
                      MVL_LabelCircle->Caption); // Label circle(&C)
  MVL_LabelRectangle->Caption =
      Ini->ReadString("MainWnd", "MVL_LabelRectangle",
                      MVL_LabelRectangle->Caption); // Label rect(&R)
  MVL_LabelName->Caption =
      Ini->ReadString("MainWnd", "MVL_LabelName",
                      MVL_LabelName->Caption); // Label name(&N)
  MV_Text->Caption = Ini->ReadString("MainWnd", "MV_Text",
                                     MV_Text->Caption); // BodyText(&T)
  MVT_Text->Caption = Ini->ReadString("MainWnd", "MVT_Text",
                                      MVT_Text->Caption); // BodyText(&T)
  MVT_Centering->Caption =
      Ini->ReadString("MainWnd", "MVT_Centering",
                      MVT_Centering->Caption); // Centering(&C)
  MVT_WordWrap->Caption =
      Ini->ReadString("MainWnd", "MVT_WordWrap",
                      MVT_WordWrap->Caption); // Word wrap(&W)
  MVT_EditInBrowser->Caption = Ini->ReadString("MainWnd", "MVT_EditInBrowser",
                                               MVT_EditInBrowser->Caption);
  MVT_AlwaysShowEditor->Caption = Ini->ReadString(
      "MainWnd", "MVT_AlwaysShowEditor", MVT_AlwaysShowEditor->Caption);
  MVT_UnderTheCard->Caption =
      Ini->ReadString("MainWnd", "MVT_UnderTheCard", MVT_UnderTheCard->Caption);
  MVT_BrowserRight->Caption =
      Ini->ReadString("MainWnd", "MVT_BrowserRight", MVT_BrowserRight->Caption);
  MVT_BrowserBottom->Caption = Ini->ReadString("MainWnd", "MVT_BrowserBottom",
                                               MVT_BrowserBottom->Caption);
  MV_Others->Caption = Ini->ReadString("MainWnd", "MV_Others",
                                       MV_Others->Caption); // Others(&O)
  MV_Score->Caption = Ini->ReadString("MainWnd", "MV_Score",
                                      MV_Score->Caption); // Score(&S)
  MVS_Score->Caption = Ini->ReadString("MainWnd", "MVS_Score",
                                       MVS_Score->Caption); // Score(&S)
  MVS_Authenticity->Caption =
      Ini->ReadString("MainWnd", "MVS_Authenticity",
                      MVS_Authenticity->Caption); // Authenticity(&A)
  MVS_StartingPoint->Caption =
      Ini->ReadString("MainWnd", "MVS_StartingPoint",
                      MVS_StartingPoint->Caption); // Starting point(&S)
  MVS_Destination->Caption =
      Ini->ReadString("MainWnd", "MVS_Destination",
                      MVS_Destination->Caption); // Destination(&D)
  MVS_Links_Out->Caption =
      Ini->ReadString("MainWnd", "MVS_Links_Out",
                      MVS_Links_Out->Caption); // Links out(&O)
  MVS_Links_In->Caption =
      Ini->ReadString("MainWnd", "MVS_Links_In",
                      MVS_Links_In->Caption); // Links in(&I)
  MVS_Links_Total->Caption =
      Ini->ReadString("MainWnd", "MVS_Links_Total",
                      MVS_Links_Total->Caption); // Links total(&T)
  MVS_Links_InOut->Caption =
      Ini->ReadString("MainWnd", "MVS_Links_InOut",
                      MVS_Links_InOut->Caption); // Links in-out(&L)
  MVS_TextLength->Caption =
      Ini->ReadString("MainWnd", "MVS_TextLength",
                      MVS_TextLength->Caption); // Body length(&X)
  if (MV_Read) {
    MV_Read->Caption = Ini->ReadString("MainWnd", "MV_Read",
                                       MV_Read->Caption); // Read aloud(&R)
  }
  if (MVR_Read) {
    MVR_Read->Caption = Ini->ReadString("MainWnd", "MVR_Read",
                                        MVR_Read->Caption); // Read aloud(&R)
  }
  if (MVR_ReadSetting) {
    MVR_ReadSetting->Caption =
        Ini->ReadString("MainWnd", "MVR_ReadSetting",
                        MVR_ReadSetting->Caption); // Read aloud setting(&E)
  }
  Setting1->Caption =
      Ini->ReadString("MainWnd", "Setting1", Setting1->Caption);
  MS_GPTAPIKey->Caption =
      Ini->ReadString("MainWnd", "MS_GPTAPIKey", MS_GPTAPIKey->Caption);
  if (MS_AutoSaveDefaults) {
    MS_AutoSaveDefaults->Caption = Ini->ReadString(
        "MainWnd", "MS_AutoSaveDefaults", MS_AutoSaveDefaults->Caption);
  }
  if (MS_AutoSaveDefault) {
    MS_AutoSaveDefault->Caption = Ini->ReadString(
        "MainWnd", "MS_AutoSaveDefault", MS_AutoSaveDefault->Caption);
  }
  if (MS_AutoReloadDefault) {
    MS_AutoReloadDefault->Caption = Ini->ReadString(
        "MainWnd", "MS_AutoReloadDefault", MS_AutoReloadDefault->Caption);
  }
  if (MS_ReadAloudRate) {
    MS_ReadAloudRate->Caption =
        Ini->ReadString("MainWnd", "MS_ReadAloudRate",
                        MS_ReadAloudRate->Caption);
  }
  MVO_CardList->Caption = Ini->ReadString("MainWnd", "MVO_CardList",
                                          MVO_CardList->Caption);
  MVO_FileList->Caption = Ini->ReadString("MainWnd", "MVO_FileList",
                                          MVO_FileList->Caption);
  MVO_Overview->Caption =
      Ini->ReadString("MainWnd", "MVO_Overview",
                      MVO_Overview->Caption); // Overview Display(&O)
  MVO_ChangeFourgroundColor->Caption = Ini->ReadString(
      "MainWnd", "MVO_ChangeFourgroundColor",
      MVO_ChangeFourgroundColor->Caption); // Foreground color change(&F)...
  MVO_ChangeBackgroundColor->Caption = Ini->ReadString(
      "MainWnd", "MVO_ChangeBackgroundColor",
      MVO_ChangeBackgroundColor->Caption); // Background color change(&B)...
  MVO_WallPaper->Caption =
      Ini->ReadString("MainWnd", "MVO_WallPaper",
                      MVO_WallPaper->Caption); // Wallpaper(&W)...
  MVO_FixWallPaper->Caption =
      Ini->ReadString("MainWnd", "MVO_FixWallPaper",
                      MVO_FixWallPaper->Caption); // Fix wallpaper(&I)
  MVO_TileWallPaper->Caption =
      Ini->ReadString("MainWnd", "MVO_TileWallPaper",
                      MVO_TileWallPaper->Caption); // Tile wallpaper Display(&T)
  MVO_AntiAliasing->Caption =
      Ini->ReadString("MainWnd", "MVO_AntiAliasing",
                      MVO_AntiAliasing->Caption); // Anti-aliasing(&A)
  MVO_BGAnimation->Caption = Ini->ReadString("MainWnd", "MVO_BGAnimation",
                                             MVO_BGAnimation->Caption);
  MVO_BGAnimationType->Caption =
      Ini->ReadString("MainWnd", "MVO_BGAnimationType",
                      MVO_BGAnimationType->Caption);
  MVO_NoScrollLag->Caption = Ini->ReadString("MainWnd", "MVO_NoScrollLag",
                                             MVO_NoScrollLag->Caption);
  MVO_CursorAnimation->Caption =
      Ini->ReadString("MainWnd", "MVO_CursorAnimation",
                      MVO_CursorAnimation->Caption);
  MVOB_MovingLine->Caption = Ini->ReadString("MainWnd", "MVOB_MovingLine",
                                             MVOB_MovingLine->Caption);
  MVOB_Bubble->Caption = Ini->ReadString("MainWnd", "MVOB_Bubble",
                                         MVOB_Bubble->Caption);
  MVOB_Snow->Caption = Ini->ReadString("MainWnd", "MVOB_Snow",
                                       MVOB_Snow->Caption);
  MVOB_CherryBlossom->Caption =
      Ini->ReadString("MainWnd", "MVOB_CherryBlossom",
                      MVOB_CherryBlossom->Caption);
  MV_ChangeLanguage->Caption =
      Ini->ReadString("MainWnd", "MV_ChangeLanguage",
                      MV_ChangeLanguage->Caption); // Language change(&H)
  MV_FullScreen->Caption =
      Ini->ReadString("MainWnd", "MV_FullScreen",
                      MV_FullScreen->Caption); // Fullscreen Display(&U)
  MV_StatusBar->Caption =
      Ini->ReadString("MainWnd", "MV_StatusBar",
                      MV_StatusBar->Caption); // Status bar(&B)
  M_Help->Caption = Ini->ReadString("MainWnd", "M_Help", M_Help->Caption);
  MH_Contents->Caption =
      Ini->ReadString("MainWnd", "MH_Contents",
                      MH_Contents->Caption); // Contents(&C)...
  MH_FrieveSite->Caption =
      Ini->ReadString("MainWnd", "MH_FrieveSite",
                      MH_FrieveSite->Caption); // Frieve site(&F)...
  MH_FIPSite->Caption =
      Ini->ReadString("MainWnd", "MH_FIPSite",
                      MH_FIPSite->Caption); // Frieve Editor site(&O)...
  MH_CheckLatest->Caption =
      Ini->ReadString("MainWnd", "MH_CheckLatest",
                      MH_CheckLatest->Caption); // Check latest version(&L)...
  MH_About->Caption = Ini->ReadString("MainWnd", "MH_About",
                                      MH_About->Caption); // About(&A)...

  // Popup menu (Editor)
  PE_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
                                     PE_Undo->Caption); // Undo(&U)
  PE_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo", PE_Redo->Caption);
  PE_Cut->Caption = Ini->ReadString("MainWnd", "ME_Cut", PE_Cut->Caption);
  PE_Copy->Caption = Ini->ReadString("MainWnd", "ME_Copy",
                                     PE_Copy->Caption); // Copy(&C)
  PE_Paste->Caption = Ini->ReadString("MainWnd", "ME_Paste",
                                      PE_Paste->Caption); // Paste(&P)
  PE_Delete->Caption = Ini->ReadString("MainWnd", "ME_Delete",
                                       PE_Delete->Caption); // delete(&D)
  PE_SelectAll->Caption =
      Ini->ReadString("MainWnd", "ME_SelectAll",
                      PE_SelectAll->Caption); // Select all(&A)
  PE_CutToNewCard->Caption =
      Ini->ReadString("MainWnd", "PM_CutToNewCard", PE_CutToNewCard->Caption);
  PE_CutToNewCardTitle->Caption = Ini->ReadString(
      "MainWnd", "PM_CutToNewCardTitle", PE_CutToNewCardTitle->Caption);
  PE_CutToNewCardTitleWithLink->Caption =
      Ini->ReadString("MainWnd", "PM_CutToNewCardTitleWithLink",
                      PE_CutToNewCardTitleWithLink->Caption);

  // Popup menu (List)
  PL_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
                                        PL_NewCard->Caption); // New card(&C)
  PL_DeleteCard->Caption =
      Ini->ReadString("MainWnd", "PM_DeleteCard", PL_DeleteCard->Caption);
  PL_CardProperty->Caption =
      Ini->ReadString("MainWnd", "PM_Property", PL_CardProperty->Caption);

  PI_NewLink->Caption =
      Ini->ReadString("MainWnd", "MI_NewLink", PI_NewLink->Caption);
  PI_DeleteLink->Caption =
      Ini->ReadString("MainWnd", "PM_DeleteLink", PI_DeleteLink->Caption);

  // Popup menu (Browser)
  PBN_Select->Caption =
      Ini->ReadString("MainWnd", "PBN_Select", PBN_Select->Caption);
  PBN_Link->Caption = Ini->ReadString("MainWnd", "PBN_Link", PBN_Link->Caption);
  PBN_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
                                      PBN_Undo->Caption); // Undo(&U)
  PBN_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo", PBN_Redo->Caption);
  PBN_Paste->Caption = Ini->ReadString("MainWnd", "ME_Paste",
                                       PBN_Paste->Caption); // Paste(&P)
  PBN_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
                                         PBN_NewCard->Caption); // New card(&C)

  PBC_Select->Caption =
      Ini->ReadString("MainWnd", "PBN_Select", PBC_Select->Caption);
  PBC_Link->Caption = Ini->ReadString("MainWnd", "PBN_Link", PBC_Link->Caption);
  PBC_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
                                      PBN_Undo->Caption); // Undo(&U)
  PBC_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo", PBN_Redo->Caption);
  PBC_Cut->Caption = Ini->ReadString("MainWnd", "ME_Cut",
                                     PBC_Cut->Caption); // Cut(&T)
  PBC_Copy->Caption = Ini->ReadString("MainWnd", "ME_Copy",
                                      PBC_Copy->Caption); // Copy(&C)
  PBC_Paste->Caption = Ini->ReadString("MainWnd", "ME_Paste",
                                       PBC_Paste->Caption); // Paste(&P)
  PBC_WebSearch->Caption = Ini->ReadString("MainWnd", "ME_WebSearch",
                                           ME_WebSearch->Caption);
  PBC_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
                                         PBC_NewCard->Caption); // New card(&C)
  PBC_NewChildCard->Caption =
      Ini->ReadString("MainWnd", "PM_NewChildCard", PBC_NewChildCard->Caption);
  PBC_NewBrotherCard->Caption = Ini->ReadString("MainWnd", "PM_NewBrotherCard",
                                                PBC_NewBrotherCard->Caption);
  PBC_NewLink->Caption =
      Ini->ReadString("MainWnd", "MI_NewLink", PBC_NewLink->Caption);
  PBC_NewExtLink->Caption =
      Ini->ReadString("MainWnd", "MI_NewExtLink", PBC_NewExtLink->Caption);
  PBC_NewLabel->Caption =
      Ini->ReadString("MainWnd", "MI_NewLabel", PBC_NewLabel->Caption);
  PBC_DeleteCard->Caption =
      Ini->ReadString("MainWnd", "PM_DeleteCard", PBC_DeleteCard->Caption);
  PBC_SetAsDefault->Caption =
      Ini->ReadString("MainWnd", "PM_SetAsDefault", PBC_SetAsDefault->Caption);
  PBC_Property->Caption =
      Ini->ReadString("MainWnd", "PM_Property", PBC_Property->Caption);

  PBL_Select->Caption =
      Ini->ReadString("MainWnd", "PBN_Select", PBL_Select->Caption);
  PBL_Link->Caption = Ini->ReadString("MainWnd", "PBN_Link", PBL_Link->Caption);
  PBL_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
                                      PBL_Undo->Caption); // Undo(&U)
  PBL_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo", PBL_Redo->Caption);
  PBL_WebSearch->Caption = Ini->ReadString("MainWnd", "ME_WebSearch",
                                           ME_WebSearch->Caption);
  PBL_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
                                         PBL_NewCard->Caption); // New card(&C)
  PBL_InsertCard->Caption =
      Ini->ReadString("MainWnd", "PM_InsertCard", PBL_InsertCard->Caption);
  PBL_NewLinkLabel->Caption =
      Ini->ReadString("MainWnd", "MI_NewLinkLabel", PBL_NewLinkLabel->Caption);
  PBL_DeleteLink->Caption =
      Ini->ReadString("MainWnd", "PM_DeleteLink", PBL_DeleteLink->Caption);
  PBL_SetAsDefault->Caption =
      Ini->ReadString("MainWnd", "PM_SetAsDefault", PBL_SetAsDefault->Caption);

  // Popup menu (Arrange)
  PAT_Normalize->Caption =
      Ini->ReadString("MainWnd", "PAT_Normalize",
                      PAT_Normalize->Caption); // Normalize(&N)
  PAT_Repulsion->Caption =
      Ini->ReadString("MainWnd", "PAT_Repulsion",
                      PAT_Repulsion->Caption); // Repulsion(&R)
  PAT_Link->Caption = Ini->ReadString("MainWnd", "PAT_Link",
                                      PAT_Link->Caption); // link(&N)
  PAT_Label->Caption = Ini->ReadString("MainWnd", "PAT_Label",
                                       PAT_Label->Caption); // label(&A)
  PAT_Index->Caption = Ini->ReadString("MainWnd", "PAT_Index",
                                       PAT_Index->Caption); // Index(&I)
  PAT_SoftLink->Caption =
      Ini->ReadString("MainWnd", "PAT_SoftLink",
                      PAT_SoftLink->Caption); // Link(soft)(&K)
  PAT_SoftLabel->Caption =
      Ini->ReadString("MainWnd", "PAT_SoftLabel",
                      PAT_SoftLabel->Caption); // Label(soft)(&B)
  PAT_SoftIndex->Caption =
      Ini->ReadString("MainWnd", "PAT_SoftIndex",
                      PAT_SoftIndex->Caption); // Soft index(&D)
  PAT_Matrix->Caption = Ini->ReadString("MainWnd", "PAT_Matrix",
                                        PAT_Matrix->Caption); // Matrix(&M)
  PAT_MatrixIndex->Caption =
      Ini->ReadString("MainWnd", "PAT_MatrixIndex",
                      PAT_MatrixIndex->Caption); // Matrix index(&E)
  PAT_Similarity->Caption =
      Ini->ReadString("MainWnd", "PAT_Similarity",
                      PAT_Similarity->Caption); // Similarity(&S)
  PAT_SimilaritySoft->Caption =
      Ini->ReadString("MainWnd", "PAT_SimilaritySoft",
                      PAT_SimilaritySoft->Caption); // Similarity(soft)
  PAT_TreeRadial->Caption =
      Ini->ReadString("MainWnd", "PAT_TreeRadial",
                      PAT_TreeRadial->Caption); // Tree radial(&T)

  // Popup menu (Statistics)
  PMSK_Label->Caption = Ini->ReadString("MainWnd", "PMSK_Label",
                                        PMSK_Label->Caption); // label(&L)
  PMSK_NumberOfLink->Caption =
      Ini->ReadString("MainWnd", "PMSK_NumberOfLink",
                      PMSK_NumberOfLink->Caption); // Number of links(&N)
  PMSK_CreatedDate->Caption =
      Ini->ReadString("MainWnd", "PMSK_CreatedDate",
                      PMSK_CreatedDate->Caption); // Created date(&C)
  PMSK_EditedDate->Caption =
      Ini->ReadString("MainWnd", "PMSK_EditedDate",
                      PMSK_EditedDate->Caption); // Edited date(&E)
  PMSK_ViewedDate->Caption =
      Ini->ReadString("MainWnd", "PMSK_ViewedDate",
                      PMSK_ViewedDate->Caption); // Viewed date(&V)
  PMSKL_Total->Caption = Ini->ReadString("MainWnd", "PMSKL_Total",
                                         PMSKL_Total->Caption); // Total(&T)
  PMSKL_Source->Caption =
      Ini->ReadString("MainWnd", "PMSKL_Source",
                      PMSKL_Source->Caption); // Link source(&S)
  PMSKL_Destination->Caption =
      Ini->ReadString("MainWnd", "PMSKL_Destination",
                      PMSKL_Destination->Caption); // Link destination(&D)
  PMSKC_Year->Caption = Ini->ReadString("MainWnd", "PMSK_Year",
                                        PMSKC_Year->Caption); // Year(&Y)
  PMSKC_Month->Caption = Ini->ReadString("MainWnd", "PMSK_Month",
                                         PMSKC_Month->Caption); // Month(&M)
  PMSKC_Day->Caption = Ini->ReadString("MainWnd", "PMSK_Day",
                                       PMSKC_Day->Caption); // Day(&D)
  PMSKC_Week->Caption = Ini->ReadString("MainWnd", "PMSK_Week",
                                        PMSKC_Week->Caption); // Week(&W)
  PMSKC_Hour->Caption = Ini->ReadString("MainWnd", "PMSK_Hour",
                                        PMSKC_Hour->Caption); // Hour(&H)
  PMSKE_Year->Caption = Ini->ReadString("MainWnd", "PMSK_Year",
                                        PMSKE_Year->Caption); // Year(&Y)
  PMSKE_Month->Caption = Ini->ReadString("MainWnd", "PMSK_Month",
                                         PMSKE_Month->Caption); // Month(&M)
  PMSKE_Day->Caption = Ini->ReadString("MainWnd", "PMSK_Day",
                                       PMSKE_Day->Caption); // Day(&D)
  PMSKE_Week->Caption = Ini->ReadString("MainWnd", "PMSK_Week",
                                        PMSKE_Week->Caption); // Week(&W)
  PMSKE_Hour->Caption = Ini->ReadString("MainWnd", "PMSK_Hour",
                                        PMSKE_Hour->Caption); // Hour(&H)
  PMSKV_Year->Caption = Ini->ReadString("MainWnd", "PMSK_Year",
                                        PMSKV_Year->Caption); // Year(&Y)
  PMSKV_Month->Caption = Ini->ReadString("MainWnd", "PMSK_Month",
                                         PMSKV_Month->Caption); // Month(&M)
  PMSKV_Day->Caption = Ini->ReadString("MainWnd", "PMSK_Day",
                                       PMSKV_Day->Caption); // Day(&D)
  PMSKV_Week->Caption = Ini->ReadString("MainWnd", "PMSK_Week",
                                        PMSKV_Week->Caption); // Week(&W)
  PMSKV_Hour->Caption = Ini->ReadString("MainWnd", "PMSK_Hour",
                                        PMSKV_Hour->Caption); // Hour(&H)

  // Others and text
  MLText.Cards = Ini->ReadString("Common", "Cards", MLText.Cards);
  MLText.Links = Ini->ReadString("Common", "Links", MLText.Links);
  MLText.Labels = Ini->ReadString("Common", "Labels", MLText.Labels);
  MLText.Line = Ini->ReadString("Common", "Line", MLText.Line);
  MLText.Len = Ini->ReadString("Common", "Len", MLText.Len);
  MLText.Size = Ini->ReadString("Common", "Size", MLText.Size);
  MLText.Title = Ini->ReadString("Common", "Title", MLText.Title);
  MLText.Created = Ini->ReadString("Common", "Created", MLText.Created);
  MLText.Edited = Ini->ReadString("Common", "Edited", MLText.Edited);
  MLText.Viewed = Ini->ReadString("Common", "Viewed", MLText.Viewed);
  MLText.Hits = Ini->ReadString("Common", "Hits", MLText.Hits);

  MLText.EditCardTitle =
      Ini->ReadString("Common", "EditCardTitle", MLText.EditCardTitle);
  MLText.EditCardText =
      Ini->ReadString("Common", "EditCardText", MLText.EditCardText);
  MLText.EditLinkTitle =
      Ini->ReadString("Common", "EditLinkTitle", MLText.EditLinkTitle);
  MLText.EditDrawing =
      Ini->ReadString("Common", "EditDrawing", MLText.EditDrawing);

  MLText.ChangeSize =
      Ini->ReadString("Common", "ChangeSize", MLText.ChangeSize);
  MLText.FixCardPosition =
      Ini->ReadString("Common", "FixCardPosition", MLText.FixCardPosition);
  MLText.SetAsRootCard =
      Ini->ReadString("Common", "SetAsRootCard", MLText.SetAsRootCard);

  MLText.EditLink = Ini->ReadString("Common", "EditLink", MLText.EditLink);

  MLText.AddLabel = Ini->ReadString("Common", "AddLabel", MLText.AddLabel);
  MLText.DeleteLabel =
      Ini->ReadString("Common", "DeleteLabel", MLText.DeleteLabel);
  MLText.ChangeLabel =
      Ini->ReadString("Common", "ChangeLabel", MLText.ChangeLabel);
  MLText.LabelName = Ini->ReadString("Common", "LabelName", MLText.LabelName);
  MLText.SelectLinkDestination = Ini->ReadString(
      "Common", "SelectLinkDestination", MLText.SelectLinkDestination);
  MLText.SelectLabelOfDestination = Ini->ReadString(
      "Common", "SelectLabelOfDestination", MLText.SelectLabelOfDestination);
  MLText.SelectLabel =
      Ini->ReadString("Common", "SelectLabel", MLText.SelectLabel);

  MLText.Color = Ini->ReadString("Common", "Color", MLText.Color);

  MLText.ArrangeCards =
      Ini->ReadString("Common", "ArrangeCards", MLText.ArrangeCards);

  MLText.ProcessingTextAnalysis = Ini->ReadString(
      "Common", "ProcessingTextAnalysis", MLText.ProcessingTextAnalysis);

  MLText.Enable = Ini->ReadString("Common", "Enable", MLText.Enable);
  MLText.Fold = Ini->ReadString("Common", "Fold", MLText.Fold);
  MLText.Show = Ini->ReadString("Common", "Show", MLText.Show);
  MLText.Hide = Ini->ReadString("Common", "Hide", MLText.Hide);
  MLText.NoAssign = Ini->ReadString("Common", "NoAssign",
                                    MLText.NoAssign); // No assign
  MLText.NoLabel = Ini->ReadString("Common", "NoLabel", MLText.NoLabel);
  MLText.SelectTargetCard =
      Ini->ReadString("Common", "SelectTargetCard",
                      MLText.SelectTargetCard); // Destination card selection
  MLText.Old = Ini->ReadString("Common", "Old", MLText.Old);
  MLText.New = Ini->ReadString("Common", "New", MLText.New);
  MLText.Near = Ini->ReadString("Common", "Near", MLText.Near);
  MLText.Older = Ini->ReadString("Common", "Older", MLText.Older);
  MLText.Newer = Ini->ReadString("Common", "Newer", MLText.Newer);
  MLText.SaveCheck = Ini->ReadString("Common", "SaveCheck", MLText.SaveCheck);
  MLText.FailedToSave =
      Ini->ReadString("Common", "FailedToSave", MLText.FailedToSave);
  MLText.NotFound = Ini->ReadString("Common", "NotFound", MLText.NotFound);
  MLText.AANotEnoughMemory =
      Ini->ReadString("Common", "AANotEnoughMemory", MLText.AANotEnoughMemory);
  delete Ini;
}
// ---------------------------------------------------------------------------

void TFo_Main::RefreshWallPaper() {
  Im_Wall->Enabled = false;
  if (SettingView.m_WallPaper != "") {
    if (FileExists(SettingView.m_WallPaper)) {
      try {
        Im_Wall->Picture->LoadFromFile(SettingView.m_WallPaper);
        Im_Wall->Enabled = true;
      } catch (...) {
      }
    }
  }
}
// ---------------------------------------------------------------------------

void TFo_Main::SetFont() {
  RE_Edit->Font->Name = SettingView.m_Font->Name;
  RE_Edit->Font->Charset = SettingView.m_Font->Charset;

  Me_EditorRelated->Font->Name = SettingView.m_Font->Name;
  Me_EditorRelated->Font->Charset = SettingView.m_Font->Charset;

  LB_List->Font->Name = SettingView.m_Font->Name;
  LB_List->Font->Charset = SettingView.m_Font->Charset;

  LB_FileList->Font->Name = SettingView.m_Font->Name;
  LB_FileList->Font->Charset = SettingView.m_Font->Charset;

  LB_Link->Font->Name = SettingView.m_Font->Name;
  LB_Link->Font->Charset = SettingView.m_Font->Charset;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MVO_FixWallPaperClick(TObject *Sender) {
  SettingView.m_bFixWallPaper = !SettingView.m_bFixWallPaper;
  m_bRedrawRequested = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MVO_TileWallPaperClick(TObject *Sender) {
  SettingView.m_bTileWallPaper = !SettingView.m_bTileWallPaper;
  m_bRedrawRequested = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MVO_WallPaperClick(TObject *Sender) {
  if (SettingView.m_WallPaper != "") {
    SettingView.m_WallPaper = "";
    RefreshWallPaper();
  } else {
    OD_WallPaper->InitialDir = ExtractFilePath(ParamStr(0)) + "wallpaper";
    if (OD_WallPaper->Execute()) {
      SettingView.m_WallPaper = OD_WallPaper->FileName;
      RefreshWallPaper();
    }
  }
  m_bRedrawRequested = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MVF_ChangeFontClick(TObject *Sender) {
  FD_Font->Font->Name = SettingView.m_Font->Name;
  FD_Font->Font->Charset = SettingView.m_Font->Charset;
  FD_Font->Font->Size = SettingView.m_Font->Size;
  if (FD_Font->Execute()) {
    SettingView.m_Font->Name = FD_Font->Font->Name;
    SettingView.m_Font->Charset = FD_Font->Font->Charset;
    SettingView.m_Font->Size = FD_Font->Font->Size;

    UD_Size->Position = SettingView.m_Font->Size;
    Ed_Size->Text = SettingView.m_Font->Size;

    SetFont();
    m_bRedrawRequested = true;
  }
}
// ---------------------------------------------------------------------------

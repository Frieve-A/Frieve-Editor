// --------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <DateUtils.hpp>
#include <HttpApp.hpp>
#include <IdURI.hpp>
#include <Vcl.filectrl.hpp>
#include <clipbrd.hpp>
#include <math.h>
#include <printers.hpp>
#include <stdio.h>

#include "fomain.h"
#include "fomain_file.h"
#include "fomain_label.h"
#include "fomain_paint.h"
#include "fomain_timer.h"
// ---------------------------------------------------------------------------

#include "foabout.h"
#include "foedittext.h"
#include "fofullscreen.h"
#include "folabeledit.h"
#include "forandomanimation.h"
#include "fosearch.h"
#include "foselectcard.h"
#include "foview.h"
#include "setting.h"
#include "utils.h"
// #include "inet.h"
#include "foinformation.h"
#include "foprogress.h"
#include "foshape.h"
#include "textproc.h"
// ---------------------------------------------------------------------------
// For Web API
#include <System.Classes.hpp>
#include <System.IOUtils.hpp>
#include <System.JSON.hpp>
#include <System.Net.HttpClient.hpp>
#include <System.Net.HttpClientComponent.hpp>
#include <System.Net.URLClient.hpp>
// ---------------------------------------------------------------------------

#pragma package(smart_init)
// #pragma link "HTTSLib_OCX"
// #pragma link "AgentObjects_OCX"
// #pragma link "ACTIVEVOICEPROJECTLib_OCX"
#pragma resource "*.dfm"

TFo_Main *Fo_Main;

#define float_ double

// ---------------------------------------------------------------------------
TCardImage::TCardImage(UnicodeString FN)
    : m_FN(FN), m_bExist(false), m_bUsed(true), m_Jpg(NULL), m_BMP(NULL),
      m_Image(NULL) {
  if (FileExists(m_FN)) {
    if (ExtractFileExt(m_FN) == ".bmp") {
      // BMP
      m_BMP = new Graphics::TBitmap();
      try {
        m_BMP->LoadFromFile(m_FN);
        m_Image = m_BMP;
        m_bExist = true;
      } catch (...) {
      }
    } else {
      // Jpeg
      m_Jpg = new TJPEGImage();
      try {
        m_Jpg->LoadFromFile(m_FN);
        m_Image = m_Jpg;
        m_bExist = true;
      } catch (...) {
      }
    }
  }
}

// ---------------------------------------------------------------------------
TCardImage::~TCardImage() {
  if (m_Jpg) {
    delete m_Jpg;
  }
  if (m_BMP) {
    delete m_BMP;
  }
}

// ---------------------------------------------------------------------------
TCardVideo::TCardVideo(UnicodeString FN)
    : m_FN(FN), m_bExist(false), m_bUsed(true), m_Video(NULL) {}

// ---------------------------------------------------------------------------
TCardVideo::~TCardVideo() {}

// ---------------------------------------------------------------------------
void __fastcall TEdit2::CreateParams(TCreateParams &Params) {
  TMemo::CreateParams(Params);
  // Params.Style = Params.Style | ES_CENTER & (0xffffffff ^ WS_VSCROLL);
  Params.Style = Params.Style & (0xffffffff ^ WS_VSCROLL);
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::WMEraseBkgnd(TWMEraseBkgnd &msg) {
  Canvas->Brush->Color = Color;
  Canvas->Brush->Style = bsSolid;
  Canvas->FillRect(Rect(0, 0, ClientWidth, ClientHeight));
  msg.Result = 1;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::WMDropFiles(TWMDropFiles &mes) {
  int fcount = DragQueryFile((HDROP)mes.Drop, 0xFFFFFFFF, NULL, 0);

  // Card selected? Open: unknown
  // If card selected, Open or Insert unclear
  // If multiple files and fip mixed, Insert
  // If fip/txt mixed, Insert
  // Default Open; if Ctrl pressed or above conditions, Insert

  // For now: 2+ files or non-fip = all external links

  if (fcount > 0) {
    wchar_t FN[MAX_PATH];
    DragQueryFile((HDROP)mes.Drop, 0, FN, MAX_PATH);

    if (fcount > 1 || LowerCase(ExtractFileExt(FN)) != ".fip") {
      // Insert external link
      if (m_nTargetCard >= 0 && m_nTargetCard == m_nCurrentCard) {
        // Insert into current card
        for (int i = 0; i < fcount; i++) {
          DragQueryFile((HDROP)mes.Drop, i, FN, MAX_PATH);
          RE_Edit->Lines->Add(FN);
        }
      } else {
        // Create card and insert
        m_Document->ClearCardSelection();
        for (int i = 0; i < fcount; i++) {
          DragQueryFile((HDROP)mes.Drop, i, FN, MAX_PATH);
          TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
          Card->m_bSelected = true;
          m_Document->SetCardText(Card, FN);
          Card->m_Title = ExtractFileName(FN);
          Card->m_Title = Card->m_Title.SubString(
              1,
              Card->m_Title.Length() - ExtractFileExt(Card->m_Title).Length());
          m_nTargetCard = Card->m_nID;
        }
      }

      if (m_nTargetCard >= 0 && m_nTargetCard == m_nCurrentCard) {
      }
    } else {
      // Open
      if (SaveCheck()) {
        LoadSub(FN);
      }
    }
  }
  DragFinish((HDROP)mes.Drop);
} // ---------------------------------------------------------------------------

__fastcall TFo_Main::TFo_Main(TComponent *Owner)
    : TForm(Owner), m_Document(NULL),
      // Undo buffer
      m_UndoRedo(NULL),
      // Display update
      m_nRefreshListCount(0), m_nRefreshLinkCount(0), m_nRefreshLabelCount(0),
      m_nTargetCard(0),  // Card being edited
      m_nTargetLink(0),  // Link being edited
      m_nTargetCard2(0), // Link destination card
      m_nCurrentCard(
          0), // Card being displayed (refresh if differs from TargetCard)
      m_nCurrentLink(
          0), // Link being displayed (refresh if differs from TargetLink)

      m_LinkIndexes(NULL), // Displayed link indexes

      // Label
      MI_Labels(NULL), MI_LinkLabels(NULL),

      m_nToolLabel(0),     // Label from label button
      m_nToolLinkLabel(0), // Label from label button

      // Browser display
      m_nBrowserWidth(0), m_nBrowserHeight(), m_nBGColor(0), m_nFGColor(0),
      m_fFontZoom(1.0f), m_nXOffset(0), m_nYOffset(0), // Printer display offset

      m_nTmpCardsCount(
          0), // Temp var buffer size (realloc in PaintSub if count changes)
      m_nTmpLinksCount(
          0), // Temp var buffer size (realloc in PaintSub if count changes)
      m_CardVisible(NULL), m_LinkVisible(NULL),
      m_CardTitle(NULL), // Temp card title (Title changes when Folded)
      m_CardRelated(NULL),
      m_CardAssign(NULL), // For Fold: move as card at this index
      m_CardShape(NULL),  // Card shape (temp; majority when Folded)
      // Card size should also use Fold average?
      m_CardX(NULL),      // Display coordinates (ignores screen size)
      m_CardY(NULL),      // Display coordinates (ignores screen size)
      m_CardWidth(NULL),  // Width (pixels)
      m_CardHeight(NULL), // Height (pixels)
      m_nFontHeight(NULL),
      m_fZoomSD(1.0f),      // Zoom std dev; 0.21 default; smaller = wider view
      m_fTickerSpeed(1.0f), // Ticker movement amount
      m_nLastTimeOut(0), m_bRedrawRequested(false),

      // Frame rate calculation
      m_fFPS(1.0f), m_nFPSCount(0), m_nLastFPSTime(0),

      // Arrange
      m_SimMatrix(NULL), // Similarity matrix per card

      // Arrange (grid layout)
      m_nMatrixWidth(1),  // Grid width
      m_nMatrixHeight(1), // Grid height

      // AutoScroll
      m_fBrowserScrollRatio(1.0f), // Scroll amount
      m_fBrowserScrollLastD(1.0f), // Distance to last target
      m_nScrollTargetX(0),         // Scroll request from Overview
      m_nScrollTargetY(0),

      // Animation
      m_DocBeforeAnimation(NULL), // Backup doc before animation
      m_nAnimation(0),
      // Animation state: 0=none, 1=RandomCard, 2=RandomCard2, 3=RandomTrace
      m_nAnimationCount(
          0), // Animation progress (usage depends on animation type)
      // Animation backups
      m_nAnimationBak_ArrangeType(0), m_bAnimationBak_Arrange(false),
      m_bAnimationBak_AutoScroll(false), m_bAnimationBak_AutoZoom(false),
      m_nAnimation_LastCard(0),

      // File
      Ini(NULL),

      // Title input
      Ed_TitleB(NULL),

      // Operation
      m_bMDownBrowser(0), // 1=normal drag, 2=edit link Dest, 3=edit link From
      m_bDblClicked(0), m_bTitleEditRequested(false),
      m_bTextEditRequested(false), m_uMDownBrowserLast(0),
      m_bMDownBrowserMoved(false), // Moved after mouse down
      m_nMDownBrowserOffsetX(0), m_nMDownBrowserOffsetY(0),
      m_nMDownBrowserX(0),                     // Mouse down coordinates
      m_nMDownBrowserY(0), m_nMDownTargetX(0), // Link line destination
      m_nMDownTargetY(0), m_nMDownBrowserScrollX(0), m_nMDownBrowserScrollY(0),
      m_bShowRecent(false), // Highlight recently viewed cards (Space key)

      // Search
      m_bSearching(false),

      m_GlobalSearchResult(NULL), // Global search result (hit card IDs)
      m_GlobalSearchItemHeight(0), m_GlobalSearchOption(0),
      m_GlobalSearchCursorIndex(0),

      MI_WebSearch(NULL), MI_GPT(NULL),

      // Undo, Redo
      m_nLastModified(0), // Last text edit time (for Undo backup)
      m_nNextCardID(0), m_nNextSelStart(0), m_nNextSelLength(0),
      // Editor cursor position for Undo/Redo
      m_nLastSelLength(0),
      m_bDoNotBackup(
          false), // Disable fine-grained Undo backup during compound edit

      // Overview coordinates
      m_nOVWidth(1), m_nOVXOffset(0), m_nOVHeight(1), m_nOVYOffset(0),

      // Focus cursor position (0=start to 100=reached)
      m_nFocusCursorPos(0), m_nLastTarget(0), // Previous target card

      MI_ExtLink(NULL),

      m_fMinScore(0.0f), m_fMaxScore(1.0f), m_ImageList(NULL),
      m_VideoList(NULL),
      m_fBGAnimationSpeed(1.0f), // Animation duration in seconds

      // Drawing
      m_Drawing(NULL), // Drawing in progress
      m_DrawingTool(0),

      // Statistics
      m_fStatisticsPos(0.0),        // Smooth graph rise factor (0.0~1.0)
      m_StatisticsRectToCard(NULL), // Cards to show on range selection

      // Continuous load (reload when edited file changes)
      m_bContinuousLoad(false),
      m_nCLFileAge(0), // Timestamp of file for continuous load

      // Demo
      m_DemoStrings(NULL), m_nDemoIndex(0),
      // For consistency
      m_bSkipAutoZoom(false),
      m_bFileListDragging(false) // Don't hide while using FileList
{
  memset(Bu_Label, 0, sizeof(Bu_Label));
  memset(Bu_LinkLabel, 0, sizeof(Bu_LinkLabel));

  memset(m_Animation_RC2Idxs, 0, sizeof(m_Animation_RC2Idxs));
  memset(m_Animation_RC2AXs, 0, sizeof(m_Animation_RC2AXs));
  memset(m_Animation_RC2AYs, 0, sizeof(m_Animation_RC2AYs));

  memset(MI_RecentFiles, 0, sizeof(MI_RecentFiles));
  memset(MI_RecentFolders, 0, sizeof(MI_RecentFolders));

  memset(m_BGAnimationBuf, 0, sizeof(m_BGAnimationBuf));
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormCreate(TObject *Sender) {
  randomize();

  // Fast load initial settings
  Ini = new TIniFile(ExtractFilePath(ParamStr(0)) + "setting.ini");
  SettingFile.ReadFromIni(Ini, "File");
  SettingView.ReadFromIni(Ini, "View");

  TIniFile *FIni = new TIniFile(ExtractFilePath(ParamStr(0)) + "setting2.ini");
  Setting2Function.ReadFromIni(FIni);
  delete FIni;

  m_Document = new TDocument();
  m_Document->GetCardByIndex(0)->m_bSelected = true;
  m_UndoRedo = new TUndoRedo(SettingFile.m_nUndoTimes);
  m_nLastModified = 0;
  m_nNextSelStart = 0;
  m_nNextSelLength = 0;
  m_nLastSelLength = 0;
  m_bDoNotBackup = false;

  m_nRefreshListCount = 0;
  m_nRefreshLinkCount = 0;
  m_nRefreshLabelCount = 0;
  m_nTargetCard = -1;
  m_nTargetLink = -1;
  m_nCurrentCard = -1;
  m_nCurrentLink = -1;
  m_nTargetCard2 = -1;
  m_bDblClicked = false;

  m_fBrowserScrollLastD = 0.0f;
  m_fBrowserScrollRatio = 0.5f;
  m_fZoomSD = 0.21f;
  m_nScrollTargetX = -65536;
  m_nScrollTargetY = -65536;

  m_fMinScore = 0.0f;
  m_fMaxScore = 1.0f;

  m_Drawing = new TDrawing();
  m_DrawingRect = Rect(0, 0, 1, 1);
  m_DrawingTool = 0;

  m_fStatisticsPos = 0.0f;
  m_StatisticsRectToCard = new TList();

  m_LinkIndexes = new TList();

  // Recent menu
  for (int i = 0; i < 10; i++) {
    MI_RecentFiles[i] = new TMenuItem(MF_RecentFiles);
    MI_RecentFiles[i]->Tag = i;
    MI_RecentFiles[i]->Visible = false;
    MI_RecentFiles[i]->OnClick = MF_RecentFilesClick;
    MF_RecentFiles->Add(MI_RecentFiles[i]);

    MI_RecentFolders[i] = new TMenuItem(MF_RecentFolders);
    MI_RecentFolders[i]->Tag = i;
    MI_RecentFolders[i]->Visible = false;
    MI_RecentFolders[i]->OnClick = MF_RecentFoldersClick;
    MF_RecentFolders->Add(MI_RecentFolders[i]);
  }

  // Web search menu
  MI_WebSearch = new TList();
  int inspos = 9;
  for (int i = 0; i < Setting2Function.m_WebSearch->Count * 4; i++) {
    TMenuItem *MI;
    switch (i / Setting2Function.m_WebSearch->Count) {
    case 0:
      MI = new TMenuItem(ME_WebSearch);
      break;
    case 1:
      MI = new TMenuItem(PE_Sep1->Owner);
      break;
    case 2:
      MI = new TMenuItem(PBC_WebSearch);
      break;
    case 3:
      MI = new TMenuItem(PBL_WebSearch);
      break;
    }
    MI->Caption = Setting2Function.m_WebSearch
                      ->Names[i % Setting2Function.m_WebSearch->Count];
    MI->OnClick = ME_WebSearchClick;
    MI->Tag = i % Setting2Function.m_WebSearch->Count;

    MI_WebSearch->Add(MI);
    switch (i / Setting2Function.m_WebSearch->Count) {
    case 0:
      if (SettingFile.m_WebSearch == "" && i == 0) {
        SettingFile.m_WebSearch = MI->Caption;
      }
      if (MI->Caption == SettingFile.m_WebSearch) {
        MI->ShortCut = TextToShortCut("Ctrl+W");
      } else {
        MI->ShortCut = 0;
      }
      ME_WebSearch->Add(MI);
      break;
    case 1:
      PM_Editor->Items->Insert(inspos++, MI);
      break;
    case 2:
      PBC_WebSearch->Add(MI);
      break;
    case 3:
      PBL_WebSearch->Add(MI);
      break;
    }
  }

  // GPT menu
  MI_GPT = new TList();
  for (int i = 0; i < Setting2Function.m_GPT->Count; i++) {
    TMenuItem *MI;
    MI = new TMenuItem(PM_GPT);
    UnicodeString caption = Setting2Function.m_GPT->Names[i];
    if (Setting2Function.m_GPT->Strings[i].Pos("(Input)") > 0) {
      caption += "...";
    }
    MI->Caption = caption;
    if (caption != "-") {
      MI->OnClick = ME_GPTClick;
    }
    MI->Tag = i;

    MI_GPT->Add(MI);
    PM_GPT->Items->Add(MI);
  }

  // Title input
  Ed_TitleB = new TEdit2(Pa_Browser);
  Ed_TitleB->Visible = false;
  Ed_TitleB->WordWrap = false;
  Ed_TitleB->OnChange = Ed_TitleChange;
  Ed_TitleB->OnEnter = Ed_TitleBEnter;
  Ed_TitleB->OnExit = Ed_TitleBExit;
  Ed_TitleB->OnKeyDown = Ed_TitleBKeyDown;
  Ed_TitleB->OnDblClick = PB_BrowserDblClick;
  Ed_TitleB->Parent = Pa_Browser;

  // Label buttons (position relative to La_Label, vertically centered in panel)
  for (int i = 0; i < MAXLABELS; i++) {
    Bu_Label[i] = new TButton(Bu_Label0->Owner);
    Bu_Label[i]->Caption = "";
    Bu_Label[i]->SetBounds(
        Bu_Label0->Left + (Bu_Label0->Width + 8) * i,
        Bu_Label0->Top,
        Bu_Label0->Width,
        Bu_Label0->Height
    );
    Bu_Label[i]->Parent = Bu_Label0->Parent;
    Bu_Label[i]->OnClick = Bu_Label0Click;
    Bu_Label[i]->Tag = i;
    Bu_Label[i]->Visible = false;

    Bu_LinkLabel[i] = new TButton(Bu_LinkLabel0->Owner);
    Bu_LinkLabel[i]->Caption = "";
    Bu_LinkLabel[i]->SetBounds(
        Bu_LinkLabel0->Left + (Bu_LinkLabel0->Width + 8) * i,
        Bu_LinkLabel0->Top,
        Bu_LinkLabel0->Width,
        Bu_LinkLabel0->Height
    );
    Bu_LinkLabel[i]->Parent = Bu_LinkLabel0->Parent;
    Bu_LinkLabel[i]->OnClick = Bu_LinkLabel0Click;
    Bu_LinkLabel[i]->Tag = i;
    Bu_LinkLabel[i]->Visible = false;
  }

  // Label selection menu
  MI_Labels = new TList();
  MI_LinkLabels = new TList();

  // External link menu
  MI_ExtLink = NULL;

  // Browser coordinates
  m_nBrowserWidth = 1;
  m_nBrowserHeight = 1;
  m_nTmpCardsCount = 0;
  m_nTmpLinksCount = 0;
  m_CardVisible = NULL;
  m_LinkVisible = NULL;
  m_CardTitle = NULL;
  m_CardRelated = NULL;
  m_CardAssign = NULL;
  m_CardShape = NULL;
  m_CardX = NULL;
  m_CardY = NULL;
  m_CardWidth = NULL;
  m_CardHeight = NULL;

  // Animation
  m_DocBeforeAnimation = NULL;
  m_nAnimation = 0;
  m_nAnimationCount = 0;

  // Misc
  Bu_ArrangeType->Tag = 2;
  m_bSearching = false;
  m_GlobalSearchResult = new TList();
  m_ImageList = new TList();
  m_VideoList = new TList();
  memset(m_BGAnimationBuf, 0, sizeof(m_BGAnimationBuf));
  m_bContinuousLoad = false;
  m_SimMatrix = NULL;

  m_bSkipAutoZoom = false;

  m_fFPS = 0.0f;
  m_nFPSCount = 0;
  m_nLastFPSTime = GetTickCount();
  m_DemoStrings = new TStringList();

  UD_Size->Position = SettingView.m_Font->Size;
  Ed_Size->Text = SettingView.m_Font->Size;
  SetFont();

  m_nLastTarget = -1;
  m_nFocusCursorPos = 0;
  m_bRedrawRequested = false;

  RefreshWallPaper();
  RefreshFileList();

  m_DemoStrings->Add("");

  m_nDemoIndex = 0;

  m_bMDownBrowser = 0;

  m_nLastTimeOut = GetTickCount();

  DragAcceptFiles(Handle, true);
  DoubleBuffered = true;
  Pa_Files->DoubleBuffered = true;
  LB_FileList->DoubleBuffered = true;
  Pa_List->DoubleBuffered = true;
  LB_List->DoubleBuffered = true;
  Pa_Client->DoubleBuffered = true;
  PC_Client->DoubleBuffered = true;
  // TS_Editor->DoubleBufferd = true;
  Pa_Editor->DoubleBuffered = true;
  RE_Edit->DoubleBuffered = true;
  TS_Browser->DoubleBuffered = true;
  Pa_BrowserTop->DoubleBuffered = true;
  Pa_Browser->DoubleBuffered = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormDestroy(TObject *Sender) {
  delete m_DemoStrings;

  if (m_DocBeforeAnimation) {
    delete m_DocBeforeAnimation;
  }

  FreeSimMatrix();

  for (int i = 0; i < m_VideoList->Count; i++) {
    delete (TCardVideo *)m_VideoList->Items[i];
  }
  delete m_VideoList;

  for (int i = 0; i < m_ImageList->Count; i++) {
    delete (TCardImage *)m_ImageList->Items[i];
  }
  delete m_ImageList;

  delete m_Drawing;

  ClearStatisticsRectToCard();
  delete m_GlobalSearchResult;

  delete m_LinkIndexes;

  ClearStatisticsRectToCard();
  delete m_StatisticsRectToCard;

  delete m_UndoRedo;
  delete m_Document;

  delete Ed_TitleB;

  FreeMILabels();
  delete MI_Labels;
  delete MI_LinkLabels;

  delete MI_WebSearch;
  delete MI_GPT;

  FreeMIExtLink();

  if (m_CardX != NULL) {
    delete[] m_CardVisible;
    delete[] m_LinkVisible;
    delete[] m_CardTitle;
    delete[] m_CardRelated;
    delete[] m_CardAssign;
    delete[] m_CardShape;
    delete[] m_CardX;
    delete[] m_CardY;
    delete[] m_CardWidth;
    delete[] m_CardHeight;
  }
}

// ---------------------------------------------------------------------------
bool TFo_Main::TreeMode() {
  return Bu_ArrangeType->Tag >= 1000 && Bu_ArrangeType->Tag < 1999 &&
         SB_Arrange->Down;
}

// ---------------------------------------------------------------------------
void TFo_Main::SetTextEditPos() {
  int cardindex = m_Document->SearchCardIndex(m_nTargetCard);
  int add = 2;
  if (Fo_FullScreen->Visible) {
    add = 0;
  }

  switch (SettingView.m_nEditInBrowserPos * !Fo_FullScreen->Visible) {
  case 0:
    if (cardindex >= 0) {
      // Browser top
      RE_Edit->Align = alNone;
      RE_Edit->Width = m_nBrowserWidth / 3;
      RE_Edit->Height = m_nBrowserHeight / 3;
      RE_Edit->Left = m_CardX[cardindex] - m_CardWidth[cardindex] / 2 + add;
      RE_Edit->Top = m_CardY[cardindex] + m_CardHeight[cardindex] / 2 + add;
      if (Fo_FullScreen->Visible) {
        RE_Edit->Parent = Fo_FullScreen;
      } else {
        RE_Edit->Parent = Pa_Browser;
      }
    } else {
      return;
    }
    break;
  case 1:
    // Browser right
    if (SettingView.m_nEditorWidthInBrowser == 0 ||
        SettingView.m_nEditorWidthInBrowser >= TS_Browser->Width) {
      SettingView.m_nEditorWidthInBrowser = TS_Browser->Width / 4;
    }
    RE_Edit->Width = SettingView.m_nEditorWidthInBrowser;
    Sp_BrowserRight->Left = (TS_Browser->Width - Sc_Y->Width -
                             SettingView.m_nEditorWidthInBrowser) -
                            Sp_BrowserRight->Width;
    Sp_BrowserRight->Visible = true;
    RE_Edit->Parent = TS_Browser;
    RE_Edit->Left =
        TS_Browser->Width - Sc_Y->Width - SettingView.m_nEditorWidthInBrowser;
    RE_Edit->Align = alRight;
    break;
  case 2:
    // Browser bottom
    if (SettingView.m_nEditorHeightInBrowser == 0 ||
        SettingView.m_nEditorHeightInBrowser >= TS_Browser->Height) {
      SettingView.m_nEditorHeightInBrowser = TS_Browser->Height / 4;
    }
    RE_Edit->Height = SettingView.m_nEditorHeightInBrowser;
    Sp_BrowserBottom->Top = TS_Browser->Height - Pa_BrowserBottom->Height -
                            SettingView.m_nEditorHeightInBrowser -
                            Sp_BrowserBottom->Height;
    Sp_BrowserBottom->Visible = true;
    RE_Edit->Top = TS_Browser->Height - Pa_BrowserBottom->Height -
                   SettingView.m_nEditorHeightInBrowser;
    RE_Edit->Parent = TS_Browser;
    RE_Edit->Align = alBottom;
    break;
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::SetEdTitleBPos() {
  int cardindex = m_Document->SearchCardIndex(m_nTargetCard);
  if (cardindex >= 0) {
    TCard *Card = m_Document->GetCardByIndex(cardindex);

    TCanvas *C;
    if (Fo_FullScreen->Visible) {
      Ed_TitleB->Parent = Fo_FullScreen;
      C = Fo_FullScreen->Canvas;
    } else {
      Ed_TitleB->Parent = Pa_Browser;
      C = PB_Browser->Canvas;
    }

    Ed_TitleB->Font->Style = TFontStyles() << fsBold;
    float SizeX = Card->m_nSize;
    for (int il = 0; il < Card->m_Labels->Count; il++) {
      TCardLabel *Label =
          m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1);
      if (Label->m_bEnable) {
        SizeX *= Label->m_nSize / 100.0f;
      }
    }
    Ed_TitleB->Font->Height = (int)((RE_Edit->Font->Height * SizeX) / 100);
    Ed_TitleB->Font->Name = RE_Edit->Font->Name;
    Ed_TitleB->Font->Charset = RE_Edit->Font->Charset;
    Ed_TitleB->BorderStyle = bsNone;
    C->Font->Style = TFontStyles() << fsBold;
    // Fo_FullScreen->Font->Style = TFontStyles() << fsBold;
    C->Font->Height = Ed_TitleB->Font->Height;
    C->Font->Name = RE_Edit->Font->Name;
    C->Font->Charset = RE_Edit->Font->Charset;
    int lines = CountStr(Ed_TitleB->Text, "\n") + 1;
    int lineheight = C->TextHeight(" ");
    int height = lineheight * lines;
    int width = 0; // PB_Browser->Canvas->TextWidth(Ed_TitleB->Lines->Text);
    for (int i = 0; i < Ed_TitleB->Lines->Count; i++) {
      int w = C->TextWidth(Ed_TitleB->Lines->Strings[i]);
      if (w > width) {
        width = w;
      }
    }

    // Resize window according to IME input state
    {
      // COMPOSITIONFORM CompForm;
      HIMC hImc = ImmGetContext(Ed_TitleB->Handle);
      if (hImc) {
        // Get IME coordinates
        char buf[1024] = {0};
        // M_File->Caption="test";
        long err = ImmGetCompositionString(hImc, GCS_COMPSTR, buf, sizeof(buf));
        if (err != IMM_ERROR_NODATA && err != IMM_ERROR_GENERAL && err > 0) {
          // M_File->Caption = buf;
          long DW = Ed_TitleB->Perform(EM_POSFROMCHAR, Ed_TitleB->SelStart,
                                       (NativeInt)0);
          int w = C->TextWidth(buf);
          if (LOWORD(DW) < 32768) {
            w += LOWORD(DW);
          }
          // M_File->Caption = LOWORD(DW);
          if (w > width) {
            width = w;
          }
        }

        ImmReleaseContext(Handle, hImc);
      }
    }

    C->Font->Height = RE_Edit->Font->Height;

    Ed_TitleB->Width = width;
    Ed_TitleB->Height = height;
    int Space = (m_nFontHeight * SizeX) / 100;
    if (Space < 1) {
      Space = 1;
    }

    int tickerheight = 0;
    bool ticker = false;
    if (SettingView.m_bTickerVisible && Card->m_Lines->Count > 0) {
      for (int il = 0;
           il < SettingView.m_nTickerLines && il < Card->m_Lines->Count; il++) {
        ticker |= Trim(Card->m_Lines->Strings[il]) != "";
      }
      if (ticker) {
        tickerheight = lineheight * 0.8f;
      }
    }

    Ed_TitleB->Left =
        m_CardX[cardindex] - m_CardWidth[cardindex] / 2 + Space / 2;
    Ed_TitleB->Top = m_CardY[cardindex] + m_CardHeight[cardindex] / 2 -
                     Space / 2 - height - tickerheight;
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormShow(TObject *Sender) {
  // Language menu
  TStringList *SL = new TStringList();
  FileListCreator(ExtractFileDir(ParamStr(0)), SL, ".lng", false);
  for (int i = 0; i < SL->Count; i++) {
    TMenuItem *MI = new TMenuItem(MV_ChangeLanguage);
    // MI->Parent = MV_ChangeLanguage;
    MI->Caption = ExtractFileNameOnly(SL->Strings[i]);
    MI->GroupIndex = 1;
    MI->RadioItem = true;
    MI->OnClick = ChangeLanguageOnClick;
    MV_ChangeLanguage->Add(MI);
    MI->Checked = SettingView.m_Language == MI->Caption;
  }
  delete SL;

  ApplyLanguageSetting();

  PC_Client->Align = alClient;
  Pa_Editor->Align = alClient;
  RE_Edit->Align = alClient;
  Ti_Check->Enabled = true;

  Pa_Files->Width = SettingView.m_nFileListWidth; // 145;
  Pa_List->Width = SettingView.m_nCardListWidth;  // 185;
  LB_Link->Height = SettingView.m_nLinkListHeight;
  Pa_EditorRelated->Width = SettingView.m_nEditorRelatedWidth;
  Pa_GlobalSearch->Width = SettingView.m_nGlobalSearchWidth;
  Pa_Client->Align = alClient;
  Pa_Browser->Align = alClient;
  PB_Browser->Align = alClient;

  LB_List->Align = alClient;
  LB_FileList->Align = alClient;

  PC_Client->ActivePage = TS_Browser;

  SB_Status->Visible = SettingView.m_bStatusBar;

  PB_Drawing->Align = alClient;

  PB_Statistics->Align = alClient;

  PB_GlobalSearch->Align = alClient;

  ParentBackground = false;
  Pa_Files->ParentBackground = false;
  Pa_FilesTop->ParentBackground = false;
  Pa_FilesBottom->ParentBackground = false;
  Pa_List->ParentBackground = false;
  Pa_ListTop->ParentBackground = false;
  Pa_ListBottom->ParentBackground = false;
  Pa_Client->ParentBackground = false;
  Pa_Card->ParentBackground = false;
  Pa_Link->ParentBackground = false;
  Pa_BrowserTop->ParentBackground = false;
  Pa_Browser->ParentBackground = false;
  Pa_EditorTop->ParentBackground = false;
  Pa_DrawingTop->ParentBackground = false;
  Pa_StatisticsTop->ParentBackground = false;
  Pa_GlobalSearch->ParentBackground = false;
  Pa_GlobalSearchTop->ParentBackground = false;
  Pa_SearchResultBottom->ParentBackground = false;

  RefreshRecent("");

  if (ParamCount() > 0) {
    LoadSub(ParamStr(1));
  } else {
    m_nTargetCard = 0;
  }

  Randomize();

  if (SettingView.m_nSpecialPaint) {
    SettingView.m_bFileList = false;
    SettingView.m_bCardList = false;
    Pa_Client->Visible = false;
    Pa_Browser->Parent = Fo_Main;
    TB_Zoom->Parent = Fo_Main;
    TB_Zoom->Left = -TB_Zoom->Width;
  }

  FormResize(Sender);
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormClose(TObject *Sender, TCloseAction &Action) {
  Ti_Check->Enabled = false;

  SettingView.m_nFileListWidth = Pa_Files->Width;
  SettingView.m_nCardListWidth = Pa_List->Width;
  SettingView.m_nLinkListHeight = LB_Link->Height;
  SettingView.m_nEditorRelatedWidth = Pa_EditorRelated->Width;
  SettingView.m_nGlobalSearchWidth = Pa_GlobalSearch->Width;

  SettingFile.WriteToIni(Ini, "File");
  SettingView.WriteToIni(Ini, "View");

  CloseEditBox();
  Ed_TitleB->Parent = Pa_Browser;
}

// ---------------------------------------------------------------------------
void TFo_Main::TextEditBackupSub(UnicodeString Action, int CardID, int SelStart,
                                 int SelLength) {
  if (!m_bDoNotBackup) {
    // Undo backup before text edit
    unsigned int tgt = GetTickCount();
    if (tgt > m_nLastModified + UNDOBACKUPSPAN) {
      m_UndoRedo->Backup(m_Document, Action.c_str(), CardID, SelStart,
                         SelLength);
    }
    m_nLastModified = tgt;
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::Ed_TitleChange(TObject *Sender) {
  if (Ed_Title->Tag == 0) {
    TextEditBackupSub(MLText.EditCardTitle);

    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bSelected) {
        if (Sender == Ed_Title) {
          Card->m_Title = Ed_Title->Text;
        } else if (Sender == Ed_TitleB) {
          Card->m_Title = EncodeES(Ed_TitleB->Text);
        }
      }
    }
    Ed_Title->Tag = 1;
    if (Sender == Ed_Title) {
      Ed_TitleB->Text = DecodeES(Ed_Title->Text);
    } else if (Sender == Ed_TitleB) {
      Ed_Title->Text = EncodeES(Ed_TitleB->Text);
    }
    Ed_Title->Tag = 0;

    /*
     TCard *Card = m_Document->GetCard(m_nCurrentCard);
     Ed_Title->Tag = 1;
     if (Sender == Ed_Title){
     Card->m_Title = Ed_Title->Text;
     Ed_TitleB->Text = DecodeES(Ed_Title->Text);
     }else if (Sender == Ed_TitleB){
     Card->m_Title = EncodeES(Ed_TitleB->Text);
     Ed_Title->Text = Card->m_Title;
     }
     Ed_Title->Tag = 0;
     */

    m_Document->RefreshList();
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::RE_EditChange(TObject *Sender) {
  if (m_nCurrentCard >= 0) {
    if (RE_Edit->Tag <= 0) {
      TCard *Card = m_Document->GetCard(m_nCurrentCard);
      if (Card) {
        int len = RE_Edit->SelLength;
        if (m_nLastSelLength >= 0) {
          len = m_nLastSelLength;
          m_nLastSelLength = 0;
        }
        if (RE_Edit->Tag == 0) {
          TextEditBackupSub(MLText.EditCardText, m_nCurrentCard,
                            RE_Edit->SelStart, len);
        }

        m_Document->SetCardText(Card, RE_Edit->Text);
        m_Document->RefreshList();

        if (PC_Client->ActivePage == TS_Editor) {
          // Text edit: update on selection change
          m_nRefreshListCount = m_Document->m_nRefreshListCount;
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::PL_DeleteCardClick(TObject *Sender) {
  BackupSub(PL_DeleteCard->Caption);

  // m_Document->DeleteCard(m_nTargetCard);
  for (int i = m_Document->m_Cards->Count - 1; i >= 0; i--) {
    TCard *Card = m_Document->GetCardByIndex(i);
    if (Card->m_bSelected) {
      m_Document->DeleteCard(Card->m_nID);
    }
  }
  m_nTargetCard = -1;
  m_nTargetCard2 = -1;
}

// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_UndoClick(TObject *Sender) {
  if (Ed_FindCard->Focused()) {
    Ed_FindCard->Undo();
  } else {
    UnicodeString undostr;
    if (!m_UndoRedo->GetCanUndo(undostr)) {
      return;
    }
    m_UndoRedo->Undo(m_Document, m_nCurrentCard, RE_Edit->SelStart,
                     RE_Edit->SelLength, &m_nNextCardID, &m_nNextSelStart,
                     &m_nNextSelLength);
    if (UnicodeString(undostr) == MLText.ArrangeCards) {
      SB_Arrange->Down = false;
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_CutClick(TObject *Sender) {
  if (Ed_FindCard->Focused()) {
    Ed_FindCard->CutToClipboard();
  } else {
    m_nLastModified = 0;
    m_nLastSelLength = RE_Edit->SelLength;
    if (RE_Edit->Focused()) {
      RE_Edit->CutToClipboard();
    } else if (Ed_Title->Focused()) {
      Ed_Title->CutToClipboard();
    } else if (Ed_TitleB->Focused()) {
      Ed_TitleB->CutToClipboard();
    } else if (Ed_LinkTitle->Focused()) {
      Ed_LinkTitle->CutToClipboard();
    } else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
      BackupSub(ME_Cut->Caption);
      m_Document->CopyToClipboard();
      m_bDoNotBackup = true;
      PL_DeleteCardClick(Sender);
      m_bDoNotBackup = false;
    } else if (PC_Client->ActivePage == TS_Drawing) {
      BackupSub(ME_Cut->Caption);
      m_Drawing->Cut();
      ApplyDrawing();
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_CopyClick(TObject *Sender) {
  if (Ed_FindCard->Focused()) {
    Ed_FindCard->CopyToClipboard();
  } else {
    if (RE_Edit->Focused()) {
      RE_Edit->CopyToClipboard();
    } else if (Me_EditorRelated->Focused()) {
      Me_EditorRelated->CopyToClipboard();
    } else if (Ed_Title->Focused()) {
      Ed_Title->CopyToClipboard();
    } else if (Ed_TitleB->Focused()) {
      Ed_TitleB->CopyToClipboard();
    } else if (Ed_LinkTitle->Focused()) {
      Ed_LinkTitle->CopyToClipboard();
    } else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
      m_Document->CopyToClipboard();
    } else if (PC_Client->ActivePage == TS_Drawing) {
      m_Drawing->Copy();
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_PasteClick(TObject *Sender) {
  if (Ed_FindCard->Focused()) {
    Ed_FindCard->PasteFromClipboard();
  } else {
    m_nLastModified = 0;
    m_nLastSelLength = RE_Edit->SelLength;
    if (RE_Edit->Focused()) {
      RE_Edit->PasteFromClipboard();
    } else if (Ed_Title->Focused()) {
      Ed_Title->PasteFromClipboard();
    } else if (Ed_TitleB->Focused()) {
      Ed_TitleB->PasteFromClipboard();
    } else if (Ed_LinkTitle->Focused()) {
      Ed_LinkTitle->PasteFromClipboard();
    } else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
      BackupSub(ME_Paste->Caption);

      // File name max 16 chars
      float cx1, cx2, dummy;
      DispPosToCardPos(0, 0, cx1, dummy);
      DispPosToCardPos(16, 0, cx2, dummy);
      m_Document->PasteFromClipboard(cx2 - cx1);
    } else if (PC_Client->ActivePage == TS_Drawing) {
      BackupSub(ME_Paste->Caption);
      m_Drawing->Paste();
      ApplyDrawing();
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_DeleteClick(TObject *Sender) {
  if (Ed_FindCard->Focused()) {
    Ed_FindCard->ClearSelection();
  } else {
    m_nLastModified = 0;
    m_nLastSelLength = RE_Edit->SelLength;
    if (RE_Edit->Focused()) {
      RE_Edit->ClearSelection();
    } else if (Ed_Title->Focused()) {
      Ed_Title->ClearSelection();
    } else if (Ed_TitleB->Focused()) {
      Ed_TitleB->ClearSelection();
    } else if (Ed_LinkTitle->Focused()) {
      Ed_LinkTitle->ClearSelection();
    } else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
      PL_DeleteCardClick(Sender);
    } else if (PC_Client->ActivePage == TS_Drawing) {
      m_Drawing->DDelete();
      ApplyDrawing();
    }
  }
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MI_NewLinkClick(TObject *Sender) {
  Fo_Select = new TFo_Select(this);
  Fo_Select->m_Document = m_Document;
  Fo_Select->Caption = MLText.SelectLinkDestination;
  Fo_Select->ShowModal();
  if (Fo_Select->ModalResult == mrOk) {
    TCard *Card = m_Document->GetCard(m_nCurrentCard);
    if (Card) {
      BackupSub(MI_NewLink->Caption);
      TLink *Link = m_Document->NewLink();
      Link->m_nFromID = m_nCurrentCard;
      Link->m_nDestID = Fo_Select->m_nID;
    }
  }
  Fo_Select->Release();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::LB_LinkDblClick(TObject *Sender) {
  if (LB_Link->ItemIndex >= 0) {
    TLink *Link = m_Document->GetLinkByIndex(
        (int)(intptr_t)m_LinkIndexes->Items[LB_Link->ItemIndex]);
    if (Link) {
      m_Document->ClearCardSelection();
      if (Link->m_nFromID == m_nCurrentCard) {
        m_nTargetCard = Link->m_nDestID;
      } else if (Link->m_nDestID == m_nCurrentCard) {
        m_nTargetCard = Link->m_nFromID;
      }
      TCard *Card = m_Document->GetCard(m_nTargetCard);
      Card->m_bSelected = true;
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PI_DeleteLinkClick(TObject *Sender) {
  if (LB_Link->ItemIndex >= 0) {
    BackupSub(PI_DeleteLink->Caption);

    m_Document->DeleteLinkByIndex(
        (int)(intptr_t)m_LinkIndexes->Items[LB_Link->ItemIndex]);
  }
}

// ---------------------------------------------------------------------------
bool TFo_Main::BrowserAutoScroll() {
  bool result = false; // Whether coords updated

  // Update start
  m_fBrowserScrollRatio += 0.1f;
  if (m_fBrowserScrollRatio > 0.5f) {
    m_fBrowserScrollRatio = 0.5f;
  }

  // Bring target card, link to center
  if (SB_AutoScroll->Down || m_nScrollTargetX > -65536) {
    float AX = 0.0f;
    float AY = 0.0f;

    if (m_nScrollTargetX <= -65536) {
      if (m_nAnimation > 0 && m_nAnimation != 3) {
        // Slow start for Animation
      } else if (m_nTargetCard >= 0) {
        TCard *Card = m_Document->GetCard(m_nTargetCard);
        if (Card) {
          AX = 0.5f - Card->m_fX;
          AY = 0.5f - Card->m_fY;
        } else {
          goto zoom;
        }
      } else if (m_nTargetLink >= 0) {
        TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
        if (Link) {
          TCard *Card1 = m_Document->GetCard(Link->m_nFromID);
          TCard *Card2 = m_Document->GetCard(Link->m_nDestID);
          AX = 0.5f - (Card1->m_fX + Card2->m_fX) * 0.5f;
          AY = 0.5f - (Card1->m_fY + Card2->m_fY) * 0.5f;
        } else {
          goto zoom;
        }
      } else {
        goto zoom;
      }

      float d = AX * AX + AY * AY;

      m_fBrowserScrollLastD = d;

      // float Zoom = pow(2, TB_Zoom->Position / 2000.0f);
      // AX /= Zoom;
      // AY /= Zoom;

      AX *= 0.9;
      AY *= 0.9;

      AX *= 10000;
      AY *= 10000;

      AX = 5000 - AX;
      AY = 5000 - AY;
    } else {
      AX = m_nScrollTargetX;
      AY = m_nScrollTargetY;
    }

    int xp = Sc_X->Position * (1.0f - m_fBrowserScrollRatio) +
             AX * m_fBrowserScrollRatio;
    int yp = Sc_Y->Position * (1.0f - m_fBrowserScrollRatio) +
             AY * m_fBrowserScrollRatio;
    if (Sc_X->Position != xp || Sc_Y->Position != yp) {
      PB_Browser->Tag = 1; // Drawing disabled
      Sc_X->Position = xp;
      Sc_Y->Position = yp;
      PB_Browser->Tag = 0; // Drawing resumed
      result = true;
    } else {
      m_nScrollTargetX = -65536;
      m_nScrollTargetY = -65536;
    }
  }

zoom:
  // Drawing disabled
  if (SB_AutoZoom->Down) {
    // Variance calculation
    float XD = 0.0f, YD = 0.0f;
    float CX = 0.5f, CY = 0.5f;

    if (SB_AutoScroll->Down) {
      if (m_nTargetCard >= 0) {
        // When AutoScroll ON and card selected
        // Zoom centered on selected card
        int cardindex = m_Document->SearchCardIndex(m_nTargetCard);
        if (cardindex >= 0) {
          CX = m_CardX[cardindex] / m_nBrowserWidth;
          CY = m_CardY[cardindex] / m_nBrowserHeight;
        }
      }
    }

    int count = 0;
    // bool coverd = true;
    float maxd = 0.0f;
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (m_CardVisible[i] &&
          (m_CardRelated[i] || (m_nTargetCard == -1 && m_nTargetLink == -1) ||
           (m_bShowRecent &&
            Card->m_nViewedOrder >= m_Document->m_Cards->Count - 3) ||
           Card->m_bSelected || SettingView.m_bLinkLimitation)) {
        float f;
        f = m_CardX[i] / m_nBrowserWidth - CX;
        maxd = fabs(f) > maxd ? fabs(f) : maxd;
        XD += f * f;
        f = m_CardY[i] / m_nBrowserHeight - CY;
        maxd = fabs(f) > maxd ? fabs(f) : maxd;
        YD += f * f;

        // coverd &= m_CardX[i] >=0.1f && m_CardX[i] <= 0.9f;
        // coverd &= m_CardY[i] >=0.1f && m_CardY[i] <= 0.9f;

        count++;
      }
    }
    if (count && XD > 0.0f && YD > 0.0f) {
      XD = sqrt(XD / count);
      YD = sqrt(YD / count);

      if (YD > XD) {
        XD = YD;
      }

      if (XD > 0.0f) {
        // Drawing resumed
        XD /= pow(2, TB_Zoom->Position / 2000.0f);   // Zoom XD
        maxd /= pow(2, TB_Zoom->Position / 2000.0f); // Zoom maxd
        float target = m_fZoomSD / XD; // Target m_fZoomSD for Drawing
        if (target > 0.4f / maxd) {    // If too large, cards would overflow
          target = 0.4f / maxd;        // Limit to 0.4 for Drawing
        }

        // Zoom so cards fit in view
        // pow(2, targetzoom / 2000.0f) = target;
        // pow(x,y) = exp(log(x) * y)
        // exp(log(x) * targetzoom / 200.0f) = target;
        // log(x) * targetzoom / 2000.0f = log(target);
        // targetzoom = log(target) / log(x) * 2000.0f;
        int targetzoom = (int)(log(target) / log(2) * 2000.0f);
        if (targetzoom < TB_Zoom->Min) {
          targetzoom = TB_Zoom->Min;
        } else if (targetzoom > TB_Zoom->Max) {
          targetzoom = TB_Zoom->Max;
        }

        // float zoomratio = 0.3f;
        float zoomratio = m_fBrowserScrollRatio;

        int zp = (int)(TB_Zoom->Position * (1.0f - zoomratio) +
                       targetzoom * zoomratio);
        /*
         if (abs(TB_Zoom->Position - zp) > 2){
         if (!coverd){
         float f = 1.0f - 10.0f / abs(TB_Zoom->Position - zp);
         if (f < 0.99f && f > 0.9f){
         m_fZoomSD = m_fZoomSD * f;
         }
         }
         }
         */
        if (TB_Zoom->Position != zp) {
          PB_Browser->Tag = 1; // Drawing disabled
          TB_Zoom->Position = zp;
          PB_Browser->Tag = 0; // Drawing resumed
          result = true;
        }

        /*
         PB_Browser->Tag = 1; // Drawing disabled
         if (XD < 0.20f){
         TB_Zoom->Position = TB_Zoom->Position + 1;
         }else if (XD > 0.21f){
         TB_Zoom->Position = TB_Zoom->Position - 1;
         }
         PB_Browser->Tag = 0; // Drawing resumed
         */
      }
    }
  }
  return result;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::LB_ListDblClick(TObject *Sender) {
  PC_Client->ActivePage = TS_Editor;
}
// ---------------------------------------------------------------------------
void __fastcall TFo_Main::Ed_SizeChange(TObject *Sender) {
  RE_Edit->Tag = 1;
  RE_Edit->Font->Size = UD_Size->Position;
  RE_Edit->Tag = 0;
  Me_EditorRelated->Font->Size = UD_Size->Position;
  LB_List->Font->Size = UD_Size->Position;
  LB_FileList->Font->Size = UD_Size->Position;
  LB_Link->Font->Size = UD_Size->Position;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::FormKeyUp(TObject *Sender, WORD &Key,
                                    TShiftState Shift) {
  if (PC_Client->ActivePage == TS_Browser) {
    if (Key == 32) {
      // Shift
      m_bShowRecent = false;
      m_bRedrawRequested = true;
    }
  }
}




void TFo_Main::Redraw() {
  // Full redraw
  if (PC_Client->ActivePage == TS_Browser) {
    PB_BrowserPaint(this);
  }
  RefreshLabel();
}
// ---------------------------------------------------------------------------

void TFo_Main::ExitFullScreen() {
  if (Fo_FullScreen->Visible) {
    Fo_FullScreen->Close();
    CloseEditBox();
    CloseTextEditBox();
  }
}

void __fastcall TFo_Main::MV_FullScreenClick(TObject *Sender) {
  if (Fo_RandomAnimation->Visible) {
    Fo_RandomAnimation->SetFocus();
    Fo_RandomAnimation->m_bDontClose = true;
  }

  if (Fo_FullScreen->Visible) {
    ExitFullScreen();
  } else {
    PC_Client->ActivePage = TS_Browser;
    Fo_FullScreen->OnDblClick = PB_BrowserDblClick;
    Fo_FullScreen->OnMouseDown = PB_BrowserMouseDown;
    Fo_FullScreen->OnMouseMove = PB_BrowserMouseMove;
    Fo_FullScreen->OnMouseUp = PB_BrowserMouseUp;
    Fo_FullScreen->OnKeyDown = FormKeyDown;
    Fo_FullScreen->OnKeyUp = FormKeyUp;
    Fo_FullScreen->Bu_Enter->OnClick = Bu_EnterClick;
    Fo_FullScreen->OnPaint = PB_BrowserPaint;
    Fo_FullScreen->Show();
  }

  if (Fo_RandomAnimation->Visible) {
    Fo_RandomAnimation->SetFocus();
    Fo_RandomAnimation->BringToFront();
    Fo_RandomAnimation->m_bDontClose = false;
  } else {
    TB_Zoom->SetFocus();
  }

  /*
   if (MV_FullScreen->Checked){
   // Taskbar display
   HWND hTrayWnd = FindWindow("Shell_TrayWnd", NULL);
   ShowWindow(hTrayWnd, SW_HIDE);

   // Each item display
   Pa_Card->Visible = false;
   Sp_Left->Visible = false;
   LB_List->Visible = false;
   Sp_Left->Align = alNone;
   LB_List->Align = alNone;
   Pa_Client->Align = alNone;
   Pa_Client->Align = alClient;

   // Window size
   WindowState = wsNormal;
   Width = Screen->Width;
   Height = Screen->Height;
   Top = 0;
   Left = 0;
   ShowMessage(Pa_Client->Left);
   ShowMessage(PC_Client->Left);
   }else{
   // Taskbar display
   HWND hTrayWnd = FindWindow("Shell_TrayWnd", NULL);
   ShowWindow(hTrayWnd, SW_SHOWNORMAL);

   // Each item display
   Sp_Left->Visible = true;
   LB_List->Visible = true;
   Pa_Card->Visible = true;
   }
   */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_ToolLabelClick(TObject *Sender) {
  m_nToolLabel = 0;
  Po_Label->Tag = -1;
  PL_NoLabel->Checked = true;

  TPoint P;
  P.x = SB_ToolLabel->Left;
  P.y = SB_ToolLabel->Top;
  P = Pa_BrowserTop->ClientToScreen(P);
  Po_Label->Popup(P.x, P.y);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_ToolLinkLabelClick(TObject *Sender) {
  m_nToolLinkLabel = 0;
  Po_LinkLabel->Tag = -1;
  PL_NoLinkLabel->Checked = true;

  TPoint P;
  P.x = SB_ToolLinkLabel->Left;
  P.y = SB_ToolLinkLabel->Top;
  P = Pa_BrowserTop->ClientToScreen(P);
  Po_LinkLabel->Popup(P.x, P.y);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PL_CardPropertyClick(TObject *Sender) {
  TCard *Card = m_Document->GetCard(m_nTargetCard);
  if (Card) {
    UnicodeString S = "";
    UnicodeString ret = "\n";

    // Title
    S += MLText.Title + UnicodeString(" : ") + Card->m_Title;
    S += ret;

    S += ret;

    // Label
    if (Card->m_Labels->Count) {
      S += MLText.Labels + UnicodeString(" : ");
      for (int i = 0; i < Card->m_Labels->Count; i++) {
        if (i > 0) {
          S += ", ";
        }
        S += m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(i) - 1)
                 ->m_Name;
      }
      S += ret;
    } else {
      S += MLText.Labels + UnicodeString(" : 0");
      S += ret;
    }

    S += ret;

    // LinkNum
    int count = 0;
    for (int il = 0; il < m_Document->m_Links->Count; il++) {
      TLink *Link = m_Document->GetLinkByIndex(il);
      if (Link->m_nFromID == Card->m_nID || Link->m_nDestID == Card->m_nID) {
        count++;
      }
    }
    S += MLText.Links + UnicodeString(" : ") + IntToStr(count);
    S += ret;

    S += ret;

    S += MLText.Size + UnicodeString(" : ") +
         SizeToStr(Card->m_Lines->Text.Length());
    S += ret;

    S += ret;

    // Date
    S +=
        MLText.Created + UnicodeString(" : ") + DateTimeToStr(Card->m_fCreated);
    S += ret;
    S += MLText.Edited + UnicodeString(" : ") + DateTimeToStr(Card->m_fUpdated);
    S += ret;
    S += MLText.Viewed + UnicodeString(" : ") + DateTimeToStr(Card->m_fViewed);

    Application->MessageBox(
        UnicodeString(S).c_str(),
        UnicodeString(DeleteActionKey(PL_CardProperty->Caption)).c_str(), 0);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MH_CheckLatestClick(TObject *Sender) {
  ExitFullScreen();

  ShellExecute(Handle, NULL, UnicodeString(ReleaseURL).c_str(), NULL, NULL,
               SW_SHOW);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MH_EnableSpecialMenuClick(TObject *Sender) {
  M_Special->Visible = true;
  /*
   // Link direction
   for (int i = 0 ; i < m_Document->m_Links->Count ; i++){
   TLink *Link = m_Document->GetLinkByIndex(i);
   int bak = Link->m_nFromID;
   Link->m_nFromID = Link->m_nDestID;
   Link->m_nDestID = bak;
   }
   m_Document->RefreshLink();
   // */

  /*
   // cardTitle for read-aloud
   for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
   TCard *Card = m_Document->GetCardByIndex(i);
   UnicodeString S;
   for (int i2 = 0 ; i2 < Card->m_Title.Length() ; i2++){
   S += "?";
   }
   Card->m_Title = S;

   }
   m_Document->m_FN = "";
   // */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVL_LabelCircleClick(TObject *Sender) {
  SettingView.m_bLabelCircleVisible = !SettingView.m_bLabelCircleVisible;
  SettingView.m_bLabelRectangleVisible = false;
  m_bRedrawRequested = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MVL_LabelRectangleClick(TObject *Sender) {
  SettingView.m_bLabelRectangleVisible = !SettingView.m_bLabelRectangleVisible;
  SettingView.m_bLabelCircleVisible = false;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Pa_ListTopResize(TObject *Sender) {
  UD_Sort->Left = Pa_ListTop->Width - UD_Sort->Width - 4;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::UD_SortClick(TObject *Sender, TUDBtnType Button) {
  if (m_nTargetCard < 0) {
    // No card selected
    return;
  }

  if (Button == btNext) {
    // Search?
    for (int i = 1; i < LB_List->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bSelected) {
        TCard *Card2 = NULL;
        int swapindex;
        if (!TreeMode()) {
          // Normal
          Card2 = m_Document->GetCardByIndex(i - 1);
          swapindex = i - 1;
        } else {
          // Tree structure
          for (int i2 = i - 1; i2 >= 0; i2--) {
            TCard *Card3 = m_Document->GetCardByIndex(i2);
            if (Card3->m_nParentID == Card->m_nParentID) {
              Card2 = Card3;
              swapindex = i2;
              break;
            }
          }
        }
        if (Card2)
          if (!Card2->m_bSelected) {
            m_Document->SwapCard(i, swapindex);
            if (LB_List->ItemIndex == i) {
              LB_List->ItemIndex = swapindex;
            }
          }
      }
    }
  } else if (Button == btPrev) {
    // Update
    for (int i = LB_List->Count - 2; i >= 0; i--) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bSelected) {
        TCard *Card2 = NULL;
        int swapindex;
        if (!TreeMode()) {
          // Normal
          Card2 = m_Document->GetCardByIndex(i + 1);
          swapindex = i + 1;
        } else {
          // Tree structure
          for (int i2 = i + 1; i2 < LB_List->Count; i2++) {
            TCard *Card3 = m_Document->GetCardByIndex(i2);
            if (Card3->m_nParentID == Card->m_nParentID) {
              Card2 = Card3;
              swapindex = i2;
              break;
            }
          }
        }
        if (Card2)
          if (!Card2->m_bSelected) {
            m_Document->SwapCard(i, swapindex);
            if (LB_List->ItemIndex == i) {
              LB_List->ItemIndex = swapindex;
            }
          }
      }
    }
  }
  /*
   int cardindex = m_Document->SearchCardIndex(m_nTargetCard);

   if (Button == btNext){
   // Upward

   if (cardindex <= 0){
   // Cannot go further
   return;
   }

   // Swap with previous card
   m_Document->SwapCard(cardindex, cardindex - 1);
   LB_List->ItemIndex = cardindex - 1;
   }else if (Button == btPrev){
   // Downward

   if (cardindex >= m_Document->m_Cards->Count - 1){
   // Cannot go further
   return;
   }

   // Swap with next card
   m_Document->SwapCard(cardindex, cardindex + 1);
   LB_List->ItemIndex = cardindex + 1;
   }
   */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::RE_EditMouseDown(TObject *Sender, TMouseButton Button,
                                           TShiftState Shift, int X, int Y) {
  if (Shift.Contains(ssCtrl)) {
    // Ctrl+click

    // Get current line
    int line = 0;
    int count = 0;
    while (line < RE_Edit->Lines->Count) {
      count += WideString(RE_Edit->Lines->Strings[line]).Length() + 2;
      if (RE_Edit->SelStart >= count) {
        line++;
      } else {
        break;
      }
    }
    // Current line
    if (line < RE_Edit->Lines->Count) {
      UnicodeString Str = RE_Edit->Lines->Strings[line];
      if (IsFileNameOrURL(Str)) {
        // Open
        OpenExtLink(Str);
      }
    }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::OpenExtLink(UnicodeString S) {
  ShellExecute(Handle, NULL, S.c_str(), NULL, NULL, SW_SHOW);
}

// ---------------------------------------------------------------------------
void TFo_Main::FreeMIExtLink() {
  if (MI_ExtLink) {
    for (int i = 0; i < MI_ExtLink->Count; i++) {
      delete (TMenuItem *)MI_ExtLink->Items[i];
    }
    delete MI_ExtLink;
    MI_ExtLink = NULL;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PM_ExtLinkClick(TObject *Sender) {
  ExitFullScreen();
  TMenuItem *MI = (TMenuItem *)Sender;
  OpenExtLink(MI->Caption);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_NewExtLinkClick(TObject *Sender) {
  if (OD_Ext->Execute()) {
    RE_Edit->Lines->Add(OD_Ext->FileName);
    /*
     if (PC_Client->ActivePage == TS_Editor){
     // Editor displayed
     }else{
     // Editor not displayed
     }
     */
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::UD_SortExit(TObject *Sender) {
  UD_Sort->Position = 0;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVT_TextClick(TObject *Sender) {
  SettingView.m_bTextVisible = !SettingView.m_bTextVisible;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVT_CenteringClick(TObject *Sender) {
  SettingView.m_bTextCentering = !SettingView.m_bTextCentering;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PE_CutToNewCardClick(TObject *Sender) {
  BackupSub(PE_CutToNewCard->Caption);

  m_bDoNotBackup = true;

  // Cut
  ME_CutClick(Sender);

  // Create new card
  MI_NewCardClick(Sender);

  // Refresh card display
  Ti_CheckTimer(Sender);

  // Paste
  RE_Edit->SetFocus();
  ME_PasteClick(Sender);

  m_bDoNotBackup = false;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ed_TitleBExit(TObject *Sender) {
  CloseEditBox();
  TB_Zoom->SetFocus();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ed_TitleBEnter(TObject *Sender) {
  Ed_TitleB->SelectAll();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::FormResize(TObject *Sender) {
  Bu_Enter->Left = Width + 100;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_EnterClick(TObject *Sender) {
  if (Ed_TitleB->Visible) {
    CloseEditBox();
  } else if (m_nTargetCard >= 0 && PC_Client->ActivePage == TS_Browser) {
    // Create card from link
    BrowserNewBrotherCard();
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::CloseEditBox() {
  Ed_TitleB->Visible = false;
  m_nLastModified = 0;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ed_TitleBKeyDown(TObject *Sender, WORD &Key,
                                           TShiftState Shift) {
  if (Key == 27) {
    CloseEditBox();
  }
  if (Key == 13 && Shift.Contains(ssCtrl) && !Ed_TitleB->WantReturns) {
    Ed_TitleB->WantReturns = true;
  }
  if (Key == 13) {
    // IME composition: keep cursor in editor when composing (Cursor stays in
    // editor)
    TPoint caret = Ed_TitleB->CaretPos;
    UnicodeString S = Ed_TitleB->Text;
    Ed_TitleB->Text = "";
    Ed_TitleB->Text = S;
    Ed_TitleB->CaretPos = caret;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PE_CutToNewCardTitleClick(TObject *Sender) {
  BackupSub(PE_CutToNewCardTitle->Caption);

  m_bDoNotBackup = true;

  // Cut
  ME_CutClick(Sender);

  TMemo *Me = new TMemo(this);
  Me->Parent = this;
  Me->WordWrap = false;
  Me->PasteFromClipboard();

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
    }
  }

  delete Me;

  m_bDoNotBackup = false;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::TB_ZoomChange(TObject *Sender) {
  m_bRedrawRequested = true;
  // PB_BrowserPaint(Sender);
  if (PB_Browser->Tag == 0) {
    if (!m_bSkipAutoZoom) {
      SB_AutoZoom->Down = false;
    }
    m_bSkipAutoZoom = false;
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_ShuffleClick(TObject *Sender) {
  BackupSub(Bu_Shuffle->Caption);

  // Card search by title
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    if (!Card->m_bFixed && m_CardAssign[i] == i) {
      Card->m_fX = GaussianRand() * 0.21 + 0.5;
      Card->m_fY = GaussianRand() * 0.21 + 0.5;
    }
  }
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    if (m_CardAssign[i] != i) {
      TCard *Card = m_Document->GetCardByIndex(i);
      TCard *Card2 = m_Document->GetCardByIndex(m_CardAssign[i]);
      Card->m_fX = Card2->m_fX;
      Card->m_fY = Card2->m_fY;
    }
  }

  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MDS_ShuffleClick(TObject *Sender) {
  BackupSub(MD_SortCard->Caption + UnicodeString(" ") +
            ((TMenuItem *)Sender)->Caption);
  m_Document->SortCards(-1, ((TMenuItem *)Sender)->Tag);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MH_DemoClick(TObject *Sender) {
  m_DemoString = m_DemoStrings->Strings[m_nDemoIndex++];
  if (m_nDemoIndex >= m_DemoStrings->Count) {
    m_nDemoIndex = 0;
  }
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ed_FindCardEnter(TObject *Sender) {
  Fo_FullScreen->Bu_Enter->Default = false;
  Bu_Enter->Default = false;
  Bu_FindCard->Default = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ed_FindCardExit(TObject *Sender) {
  Bu_FindCard->Default = false;
  Bu_Enter->Default = true;
  Fo_FullScreen->Bu_Enter->Default = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_FindCardClick(TObject *Sender) {
  Ed_FindCard->SelectAll();
  bSearchRequest = 0x1 | 0x4; // Card title search
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ed_FindCardChange(TObject *Sender) {
  SearchKeyword = Ed_FindCard->Text;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_ShapeClick(TObject *Sender) {
  TPoint P;
  P.x = SB_Shape->Left;
  P.y = SB_Shape->Top;
  P = Pa_Card->ClientToScreen(P);

  TCard *Card = m_Document->GetCard(m_nTargetCard);
  if (Card) {
    Fo_Shape->IL_Shape = IL_Shape;
    Fo_Shape->m_nMode = 0;
    Fo_Shape->m_nItemIndex = Card->m_nShape;
    Fo_Shape->m_bIgnoreChange = true;
    Fo_Shape->Left = P.x;
    Fo_Shape->Top = P.y;
    Fo_Shape->Show();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ed_CardSizeChange(TObject *Sender) {
  if (m_nTargetCard >= 0 && Ed_CardSize->Tag == 0) {
    TextEditBackupSub(MLText.ChangeSize);

    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bSelected) {
        Card->m_nSize =
            (int)(100 * exp(UD_CardSize->Position / (8 / log(4))) + 100) - 100;
      }
    }
    /*
     TCard *Card = m_Document->GetCard(m_nTargetCard);
     Card->m_nSize = (int)(100 * exp(UD_CardSize->Position / (8 / log(4))) +
     100) - 100;
     */
    m_Document->RefreshList();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PC_ClientChange(TObject *Sender) {
  if (PC_Client->ActivePage == TS_Editor) {
    CloseTextEditBox();
  }
  /*
   if (PC_Client->ActivePage == TS_Drawing){
   m_Drawing->SetTool(0);
   SB_DCursor->Down = true;
   }
   */
  m_fStatisticsPos = 0.5f; // Statistics position default 0.5
                           /*
                            if (PC_Client->ActivePage == TS_Browser){
                            if (SettingView.m_bEditInBrowserAlways){
                            SetTextEditPos();
                            }
                            }
                            */
  // Pa_ClientBottom->Visible = PC_Client->ActivePage == TS_Editor;
  /*
   if (PC_Client->ActivePage == TS_Editor){
   RE_Edit->SetFocus();
   }
   */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MV_StatusBarClick(TObject *Sender) {
  SettingView.m_bStatusBar = !SettingView.m_bStatusBar;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::RE_EditKeyUp(TObject *Sender, WORD &Key,
                                       TShiftState Shift) {
  RefreshLaStatus();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::RE_EditMouseUp(TObject *Sender, TMouseButton Button,
                                         TShiftState Shift, int X, int Y) {
  RefreshLaStatus();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_SelectAllClick(TObject *Sender) {
  if (RE_Edit->Focused()) {
    RE_Edit->SelectAll();
  } else if (Me_EditorRelated->Focused()) {
    Me_EditorRelated->SelectAll();
  } else if (Ed_Title->Focused()) {
    Ed_Title->SelectAll();
  } else if (Ed_TitleB->Focused()) {
    Ed_TitleB->SelectAll();
  } else if (Ed_LinkTitle->Focused()) {
    Ed_LinkTitle->SelectAll();
  } else if (Ed_FindCard->Focused()) {
    Ed_FindCard->SelectAll();
  } else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      m_Document->GetCardByIndex(i)->m_bSelected = true;
    }
  } else if (PC_Client->ActivePage == TS_Drawing) {
    m_Drawing->SelectAll();
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MF_PrintImageClick(TObject *Sender) {
  PC_Client->ActivePage = TS_Browser;
  if (PD_Image->Execute()) {
    TPrinter *Prntr = Printer();

    // Print
    int printwidth = Prntr->PageWidth * 0.95; // 95% of page width
    int printheight = Prntr->PageHeight * 0.95;

    if (printheight > (printwidth * PB_Browser->Height) / PB_Browser->Width) {
      printheight = (printwidth * PB_Browser->Height) / PB_Browser->Width;
    }
    if (printwidth > (printheight * PB_Browser->Width) / PB_Browser->Height) {
      printwidth = (printheight * PB_Browser->Width) / PB_Browser->Height;
    }

    // Print
    RECT printr = Rect(Prntr->PageWidth / 2 - printwidth / 2,
                       Prntr->PageHeight / 2 - printheight / 2,
                       Prntr->PageWidth / 2 + printwidth / 2,
                       Prntr->PageHeight / 2 + printheight / 2);

    // Paste Drawing
    for (int i = 0; i < PD_Image->Copies; i++) {
      HRGN MyRgn;
      MyRgn =
          ::CreateRectRgn(printr.left, printr.top, printr.right, printr.bottom);

      Prntr->BeginDoc();
      ::SelectClipRgn(Prntr->Canvas->Handle, MyRgn);

      m_nBrowserWidth = printwidth;
      m_nBrowserHeight = printheight;
      m_nXOffset = printr.left;
      m_nYOffset = printr.top;
      m_fFontZoom = (printwidth * 1.0) / PB_Browser->Width;
      m_nBGColor = 0xffffff;
      m_nFGColor = 0x0;
      PaintSub(Prntr->Canvas);
      Prntr->EndDoc();

      ::DeleteObject(MyRgn);
    }
  }
  PB_BrowserPaint(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MF_PrintTextClick(TObject *Sender) {
  if (PD_Text->Execute()) {
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_ChangeBackgroundColorClick(TObject *Sender) {
  CD->Color = (TColor)SettingView.m_nBackgroundColor;
  if (CD->Execute()) {
    SettingView.m_nBackgroundColor = (int)CD->Color;
    PB_BrowserPaint(Sender);
    if (Pa_GlobalSearch->Visible) {
      PB_GlobalSearchPaint(Sender);
    }
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVO_ChangeFourgroundColorClick(TObject *Sender) {
  CD->Color = (TColor)SettingView.m_nFourgroundColor;
  if (CD->Execute()) {
    SettingView.m_nFourgroundColor = (int)CD->Color;
    PB_BrowserPaint(Sender);
  }
}


void __fastcall TFo_Main::MVL_LabelNameClick(TObject *Sender) {
  SettingView.m_bLabelNameVisible = !SettingView.m_bLabelNameVisible;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MVC_EnglishClick(TObject *Sender) {
  MVC_English->Checked = true;
  SettingView.m_Language = "";
  ShowMessage("Please restart Frieve Editor.");
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::ChangeLanguageOnClick(TObject *Sender) {
  TMenuItem *MI = (TMenuItem *)Sender;
  MI->Checked = true;
  SettingView.m_Language = MI->Caption;
  ApplyLanguageSetting();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVS_ScoreClick(TObject *Sender) {
  SettingView.m_bScore = !SettingView.m_bScore;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MDS_ScoreClick(TObject *Sender) {
  BackupSub(MD_SortCard->Caption + UnicodeString(" ") +
            ((TMenuItem *)Sender)->Caption);
  m_Document->SortCards(4, ((TMenuItem *)Sender)->Tag);
}

// ---------------------------------------------------------------------------
TCardImage *TFo_Main::GetImage(int index) {
  return (TCardImage *)m_ImageList->Items[index];
}

// ---------------------------------------------------------------------------
TCardImage *TFo_Main::SearchImage(UnicodeString FN) {
  for (int ii = 0; ii < m_ImageList->Count; ii++) {
    TCardImage *Image = GetImage(ii);
    if (Image->m_FN == FN) {
      return Image;
    }
  }
  return NULL;
}

// ---------------------------------------------------------------------------
bool TFo_Main::UpdateImageList() {
  // Load images in use but not loaded; discard unused images

  bool result = false;

  // Reset used flag
  for (int i = 0; i < m_ImageList->Count; i++) {
    GetImage(i)->m_bUsed = false;
  }

  // Card loop
  if (SettingView.m_bImageVisible)
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_ImageFN != "") { // Card->m_bVisible &&
        // Search image from image list
        TCardImage *Image = SearchImage(Card->m_ImageFN);
        if (Image) {
          // Found
          Image->m_bUsed = true;
        } else if (Card->m_bVisible) {
          // Not found and displayed -> load
          // Screen->Cursor = crHourGlass;
          if (!result || SettingView.m_bLoadImageatOnce) {
            m_ImageList->Add(new TCardImage(Card->m_ImageFN));
          }
          result = true;
        }
      }
    }

  // Discard unused images
  for (int i = m_ImageList->Count - 1; i >= 0; i--) {
    TCardImage *Image = GetImage(i);
    if (!Image->m_bUsed) {
      delete Image;
      m_ImageList->Delete(i);
      result = true;
    }
  }

  return result;
}

// ---------------------------------------------------------------------------
TCardVideo *TFo_Main::GetVideo(int index) {
  return (TCardVideo *)m_VideoList->Items[index];
}

// ---------------------------------------------------------------------------
TCardVideo *TFo_Main::SearchVideo(UnicodeString FN) {
  for (int ii = 0; ii < m_VideoList->Count; ii++) {
    TCardVideo *Video = GetVideo(ii);
    if (Video->m_FN == FN) {
      return Video;
    }
  }
  return NULL;
}

// ---------------------------------------------------------------------------
bool TFo_Main::UpdateVideoList() {
  // Load videos in use but not loaded; discard unused videos

  bool result = false;

  // Reset used flag
  for (int i = 0; i < m_VideoList->Count; i++) {
    GetVideo(i)->m_bUsed = false;
  }

  // Card loop
  if (SettingView.m_bVideoVisible)
    for (int i = 0; i < m_Document->m_Cards->Count; i++) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bVisible && Card->m_VideoFN != "") {
        // Search video from video list
        TCardVideo *Video = SearchVideo(Card->m_VideoFN);
        if (Video) {
          // Found
          Video->m_bUsed = true;
        } else {
          // Not found -> load
          Screen->Cursor = crHourGlass;
          if (!result || SettingView.m_bLoadImageatOnce) {
            m_VideoList->Add(new TCardVideo(Card->m_VideoFN));
          }
          result = true;
        }
      }
    }

  // Discard unused videos
  for (int i = m_VideoList->Count - 1; i >= 0; i--) {
    TCardVideo *Video = GetVideo(i);
    if (!Video->m_bUsed) {
      delete Video;
      m_VideoList->Delete(i);
      result = true;
    }
  }

  return result;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVC_ImageClick(TObject *Sender) {
  SettingView.m_bImageVisible = !SettingView.m_bImageVisible;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVI_ImageLimitationClick(TObject *Sender) {
  TMenuItem *MI = (TMenuItem *)Sender;
  SettingView.m_nImageLimitation = MI->Tag;
  m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVR_ReadClick(TObject *Sender) {
  SettingView.m_bRead = !SettingView.m_bRead;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVR_ReadSettingClick(TObject *Sender) {}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVR_ChangeAgentClick(TObject *Sender) {}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVT_WordWrapClick(TObject *Sender) {
  SettingView.m_bTextWordWrap = !SettingView.m_bTextWordWrap;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVS_ScoreClickType(TObject *Sender) {
  SettingView.m_ScoreType = ((TMenuItem *)Sender)->Tag;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_TestClick(TObject *Sender) {
  int doccount = m_Document->m_Cards->Count;

  TWideStringList *WS = new TWideStringList();
  for (int i = 0; i < doccount; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    WS->Add(Card->m_Title + "\n" + Card->m_Lines->Text);
  }
  unsigned int t;
  t = GetTickCount();
  float fdummy;
  bool bdummy = false;
  TTextDecomposer *TD =
      new TTextDecomposer(WS, 10, 100, 0.0f, 0.0f, fdummy, bdummy);
  RE_Edit->Clear();
  RE_Edit->Lines->BeginUpdate();
  RE_Edit->Lines->Add(UnicodeString("Time elapsed : ") +
                      IntToStr((int)(GetTickCount() - t)) + "ms");
  RE_Edit->Lines->Add(UnicodeString("Items : ") + IntToStr(TD->m_nMaxSN));
  RE_Edit->Lines->Add("");
  for (int i = 0; i < TD->m_nMaxCombi; i++) {
    RE_Edit->Lines->Add(UnicodeString("<<<<<  ") + IntToStr(i + 1) +
                        "-gram  >>>>>");
    RE_Edit->Lines->Add("");
    WideString WS;
    for (int i2 = 0; i2 < TD->m_Gram[i]->Count; i2++) {
      if (TD->m_Gram[i]->Enabled(i2)) {
        WS += TD->m_Gram[i]->Strings(i2) + WideString("(") +
              IntToStr(TD->m_Gram[i]->Values(i2)) + WideString("),");
      }
      // RE_Edit->Lines->Add(TD->m_Gram[i]->Strings(i2));
    }
    RE_Edit->Lines->Add(WS);
    RE_Edit->Lines->Add("");
  }
  /*
   for (int ii = 0 ; ii < 100 ; ii++){
   int idx = rand() % TD->m_Gram[0]->Count;
   for (int i = 1 ; i < TD->m_nMaxCombi ; i++){
   if (TD->m_Gram[i - 1]->From(idx) == -1){
   break;
   }else{
   WideString WS = TD->m_Gram[i - 1]->Strings(idx) + " : ";
   for (int i2 = TD->m_Gram[i - 1]->From(idx) ; i2 <= TD->m_Gram[i - 1]->To(idx)
   ; i2++){ WS += TD->m_Gram[i]->Strings(i2) + ", ";
   }
   RE_Edit->Lines->Add(WS);
   idx = rand() % (TD->m_Gram[i - 1]->To(idx) + 1 - TD->m_Gram[i -
   1]->From(idx)) + TD->m_Gram[i - 1]->From(idx);
   }
   }
   RE_Edit->Lines->Add("");
   }

   {
   // Search operation check
   for (int ii = 0 ; ii < TD->m_Gram[0]->Count ; ii++){
   int idx = TD->m_Gram[0]->Search(0, TD->m_Gram[0]->Count - 1,
   TD->m_Gram[0]->Strings(ii)); if (idx != ii){ RE_Edit->Lines->Add(IntToStr(ii)
   + " : " + IntToStr(idx));
   }
   }
   }

   {
   // Combinations where Enabled is false
   for (int i = 0 ; i < TD->m_nMaxCombi ; i++){
   WideString WS;
   for (int ii = 0 ; ii < TD->m_Gram[i]->Count ; ii++){
   if (!TD->m_Gram[i]->Enabled(ii)){
   WS += TD->m_Gram[i]->Strings(ii);
   }
   }
   RE_Edit->Lines->Add(WS);
   RE_Edit->Lines->Add("");
   }
   }
   */

  RE_Edit->Lines->EndUpdate();

  // Doc-word occurrence probability storage
  t = GetTickCount();
  TSMatrix *SM = new TSMatrix();

  // Fill SM from TD result
  // Document loop
  for (int id = 0; id < doccount; id++) {
    // Character loop
    WideString doc = WideLowerCase(WS->Strings(id));
    int doclen = doc.Length();
    for (int ic = 0; ic < doclen; ic++) {
      int len = 1;
      int start = 0;
      int end = TD->m_Gram[0]->Count - 1;
      while (start >= 0) {
        TWSandValueList *WSVL = TD->m_Gram[len - 1];
        WideString c = doc.SubString(ic + 1, len);
        // Search this char from 1-char words
        int idx = WSVL->Search(start, end, c);
        if (idx >= 0) {
          int sn = WSVL->SN(idx);
          // WideString WS = WSVL->Strings(idx);
          // WS = WS;
          if (sn >= 0) {
            // SM->Add(sn, id, 1.0f);
            SM->Add(sn, id, 1.0f / WSVL->Values(idx));
          }
          start = WSVL->From(idx);
          end = WSVL->To(idx);
          len++;
        } else {
          break;
        }
      }
    }
  }

  // Remove duplicates
  unsigned int t2 = GetTickCount();
  SM->Finalize(true);
  RE_Edit->Lines->Add(
      UnicodeString("SM : ") + IntToStr((int)(GetTickCount() - t)) +
      "ms  (Ex : " + IntToStr((int)(t2 - t)) +
      "ms  Fn : " + IntToStr((int)(GetTickCount() - t2)) + "ms)");
  RE_Edit->Lines->Add(UnicodeString("SM Items : ") + SM->m_Items->Count);
  RE_Edit->Lines->Add("");

  // Simple similarity matrix generation
  t = GetTickCount();
  FreeSimMatrix();
  SM->DeleteSameCol(true);
  // SM->MergeCol(500, 0.9f, 30);//Merge words with similarity>=0.9 every 500
  // words; repeat until <=50 words
  SM->PrepareCosRow();
  m_SimMatrix = new TDMatrix(doccount);
  for (int i = 0; i < doccount; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    m_SimMatrix->ID(i) = Card->m_nID;
    for (int i2 = i + 1; i2 < doccount; i2++) {
      float cos = SM->CosRow(i, i2);
      m_SimMatrix->S(i, i2) = m_SimMatrix->S(i2, i) = cos;
    }
  }
  t2 = GetTickCount();
  m_SimMatrix->Finalize();
  RE_Edit->Lines->Add(UnicodeString("SM Items (after DeleteSameCol): ") +
                      SM->m_Items->Count);
  RE_Edit->Lines->Add(
      UnicodeString("DM : ") + IntToStr((int)(GetTickCount() - t)) +
      "ms  (Cos : " + IntToStr((int)(t2 - t)) +
      "ms  Fn : " + IntToStr((int)(GetTickCount() - t2)) + "ms)");

  delete SM;

  delete TD;
  delete WS;
}
// ---------------------------------------------------------------------------


// --------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <math.h>
#include <stdio.h>
#include <mmsystem.h>
#include <printers.hpp>
#include <clipbrd.hpp>
#include <Vcl.filectrl.hpp>
#include <HttpApp.hpp>
#include <DateUtils.hpp>
#include <IdURI.hpp>

#include "fomain.h"
// ---------------------------------------------------------------------------

#define OVERVIEWSIZE 0.75f

#include "foselectcard.h"
#include "utils.h"
#include "setting.h"
#include "fosearch.h"
#include "foabout.h"
#include "foedittext.h"
#include "folabeledit.h"
#include "foview.h"
#include "forandomanimation.h"
#include "fofullscreen.h"
// #include "inet.h"
#include "foinformation.h"
#include "foshape.h"
#include "textproc.h"
#include "foprogress.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
// #pragma link "HTTSLib_OCX"
// #pragma link "AgentObjects_OCX"
// #pragma link "ACTIVEVOICEPROJECTLib_OCX"
#pragma resource "*.dfm"

TFo_Main *Fo_Main;

#define float_ double

// ---------------------------------------------------------------------------
TCardImage::TCardImage(UnicodeString FN) : m_FN(FN), m_bExist(false),
	m_bUsed(true), m_Jpg(NULL), m_BMP(NULL), m_Image(NULL) {
	if (FileExists(m_FN)) {
		if (ExtractFileExt(m_FN) == ".bmp") {
			// BMP
			m_BMP = new Graphics::TBitmap();
			try {
				m_BMP->LoadFromFile(m_FN);
				m_Image = m_BMP;
				m_bExist = true;
			}
			catch (...) {
			}
		}
		else {
			// Jpeg
			m_Jpg = new TJPEGImage();
			try {
				m_Jpg->LoadFromFile(m_FN);
				m_Image = m_Jpg;
				m_bExist = true;
			}
			catch (...) {
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
		delete m_Jpg;
	}
}

// ---------------------------------------------------------------------------
TCardVideo::TCardVideo(UnicodeString FN) : m_FN(FN), m_bExist(false),
	m_bUsed(true), m_Video(NULL) {
	/*
	 m_Video = new TDSDecoder(true);
	 WideString WSFN = m_FN;
	 if (WSFN.Pos("|")){
	 WideString PlayPos = SplitStrBy(WSFN, "|");
	 m_Video->m_fLoopStartPos = StrToIntDef(SplitStrBy(PlayPos, "-"), 0);
	 m_Video->m_fLoopEndPos = StrToIntDef(PlayPos, -1);
	 }
	 m_Video->m_FN = WSFN;
	 m_Video->m_bGrub = true;
	 m_Video->m_bLoop = true;
	 m_Video->Resume();
	 m_Video->WaitPrepared();
	 m_bExist = m_Video->GetLength() > 1;
	 */
}

// ---------------------------------------------------------------------------
TCardVideo::~TCardVideo() {
	if (m_Video) {
		/*
		 m_Video->FreeOnTerminate = true;
		 m_Video->Terminate();
		 */
	}
}

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

	// カードが選択されている？Open：不明
	// カードが選択されていたらOpenかInsertか不明
	// 複数ファイルかつfipが混じっていたらInsert
	// fip、txt以外が混ざっていたらInsert
	// 基本開くで、Ctrlキーが押されているか、上記条件によりInsertと判断されたら挿入？

	// →とりあえず、2個以上だったりfipじゃなかったら全て外部リンクとする

	if (fcount > 0) {
		wchar_t FN[MAX_PATH];
		DragQueryFile((HDROP)mes.Drop, 0, FN, MAX_PATH);

		if (fcount > 1 || LowerCase(ExtractFileExt(FN)) != ".fip") {
			// 外部リンク挿入
			if (m_nTargetCard >= 0 && m_nTargetCard == m_nCurrentCard) {
				// 現在カードに挿入
				for (int i = 0; i < fcount; i++) {
					DragQueryFile((HDROP)mes.Drop, i, FN, MAX_PATH);
					RE_Edit->Lines->Add(FN);
				}
			}
			else {
				// カードを作成して挿入
				m_Document->ClearCardSelection();
				for (int i = 0; i < fcount; i++) {
					DragQueryFile((HDROP)mes.Drop, i, FN, MAX_PATH);
					TCard *Card =
						m_Document->NewCard(m_Document->m_Cards->Count);
					Card->m_bSelected = true;
					m_Document->SetCardText(Card, FN);
					Card->m_Title = ExtractFileName(FN);
					Card->m_Title =
						Card->m_Title.SubString(1,
						Card->m_Title.Length() - ExtractFileExt(Card->m_Title)
						.Length());
					m_nTargetCard = Card->m_nID;
				}
			}

			if (m_nTargetCard >= 0 && m_nTargetCard == m_nCurrentCard) {
			}
		}
		else {
			// 開く
			if (SaveCheck()) {
				LoadSub(FN);
			}
		}
	}
	DragFinish((HDROP)mes.Drop);
} // ---------------------------------------------------------------------------

__fastcall TFo_Main::TFo_Main(TComponent* Owner) : TForm(Owner) {
}

// ---------------------------------------------------------------------------
void TFo_Main::BackupSub(UnicodeString Action) {
	if (!m_bDoNotBackup) {
		m_UndoRedo->Backup(m_Document, DeleteActionKey(Action).c_str());
		/*
		 if (m_DocBeforeAnimation){
		 //アニメーション中に編集があったら、アニメーション前に戻さないようにする
		 //現状UndoRedo以外、アニメーションから抜けるものばかりだから必要なし

		 delete m_DocBeforeAnimation;
		 m_DocBeforeAnimation = NULL;
		 }
		 */
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshRecent(UnicodeString FN) {
	// 履歴更新
	if (FN != "" && FN != (ExtractFilePath(ParamStr(0)) +
		UnicodeString("help.fip"))) {
		// ファイル名
		for (int i = 0; i < 10; i++) {
			if (SettingFile.m_RecentFiles[i] == FN) {
				// 既にリストにある場合
				if (i > 0) {
					for (int i2 = i; i2 >= 1; i2--) {
						SettingFile.m_RecentFiles[i2] =
							SettingFile.m_RecentFiles[i2 - 1];
					}
				}
				SettingFile.m_RecentFiles[0] = FN;
				break;
			}
		}

		if (SettingFile.m_RecentFiles[0] != FN) {
			// 重複無し
			for (int i = 9; i >= 1; i--) {
				SettingFile.m_RecentFiles[i] = SettingFile.m_RecentFiles[i - 1];
			}
			SettingFile.m_RecentFiles[0] = FN;
		}

		FN = ExtractFilePath(FN);

		// フォルダ名
		for (int i = 0; i < 10; i++) {
			if (SettingFile.m_RecentFolders[i] == FN) {
				// 既にリストにある場合
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
			// 重複無し
			for (int i = 9; i >= 1; i--) {
				SettingFile.m_RecentFolders[i] =
					SettingFile.m_RecentFolders[i - 1];
			}
			SettingFile.m_RecentFolders[0] = FN;
		}

	}

	// メニュー更新
	for (int i = 0; i < 10; i++) {
		if (SettingFile.m_RecentFiles[i] != "") {
			if (i < 9) {
				MI_RecentFiles[i]->Caption =
					UnicodeString("&") + IntToStr(i + 1) + " " +
					SettingFile.m_RecentFiles[i];
			}
			else {
				MI_RecentFiles[i]->Caption =
					UnicodeString("1&0 ") + SettingFile.m_RecentFiles[i];
			}
			MI_RecentFiles[i]->Visible = true;
		}
		else {
			MI_RecentFiles[i]->Visible = false;
		}

		if (SettingFile.m_RecentFolders[i] != "") {
			if (i < 9) {
				MI_RecentFolders[i]->Caption =
					UnicodeString("&") + IntToStr(i + 1) + " " +
					SettingFile.m_RecentFolders[i] + "...";
			}
			else {
				MI_RecentFolders[i]->Caption =
					UnicodeString("1&0 ") + SettingFile.m_RecentFolders[i]
					+ "...";
			}
			MI_RecentFolders[i]->Visible = true;
		}
		else {
			MI_RecentFolders[i]->Visible = false;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormCreate(TObject *Sender) {
	randomize();

	/*
	 try{
	 m_Agent = new TAgent(this);
	 m_Agent->Parent = Fo_Main;

	 }
	 catch(...){
	 m_Agent = NULL;
	 }
	 try{
	 m_TTS = new TDirectSS(this);
	 m_TTS->Visible = false;
	 m_TTS->Parent = Fo_Main;

	 }
	 catch(...){
	 m_Agent = NULL;
	 }
	 m_AgentChar = NULL;
	 */

	// 初期設定を高速読み込み
	Ini = new TIniFile(ExtractFilePath(ParamStr(0)) + "setting.ini");
	SettingFile.ReadFromIni(Ini, "File");
	SettingView.ReadFromIni(Ini, "View");

	TIniFile *FIni = new TIniFile(ExtractFilePath(ParamStr(0)) +
		"setting2.ini");
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

	// Recentメニュー
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

	// Web検索メニュー追加
	MI_WebSearch = new TList();
	int inspos = 9;
	/*
	 for (int i = 0 ; i < PM_Editor->Items->Count ; i++){
	 if (&PM_Editor->Items[i] == &PE_Sep1){
	 inspos = i + 1;
	 break;
	 }
	 }
	 */
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
		MI->Caption = Setting2Function.m_WebSearch->Names
			[i % Setting2Function.m_WebSearch->Count];
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
			}
			else {
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

	// タイトル入力
	Ed_TitleB = new TEdit2(Pa_Browser);
	Ed_TitleB->Visible = false;
	Ed_TitleB->WordWrap = false;
	Ed_TitleB->OnChange = Ed_TitleChange;
	Ed_TitleB->OnEnter = Ed_TitleBEnter;
	Ed_TitleB->OnExit = Ed_TitleBExit;
	Ed_TitleB->OnKeyDown = Ed_TitleBKeyDown;
	Ed_TitleB->OnDblClick = PB_BrowserDblClick;
	Ed_TitleB->Parent = Pa_Browser;

	// ラベルボタン
	for (int i = 0; i < MAXLABELS; i++) {
		Bu_Label[i] = new TButton(Bu_Label0->Owner);
		Bu_Label[i]->Caption = "";
		Bu_Label[i]->SetBounds(590 + 112 * i,
			// Bu_Label0->Left + (Bu_Label0->Width + 8) * i,
			8, // Bu_Label0->Top,
			104, // Bu_Label0->Width,
			26 // Bu_Label0->Height
			);
		Bu_Label[i]->Parent = Bu_Label0->Parent;
		Bu_Label[i]->OnClick = Bu_Label0Click;
		Bu_Label[i]->Tag = i;
		Bu_Label[i]->Visible = false;

		Bu_LinkLabel[i] = new TButton(Bu_LinkLabel0->Owner);
		Bu_LinkLabel[i]->Caption = "";
		Bu_LinkLabel[i]->SetBounds(590 + 112 * i,
			// Bu_LinkLabel0->Left + (Bu_LinkLabel0->Width + 8) * i,
			8, // Bu_LinkLabel0->Top,
			104, // Bu_LinkLabel0->Width,
			26 // Bu_LinkLabel0->Height
			);
		Bu_LinkLabel[i]->Parent = Bu_LinkLabel0->Parent;
		Bu_LinkLabel[i]->OnClick = Bu_LinkLabel0Click;
		Bu_LinkLabel[i]->Tag = i;
		Bu_LinkLabel[i]->Visible = false;
	}

	// ラベル選択メニュー
	MI_Labels = new TList();
	MI_LinkLabels = new TList();

	// 外部リンクメニュー
	MI_ExtLink = NULL;

	// ブラウザ座標データ
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

	// アニメーション
	m_DocBeforeAnimation = NULL;
	m_nAnimation = 0;
	m_nAnimationCount = 0;

	// その他
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
	m_nLastFPSTime = timeGetTime();
	m_DemoStrings = new TStringList();

	UD_Size->Position = SettingView.m_Font->Size;
	Ed_Size->Text = SettingView.m_Font->Size;
	SetFont();

	m_nLastTarget = -1;
	m_nFocusCursorPos = 0;
	m_bRedrawRequested = false;

	RefreshWallPaper();
	RefreshFileList();

	/*
	 m_DemoStrings->Add("クリックしてカードを選択。");
	 m_DemoStrings->Add("もう一度クリックしてカード名を入力します。");
	 m_DemoStrings->Add("ダブルクリックもしくはCtrl+Mで新しいカードを作成。");
	 m_DemoStrings->Add("次々にカードを入力していきます。");
	 m_DemoStrings->Add("カードの位置はドラッグで変更できます。");
	 m_DemoStrings->Add("Deleteキーでカードを削除。");

	 m_DemoStrings->Add("リンクボタンを押して、リンク元からリンク先までドラッグ。");
	 m_DemoStrings->Add("カードの間にリンクを張ることができます。");
	 m_DemoStrings->Add("リンク元を選択し…");
	 m_DemoStrings->Add("Shiftキーを押しながらリンク先をクリックしてもOK。");

	 m_DemoStrings->Add("カードを選択し、New Labelでカードに新しいラベルをつけます。");
	 m_DemoStrings->Add("他のカードも選択し、Labelボタンで同じラベルをつけます。");
	 m_DemoStrings->Add("また新しいラベルを作成します。");
	 m_DemoStrings->Add("ラベリングボタンを使うと、ワンクリックでラベル付けができます。");
	 // */

	/*
	 m_DemoStrings->Add("Arrangeをクリックしてカードを一発整頓！");
	 m_DemoStrings->Add("カードの並び順に従って整理したり");
	 m_DemoStrings->Add("リンクにしたがって整理したり");
	 m_DemoStrings->Add("同じラベルのカードを集めたりすることができます。");

	 m_DemoStrings->Add("VIEWボタンで表示するカード、リンクを選択できます。");
	 m_DemoStrings->Add("同じラベルのカードをまとめて表示したり");
	 m_DemoStrings->Add("ラベルごとに表示/非表示を指定することができます。");

	 m_DemoStrings->Add("Link Limitationで、リンクの近いカードのみ表示。");
	 m_DemoStrings->Add("関連の強いカードのみ見ながら作業をすることができます。");
	 // */

	/*
	 m_DemoStrings->Add("Auto Scroll、Auto Zoomで、カードを快適ブラウズ。");
	 m_DemoStrings->Add("Frieve Editorはいくつかのアニメーション機能を備えています。");
	 m_DemoStrings->Add("Random Cardでは、ランダムにカードを表示しながら");
	 m_DemoStrings->Add("アイデアを練ることができます。");
	 m_DemoStrings->Add("Random Card 2では、カードをランダムにスクロールします。");
	 m_DemoStrings->Add("Random Traceでは、ランダムにリンクを辿って表示します。");
	 // */

	/*
	 m_DemoStrings->Add("Insertキーで子ノードを作成可能になりました。");
	 m_DemoStrings->Add("Enterキーで兄弟ノードを作成可能になりました。");
	 m_DemoStrings->Add("リンク選択中にInsertキーで、");
	 m_DemoStrings->Add("リンクの間にカードを作成可能になりました。");
	 m_DemoStrings->Add("ドラッグによるリンクの編集が可能になりました。");
	 // */
	/*
	 m_DemoStrings->Add("カードタイトル、本文に限定した検索が可能になりました。");
	 m_DemoStrings->Add("ブラウザ画面からカードタイトルの検索が可能になりました。");
	 m_DemoStrings->Add("Shift+ドラッグで");
	 m_DemoStrings->Add("同じラベルのカードをまとめて移動可能になりました。");
	 // */

	/*
	 m_DemoStrings->Add("Click to select a card.");
	 m_DemoStrings->Add("Click again to input name of the card.");
	 m_DemoStrings->Add("Double-click or Ctrl+M to create a new card.");
	 m_DemoStrings->Add("Drag to move a card.");
	 m_DemoStrings->Add("Push delete key to delete a selected card.");

	 m_DemoStrings->Add("Push a link button and drag from source to destination to add a link.");
	 m_DemoStrings->Add("Or, select a source and click a destination with Shift key.");

	 m_DemoStrings->Add("Select card and select \"New Label\" to add new label to the card.");
	 m_DemoStrings->Add("Select other card and add same label with Label button.");
	 m_DemoStrings->Add("Use labeling button to add label with single click.");
	 // */

	/*
	 m_DemoStrings->Add("Push \"Arrange\" button to arrange cards automatically.");
	 m_DemoStrings->Add("Choose \"Index\" to arrange cards with index of cards.");
	 m_DemoStrings->Add("Choose \"Link\" to arrange cards with links between cards.");
	 m_DemoStrings->Add("Choose \"Label\" to collect cards which has same label.");

	 m_DemoStrings->Add("Use \"VIEW\" button to limit cards to display.");
	 m_DemoStrings->Add("Check \"Fold\" to bind up cards with same label.");
	 m_DemoStrings->Add("Check \"Show\" or \"Hide\" to show or hide cards with label.");

	 m_DemoStrings->Add("Check \"Link Limitation\" to limit cards to display by number of link.");
	 // */

	/*
	 m_DemoStrings->Add("\"Auto Scroll\" and \"Auto Zoom\" will be useful to browse cards.");
	 m_DemoStrings->Add("Frieve Editor has some animation function.");
	 m_DemoStrings->Add("Select \"Random Flush\" to flush random cards.");
	 m_DemoStrings->Add("Select \"Random Scroll\" to scroll random cards.");
	 m_DemoStrings->Add("Select \"Random Trace\" to trace links by randomly.");
	 // */

	m_DemoStrings->Add("");

	m_nDemoIndex = 0;

	m_bMDownBrowser = 0;

	m_nLastTimeOut = timeGetTime();

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
	// TB_Zoom->DoubleBuffered = true;
	// Ed_FindCard->DoubleBuffered = true;
	/*
	 Pa_Files->DoubleBuffered = true;
	 Pa_List->DoubleBuffered = true;
	 TS_Editor->DoubleBuffered = true;
	 */
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormDestroy(TObject *Sender) {
	delete m_DemoStrings;

	if (m_DocBeforeAnimation) {
		delete m_DocBeforeAnimation;
	}

	FreeSimMatrix();

	for (int i = 0; i < m_VideoList->Count; i++) {
		delete(TCardVideo*)m_VideoList->Items[i];
	}
	delete m_VideoList;

	for (int i = 0; i < m_ImageList->Count; i++) {
		delete(TCardImage*)m_ImageList->Items[i];
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

	FreeMIExtLink();

	if (m_CardX != NULL) {
		delete[]m_CardVisible;
		delete[]m_LinkVisible;
		delete[]m_CardTitle;
		delete[]m_CardRelated;
		delete[]m_CardAssign;
		delete[]m_CardShape;
		delete[]m_CardX;
		delete[]m_CardY;
		delete[]m_CardWidth;
		delete[]m_CardHeight;
	}

	/*
	 //Fo_Mainといっしょに破棄される
	 if (TTS){
	 delete TTS;
	 }
	 */
}

// ---------------------------------------------------------------------------
void TFo_Main::FreeMILabels() {
	for (int i = 0; i < MI_Labels->Count; i++) {
		delete(TMenuItem*)(MI_Labels->Items[i]);
	}
	MI_Labels->Clear();

	for (int i = 0; i < MI_LinkLabels->Count; i++) {
		delete(TMenuItem*)(MI_LinkLabels->Items[i]);
	}
	MI_LinkLabels->Clear();
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshLabel() {
	// ラベル更新

	if (m_nTargetCard < 0) {
		for (int i = 0; i < MAXLABELS; i++) {
			Bu_Label[i]->Visible = false;
		}
	}
	else {
		TCard *Card = m_Document->GetCard(m_nTargetCard);
		if (Card)
			for (int i = 0; i < MAXLABELS; i++) {
				if (i < Card->m_Labels->Count) {
					// ラベルあり
					Bu_Label[i]->Caption =
						m_Document->GetLabelByIndex(0,
						Card->m_Labels->GetLabel(i) - 1)->m_Name;
					Bu_Label[i]->Visible = true;
				}
				else if (i == Card->m_Labels->Count) {
					Bu_Label[i]->Caption = "";
					Bu_Label[i]->Visible = true;
				}
				else {
					Bu_Label[i]->Visible = false;
				}
			}
	}

	if (m_nTargetLink < 0) {
		for (int i = 0; i < MAXLABELS; i++) {
			Bu_LinkLabel[i]->Visible = false;
		}
	}
	else {
		TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
		if (Link)
			for (int i = 0; i < MAXLABELS; i++) {
				if (i < Link->m_Labels->Count) {
					// ラベルあり
					Bu_LinkLabel[i]->Caption =
						m_Document->GetLabelByIndex(1,
						Link->m_Labels->GetLabel(i) - 1)->m_Name;
					Bu_LinkLabel[i]->Visible = true;
				}
				else if (i == Link->m_Labels->Count) {
					Bu_LinkLabel[i]->Caption = "";
					Bu_LinkLabel[i]->Visible = true;
				}
				else {
					Bu_LinkLabel[i]->Visible = false;
				}
			}
	}
}

// ---------------------------------------------------------------------------
bool TFo_Main::TreeMode() {
	return Bu_ArrangeType->Tag >= 1000 && Bu_ArrangeType->Tag <
		1999 && SB_Arrange->Down;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::Ti_CheckTimer(TObject *Sender) {
	unsigned int tgt = timeGetTime();

	// Continuous Load
	if (m_bContinuousLoad) {
		int newage = FileAge(m_Document->m_FN);
		if (newage != m_nCLFileAge) {
			LoadSub(m_Document->m_FN, true);
		}
	}

	// プラグインへのタイムアウト
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

	// 操作リクエスト
	if (fReqZoom >= -999.0f) {
		TB_Zoom->Position = (int)(fReqZoom * 2000);
		fReqZoom = -1000.0f;
	}
	if (fReqX >= -999.0f) {
		if (PC_Client->ActivePage != TS_Browser) {
			Sc_X->Position = (int)(fReqX * 10000);
		}
		else {
			m_fBrowserScrollRatio = 0.0f;
			m_nScrollTargetX = fReqX * 10000;
		}
		fReqX = -1000.0f;
	}
	if (fReqY >= -999.0f) {
		if (PC_Client->ActivePage != TS_Browser) {
			Sc_Y->Position = (int)(fReqY * 10000);
		}
		else {
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

	// タスクバーの表示
	{
		UnicodeString S;
		if (m_Document->m_FN != "") {
			S = ExtractFileNameOnly(m_Document->m_FN);
		}
		else {
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
		}
		else {
			S = UnicodeString(AppTitle) + " " + m_Document->m_FN;
			if (m_Document->m_bChanged) {
				S += " *";
			}
		}
		if (Caption != S) {
			Caption = S;
		}
	}

	// ファイルリスト
	{
		TPoint pos, listpos;
		GetCursorPos(&pos);
		listpos.x = Pa_Files->Left;
		listpos.y = Pa_Files->Top;
		listpos = ClientToScreen(listpos);
		if (pos.x >= Left && pos.x < Left + Width && pos.y >= Top && pos.y <
			Top + Height) {
			/*
			 M_File->Caption =
			 IntToStr((pos.x < Left + 16 && pos.y >= listpos.y && pos.y < listpos.y + Pa_List->Height)) +
			 IntToStr(!Pa_Files->Visible) +
			 IntToStr(m_nAnimation == 0) +
			 IntToStr(Application->Active);
			 */
			if ((pos.x < Left + 16 && pos.y >= listpos.y && pos.y <
				listpos.y + Pa_List->Height)
				&& !Pa_Files->Visible && m_nAnimation == 0 &&
				Application->Active && !m_bMDownBrowser &&
				SettingView.m_bFileList) {
				CloseEditBox();
				// ちらつきを防止しながらファイルリストを表示
				/*
				 Pa_Files->Visible = true;
				 Sp_Left->Left = Pa_Files->Width;
				 Sp_Left->Visible = true;
				 */
				// *
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
				Application->ProcessMessages(); // 一旦各コンポーネントのサイズを変化させる
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
				Application->ProcessMessages(); // 一旦各コンポーネントのサイズを変化させる
				Ti_Check->Enabled = true;

				PB_Browser->Tag = 0;
				// */
			}
			else if ((pos.x >= Left + Pa_List->Left + 32 || pos.y <
				listpos.y || pos.y > listpos.y + Pa_Files->Height ||
				!Application->Active) && Pa_Files->Visible) {
				// ちらつきを防止しながらファイルリストを非表示
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
				/*
				 if (PC_Client->ActivePage == TS_Editor){
				 Pa_Editor->Visible = false;
				 RE_Edit->Visible = false;
				 }
				 */
				Pa_List->Visible = true;
				Sp_Left2->Visible = true;
				Pa_Client->Visible = true;
				/*
				 if (PC_Client->ActivePage == TS_Editor){
				 Pa_Editor->Visible = true;
				 RE_Edit->Visible = true;
				 }
				 */

				Ti_Check->Enabled = false;
				Application->ProcessMessages(); // 一旦各コンポーネントのサイズを変化させる
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

	// カードリスト
	if (Pa_List->Visible != SettingView.m_bCardList) {
		Pa_Client->Align = alNone;
		Pa_List->Visible = SettingView.m_bCardList;
		Sp_Left2->Left = Pa_List->Left + Pa_List->Width;
		Sp_Left2->Visible = SettingView.m_bCardList;
		Pa_Client->Align = alClient;
	}

	// 関連テキスト
	bool RelatedTextRefreshRequest = false; // 関連テキスト更新要求
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
			// 表示にした場合は更新要求
		}
	}

	bool drawrequest = false; // ブラウザ描画要求（データ更新用）
	bool drawrequest2 = m_bRedrawRequested; // ブラウザ描画要求（アニメーション表示用）
	bool drawrequest_s = m_bRedrawRequested; // 統計描画要求
	m_bRedrawRequested = false;

	// アニメーション
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

	// アニメーション中のフルスクリーン
	if (Fo_RandomAnimation->m_bFullScreenRequest) {
		Fo_RandomAnimation->m_bFullScreenRequest = false;
		MV_FullScreenClick(Sender);
	}

	// アニメーション中のZoom
	if (Fo_RandomAnimation->Visible) {
		if (TB_Zoom->Position != Fo_RandomAnimation->TB_Zoom->Position) {
			if (Fo_RandomAnimation->m_bZoomChanged) {
				// ユーザがZoom操作を行った
				PB_Browser->Tag = 1; // 描画禁止
				TB_Zoom->Position = Fo_RandomAnimation->TB_Zoom->Position;
				PB_Browser->Tag = 0; // 描画再開
				drawrequest2 = true;
				SB_AutoZoom->Down = false;
			}
			else {
				// Auto Zoomによるズーム
				Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
			}
			Fo_RandomAnimation->m_bZoomChanged = false;
		}
	}

	// カードリスト

	// 現在のカードを選択していなかったか、選択カードがあるのにTargetCardがないときの処理
	bool SelectionChanged = false;
	bool SelectedExist = true;
	if (LB_List->Count == m_Document->m_Cards->Count) {
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			TCard* Card = m_Document->GetCardByIndex(i);
			if (!SettingView.m_bFocusCursorAnimation) {
				if (Card->m_bSelected) {
					Card->m_nSelected = 100;
				}
				else {
					Card->m_nSelected = 0;
				}
			}
			SelectionChanged |= Card->m_bSelected != LB_List->Selected[i];
			SelectedExist |= Card->m_bSelected;
		}
	} {
		bool b;
		TCard *Card = m_Document->GetCard(m_nTargetCard);

		if (Card) {
			b = !Card->m_bSelected;
		}
		else {
			b = SelectedExist;
		}
		if (b) {
			// カードが選択されていない
			// 選択されているカードで、一番最近触ったカードに移動
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

	SelectionChanged |= m_nTargetCard >= 0 && m_Document->SearchCardIndex
		(m_nTargetCard) != LB_List->ItemIndex;
	SelectionChanged |= m_nCurrentCard != m_nTargetCard;
	bool ListChanged = LB_List->Count != m_Document->m_Cards->Count ||
		m_nRefreshListCount != m_Document->m_nRefreshListCount;
	if (SelectionChanged || ListChanged || Fo_View->m_bRefreshRequest ||
		m_UndoRedo->m_bChanged) {
		m_UndoRedo->m_bChanged = false;
		RelatedTextRefreshRequest = true; // 関連テキスト更新要求

		// 何らかのリストに関する編集操作があった
		m_nRefreshListCount = m_Document->m_nRefreshListCount;
		m_fZoomSD = 0.21f;
		drawrequest = true;

		if (TreeMode()) {
			SetCardVisible(false);
			m_Document->RefreshCardLevel();
			SetCardVisible(true);
		}

		// カードリストの表示更新
		int lasttopindex = LB_List->TopIndex;
		LB_List->Items->BeginUpdate();
		if (ListChanged) {
			LB_List->Items->Clear();
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				TCard *Card = m_Document->GetCardByIndex(i);
				UnicodeString S;
				if (Card->m_bTop) {
					S = ">";
				}
				else {
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
		}
		else {
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

		// カード本文などの変更反映（Undoのとき必要）
		if (m_nCurrentCard >= 0 && m_nCurrentCard == m_nTargetCard) {
			TCard *Card = m_Document->GetCard(m_nCurrentCard);
			if (Card) {
				UnicodeString Text =
					Card->m_Lines->Text.SubString(1,
					Card->m_Lines->Text.Length() - 2);
				if (Text != RE_Edit->Text && Card->m_Lines->Text !=
					RE_Edit->Text) {
					RE_Edit->Tag = 1;
					RE_Edit->Lines->BeginUpdate();
					int line = RE_Edit->Perform(EM_GETFIRSTVISIBLELINE, 0,
						(NativeInt)0);
					RE_Edit->Text = Text;
					if (m_nNextCardID == m_nCurrentCard) {
						int line2 = RE_Edit->Perform(EM_GETFIRSTVISIBLELINE, 0,
							(NativeInt)0);
						if (line2 > line) {
							while (line2-- > line) {
								RE_Edit->Perform(EM_SCROLL, SB_LINEUP,
									(NativeInt)0);
							}
						}
						else {
							while (line2++ < line) {
								RE_Edit->Perform(EM_SCROLL, SB_LINEDOWN,
									(NativeInt)0);
							}
						}
						RE_Edit->Lines->EndUpdate();

						RE_Edit->SelStart = m_nNextSelStart;
						RE_Edit->SelLength = 0;
						RE_Edit->SelLength = m_nNextSelLength;
					}
					else {
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

		// 選択中のIndexを元に戻す
		LB_List->TopIndex = lasttopindex;
		if (LB_List->ItemIndex >= 0) {
			while (LB_List->ItemRect(LB_List->ItemIndex).top < 0) {
				LB_List->TopIndex = LB_List->TopIndex - 1;
			}
			while (LB_List->ItemRect(LB_List->ItemIndex).bottom >
				LB_List->Height) {
				LB_List->TopIndex = LB_List->TopIndex + 1;
			}
		}

		LB_List->Items->EndUpdate();

	}

	// 存在しないカードが選択されていたときの処理（Undo時などに発生）
	if (m_nCurrentCard == m_nTargetCard) {
		TCard *Card = m_Document->GetCard(m_nCurrentCard);
		if (Card == NULL) {
			RelatedTextRefreshRequest = true; // 関連テキスト更新要求
			m_nTargetCard = -1;
		}
	}

	// 文章
	TCard *Card = m_Document->GetCard(m_nTargetCard);
	if (m_nCurrentCard != m_nTargetCard) {
		RelatedTextRefreshRequest = true; // 関連テキスト更新要求
		TCard *LastCard = m_Document->GetCard(m_nCurrentCard);
		m_nLastTarget = -1;
		if (LastCard && Card) {
			if (LastCard->m_nSelected >
				0 && !LastCard->m_bSelected && Card->m_nSelected < 100) {
				m_nLastTarget = m_nCurrentCard;
			}
		}
		if (SettingView.m_bFocusCursorAnimation) {
			if (Card) {
				if (Card->m_nSelected < 100) {
					m_nFocusCursorPos = 0;
				}
			}
			else {
				m_nFocusCursorPos = 0;
			}
		}
		else {
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

			// 絵をクリア
			delete m_Drawing;
			m_Drawing = new TDrawing();

			PB_Drawing->Enabled = false;
			RefreshLabel();
		}
		else {
			Pa_Editor->Visible = true;
			RE_Edit->Enabled = true;
			Ed_Title->Enabled = true;
			Ed_CardSize->Enabled = true;
			UD_CardSize->Enabled = true;
			if (Card) {
				// 本文
				RE_Edit->Tag = 1;
				// RE_Edit->Lines->Assign(Card->m_Lines);
				// RE_Edit->Text = Card->m_Lines->Text.SubString(1, Card->m_Lines->Text.Length() - 2);
				RE_Edit->Text = Card->m_Lines->Text;
				Card->m_fViewed = Now();
				RE_Edit->SelStart = 0;
				RE_Edit->Tag = 0;
				// 本文以外
				// タイトル
				if (Ed_Title->Text != Card->m_Title) {
					Ed_Title->Tag = 1;
					Ed_Title->Text = Card->m_Title;

					Ed_TitleB->Text = DecodeES(Card->m_Title);
					if (CountStr(Card->m_Title, "\\n")) {
						Ed_TitleB->WantReturns = true;
					}
					else {
						Ed_TitleB->WantReturns = false;
					}

					/*
					 if (SettingView.m_bRead) if (m_AgentChar && m_TTS){
					 m_AgentChar->Stop();
					 m_AgentChar->Speak(Variant(WideString(UnicodeString("\\Pit=200\\\\Spd=200\\") + Card->m_Title.c_str())));
					 //m_AgentChar->Speak(Variant(WideString(UnicodeString("\\Spd=200\\") + Card->m_Title.c_str())));
					 }
					 */
					Ed_Title->Tag = 0;
				}
				// ラベル
				RefreshLabel();

				// 絵
				delete m_Drawing;
				m_Drawing = new TDrawing(*Card->m_Drawing);

				// Date
				La_Created->Caption = DateTimeToStr(Card->m_fCreated);
				La_Edited->Caption = DateTimeToStr(Card->m_fUpdated);
				La_Viewed->Caption = DateTimeToStr(Card->m_fViewed);
				m_nRefreshLinkCount = m_Document->m_nRefreshLinkCount;
				RefreshLinks();

				// ビデオを先頭に戻す
				if (Card->m_VideoFN != "") {
					TCardVideo *Video = SearchVideo(Card->m_VideoFN);
					if (Video) {
						// Video->m_Video->SetPosition(Video->m_Video->m_fLoopStartPos);
					}
				}
			}
			else {
				delete m_Drawing;
				m_Drawing = new TDrawing();
			}

			m_Drawing->m_nTool = nDrawingTool;

			// 絵
			PB_Drawing->Enabled = true;
		}
		RefreshLaStatus();
	}

	// 関連テキスト更新
	if (RelatedTextRefreshRequest && Pa_EditorRelated->Visible) {
		TStringList *NewStr = new TStringList();
		if (m_nTargetCard >= 0) {
			TCard *Card = m_Document->GetCard(m_nTargetCard);
			TList *Str = new TList(); // 文字列リスト
			TList *Order = new TList(); // 表示オーダーリスト
			// リンクループ
			for (int il = 0; il < m_Document->m_Links->Count; il++) {
				TLink *Link = m_Document->GetLinkByIndex(il);
				TCard *Card2 = NULL;
				if (Link->m_nFromID == Card->m_nID && Link->m_nDestID !=
					Card->m_nID) {
					Card2 = m_Document->GetCard(Link->m_nDestID);
				}
				else if (Link->m_nDestID == Card->m_nID && Link->m_nFromID !=
					Card->m_nID) {
					Card2 = m_Document->GetCard(Link->m_nFromID);
				}

				if (Card2)
					if (Card2->m_Lines->Count > 0) {
						if (Card2->m_bVisible) {
							Str->Add
								(new UnicodeString(UnicodeString("Title : ") +
								DecodeES(Card2->m_Title,
								" ") + UnicodeString("\r\n\r\n") +
								Card2->m_Lines->Text + UnicodeString
								("\r\n\r\n")));
						}
						else {
							Str->Add
								(new UnicodeString(UnicodeString("(Title : ") +
								DecodeES(Card2->m_Title,
								" ") + UnicodeString(")\r\n\r\n") +
								Card2->m_Lines->Text + UnicodeString
								("\r\n\r\n")));
						}
						int order = Card2->m_nViewedOrder;
						if (!Card2->m_bVisible) {
							order += m_Document->m_Cards->Count;
							// 非表示カードのテキストは後ろへ
						}
						Order->Add((void*)order);
					}
			}

			while (Str->Count) {
				int minorder = (int)Order->Items[0];
				int minindex = 0;
				for (int io = 1; io < Order->Count; io++) {
					int o = (int)Order->Items[io];
					if (minorder > o) {
						minorder = o;
						minindex = io;
					}
				}

				NewStr->Add(*(UnicodeString*)Str->Items[minindex]);
				delete(UnicodeString*)Str->Items[minindex];
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

	// 検索ウインドウにフォーカスが移っても選択範囲が見えるようにする
	if (RE_Edit->HideSelection != !Fo_Search->Visible) { {
			int selstartbak = RE_Edit->SelStart;
			int sellengthbak = RE_Edit->SelLength;
			RE_Edit->HideSelection = !Fo_Search->Visible;
			RE_Edit->SelStart = selstartbak;
			RE_Edit->SelLength = sellengthbak;
		} {
			int selstartbak = Ed_Title->SelStart;
			int sellengthbak = Ed_Title->SelLength;
			Ed_Title->HideSelection = !Fo_Search->Visible;
			Ed_Title->SelStart = selstartbak;
			Ed_Title->SelLength = sellengthbak;
		}
	}

	// 存在しないリンクが選択されていたときの処理（Undo時などに発生）
	if (m_nCurrentLink == m_nTargetLink) {
		TLink *Link = m_Document->GetLinkByIndex(m_nCurrentLink);
		if (Link == NULL) {
			m_nTargetLink = -1;
		}
	}

	// リンクリスト
	if (m_nRefreshLinkCount != m_Document->m_nRefreshLinkCount) {
		m_nRefreshLinkCount = m_Document->m_nRefreshLinkCount;
		RefreshLinks();

		// リンクタイトル変更反映
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

	// リンク
	TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
	if (m_nCurrentLink != m_nTargetLink) {
		m_nCurrentLink = m_nTargetLink;

		Ed_LinkTitle->Tag = 1;
		// SetCardVisible();

		if (Link) {
			// リンク選択中
			Pa_Card->Visible = false;
			Pa_Link->Visible = true;

			Ed_LinkTitle->Text = Link->m_Name;
			Ch_LinkDirection->Checked = Link->m_bDirection;
		}
		else {
			Ed_LinkTitle->Text = "";
			Ch_LinkDirection->Checked = false;

			// リンク非選択
			Pa_Link->Visible = false;
			Pa_Card->Visible = true;
		}

		Ed_LinkTitle->Tag = 0;

		// ラベル
		RefreshLabel();
		drawrequest = true;
	}

	// カード数、リンク数
	{
		UnicodeString S = IntToStr(m_Document->m_Cards->Count) +
			UnicodeString(" ") + MLText.Cards + UnicodeString(", ") +
			IntToStr(m_Document->m_Links->Count) + UnicodeString(" ") +
			MLText.Links;
		if (La_Count->Caption != S) {
			La_Count->Caption = S;
		}
	}

	// サイズ
	if (m_nTargetCard >= 0 && Card) {
		// サイズ表示変更
		int size = (int)(log(Card->m_nSize / 100.0) * (8 / log(4)) +
			100.5) - 100; // 8で4倍
		if (size > 8) {
			size = 8;
		}
		else if (size < -8) {
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
			// カード
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				TCard *Card = m_Document->GetCardByIndex(i);
				if (Card->m_bSelected) {
					Card->m_nShape = Fo_Shape->m_nItemIndex;
				}
			}
			m_Document->RefreshList();
			/*
			 if (m_nTargetCard >= 0 && Card){
			 //シェイプ変更反映
			 Card->m_nShape = Fo_Shape->m_nItemIndex;
			 m_Document->RefreshList();
			 }
			 */
			break;
		case 1:
			// リンク
			if (m_nTargetLink >= 0 && Link) {
				// シェイプ変更反映
				Link->m_nShape = Fo_Shape->m_nItemIndex;
				m_Document->RefreshLink();
			}
			break;
		case 2:
			// 全カード
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				m_Document->GetCardByIndex(i)->m_nShape =
					Fo_Shape->m_nItemIndex;
			}
			m_Document->RefreshList();
			break;
		case 3:
			// 全リンク
			for (int i = 0; i < m_Document->m_Links->Count; i++) {
				// シェイプ変更反映
				m_Document->GetLinkByIndex(i)->m_nShape =
					Fo_Shape->m_nItemIndex;
			}
			m_Document->RefreshLink();
			break;
		}
		Fo_Shape->m_bSelected = false;
	}
	if (m_nTargetCard >= 0 && Card) {
		// シェイプボタン表示変更
		if (Card->m_nShape != SB_Shape->Tag) {
			SB_Shape->Tag = Card->m_nShape;
			if (Card->m_nShape < IL_Shape->Count) {
				SB_Shape->Glyph->Canvas->FillRect(Rect(0, 0, 31, 13));
				IL_Shape->GetBitmap(Card->m_nShape, SB_Shape->Glyph);
			}
		}

	}

	if (m_nTargetLink >= 0 && Link) {
		// シェイプボタン表示変更
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

	// ルートカード
	if (SB_Top->Visible != (m_nTargetCard >= 0)) {
		SB_Top->Visible = (m_nTargetCard >= 0);
	}

	if (m_nTargetCard >= 0 && Card) {
		if (SB_Top->Down != Card->m_bTop) {
			SB_Top->Down = Card->m_bTop;
		}
	}

	// 表示切替
	if (Fo_View->m_bRefreshRequest) {
		Fo_View->m_bRefreshRequest = false;
		RefreshLabel();
		// SetCardVisible();
		drawrequest = true;
	}

	// ラベル
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
		// データに更新がった場合、描画パラメータを得るためにArrange前にここで描画
		/*
		 HRGN MyRgn;
		 if (PC_Client->ActivePage != TS_Browser){
		 MyRgn = ::CreateRectRgn(0, 0, 0, 0);
		 ::SelectClipRgn(PB_Browser->Canvas->Handle, MyRgn);
		 }
		 PB_BrowserPaint(Sender);
		 if (PC_Client->ActivePage != TS_Browser){
		 ::SelectClipRgn(PB_Browser->Canvas->Handle, NULL);
		 ::DeleteObject(MyRgn);
		 }
		 */
		PB_BrowserPaint(Sender);
		RefreshLaStatus();
	}

	if (drawrequest && Pa_GlobalSearch->Visible) {
		PB_GlobalSearchPaint(Sender);
	}

	// Bu_ArrangeTypeのキャプション
	{
		UnicodeString S;
		switch (Bu_ArrangeType->Tag) {
			// 0～99=Hard、100～199=Soft、200～299＝Matrix
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

			// 500～999＝Similarity
		case 500:
			S = PAT_Similarity->Caption;
			break;
		case 600:
			S = PAT_SimilaritySoft->Caption;
			break;

			// 1000～1999＝Tree
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
		(SB_Arrange->Down && Bu_ArrangeType->Tag >=
		500 && Bu_ArrangeType->Tag <= 999)) {
		SB_ArrangeRefresh->Enabled =
			(SB_Arrange->Down && Bu_ArrangeType->Tag >=
			500 && Bu_ArrangeType->Tag <= 999);
	}

	// ブラウザ座標
	m_fTickerSpeed = (tgt - m_nLastTimeOut) * (0.1f / 120);
	// 120msで0.1文字？//TickerSpeed
	m_fBGAnimationSpeed = (tgt - m_nLastTimeOut) * 0.001f;
	if (PC_Client->ActivePage == TS_Browser && (Application->Active ||
		SettingView.m_bRedrawInBackground) /* && !m_bMDownBrowser */) {
		bool b = true;
		if (SettingFile.fepOperation) {
			if (SettingFile.fepOperation(m_Document,
				UnicodeString("Browser_RedrawByTimer").c_str())) {
				b = false;
			}
		}
		if (b) {
			if (SB_Arrange->Down
				/* && timeGetTime() >= m_uMDownBrowserLast + 500 */) {
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
					/*
					 case 5:
					 case 6:
					 case 7:
					 //Date
					 BrowserArrangeByDate(Bu_ArrangeType->Tag - 2);
					 drawrequest = true;
					 break;
					 */
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

			// スコア、Ticker、背景アニメーションによる再描画
			drawrequest2 |= SettingView.m_bScore ||
				SettingView.m_bTickerVisible || SettingView.m_bBGAnimation;

			// オートスクロールによる再描画
			if (!(m_bMDownBrowser > 0 && m_bMDownBrowser < 4)
				&& m_bMDownBrowser != 5) { // 4はoverviewから表示位置指定中なのでAutoScroll
				if ((SB_AutoScroll->Down || SB_AutoZoom->Down ||
					m_nScrollTargetX > -65536) && timeGetTime() >=
					m_uMDownBrowserLast + 500) {
					drawrequest2 |= BrowserAutoScroll();
				}
			}

			// サムネイル、ビデオの更新による再描画
			TCursor crbak = Screen->Cursor;
			bool imageupdated = UpdateImageList() || UpdateVideoList();
			drawrequest2 |= imageupdated || m_VideoList->Count > 0;

			if (imageupdated) {
				Screen->Cursor = crbak;
			}

			// フォーカスカーソルによる再描画
			if (SettingView.m_bFocusCursor) {
				if (SettingView.m_bFocusCursorAnimation) {
					int speed = (tgt - m_nLastTimeOut) / 5; // 0.5秒でアニメーション;
					bool last = m_nFocusCursorPos % 100 < 50;
					if (m_nFocusCursorPos < 100) {
						m_nFocusCursorPos += speed;
						if (m_nFocusCursorPos > 100) {
							m_nFocusCursorPos = 100; // いきなり点滅に入るのを防ぐ
						}
					}
					else {
						// 点滅（分かりやすいが目障り）
						m_nFocusCursorPos += (tgt - m_nLastTimeOut) / 10;
					}
					drawrequest2 |= m_nFocusCursorPos <
						100 || (m_nFocusCursorPos % 100 < 50) != last;

					// 普通のカードのSelection
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
							}
							else {
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

	// 図形
	if (PC_Client->ActivePage == TS_Drawing) {
		if (m_Drawing->m_bDrawRequest) {
			PB_DrawingPaint(Sender);
		}
	}

	// 統計
	if (PC_Client->ActivePage == TS_Statistics) {
		// スムーズ描画による再描画
		if (m_fStatisticsPos < 1.0f) {
			m_fStatisticsPos = 1.03f - (1.03f - m_fStatisticsPos) * pow(0.5,
				(tgt - m_nLastTimeOut) / 150.0); // 150msで半分1.0に近づく
			if (m_fStatisticsPos > 1.0f) {
				m_fStatisticsPos = 1.0f;
			}
			drawrequest_s = true;
		}

		// 背景アニメーションによる再描画
		drawrequest_s |= SettingView.m_bBGAnimation;

		if (drawrequest_s) {
			PB_StatisticsPaint(Sender);
		}
	}

	// Ed_TitleBの位置
	/*
	 {
	 int size = RE_Edit->Font->Size;
	 if (m_nTargetCard >= 0){
	 size = (size * Card->m_nSize) / 100;
	 }
	 if (Ed_TitleB->Font->Size != size){
	 Ed_TitleB->Font->Size = size;
	 }
	 }
	 */
	if (PC_Client->ActivePage == TS_Browser) {
		if (m_nTargetCard >= 0) {
			if (Ed_TitleB->Visible) {
				SetEdTitleBPos();
			}
		}
		if (SettingView.m_nEditInBrowserPos == 0) {
			SettingView.m_bEditInBrowserAlways = false;
		}
		if (RE_Edit->Parent != Pa_Editor || SettingView.m_bEditInBrowserAlways)
		{
			SetTextEditPos();
		}
	}

	// 検索
	if (bSearchRequest) {
		int srbak = bSearchRequest;
		bSearchRequest = 0;
		if (SearchKeyword != WideString("")) {
			// キーワード履歴保存
			for (int i = 0; i < Fo_Search->Co_Keyword->Items->Count; i++) {
				if (SearchKeyword == Fo_Search->Co_Keyword->Items->Strings[i]) {
					Fo_Search->Co_Keyword->Items->Delete(i);
					break;
				}
			}
			while (Fo_Search->Co_Keyword->Items->Count >= 100) {
				Fo_Search->Co_Keyword->Items->Delete
					(Fo_Search->Co_Keyword->Items->Count - 1);
			}
			Fo_Search->Co_Keyword->Items->Insert(0, SearchKeyword);

			if (srbak & 0x40) {
				// グローバル検索
				if (!(srbak & 0x100)) {
					La_SearchResultKeyword->Caption =
						UnicodeString("\"") + SearchKeyword +
						UnicodeString("\"");

					GlobalSearch(srbak);
					m_GlobalSearchOption = srbak;
					m_GlobalSearchKeyword = SearchKeyword;
					// 置換ではない

					// グローバル検索結果を表示
					Sc_GlobalSearch->Tag = 1;
					Sc_GlobalSearch->Position = 0;
					Sc_GlobalSearch->Tag = 0;
					m_GlobalSearchCursorIndex = -1;
					La_GlobalSearchCount->Caption =
						IntToStr(m_GlobalSearchResult->Count) +
						UnicodeString(" Hits");
					if (m_GlobalSearchResult->Count) {
						// 見つかった
						ExitFullScreen();
						Fo_Search->Visible = false;
						Sc_GlobalSearch->Max = m_GlobalSearchResult->Count - 1;
						if (!Pa_GlobalSearch->Visible) {
							Pa_Client->Align = alNone;
							Pa_GlobalSearch->Visible = true;
							Sp_GlobalSearch->Visible = true;
							Pa_Client->Align = alClient;
						}
						else {
							PB_GlobalSearchPaint(Sender);
						}
						Sc_GlobalSearch->SetFocus();
					}
					else {
						// 見つからなかった
						Sc_GlobalSearch->Max = 0;
						PB_GlobalSearchPaint(Sender);
						ShowMessage(UnicodeString("\"") + SearchKeyword +
							UnicodeString("\"") + MLText.NotFound);
					}
				}
				else {
					// 置換
					int replaced = ReplaceAll(srbak);

					if (replaced) {
						// 置換成功
						Fo_Search->Visible = false;
					}
					else {
						// 見つからなかった
						ShowMessage(UnicodeString("\"") + SearchKeyword +
							UnicodeString("\"") + MLText.NotFound);
					}
				}
			}
			else {
				Search(srbak);
				if (srbak & 0x8) {
					Fo_Search->SetFocus();
				}
				else if (srbak & 0x4) {
					Ed_FindCard->SetFocus();
				}
			}
		}
	}
	if (Ed_FindCard->Text != SearchKeyword) {
		Ed_FindCard->Text = SearchKeyword;
	}

	// メニューアイテム

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
	}
	else {
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
	} {
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
		// Cut（文字列が選択されているか、カードが選択されていればEnable）
		bool editenabled = (RE_Edit->Focused() && RE_Edit->SelLength > 0) ||
			(Ed_Title->Focused() && Ed_Title->SelLength > 0) ||
			(Ed_TitleB->Focused() && Ed_TitleB->SelLength > 0) ||
			(Ed_LinkTitle->Focused() && Ed_LinkTitle->SelLength > 0) ||
			(Ed_FindCard->Focused() && Ed_FindCard->SelLength > 0) ||
			(Ed_FindCard->Focused() && Ed_FindCard->SelLength > 0) ||
			(TB_DZoom->Focused() && m_Drawing->Selected() > 0) ||
			(!RE_Edit->Focused() && !Me_EditorRelated->Focused()
			&& !Ed_Title->Focused() && !Ed_TitleB->Focused()
			&& !Ed_LinkTitle->Focused() && !Ed_FindCard->Focused()
			&& !TB_DZoom->Focused() && m_nTargetCard >= 0);
		if (ME_Cut->Enabled != editenabled) {
			ME_Cut->Enabled = editenabled;
			PE_Cut->Enabled = editenabled;
			PD_Cut->Enabled = editenabled;
			PBC_Cut->Enabled = editenabled;
		}
	} {
		// Copy（文字列が選択されているか、カードが選択されていればEnable）
		bool editenabled = (RE_Edit->Focused() && RE_Edit->SelLength > 0) ||
			(Me_EditorRelated->Focused() && Me_EditorRelated->SelLength > 0) ||
			(Ed_Title->Focused() && Ed_Title->SelLength > 0) ||
			(Ed_TitleB->Focused() && Ed_TitleB->SelLength > 0) ||
			(Ed_LinkTitle->Focused() && Ed_LinkTitle->SelLength > 0) ||
			(Ed_FindCard->Focused() && Ed_FindCard->SelLength > 0) ||
			(TB_DZoom->Focused() && m_Drawing->Selected() > 0) ||
			(!RE_Edit->Focused() && !Me_EditorRelated->Focused()
			&& !Ed_Title->Focused() && !Ed_TitleB->Focused()
			&& !Ed_LinkTitle->Focused() && !Ed_FindCard->Focused()
			&& !TB_DZoom->Focused() && m_nTargetCard >= 0);
		if (ME_Copy->Enabled != editenabled) {
			ME_Copy->Enabled = editenabled;
			PE_Copy->Enabled = editenabled;
			PD_Copy->Enabled = editenabled;
			PBC_Copy->Enabled = editenabled;
		}
	}

	{
		// Delete（文字編集中か、カードが選択されていればEnable）
		bool editenabled = RE_Edit->Focused() || Ed_Title->Focused() ||
			Ed_TitleB->Focused() || Ed_LinkTitle->Focused() ||
			Ed_FindCard->Focused() ||
			(TB_DZoom->Focused() && m_Drawing->Selected() > 0) ||
			(!RE_Edit->Focused() && !Me_EditorRelated->Focused()
			&& !Ed_Title->Focused() && !Ed_TitleB->Focused()
			&& !Ed_LinkTitle->Focused() && !Ed_FindCard->Focused()
			&& !TB_DZoom->Focused() && m_nTargetCard >= 0);
		if (ME_Delete->Enabled != editenabled) {
			ME_Delete->Enabled = editenabled;
			PE_Delete->Enabled = editenabled;
			PD_Delete->Enabled = editenabled;
		}
	}

	{
		// Paste
		bool editenabled = Clipboard()->HasFormat(CF_TEXT) &&
			(RE_Edit->Focused() || Ed_Title->Focused() || Ed_TitleB->Focused()
			|| Ed_LinkTitle->Focused() || Ed_FindCard->Focused() ||
			TB_DZoom->Focused() || PC_Client->ActivePage == TS_Browser);
		// テキストがペースト可能
		editenabled &= !Me_EditorRelated->Focused(); // 関連テキストは編集不能
		if (ME_Paste->Enabled != editenabled) {
			ME_Paste->Enabled = editenabled;
			PE_Paste->Enabled = editenabled;
			PD_Paste->Enabled = editenabled;
			PBN_Paste->Enabled = editenabled;
			PBC_Paste->Enabled = editenabled;
		}
	} {
		// Cutしてカード作成
		bool editenabled = RE_Edit->SelLength > 0;
		if (PE_CutToNewCard->Enabled != editenabled) {
			PE_CutToNewCard->Enabled = editenabled;
			PE_CutToNewCardTitle->Enabled = editenabled;
			PE_CutToNewCardTitleWithLink->Enabled = editenabled;
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
		if (ME_WebSearch->Enabled != websearchenabled || LastSelText != SelText)
		{
			ME_WebSearch->Enabled = websearchenabled;
			PBC_WebSearch->Enabled = websearchenabled;
			PBL_WebSearch->Enabled = websearchenabled;
			for (int i = 0; i < MI_WebSearch->Count; i++) {
				TMenuItem *MI = (TMenuItem*)MI_WebSearch->Items[i];
				if (websearchenabled) {
					MI->Caption =
						Setting2Function.m_WebSearch->Names
						[i % Setting2Function.m_WebSearch->Count] +
						UnicodeString(" \"") + SelText + UnicodeString("\"");
				}
				else {
					MI->Caption =
						Setting2Function.m_WebSearch->Names
						[i % Setting2Function.m_WebSearch->Count];
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

	/*
	 if (MVLi_None->Checked != (SettingView.m_nLinkLimitation == -1)){
	 MVLi_None->Checked = (SettingView.m_nLinkLimitation == -1);
	 }
	 if (MVLi_1->Checked != (SettingView.m_nLinkLimitation == 1)){
	 MVLi_1->Checked = (SettingView.m_nLinkLimitation == 1);
	 }
	 if (MVLi_2->Checked != (SettingView.m_nLinkLimitation == 2)){
	 MVLi_2->Checked = (SettingView.m_nLinkLimitation == 2);
	 }
	 if (MVLi_3->Checked != (SettingView.m_nLinkLimitation == 3)){
	 MVLi_3->Checked = (SettingView.m_nLinkLimitation == 3);
	 }
	 if (MVLi_4->Checked != (SettingView.m_nLinkLimitation == 4)){
	 MVLi_4->Checked = (SettingView.m_nLinkLimitation == 4);
	 }
	 if (MVLi_5->Checked != (SettingView.m_nLinkLimitation == 5)){
	 MVLi_5->Checked = (SettingView.m_nLinkLimitation == 5);
	 }
	 */

	// View→Card
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
	} {
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
	} {
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

	// View→Link
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

	// View→Label
	if (MVL_LabelCircle->Checked != SettingView.m_bLabelCircleVisible) {
		MVL_LabelCircle->Checked = SettingView.m_bLabelCircleVisible;
	}
	if (MVL_LabelRectangle->Checked != SettingView.m_bLabelRectangleVisible) {
		MVL_LabelRectangle->Checked = SettingView.m_bLabelRectangleVisible;
	}
	if (MVL_LabelName->Checked != SettingView.m_bLabelNameVisible) {
		MVL_LabelName->Checked = SettingView.m_bLabelNameVisible;
	}

	// View→Text
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
			// 右端で折り返す
			RE_Edit->ScrollBars = ssVertical;
		}
		else {
			// 折り返さない
			RE_Edit->ScrollBars = ssBoth;
		}
		if (PC_Client->ActivePage == TS_Browser) {
			m_bRedrawRequested = true;
			/*
			 PC_Client->ActivePage = TS_Editor;
			 PC_Client->ActivePage = TS_Browser;
			 */
		}
	}
	if (MVT_EditInBrowser->Checked != SettingView.m_bEditInBrowser) {
		MVT_EditInBrowser->Checked = SettingView.m_bEditInBrowser;
	}
	if (MVT_AlwaysShowEditor->Checked != SettingView.m_bEditInBrowserAlways) {
		MVT_AlwaysShowEditor->Checked = SettingView.m_bEditInBrowserAlways;
	} {
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

	// カードポップアップ
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

	/*
	 //View->Read
	 if (MV_Read->Visible != (m_Agent != NULL && m_TTS != NULL)){
	 MV_Read->Visible = m_Agent != NULL && m_TTS != NULL;
	 }
	 if (MVR_Read->Checked != SettingView.m_bRead){
	 MVR_Read->Checked = SettingView.m_bRead;
	 if (MVR_Read->Checked){
	 //キャラクタ読み込み
	 LoadAgent();
	 }else{
	 //エージェント破棄
	 UnloadAgent();
	 }
	 }
	 */

	// View→Others
	if (MVO_CardList->Checked != SettingView.m_bCardList) {
		MVO_CardList->Checked = SettingView.m_bCardList;
	}
	if (MVO_FileList->Checked != SettingView.m_bFileList) {
		MVO_FileList->Checked = SettingView.m_bFileList;
	}

	if (MVO_Overview->Checked != SettingView.m_bOverview) {
		MVO_Overview->Checked = SettingView.m_bOverview;
	}

	// ポップアップメニュー
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

	// 表示色
	if (LB_List->Font->Color != (TColor)SettingView.m_nFourgroundColor) {
		LB_List->Font->Color = (TColor)SettingView.m_nFourgroundColor;
		LB_FileList->Font->Color = (TColor)SettingView.m_nFourgroundColor;
		RE_Edit->Font->Color = (TColor)SettingView.m_nFourgroundColor;
		Me_EditorRelated->Font->Color = (TColor)SettingView.m_nFourgroundColor;
		LB_Link->Font->Color = (TColor)SettingView.m_nFourgroundColor;
		/*
		 Ed_Title->Font->Color = (TColor)SettingView.m_nFourgroundColor;
		 Ed_CardSize->Font->Color = (TColor)SettingView.m_nFourgroundColor;
		 Ed_LinkTitle->Font->Color = (TColor)SettingView.m_nFourgroundColor;
		 Ed_FindCard->Font->Color = (TColor)SettingView.m_nFourgroundColor;
		 */
	}
	if (LB_List->Color != (TColor)SettingView.m_nBackgroundColor) {
		LB_List->Color = (TColor)SettingView.m_nBackgroundColor;
		LB_FileList->Color = (TColor)SettingView.m_nBackgroundColor;
		RE_Edit->Color = (TColor)SettingView.m_nBackgroundColor;
		Me_EditorRelated->Color = (TColor)SettingView.m_nBackgroundColor;
		LB_Link->Color = (TColor)SettingView.m_nBackgroundColor;
		/*
		 Ed_Title->Color = (TColor)SettingView.m_nBackgroundColor;
		 Ed_CardSize->Color = (TColor)SettingView.m_nBackgroundColor;
		 Ed_LinkTitle->Color = (TColor)SettingView.m_nBackgroundColor;
		 Ed_FindCard->Color = (TColor)SettingView.m_nBackgroundColor;
		 */
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
		} // なぜかTB_Zoomの背景色が異なる問題対策
	}

	// 壁紙
	if (MVO_WallPaper->Checked != (SettingView.m_WallPaper != "")) {
		MVO_WallPaper->Checked = (SettingView.m_WallPaper != "");
	}
	if (MVO_FixWallPaper->Checked != SettingView.m_bFixWallPaper) {
		MVO_FixWallPaper->Checked = SettingView.m_bFixWallPaper;
	}
	if (MVO_TileWallPaper->Checked != SettingView.m_bTileWallPaper) {
		MVO_TileWallPaper->Checked = SettingView.m_bTileWallPaper;
	}

	// 背景アニメーション
	if (MVO_BGAnimation->Checked != SettingView.m_bBGAnimation) {
		MVO_BGAnimation->Checked = SettingView.m_bBGAnimation;
	} {
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
	// カーソルアニメーション
	if (MVO_CursorAnimation->Checked != SettingView.m_bFocusCursorAnimation) {
		MVO_CursorAnimation->Checked = SettingView.m_bFocusCursorAnimation;
	}
	// 即スクロール
	if (MVO_NoScrollLag->Checked != SettingView.m_bNoScrollLag) {
		MVO_NoScrollLag->Checked = SettingView.m_bNoScrollLag;
	}

	// アンチエイリアス
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

	// 統計のキー
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

	// テキスト編集ボックス
	if (m_bTextEditRequested) {
		if (PC_Client->ActivePage == TS_Browser && m_nTargetCard >= 0) {
			SetTextEditPos();
			RE_Edit->SetFocus();
		}
		m_bTextEditRequested = false;
		m_bTitleEditRequested = false;
	}
	else if (m_bTitleEditRequested) {
		if (PC_Client->ActivePage == TS_Browser && m_nTargetCard >= 0) {
			SetEdTitleBPos();
			Ed_TitleB->Visible = true;
			Ed_TitleB->SetFocus();
		}
		m_bTitleEditRequested = false;
	}

	// CheckCountのインクリメント
	m_Document->m_nCheckCount++;

	m_nLastTimeOut = tgt;
	m_fTickerSpeed = 0.0;
	m_fBGAnimationSpeed = 0.0;
}

// ---------------------------------------------------------------------------
/*
 void TFo_Main::LoadAgent()
 {
 if (MVR_Read->Checked){
 m_Agent->Characters->Load(WideString("Agent"), Variant(SettingView.m_ReadAgentFile));
 m_AgentChar = m_Agent->Characters->Character(WideString("Agent"));
 m_AgentChar->MoveTo(Left + Width - 160, Top + Height - 160);
 m_AgentChar->Show();
 }
 }
 //---------------------------------------------------------------------------
 void TFo_Main::UnloadAgent()
 {
 if (m_AgentChar){
 m_AgentChar->StopAll();
 m_AgentChar->Hide();
 m_Agent->Characters->Unload(WideString("Agent"));
 m_AgentChar = NULL;
 }
 }
 */
// ---------------------------------------------------------------------------
void TFo_Main::SetTextEditPos() {
	int cardindex = m_Document->SearchCardIndex(m_nTargetCard);
	int add = 2;
	if (Fo_FullScreen->Visible) {
		add = 0;
	}

	switch (SettingView.m_nEditInBrowserPos*!Fo_FullScreen->Visible) {
	case 0:
		if (cardindex >= 0) {
			// Browser上
			RE_Edit->Align = alNone;
			RE_Edit->Width = m_nBrowserWidth / 3;
			RE_Edit->Height = m_nBrowserHeight / 3;
			RE_Edit->Left = m_CardX[cardindex] -
				m_CardWidth[cardindex] / 2 + add;
			RE_Edit->Top = m_CardY[cardindex] +
				m_CardHeight[cardindex] / 2 + add;
			if (Fo_FullScreen->Visible) {
				RE_Edit->Parent = Fo_FullScreen;
			}
			else {
				RE_Edit->Parent = Pa_Browser;
			}
		}
		else {
			return;
		}
		break;
	case 1:
		// Browser右
		if (SettingView.m_nEditorWidthInBrowser == 0 ||
			SettingView.m_nEditorWidthInBrowser >= TS_Browser->Width) {
			SettingView.m_nEditorWidthInBrowser = TS_Browser->Width / 4;
		}
		RE_Edit->Width = SettingView.m_nEditorWidthInBrowser;
		/*
		 if (RE_Edit->Left < TS_Browser->Width / 4){
		 RE_Edit->Left = TS_Browser->Width;
		 }
		 */
		Sp_BrowserRight->Left =
			(TS_Browser->Width - Sc_Y->Width -
			SettingView.m_nEditorWidthInBrowser) - Sp_BrowserRight->Width;
		Sp_BrowserRight->Visible = true;
		RE_Edit->Parent = TS_Browser;
		RE_Edit->Left = TS_Browser->Width - Sc_Y->Width -
			SettingView.m_nEditorWidthInBrowser;
		RE_Edit->Align = alRight;
		break;
	case 2:
		// Browser下
		if (SettingView.m_nEditorHeightInBrowser == 0 ||
			SettingView.m_nEditorHeightInBrowser >= TS_Browser->Height) {
			SettingView.m_nEditorHeightInBrowser = TS_Browser->Height / 4;
		}
		RE_Edit->Height = SettingView.m_nEditorHeightInBrowser;
		/*
		 if (RE_Edit->Top < TS_Browser->Height / 5){
		 RE_Edit->Top = TS_Browser->Height;
		 }
		 */
		Sp_BrowserBottom->Top = TS_Browser->Height - Pa_BrowserBottom->Height -
			SettingView.m_nEditorHeightInBrowser - Sp_BrowserBottom->Height;
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
		}
		else {
			Ed_TitleB->Parent = Pa_Browser;
			C = PB_Browser->Canvas;
		}

		Ed_TitleB->Font->Style = TFontStyles() << fsBold;
		float SizeX = Card->m_nSize;
		for (int il = 0; il < Card->m_Labels->Count; il++) {
			TCardLabel *Label = m_Document->GetLabelByIndex(0,
				Card->m_Labels->GetLabel(il) - 1);
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

		// IMEの入力状態に従ってWindowを大きくする
		{
			// COMPOSITIONFORM CompForm;
			HIMC hImc = ImmGetContext(Ed_TitleB->Handle);
			if (hImc) {
				// IMEの座標を得る
				/*
				 //ImmGetCompositionWindowではリアルタイムに変化する情報は得られない？
				 //ImmGetCompositionStringで変換中の文字列の情報を取得すればよい？
				 if (ImmGetCompositionWindow(hImc,&CompForm)){
				 CompForm.
				 int w = CompForm.rcArea.right - CompForm.rcArea.left;
				 if (w > width){
				 width = w;
				 }
				 }
				 */
				char buf[1024] = {0};
				// M_File->Caption="test";
				long err = ImmGetCompositionString(hImc, GCS_COMPSTR, buf,
					sizeof(buf));
				if (err != IMM_ERROR_NODATA && err != IMM_ERROR_GENERAL &&
					err > 0) {
					// M_File->Caption = buf;
					long DW = Ed_TitleB->Perform(EM_POSFROMCHAR,
						Ed_TitleB->SelStart, (NativeInt)0);
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
			for (int il = 0; il < SettingView.m_nTickerLines && il <
				Card->m_Lines->Count; il++) {
				ticker |= Trim(Card->m_Lines->Strings[il]) != "";
			}
			if (ticker) {
				tickerheight = lineheight * 0.8f;
			}
		}

		Ed_TitleB->Left = m_CardX[cardindex] - m_CardWidth[cardindex] / 2 +
			Space / 2;
		Ed_TitleB->Top = m_CardY[cardindex] + m_CardHeight[cardindex] / 2 -
			Space / 2 - height - tickerheight;

		/*
		 if (Fo_FullScreen->Visible){
		 Ed_TitleB->Left = m_CardX[cardindex] - m_CardWidth[cardindex] / 2 + Space / 2;
		 Ed_TitleB->Top = m_CardY[cardindex] + m_CardHeight[cardindex] / 2 - Space / 2 - height - tickerheight;
		 }else{
		 //PB_Browserが置いてあるパネルの縁取りの分の座標2ドットを足している
		 Ed_TitleB->Left = m_CardX[cardindex] - m_CardWidth[cardindex] / 2 + Space / 2 + 2;
		 Ed_TitleB->Top = m_CardY[cardindex] + m_CardHeight[cardindex] / 2 - Space / 2 - height - tickerheight + 2;
		 }
		 */
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormShow(TObject *Sender) {
	// 言語メニュー
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
	Pa_List->Width = SettingView.m_nCardListWidth; // 185;
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
	}
	else {
		m_nTargetCard = 0;
	}

	Randomize();

	/*
	 if (SettingFile.m_bCheckNew) if (IsInternetConnected()){
	 AnsiString S;
	 GetHtml(InfoURL, S);
	 int Size = S.Length();
	 int Size2 = Ini->ReadInteger("Info", "InfoSize", 0);
	 if (Size != Size2){
	 Ini->WriteInteger("Info", "InfoSize", Size);
	 MH_CheckLatestClick(Sender);
	 }
	 }
	 */

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
		// テキスト編集前にUndo用バックアップを行う
		unsigned int tgt = timeGetTime();
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
				}
				else if (Sender == Ed_TitleB) {
					Card->m_Title = EncodeES(Ed_TitleB->Text);
				}
			}
		}
		Ed_Title->Tag = 1;
		if (Sender == Ed_Title) {
			Ed_TitleB->Text = DecodeES(Ed_Title->Text);
		}
		else if (Sender == Ed_TitleB) {
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
		if (RE_Edit->Tag == 0) {
			TCard *Card = m_Document->GetCard(m_nCurrentCard);
			if (Card) {
				int len = RE_Edit->SelLength;
				if (m_nLastSelLength >= 0) {
					len = m_nLastSelLength;
					m_nLastSelLength = 0;
				}
				TextEditBackupSub(MLText.EditCardText, m_nCurrentCard,
					RE_Edit->SelStart, len);

				m_Document->SetCardText(Card, RE_Edit->Text);
				m_Document->RefreshList();

				if (PC_Client->ActivePage == TS_Editor) {
					// テキスト編集のたびにいちいち画面更新させないため
					m_nRefreshListCount = m_Document->m_nRefreshListCount;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshLaStatus() {
	if (m_nCurrentCard >= 0) {
		TCard *Card = m_Document->GetCard(m_nCurrentCard);

		if (Card) {
			// ステータスバー
			UnicodeString S = "";

			// 選択中カード数
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

			// Label
			S += IntToStr(Card->m_Labels->Count) + UnicodeString(" ") +
				MLText.Labels + UnicodeString("  ");

			// LinkNum
			int lcount = 0;
			for (int il = 0; il < m_Document->m_Links->Count; il++) {
				TLink *Link = m_Document->GetLinkByIndex(il);
				if (Link->m_nFromID == Card->m_nID ||
					Link->m_nDestID == Card->m_nID) {
					lcount++;
				}
			}
			S += IntToStr(lcount) + UnicodeString(" ") + MLText.Links +
				UnicodeString("  ");

			// 文字数
			int line = RE_Edit->Perform(EM_LINEFROMCHAR, -1, (NativeInt)0);
			S += UnicodeString("X : ") +
				IntToStr(RE_Edit->SelStart - RE_Edit->Perform(EM_LINEINDEX,
				line, (NativeInt)0) + 1) + UnicodeString("  ");

			S += MLText.Line + UnicodeString(" : ") + IntToStr(line + 1) +
				UnicodeString("/") + IntToStr(Card->m_Lines->Count) +
				UnicodeString("  ");
			S += MLText.Len + UnicodeString(" : ") +
				IntToStr(WideString(RE_Edit->Text).SubString(1,
				RE_Edit->SelStart + 1).Length() - line * 2) + UnicodeString("/")
				+ IntToStr(WideString(Card->m_Lines->Text).Length() -
				Card->m_Lines->Count * 2) + UnicodeString("  ");
			S += MLText.Size + UnicodeString(" : ") +
				/* SizeToStr(RE_Edit->SelStart + 1) +
				 UnicodeString("/") + */ SizeToStr(Card->m_Lines->Text.Length())
				+ UnicodeString("  ");

			SB_Status->Panels->Items[0]->Text = S;

			S = "";
			// Date
			S += MLText.Created + UnicodeString(" : ") + DateTimeToStr
				(Card->m_fCreated) + UnicodeString("  ");
			S += MLText.Edited + UnicodeString(" : ") + DateTimeToStr
				(Card->m_fUpdated) + UnicodeString("  ");
			S += MLText.Viewed + UnicodeString(" : ") + DateTimeToStr
				(Card->m_fViewed) + UnicodeString("  ");

			SB_Status->Panels->Items[1]->Text = S;
		}
	}
	else {
		SB_Status->Panels->Items[0]->Text = "";
		SB_Status->Panels->Items[1]->Text = "";
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
void __fastcall TFo_Main::MF_NewClick(TObject *Sender) {
	if (SaveCheck()) {
		SB_CloseGlobalSearchClick(Sender); // Global検索表示を消す

		m_bContinuousLoad = false;
		delete m_Document;
		m_Document = new TDocument();
		m_UndoRedo->ClearUndos();
		m_UndoRedo->ClearRedos();
		m_nCurrentCard = -1;
		m_nTargetCard = 0;
		m_Document->GetCardByIndex(0)->m_bSelected = true;
		m_nTargetLink = -1;
		SettingView.m_nLinkTarget = -1; // LinkLimitationのリンク先カード制約
		SettingView.m_bNoLabelVisible = true;
		SettingView.m_bNoLinkLabelVisible = true;

		SB_Cursor->Down = true;
		SB_Arrange->Down = false;

		RefreshFileList();
	}
}

// ---------------------------------------------------------------------------
bool TFo_Main::SaveCheck() {
	if (m_Document->m_bChanged && !m_Document->m_bReadOnly) {
		int result = Application->MessageBox
			(UnicodeString(MLText.SaveCheck).c_str(),
			UnicodeString(AppTitle).c_str(), MB_YESNOCANCEL);
		switch (result) {
		case ID_YES:
			// 保存する
			if (Save()) {
				// 保存に成功
				return true;
			}
			else {
				// 保存に失敗
				return false;
			}
		case ID_NO:
			// 保存せずに新規作成or読み込みor終了
			return true;
		case ID_CANCEL:
			// キャンセル
			return false;
		}
	}
	// 変更されていないので即時新規作成or読み込みor終了
	return true;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MF_SaveClick(TObject *Sender) {
	Save();
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MF_SaveAsClick(TObject *Sender) {
	SaveAs();
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
				// 自分がFrom
				Card2 = m_Document->GetCard(Link->m_nDestID);
				SDirection = "-> ";
			}
			else if (Link->m_nDestID == m_nCurrentCard) {
				// 自分がDest
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
						S2 += m_Document->GetLabelByIndex(0,
							Card2->m_Labels->GetLabel(i2) - 1)->m_Name;
					}
					S2 += UnicodeString("] ");

					S = S2 + S;
				}

				if (Link->m_bDirection) {
					S = SDirection + S;
				}

				if (Link->m_Name != "") {
					S += UnicodeString(" (") + Link->m_Name +
						UnicodeString(")");
				}
				/*
				 switch (Card->m_nType){
				 case 1://Problem
				 switch (Card2->m_nType){
				 case 2:
				 S = "[この問題を解決する手法] " + S;
				 break;
				 case 3:
				 S = "[問題の背景] " + S;
				 break;
				 }
				 break;
				 case 2://Solution
				 switch (Card2->m_nType){
				 case 1:
				 S = "[この手法での解決が期待できる問題] " + S;
				 break;
				 case 3:
				 S = "[この手法を適用した結果] " + S;
				 break;
				 }
				 break;
				 case 3://Result
				 switch (Card2->m_nType){
				 case 1:
				 S = "[次の課題] " + S;
				 break;
				 case 2:
				 S = "[この結果につながった手法] " + S;
				 break;
				 }
				 break;
				 }
				 */
				/*
				 switch (Card->m_nType){
				 case 1://Problem
				 switch (Card2->m_nType){
				 case 2:
				 S = S + "[The ways to solve this problem]";
				 break;
				 case 3:
				 S = S + "[The background of this problem]";
				 break;
				 }
				 break;
				 case 2://Solution
				 switch (Card2->m_nType){
				 case 1:
				 S = S + "[This solution will solve the problem]";
				 break;
				 case 3:
				 S = S + "[The result of this solution]";
				 break;
				 }
				 break;
				 case 3://Result
				 switch (Card2->m_nType){
				 case 1:
				 S = S + "[Next Problem]";
				 break;
				 case 2:
				 S = S + "[The solution]";
				 break;
				 }
				 break;
				 }
				 */
				LB_Link->Items->Add(S);
				m_LinkIndexes->Add((void *)i);
			}
		}
	}
	LB_Link->Items->EndUpdate();
}

// ---------------------------------------------------------------------------
bool TFo_Main::Save() {
	if (m_Document->m_bReadOnly) {
		// 読み込み専用ならなにもしない
		return false;
	}

	// 上書きもしくは名前をつけて保存

	if (m_Document->m_FN == "") {
		// ファイル名がないので名前をつけて保存
		return SaveAs();
	}

	// 保存処理
	TCursor crbak = Screen->Cursor;
	Screen->Cursor = crHourGlass;

	m_Document->m_nCardID = m_nTargetCard;
	Ti_Check->Enabled = false;
	Application->ProcessMessages();

	// 表示設定反映
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

	// 保存
	bool result = m_Document->Save();

	// リクエストリセット
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
		ShowMessage(MLText.FailedToSave + m_Document->m_FN +
			UnicodeString("."));
	}
	Ti_Check->Enabled = true;
	if (result && m_bContinuousLoad) {
		m_nCLFileAge = FileAge(m_Document->m_FN);
	}

	// 最近使ったファイル更新
	RefreshRecent(m_Document->m_FN);

	// 画面左のファイルリスト更新
	RefreshFileList();

	Screen->Cursor = crbak;

	return result;
}

// ---------------------------------------------------------------------------
bool TFo_Main::SaveAs() {
	if (m_Document->m_bReadOnly) {
		// 読み込み専用ならなにもしない
		return false;
	}

	// 名前をつけて保存

	if (SD->Execute()) {
		// ファイル名が決まった
		m_Document->m_FN = SD->FileName;

		return Save();
	}
	else {
		// ファイル名が決まらないのでキャンセル
		return false;
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::LoadSub(UnicodeString FN, bool bSoftLoad, bool bRefreshRecent) {
	SB_CloseGlobalSearchClick(this); // Global検索表示を消す

	TCursor crbak = Screen->Cursor;
	Screen->Cursor = crHourGlass;

	// 最近使ったファイル更新
	if (bRefreshRecent) {
		RefreshRecent(FN);
	}

	// 読み込み
	bool result = m_Document->Load(FN, bSoftLoad);

	if (result) {
		// 読み込み成功

		// 画面更新（最初のカードを表示）
		if (bSoftLoad) {
			m_nCurrentCard = -1;
			m_nTargetLink = -1;
			if (m_Document->m_Cards->Count) {
				if (m_Document->m_nCardID >= 0) {
					m_nTargetCard = m_Document->m_nCardID;
				}
				else {
					if (m_Document->SearchCardIndex(m_nTargetCard) < 0) {
						m_nTargetCard = -1;
					}
				}
			}
		}
		else {
			m_UndoRedo->ClearUndos();
			m_UndoRedo->ClearRedos();

			m_nCurrentCard = -2;
			m_nTargetCard = -1;
			SettingView.m_nLinkTarget = -1; // LinkLimitationのリンク先カード制約
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
				else {
					// 最初のカードを強制選択？
					// m_nTargetCard = m_Document->GetCardByIndex(0)->m_nID;
				}
				m_nTargetLink = -1;
			}
		}

		SB_Cursor->Down = true;
		SB_Arrange->Down = false;

		m_bContinuousLoad = FN == ExtractFilePath(ParamStr(0)) +
			"_continuousload.fip";
		if (m_bContinuousLoad) {
			m_nCLFileAge = FileAge(FN);
		}

		RefreshFileList();
	}
	else {
		// 読み込み失敗！
	}

	Screen->Cursor = crbak;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MF_OpenClick(TObject *Sender) {
	if (SaveCheck()) {
		OD->InitialDir = ExtractFileDir(m_Document->m_FN);
		if (OD->Execute()) {
			// ファイル名が決まった

			LoadSub(OD->FileName);
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::FormCloseQuery(TObject *Sender, bool &CanClose) {
	CanClose = SaveCheck();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MF_ExitClick(TObject *Sender) {
	Close();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_UndoClick(TObject *Sender) {
	if (Ed_FindCard->Focused()) {
		Ed_FindCard->Undo();
	}
	else {
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

	/*
	 if (RE_Edit->Focused()){
	 RE_Edit->Undo();
	 }else if (Ed_Title->Focused()){
	 Ed_Title->Undo();
	 }else if (Ed_TitleB->Focused()){
	 Ed_TitleB->Undo();
	 }else if (Ed_LinkTitle->Focused()){
	 Ed_LinkTitle->Undo();
	 }else if (Ed_FindCard->Focused()){
	 Ed_FindCard->Undo();
	 }
	 */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_CutClick(TObject *Sender) {
	if (Ed_FindCard->Focused()) {
		Ed_FindCard->CutToClipboard();
	}
	else {
		m_nLastModified = 0;
		m_nLastSelLength = RE_Edit->SelLength;
		if (RE_Edit->Focused()) {
			RE_Edit->CutToClipboard();
		}
		else if (Ed_Title->Focused()) {
			Ed_Title->CutToClipboard();
		}
		else if (Ed_TitleB->Focused()) {
			Ed_TitleB->CutToClipboard();
		}
		else if (Ed_LinkTitle->Focused()) {
			Ed_LinkTitle->CutToClipboard();
		}
		else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
			BackupSub(ME_Cut->Caption);
			m_Document->CopyToClipboard();
			m_bDoNotBackup = true;
			PL_DeleteCardClick(Sender);
			m_bDoNotBackup = false;
		}
		else if (PC_Client->ActivePage == TS_Drawing) {
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
	}
	else {
		if (RE_Edit->Focused()) {
			RE_Edit->CopyToClipboard();
		}
		else if (Me_EditorRelated->Focused()) {
			Me_EditorRelated->CopyToClipboard();
		}
		else if (Ed_Title->Focused()) {
			Ed_Title->CopyToClipboard();
		}
		else if (Ed_TitleB->Focused()) {
			Ed_TitleB->CopyToClipboard();
		}
		else if (Ed_LinkTitle->Focused()) {
			Ed_LinkTitle->CopyToClipboard();
		}
		else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
			m_Document->CopyToClipboard();
		}
		else if (PC_Client->ActivePage == TS_Drawing) {
			m_Drawing->Copy();
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_PasteClick(TObject *Sender) {
	if (Ed_FindCard->Focused()) {
		Ed_FindCard->PasteFromClipboard();
	}
	else {
		m_nLastModified = 0;
		m_nLastSelLength = RE_Edit->SelLength;
		if (RE_Edit->Focused()) {
			RE_Edit->PasteFromClipboard();
		}
		else if (Ed_Title->Focused()) {
			Ed_Title->PasteFromClipboard();
		}
		else if (Ed_TitleB->Focused()) {
			Ed_TitleB->PasteFromClipboard();
		}
		else if (Ed_LinkTitle->Focused()) {
			Ed_LinkTitle->PasteFromClipboard();
		}
		else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
			BackupSub(ME_Paste->Caption);

			// ディスプレイ上の16ドット分を求める
			float cx1, cx2, dummy;
			DispPosToCardPos(0, 0, cx1, dummy);
			DispPosToCardPos(16, 0, cx2, dummy);
			m_Document->PasteFromClipboard(cx2 - cx1);
		}
		else if (PC_Client->ActivePage == TS_Drawing) {
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
	}
	else {
		m_nLastModified = 0;
		m_nLastSelLength = RE_Edit->SelLength;
		if (RE_Edit->Focused()) {
			RE_Edit->ClearSelection();
		}
		else if (Ed_Title->Focused()) {
			Ed_Title->ClearSelection();
		}
		else if (Ed_TitleB->Focused()) {
			Ed_TitleB->ClearSelection();
		}
		else if (Ed_LinkTitle->Focused()) {
			Ed_LinkTitle->ClearSelection();
		}
		else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
			PL_DeleteCardClick(Sender);
		}
		else if (PC_Client->ActivePage == TS_Drawing) {
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
		TLink *Link = m_Document->GetLinkByIndex
			((int)m_LinkIndexes->Items[LB_Link->ItemIndex]);
		if (Link) {
			m_Document->ClearCardSelection();
			if (Link->m_nFromID == m_nCurrentCard) {
				m_nTargetCard = Link->m_nDestID;
			}
			else if (Link->m_nDestID == m_nCurrentCard) {
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

		m_Document->DeleteLinkByIndex
			((int)m_LinkIndexes->Items[LB_Link->ItemIndex]);
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::CalcCardSize(TCanvas *C, TCard *Card, int Size, int cardindex) {
	// Y方向座標計算
	int Space = (m_nFontHeight * Size) / 100;
	if (Space < 1) {
		Space = 1;
	}

	TFont *F = C->Font;

	int imagex = 100; // イメージ倍率
	bool Drawing = false;
	TCardVideo *Video = NULL;
	TCardImage *Image = NULL;
	if (Card) {
		Drawing = SettingView.m_bDrawingVisible && (Card->m_Drawing->Count > 0);
		if (SettingView.m_bVideoVisible && Card->m_VideoFN != "") {
			Video = SearchVideo(Card->m_VideoFN);
		}
		if (SettingView.m_bImageVisible && !Video && Card->m_ImageFN != "") {
			Image = SearchImage(Card->m_ImageFN);
		}
	}

	TStringList *Title = new TStringList();
	WideString S = DecodeES(m_CardTitle[cardindex]);
	int lines = CountStr(S, "\n") + 1;
	if (lines <= 1) {
		Title->Add(S);
	}
	else {
		while (S != WideString("")) {
			Title->Add(SplitStrBy(S, "\n"));
		}
	}

	int lineheight = (m_nFontHeight * Size) / 100;
	int tickerheight = 0;
	bool ticker = false;
	if (SettingView.m_bTickerVisible && Card->m_Lines->Count > 0) {
		for (int il = 0; il < SettingView.m_nTickerLines && il <
			Card->m_Lines->Count; il++) {
			ticker |= Trim(Card->m_Lines->Strings[il]) != "";
		}
		if (ticker) {
			tickerheight = lineheight * 0.8f;
		}
	}
	m_CardHeight[cardindex] = lineheight * lines + tickerheight + Space;

	if (Drawing) {
		imagex =
			abs((m_nFontHeight * Size * F->Size) /
			(RE_Edit->Font->Height * SettingView.m_Font->Size));
		m_CardHeight[cardindex] +=
			SettingView.m_nImageLimitation * 0.75 * imagex * 0.01;
	}

	/*
	 VIDEOINFOHEADER *vi = NULL;
	 if (Video){
	 vi = Video->m_Video->GetVideoInfoHeader();
	 if (Video->m_bExist && vi){
	 imagex = abs((m_nFontHeight * Size * F->Size) / (RE_Edit->Font->Height * SettingView.m_Font->Size));
	 if (vi->bmiHeader.biHeight > vi->bmiHeader.biWidth){
	 //Y方向の方が大きい
	 if (vi->bmiHeader.biHeight > SettingView.m_nImageLimitation){
	 imagex = (imagex * SettingView.m_nImageLimitation) / vi->bmiHeader.biHeight;
	 }
	 }else{
	 //X方向の方が大きい
	 if (vi->bmiHeader.biWidth > SettingView.m_nImageLimitation){
	 imagex = (imagex * SettingView.m_nImageLimitation) / vi->bmiHeader.biWidth;
	 }
	 }
	 m_CardHeight[cardindex] += (vi->bmiHeader.biHeight * imagex) / 100;
	 }else{
	 Video = NULL;
	 }
	 }
	 */
	if (Image) {
		if (Image->m_bExist) {
			imagex =
				abs((m_nFontHeight * Size * F->Size) /
				(RE_Edit->Font->Height * SettingView.m_Font->Size));
			if (Image->m_Image->Height > Image->m_Image->Width) {
				// Y方向の方が大きい
				if (Image->m_Image->Height > SettingView.m_nImageLimitation) {
					imagex = (imagex * SettingView.m_nImageLimitation)
						/ Image->m_Image->Height;
				}
			}
			else {
				// X方向の方が大きい
				if (Image->m_Image->Width > SettingView.m_nImageLimitation) {
					imagex = (imagex * SettingView.m_nImageLimitation)
						/ Image->m_Image->Width;
				}
			}
			m_CardHeight[cardindex] += (Image->m_Image->Height * imagex) / 100;
		}
		else {
			Image = NULL;
		}
	}

	// 座標計算
	int OrgFontHeight = F->Height;
	F->Height = (F->Height * Size) / 100;
	if (F->Height == 0) {
		F->Height = -1;
	}

	// X方向座標計算
	m_CardWidth[cardindex] = C->TextWidth(Title->Strings[0]) + Space;
	for (int i = 1; i < Title->Count; i++) {
		int w = C->TextWidth(Title->Strings[i]) + Space;
		if (w > m_CardWidth[cardindex]) {
			m_CardWidth[cardindex] = w;
		}
	}
	if (Drawing)
		if (m_CardWidth[cardindex] < (SettingView.m_nImageLimitation * imagex)
			/ 100 + Space) {
			m_CardWidth[cardindex] = (SettingView.m_nImageLimitation * imagex)
				/ 100 + Space;
		}
	/*
	 if (Video) if (m_CardWidth[cardindex] < (vi->bmiHeader.biWidth * imagex) / 100 + Space){
	 m_CardWidth[cardindex] = (vi->bmiHeader.biWidth * imagex) / 100 + Space;
	 }
	 */
	if (Image)
		if (m_CardWidth[cardindex] < (Image->m_Image->Width * imagex)
			/ 100 + Space) {
			m_CardWidth[cardindex] =
				(Image->m_Image->Width * imagex) / 100 + Space;
		}
	F->Height = OrgFontHeight;
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawCard(TCanvas *C, TCard *Card, int Size, int cardindex,
	TColor HMColor, int option) {
	// Y方向座標計算
	int Space = (m_nFontHeight * Size) / 100;
	if (Space < 1) {
		Space = 1;
	}

	TBrush *B = C->Brush;
	TPen *P = C->Pen;
	TFont *F = C->Font;

	TColor PColor = P->Color; // 色保存
	TColor BColor = B->Color;

	int imagex = 100; // イメージ倍率
	bool Drawing = false;
	TCardVideo *Video = NULL;
	TCardImage *Image = NULL;
	if (Card) {
		Drawing = SettingView.m_bDrawingVisible && (Card->m_Drawing->Count > 0);
		if (SettingView.m_bVideoVisible && !Drawing && Card->m_VideoFN != "") {
			Video = SearchVideo(Card->m_VideoFN);
		}
		if (SettingView.m_bImageVisible && !Drawing && !Video &&
			Card->m_ImageFN != "") {
			Image = SearchImage(Card->m_ImageFN);
		}
	}

	TStringList *Title = new TStringList();
	WideString S = DecodeES(m_CardTitle[cardindex]);
	int lines = CountStr(S, "\n") + 1;
	if (lines <= 1) {
		Title->Add(S);
	}
	else {
		while (S != WideString("")) {
			Title->Add(SplitStrBy(S, "\n"));
		}
	}

	int lineheight = (m_nFontHeight * Size) / 100;
	int tickerheight = 0;
	bool ticker = false;
	if (SettingView.m_bTickerVisible && Card->m_Lines->Count > 0) {
		for (int il = 0; il < SettingView.m_nTickerLines && il <
			Card->m_Lines->Count; il++) {
			ticker |= Trim(Card->m_Lines->Strings[il]) != "";
		}
		if (ticker) {
			tickerheight = lineheight * 0.8f;
		}
	}
	m_CardHeight[cardindex] = lineheight * lines + tickerheight + Space;

	if (Drawing) {
		imagex =
			abs((m_nFontHeight * Size * F->Size) /
			(RE_Edit->Font->Height * SettingView.m_Font->Size));
		m_CardHeight[cardindex] +=
			SettingView.m_nImageLimitation * 0.75 * imagex * 0.01;
	}

	/*
	 VIDEOINFOHEADER *vi = NULL;
	 if (Video){
	 vi = Video->m_Video->GetVideoInfoHeader();
	 if (Video->m_bExist && vi){
	 imagex = abs((m_nFontHeight * Size * F->Size) / (RE_Edit->Font->Height * SettingView.m_Font->Size));
	 if (vi->bmiHeader.biHeight > vi->bmiHeader.biWidth){
	 //Y方向の方が大きい
	 if (vi->bmiHeader.biHeight > SettingView.m_nImageLimitation){
	 imagex = (imagex * SettingView.m_nImageLimitation) / vi->bmiHeader.biHeight;
	 }
	 }else{
	 //X方向の方が大きい
	 if (vi->bmiHeader.biWidth > SettingView.m_nImageLimitation){
	 imagex = (imagex * SettingView.m_nImageLimitation) / vi->bmiHeader.biWidth;
	 }
	 }
	 m_CardHeight[cardindex] += (vi->bmiHeader.biHeight * imagex) / 100;
	 }else{
	 Video = NULL;
	 }
	 }
	 */
	if (Image) {
		if (Image->m_bExist) {
			imagex =
				abs((m_nFontHeight * Size * F->Size) /
				(RE_Edit->Font->Height * SettingView.m_Font->Size));
			if (Image->m_Image->Height > Image->m_Image->Width) {
				// Y方向の方が大きい
				if (Image->m_Image->Height > SettingView.m_nImageLimitation) {
					imagex = (imagex * SettingView.m_nImageLimitation)
						/ Image->m_Image->Height;
				}
			}
			else {
				// X方向の方が大きい
				if (Image->m_Image->Width > SettingView.m_nImageLimitation) {
					imagex = (imagex * SettingView.m_nImageLimitation)
						/ Image->m_Image->Width;
				}
			}
			m_CardHeight[cardindex] += (Image->m_Image->Height * imagex) / 100;
		}
		else {
			Image = NULL;
		}
	}

	// 座標計算
	int top = m_CardY[cardindex] - m_CardHeight[cardindex] / 2;
	int bottom = m_CardY[cardindex] + m_CardHeight[cardindex] / 2;
	int bottom2 = bottom;
	if (option & 0x1) {
		bottom2 += m_nFontHeight / 2;
	}

	{
		int OrgFontHeight = F->Height;
		F->Height = (F->Height * Size) / 100;
		if (F->Height == 0) {
			F->Height = -1;
		}

		// X方向座標計算
		m_CardWidth[cardindex] = C->TextWidth(Title->Strings[0]) + Space;
		for (int i = 1; i < Title->Count; i++) {
			int w = C->TextWidth(Title->Strings[i]) + Space;
			if (w > m_CardWidth[cardindex]) {
				m_CardWidth[cardindex] = w;
			}
		}
		if (Drawing)
			if (m_CardWidth[cardindex] <
				(SettingView.m_nImageLimitation * imagex) / 100 + Space) {
				m_CardWidth[cardindex] =
					(SettingView.m_nImageLimitation * imagex) / 100 + Space;
			}
		/*
		 if (Video) if (m_CardWidth[cardindex] < (vi->bmiHeader.biWidth * imagex) / 100 + Space){
		 m_CardWidth[cardindex] = (vi->bmiHeader.biWidth * imagex) / 100 + Space;
		 }
		 */
		if (Image)
			if (m_CardWidth[cardindex] < (Image->m_Image->Width * imagex)
				/ 100 + Space) {
				m_CardWidth[cardindex] =
					(Image->m_Image->Width * imagex) / 100 + Space;
			}
		int left = m_CardX[cardindex] - m_CardWidth[cardindex] / 2;
		int right = m_CardX[cardindex] + m_CardWidth[cardindex] / 2;
		int right2 = right;
		if (option & 0x1) {
			right2 += m_nFontHeight / 2;
		}

		if (!((top < 0 && bottom2 < 0) || (top > m_nBrowserHeight && bottom2 >
			m_nBrowserHeight)) && !((left < 0 && right2 < 0) || (left >
			m_nBrowserWidth && right2 > m_nBrowserWidth))) {
			// 画面内

			// 文字枠描画
			B->Style = bsSolid;
			int drawtimes = 1;
			if (SettingView.m_bCardGradation && !(option & 0x1)) {
				// グラデーション設定で、影ではない
				drawtimes++;
			}
			while (drawtimes) {
				HRGN MyRgn, RgnBak;
				int rgnexist;

				if (SettingView.m_bCardGradation && !(option & 0x1)) {
					// MyRgn = ::CreateRectRgn(m_nXOffset + left + Space / 2, Y, m_nXOffset + right - Space / 2, Y + tickerheight);
					switch (drawtimes) {
					case 2:
						// 下半分
						MyRgn = ::CreateRectRgn(m_nXOffset,
							m_nYOffset + m_CardY[cardindex],
							m_nXOffset + m_nBrowserWidth,
							m_nYOffset + m_nBrowserHeight);
						break;
					case 1:
						// 上半分
						// B->Color = BrightColor(B->Color, 1.25f);
						B->Color = HalfColor(B->Color, 0xffffff, 0.25f);
						MyRgn = ::CreateRectRgn(m_nXOffset, m_nYOffset,
							m_nXOffset + m_nBrowserWidth,
							m_nYOffset + m_CardY[cardindex]);
						break;
					}

					RgnBak = ::CreateRectRgn(0, 0, 0, 0);
					rgnexist = ::GetClipRgn(C->Handle, RgnBak);
					if (rgnexist) {
						HRGN CmbRgn;
						CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
						::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
						::SelectClipRgn(C->Handle, CmbRgn);
						::DeleteObject(CmbRgn);
					}
					else {
						::SelectClipRgn(C->Handle, MyRgn);
					}

				}
				switch (m_CardShape[cardindex]) {
				case 0: // 描画なし
					break;
				case 1: // Rect
					C->Rectangle(m_nXOffset + left, m_nYOffset + top,
						m_nXOffset + right, m_nYOffset + bottom);
					break;
				case 3: // RRect2
					C->RoundRect(m_nXOffset + left, m_nYOffset + top,
						m_nXOffset + right, m_nYOffset + bottom,
						m_CardHeight[cardindex], m_CardHeight[cardindex]);
					break;
				case 4: // Ellipse
					C->Ellipse(m_nXOffset + left, m_nYOffset + top,
						m_nXOffset + right, m_nYOffset + bottom);
					break;
				case 5: // Dia
					{
						TPoint ps[4];
						// 上
						ps[0].x = m_nXOffset + m_CardX[cardindex];
						ps[0].y = m_nYOffset + top;
						// 右
						ps[1].x = m_nXOffset + right;
						ps[1].y = m_nYOffset + m_CardY[cardindex];
						// 下
						ps[2].x = m_nXOffset + m_CardX[cardindex];
						ps[2].y = m_nYOffset + bottom;
						// 左
						ps[3].x = m_nXOffset + left;
						ps[3].y = m_nYOffset + m_CardY[cardindex];
						C->Polygon(ps, 3);
					} break;
				case 6: // Hex
					{
						TPoint ps[6];
						// 左上
						ps[0].x = m_nXOffset + left +
							m_CardHeight[cardindex] / 3;
						ps[0].y = m_nYOffset + top;
						// 右上
						ps[1].x = m_nXOffset + right -
							m_CardHeight[cardindex] / 3;
						ps[1].y = m_nYOffset + top;
						// 右
						ps[2].x = m_nXOffset + right;
						ps[2].y = m_nYOffset + m_CardY[cardindex];
						// 下右
						ps[3].x = m_nXOffset + right -
							m_CardHeight[cardindex] / 3;
						ps[3].y = m_nYOffset + bottom;
						// 下左
						ps[4].x = m_nXOffset + left +
							m_CardHeight[cardindex] / 3;
						ps[4].y = m_nYOffset + bottom;
						// 左
						ps[5].x = m_nXOffset + left;
						ps[5].y = m_nYOffset + m_CardY[cardindex];
						C->Polygon(ps, 5);
					} break;
				case 7: // 台形上
					{
						TPoint ps[4];
						// 左上
						ps[0].x = m_nXOffset + left +
							m_CardHeight[cardindex] / 4;
						ps[0].y = m_nYOffset + top;
						// 右上
						ps[1].x = m_nXOffset + right -
							m_CardHeight[cardindex] / 4;
						ps[1].y = m_nYOffset + top;
						// 下右
						ps[2].x = m_nXOffset + right;
						ps[2].y = m_nYOffset + bottom;
						// 下左
						ps[3].x = m_nXOffset + left;
						ps[3].y = m_nYOffset + bottom;
						C->Polygon(ps, 3);
					} break;
				case 8: // 台形下
					{
						TPoint ps[4];
						// 左上
						ps[0].x = m_nXOffset + left;
						ps[0].y = m_nYOffset + top;
						// 右上
						ps[1].x = m_nXOffset + right;
						ps[1].y = m_nYOffset + top;
						// 下右
						ps[2].x = m_nXOffset + right -
							m_CardHeight[cardindex] / 4;
						ps[2].y = m_nYOffset + bottom;
						// 下左
						ps[3].x = m_nXOffset + left +
							m_CardHeight[cardindex] / 4;
						ps[3].y = m_nYOffset + bottom;
						C->Polygon(ps, 3);
					} break;
				case 9: // 小さい四角
					C->Rectangle(m_nXOffset + (left + right) / 2 - Space,
						m_nYOffset + (top + bottom) / 2 - Space,
						m_nXOffset + (left + right) / 2 + Space,
						m_nYOffset + (top + bottom) / 2 + Space);
					break;
				case 10: // 小さい円
					C->Ellipse(m_nXOffset + (left + right) / 2 - Space,
						m_nYOffset + (top + bottom) / 2 - Space,
						m_nXOffset + (left + right) / 2 + Space,
						m_nYOffset + (top + bottom) / 2 + Space);
					break;
				case 11: // 小さい三角（上向き）
					{
						TPoint ps[3];
						// 上
						ps[0].x = m_nXOffset + (left + right) / 2;
						ps[0].y = m_nYOffset + (top + bottom) / 2 - Space;
						// 下右
						ps[1].x = m_nXOffset + (left + right) / 2 +
							Space * (1.7320508 / 2);
						ps[1].y = m_nYOffset + (top + bottom) / 2 + Space / 2;
						// 下左
						ps[2].x = m_nXOffset + (left + right) / 2 -
							Space * (1.7320508 / 2);
						ps[2].y = m_nYOffset + (top + bottom) / 2 + Space / 2;
						C->Polygon(ps, 2);
					} break;
				case 12: // 小さい三角（下向き）
					{
						TPoint ps[3];
						// 下
						ps[0].x = m_nXOffset + (left + right) / 2;
						ps[0].y = m_nYOffset + (top + bottom) / 2 + Space;
						// 上右
						ps[1].x = m_nXOffset + (left + right) / 2 +
							Space * (1.7320508 / 2);
						ps[1].y = m_nYOffset + (top + bottom) / 2 - Space / 2;
						// 上左
						ps[2].x = m_nXOffset + (left + right) / 2 -
							Space * (1.7320508 / 2);
						ps[2].y = m_nYOffset + (top + bottom) / 2 - Space / 2;
						C->Polygon(ps, 2);
					} break;
				case 13: // Dia
					{
						TPoint ps[4];
						// 上
						ps[0].x = m_nXOffset + (left + right) / 2;
						ps[0].y = m_nYOffset + (top + bottom) / 2 - Space;
						// 右
						ps[1].x = m_nXOffset + (left + right) / 2 + Space;
						ps[1].y = m_nYOffset + (top + bottom) / 2;
						// 下
						ps[2].x = m_nXOffset + (left + right) / 2;
						ps[2].y = m_nYOffset + (top + bottom) / 2 + Space;
						// 左
						ps[3].x = m_nXOffset + (left + right) / 2 - Space;
						ps[3].y = m_nYOffset + (top + bottom) / 2;
						C->Polygon(ps, 3);
					} break;
				case 14: // Hex
					{
						TPoint ps[6];
						ps[0].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 6 * 1) * Space;
						ps[0].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 6 * 1) * Space;
						ps[1].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 6 * 3) * Space;
						ps[1].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 6 * 3) * Space;
						ps[2].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 6 * 5) * Space;
						ps[2].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 6 * 5) * Space;
						ps[3].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 6 * 7) * Space;
						ps[3].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 6 * 7) * Space;
						ps[4].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 6 * 9) * Space;
						ps[4].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 6 * 9) * Space;
						ps[5].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 6 * 11) * Space;
						ps[5].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 6 * 11) * Space;
						C->Polygon(ps, 5);
					} break;
				case 15: // ☆
					{
						int c = (Space * sin(M_PI * (18.0 / 360.0))) / sin
							(M_PI * (54.0 / 360.0));
						TPoint ps[10];
						ps[0].x = m_nXOffset + (left + right) / 2 +
							sin(0.0) * Space;
						ps[0].y = m_nYOffset + (top + bottom) / 2 -
							cos(0.0) * Space;
						ps[1].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 5 * 1) * c;
						ps[1].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 5 * 1) * c;
						ps[2].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 5 * 2) * Space;
						ps[2].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 5 * 2) * Space;
						ps[3].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 5 * 3) * c;
						ps[3].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 5 * 3) * c;
						ps[4].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 5 * 4) * Space;
						ps[4].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 5 * 4) * Space;
						ps[5].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 5 * 5) * c;
						ps[5].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 5 * 5) * c;
						ps[6].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 5 * 6) * Space;
						ps[6].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 5 * 6) * Space;
						ps[7].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 5 * 7) * c;
						ps[7].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 5 * 7) * c;
						ps[8].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 5 * 8) * Space;
						ps[8].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 5 * 8) * Space;
						ps[9].x = m_nXOffset + (left + right) / 2 +
							sin(M_PI / 5 * 9) * c;
						ps[9].y = m_nYOffset + (top + bottom) / 2 -
							cos(M_PI / 5 * 9) * c;
						C->Polygon(ps, 9);
					} break;
				default:
					C->RoundRect(m_nXOffset + left, m_nYOffset + top,
						m_nXOffset + right, m_nYOffset + bottom,
						(m_nFontHeight * 2) / 3, (m_nFontHeight * 2) / 3);
				}
				drawtimes--;
				if (SettingView.m_bCardGradation && !(option & 0x1)) {
					if (rgnexist) {
						::SelectClipRgn(C->Handle, RgnBak);
					}
					else {
						::SelectClipRgn(C->Handle, NULL);
					}
					::DeleteObject(RgnBak);
					::DeleteObject(MyRgn);
				}
			}

			// 固定ピン表示
			if (Card->m_bFixed) {
				P->Style = psSolid;
				P->Width = m_fFontZoom;
				B->Style = bsSolid;
				if ((option & 0x1) == 0) {
					// 影でない
					P->Color = HalfColor(0x0000ff, B->Color, 0.5f);
					B->Color = TColor(0x0000ff);
				}
				C->Ellipse(m_nXOffset + left + Space / 2,
					m_nYOffset + top + Space / 2, m_nXOffset + left + Space / 4,
					m_nYOffset + top + Space / 4);
				C->Ellipse(m_nXOffset + right - Space / 2,
					m_nYOffset + top + Space / 2,
					m_nXOffset + right - Space / 4,
					m_nYOffset + top + Space / 4);
				C->Ellipse(m_nXOffset + left + Space / 2,
					m_nYOffset + bottom - Space / 2,
					m_nXOffset + left + Space / 4,
					m_nYOffset + bottom - Space / 4);
				C->Ellipse(m_nXOffset + right - Space / 2,
					m_nYOffset + bottom - Space / 2,
					m_nXOffset + right - Space / 4,
					m_nYOffset + bottom - Space / 4);
			}

			// 絵の描画
			if (Drawing) {
				if ((option & 0x1) == 0) {
					HRGN MyRgn, RgnBak;
					MyRgn = ::CreateRectRgn(m_nXOffset + left + Space / 2,
						m_nYOffset + top + Space / 2,
						m_nXOffset + left + Space / 2 +
						(SettingView.m_nImageLimitation * imagex) / 100,
						m_nYOffset + top + Space / 2 +
						(SettingView.m_nImageLimitation * imagex) / 100 * 0.75);
					RgnBak = ::CreateRectRgn(0, 0, 0, 0);
					int exist = ::GetClipRgn(C->Handle, RgnBak);
					if (exist) {
						HRGN CmbRgn;
						CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
						::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
						::SelectClipRgn(C->Handle, CmbRgn);
						::DeleteObject(CmbRgn);
					}
					else {
						::SelectClipRgn(C->Handle, MyRgn);
					}

					TRect R = Rect(m_nXOffset + left + Space / 2,
						m_nYOffset + top + Space / 2 -
						(SettingView.m_nImageLimitation * imagex) / 100 * 0.125,
						m_nXOffset + left + Space / 2 +
						(SettingView.m_nImageLimitation * imagex) / 100,
						m_nYOffset + top + Space / 2 +
						(SettingView.m_nImageLimitation * imagex)
						/ 100 * 0.875);

					Card->m_Drawing->Draw(C, R);

					if (exist) {
						::SelectClipRgn(C->Handle, RgnBak);
					}
					else {
						::SelectClipRgn(C->Handle, NULL);
					}
					::DeleteObject(RgnBak);
					::DeleteObject(MyRgn);
				} /* else{
				 //影
				 B->Style = bsSolid;
				 C->FillRect(
				 Rect(
				 m_nXOffset + left + Space / 2,
				 m_nYOffset + top + Space / 2,
				 m_nXOffset + left + Space / 2 + SettingView.m_nImageLimitation,
				 m_nYOffset + top + Space / 2 + SettingView.m_nImageLimitation * 0.75
				 ));
				 }
				 */
			}

			// ビデオ描画
			/*
			 if (Video){
			 if ((option & 0x1) == 0){
			 //影ではない
			 Graphics::TBitmap *BMP = new Graphics::TBitmap();
			 Video->m_Video->GetBMP(BMP);
			 if (imagex == 100){
			 C->Draw(
			 m_nXOffset + left + Space / 2,
			 m_nYOffset + top + Space / 2,
			 BMP
			 );
			 }else{
			 C->StretchDraw(
			 Rect(
			 m_nXOffset + left + Space / 2,
			 m_nYOffset + top + Space / 2,
			 m_nXOffset + left + Space / 2 + (imagex * vi->bmiHeader.biWidth) / 100,
			 m_nYOffset + top + Space / 2 + (imagex * vi->bmiHeader.biHeight) / 100
			 ),
			 BMP
			 );
			 }
			 delete BMP;
			 }else{
			 //影
			 B->Style = bsSolid;
			 C->FillRect(
			 Rect(
			 m_nXOffset + left + Space / 2,
			 m_nYOffset + top + Space / 2,
			 m_nXOffset + left + Space / 2 + (imagex * vi->bmiHeader.biWidth) / 100,
			 m_nYOffset + top + Space / 2 + (imagex * vi->bmiHeader.biHeight) / 100
			 ));
			 }
			 }
			 */

			// イメージ描画
			if (Image) {
				if ((option & 0x1) == 0) {
					if (imagex == 100) {
						C->Draw(m_nXOffset + left + Space / 2,
							m_nYOffset + top + Space / 2, Image->m_Image);
					}
					else {
						C->StretchDraw(Rect(m_nXOffset + left + Space / 2,
							m_nYOffset + top + Space / 2,
							m_nXOffset + left + Space / 2 +
							(imagex * Image->m_Image->Width) / 100,
							m_nYOffset + top + Space / 2 +
							(imagex * Image->m_Image->Height) / 100),
							Image->m_Image);
					}
				}
				else {
					// 影
					B->Style = bsSolid;
					C->FillRect(Rect(m_nXOffset + left + Space / 2,
						m_nYOffset + top + Space / 2,
						m_nXOffset + left + Space / 2 +
						(imagex * Image->m_Image->Width) / 100,
						m_nYOffset + top + Space / 2 +
						(imagex * Image->m_Image->Height) / 100));
				}
			}

			// 文字描画
			B->Style = bsClear;
			TColor fcbak = F->Color; {
				float X = m_nXOffset + left + Space / 2;
				float Y = m_nYOffset + bottom - Space / 2 - lineheight * lines -
					tickerheight;
				if (m_CardShape[cardindex] >= 9 && m_CardShape[cardindex] <= 15)
				{
					/*
					 //影付き
					 {
					 int d;
					 d = Space / 16;
					 if (d < 1){
					 d = 1;
					 }
					 TColor bkcolor = F->Color;
					 F->Color = (TColor)SettingView.m_nBackgroundColor;
					 for (int i = 0 ; i < Title->Count ; i++){
					 C->TextOut(X + d, Y + i * lineheight + d, Title->Strings[i]);
					 }
					 F->Color = bkcolor;
					 }
					 */
					// *
					// 縁取り
					BeginPath(C->Handle);
					for (int i = 0; i < Title->Count; i++) {
						C->TextOut(X, Y + i * lineheight, Title->Strings[i]);
					}
					EndPath(C->Handle);
					P->Color = F->Color;
					P->Width = (abs(F->Height) / 8);
					if (P->Width < 4) {
						P->Width = 4;
					}
					StrokeAndFillPath(C->Handle);
					if ((option & 0x1) == 0) {
						// 影でない
						F->Color = (TColor)SettingView.m_nBackgroundColor;
					}
					// */
					/*
					 {
					 int d;
					 d = Space / 16;
					 if (d < 1){
					 d = 1;
					 }
					 for (int i = 0 ; i < Title->Count ; i++){
					 C->TextOut(X - d, Y + i * lineheight, Title->Strings[i]);
					 C->TextOut(X, Y + i * lineheight - d, Title->Strings[i]);
					 C->TextOut(X + d, Y + i * lineheight, Title->Strings[i]);
					 C->TextOut(X, Y + i * lineheight + d, Title->Strings[i]);
					 }
					 d = Space / 22.6272;
					 if (d < 1){
					 d = 1;
					 }
					 for (int i = 0 ; i < Title->Count ; i++){
					 C->TextOut(X - d, Y + i * lineheight - d, Title->Strings[i]);
					 C->TextOut(X + d, Y + i * lineheight - d, Title->Strings[i]);
					 C->TextOut(X + d, Y + i * lineheight + d, Title->Strings[i]);
					 C->TextOut(X - d, Y + i * lineheight + d, Title->Strings[i]);
					 }
					 F->Color = (TColor)SettingView.m_nBackgroundColor;
					 }
					 */
					for (int i = 0; i < Title->Count; i++) {
						C->TextOut(X, Y + i * lineheight, Title->Strings[i]);
					}
				}
				else if (m_CardShape[cardindex] >= 3 || ((option & 0x1) == 0)) {
					for (int i = 0; i < Title->Count; i++) {
						C->TextOut(X, Y + i * lineheight, Title->Strings[i]);
						// WideString W = Title->Strings[i];
						// TextOutW(C->Handle, X, Y + i * lineheight, W, W.Length());
					}
				}
			} F->Color = fcbak;

			// Ticker表示
			if (SettingView.m_bTickerVisible && tickerheight > 0 && ticker) {
				int FontHeightBak = F->Height;

				F->Height *= 0.8f;
				float X = m_nXOffset + right - Space / 2;
				float Y = m_nYOffset + bottom - Space / 2 - tickerheight;
				UnicodeString S;
				for (int il = 0; il < SettingView.m_nTickerLines && il <
					Card->m_Lines->Count; il++) {
					S += Card->m_Lines->Strings[il];
					S += " ";
				}
				int tickerwidth = C->TextWidth(S);
				if (Card->m_fTickerPos * m_nFontHeight > tickerwidth +
					m_CardWidth[cardindex] - Space) {
					Card->m_fTickerPos = 0.0f;
				}
				Card->m_fTickerPos += m_fTickerSpeed;

				/*
				 HRGN MyRgn, RgnBak;
				 MyRgn = ::CreateRectRgn(m_nXOffset + left + Space / 2, Y, m_nXOffset + right - Space / 2, Y + tickerheight);
				 RgnBak = ::CreateRectRgn(0, 0, 0, 0);
				 int exist = ::GetClipRgn(C->Handle, RgnBak);
				 if (exist){
				 HRGN CmbRgn;
				 CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
				 ::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
				 ::SelectClipRgn(C->Handle, CmbRgn);
				 ::DeleteObject(CmbRgn);
				 }else{
				 ::SelectClipRgn(C->Handle, MyRgn);
				 }
				 C->TextOut(X - Card->m_fTickerPos * m_nFontHeight, Y, S);
				 if (exist){
				 ::SelectClipRgn(C->Handle, RgnBak);
				 }else{
				 ::SelectClipRgn(C->Handle, NULL);
				 }
				 ::DeleteObject(RgnBak);
				 ::DeleteObject(MyRgn);
				 // */

				if (m_CardShape[cardindex] >= 3 || ((option & 0x1) == 0)) {
					C->TextRect(Rect(m_nXOffset + left + Space / 2, Y,
						m_nXOffset + right - Space / 2, Y + tickerheight),
						X - Card->m_fTickerPos * m_nFontHeight, Y, S);
				}
				F->Height = FontHeightBak;
			}

			// スコア描画
			if (SettingView.m_bScore) {
				if (m_CardShape[cardindex] >= 3 || ((option & 0x1) == 0)) {
					if (Card) {
						float Y = m_nYOffset + bottom - Space / 2;
						float score = 0.0f;
						if (Card->m_fScore > 0.0f && m_fMaxScore -
							m_fMinScore > 0.0f) {
							score = (log(Card->m_fScore) - m_fMinScore) /
								(m_fMaxScore - m_fMinScore) * 5.0f;
						}
						B->Style = bsSolid;
						if ((option & 0x1) == 0) {
							// 影でない
							B->Color = clRed;
						}
						else {
							B->Color = P->Color;
						}
						C->FillRect(Rect(m_nXOffset + left + Space / 2, Y,
							m_nXOffset + left + Space / 2 + Space * score,
							Y + Space / 4));
					}
				}
			}

			// Foldボタン描画
			if (Card)
				if ((option & 0x1) == 0 && TreeMode() && Card->m_bHasChild) {
					P->Style = psSolid;
					B->Style = bsSolid;
					P->Color = PColor;
					P->Width = m_fFontZoom;
					B->Color = HalfColor(BColor, 0xffffff, 0.5f);
					int fleft = m_nXOffset + right - m_nFontHeight;
					int ftop = m_nYOffset + bottom - m_nFontHeight / 2;
					int fsize = (m_nFontHeight * 2) / 3;
					fsize = fsize / 2 * 2 + 1;
					if (fsize < 13) {
						fsize = 13;
					}
					int fright = m_nXOffset + right - m_nFontHeight + fsize;
					int fbottom = m_nYOffset + bottom -
						m_nFontHeight / 2 + fsize;
					C->Rectangle(fleft, ftop, fright, fbottom);
					P->Color = TColor(m_nFGColor);
					C->MoveTo(fleft + fsize / 4, (ftop + fbottom) / 2);
					C->LineTo(fright - fsize / 4, (ftop + fbottom) / 2);
					if (Card->m_bFold) {
						C->MoveTo((fleft + fright) / 2, ftop + fsize / 4);
						C->LineTo((fleft + fright) / 2, fbottom - fsize / 4);
					}
				}
		}
		F->Height = OrgFontHeight;
	}

	delete Title;

	// FocusCursor
	if (Card && (option & 0x1) == 0) {
		// カードがあり、影でない
		if (Card->m_nSelected && Card->m_nID != m_nLastTarget && Card->m_nID !=
			m_nTargetCard) {
			// 選択アニメーション中で、選択中カードなどではない
			int pos = m_nFocusCursorPos;
			float br = 0.0f;
			if (pos % 100 >= 50 && Card->m_nSelected == 100) {
				br = 0.33f;
			}

			// 場所、サイズ決定
			float ratio = Card->m_nSelected * 0.01f;
			float fx, fy, fw, fh;
			float addsize; // 0～最大1になる（大きくなるほど薄く）
			fx = m_CardX[cardindex];
			fy = m_CardY[cardindex];
			addsize = 1.0f - ratio;
			fw = m_CardWidth[cardindex];
			fh = m_CardHeight[cardindex];

			fw += m_nFontHeight * (addsize * 10.0f + 1);
			fh += m_nFontHeight * (addsize * 10.0f + 1);

			P->Color = HalfColor(m_nFGColor, m_nBGColor, addsize + br);

			DrawFocusCursor(C, fx, fy, fw, fh);
		}
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawFocusCursor(TCanvas *C, float fx, float fy, float fw,
	float fh) {
	TPen *P = C->Pen;

	int space = (m_nFontHeight * 2) / 3;
	TColor color = P->Color;

	for (int i = 0; i < 2; i++) {
		if (i == 0) {
			P->Color = TColor(m_nBGColor);
			P->Width = m_fFontZoom * 5;
		}
		else {
			P->Color = color;
			P->Width = m_fFontZoom * 3;
		}
		// 左上
		C->MoveTo(m_nXOffset + fx - fw * 0.5f + space,
			m_nYOffset + fy - fh * 0.5f);
		C->LineTo(m_nXOffset + fx - fw * 0.5f, m_nYOffset + fy - fh * 0.5f);
		C->LineTo(m_nXOffset + fx - fw * 0.5f,
			m_nYOffset + fy - fh * 0.5f + space);

		// 右上
		C->MoveTo(m_nXOffset + fx + fw * 0.5f - space,
			m_nYOffset + fy - fh * 0.5f);
		C->LineTo(m_nXOffset + fx + fw * 0.5f, m_nYOffset + fy - fh * 0.5f);
		C->LineTo(m_nXOffset + fx + fw * 0.5f,
			m_nYOffset + fy - fh * 0.5f + space);

		// 左下
		C->MoveTo(m_nXOffset + fx - fw * 0.5f + space,
			m_nYOffset + fy + fh * 0.5f);
		C->LineTo(m_nXOffset + fx - fw * 0.5f, m_nYOffset + fy + fh * 0.5f);
		C->LineTo(m_nXOffset + fx - fw * 0.5f,
			m_nYOffset + fy + fh * 0.5f - space);

		// 右下
		C->MoveTo(m_nXOffset + fx + fw * 0.5f - space,
			m_nYOffset + fy + fh * 0.5f);
		C->LineTo(m_nXOffset + fx + fw * 0.5f, m_nYOffset + fy + fh * 0.5f);
		C->LineTo(m_nXOffset + fx + fw * 0.5f,
			m_nYOffset + fy + fh * 0.5f - space);
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawPatternLine(TCanvas *C, int Pattern, int X1, int Y1, int X2,
	int Y2, int penwidth) {
	// 線の長さ
	float xd = X2 - X1;
	float yd = Y2 - Y1;
	float len = sqrt(xd * xd + yd * yd);
	if (len == 0.0f) {
		return;
	}
	float xstep = xd / len;
	float ystep = yd / len;

	int Pat[16];
	int PatNum;
	switch (Pattern) {
	case 0: // 3-3-点線
		Pat[0] = 3;
		Pat[1] = 3;
		PatNum = 2;
		break;
	case 1: // 9-3-破線
		Pat[0] = 9;
		Pat[1] = 3;
		PatNum = 2;
		break;
	case 2: // 9-3-3-破線
		Pat[0] = 9;
		Pat[1] = 3;
		Pat[2] = 3;
		Pat[3] = 3;
		PatNum = 4;
		break;
	case 3: // 9-3-3-破線
		Pat[0] = 9;
		Pat[1] = 3;
		Pat[2] = 3;
		Pat[3] = 3;
		Pat[4] = 3;
		Pat[5] = 3;
		PatNum = 6;
		break;
	}

	for (int i = 0; i < PatNum; i++) {
		Pat[i] *= penwidth;
	}

	float pos = 0.0f;
	int ppos = 0;
	while (pos < len - 1) {
		C->MoveTo(X1 + xstep * pos, Y1 + ystep * pos);
		pos += Pat[ppos++];
		if (pos > len) {
			pos = len;
		}
		C->LineTo(X1 + xstep * pos, Y1 + ystep * pos);
		pos += Pat[ppos++];
		if (ppos >= PatNum) {
			ppos = 0;
		}
	}
}

void CurvePosToXY(float pos, float &x0, float &y0) {
	/*
	 x0 = sin(pos) * 0.5;
	 if (pos > M_PI * 0.5) {
	 x0 = 1.0f - x0;
	 }
	 y0 = (sin(pos - M_PI * 0.5) + 1.0) * 0.5;
	 */
	x0 = sqrt(fabs(sin(pos))) * 0.5;
	if (pos > M_PI * 0.5) {
		x0 = 1.0f - x0;
	}
	y0 = (sin(pos - M_PI * 0.5) + 1.0) * 0.5;
}

void TFo_Main::DrawCurvedLine(TCanvas *C, int Pattern, int X1, int Y1, int X2,
	int Y2, int penwidth, int direction) {
	// 曲線描画
	// 線の長さ
	float xd = X2 - X1;
	float yd = Y2 - Y1;
	float len = sqrt(xd * xd + yd * yd) * sqrt(2);
	if (len == 0.0f) {
		return;
	}
	float radstep = M_PI / len;

	int Pat[16];
	int PatNum;
	switch (Pattern) {
	case 0: // 3-3-点線
		Pat[0] = 3;
		Pat[1] = 3;
		PatNum = 2;
		break;
	case 1: // 9-3-破線
		Pat[0] = 9;
		Pat[1] = 3;
		PatNum = 2;
		break;
	case 2: // 9-3-3-破線
		Pat[0] = 9;
		Pat[1] = 3;
		Pat[2] = 3;
		Pat[3] = 3;
		PatNum = 4;
		break;
	case 3: // 9-3-3-破線
		Pat[0] = 9;
		Pat[1] = 3;
		Pat[2] = 3;
		Pat[3] = 3;
		Pat[4] = 3;
		Pat[5] = 3;
		PatNum = 6;
		break;
	default:
		Pat[0] = 2;
		PatNum = 1;
	}

	TList *Ps = NULL;
	if (Pattern >= 4) {
		Ps = new TList();
	}

	for (int i = 0; i < PatNum; i++) {
		Pat[i] *= penwidth;
	}

	float pos = 0.0f; // < PI
	int ppos = 0;
	while (pos < M_PI) {
		float lastx, lasty; {
			float x0, y0;
			CurvePosToXY(pos, x0, y0);

			lastx = xd * x0;
			lasty = yd * y0;
			if (Pattern < 4) {
				C->MoveTo(X1 + lastx, Y1 + lasty);
			}
			else {
				Ps->Add((void*)*(int*)&lastx);
				Ps->Add((void*)*(int*)&lasty);
			}
		}

		while (pos < M_PI) {
			pos += radstep;
			float x0, y0;
			CurvePosToXY(pos, x0, y0);

			if ((lastx - xd * x0) * (lastx - xd * x0) + (lasty - yd * y0) *
				(lasty - yd * y0) > Pat[ppos] * Pat[ppos]) {
				break;
			}
		}
		ppos++;
		// pos += Pat[ppos++] * radstep;
		if (pos > M_PI) {
			pos = M_PI;
		} {
			float x0, y0;
			CurvePosToXY(pos, x0, y0);

			lastx = xd * x0;
			lasty = yd * y0;
			if (Pattern < 4) {
				C->LineTo(X1 + lastx, Y1 + lasty);
			}
			else {
				Ps->Add((void*)*(int*)&lastx);
				Ps->Add((void*)*(int*)&lasty);
			}
		}
		if (ppos < PatNum) {
			while (pos < M_PI) {
				pos += radstep;
				float x0 = sin(pos) * 0.5;
				if (pos > M_PI * 0.5) {
					x0 = 1.0f - x0;
				}
				float y0 = (sin(pos - M_PI * 0.5) + 1.0) * 0.5;
				if ((lastx - xd * x0) * (lastx - xd * x0) + (lasty - yd * y0) *
					(lasty - yd * y0) > Pat[ppos] * Pat[ppos]) {
					break;
				}
			}
			ppos++;
			// pos += Pat[ppos++] * radstep;
		}
		if (ppos >= PatNum) {
			ppos = 0;
		}
	}

	if (Ps) {
		int count = Ps->Count / 2;
		TPoint *ps = new TPoint[count * 2];
		TPen *P = C->Pen;
		TBrush *B = C->Brush;
		P->Style = psClear;
		B->Style = bsSolid;
		B->Color = P->Color;
		for (int i2 = 0; i2 < 2; i2++) {
			float rad = 0.0f;
			for (int i = 0; i < count; i++) {
				int l;
				l = (int)Ps->Items[i * 2];
				float X1_ = *(float*)&l;
				l = (int)Ps->Items[i * 2 + 1];
				float Y1_ = *(float*)&l;
				if (i < count - 1) {
					l = (int)Ps->Items[(i + 1) * 2];
					float X2_ = *(float*)&l;
					l = (int)Ps->Items[(i + 1) * 2 + 1];
					float Y2_ = *(float*)&l;
					if (Y2_ != Y1_ || X2_ != X1_) {
						rad = atan2(Y2_ - Y1_, X2_ - X1_);
					}
				}
				float size = m_nFontHeight / 6.0f;
				if (direction | 0x1) {
					size = ((count - i) * m_nFontHeight) / (3.0f * count) -
						i2 * 0.5;
				}
				if (direction | 0x2) {
					size += P->Width * 0.33f;
				}
				if (i2) {
					ps[i].x = X1 + X1_ - cos(rad + M_PI * 0.5) * size;
					ps[i].y = Y1 + Y1_ - sin(rad + M_PI * 0.5) * size;
				}
				else {
					ps[i].x = X1 + X1_ - cos(rad - M_PI * 0.5) * size;
					ps[i].y = Y1 + Y1_ - sin(rad - M_PI * 0.5) * size;
				}
				ps[count * 2 - 1 - i].x = X1 + X1_;
				ps[count * 2 - 1 - i].y = Y1 + Y1_;
			}
			C->Polygon(ps, count * 2 - 1);
			B->Color = HalfColor(B->Color, 0xffffff, 0.25f);
		}
		delete[]ps;
		delete Ps;
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawLink2(TCanvas *C, TLink *Link, int X1, int Y1, int X2,
	int Y2, TColor HMColor, int option) {
	// リンク描画

	if (!SettingView.m_bLinkVisible) {
		// 描画しない
		return;
	}

	TPen *P = C->Pen;
	TBrush *B = C->Brush;
	int penwidth = P->Width;

	int shape = SettingView.m_nLinkDefaultShape;
	if (Link) {
		shape = Link->m_nShape;
	}

	float rad = 0.0;
	if (Y2 != Y1 || X2 != X1) {
		if (shape < 6) {
			// 直線
			rad = atan2(Y2 - Y1, X2 - X1);
		}
		else {
			// 曲線
			rad = atan2(pow(abs(Y2 - Y1), 1.73) * (-1 + (Y2 > Y1) * 2),
			X2 - X1);
		}
	}

	bool barrow;
	if (Link) {
		barrow = Link->m_bDirection;
	}
	else {
		barrow = true;
	}
	barrow &= SettingView.m_bLinkDirectionVisible;

	switch (shape) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		// 通常矢印
		{
			P->Style = psSolid;
			// P->Style = psDashDot;

			if (option & 0x2) {
				// 縁取り
				TColor pcbak = P->Color;
				int pwbak = P->Width;
				P->Color = TColor(HMColor); // InverseColor(pcbak);
				P->Width = C->Pen->Width * 3;
				// int FontHeightBak = m_nFontHeight;

				switch (shape) {
				case 1:
				case 2:
				case 3:
				case 4:
					DrawPatternLine(C, shape - 1, X1, Y1, X2, Y2, penwidth);
					break;
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
					DrawCurvedLine(C, shape - 7, X1, Y1, X2, Y2, penwidth,
						barrow | 0x2);
					break;
				default:
					C->MoveTo(X1, Y1);
					C->LineTo(X2, Y2);
					break;
				}

				// リンク方向
				if (barrow && shape < 11) {
					float mX = (X1 + X2) / 2;
					float mY = (Y1 + Y2) / 2;
					if (Y2 != Y1 || X2 != X1) {
						C->MoveTo(mX - cos(rad + 0.1 * 2 * M_PI) *
							(m_nFontHeight * 2) / 3,
							mY - sin(rad + 0.1 * 2 * M_PI) *
							(m_nFontHeight * 2) / 3);
						C->LineTo(mX, mY);
						C->MoveTo(mX - cos(rad - 0.1 * 2 * M_PI) *
							(m_nFontHeight * 2) / 3,
							mY - sin(rad - 0.1 * 2 * M_PI) *
							(m_nFontHeight * 2) / 3);
						C->LineTo(mX, mY);
					}
				}
				P->Color = pcbak;
				P->Width = pwbak;
			}

			switch (shape) {
			case 1:
			case 2:
			case 3:
			case 4:
				DrawPatternLine(C, shape - 1, X1, Y1, X2, Y2, penwidth);
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				DrawCurvedLine(C, shape - 7, X1, Y1, X2, Y2, penwidth, barrow);
				break;
			default:
				C->MoveTo(X1, Y1);
				C->LineTo(X2, Y2);
				break;
			}

			// リンク方向
			if (barrow && shape < 11) {
				float mX = (X1 + X2) / 2;
				float mY = (Y1 + Y2) / 2;
				if (Y2 != Y1 || X2 != X1) {
					C->MoveTo(mX - cos(rad + 0.1 * 2 * M_PI) *
						(m_nFontHeight * 2) / 3,
						mY - sin(rad + 0.1 * 2 * M_PI) *
						(m_nFontHeight * 2) / 3);
					C->LineTo(mX, mY);
					C->MoveTo(mX - cos(rad - 0.1 * 2 * M_PI) *
						(m_nFontHeight * 2) / 3,
						mY - sin(rad - 0.1 * 2 * M_PI) *
						(m_nFontHeight * 2) / 3);
					C->LineTo(mX, mY);
				}
			}
		} break;
	case 5:
		// くさび型
		{
			if (option & 0x2) {
				// 縁取り
				TColor pcbak = P->Color;
				P->Color = TColor(HMColor); // InverseColor(pcbak);
				P->Style = psSolid;

				if (barrow) {
					TPoint ps[3];
					ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 3;
					ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 3;
					ps[1].x = X1 - cos(rad - M_PI * 0.5) * m_nFontHeight / 3;
					ps[1].y = Y1 - sin(rad - M_PI * 0.5) * m_nFontHeight / 3;

					ps[2].x = X2;
					ps[2].y = Y2;
					C->Polygon(ps, 2);
				}
				else {
					TPoint ps[4];
					ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[3].x = X2 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[3].y = Y2 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;

					ps[1].x = X1 - cos(rad - M_PI * 0.5) * m_nFontHeight / 6;
					ps[1].y = Y1 - sin(rad - M_PI * 0.5) * m_nFontHeight / 6;

					ps[2].x = X2 - cos(rad - M_PI * 0.5) * m_nFontHeight / 6;
					ps[2].y = Y2 - sin(rad - M_PI * 0.5) * m_nFontHeight / 6;
					C->Polygon(ps, 3);
				}

				P->Color = pcbak;
			}

			B->Color = P->Color;
			B->Style = bsSolid;
			P->Style = psClear;
			if (barrow) {
				TPoint ps[3];
				if (SettingView.m_bCardGradation) {
					ps[0].x = X1;
					ps[0].y = Y1;
				}
				else {
					ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 3;
					ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 3;
				}
				ps[1].x = X1 - cos(rad - M_PI * 0.5) * m_nFontHeight / 3;
				ps[1].y = Y1 - sin(rad - M_PI * 0.5) * m_nFontHeight / 3;

				ps[2].x = X2;
				ps[2].y = Y2;
				C->Polygon(ps, 2);
			}
			else {
				TPoint ps[4];
				if (SettingView.m_bCardGradation) {
					ps[0].x = X1;
					ps[0].y = Y1;
					ps[3].x = X2;
					ps[3].y = Y2;
				}
				else {
					ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[3].x = X2 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[3].y = Y2 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
				}

				ps[1].x = X1 - cos(rad - M_PI * 0.5) * m_nFontHeight / 6;
				ps[1].y = Y1 - sin(rad - M_PI * 0.5) * m_nFontHeight / 6;

				ps[2].x = X2 - cos(rad - M_PI * 0.5) * m_nFontHeight / 6;
				ps[2].y = Y2 - sin(rad - M_PI * 0.5) * m_nFontHeight / 6;
				C->Polygon(ps, 3);
			}

			if (SettingView.m_bCardGradation) {
				B->Color = HalfColor(P->Color, 0xffffff, 0.25f);
				if (barrow) {
					TPoint ps[3];
					ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 3;
					ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 3;
					ps[1].x = X1;
					ps[1].y = Y1;

					ps[2].x = X2;
					ps[2].y = Y2;
					C->Polygon(ps, 2);
				}
				else {
					TPoint ps[4];
					ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[1].x = X1;
					ps[1].y = Y1;

					ps[3].x = X2 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[3].y = Y2 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
					ps[2].x = X2;
					ps[2].y = Y2;
					C->Polygon(ps, 3);
				}
			}
		}
	}

	// リンク名
	if (SettingView.m_bLinkNameVisible && Link) {
		if (Link->m_Name != "") {
			TBrush *B = C->Brush;
			B->Style = bsClear;

			float mX = (X1 + X2) / 2;
			float mY = (Y1 + Y2) / 2;

			UnicodeString S = Link->m_Name;
			if (mX > m_nBrowserWidth / 2) {
				C->TextOut(mX + m_nFontHeight / 2, mY - m_nFontHeight / 2, S);
			}
			else {
				C->TextOut(mX - m_nFontHeight / 2 - C->TextWidth(S),
					mY - m_nFontHeight / 2, S);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawLink(TCanvas *C, TLink *Link, int card1index, int card2index,
	TColor HMColor, int option) {
	// リンク描画

	// リンク線
	float X1 = m_CardX[card1index];
	float Y1 = m_CardY[card1index];
	float X2 = m_CardX[card2index];
	float Y2 = m_CardY[card2index];

	if (!((X1 < 0 && X2 < 0) || (Y1 < 0 && Y2 < 0) || (X1 >
		m_nBrowserWidth && X2 > m_nBrowserWidth) || (Y1 >
		m_nBrowserHeight && Y2 > m_nBrowserHeight))) {
		X1 += m_nXOffset;
		Y1 += m_nYOffset;
		X2 += m_nXOffset;
		Y2 += m_nYOffset;

		DrawLink2(C, Link, X1, Y1, X2, Y2, HMColor, option);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_BrowserPaint(TObject *Sender) {
	if (PB_Browser->Tag) {
		return;
	}

	HRGN RgnBak = ::CreateRectRgn(0, 0, 0, 0);
	int rgnexist;

	bool err = false; // AAによるエラー

	// サイズ決定
	if (Fo_FullScreen->Visible) {
		if (Fo_FullScreen->Width > 0 && Fo_FullScreen->Height > 0) {
			m_nBrowserWidth = Fo_FullScreen->Width;
			m_nBrowserHeight = Fo_FullScreen->Height;
		}
		rgnexist = ::GetClipRgn(Fo_FullScreen->Canvas->Handle, RgnBak);
	}
	else {
		if (PB_Browser->Width > 0 && PB_Browser->Height > 0) {
			m_nBrowserWidth = PB_Browser->Width;
			m_nBrowserHeight = PB_Browser->Height;
		}
		rgnexist = ::GetClipRgn(PB_Browser->Canvas->Handle, RgnBak);
	}
	m_nXOffset = 0;
	m_nYOffset = 0;

	// バックグラウンドバッファ作成
	Graphics::TBitmap *BMP = new Graphics::TBitmap();
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

		m_nBrowserWidth = m_nBrowserWidth << countshift;
		m_nBrowserHeight = m_nBrowserHeight << countshift;

		// 数倍のサイズのビットマップ作成
		Graphics::TBitmap *BMP2 = new Graphics::TBitmap();
		try {
			BMP2->Width = m_nBrowserWidth << countshift;
			BMP2->Height = m_nBrowserHeight << countshift;
			BMP2->PixelFormat = pf32bit;
		}
		catch (...) {
			SettingView.m_bAntiAlias = false;
			ShowMessage(MLText.AANotEnoughMemory);
			Redraw();
			err = true;
		}

		if (!err) {
			// BMPに描画
			m_fFontZoom = SettingView.m_nAntiAliasX;
			PaintSub(BMP2->Canvas);
			/*
			 Graphics::TBitmap *BMP3 = new Graphics::TBitmap();
			 BMP3->Width = BMP2->Width;
			 BMP3->Height = BMP2->Height;
			 BMP3->PixelFormat = pf32bit;
			 BMP3->Canvas->Draw(0, 0, BMP2);
			 */
			// アンチエイリアシング
			int **P2 = new int*[BMP2->Height];
			for (int iy = 0; iy < BMP2->Height; iy++) {
				P2[iy] = (int*)BMP2->ScanLine[iy];
			}
			for (int iy = 0; iy < BMP->Height; iy++) {
				int *PW = (int*)BMP->ScanLine[iy];
				for (int ix = 0; ix < BMP->Width; ix++) {
					int r = 0, g = 0, b = 0;
					for (int iy2 = 0; iy2 < count; iy2++) {
						int *PR = P2[(iy << countshift) + iy2] +
							(ix << countshift);
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
			delete[]P2;

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
			m_nOVWidth /= count;
			m_nOVXOffset /= count;
			m_nOVHeight /= count;
			m_nOVYOffset /= count;
			// delete BMP3;
			m_fFontZoom = 1.0f;
		}

		delete BMP2;
	}
	else {
		BMP->Width = m_nBrowserWidth;
		BMP->Height = m_nBrowserHeight;

		if (rgnexist) {
			::SelectClipRgn(BMP->Canvas->Handle, RgnBak);
		}
		// BMPに描画
		PaintSub(BMP->Canvas);
	}

	// BMPをフォアグラウンドにコピー
	if (!err) {
		if (Fo_FullScreen->Visible) {
			Fo_FullScreen->Canvas->Draw(0, 0, BMP);
		}
		else {
			PB_Browser->Canvas->Draw(0, 0, BMP);
		}
	}

	// BMP破棄
	delete BMP;

	::DeleteObject(RgnBak);
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawLabelCircleRect(TCanvas *C, bool drawtoporder,
	bool drawothers) {
	TBrush *B = C->Brush;
	TPen *P = C->Pen;
	TFont *F = C->Font;
	if (m_Document->m_Labels[0]->Count >
		0 && (SettingView.m_bLabelCircleVisible ||
		SettingView.m_bLabelRectangleVisible)) {
		// 各ラベルのDateを指定
		for (int il = 0; il < m_Document->m_Labels[0]->Count; il++) {
			TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
			Label->m_fTouched = 0.0;
		}
		// カードループ
		for (int i = 0; i < m_Document->m_Cards->Count; i++)
			if (m_CardVisible[i]) {
				TCard *Card = m_Document->GetCardByIndex(i);
				// カードのラベルループ
				for (int il = 0; il < Card->m_Labels->Count; il++) {
					TCardLabel *Label =
						m_Document->GetLabelByIndex(0,
						Card->m_Labels->GetLabel(il) - 1);
					if (Label->m_fTouched < Card->m_fViewed) {
						Label->m_fTouched = Card->m_fViewed;
					}
				}
			}
		// 表示順
		m_Document->RefreshDateOrder_Label();

		int *draworder = new int[m_Document->m_Labels[0]->Count];
		memset(draworder, 0, sizeof(int) * m_Document->m_Labels[0]->Count);
		for (int i = 0; i < m_Document->m_Labels[0]->Count; i++) {
			int vo = m_Document->GetLabelByIndex(0, i)->m_nTouchedOrder;
			draworder[vo] = i;
		}

		bool first = true;
		if (SettingView.m_bLabelCircleVisible) {
			for (int i = 0; i < m_Document->m_Labels[0]->Count; i++) {
				int il = draworder[i];
				TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
				if (Label->m_bEnable && !Label->m_bFold) {
					// 有効かつFoldされていないラベル

					if ((first && drawtoporder) || (!first && drawothers)) {
						// 平均
						float Mean[2], StDev[2], Cov = 0.0f;
						int count = 0;
						for (int i = 0; i < 2; i++) {
							Mean[i] = 0.0f;
							StDev[i] = 0.0f;
						}

						for (int i = 0; i < m_Document->m_Cards->Count; i++)
							if (m_CardVisible[i]) {
								TCard *Card = m_Document->GetCardByIndex(i);
								if (Card->m_Labels->Contain(il + 1)) {
									Mean[0] += m_CardX[i];
									Mean[1] += m_CardY[i];
									count++;
								}
							}
						if (count) {
							for (int i = 0; i < 2; i++) {
								Mean[i] /= count;
							}

							// 分散、共分散
							for (int i = 0; i < m_Document->m_Cards->Count; i++)
								if (m_CardVisible[i]) {
									TCard *Card = m_Document->GetCardByIndex(i);
									if (Card->m_Labels->Contain(il + 1)) {
										StDev[0] +=
										(m_CardX[i] - Mean[0]) *
										(m_CardX[i] - Mean[0]);
										StDev[1] +=
										(m_CardY[i] - Mean[1]) *
										(m_CardY[i] - Mean[1]);
										Cov += (m_CardX[i] - Mean[0]) *
										(m_CardY[i] - Mean[1]);
									}
								}

							if (StDev[0] > 0.0f && StDev[1] > 0.0f) {
								for (int i = 0; i < 2; i++) {
									StDev[i] = sqrt(StDev[i] / count);
								}
								Cov = Cov / count;

								if (StDev[0] < StDev[1] * 0.25) {
									StDev[0] = StDev[1] * 0.25;
								}
								if (StDev[1] < StDev[0] * 0.25) {
									StDev[1] = StDev[0] * 0.25;
								}

								// float correl = Cov / (StDev[0] * StDev[1]);

								P->Width = (int)(5 * m_fFontZoom);
								P->Color =
									HalfColor((TColor)Label->m_nColor,
									m_nBGColor, 0.5f);

								// 塗りつぶす
								if (SettingView.m_bLabelFill &&
									(Bu_ArrangeType->Tag % 100 == 3)
									&& Bu_ArrangeType->Tag < 500) {
									B->Style = bsSolid;
									B->Color =
										HalfColor((TColor)Label->m_nColor,
										m_nBGColor, 0.9f);
								}
								else {
									B->Style = bsClear;
								}

								C->Ellipse((Mean[0] - StDev[0] * 2.1f) +
									m_nXOffset, (Mean[1] - StDev[1] * 2.1f) +
									m_nYOffset, (Mean[0] + StDev[0] * 2.1f) +
									m_nXOffset, (Mean[1] + StDev[1] * 2.1f) +
									m_nYOffset);

								if (SettingView.m_bLabelNameVisible) {
									F->Color = (TColor)Label->m_nColor;
									F->Height =
										(RE_Edit->Font->Height *
										Label->m_nSize * m_fFontZoom) * 0.01;
									F->Style = TFontStyles() << fsBold;
									C->TextOut
										(Mean[0] + m_nXOffset -
										C->TextWidth(Label->m_Name) * 0.5,
										(Mean[1] - StDev[1] * 2.1f) +
										m_nYOffset + C->TextHeight
										(Label->m_Name) * 0.5, Label->m_Name);
								}

							}

						}
					}
					first = false;
				}
			}
		}
		else if (SettingView.m_bLabelRectangleVisible) {
			// ラベルを囲む四角

			for (int i = 0; i < m_Document->m_Labels[0]->Count; i++) {
				int il = draworder[i];
				TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
				if (Label->m_bEnable && !Label->m_bFold) {
					// 有効かつFoldされていないラベル

					if ((first && drawtoporder) || (!first && drawothers)) {
						// 座標
						float MinX, MinY, MaxX, MaxY, OrgMinY;
						int count = 0;

						for (int i = 0; i < m_Document->m_Cards->Count; i++)
							if (m_CardVisible[i]) {
								TCard *Card = m_Document->GetCardByIndex(i);
								if (Card->m_Labels->Contain(il + 1)) {
									if (count == 0 || MinX > m_CardX[i] -
										(m_CardWidth[i] >> 1)) {
										MinX = m_CardX[i] -
										(m_CardWidth[i] >> 1);
									}
									if (count == 0 || OrgMinY > Card->m_fY) {
										OrgMinY = Card->m_fY;
									}
									if (count == 0 || MinY > m_CardY[i] -
										(m_CardHeight[i] >> 1)) {
										MinY = m_CardY[i] -
										(m_CardHeight[i] >> 1);
									}
									if (count == 0 || MaxX < m_CardX[i] +
										(m_CardWidth[i] >> 1)) {
										MaxX = m_CardX[i] +
										(m_CardWidth[i] >> 1);
									}
									if (count == 0 || MaxY < m_CardY[i] +
										(m_CardHeight[i] >> 1)) {
										MaxY = m_CardY[i] +
										(m_CardHeight[i] >> 1);
									}
									count++;
								}
							}
						if (count) {
							P->Width = (int)(5 * m_fFontZoom);
							P->Color =
								HalfColor((TColor)Label->m_nColor,
								m_nBGColor, 0.5f);
							B->Style = bsClear;

							// 塗りつぶす
							if (SettingView.m_bLabelFill &&
								(Bu_ArrangeType->Tag % 100 == 3)
								&& Bu_ArrangeType->Tag < 500) {
								B->Style = bsSolid;
								B->Color =
									HalfColor((TColor)Label->m_nColor,
									m_nBGColor, 0.9f);
							}
							else {
								B->Style = bsClear;
							}

							int space = m_nFontHeight / 2;

							C->RoundRect(MinX + m_nXOffset - space,
								MinY + m_nYOffset - space,
								MaxX + m_nXOffset + space,
								MaxY + m_nYOffset + space, space, space);

							if (SettingView.m_bLabelNameVisible) {
								F->Color = (TColor)Label->m_nColor;
								F->Height =
									(RE_Edit->Font->Height * Label->m_nSize *
									m_fFontZoom) * 0.01;
								F->Style = TFontStyles() << fsBold;
								B->Style = bsClear;
								if (OrgMinY < 0.4f) {
									C->TextOut
										(((MinX + MaxX) / 2) + m_nXOffset -
										C->TextWidth(Label->m_Name) * 0.5,
										MinY - space - space -
										C->TextHeight(Label->m_Name) +
										m_nYOffset, Label->m_Name);
								}
								else {
									C->TextOut
										(((MinX + MaxX) / 2) + m_nXOffset -
										C->TextWidth(Label->m_Name) * 0.5,
										MaxY + space + space + m_nYOffset,
										Label->m_Name);
								}
							}
						}
					}
					first = false;
				}
			}
		}

		delete[]draworder;
	}
}

// ---------------------------------------------------------------------------
TColor TFo_Main::GetCardColor(TCard *Card, float &SizeX) {
	// ラベルによるカードの基本色を取得
	TColor c = (TColor)0;
	int count = 0;
	for (int il = 0; il < Card->m_Labels->Count; il++) {
		TCardLabel *Label = m_Document->GetLabelByIndex(0,
			Card->m_Labels->GetLabel(il) - 1);
		if (Label->m_bEnable) {
			c = HalfColor(c, (TColor)Label->m_nColor, 1.0f / (count + 1));
			SizeX *= m_Document->GetLabelByIndex(0,
				Card->m_Labels->GetLabel(il) - 1)->m_nSize / 100.0f;
			count++;
		}
	}

	return c;
}

// ---------------------------------------------------------------------------
void TFo_Main::PaintSub(TCanvas *C) {
	if (SettingView.m_nSpecialPaint == 1) {
		PaintSub_GUC(C);
		return;
	}

	SetCardVisible();

	// DeZoom
	float Zoom = pow(2, TB_Zoom->Position / 2000.0f);
	Sc_X->LargeChange = 10000 / Zoom;
	Sc_Y->LargeChange = 10000 / Zoom;
	Sc_X->SmallChange = 1000 / Zoom;
	Sc_Y->SmallChange = 1000 / Zoom;

	// 状態変数準備
	if (m_nTmpCardsCount != m_Document->m_Cards->Count) {
		m_nTmpCardsCount = m_Document->m_Cards->Count;
		if (m_CardX != NULL) {
			delete[]m_CardVisible;
			delete[]m_LinkVisible;
			delete[]m_CardTitle;
			delete[]m_CardRelated;
			delete[]m_CardAssign;
			delete[]m_CardShape;
			delete[]m_CardX;
			delete[]m_CardY;
			delete[]m_CardWidth;
			delete[]m_CardHeight;
		}
		m_CardVisible = new bool[m_Document->m_Cards->Count];
		m_LinkVisible = new bool[m_Document->m_Links->Count];
		m_CardTitle = new UnicodeString[m_Document->m_Cards->Count];
		m_CardX = new float[m_Document->m_Cards->Count];
		m_CardY = new float[m_Document->m_Cards->Count];
		m_CardWidth = new int[m_Document->m_Cards->Count];
		m_CardHeight = new int[m_Document->m_Cards->Count];
		m_CardRelated = new bool[m_Document->m_Cards->Count]; // 関係あるカード
		m_CardAssign = new int[m_Document->m_Cards->Count];
		m_CardShape = new int[m_Document->m_Cards->Count];
	}
	// 状態変数準備
	if (m_nTmpLinksCount != m_Document->m_Links->Count) {
		m_nTmpLinksCount = m_Document->m_Links->Count;
		if (m_LinkVisible != NULL) {
			delete[]m_LinkVisible;
		}
		m_LinkVisible = new bool[m_Document->m_Links->Count];
	}
	memset(m_CardRelated, 0, sizeof(bool) * m_Document->m_Cards->Count);
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		m_CardAssign[i] = i;
	}

	// 各カードの座標を得て、ノーマライズする
	// float maxx = 0.5f, minx = 0.5f, maxy = 0.5f, miny = 0.5f;
	float addx = (Zoom * (0.05f - Sc_X->Position * 0.0001f) + 0.5f)
		* m_nBrowserWidth;
	float addy = (Zoom * (0.05f - Sc_Y->Position * 0.0001f) + 0.5f)
		* m_nBrowserHeight;
	float mpyx = Zoom * 0.9f * m_nBrowserWidth;
	float mpyy = Zoom * 0.9f * m_nBrowserHeight;
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		m_CardVisible[i] = Card->m_bVisible;
		m_CardTitle[i] = Card->m_Title;

		/*
		 m_CardX[i] = (((x * 0.9f + 0.05f) - (Sc_X->Position - 5000.0f) * 0.0001f) - 0.5f) * Zoom + 0.5f;
		 m_CardX[i] = (x * 0.9f + 0.05f - Sc_X->Position * 0.0001f) * Zoom + 0.5f;
		 m_CardX[i] = Zoom * x * 0.9f + Zoom * 0.05f - Zoom * Sc_X->Position * 0.0001f + 0.5f;
		 m_CardX[i] = Zoom * 0.9f * x + Zoom * (0.05f - Sc_X->Position * 0.0001f) + 0.5f;
		 */
		m_CardX[i] = mpyx * Card->m_fX + addx;
		m_CardY[i] = mpyy * Card->m_fY + addy;

		/*
		 float scrollx = (Sc_X->Position - 5000.0f) * 0.0001f;
		 float x = Card->m_fX;
		 x = x * 0.9f + 0.05f;
		 x -= scrollx;
		 x = (x - 0.5f) * Zoom + 0.5f;
		 m_CardX[i] = x;

		 float scrolly = (Sc_Y->Position - 5000.0f) * 0.0001f;
		 float y = Card->m_fY;
		 y = y * 0.9f + 0.05f;
		 y -= scrolly;
		 y = (y - 0.5f) * Zoom + 0.5f;
		 m_CardY[i] = y;
		 */

		m_CardShape[i] = Card->m_nShape;
	}

	// TCanvas *C = BMP->Canvas;
	TBrush *B = C->Brush;
	TPen *P = C->Pen;
	TFont *F = C->Font;
	F->Name = RE_Edit->Font->Name;
	F->Charset = RE_Edit->Font->Charset;
	F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);
	m_nFontHeight = C->TextHeight(" ");

	// 背景塗りつぶし
	if (fabs(m_fFontZoom - 1.0) <
		0.1 && !(Im_Wall->Enabled && SettingView.m_bFixWallPaper)) {
		B->Color = TColor(m_nBGColor);
		B->Style = bsSolid;
		C->FillRect(Rect(0, 0, m_nBrowserWidth, m_nBrowserHeight));
	}

	// 壁紙表示
	if (Im_Wall->Enabled) {
		if (SettingView.m_bFixWallPaper) {
			// 固定表示
			C->StretchDraw(Rect(m_nXOffset, m_nYOffset,
				m_nXOffset + m_nBrowserWidth, m_nYOffset + m_nBrowserHeight),
				Im_Wall->Picture->Graphic);
		}
		else {
			// スクロール表示
			int startx = 0, starty = 0, endx = 0, endy = 0;
			if (SettingView.m_bTileWallPaper) {
				startx = -4;
				endx = 4;
				starty = -4;
				endy = 4;
			}
			for (int iy = starty; iy <= endy; iy++) {
				int top =
					((iy - (Sc_Y->Position - 5000.0f) * 0.0001f - 0.5f) * Zoom +
					0.5f) * m_nBrowserHeight;
				int bottom =
					((iy + 1 - (Sc_Y->Position - 5000.0f) * 0.0001f - 0.5f)
					* Zoom + 0.5f) * m_nBrowserHeight;
				if (bottom >= 0 && top < m_nBrowserHeight) {
					for (int ix = startx; ix <= endx; ix++) {
						int left =
							((ix - (Sc_X->Position - 5000.0f) * 0.0001f - 0.5f)
							* Zoom + 0.5f) * m_nBrowserWidth;
						int right =
							((ix + 1 - (Sc_X->Position - 5000.0f) * 0.0001f -
							0.5f) * Zoom + 0.5f) * m_nBrowserWidth;
						if (right >= 0 && left < m_nBrowserWidth) {
							C->StretchDraw(Rect(left + m_nXOffset,
								top + m_nYOffset, right + m_nXOffset,
								bottom + m_nYOffset),
							Im_Wall->Picture->Graphic);
						}
					}
				}
			}
		}

	}

	// 背景アニメーション
	if (SettingView.m_bBGAnimation) {
		BGAnimation(C);
	}

	if (SettingView.m_bTextVisible && m_nTargetCard >= 0) {
		// カードの本文描画
		F->Color = (TColor)HalfColor(m_nBGColor, m_nFGColor, 0.33);
		B->Style = bsClear;
		int height = C->TextHeight(" ") * 1.3;
		int x = (m_nFontHeight * 2) / 3;
		int y = (m_nFontHeight * 2) / 3;
		if (SettingView.m_bTextCentering) {
			int maxwidth = 0;
			for (int i = 0; i < RE_Edit->Lines->Count && y + i * height <
				m_nBrowserHeight; i++) {
				int width = C->TextWidth(RE_Edit->Lines->Strings[i]);
				if (width > maxwidth) {
					maxwidth = width;
				}
			}
			x = (m_nBrowserWidth - maxwidth) >> 1;
			if (x < (m_nFontHeight * 2) / 3) {
				x = (m_nFontHeight * 2) / 3;
			}
			int maxheight = height * RE_Edit->Lines->Count;
			y = (m_nBrowserHeight - maxheight) >> 1;
			if (y < (m_nFontHeight * 2) / 3) {
				y = (m_nFontHeight * 2) / 3;
			}
		}
		for (int i = 0; i < RE_Edit->Lines->Count && y + i * height <
			m_nBrowserHeight; i++) {
			C->TextOut(x + m_nXOffset, y + height * i + m_nYOffset,
				RE_Edit->Lines->Strings[i]);
		}
	}

	int *LinkCard1Index = new int[m_Document->m_Links->Count];
	int *LinkCard2Index = new int[m_Document->m_Links->Count];

	// LinkVisible設定
	for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++) {
		TLink *Link = m_Document->GetLinkByIndex(i2);

		LinkCard1Index[i2] = m_Document->SearchCardIndex(Link->m_nFromID);
		LinkCard2Index[i2] = m_Document->SearchCardIndex(Link->m_nDestID);

		bool visible = Link->m_bVisible;

		if (visible) {
			if (LinkCard1Index[i2] >= 0 && LinkCard2Index[i2] >= 0) {
				visible &= m_CardVisible[LinkCard1Index[i2]] && m_CardVisible
					[LinkCard2Index[i2]];
			}
			else {
				visible = false;
			}
		}

		m_LinkVisible[i2] = visible;
		Link->m_bVisible = visible;
	}

	// スコア計算。LinkVisibleが必要なのでこの位置
	if (SettingView.m_bScore) {
		IterScore();
	}

	// リンクの関係設定
	for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
		if (m_LinkVisible[i2]) {
			TLink *Link = m_Document->GetLinkByIndex(i2);

			if (Link->m_nFromID != m_nTargetCard && Link->m_nDestID !=
				m_nTargetCard && i2 != m_nTargetLink) {
				// 無関係なリンク
			}
			else {
				m_CardRelated[LinkCard1Index[i2]] = true;
				m_CardRelated[LinkCard2Index[i2]] = true;
			}
		}
	if (m_nTargetCard2 >= 0) {
		int card1index = m_Document->SearchCardIndex(m_nTargetCard2);
		m_CardRelated[card1index] = true;
	}
	if (SettingView.m_bLinkLimitation && SettingView.m_nLinkTarget >= 0) {
		int card1index = m_Document->SearchCardIndex(SettingView.m_nLinkTarget);
		m_CardRelated[card1index] = true;
	}

	// ラベルfold
	// （foldされるラベルのついているカードのうち、一つでもラベルがついていたら
	// すべてRelatedをTrueにする→どうせ関係あるリンクが後から表示されるので不要

	// 全てfoldされたラベルを持つカードが選択されている場合、、
	// 同じラベルを持つカードとそこからリンクの貼られたカードは全てRelatedにする

	// 折りたたまれたカードの反発力を下げる

	// また、カードについているラベルのすべてがFoldされている場合、
	// カード名をラベル名にする）

	// カードループ
	// まず全てのカードのfold状態を調べる
	for (int i = 0; i < m_Document->m_Cards->Count; i++)
		if (m_CardVisible[i]) {
			TCard *Card = m_Document->GetCardByIndex(i);
			Card->m_bLabelIsFold = m_Document->LabelIsFold(Card);
		}

	// カードループ
	for (int i = 0; i < m_Document->m_Cards->Count; i++)
		if (m_CardVisible[i]) {
			TCard *Card = m_Document->GetCardByIndex(i);
			if (Card->m_bLabelIsFold) {
				// すべてfoldされている
				// カードタイトルをラベル名に
				UnicodeString S = "";
				for (int il = 0; il < Card->m_Labels->Count; il++) {
					TCardLabel *Label =
						m_Document->GetLabelByIndex(0,
						Card->m_Labels->GetLabel(il) - 1);
					if (Label->m_bEnable) {
						if (S != "") {
							S += ", ";
						}
						S += Label->m_Name;
					}
				}
				m_CardTitle[i] = S;

				if (Card->m_nID == m_nTargetCard) {
					// 選択中のカード

					// カードループ
					// 同じラベルを持つカードは全てRelatedにする
					m_CardAssign[i] = i;
					for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
						if (i != i2 && m_CardVisible[i2]) {
							TCard *Card2 = m_Document->GetCardByIndex(i2);

							if (m_Document->LabelIsSame(Card, Card2)) {
								// 同じラベル構造のカード

								// 最後に見つかった同じラベル構造のカードに座標をアサインする
								m_CardAssign[i2] = i;

								m_CardRelated[i2] = true;

								// ここからリンクの貼られたカードは全てRelatedにする
								for (int i2 = 0;
								i2 < m_Document->m_Links->Count; i2++)
									if (m_LinkVisible[i2]) {
										TLink *Link =
										m_Document->GetLinkByIndex(i2);
										if (Card2->m_nID == Link->m_nFromID) {
										int idx = LinkCard2Index[i2];
										if (m_CardVisible[idx]) {
										m_CardRelated[idx] = true;
										}
										}
										else if (Card2->m_nID ==
										Link->m_nDestID) {
										int idx = LinkCard1Index[i2];
										if (m_CardVisible[idx]) {
										m_CardRelated[idx] = true;
										}
										}
									}
							}
						}
				}
				else if (m_CardAssign[i] == i) {
					// 選択中のカードでなく、代表カード
					// カードループ
					// 同じラベルを持つカードをiの示すカードにアサインする
					for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
						if (i != i2 && m_CardVisible[i2]) {
							TCard *Card2 = m_Document->GetCardByIndex(i2);

							if (m_Document->LabelIsSame(Card, Card2)) {
								// 同じラベル構造のカード

								// 最初に見つかった同じラベル構造のカードに座標をアサインする
								// 選択中でないカードは、一番裏（最初のほう）のカードにAssignされる
								m_CardAssign[i2] = i;
							}
						}
				}
				else {
					// 選択中のカードでもなく、代表カードでない

					int idxbak = m_CardAssign[i];

					// カードループ
					// 後に出てきたこのカードを代表カードに
					for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
						if (m_CardVisible[i2] && m_CardAssign[i2] == idxbak) {
							m_CardAssign[i2] = i;
						}
				}
			}
		}

	// 関連カードのうち一つでもRelatedがTrueなら、関連カードのRelatedをすべてTrueに
	// カードループ
	for (int i = 0; i < m_Document->m_Cards->Count; i++)
		if (m_CardVisible[i] && m_CardRelated[i]) {
			// 関連カード
			TCard *Card = m_Document->GetCardByIndex(i);

			if (Card->m_bLabelIsFold) {
				// すべてfoldされている

				// カードループ
				// 関連カードを全てRelatedに
				for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
					if (m_CardAssign[i] == m_CardAssign[i2]) {
						m_CardRelated[i2] = true;
					}
			}
		}

	// Foldされたものの中でShapeの多数決を取る
	int *shapevote = new int[IL_Shape->Count];
	for (int i = 0; i < m_Document->m_Cards->Count; i++)
		if (m_CardVisible[i] && m_CardAssign[i] == i) {
			// 表示対象カードで代表カード
			TCard *Card = m_Document->GetCardByIndex(i);

			if (Card->m_bLabelIsFold) {
				// Foldされている

				// 投票開始
				memset(shapevote, 0, sizeof(int) * IL_Shape->Count);

				for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
					if (m_CardVisible[i] && m_CardAssign[i2] == i) {
						TCard *Card2 = m_Document->GetCardByIndex(i2);
						if (Card2->m_nShape < IL_Shape->Count) {
							shapevote[Card2->m_nShape]++;
						}
					}

				// 一番多い形を得る
				int maxindex = 2;
				int max = 0;
				for (int i2 = 0; i2 < IL_Shape->Count; i2++) {
					if (shapevote[i2] > max) {
						max = shapevote[i2];
						maxindex = i2;
					}
				}

				// 反映
				for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
					if (m_CardVisible[i] && m_CardAssign[i2] == i) {
						m_CardShape[i2] = maxindex;
					}
			}
		}
	delete[]shapevote;

	int *draworder = new int[m_Document->m_Cards->Count];
	memset(draworder, 0, sizeof(int) * m_Document->m_Cards->Count);
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		int vo = m_Document->GetCardByIndex(i)->m_nViewedOrder;
		draworder[vo] = i;
	}

	// 縁取り色
	// TColor HMColor = HalfColor(m_nBGColor, m_nFGColor, 0.125);
	TColor HMColor = TColor(m_nBGColor);

	// 必要に応じてカードサイズの事前計算
	// 影描画
	if ((m_Document->m_Labels[0]->Count >
		0 && SettingView.m_bLabelRectangleVisible) || // ラベルを囲む矩形
		SettingView.m_bCardShadow) {
		for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
			int i = draworder[ic];
			if (m_CardVisible[i] && m_CardAssign[i] == i) {
				TCard *Card = m_Document->GetCardByIndex(i);

				if ((ic >= m_Document->m_Cards->Count - 3 && m_bShowRecent) ||
					m_nTargetCard == Card->m_nID) {
					// 最近表示したカード強調
					F->Style = TFontStyles() << fsBold;
				}
				else {
					F->Style = TFontStyles();
				}

				if (m_CardRelated[i]) {
					// 最近表示したカード強調
					P->Width = (int)(3 * m_fFontZoom);
				}
				else {
					P->Width = (int)(1 * m_fFontZoom);
				}

				float SizeX = Card->m_nSize;
				if (m_Document->CountEnableLabel(Card)) {
					// ラベルあり
					for (int il = 0; il < Card->m_Labels->Count; il++) {
						TCardLabel *Label =
							m_Document->GetLabelByIndex(0,
							Card->m_Labels->GetLabel(il) - 1);
						if (Label->m_bEnable) {
							SizeX *=
								m_Document->GetLabelByIndex(0,
								Card->m_Labels->GetLabel(il) - 1)
								->m_nSize / 100.0f;
						}
					}
				}
				if (SettingView.m_bMagnifyFocused && Card->m_nID ==
					m_nTargetCard) {
					SizeX *= 1.2;
				}

				if (SettingView.m_bCardShadow) {
					int xoffsetbak = m_nXOffset;
					int yoffsetbak = m_nYOffset;
					m_nXOffset += m_nFontHeight / 2;
					m_nYOffset += m_nFontHeight / 2;

					// 影の色（中間色を暗くしたもの）
					P->Color =
						HalfColor(HalfColor(m_nFGColor, m_nBGColor, 0.5f),
						0x0, 0.33f);
					B->Color = P->Color;
					B->Style = bsSolid;
					F->Color = P->Color;
					DrawCard(C, Card, (int)SizeX, i, HMColor, 0x1);

					m_nXOffset = xoffsetbak;
					m_nYOffset = yoffsetbak;
				}
				else {
					CalcCardSize(C, Card, (int)SizeX, i);
				}
			}
		}
	}

	int linkoption = 0;
	if (SettingView.m_bLinkShadow) {
		linkoption |= 0x1;
	}
	if (SettingView.m_bLinkHemming) {
		linkoption |= 0x2;
	}

	// リンクの影
	if (SettingView.m_bLinkShadow) {
		P->Style = psSolid;
		P->Width = (int)(1 * m_fFontZoom);
		F->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.25);

		int xoffsetbak = m_nXOffset;
		int yoffsetbak = m_nYOffset;
		m_nXOffset += m_nFontHeight / 2;
		m_nYOffset += m_nFontHeight / 2;

		for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
			if (m_LinkVisible[i2]) {
				TLink *Link = m_Document->GetLinkByIndex(i2);

				P->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.5);
				Link->m_Color = P->Color;
				DrawLink(C, Link, LinkCard1Index[i2], LinkCard2Index[i2],
					HMColor, 0x1);
			}

		m_nXOffset = xoffsetbak;
		m_nYOffset = yoffsetbak;
	}

	// ラベルを囲む円、四角
	DrawLabelCircleRect(C, !SettingView.m_bLavelCRFocusedOnTop, true);

	// フォントを元に戻す
	F->Style = TFontStyles();
	F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);

	// リンク描画（無関係）
	P->Width = (int)(3 * m_fFontZoom);
	F->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.5);

	for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
		if (m_LinkVisible[i2]) {
			TLink *Link = m_Document->GetLinkByIndex(i2);

			if (!(m_CardRelated[LinkCard1Index[i2]] && m_CardRelated
				[LinkCard2Index[i2]])) {
				// 無関係なリンク

				if (Link->m_Labels->Count == 0) {
					P->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.25);
				}
				else {
					TColor c = (TColor)0;
					for (int il = 0; il < Link->m_Labels->Count; il++) {
						TCardLabel *Label =
							m_Document->GetLabelByIndex(1,
							Link->m_Labels->GetLabel(il) - 1);
						c = HalfColor(c, (TColor)Label->m_nColor,
							1.0f / (il + 1));
					}
					P->Color = c;
				}

				if ((Bu_ArrangeType->Tag % 100 > 2 && Bu_ArrangeType->Tag <=
					499) || (Bu_ArrangeType->Tag >=
					500 && Bu_ArrangeType->Tag <= 999)) {
					// リンクが重要でないArrangeではリンクの色を薄く
					P->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
				}
				else if (Bu_ArrangeType->Tag >=
					1000 && Bu_ArrangeType->Tag <= 1999) {
					// 階層表示
					TCard *From = m_Document->GetCard(Link->m_nFromID);
					TCard *Dest = m_Document->GetCard(Link->m_nDestID);
					if (From->m_nParentID != Dest->m_nID && Dest->m_nParentID !=
						From->m_nID) {
						P->Color = HalfColor(P->Color, m_nBGColor, 0.75f);
					}
				}

				// 無関係なリンク
				Link->m_Color = P->Color;
				DrawLink(C, Link, LinkCard1Index[i2], LinkCard2Index[i2],
					HMColor, linkoption);
			}
		}

	// カード描画（無関係）
	P->Style = psSolid;
	B->Style = bsSolid;
	F->Color = (TColor)m_nFGColor;
	for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
		int i = draworder[ic];
		// int i = ic;
		TCard *Card = m_Document->GetCardByIndex(i);

		if (!m_CardRelated[i] && m_CardAssign[i] == i && Card->m_bVisible &&
			Card->m_nID != m_nTargetCard) {
			// 無関係なカード
			if (ic >= m_Document->m_Cards->Count - 3 && m_bShowRecent) {
				// 最近表示したカード強調
				F->Style = TFontStyles() << fsBold;
				P->Width = (int)(3 * m_fFontZoom);
			}
			else {
				F->Style = TFontStyles();
				P->Width = (int)(1 * m_fFontZoom);
			}

			float SizeX = Card->m_nSize;
			if (m_Document->CountEnableLabel(Card)) {
				// ラベルあり（ラベルの色にする）
				TColor c = GetCardColor(Card, SizeX);
				P->Color = HalfColor(c, m_nBGColor, 0.33f);
				B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
			}
			else {
				// ラベルなし
				P->Color = HalfColor(m_nFGColor, m_nBGColor, 0.5f);
				// B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor, 0.75f);
				B->Color = HalfColor(P->Color, m_nBGColor, 0.875f);
			}

			Card->m_Color = P->Color;
			DrawCard(C, Card, (int)SizeX, i, HMColor, 0);
		}
	}

	// リンク描画（関係あり）
	P->Style = psSolid;
	P->Width = (int)(3 * m_fFontZoom);
	F->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.25);
	F->Style = TFontStyles();
	for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
		if (m_LinkVisible[i2] && m_nTargetLink != i2) {

			if (m_CardRelated[LinkCard1Index[i2]] && m_CardRelated
				[LinkCard2Index[i2]]) {

				TLink *Link = m_Document->GetLinkByIndex(i2);

				if (Link->m_Labels->Count == 0) {
					P->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor,
						0.125); // (TColor)m_nFGColor;
				}
				else {
					TColor c = (TColor)0;
					for (int il = 0; il < Link->m_Labels->Count; il++) {
						TCardLabel *Label =
							m_Document->GetLabelByIndex(1,
							Link->m_Labels->GetLabel(il) - 1);
						c = HalfColor(c, (TColor)Label->m_nColor,
							1.0f / (il + 1));
					}
					P->Color = c;
				}

				if ((Bu_ArrangeType->Tag % 100 > 2 && Bu_ArrangeType->Tag <=
					499) || (Bu_ArrangeType->Tag >=
					500 && Bu_ArrangeType->Tag <= 999)) {
					// リンクが重要でないArrangeではリンクの色を薄く
					P->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
				}

				// 関係あるリンク
				Link->m_Color = P->Color;
				TColor HMColor2 = HalfColor(m_nFGColor, m_nBGColor, 0.5);
				// HalfColor(HMColor, m_nFGColor, 0.5)
				DrawLink(C, Link, LinkCard1Index[i2], LinkCard2Index[i2],
					HMColor2, linkoption | 0x02);
			}
		}

	P->Color = (TColor)m_nFGColor;
	if (m_bMDownBrowser == 1 && m_nTargetCard >=
		0 && (SB_Line->Down || SB_ToolLinkLabel->Down)) {
		// リンク線追加中
		if (SB_ToolLinkLabel->Down && m_nToolLinkLabel > 0) {
			// デフォルトリンクラベルあり
			TCardLabel *Label = m_Document->GetLabelByIndex(1,
				m_nToolLinkLabel - 1);
			P->Color = (TColor)Label->m_nColor;
		}

		if (m_nTargetCard2 >= 0) {
			// リンク線追加（相手あり）
			int card1index = m_Document->SearchCardIndex(m_nTargetCard);
			int card2index = m_Document->SearchCardIndex(m_nTargetCard2);

			DrawLink(C, NULL, card1index, card2index, HMColor, linkoption);
		}
		else if (m_bMDownBrowser && m_nTargetCard >= 0) {
			// リンク線追加中（相手無し）
			int card1index = m_Document->SearchCardIndex(m_nTargetCard);

			// リンク線追加中
			float X1 = m_CardX[card1index];
			float Y1 = m_CardY[card1index];
			DrawLink2(C, NULL, X1, Y1, m_nMDownTargetX, m_nMDownTargetY,
				HMColor, linkoption);
		}
	}

	// リンク描画（操作中）
	P->Color = (TColor)m_nFGColor;
	P->Style = psSolid;
	P->Width = (int)(3 * m_fFontZoom);
	F->Color = (TColor)m_nFGColor;
	if (m_nTargetLink >= 0) {
		// 選択中
		int i2 = m_nTargetLink;
		TLink *Link = m_Document->GetLinkByIndex(i2);
		if (Link) {
			// ラベルによる色
			if (Link->m_Labels->Count == 0) {
				P->Color = (TColor)m_nFGColor;
			}
			else {
				TColor c = (TColor)0;
				for (int il = 0; il < Link->m_Labels->Count; il++) {
					c = HalfColor(c, (TColor)m_Document->GetLabelByIndex(1,
						Link->m_Labels->GetLabel(il) - 1)->m_nColor,
						1.0f / (il + 1));
				}
				P->Color = c;
			}

			Link->m_Color = P->Color;
			if (!m_bMDownBrowserMoved || m_bMDownBrowser <= 1) {
				// ドラッグしていない
				int pos = m_nFocusCursorPos;
				float br = 0.0f;
				if (pos % 100 < 50) {
					br = 0.4f;
				}
				else {
				}
				TColor HMColor2 = HalfColor(m_nFGColor, m_nBGColor, br);
				// HalfColor(HMColor, m_nFGColor, 0.5)
				DrawLink(C, Link, LinkCard1Index[m_nTargetLink],
					LinkCard2Index[m_nTargetLink], HMColor2, linkoption | 0x2);
			}
			else
				switch (m_bMDownBrowser) {
				case 2:
					// Dest変更中
					if (m_nTargetCard2 >= 0) {
						// 相手がいる
						int index = m_Document->SearchCardIndex(m_nTargetCard2);
						DrawLink(C, Link, LinkCard1Index[m_nTargetLink], index,
							HMColor, linkoption);
					}
					else {
						// 相手がいない
						float X1 = m_CardX[LinkCard1Index[m_nTargetLink]];
						float Y1 = m_CardY[LinkCard1Index[m_nTargetLink]];
						DrawLink2(C, Link, X1, Y1, m_nMDownTargetX,
							m_nMDownTargetY, HMColor, linkoption);
					}
					break;
				case 3:
					// From変更中
					if (m_nTargetCard2 >= 0) {
						// 相手がいる
						int index = m_Document->SearchCardIndex(m_nTargetCard2);
						DrawLink(C, Link, index, LinkCard2Index[m_nTargetLink],
							HMColor, linkoption);
					}
					else {
						// 相手がいない
						float X1 = m_CardX[LinkCard2Index[m_nTargetLink]];
						float Y1 = m_CardY[LinkCard2Index[m_nTargetLink]];
						DrawLink2(C, Link, m_nMDownTargetX, m_nMDownTargetY, X1,
							Y1, HMColor, linkoption);
					}
					break;
				}
		}
	}

	delete[]LinkCard1Index;
	delete[]LinkCard2Index;

	// カード描画（関係あり）
	// for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
	P->Style = psSolid;
	B->Style = bsSolid;
	for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
		int i = draworder[ic];
		TCard *Card = m_Document->GetCardByIndex(i);

		if (m_CardRelated[i] && m_CardAssign[i]
			== i && Card->m_nID != m_nTargetCard && Card->m_bVisible) {
			// 関係のあるカード
			P->Width = (int)(3 * m_fFontZoom);
			if (ic >= m_Document->m_Cards->Count - 3 && m_bShowRecent) {
				// 最近表示したカード強調
				F->Style = TFontStyles() << fsBold;
			}
			else {
				F->Style = TFontStyles();
			}
			float SizeX = Card->m_nSize;
			if (m_Document->CountEnableLabel(Card)) {
				// ラベルあり（ラベルの色にする）
				TColor c = GetCardColor(Card, SizeX);
				P->Color = HalfColor(c, m_nBGColor, 0.25f);
				B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
			}
			else {
				P->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.25f);
				// B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor, 0.875f);
				B->Color = HalfColor(P->Color, m_nBGColor, 0.9375f);
			}

			Card->m_Color = P->Color;
			DrawCard(C, Card, (int)SizeX, i, HMColor, 0);
		}
	}

	// ラベルを囲む円、四角（普通は使わないオプション）
	/*
	 if (SettingView.m_bLavelCRFocusedOnTop){
	 DrawLabelCircleRect(C, m_fFontZoom, m_nFGColor, m_nBGColor, true, false);

	 //フォントを元に戻す
	 F->Style = TFontStyles();
	 F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);

	 //操作中のカードと同じラベルのカードを再表示
	 }
	 */

	delete[]draworder;

	// カード描画（操作中のカード）
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);

		if (Card->m_nID == m_nTargetCard) {
			// 編集中のカード
			F->Style = TFontStyles() << fsBold;

			P->Width = (int)(3 * m_fFontZoom);
			float SizeX = Card->m_nSize;
			if (m_Document->CountEnableLabel(Card)) {
				// ラベルあり（ラベルの色にする）
				TColor c = GetCardColor(Card, SizeX);
				P->Color = c;
				B->Color = HalfColor(P->Color, m_nBGColor, 0.33f);
			}
			else {
				P->Color = (TColor)m_nFGColor;
				// B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor, 0.875f);
				B->Color = HalfColor(P->Color, m_nBGColor, 0.96875);
			}

			if (SettingView.m_bMagnifyFocused) {
				SizeX *= 1.2;
			}

			Card->m_Color = P->Color;
			DrawCard(C, Card, (int)SizeX, i, HMColor, 0);
		}
	}

	// Focusを示すカーソル
	if (SettingView.m_bFocusCursor && (m_nTargetCard >=
		0 || m_nLastTarget >= 0)) {
		int current = m_Document->SearchCardIndex(m_nTargetCard);
		int last = m_Document->SearchCardIndex(m_nLastTarget);
		if (current >= 0 || last >= 0) {
			int pos = m_nFocusCursorPos;
			float br = 0.0f;
			if (pos > 100) {
				// 点滅処理
				if (pos % 100 < 50) {
					// pos = 100;
				}
				else {
					// pos = -1;
					br = 0.33f;
				}
				pos = 100;
			}

			if (pos >= 0) {
				// 場所、サイズ決定
				float ratio = pos * 0.01f;
				float fx, fy, fw, fh;
				float addsize; // 0～最大10になる（大きくなるほど薄く）
				if (last >= 0 && current >= 0) {
					// 現在のカードと直前のカードがある
					fx = m_CardX[last] * (1.0f - ratio) +
						m_CardX[current] * ratio;
					fy = m_CardY[last] * (1.0f - ratio) +
						m_CardY[current] * ratio;
					addsize = 0.5f - fabs(0.5f - ratio);
					fw = m_CardWidth[last] * (1.0f - ratio) +
						m_CardWidth[current] * ratio;
					fh = m_CardHeight[last] * (1.0f - ratio) +
						m_CardHeight[current] * ratio;
				}
				else if (last >= 0) {
					// 直前のカードのみある
					fx = m_CardX[last];
					fy = m_CardY[last];
					addsize = ratio;
					fw = m_CardWidth[last];
					fh = m_CardHeight[last];
				}
				else {
					// 現在のカードのみある
					fx = m_CardX[current];
					fy = m_CardY[current];
					addsize = 1.0f - ratio;
					fw = m_CardWidth[current];
					fh = m_CardHeight[current];
				}
				fw += m_nFontHeight * (addsize * 10.0f + 1);
				fh += m_nFontHeight * (addsize * 10.0f + 1);

				P->Color = HalfColor(m_nFGColor, m_nBGColor, addsize + br);

				P->Style = psSolid;
				if (addsize < 0.99f) {
					/*
					 C->Rectangle(
					 m_nXOffset + fx - fw * 0.5f,
					 m_nYOffset + fy - fh * 0.5f,
					 m_nXOffset + fx + fw * 0.5f,
					 m_nYOffset + fy + fh * 0.5f
					 );
					 */
					DrawFocusCursor(C, fx, fy, fw, fh);
				}
			}
		}
	}

	// 範囲選択
	if (m_bMDownBrowser == 5) {
		B->Style = bsClear;
		P->Width = m_fFontZoom * 3;
		P->Color = TColor(m_nFGColor);
		C->RoundRect(m_nXOffset + m_nMDownBrowserX * m_fFontZoom,
			m_nYOffset + m_nMDownBrowserY * m_fFontZoom,
			m_nXOffset + m_nMDownTargetX * m_fFontZoom,
			m_nYOffset + m_nMDownTargetY * m_fFontZoom, m_nFontHeight,
			m_nFontHeight);
	}

	// 縮小版表示
	if (SettingView.m_bOverview) {
		m_nOVWidth = m_nBrowserWidth / 6;
		m_nOVXOffset = m_nXOffset + m_nBrowserWidth - m_nOVWidth;
		m_nOVHeight = m_nBrowserHeight / 6;
		m_nOVYOffset = m_nYOffset + m_nBrowserHeight - m_nOVHeight;
		P->Color = HalfColor(m_nFGColor, m_nBGColor, 0.80f);
		B->Color = HalfColor(m_nFGColor, m_nBGColor, 0.95f);
		B->Style = bsSolid;
		P->Width = m_fFontZoom;
		P->Style = psSolid;
		C->Rectangle(m_nOVXOffset, m_nOVYOffset, m_nOVXOffset + m_nOVWidth,
			m_nOVYOffset + m_nOVHeight);

		HRGN MyRgn;
		MyRgn = ::CreateRectRgn(m_nOVXOffset, m_nOVYOffset,
			m_nOVXOffset + m_nOVWidth, m_nOVYOffset + m_nOVHeight);
		::SelectClipRgn(C->Handle, MyRgn);

		for (int i = 0; i < m_Document->m_Cards->Count; i++)
			if (m_CardVisible[i]) {
				TCard *Card = m_Document->GetCardByIndex(i);
				int x = m_nOVXOffset + ((Card->m_fX - 0.5f) * OVERVIEWSIZE +
					0.5f) * m_nOVWidth;
				int y = m_nOVYOffset + ((Card->m_fY - 0.5f) * OVERVIEWSIZE +
					0.5f) * m_nOVHeight;
				P->Color = Card->m_Color;
				B->Color = Card->m_Color;
				C->FillRect(Rect(x - 2 * m_fFontZoom, y - 1 * m_fFontZoom,
					x + 2 * m_fFontZoom, y + 1 * m_fFontZoom));
				// C->Ellipse(x - 2, y - 1, x + 3, y + 2);
			}
		float left, top, right, bottom;
		DispPosToCardPos(0, 0, left, top);
		DispPosToCardPos(m_nBrowserWidth - 1, m_nBrowserHeight - 1, right,
			bottom);
		P->Color = clRed; // TColor(0x00ffff);
		B->Style = bsClear;
		left = m_nOVXOffset + ((left - 0.5f) * OVERVIEWSIZE + 0.5f)
			* m_nOVWidth;
		top = m_nOVYOffset + ((top - 0.5f) * OVERVIEWSIZE + 0.5f) * m_nOVHeight;
		right = m_nOVXOffset + ((right - 0.5f) * OVERVIEWSIZE + 0.5f)
			* m_nOVWidth;
		bottom = m_nOVYOffset + ((bottom - 0.5f) * OVERVIEWSIZE + 0.5f)
			* m_nOVHeight;
		C->Rectangle(left, top, right, bottom);

		::DeleteObject(MyRgn);
	}

	// 特殊テキスト描画
	F->Height = Canvas->Font->Height * 2;
	B->Style = bsClear;
	F->Style = TFontStyles() << fsBold;
	F->Color = (TColor)SettingView.m_nFourgroundColor;

	// Demo String
	if (m_DemoString != "") {
		C->TextOut(8, m_nBrowserHeight - 8 - C->TextHeight(m_DemoString),
			m_DemoString);
	}

	// FPS
	m_nFPSCount++;
	unsigned int tgt = timeGetTime();
	if (tgt > m_nLastFPSTime + 1000) {
		unsigned int d = tgt - m_nLastFPSTime; // 経過ms
		m_fFPS = (m_nFPSCount * 1000.0f) / d;
		m_nFPSCount = 0;
		m_nLastFPSTime = tgt;
	}
	// C->TextOut(8, 8, FormatFloat("0.00", m_fFPS));
	// M_File->Caption = FloatToStr(m_fFPS);
}

// ---------------------------------------------------------------------------
void TFo_Main::PaintSub_GUC(TCanvas *C) {
	SetCardVisible();

	// DeZoom
	float Zoom = pow(2, TB_Zoom->Position / 2000.0f);
	Sc_X->LargeChange = 10000 / Zoom;
	Sc_Y->LargeChange = 10000 / Zoom;
	Sc_X->SmallChange = 1000 / Zoom;
	Sc_Y->SmallChange = 1000 / Zoom;

	// 状態変数準備
	if (m_nTmpCardsCount != m_Document->m_Cards->Count) {
		m_nTmpCardsCount = m_Document->m_Cards->Count;
		if (m_CardX != NULL) {
			delete[]m_CardVisible;
			delete[]m_LinkVisible;
			delete[]m_CardTitle;
			delete[]m_CardRelated;
			delete[]m_CardAssign;
			delete[]m_CardShape;
			delete[]m_CardX;
			delete[]m_CardY;
			delete[]m_CardWidth;
			delete[]m_CardHeight;
		}
		m_CardVisible = new bool[m_Document->m_Cards->Count];
		m_LinkVisible = new bool[m_Document->m_Links->Count];
		m_CardTitle = new UnicodeString[m_Document->m_Cards->Count];
		m_CardX = new float[m_Document->m_Cards->Count];
		m_CardY = new float[m_Document->m_Cards->Count];
		m_CardWidth = new int[m_Document->m_Cards->Count];
		m_CardHeight = new int[m_Document->m_Cards->Count];
		m_CardRelated = new bool[m_Document->m_Cards->Count]; // 関係あるカード
		m_CardAssign = new int[m_Document->m_Cards->Count];
		m_CardShape = new int[m_Document->m_Cards->Count];
	}
	// 状態変数準備
	if (m_nTmpLinksCount != m_Document->m_Links->Count) {
		m_nTmpLinksCount = m_Document->m_Links->Count;
		if (m_LinkVisible != NULL) {
			delete[]m_LinkVisible;
		}
		m_LinkVisible = new bool[m_Document->m_Links->Count];
	}
	memset(m_CardRelated, 0, sizeof(bool) * m_Document->m_Cards->Count);
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		m_CardAssign[i] = i;
	}

	// 各カードの座標を得て、ノーマライズする
	// float maxx = 0.5f, minx = 0.5f, maxy = 0.5f, miny = 0.5f;
	float addx = (Zoom * (0.05f - Sc_X->Position * 0.0001f) + 0.5f)
		* m_nBrowserWidth;
	float addy = (Zoom * (0.05f - Sc_Y->Position * 0.0001f) + 0.5f)
		* m_nBrowserHeight;
	float mpyx = Zoom * 0.9f * m_nBrowserWidth;
	float mpyy = Zoom * 0.9f * m_nBrowserHeight;
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		m_CardVisible[i] = Card->m_bVisible;
		m_CardTitle[i] = Card->m_Title;

		m_CardX[i] = mpyx * Card->m_fX + addx;
		m_CardY[i] = mpyy * Card->m_fY + addy;

		m_CardShape[i] = Card->m_nShape;
	}

	// Link非表示
	memset(m_LinkVisible, 0, sizeof(bool) * m_Document->m_Links->Count);

	TBrush *B = C->Brush;
	TPen *P = C->Pen;
	TFont *F = C->Font;
	F->Name = RE_Edit->Font->Name;
	F->Charset = RE_Edit->Font->Charset;
	F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);
	m_nFontHeight = C->TextHeight(" ");

	// 背景塗りつぶし
	if (fabs(m_fFontZoom - 1.0) <
		0.1 && !(Im_Wall->Enabled && SettingView.m_bFixWallPaper)) {
		B->Color = TColor(m_nBGColor);
		B->Style = bsSolid;
		C->FillRect(Rect(0, 0, m_nBrowserWidth, m_nBrowserHeight));
	}

	// 壁紙表示
	if (Im_Wall->Enabled) {
		if (SettingView.m_bFixWallPaper) {
			// 固定表示
			C->StretchDraw(Rect(m_nXOffset, m_nYOffset,
				m_nXOffset + m_nBrowserWidth, m_nYOffset + m_nBrowserHeight),
				Im_Wall->Picture->Graphic);
		}
		else {
			// スクロール表示
			int startx = 0, starty = 0, endx = 0, endy = 0;
			if (SettingView.m_bTileWallPaper) {
				startx = -4;
				endx = 4;
				starty = -4;
				endy = 4;
			}
			for (int iy = starty; iy <= endy; iy++) {
				int top =
					((iy - (Sc_Y->Position - 5000.0f) * 0.0001f - 0.5f) * Zoom +
					0.5f) * m_nBrowserHeight;
				int bottom =
					((iy + 1 - (Sc_Y->Position - 5000.0f) * 0.0001f - 0.5f)
					* Zoom + 0.5f) * m_nBrowserHeight;
				if (bottom >= 0 && top < m_nBrowserHeight) {
					for (int ix = startx; ix <= endx; ix++) {
						int left =
							((ix - (Sc_X->Position - 5000.0f) * 0.0001f - 0.5f)
							* Zoom + 0.5f) * m_nBrowserWidth;
						int right =
							((ix + 1 - (Sc_X->Position - 5000.0f) * 0.0001f -
							0.5f) * Zoom + 0.5f) * m_nBrowserWidth;
						if (right >= 0 && left < m_nBrowserWidth) {
							C->StretchDraw(Rect(left + m_nXOffset,
								top + m_nYOffset, right + m_nXOffset,
								bottom + m_nYOffset),
							Im_Wall->Picture->Graphic);
						}
					}
				}
			}
		}

	}

	// 背景アニメーション
	if (SettingView.m_bBGAnimation) {
		BGAnimation(C);
	}

	int *draworder = new int[m_Document->m_Cards->Count];
	memset(draworder, 0, sizeof(int) * m_Document->m_Cards->Count);
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		int vo = m_Document->GetCardByIndex(i)->m_nViewedOrder;
		draworder[vo] = i;
	}

	// 縁取り色
	// TColor HMColor = HalfColor(m_nBGColor, m_nFGColor, 0.125);
	TColor HMColor = TColor(m_nBGColor);

	// 必要に応じてカードサイズの事前計算
	// 影描画
	if ((m_Document->m_Labels[0]->Count >
		0 && SettingView.m_bLabelRectangleVisible) || // ラベルを囲む矩形
		SettingView.m_bCardShadow) {
		for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
			int i = draworder[ic];
			if (m_CardVisible[i] && m_CardAssign[i] == i) {
				TCard *Card = m_Document->GetCardByIndex(i);

				if ((ic >= m_Document->m_Cards->Count - 3 && m_bShowRecent) ||
					m_nTargetCard == Card->m_nID) {
					// 最近表示したカード強調
					F->Style = TFontStyles() << fsBold;
				}
				else {
					F->Style = TFontStyles();
				}

				if (m_CardRelated[i]) {
					// 最近表示したカード強調
					P->Width = (int)(3 * m_fFontZoom);
				}
				else {
					P->Width = (int)(1 * m_fFontZoom);
				}

				float SizeX = Card->m_nSize;
				if (m_Document->CountEnableLabel(Card)) {
					// ラベルあり
					for (int il = 0; il < Card->m_Labels->Count; il++) {
						TCardLabel *Label =
							m_Document->GetLabelByIndex(0,
							Card->m_Labels->GetLabel(il) - 1);
						if (Label->m_bEnable) {
							SizeX *=
								m_Document->GetLabelByIndex(0,
								Card->m_Labels->GetLabel(il) - 1)
								->m_nSize / 100.0f;
						}
					}
				}
				if (SettingView.m_bMagnifyFocused && Card->m_nID ==
					m_nTargetCard) {
					SizeX *= 1.2;
				}
				/*
				 if (SettingView.m_bCardShadow){
				 int xoffsetbak = m_nXOffset;
				 int yoffsetbak = m_nYOffset;
				 m_nXOffset += m_nFontHeight / 2;
				 m_nYOffset += m_nFontHeight / 2;

				 //影の色（中間色を暗くしたもの）
				 P->Color = HalfColor(HalfColor(m_nFGColor, m_nBGColor, 0.5f), 0x0, 0.33f);
				 B->Color = P->Color;
				 B->Style = bsSolid;
				 F->Color = P->Color;
				 DrawCard(C, Card, (int)SizeX, i, HMColor, 0x1);

				 m_nXOffset = xoffsetbak;
				 m_nYOffset = yoffsetbak;
				 }else{
				 CalcCardSize(C, Card, (int)SizeX, i);
				 } */
				CalcCardSize(C, Card, (int)SizeX, i);
			}
		}
	}

	// 各ラベルのDateを指定
	for (int il = 0; il < m_Document->m_Labels[0]->Count; il++) {
		TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
		Label->m_fTouched = 0.0;
	}
	// カードループ
	for (int i = 0; i < m_Document->m_Cards->Count; i++)
		if (m_CardVisible[i]) {
			TCard *Card = m_Document->GetCardByIndex(i);
			// カードのラベルループ
			for (int il = 0; il < Card->m_Labels->Count; il++) {
				TCardLabel *Label =
					m_Document->GetLabelByIndex(0,
					Card->m_Labels->GetLabel(il) - 1);
				if (Label->m_fTouched < Card->m_fViewed) {
					Label->m_fTouched = Card->m_fViewed;
				}
			}
		}
	// 表示順
	m_Document->RefreshDateOrder_Label();

	int *draworder_label = new int[m_Document->m_Labels[0]->Count];
	memset(draworder_label, 0, sizeof(int) * m_Document->m_Labels[0]->Count);
	for (int i = 0; i < m_Document->m_Labels[0]->Count; i++) {
		int vo = m_Document->GetLabelByIndex(0, i)->m_nTouchedOrder;
		draworder_label[vo] = i;
	}

	// ラベルとカード描画
	// ラベルループ
	for (int il_ = 0; il_ < m_Document->m_Labels[0]->Count; il_++) {
		int il = draworder_label[il_];
		TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
		if (Label->m_bEnable && !Label->m_bFold) {
			// 有効かつFoldされていないラベル

			// 座標
			float MinX, MinY, MaxX, MaxY, OrgMinY;
			int count = 0;

			for (int i = 0; i < m_Document->m_Cards->Count; i++)
				if (m_CardVisible[i]) {
					TCard *Card = m_Document->GetCardByIndex(i);
					if (Card->m_Labels->Contain(il + 1)) {
						if (count == 0 || MinX > m_CardX[i] -
							(m_CardWidth[i] >> 1)) {
							MinX = m_CardX[i] - (m_CardWidth[i] >> 1);
						}
						if (count == 0 || OrgMinY > Card->m_fY) {
							OrgMinY = Card->m_fY;
						}
						if (count == 0 || MinY > m_CardY[i] -
							(m_CardHeight[i] >> 1)) {
							MinY = m_CardY[i] - (m_CardHeight[i] >> 1);
						}
						if (count == 0 || MaxX < m_CardX[i] +
							(m_CardWidth[i] >> 1)) {
							MaxX = m_CardX[i] + (m_CardWidth[i] >> 1);
						}
						if (count == 0 || MaxY < m_CardY[i] +
							(m_CardHeight[i] >> 1)) {
							MaxY = m_CardY[i] + (m_CardHeight[i] >> 1);
						}
						count++;
					}
				}
			if (count) {
				P->Width = (int)(5 * m_fFontZoom);
				P->Color = HalfColor((TColor)Label->m_nColor, m_nBGColor, 0.5f);
				B->Style = bsClear;

				// 塗りつぶす
				if (SettingView.m_bLabelFill && (Bu_ArrangeType->Tag % 100 == 3)
					&& Bu_ArrangeType->Tag < 500) {
					B->Style = bsSolid;
					B->Color = HalfColor((TColor)Label->m_nColor,
						m_nBGColor, 0.9f);
				}
				else {
					B->Style = bsClear;
				}

				int space = m_nFontHeight / 2;

				C->RoundRect(MinX + m_nXOffset - space,
					MinY + m_nYOffset - space, MaxX + m_nXOffset + space,
					MaxY + m_nYOffset + space, space, space);
			}

			// ラベルを持つカード描画
			// フォントを元に戻す
			F->Style = TFontStyles();
			F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);

			P->Style = psSolid;
			B->Style = bsSolid;
			F->Color = (TColor)m_nFGColor;
			for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
				int i = draworder[ic];
				// int i = ic;
				TCard *Card = m_Document->GetCardByIndex(i);

				if (!m_CardRelated[i] && m_CardAssign[i]
					== i && Card->m_bVisible && Card->m_nID != m_nTargetCard &&
					Card->m_Labels->Contain(il + 1)) {
					// 無関係なカード
					if (ic >= m_Document->m_Cards->Count - 3 && m_bShowRecent) {
						// 最近表示したカード強調
						F->Style = TFontStyles() << fsBold;
						P->Width = (int)(3 * m_fFontZoom);
					}
					else {
						F->Style = TFontStyles();
						P->Width = (int)(1 * m_fFontZoom);
					}

					float SizeX = Card->m_nSize;
					if (m_Document->CountEnableLabel(Card)) {
						// ラベルあり（ラベルの色にする）
						TColor c = GetCardColor(Card, SizeX);
						P->Color = HalfColor(c, m_nBGColor, 0.33f);
						B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
					}
					else {
						// ラベルなし
						P->Color = HalfColor(m_nFGColor, m_nBGColor, 0.5f);
						// B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor, 0.75f);
						B->Color = HalfColor(P->Color, m_nBGColor, 0.875f);
					}

					Card->m_Color = P->Color;
					DrawCard(C, Card, (int)SizeX, i, HMColor, 0);
				}
			}
		}
	}

	// ラベル名描画
	// ラベルループ
	for (int il_ = 0; il_ < m_Document->m_Labels[0]->Count; il_++) {
		int il = draworder_label[il_];
		TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
		if (Label->m_bEnable && !Label->m_bFold) {
			// 有効かつFoldされていないラベル

			// 座標
			float MinX, MinY, MaxX, MaxY, OrgMinY;
			int count = 0;

			for (int i = 0; i < m_Document->m_Cards->Count; i++)
				if (m_CardVisible[i]) {
					TCard *Card = m_Document->GetCardByIndex(i);
					if (Card->m_Labels->Contain(il + 1)) {
						if (count == 0 || MinX > m_CardX[i] -
							(m_CardWidth[i] >> 1)) {
							MinX = m_CardX[i] - (m_CardWidth[i] >> 1);
						}
						if (count == 0 || OrgMinY > Card->m_fY) {
							OrgMinY = Card->m_fY;
						}
						if (count == 0 || MinY > m_CardY[i] -
							(m_CardHeight[i] >> 1)) {
							MinY = m_CardY[i] - (m_CardHeight[i] >> 1);
						}
						if (count == 0 || MaxX < m_CardX[i] +
							(m_CardWidth[i] >> 1)) {
							MaxX = m_CardX[i] + (m_CardWidth[i] >> 1);
						}
						if (count == 0 || MaxY < m_CardY[i] +
							(m_CardHeight[i] >> 1)) {
							MaxY = m_CardY[i] + (m_CardHeight[i] >> 1);
						}
						count++;
					}
				}
			if (count) {
				P->Width = (int)(5 * m_fFontZoom);
				P->Color = HalfColor((TColor)Label->m_nColor, m_nBGColor, 0.5f);
				B->Style = bsClear;

				int space = m_nFontHeight / 2;

				if (SettingView.m_bLabelNameVisible) {
					F->Color = (TColor)Label->m_nColor;
					F->Height =
						(RE_Edit->Font->Height * Label->m_nSize *
						m_fFontZoom) * 0.01;
					F->Style = TFontStyles() << fsBold;
					B->Style = bsClear;

					BeginPath(C->Handle);
					if (OrgMinY < 0.4f) {
						C->TextOut(((MinX + MaxX) / 2) + m_nXOffset -
							C->TextWidth(Label->m_Name) * 0.5,
							MinY - space - space - C->TextHeight(Label->m_Name)
							+ m_nYOffset, Label->m_Name);
					}
					else {
						C->TextOut(((MinX + MaxX) / 2) + m_nXOffset -
							C->TextWidth(Label->m_Name) * 0.5,
							MaxY + space + space + m_nYOffset, Label->m_Name);
					}
					EndPath(C->Handle);
					P->Color = TColor(m_nBGColor);
					P->Width = (abs(F->Height) / 8);
					if (P->Width < 4) {
						P->Width = 4;
					}
					StrokeAndFillPath(C->Handle);

					if (OrgMinY < 0.4f) {
						C->TextOut(((MinX + MaxX) / 2) + m_nXOffset -
							C->TextWidth(Label->m_Name) * 0.5,
							MinY - space - space - C->TextHeight(Label->m_Name)
							+ m_nYOffset, Label->m_Name);
					}
					else {
						C->TextOut(((MinX + MaxX) / 2) + m_nXOffset -
							C->TextWidth(Label->m_Name) * 0.5,
							MaxY + space + space + m_nYOffset, Label->m_Name);
					}
				}
			}
		}
	}

	delete[]draworder_label;

	// フォントを元に戻す
	F->Style = TFontStyles();
	F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);
	F->Color = (TColor)m_nFGColor;

	delete[]draworder;

	// カード描画（操作中のカード）
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);

		if (Card->m_nID == m_nTargetCard) {
			// 編集中のカード
			F->Style = TFontStyles() << fsBold;

			P->Width = (int)(3 * m_fFontZoom);
			float SizeX = Card->m_nSize;
			if (m_Document->CountEnableLabel(Card)) {
				// ラベルあり（ラベルの色にする）
				TColor c = GetCardColor(Card, SizeX);
				P->Color = c;
				B->Color = HalfColor(P->Color, m_nBGColor, 0.33f);
			}
			else {
				P->Color = (TColor)m_nFGColor;
				// B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor, 0.875f);
				B->Color = HalfColor(P->Color, m_nBGColor, 0.96875);
			}

			if (SettingView.m_bMagnifyFocused) {
				SizeX *= 1.2;
			}

			Card->m_Color = P->Color;
			DrawCard(C, Card, (int)SizeX, i, HMColor, 0);
		}
	}

	// Focusを示すカーソル
	if (SettingView.m_bFocusCursor && (m_nTargetCard >=
		0 || m_nLastTarget >= 0)) {
		int current = m_Document->SearchCardIndex(m_nTargetCard);
		int last = m_Document->SearchCardIndex(m_nLastTarget);
		if (current >= 0 || last >= 0) {
			int pos = m_nFocusCursorPos;
			float br = 0.0f;
			if (pos > 100) {
				// 点滅処理
				if (pos % 100 < 50) {
					// pos = 100;
				}
				else {
					// pos = -1;
					br = 0.33f;
				}
				pos = 100;
			}

			if (pos >= 0) {
				// 場所、サイズ決定
				float ratio = pos * 0.01f;
				float fx, fy, fw, fh;
				float addsize; // 0～最大10になる（大きくなるほど薄く）
				if (last >= 0 && current >= 0) {
					// 現在のカードと直前のカードがある
					fx = m_CardX[last] * (1.0f - ratio) +
						m_CardX[current] * ratio;
					fy = m_CardY[last] * (1.0f - ratio) +
						m_CardY[current] * ratio;
					addsize = 0.5f - fabs(0.5f - ratio);
					fw = m_CardWidth[last] * (1.0f - ratio) +
						m_CardWidth[current] * ratio;
					fh = m_CardHeight[last] * (1.0f - ratio) +
						m_CardHeight[current] * ratio;
				}
				else if (last >= 0) {
					// 直前のカードのみある
					fx = m_CardX[last];
					fy = m_CardY[last];
					addsize = ratio;
					fw = m_CardWidth[last];
					fh = m_CardHeight[last];
				}
				else {
					// 現在のカードのみある
					fx = m_CardX[current];
					fy = m_CardY[current];
					addsize = 1.0f - ratio;
					fw = m_CardWidth[current];
					fh = m_CardHeight[current];
				}
				fw += m_nFontHeight * (addsize * 10.0f + 1);
				fh += m_nFontHeight * (addsize * 10.0f + 1);

				P->Color = HalfColor(m_nFGColor, m_nBGColor, addsize + br);

				P->Style = psSolid;
				if (addsize < 0.99f) {
					/*
					 C->Rectangle(
					 m_nXOffset + fx - fw * 0.5f,
					 m_nYOffset + fy - fh * 0.5f,
					 m_nXOffset + fx + fw * 0.5f,
					 m_nYOffset + fy + fh * 0.5f
					 );
					 */
					DrawFocusCursor(C, fx, fy, fw, fh);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
bool TFo_Main::BrowserAutoScroll() {
	bool result = false; // 座標が更新されたかどうか

	// ゆっくりスタート
	m_fBrowserScrollRatio += 0.1f;
	if (m_fBrowserScrollRatio > 0.5f) {
		m_fBrowserScrollRatio = 0.5f;
	}

	// ターゲットカード、リンクを中心に持ってくる
	if (SB_AutoScroll->Down || m_nScrollTargetX > -65536) {
		float AX = 0.0f;
		float AY = 0.0f;

		if (m_nScrollTargetX <= -65536) {
			if (m_nAnimation > 0 && m_nAnimation != 3) {
				// ランダムアニメーションのときは中心に持ってくる
			}
			else if (m_nTargetCard >= 0) {
				TCard *Card = m_Document->GetCard(m_nTargetCard);
				if (Card) {
					AX = 0.5f - Card->m_fX;
					AY = 0.5f - Card->m_fY;
				}
				else {
					goto zoom;
				}
			}
			else if (m_nTargetLink >= 0) {
				TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
				if (Link) {
					TCard *Card1 = m_Document->GetCard(Link->m_nFromID);
					TCard *Card2 = m_Document->GetCard(Link->m_nDestID);
					AX = 0.5f - (Card1->m_fX + Card2->m_fX) * 0.5f;
					AY = 0.5f - (Card1->m_fY + Card2->m_fY) * 0.5f;
				}
				else {
					goto zoom;
				}
			}
			else {
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
		}
		else {
			AX = m_nScrollTargetX;
			AY = m_nScrollTargetY;
		}

		int xp = Sc_X->Position * (1.0f - m_fBrowserScrollRatio) +
			AX * m_fBrowserScrollRatio;
		int yp = Sc_Y->Position * (1.0f - m_fBrowserScrollRatio) +
			AY * m_fBrowserScrollRatio;
		if (Sc_X->Position != xp || Sc_Y->Position != yp) {
			PB_Browser->Tag = 1; // 描画禁止
			Sc_X->Position = xp;
			Sc_Y->Position = yp;
			PB_Browser->Tag = 0; // 描画再開
			result = true;
		}
		else {
			m_nScrollTargetX = -65536;
			m_nScrollTargetY = -65536;
		}
	}

zoom:
	// カードが画面内に収まるようにズーム
	if (SB_AutoZoom->Down) {
		// 分散計算
		float XD = 0.0f, YD = 0.0f;
		float CX = 0.5f, CY = 0.5f;

		if (SB_AutoScroll->Down) {
			if (m_nTargetCard >= 0) {
				// オートスクロールONでカードが選択されている場合、
				// 選択中のカードを中心にズーム
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
			if (m_CardVisible[i] && (m_CardRelated[i] ||
				(m_nTargetCard == -1 && m_nTargetLink == -1) ||
				(m_bShowRecent && Card->m_nViewedOrder >=
				m_Document->m_Cards->Count - 3) || Card->m_bSelected ||
				SettingView.m_bLinkLimitation)) {
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
				// 初期状態から何倍すればぴったりか
				XD /= pow(2, TB_Zoom->Position / 2000.0f); // 元座標での分散
				maxd /= pow(2, TB_Zoom->Position / 2000.0f); // 元座標でのずれ最大値
				float target = m_fZoomSD / XD; // 標準偏差がm_fZoomSDになる範囲を描画
				if (target > 0.4f / maxd) { // 標準偏差だけでは、はみだすカードが出てしまう場合
					target = 0.4f / maxd; // 最大のズレが0.4になる範囲を描画
				}

				// Zoomをいくつにすればぴったりか
				// pow(2, targetzoom / 2000.0f) = target;
				// pow(x,y) = exp(log(x) * y)
				// exp(log(x) * targetzoom / 200.0f) = target;
				// log(x) * targetzoom / 2000.0f = log(target);
				// targetzoom = log(target) / log(x) * 2000.0f;
				int targetzoom = (int)(log(target) / log(2) * 2000.0f);
				if (targetzoom < TB_Zoom->Min) {
					targetzoom = TB_Zoom->Min;
				}
				else if (targetzoom > TB_Zoom->Max) {
					targetzoom = TB_Zoom->Max;
				}

				// float zoomratio = 0.3f;
				float zoomratio = m_fBrowserScrollRatio;

				int zp =
					(int)(TB_Zoom->Position * (1.0f - zoomratio) +
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
					PB_Browser->Tag = 1; // 描画禁止
					TB_Zoom->Position = zp;
					PB_Browser->Tag = 0; // 描画再開
					result = true;
				}

				/*
				 PB_Browser->Tag = 1;//描画禁止
				 if (XD < 0.20f){
				 TB_Zoom->Position = TB_Zoom->Position + 1;
				 }else if (XD > 0.21f){
				 TB_Zoom->Position = TB_Zoom->Position - 1;
				 }
				 PB_Browser->Tag = 0;//描画再開
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
			int fright = m_CardX[i] + m_CardWidth[i] / 2 -
				m_nFontHeight + fsize;
			int fbottom = m_CardY[i] + m_CardHeight[i] / 2 -
				m_nFontHeight / 2 + fsize;
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

double DistanceToLine(int X, int Y, double X1, double Y1, double X2, double Y2)
{
	/*
	 double dtosource =
	 (X1 - X) * (X1 - X) +
	 (Y1 - Y) * (Y1 - Y);//Sourceまでの距離^2
	 double dtodest =
	 (X2 - X) * (X2 - X) +
	 (Y2 - Y) * (Y2 - Y);//Destまでの距離^2
	 */
	double K1, K2;
	if (X2 != X1) {
		K1 = (Y2 - Y1) / (X2 - X1);
	}
	else {
		K1 = 100000000000.0;
	}
	if (Y2 != Y1) {
		K2 = (X2 - X1) / (Y2 - Y1);
	}
	else {
		K2 = 100000000000.0;
	}
	double LX = (K1 * X1 + K2 * X - Y1 + Y) / (K1 + K2);
	double LY = K1 * ((K1 * X1 + K2 * X - Y1 + Y) / (K1 + K2) - X1) + Y1;

	double S = 0.5;
	if (fabs(X1 - X2) > 0.0 || fabs(Y1 - Y2) > 0.0) {
		if (fabs(X1 - X2) > fabs(Y1 - Y2)) {
			S = (LX - X1) / (X2 - X1);
		}
		else {
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

void __fastcall TFo_Main::PB_BrowserMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
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

	/*
	 if (RE_Edit->Parent == Pa_Editor){
	 //Browserで本文編集中ではない
	 TB_Zoom->SetFocus();
	 }
	 */
	bool LastEd_TitleBVisible = Ed_TitleB->Visible;
	RE_EditExit(Sender);
	TB_Zoom->SetFocus();
	/*
	 if (Fo_FullScreen->Visible){
	 }else{
	 TB_Zoom->SetFocus();
	 }
	 */
	m_bTitleEditRequested = false;
	// m_bTextEditRequested = false;

	if (SettingView.m_bOverview && Button == mbLeft) {
		if (X >= m_nOVXOffset && X < m_nOVXOffset + m_nOVWidth && Y >=
			m_nOVYOffset && Y < m_nOVYOffset + m_nOVHeight) {
			// Overview上がクリックされた

			m_bMDownBrowser = 4;
			m_fBrowserScrollRatio = 0.0f;
			PB_BrowserMouseMove(Sender, Shift, X, Y);
			return;
		}
	}

	m_nScrollTargetX = -65536;
	m_nScrollTargetY = -65536;
	/*
	 if (TreeMode()){
	 //Foldボタン
	 for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardVisible[i] && m_CardAssign[i] == i){
	 TCard *Card = m_Document->GetCardByIndex(i);
	 if (Card->m_bHasChild){
	 int fleft = m_CardX[i] + m_CardWidth[i] / 2 - m_nFontHeight;
	 int ftop = m_CardY[i] + m_CardHeight[i] / 2 - m_nFontHeight / 2;
	 int fsize = (m_nFontHeight * 2) / 3;
	 fsize = fsize / 2 * 2 + 1;
	 if (fsize < 13){
	 fsize = 13;
	 }
	 int fright = m_CardX[i] + m_CardWidth[i] / 2 - m_nFontHeight + fsize;
	 int fbottom = m_CardY[i] + m_CardHeight[i] / 2 - m_nFontHeight / 2 + fsize;
	 if (X >= fleft && X < fright && Y >= ftop && Y < fbottom){
	 Card->m_bFold = !Card->m_bFold;
	 m_bMDownBrowser = 0;
	 m_Document->RefreshList();
	 return;
	 }
	 }
	 }
	 }
	 */

	// カード選択
	int lastcard = m_nTargetCard;
	m_nTargetCard = -1;
	m_nTargetLink = -1;
	m_nTargetCard2 = -1;
	int aindex = -1;

	// 選択中のカード
	if (lastcard >= 0) {
		int i = m_Document->SearchCardIndex(lastcard);
		if (i >= 0)
			if (m_CardVisible[i] && m_CardAssign[i] == i) {
				if (CheckTreeButton(i, X, Y)) {
					return;
				}
				else if (X >= m_CardX[i] - m_CardWidth[i] / 2 && X <
					m_CardX[i] + m_CardWidth[i] / 2 && Y >= m_CardY[i] -
					m_CardHeight[i] / 2 && Y < m_CardY[i] + m_CardHeight[i] / 2)
				{
					aindex = i;
				}
			}
	}

	// 描画順序
	int *draworder = new int[m_Document->m_Cards->Count];
	memset(draworder, 0, sizeof(int) * m_Document->m_Cards->Count);
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		int vo = m_Document->GetCardByIndex(i)->m_nViewedOrder;
		draworder[vo] = i;
	}

	// 関連カード
	// for (int i = m_Document->m_Cards->Count - 1 ; i >= 0 && aindex == -1 ; i--) if (m_CardVisible[i] && m_CardRelated[i] && m_CardAssign[i] == i){
	for (int ic = m_Document->m_Cards->Count - 1; ic >= 0 && aindex == -1; ic--)
	{
		int i = draworder[ic];
		if (m_CardVisible[i] && m_CardRelated[i] && m_CardAssign[i] == i) {
			if (CheckTreeButton(i, X, Y)) {
				return;
			}
			else if (X >= m_CardX[i] - m_CardWidth[i] / 2 && X < m_CardX[i] +
				m_CardWidth[i] / 2 && Y >= m_CardY[i] -
				m_CardHeight[i] / 2 && Y < m_CardY[i] + m_CardHeight[i] / 2) {
				aindex = i;
			}
		}
	}

	// その他のカード
	// for (int i = m_Document->m_Cards->Count - 1 ; i >= 0 && aindex == -1 ; i--){
	for (int ic = m_Document->m_Cards->Count - 1; ic >= 0 && aindex == -1; ic--)
	{
		int i = draworder[ic];
		if (m_CardVisible[i] && m_CardAssign[i] == i) {
			if (CheckTreeButton(i, X, Y)) {
				return;
			}
			else if (X >= m_CardX[i] - m_CardWidth[i] / 2 && X < m_CardX[i] +
				m_CardWidth[i] / 2 && Y >= m_CardY[i] -
				m_CardHeight[i] / 2 && Y < m_CardY[i] + m_CardHeight[i] / 2) {
				aindex = i;
			}
		}
	}
	delete[]draworder;

	if (Shift.Contains(ssAlt) && Button == mbLeft) {
		// Altキーが押されている＝Link LimitationのTargetCard指定
		if (aindex >= 0) {
			int i = aindex;
			TCard *Card = m_Document->GetCardByIndex(i);
			SettingView.m_nLinkTarget = Card->m_nID;
		}
		else {
			SettingView.m_nLinkTarget = -1;
		}
		m_nTargetCard = lastcard;
		m_bMDownBrowser = 0;
		return;
		/*
		 }else if (Shift.Contains(ssCtrl) && !Shift.Contains(ssShift) && aindex >= 0 && lastcard >= 0){
		 //Ctrlを押しながらクリック＝選択操作
		 TCard *Card = m_Document->GetCardByIndex(aindex);
		 if (Card->m_nID != m_nTargetCard){
		 Card->m_bSelected = !Card->m_bSelected;
		 }
		 m_nTargetCard = lastcard;
		 m_bMDownBrowser = 0;
		 return;
		 */
	}
	else if (aindex >= 0) {
		// 何も押さずにクリック＝カード選択

		int i = aindex;
		TCard *Card = m_Document->GetCardByIndex(i);
		m_nTargetCard = Card->m_nID;

		if (!Shift.Contains(ssCtrl) && Button != mbRight && !Card->m_bSelected)
		{
			// 選択クリア
			m_Document->ClearCardSelection();
		}

		if (Button == mbLeft && Shift.Contains(ssCtrl) && !Shift.Contains
			(ssShift)) {
			Card->m_bSelected = !Card->m_bSelected;
		}
		else {
			Card->m_bSelected = true;
		}
		m_nMDownBrowserOffsetX = m_CardX[i] - X;
		m_nMDownBrowserOffsetY = m_CardY[i] - Y;

		if (Button == mbLeft) {
			if (!Card->m_bLabelIsFold) {
				// 折りたたまれていない
				if (SB_ToolLabel->Down) {
					if (m_nToolLabel > 0) {
						BackupSub(MLText.AddLabel);
						Card->m_Labels->AddLabel(m_nToolLabel);
					}
					else {
						BackupSub(MLText.DeleteLabel);
						Card->m_Labels->Clear();
					}
					m_Document->RefreshLabel();
				}

				if (Shift.Contains(ssShift) && Shift.Contains(ssCtrl)) {
					// Ctrlキー+Shiftを押しながらのクリック（外部リンク）
					TStringList *SL = new TStringList();
					for (int i2 = 0; i2 < Card->m_Lines->Count; i2++) {
						if (IsFileNameOrURL(Card->m_Lines->Strings[i2])) {
							SL->Add(Card->m_Lines->Strings[i2]);
						}
					}

					if (SL->Count == 1) {
						// 1つのみリンクが見つかった
						OpenExtLink(SL->Strings[0]);
						m_bMDownBrowser = 0;
						delete SL;
						return;
					}
					else if (SL->Count > 1) {
						// 複数リンクが見つかった

						// ポップアップ表示

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
						P.x = ((TControl*)Sender)->Left + X;
						P.y = ((TControl*)Sender)->Top + Y;
						P = ((TControl*)Sender)->ClientToScreen(P);
						Po_ExtLink->Popup(P.x, P.y);
						m_bMDownBrowser = 0;

						delete SL;
						return;
					}

					delete SL;
				}
				else if (!Card->m_bSelected) {
					// 選択されているカードをクリックし、非選択にした（Ctrlを押していた）

					m_bMDownBrowser = 0;
				}
				else if (lastcard == m_nTargetCard && !LastEd_TitleBVisible) {
					// 同じカードをクリック→タイトル編集
					bool b = true;
					if (SettingFile.fepOperation) {
						b = !SettingFile.fepOperation(m_Document,
							UnicodeString("Card_EditTitle").c_str());
					}

					if (RE_Edit->Parent != Pa_Editor &&
						!SettingView.m_bEditInBrowserAlways) {
						CloseTextEditBox();
					}

					if (b) {
						m_bTitleEditRequested = true;
					}
				}
				else if (Shift.Contains(ssShift) && !Shift.Contains(ssCtrl)) {
					// Shiftキーを押しながらのクリック（リンクを貼る）
					if (lastcard >= 0) {
						BackupSub(MI_NewLink->Caption);
						TLink *Link = m_Document->NewLink();
						Link->m_nFromID = lastcard;
						Link->m_nDestID = m_nTargetCard;

						if (SB_ToolLinkLabel->Down && m_nToolLinkLabel > 0) {
							// デフォルトリンクラベルあり
							Link->m_Labels->AddLabel(m_nToolLinkLabel);
						}
					}
				}
			}
			else {
				// ラベルがFoldされている（同じラベルのカードも同様に選択、非選択にする）
				for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
					if (m_CardVisible[i2] && i != i2) {
						TCard *Card2 = m_Document->GetCardByIndex(i2);
						if (m_Document->LabelIsSame(Card, Card2)) {
							Card2->m_bSelected = Card->m_bSelected;
						}
					}
			}
		}
	}
	else {
		// 空白をクリック

		if (Shift.Contains(ssCtrl)) {
			// Ctrlを押しながら
			// 選択クリアせず
			m_nTargetCard = lastcard;
			if (!Shift.Contains(ssShift)) {
				m_bMDownBrowser = 0;
				return;
			}
		}
		else {
			// カード選択クリア
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				TCard *Card = m_Document->GetCardByIndex(i);
				Card->m_bSelected = false;
			}
		}

		if (Shift.Contains(ssShift)) {
			// Shiftを押しながら（範囲選択）
			m_bMDownBrowser = 5;
			return;
		}
	}

	// リンク選択
	if (m_nTargetCard == -1 && m_Document->m_Links->Count > 0) {
		double mind = 0.0;
		int min = 0;
		bool editdest = false;
		int count = 0;

		// カードが選択されていない
		for (int i = 0; i < m_Document->m_Links->Count; i++)
			if (m_LinkVisible[i]) {
				TLink *Link = m_Document->GetLinkByIndex(i);
				int card1index = m_Document->SearchCardIndex(Link->m_nFromID);
				int card2index = m_Document->SearchCardIndex(Link->m_nDestID);
				if (m_CardVisible[card1index] && m_CardVisible[card2index]) {
					// リンクが表示されている（Linkをラベルで表示非表示するようになったら、それも考慮する必要あり）
					// TCard *Card1 = m_Document->GetCardByIndex(card1index);
					// TCard *Card2 = m_Document->GetCardByIndex(card2index);

					double X1 = m_CardX[card1index];
					double Y1 = m_CardY[card1index];
					double X2 = m_CardX[card2index];
					double Y2 = m_CardY[card2index];

					double S = 100.0f;
					if (Link->m_nShape < 6) {
						// 直線
						S = DistanceToLine(X, Y, X1, Y1, X2, Y2);
					}
					else {
						// 曲線
						// 線の長さ
						float xd = X2 - X1;
						float yd = Y2 - Y1;
						float len = sqrt(xd * xd + yd * yd) * sqrt(2);
						if (len > 0.0f) {
							float radstep = M_PI / len;

							float pos = radstep; // < PI
							float lastx = 0.0f, lasty = 0.0f;
							while (pos < M_PI) {
								float x0, y0;
								CurvePosToXY(pos, x0, y0);
								double S1 =
									DistanceToLine(X, Y, X1 + lastx, Y1 + lasty,
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
					}
					else {
						BackupSub(MLText.DeleteLabel);
						Link->m_Labels->Clear();
					}
					m_Document->RefreshLabel();
				}

				if (editdest) {
					m_bMDownBrowser = 2;
				}
				else {
					m_bMDownBrowser = 3;
				}
			}
		}
	}

	m_nMDownBrowserScrollX = Sc_X->Position;
	m_nMDownBrowserScrollY = Sc_Y->Position;
	if (!SettingView.m_bNoScrollLag) {
		m_uMDownBrowserLast = timeGetTime();
	}
	m_bMDownBrowserMoved = false;

	// ポップアップメニューの更新
	Ti_CheckTimer(Sender);
}
// ---------------------------------------------------------------------------

void TFo_Main::BrowserNewCard() {
	BackupSub(MI_NewCard->Caption);

	// 新規カード
	m_Document->ClearCardSelection();
	TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
	Card->m_bSelected = true;
	float cx, cy;
	DispPosToCardPos(m_nMDownTargetX, m_nMDownTargetY, cx, cy);
	Card->m_fX = cx;
	Card->m_fY = cy;

	if (SB_ToolLabel->Down && m_nToolLabel > 0) {
		// デフォルトラベルあり
		Card->m_Labels->AddLabel(m_nToolLabel);
	}

	m_nTargetCard = Card->m_nID;
	m_nTargetLink = -1;
	m_bDblClicked = true;

	// 編集ボックス移動
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

void __fastcall TFo_Main::PB_BrowserDblClick(TObject *Sender) {
	CloseEditBox();

	// ダブルクリックのフック
	if (SettingFile.fepOperation) {
		if (SettingFile.fepOperation(m_Document,
			UnicodeString("Card_DblClick").c_str())) {
			return;
		}
	}

	if (m_nTargetCard >= 0) {
		TCard *Card = m_Document->GetCard(m_nTargetCard);
		if (Card->m_bLabelIsFold) {
			// 折りたたまれている
			for (int il = 0; il < Card->m_Labels->Count; il++) {
				m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1)
					->m_bFold = false;
			}
		}
		else {
			// カードをダブルクリック（エディタを開く）
			if (SettingView.m_bEditInBrowser) {
				m_bTextEditRequested = true;
			}
			else {
				MV_EditorClick(Sender);
			}
		}
	}
	else {
		// 空白をダブルクリック
		BrowserNewCard();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_BrowserMouseUp(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	int MDownBrowserBak = m_bMDownBrowser;
	m_bMDownBrowser = 0;
	if (m_nTargetCard2 >= 0) {
		// リンク線追加、編集
		switch (MDownBrowserBak) {
		case 1: {
				// リンク追加
				TCard *Card = m_Document->GetCard(m_nTargetCard);
				if (Card) {
					BackupSub(MI_NewLink->Caption);
					TLink *Link = m_Document->NewLink();
					Link->m_nFromID = m_nTargetCard;
					Link->m_nDestID = m_nTargetCard2;

					if (SB_ToolLinkLabel->Down && m_nToolLinkLabel > 0) {
						// デフォルトリンクラベルあり
						Link->m_Labels->AddLabel(m_nToolLinkLabel);
					}

					m_nTargetCard = -1;
					m_nTargetLink = m_Document->m_Links->Count - 1;
				}
			} break;
		case 2:
			// Dest側を編集
			{
				BackupSub(MLText.EditLink);
				TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
				Link->m_nDestID = m_nTargetCard2;
				m_Document->RefreshList();
				m_Document->RefreshLink();
			} break;
		case 3:
			// From側を編集
			{
				BackupSub(MLText.EditLink);
				TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
				Link->m_nFromID = m_nTargetCard2;
				m_Document->RefreshList();
				m_Document->RefreshLink();
			} break;
		}

		// タイトル編集を消す
		CloseEditBox();
		CloseTextEditBox();
	}

	m_nTargetCard2 = -1;

	m_bRedrawRequested = true;

	m_bDblClicked = false;
}

// ---------------------------------------------------------------------------
void TFo_Main::CardPosToDispPos(float cx, float cy, float &dx, float &dy) {
	float Zoom = pow(2, TB_Zoom->Position / 2000.0f);
	float addx = (Zoom * (0.05f - Sc_X->Position * 0.0001f) + 0.5f)
		* m_nBrowserWidth;
	float addy = (Zoom * (0.05f - Sc_Y->Position * 0.0001f) + 0.5f)
		* m_nBrowserHeight;
	float mpyx = Zoom * 0.9f * m_nBrowserWidth;
	float mpyy = Zoom * 0.9f * m_nBrowserHeight;
	dx = mpyx * cx + addx;
	dy = mpyy * cy + addy;
}

// ---------------------------------------------------------------------------
void TFo_Main::DispPosToCardPos(float dx, float dy, float &cx, float &cy) {
	// カードをドラッグ中
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
		// 通常のドラッグ
		if (m_nMDownTargetX != m_nMDownBrowserX ||
			m_nMDownTargetY != m_nMDownBrowserY) {
			CloseEditBox();
		}
		if (m_nTargetCard >= 0) {
			if (SB_Cursor->Down) {
				float CardX;
				float CardY;
				DispPosToCardPos(X + m_nMDownBrowserOffsetX,
					Y + m_nMDownBrowserOffsetY, CardX, CardY);

				// カードをドラッグ中
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
						// foldされているか、Ctrlが押されている場合、同じラベルの全てのカードを移動
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
									}
									else {
										Card2->m_fX = CardX;
										Card2->m_fY = CardY;
									}
									moved->Add(Card2);
								}
							}
						}
					}
					else {
						// 通常の場合、選択中のカードを移動
						for (int i = 0; i < m_Document->m_Cards->Count; i++) {
							TCard *Card2 = m_Document->GetCardByIndex(i);
							if (Card->m_nID != Card2->m_nID &&
								Card2->m_bSelected) {
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
								/*
								 bool fold = Card2->m_bLabelIsFold;
								 if (fold){
								 for (int i3 = i + 1 ; i3 < m_Document->m_Cards->Count ; i3++) if (i3 != i){
								 TCard *Card3 = m_Document->GetCardByIndex(i3);
								 if (m_Document->LabelIsSame(Card2, Card3)){
								 Card3->m_fX = Card2->m_fX;
								 Card3->m_fY = Card2->m_fY;
								 }
								 }
								 }
								 */
							}
						}
					}

					if (TreeMode()) {
						// 木構造（ドラッグでIndex変更）

						// 動いたカードループ
						for (int i = 0; i < moved->Count; i++) {
							TCard *Card = (TCard*)moved->Items[i];

							// X座標決定
							float x = 0.0f;
							bool first = true;
							// カードループ
							for (int ic = 0;
							ic < m_Document->m_Cards->Count; ic++)
								if (m_CardVisible[ic]) {
									TCard *Card2 =
										m_Document->GetCardByIndex(ic);
									if (Card != Card2 && Card2->m_nParentID ==
										Card->m_nParentID) {
										// 同じ親を持つカード
										if (first) {
										x = Card2->m_fX;
										first = false;
										}
										else if (fabs(Card2->m_fX - Card->m_fX)
										< fabs(x - Card->m_fX)) {
										// よりX座標が今のカードに近い
										x = Card2->m_fX;
										}
									}
								}

							// 対象カード列挙
							TList *cards = new TList(); // 考慮対象
							for (int ic = 0;
							ic < m_Document->m_Cards->Count; ic++)
								if (m_CardVisible[ic]) {
									TCard *Card2 =
										m_Document->GetCardByIndex(ic);
									if (Card != Card2 && Card2->m_nParentID ==
										Card->m_nParentID) {
										if (fabs(Card2->m_fX - x) < 1.0e-5) {
										// X座標が同じ
										cards->Add(Card2);
										}
									}
								}

							// 対象カードと座標を比べながらスワップ
							for (int ic = 0; ic < cards->Count; ic++) {
								TCard *Card2 = (TCard*)cards->Items[ic];
								int index1 =
									m_Document->SearchCardIndex(Card->m_nID);
								int index2 =
									m_Document->SearchCardIndex(Card2->m_nID);
								if ((index1 < index2 && Card->m_fY >
									Card2->m_fY) ||
									(index1 > index2 && Card->m_fY <
									Card2->m_fY)) {
									// Indexが手前なのに、Y座標は後ろか、
									// Indexが後ろなのに、Y座標は手前

									// IndexをSwap
									m_Document->SwapCard(index1, index2);
								}
							}

							delete cards;
						}
					}

					delete moved;

					m_bRedrawRequested = true;
					/*
					 if (!Ed_TitleB->Visible){
					 m_bRedrawRequested = true;
					 }else{
					 PB_BrowserPaint(Sender);
					 }
					 */
				}
			}
			else {
				// リンク線
				m_nTargetCard2 = -1;
				for (int i = m_Document->m_Cards->Count - 1; i >= 0; i--)
					if (m_Document->GetCardByIndex(i)
						->m_nID != m_nTargetCard && m_CardVisible[i]) {
						if (X >= m_CardX[i] - m_CardWidth[i] / 2 && X <
							m_CardX[i] + m_CardWidth[i] / 2 && Y >=
							m_CardY[i] - m_CardHeight[i] / 2 && Y <
							m_CardY[i] + m_CardHeight[i] / 2) {

							m_nTargetCard2 =
								m_Document->GetCardByIndex(i)->m_nID;
							m_nMDownBrowserOffsetX = m_CardX[i] - X;
							m_nMDownBrowserOffsetY = m_CardY[i] - Y;
						}
					}
				m_bRedrawRequested = true;
				// PB_BrowserPaint(Sender);
			}
		}
		else {
			// シートをドラッグ中
			float ShiftX = X - m_nMDownBrowserX;
			float ShiftY = Y - m_nMDownBrowserY;

			// Client to 0-1
			ShiftX /= m_nBrowserWidth;
			ShiftY /= m_nBrowserHeight;

			// DeZoom
			float Zoom = pow(2, TB_Zoom->Position / 2000.0f);
			ShiftX /= Zoom;
			ShiftY /= Zoom;

			PB_Browser->Tag = 1; // 更新禁止
			Sc_X->Position = m_nMDownBrowserScrollX - ShiftX * 10000;
			Sc_Y->Position = m_nMDownBrowserScrollY - ShiftY * 10000;
			PB_Browser->Tag = 0; // 更新再開

			// PB_BrowserPaint(Sender);
			m_bRedrawRequested = true;
		}

		// タイトル編集を追従させる
		SetEdTitleBPos();
		break;
	case 2:
		// リンク線ドラッグ（Dest側）
		if (m_nTargetLink >= 0) {
			TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);

			m_nTargetCard2 = -1;
			for (int i = m_Document->m_Cards->Count - 1; i >= 0; i--)
				if (m_Document->GetCardByIndex(i)
					->m_nID != Link->m_nFromID && m_CardVisible[i]) {
					if (X >= m_CardX[i] - m_CardWidth[i] / 2 && X <
						m_CardX[i] + m_CardWidth[i] / 2 && Y >= m_CardY[i] -
						m_CardHeight[i] / 2 && Y < m_CardY[i] +
						m_CardHeight[i] / 2) {

						m_nTargetCard2 = m_Document->GetCardByIndex(i)->m_nID;
						m_nMDownBrowserOffsetX = m_CardX[i] - X;
						m_nMDownBrowserOffsetY = m_CardY[i] - Y;
					}
				}
			// PB_BrowserPaint(Sender);
			m_bRedrawRequested = true;
		}
		break;
	case 3:
		// リンク線ドラッグ（Source側）
		if (m_nTargetLink >= 0) {
			TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);

			m_nTargetCard2 = -1;
			for (int i = m_Document->m_Cards->Count - 1; i >= 0; i--)
				if (m_Document->GetCardByIndex(i)
					->m_nID != Link->m_nDestID && m_CardVisible[i]) {
					if (X >= m_CardX[i] - m_CardWidth[i] / 2 && X <
						m_CardX[i] + m_CardWidth[i] / 2 && Y >= m_CardY[i] -
						m_CardHeight[i] / 2 && Y < m_CardY[i] +
						m_CardHeight[i] / 2) {

						m_nTargetCard2 = m_Document->GetCardByIndex(i)->m_nID;
						m_nMDownBrowserOffsetX = m_CardX[i] - X;
						m_nMDownBrowserOffsetY = m_CardY[i] - Y;
					}
				}
			// PB_BrowserPaint(Sender);
			m_bRedrawRequested = true;
		}
		break;
	case 4:
		// Overview
		{
			// カード座標に変換
			float cx =
				(((X * 1.0f - m_nOVXOffset) / m_nOVWidth - 0.5f)
				/ OVERVIEWSIZE + 0.5f);
			float cy =
				(((Y * 1.0f - m_nOVYOffset) / m_nOVHeight - 0.5f)
				/ OVERVIEWSIZE + 0.5f);

			// このカード座標を中心に持ってくるスクロール位置を算出
			float AX = 0.5f - cx;
			float AY = 0.5f - cy;

			AX *= 0.9f;
			AY *= 0.9f;

			AX *= 10000;
			AY *= 10000;

			PB_Browser->Tag = 1; // 描画禁止
			if (m_nTargetCard >= 0 || m_nTargetLink >= 0) {
				SB_AutoScroll->Down = false;
			}
			// Sc_X->Position = 5000 - AX;
			// Sc_Y->Position = 5000 - AY;
			m_fBrowserScrollRatio = 0.0f;
			m_nScrollTargetX = 5000 - AX;
			m_nScrollTargetY = 5000 - AY;
			PB_Browser->Tag = 0; // 描画再開
			BrowserAutoScroll();
		} break;
	case 5:
		// 範囲選択
		int leftx, rightx, topy, bottomy;
		if (m_nMDownBrowserX < m_nMDownTargetX) {
			leftx = m_nMDownBrowserX;
			rightx = m_nMDownTargetX;
		}
		else {
			rightx = m_nMDownBrowserX;
			leftx = m_nMDownTargetX;
		}
		if (m_nMDownBrowserY < m_nMDownTargetY) {
			topy = m_nMDownBrowserY;
			bottomy = m_nMDownTargetY;
		}
		else {
			bottomy = m_nMDownBrowserY;
			topy = m_nMDownTargetY;
		}
		if (m_CardVisible)
			for (int i = 0; i < m_Document->m_Cards->Count; i++)
				if (m_CardVisible[i]) {
					if (m_CardX[i] - m_CardWidth[i] / 2 >= leftx && m_CardY[i] -
						m_CardHeight[i] / 2 >= topy && m_CardX[i] +
						m_CardWidth[i] / 2 <= rightx && m_CardY[i] +
						m_CardHeight[i] / 2 <= bottomy) {
						TCard *Card = m_Document->GetCardByIndex(i);
						Card->m_bSelected = true;
						if (m_nTargetCard < 0) {
							m_nFocusCursorPos = 0;
							m_nTargetCard = Card->m_nID;
						}
						if (Card->m_bLabelIsFold) {
							for (int i2 = 0;
							i2 < m_Document->m_Cards->Count; i2++)
								if (m_CardVisible[i2] && i2 != i) {
									TCard *Card2 =
										m_Document->GetCardByIndex(i2);
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
	// PB_BrowserPaint(Sender);
	if (PB_Browser->Tag == 0) {
		SB_AutoScroll->Down = false;
		m_nScrollTargetX = -65536;
		m_nScrollTargetY = -65536;
	}
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

// ---------------------------------------------------------------------------
void TFo_Main::BrowserNewChildCard(bool binverse) {
	BackupSub(PBC_NewChildCard->Caption);

	// カードからリンクを張った新規カード作成
	int lastcard = m_nTargetCard;
	int lastindex = m_Document->SearchCardIndex(lastcard);
	int insindex = lastindex + 1;
	TCard *LastCard = m_Document->GetCardByIndex(lastindex);

	if (!binverse) {
		// このカードからリンクを張っている最後のカードの次にこのカードを挿入
		for (int il = 0; il < m_Document->m_Links->Count; il++) {
			TLink *Link = m_Document->GetLinkByIndex(il);
			if (Link->m_nFromID == lastcard) {
				int index = m_Document->SearchCardIndex(Link->m_nDestID) + 1;
				if (index > insindex) {
					insindex = index;
				}
			}
		}
	}
	else {
		// シフトを押している→逆方向リンク

		// このカードへリンクを張っている最後のカードの次にこのカードを挿入
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
		// 自動整理中は親カードと同じ位置に挿入
		NewCard->m_fX = LastCard->m_fX;
		NewCard->m_fY = LastCard->m_fY;
	}
	else {
		// 自動整理で無い場合は、元カードと近い位置に挿入
		// リンクしているカードの平均の長さを求める
		int count = 0;
		float r = 0.0f;
		for (int i = 0; i < m_Document->m_Links->Count; i++) {
			TLink *Link = m_Document->GetLinkByIndex(i);
			if (Link->m_nDestID == lastcard || Link->m_nFromID == lastcard) {
				TCard *Card;
				if (Link->m_nDestID == lastcard) {
					Card = m_Document->GetCard(Link->m_nFromID);
				}
				else {
					Card = m_Document->GetCard(Link->m_nDestID);
				}
				r += sqrt((Card->m_fX - LastCard->m_fX) * (Card->m_fX -
					LastCard->m_fX) + (Card->m_fY - LastCard->m_fY) *
					(Card->m_fY - LastCard->m_fY));
				count++;
			}
		}
		if (count) {
			r /= count;
		}
		// ディスプレイ上の64ドット分を求める
		float cx1, cx2, dummy;
		DispPosToCardPos(0, 0, cx1, dummy);
		DispPosToCardPos(64, 0, cx2, dummy);
		float r2 = fabs(cx2 - cx1);
		if (r < r2) {
			r = r2;
		}
		else if (r > r2 * 3) {
			r = r2 * 3;
		}
		float rbak = r; // 元のカードから離す距離の初期値
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
				if (Card != NewCard && (Card->m_fX - NewCard->m_fX) *
					(Card->m_fX - NewCard->m_fX) + (Card->m_fY - NewCard->m_fY)
					* (Card->m_fY - NewCard->m_fY) < (rbak * rbak / 4.0f)) {
					found = true;
				}
			}
			if (!found) {
				break;
			}
			else {
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
		// 子ノード
		Link->m_nFromID = lastcard;
		Link->m_nDestID = m_nTargetCard;
	}
	else {
		// シフトが押されていたら逆方向リンク
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
	}
	else {
		Ed_Title->SelectAll();
		Ed_Title->SetFocus();
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserNewBrotherCard() {
	int ParentID = m_Document->SearchParent(m_nTargetCard, false, false);

	if (ParentID >= 0) {
		BackupSub(PBC_NewBrotherCard->Caption);

		// 親カードが見つかった
		// 親カードからリンクを張った新規カード作成
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
		}
		else {
			// 自動整理で無い場合は、親カードと近い位置に挿入
			TCard *ParentCard = m_Document->GetCard(ParentID);
			// リンクしているカードの平均の長さを求める
			int count = 0;
			float r = 0.0f;
			for (int i = 0; i < m_Document->m_Links->Count; i++) {
				TLink *Link = m_Document->GetLinkByIndex(i);
				if (Link->m_nDestID == ParentID || Link->m_nFromID == ParentID)
				{
					TCard *Card;
					if (Link->m_nDestID == ParentID) {
						Card = m_Document->GetCard(Link->m_nFromID);
					}
					else {
						Card = m_Document->GetCard(Link->m_nDestID);
					}
					r += sqrt((Card->m_fX - ParentCard->m_fX) * (Card->m_fX -
						ParentCard->m_fX) + (Card->m_fY - ParentCard->m_fY) *
						(Card->m_fY - ParentCard->m_fY));
					count++;
				}
			}
			if (count) {
				r /= count;
			}
			// ディスプレイ上の64ドット分を求める
			float cx1, cx2, dummy;
			DispPosToCardPos(0, 0, cx1, dummy);
			DispPosToCardPos(64, 0, cx2, dummy);
			float r2 = fabs(cx2 - cx1);
			if (r < r2) {
				r = r2;
			}
			float rbak = r; // 元のカードから離す距離の初期値
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
					if (Card != NewCard && (Card->m_fX - NewCard->m_fX) *
						(Card->m_fX - NewCard->m_fX) +
						(Card->m_fY - NewCard->m_fY) * (Card->m_fY -
						NewCard->m_fY) < (rbak * rbak / 4.0f)) {
						found = true;
					}
				}
				if (!found) {
					break;
				}
				else {
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
		}
		else {
			Ed_Title->SelectAll();
			Ed_Title->SetFocus();
		}
	}
	else {
		// 親カードが見つからなかった

		// カード作成しない

		/*
		 //通常の新規カード作成
		 MI_NewCardClick(Sender);
		 */
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::BrowserInsertCardToLink() {
	BackupSub(PBL_InsertCard->Caption);

	// リンクの間に新カード挿入
	TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
	int DestID = Link->m_nDestID; // リンク先ID
	int lastcard = Link->m_nFromID;
	int lastindex = m_Document->SearchCardIndex(Link->m_nFromID);
	int insindex = lastindex + 1;
	TCard *DestCard = m_Document->GetCard(DestID); // 元のリンク先カード
	TCard *LastCard = m_Document->GetCardByIndex(lastindex); // 元のリンク元カード

	// リンク元カードからリンクを張っている最後のカードの次にこのカードを挿入
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

	// 新カードの座標をリンクの中心に
	NewCard->m_fX = (LastCard->m_fX + DestCard->m_fX) * 0.5f;
	NewCard->m_fY = (LastCard->m_fY + DestCard->m_fY) * 0.5f;

	// 元リンクのDestを新カードに変更
	Link->m_nDestID = NewCard->m_nID;

	m_nTargetCard = NewCard->m_nID;
	m_nTargetLink = -1;

	// 新リンク
	TLink *Link2 = m_Document->NewLink();
	Link2->m_nFromID = NewCard->m_nID;
	Link2->m_nDestID = DestID;
	for (int il = 0; il < Link->m_Labels->Count; il++) { // ラベル引継ぎ
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
	}
	else {
		Ed_Title->SelectAll();
		Ed_Title->SetFocus();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift) {
	// M_File->Caption = Key;
	// M_File->Caption = (int)Active;
	if (Key >= 37 && Key <=
		40 && PC_Client->ActivePage == TS_Browser && m_CardVisible &&
		TB_Zoom->Focused()) {
		// if (Key >= 37 && Key <= 40 && PC_Client->ActivePage == TS_Browser && m_CardVisible && (TB_Zoom->Focused() || !Active || Fo_FullScreen->Visible)){
		m_bSkipAutoZoom = true; // カーソルでAutoZoomが消えないようにするため
		// カーソルでカード移動

		if (!Shift.Contains(ssCtrl)) {
			// Ctrlキーが押されていない（上下左右の近いカードに移動）

			float sx = 0.5f * m_nBrowserWidth, sy = 0.5f * m_nBrowserHeight;
			if (m_nTargetCard >= 0) {
				// カード選択中
				int index = m_Document->SearchCardIndex(m_nTargetCard);
				if (index >= 0) {
					sx = m_CardX[index];
					sy = m_CardY[index];
				}
			}

			float mindist = -1.0f;
			int minindex = -1;
			if (!Shift.Contains(ssShift) || m_nTargetCard < 0) {
				// リンクとは無関係に移動
				for (int i = 0; i < m_Document->m_Cards->Count; i++)
					if (m_CardVisible[i]) {
						TCard *Card = m_Document->GetCardByIndex(i);
						if (Card->m_bGetFocus) {
							float md =
								MoveDistance(sx, sy, m_CardX[i], m_CardY[i],
								Key - 37);
							if (md > 0.0f && (mindist < 0.0f || md < mindist)) {
								mindist = md;
								minindex = i;
							}
						}
					}
			}
			else {
				// リンクされているカードに移動
				for (int i = 0; i < m_Document->m_Links->Count; i++)
					if (m_LinkVisible[i]) {
						TLink *Link = m_Document->GetLinkByIndex(i);
						int index = -1;
						if (Link->m_nFromID == m_nTargetCard) {
							index = m_Document->SearchCardIndex
								(Link->m_nDestID);
						}
						if (Link->m_nDestID == m_nTargetCard) {
							index = m_Document->SearchCardIndex
								(Link->m_nFromID);
						}
						if (index >= 0) {
							TCard *Card = m_Document->GetCardByIndex(index);
							if (Card->m_bGetFocus) {
								float md =
									MoveDistance(sx, sy, m_CardX[index],
									m_CardY[index], Key - 37);
								if (md > 0.0f && (mindist < 0.0f || md <
									mindist)) {
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
		}
		else {
			// Ctrlキーが押されている。

			// 親ノードを探す
			int parent = m_Document->SearchParent(m_nTargetCard,
			Key == 40, true);

			if (parent >= 0)
				switch (Key) {
				case 38: // 上＝親ノードへ（BSと同じ）
				case 40: // 下＝子ノードへ
					{
						m_nTargetCard = parent;
						m_Document->ClearCardSelection();
						TCard *Card = m_Document->GetCardByIndex(parent);
						Card->m_bSelected = true;
					} break;
				case 37: // 時計回りに一番近い兄弟ノードを探す
				case 39: // 反時計回りに一番近い兄弟ノードを探す
					{
						int card = m_Document->SearchBrother(m_nTargetCard,
							parent, Key == 39, false, true);
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
		// if (Key == 8 && PC_Client->ActivePage == TS_Browser && m_CardVisible && (TB_Zoom->Focused() || !Active)){
		/*
		 //直前に表示していたカードに戻る
		 //辿っている間ににViewedが更新されてうまくいかない！
		 int last = m_Document->SearchLast(m_nTargetCard);
		 if (last > 0){
		 m_nTargetCard = last;
		 }
		 */

		// 親ノードに戻る
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
		// ブラウザ画面
		if (Key == 46 && TB_Zoom->Focused()) {
			// Delete
			if (m_nTargetCard >= 0) {
				// カードを削除
				PL_DeleteCardClick(Sender);
				/*
				 m_Document->DeleteCard(m_nTargetCard);
				 m_nTargetCard = -1;
				 m_nTargetCard2 = -1;
				 */
			}
			else if (m_nTargetLink >= 0) {
				// リンクを削除
				BackupSub(PI_DeleteLink->Caption);
				m_Document->DeleteLinkByIndex(m_nTargetLink);
				m_nTargetLink = -1;
			}
			PB_BrowserMouseUp(Sender, mbLeft, TShiftState(), 0, 0);
			Key = 0;
		}
		else if (Key == 32) {
			// Space
			if (!m_bShowRecent) { // キーリピートで何度も現在位置が表示されないように
				m_bShowRecent = true;

				// 現在位置も表示
				m_nLastTarget = -1;
				m_nFocusCursorPos = 0;
				m_bRedrawRequested = true;
			}
		}
	}
	if (Key == 46 && LB_List->Focused()) {
		// Delete
		if (m_nTargetCard >= 0) {
			// カードを削除
			PL_DeleteCardClick(Sender);
		}
		Key = 0;
	}

	if (Key == 27 && Fo_FullScreen->Visible) { // ESC
		ExitFullScreen();
		Key = 0;
	}

	if (Key == 45 && PC_Client->ActivePage == TS_Browser) {
		// BrowserでInsertキー（子ノード作成）

		if (m_nTargetCard >= 0) {
			// カード選択中

			// 子（もしくは親）ノード作成
			BrowserNewChildCard(Shift.Contains(ssShift));
			Key = 0;
		}
		else if (m_nTargetLink >= 0) {
			// リンク選択中

			// リンクの間に新カードを挿入
			BrowserInsertCardToLink();
			Key = 0;
		}
		else {
			// カード選択中でもリンク選択中でも無い

			// カード作成しない

			/*
			 //通常の新規カード作成
			 MI_NewCardClick(Sender);
			 */
		}
	}

	// M_File->Caption = Key;
	if (PC_Client->ActivePage == TS_Browser && TB_Zoom->Focused() &&
		((Key >= 48 && Key <= 90) || (Key >= 96 && Key <= 111) || (Key >=
		186 && Key <= 226)) && !Ed_TitleB->Visible && !m_bTitleEditRequested &&
		!Shift.Contains(ssAlt) && !Shift.Contains(ssCtrl)
		// !Shift.Contains(ssShift) &&
		) {
		if (m_nTargetLink >= 0 && !Ed_LinkTitle->Focused()) {
			Ed_LinkTitle->SelectAll();
			Ed_LinkTitle->SetFocus();
			keybd_event(Key, 0, 0, 0);
			keybd_event(Key, 0, KEYEVENTF_KEYUP, 0);
		}
		else {
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
		// 図形描画画面
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
	if (UD_Size->Position > SettingView.m_Font->Size && last <
		SettingView.m_Font->Size) {
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
	if (UD_Size->Position < SettingView.m_Font->Size && last >
		SettingView.m_Font->Size) {
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
		// カード選択中
		insindex = m_Document->SearchCardIndex(m_nTargetCard);

		if (Sender != PL_NewCard) {
			// カードリストのポップアップメニュー以外から挿入
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
	}
	else {
		Ed_Title->SelectAll();
		Ed_Title->SetFocus();
	}
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
	Fo_Search->Ch_Backward->Checked = Sender == (TObject*)ME_FindPrevious;
	Fo_Search->Ch_GlobalSearch->Checked =
		Sender != (TObject*)ME_FindNext && Sender != (TObject*)ME_FindPrevious;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_FindNextClick(TObject *Sender) {
	if (SearchKeyword != WideString("")) {
		if (PC_Client->ActivePage == TS_Browser) {
			bSearchRequest = (bLastSearchRequest & 0x30) | 0x01;
		}
		else {
			bSearchRequest = (bLastSearchRequest & 0x30) | 0x03;
		}
	}
	else {
		if (bLastSearchRequest & 0x100) {
			ME_ReplaceClick(Sender);
		}
		else {
			ME_FindClick(Sender);
		}
	}
}

// ---------------------------------------------------------------------------

void __fastcall TFo_Main::ME_FindPreviousClick(TObject *Sender) {
	if (SearchKeyword != WideString("")) {
		if (PC_Client->ActivePage == TS_Browser) {
			bSearchRequest = (bLastSearchRequest & 0x30) | 0x81;
		}
		else {
			bSearchRequest = (bLastSearchRequest & 0x30) | 0x83;
		}
	}
	else {
		ME_FindClick(Sender);
	}
}
// ---------------------------------------------------------------------------

void TFo_Main::GlobalSearch(int SearchRequest) {
	// ヒットした結果をm_GlobalSearchResultに入れる
	m_GlobalSearchResult->Clear();

	if (m_Document->m_Cards->Count == 0) {
		// カードが無い
		return;
	}

	Screen->Cursor = crHourGlass;

	for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
		TCard *Card = m_Document->GetCardByIndex(ic);
		int foundat = 0;
		if (!Card->m_bVisible && !(SearchRequest & 0x20)) {
			// 非表示かつ、非表示カードが検索対称に入っていない
		}
		else {
			if (SearchRequest & 0x1) {
				// タイトルから検索
				if (SearchRequest & 0x10) {
					// 大文字と小文字を区別しない
					WideString Target =
						WideLowerCase(WideString(Card->m_Title));
					foundat = Target.Pos(WideLowerCase(SearchKeyword));
				}
				else {
					// 大文字と小文字を区別
					foundat = WideString(Card->m_Title).Pos(SearchKeyword);
				}
			}
			if (foundat == 0 && SearchRequest & 0x2) {
				// 本文から検索
				if (SearchRequest & 0x10) {
					WideString Target =
						WideLowerCase(WideString(Card->m_Lines->Text));
					foundat = Target.Pos(WideLowerCase(SearchKeyword));
				}
				else {
					foundat = WideString(Card->m_Lines->Text).Pos
						(SearchKeyword);
				}
			}
		}

		if (foundat > 0) {
			// 見つかった
			m_GlobalSearchResult->Add((void*)Card->m_nID);
		}
	}

	Screen->Cursor = crDefault;
}

// ---------------------------------------------------------------------------
int TFo_Main::ReplaceAll(int SearchRequest) {
	if (m_Document->m_Cards->Count == 0) {
		// カードが無い
		return false;
	}

	Screen->Cursor = crHourGlass;

	int Replaced = 0;
	bool Backup = false;

	for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
		TCard *Card = m_Document->GetCardByIndex(ic);
		int foundat;
		if (!Card->m_bVisible && !(SearchRequest & 0x20)) {
			// 非表示かつ、非表示カードが検索対称に入っていない
		}
		else {
			if (SearchRequest & 0x1) {
				int lastpos = 1;
				// タイトルから検索
				WideString S = Card->m_Title;
				while (true) {
					if (SearchRequest & 0x10) {
						// 大文字と小文字を区別しない
						WideString Target = WideLowerCase(S);
						Target = Target.SubString(lastpos, Target.Length());
						foundat = Target.Pos(WideLowerCase(SearchKeyword)) +
							lastpos - 1;
					}
					else {
						// 大文字と小文字を区別
						WideString Target = S.SubString(lastpos, S.Length());
						foundat = Target.Pos(SearchKeyword) + lastpos - 1;
					}
					if (foundat < lastpos) {
						break;
					}
					else {
						S = S.SubString(1, foundat - 1) + ReplaceKeyword +
							S.SubString(foundat + SearchKeyword.Length(),
							S.Length());
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
				// 本文から検索
				WideString S = Card->m_Lines->Text;
				while (true) {
					if (SearchRequest & 0x10) {
						WideString Target = WideLowerCase(S);
						Target = Target.SubString(lastpos, Target.Length());
						foundat = Target.Pos(WideLowerCase(SearchKeyword)) +
							lastpos - 1;
					}
					else {
						WideString Target = S.SubString(lastpos, S.Length());
						foundat = Target.Pos(SearchKeyword) + lastpos - 1;
					}
					if (foundat < lastpos) {
						break;
					}
					else {
						S = S.SubString(1, foundat - 1) + ReplaceKeyword +
							S.SubString(foundat + SearchKeyword.Length(),
							S.Length());
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
		// カードが無い
		return;
	}

	m_bSearching = true;

	Screen->Cursor = crHourGlass;

	// 検索ダイアログからフォーカスを奪う
	SetFocus();

	int targetcard = m_nTargetCard;
	if (targetcard < 0) {
		// カードが選択されていない

		// 最初のカードを選択
		targetcard = m_Document->GetCardByIndex(0)->m_nID;
	}

	// 検索開始場所
	int StartTarget = 0;
	int StartPos = 0;
	if (RE_Edit->Focused()) {
		// 文章編集中
		StartTarget = 1;
		if ((SearchRequest & 0x100) && ((GetSelText() == SearchKeyword) ||
			((SearchRequest & 0x10) && (WideLowerCase(GetSelText())
			== WideLowerCase(SearchKeyword))))) {
			// 置換
			BackupSub(ME_Replace->Caption);
			WideString S = RE_Edit->Text;
			int selstartbak = RE_Edit->SelStart;
			RE_Edit->Text = S.SubString(1, RE_Edit->SelStart) + ReplaceKeyword +
				S.SubString(1 + RE_Edit->SelStart + RE_Edit->SelLength,
				S.Length());
			RE_Edit->SelStart = selstartbak;
			RE_Edit->SelLength = ReplaceKeyword.Length();
		}
		if (SearchRequest & 0x80) {
			// 後ろ向き検索
			StartPos = RE_Edit->SelStart;
			/*
			 if (RE_Edit->SelLength){
			 StartPos = RE_Edit->SelStart;
			 }else{
			 StartPos = (RE_Edit->Text).Length();
			 }
			 */
		}
		else {
			// 前向き検索
			StartPos = RE_Edit->SelStart + RE_Edit->SelLength;
			/*
			 if (RE_Edit->SelLength){
			 StartPos = RE_Edit->SelStart + RE_Edit->SelLength;
			 }else{
			 StartPos = 0;
			 }
			 */
		}
	}
	else if (m_nTargetCard >= 0) {
		// カード選択中
		StartTarget = 0;
		if ((SearchRequest & 0x100) && ((GetSelText() == SearchKeyword) ||
			((SearchRequest & 0x10) && (WideLowerCase(GetSelText())
			== WideLowerCase(SearchKeyword))))) {
			// 置換
			BackupSub(ME_Replace->Caption);
			WideString S = Ed_Title->Text;
			int selstartbak = Ed_Title->SelStart;
			Ed_Title->Text = S.SubString(1, Ed_Title->SelStart) +
				ReplaceKeyword + S.SubString
				(1 + Ed_Title->SelStart + Ed_Title->SelLength, S.Length());
			Ed_Title->SelStart = selstartbak;
			Ed_Title->SelLength = ReplaceKeyword.Length();
		}
		if (SearchRequest & 0x80) {
			// 後ろ向き検索
			StartPos = Ed_Title->SelStart;
			/*
			 if (Ed_Title->SelLength){
			 StartPos = Ed_Title->SelStart;
			 }else{
			 StartPos = WideString(Ed_Title->Text).Length();
			 }
			 */
		}
		else {
			StartPos = Ed_Title->SelStart + Ed_Title->SelLength;
			/*
			 //前向き検索
			 if (Ed_Title->SelLength){
			 StartPos = Ed_Title->SelStart + Ed_Title->SelLength;
			 }else{
			 StartPos = 0;
			 }
			 */
		}
	}

	// 見つかったかどうか
	bool Found = false;
	bool NotFound = false; // 1週して見つからなかったかどうか

	// 開始カード、ターゲット、場所
	int CardIndex = m_Document->SearchCardIndex(targetcard);
	int Target = StartTarget;
	int Pos = StartPos;
	// 検索開始カード
	int StartCard = CardIndex;

	// RichEditの場合
	// TRichEdit *RE = new TRichEdit(this);

	TMemo *RE = new TMemo(this);
	RE->Visible = false;
	RE->Parent = this;

	WideString Key;
	if (SearchRequest & 0x10) {
		// 大文字と小文字を区別しない
		Key = WideLowerCase(SearchKeyword);
	}
	else {
		// 大文字と小文字を区別
		Key = SearchKeyword;
	}

	bool resetpos = false;

	while (true) {
		TCard *Card = m_Document->GetCardByIndex(CardIndex);
		// 現在のTargetの現在位置から検索
		int foundat = -1;
		if (!Card->m_bVisible && !(SearchRequest & 0x20)) {
			// 非表示かつ、非表示カードが検索対称に入っていない
		}
		else if (Target == 0) {
			if (SearchRequest & 0x1) {
				// タイトルから検索
				int ToEnd;
				WideString Target;
				if (SearchRequest & 0x10) {
					// 大文字と小文字を区別しない
					Target = WideLowerCase(WideString(Card->m_Title));
				}
				else {
					// 大文字と小文字を区別
					Target = Card->m_Title;
				}
				if (SearchRequest & 0x80) {
					// 後ろ向き検索
					if (resetpos) {
						Pos = Target.Length();
					}
					int pos2 = 1, len = Pos;
					while (true) {
						int at = Target.SubString(pos2, len).Pos(Key);
						if (at) {
							foundat = at + pos2 - 1;
							pos2 += at;
							len -= at;
						}
						else {
							break;
						}
					}
					if (foundat <= 0) {
						foundat = -1;
					}
				}
				else {
					// 前向き検索
					if (resetpos) {
						Pos = 0;
					}
					ToEnd = Target.Length() - Pos;
					foundat = Target.SubString(Pos + 1, ToEnd).Pos(Key);
					if (foundat > 0) {
						foundat += Pos;
					}
					else {
						foundat = -1;
					}
				}
				if (foundat >= 0) {
					// 見つかった
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
		}
		else {
			if (SearchRequest & 0x2) {
				// 本文から検索
				RE->Lines->Assign(Card->m_Lines);
				int ToEnd;
				WideString Target;
				if (SearchRequest & 0x10) {
					// 大文字と小文字を区別しない
					Target = WideLowerCase(WideString(RE->Text));
				}
				else {
					// 大文字と小文字を区別
					Target = RE->Text;
				}
				if (SearchRequest & 0x80) {
					// 後ろ向き検索
					if (resetpos) {
						Pos = Target.Length();
					}
					int pos2 = 1, len = Pos;
					while (true) {
						int at = Target.SubString(pos2, len).Pos(Key);
						if (at) {
							foundat = at + pos2 - 1;
							pos2 += at;
							len -= at;
						}
						else {
							break;
						}
					}
					if (foundat <= 0) {
						foundat = -1;
					}
				}
				else {
					// 前向き検索
					if (resetpos) {
						Pos = 0;
					}
					ToEnd = Target.Length() - Pos;
					foundat = Target.SubString(Pos + 1, ToEnd).Pos(Key);
					if (foundat > 0) {
						foundat += Pos;
					}
					else {
						foundat = -1;
					}
				}

				// RichEditの場合
				// int ToEnd = RE->Text.Length() - Pos;
				// foundat = RE->FindTextA(SearchKeyword, Pos, ToEnd, TSearchTypes() << stMatchCase);

				if (foundat >= 0) {
					// 見つかった
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

		// 見つからなかったので次のカード、ターゲットへ
		resetpos = true;
		Target = 1 - Target;
		if (Target == 1 && (SearchRequest & 0x80)) {
			// 後ろ向きサーチ
			// 前のカードを選択
			CardIndex = (CardIndex + m_Document->m_Cards->Count - 1)
				% m_Document->m_Cards->Count;
		}
		else if (Target == 0 && !(SearchRequest & 0x80)) {
			// 前向きサーチ
			// 次のカードを選択
			CardIndex = (CardIndex + 1) % m_Document->m_Cards->Count;
		}

		// 全体を通して見つからなかったチェック
		if (NotFound) {
			break;
		}
		if (CardIndex == StartCard && Target == StartTarget) {
			// 見つからなかった
			NotFound = true;
		}
	}

	delete RE;

	Screen->Cursor = crDefault;

	if (!Found) {
		ShowMessage(UnicodeString("\"") + SearchKeyword + UnicodeString("\"") +
			MLText.NotFound);
	}

	// 更新再開
	/*
	 LB_List->Perform(WM_SETREDRAW, 1, 0);
	 Ed_Title->Perform(WM_SETREDRAW, 1, 0);
	 Co_Label->Perform(WM_SETREDRAW, 1, 0);
	 RE_Edit->Perform(WM_SETREDRAW, 1, 0);
	 LB_Link->Perform(WM_SETREDRAW, 1, 0);
	 */
	/*
	 Perform(WM_SETREDRAW, 1, 0);

	 //描画
	 //Invalidate();//なぜかRedrawされない
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
	// 現在選択中のテキストを得る

	WideString Key;
	int SelStart = 0, SelLength = 0;
	if (Ed_FindCard->Focused()) {
		Key = Ed_FindCard->Text;
		SelStart = Ed_FindCard->SelStart;
		SelLength = Ed_FindCard->SelLength;
	}
	else if (RE_Edit->Focused()) {
		Key = RE_Edit->Text;
		SelStart = RE_Edit->SelStart;
		SelLength = RE_Edit->SelLength;
	}
	else if (Me_EditorRelated->Focused()) {
		Key = Me_EditorRelated->Text;
		SelStart = Me_EditorRelated->SelStart;
		SelLength = Me_EditorRelated->SelLength;
	}
	else if (Ed_Title->Focused()) {
		Key = Ed_Title->Text;
		SelStart = Ed_Title->SelStart;
		SelLength = Ed_Title->SelLength;
	}
	else if (Ed_TitleB->Focused()) {
		Key = Ed_TitleB->Text;
		SelStart = Ed_TitleB->SelStart;
		SelLength = Ed_TitleB->SelLength;
	}
	else if (Ed_LinkTitle->Focused()) {
		Key = Ed_LinkTitle->Text;
		SelStart = Ed_LinkTitle->SelStart;
		SelLength = Ed_LinkTitle->SelLength;
	}
	else if (m_nTargetCard >= 0) {
		TCard *Card = m_Document->GetCard(m_nTargetCard);
		if (Card) {
			Key = Card->m_Title;
			SelStart = 0;
			SelLength = Key.Length();
		}
	}
	else if (m_nTargetLink >= 0) {
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

	TMenuItem *MI = (TMenuItem*)Sender;

	WideString Key = GetSelText();

	UnicodeString S = Setting2Function.m_WebSearch->Values
		[Setting2Function.m_WebSearch->Names[MI->Tag]] + TIdURI::URLEncode(Key);
	UnicodeString S2 = "";
	ShellExecute(Handle, NULL, S.c_str(), S2.c_str(), NULL, SW_SHOW);

	// ショートカット更新
	SettingFile.m_WebSearch = Setting2Function.m_WebSearch->Names[MI->Tag];
	for (int i = 0; i < Setting2Function.m_WebSearch->Count; i++) {
		TMenuItem *MI = (TMenuItem*)MI_WebSearch->Items[i];
		if (SettingFile.m_WebSearch == Setting2Function.m_WebSearch->Names
			[MI->Tag]) {
			MI->ShortCut = TextToShortCut("Ctrl+W");
		}
		else {
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
	BackupSub(MD_SortCard->Caption + UnicodeString(" ") + ((TMenuItem*)Sender)
		->Caption);
	m_Document->SortCards(0, ((TMenuItem*)Sender)->Tag);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MDS_Date_CreatedClick(TObject *Sender) {
	BackupSub(MD_SortCard->Caption + UnicodeString(" ") + ((TMenuItem*)Sender)
		->Caption);
	m_Document->SortCards(1, ((TMenuItem*)Sender)->Tag);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MDS_Date_EditedClick(TObject *Sender) {
	BackupSub(MD_SortCard->Caption + UnicodeString(" ") + ((TMenuItem*)Sender)
		->Caption);
	m_Document->SortCards(2, ((TMenuItem*)Sender)->Tag);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MDS_Date_ViewedClick(TObject *Sender) {
	BackupSub(MD_SortCard->Caption + UnicodeString(" ") + ((TMenuItem*)Sender)
		->Caption);
	m_Document->SortCards(3, ((TMenuItem*)Sender)->Tag);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_NewLabelClick(TObject *Sender) {
	if (m_nTargetCard >= 0) {
		Fo_EditText = new TFo_EditText(this);
		Fo_EditText->La_Text->Caption = MLText.LabelName + ":";
		Fo_EditText->Caption = DeleteActionKey(MI_NewLabel->Caption);
		Fo_EditText->ShowModal();
		if (Fo_EditText->ModalResult == mrOk &&
			Fo_EditText->Ed_Text->Text != "") {
			BackupSub(MI_NewLabel->Caption);

			TCardLabel *Label = m_Document->NewLabel(0);
			Label->m_Name = Fo_EditText->Ed_Text->Text;
			Label->m_nColor = GetColor(rand() % 10001 / 10000.0, 0, 255);

			/*
			 TCard *Card = m_Document->GetCard(m_nTargetCard);
			 Card->m_Labels->AddLabel(m_Document->m_Labels[0]->Count);
			 */
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				TCard *Card = m_Document->GetCardByIndex(i);
				if (Card->m_bSelected) {
					// Add
					Card->m_Labels->AddLabel(m_Document->m_Labels[0]->Count);
				}
			}
		}
		Fo_EditText->Release();
	}
}
// ---------------------------------------------------------------------------

void TFo_Main::Redraw() {
	// 全体再描画
	if (PC_Client->ActivePage == TS_Browser) {
		PB_BrowserPaint(this);
	}
	RefreshLabel();
}

// ---------------------------------------------------------------------------
void TFo_Main::LinkLimitationSub(int targetcard, int targetlink, int linkdepth,
	int *Score, bool linkdirection, bool linkbackward) {
	int *IFound = new int[m_Document->m_Cards->Count];
	memset(IFound, 0, sizeof(int) * m_Document->m_Cards->Count);

	int startingscore = linkdepth < 11 ? linkdepth + 1 : 0xffff;

	if (targetcard >= 0) {
		// カードから辿る
		int idx = m_Document->SearchCardIndex(targetcard);
		if (idx >= 0) {
			Score[idx] = startingscore;
		}
	}
	else if (targetlink >= 0) {
		// リンクから辿る
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

	// リンク回数ループ
	bool found = true;
	while (found)
	{ // for (int i = 0 ; i < SettingView.m_nLinkLimitation ; i++){
		found = false;

		// ループ
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
						if (Link->m_nFromID == id && (!linkdirection ||
							!linkbackward || !Link->m_bDirection)) {
							targetidx =
								m_Document->SearchCardIndex(Link->m_nDestID);
						}
						else if (Link->m_nDestID == id && (!linkdirection ||
							linkbackward || !Link->m_bDirection)) {
							targetidx =
								m_Document->SearchCardIndex(Link->m_nFromID);
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

	delete[]IFound;
}

// ---------------------------------------------------------------------------
void TFo_Main::SetCardVisible(bool bFoldTree) {
	// 表示するカード決定

	if (m_nAnimation > 0 && m_nAnimation != 3) {
		// ランダムカードの場合Visibleを設定しない
		return;
	}

	// まずは全て表示
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		Card->m_bVisibleBak = Card->m_bVisible;
		Card->m_bVisible = true;
	}
	for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++) {
		m_Document->GetLinkByIndex(i2)->m_bVisible = true;
	}

	// ラベル、サイズ、日付でフィルタリング
	FilteringCard();

	// リンク数でフィルタリング
	if (SettingView.m_bLinkLimitation && (m_nTargetCard >=
		0 || m_nTargetLink >= 0)) {
		// リンク数制限あり＆カードかリンクが選択されている
		/*
		 //とりあえず全非表示
		 for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
		 m_Document->GetCardByIndex(i)->m_bVisible = false;
		 }
		 */

		int *Score = new int[m_Document->m_Cards->Count];
		memset(Score, 0, sizeof(int) * m_Document->m_Cards->Count);
		/*
		 int *IFound = new int[m_Document->m_Cards->Count];
		 memset(IFound, 0, sizeof(int) * m_Document->m_Cards->Count);

		 int startingscore = SettingView.m_nLinkLimitation < 11 ? SettingView.m_nLinkLimitation + 1 : 0xffff;

		 if (m_nTargetCard >= 0){
		 //カードから辿る
		 int idx = m_Document->SearchCardIndex(m_nTargetCard);
		 Score[idx] = startingscore;
		 }else if (m_nTargetLink >= 0){
		 //リンクから辿る
		 TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
		 int idx;
		 if (!SettingView.m_bLinkDirection || SettingView.m_bLinkBackward || !Link->m_bDirection){
		 idx = m_Document->SearchCardIndex(Link->m_nFromID);
		 Score[idx] = startingscore;
		 }
		 if (!SettingView.m_bLinkDirection || !SettingView.m_bLinkBackward || !Link->m_bDirection){
		 idx = m_Document->SearchCardIndex(Link->m_nDestID);
		 Score[idx] = startingscore;
		 }
		 }

		 //リンク回数ループ
		 bool found = true;
		 while (found){//for (int i = 0 ; i < SettingView.m_nLinkLimitation ; i++){
		 found = false;

		 //ループ
		 for (int ic = 0 ; ic < m_Document->m_Cards->Count ; ic++) if (Score[ic] > 1 && !IFound[ic]){
		 IFound[ic] = 1;
		 TCard *Card = m_Document->GetCardByIndex(ic);
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
		// 現在カードから指定リンク数で繋がるカードのみ表示
		LinkLimitationSub(m_nTargetCard, m_nTargetLink,
			SettingView.m_nLinkLimitation, Score, SettingView.m_bLinkDirection,
			SettingView.m_bLinkBackward);

		if (SettingView.m_nLinkTarget >= 0) {
			// TargetCardまでの間にあるリンクだけ表示
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
						 //カードタイトルに反映してデバッグ
						 TCard *Card_ = m_Document->GetCardByIndex(ic);
						 Card_->m_Title = IntToStr(Score[ic]) + " : " + IntToStr(Score2[ic]);
						 */
						if (Score2[ic] == 0) {
							Score[ic] = 0;
						}
						else if (SettingView.m_nLinkLimitation <
							11 && Score[ic] + Score2[ic] >
							SettingView.m_nLinkLimitation + 2) {
							Score[ic] = 0;
						}
					}
					delete[]Score2;
				}
				else {
					// 指定リンク数ではTargetCardに達することがないので、全て非表示
					for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
						Score[ic] = 0;
					}
				}
			}
			else {
				// TargetCardが見つからない

				// TargetCardをリセットする
				SettingView.m_nLinkTarget = -1;
			}

			// 本当は、ここで絞り込まれたカードからさらに指定リンクで辿れるルートかを判断する必要あり？
		}

		for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
			TCard *Card = m_Document->GetCardByIndex(ic);
			Card->m_bVisible &= Score[ic] > 0;
		}

		delete[]Score;

		// カードタイトルに反映してデバッグ
		// m_Document->RefreshList();

	} /* else{
	 //リンク数制限なしorカードが選択されていない（全て表示）
	 for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
	 m_Document->GetCardByIndex(i)->m_bVisible = true;
	 }
	 } */

	// 階層表示時、折りたたまれているより下のカードを非常時にする
	int changed = true;
	while (bFoldTree && changed) {
		changed = false;
		for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
			TCard *Card = m_Document->GetCardByIndex(ic);
			if (Card->m_bVisible && Card->m_nParentID >= 0) {
				// 表示中で、親ノードがある
				TCard *Card2 = m_Document->GetCard(Card->m_nParentID);
				if (Card2->m_bFold || !Card2->m_bVisible) {
					Card->m_bVisible = false;
					m_CardVisible[ic] = false;
					changed = true;
				}
			}
		}
	}

	// 選択中カードを見せる
	if (m_nTargetCard >= 0) {
		TCard *Card = m_Document->GetCard(m_nTargetCard);
		if (Card) {
			Card->m_bVisible = true;
		}
	}

	// 選択中リンクの両端のカードを見せる
	if (m_nTargetLink >= 0) {
		TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
		if (Link) {
			m_Document->GetCard(Link->m_nFromID)->m_bVisible = true;
			m_Document->GetCard(Link->m_nDestID)->m_bVisible = true;
		}
	}

	// 表示状態に変化があった場合の処理
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		if (Card->m_bVisibleBak != Card->m_bVisible) {
			// Ticker位置をリセット
			Card->m_fTickerPos = 0.0;
		}
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::FilteringCard() {
	// Labelで表示するカードを選別する

	// ラベル(Show)
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		if (Card->m_bVisible) {
			if (Card->m_Labels->Count) {
				bool visible = false;
				for (int il = 0; il < Card->m_Labels->Count && !visible; il++) {
					visible |= m_Document->GetLabelByIndex(0,
						Card->m_Labels->GetLabel(il) - 1)->m_bShow;
				}
				Card->m_bVisible &= visible;
			}
			else {
				Card->m_bVisible &= SettingView.m_bNoLabelVisible;
			}
		}
	}

	// ラベル(Hide)
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		if (Card->m_bVisible) {
			if (Card->m_Labels->Count) {
				for (int il = 0;
				il < Card->m_Labels->Count && Card->m_bVisible; il++) {
					Card->m_bVisible &=
						!m_Document->GetLabelByIndex(0,
						Card->m_Labels->GetLabel(il) - 1)->m_bHide;
				}
			}
		}
	}

	// ラベル（Link)
	for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++) {
		TLink *Link = m_Document->GetLinkByIndex(i2);
		if (Link->m_bVisible) {
			bool visible = true;
			if (Link->m_Labels->Count > 0 && m_nTargetLink != i2) {
				visible = false;
				for (int il = 0; il < Link->m_Labels->Count && !visible; il++) {
					TCardLabel *Label =
						m_Document->GetLabelByIndex(1,
						Link->m_Labels->GetLabel(il) - 1);
					visible |= Label->m_bShow;
				}
				for (int il = 0; il < Link->m_Labels->Count && visible; il++) {
					TCardLabel *Label =
						m_Document->GetLabelByIndex(1,
						Link->m_Labels->GetLabel(il) - 1);
					visible &= !Label->m_bHide;
				}
			}
			else if (m_nTargetLink != i2) {
				visible = SettingView.m_bNoLinkLabelVisible;
			}
			Link->m_bVisible = visible;
		}
	}

	// サイズで表示するカードを選別する

	// サイズ
	if (SettingView.m_bSizeLimitation)
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			TCard *Card = m_Document->GetCardByIndex(i);
			if (Card->m_bVisible) {
				float size = Card->m_nSize;
				if (Card->m_Labels->Count) {
					for (int il = 0; il < Card->m_Labels->Count; il++) {
						size *= m_Document->GetLabelByIndex(0,
							Card->m_Labels->GetLabel(il) - 1)->m_nSize * 0.01f;
					}
				}
				Card->m_bVisible &= size >= SettingView.m_nSizeLimitation - 1;
			}
		}

	// 日付で表示するカードを選別する
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
						Card->m_bVisible &= fabs(date - currentdate) <
							daterange;
					}
					break;
				case 3:
					// Older
					if (currentdate > -1.0f) {
						Card->m_bVisible &=
							date <= currentdate && fabs(date - currentdate) <
							daterange;
					}
					break;
				case 4:
					// Newer
					if (currentdate > -1.0f) {
						Card->m_bVisible &=
							date >= currentdate && fabs(date - currentdate) <
							daterange;
					}
					break;
				}

			}
		}
	}

}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_Label0Click(TObject *Sender) {
	int index = ((TMenuItem*)Sender)->Tag;
	if (m_nTargetCard < 0) {
		// カード非選択
		return;
	}
	else {
		TCard *Card = m_Document->GetCard(m_nTargetCard);
		if (index >= Card->m_Labels->Count) {
			// ラベルなし
			PL_NoLabel->Checked = true;
		}
		else {
			// ラベルあり
			((TMenuItem*)MI_Labels->Items[Card->m_Labels->GetLabel(index) - 1])
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
	int label = ((TMenuItem*)Sender)->Tag;
	int index = Po_Label->Tag;

	if (index < 0) {
		// ツールラベル指定
		m_nToolLabel = label;
		return;
	}

	if (m_nTargetCard < 0) {
		// カード非選択
		return;
	}

	int AddLabel = -1;
	int DelLabel = -1;

	TCard *Card = m_Document->GetCard(m_nTargetCard);
	if (index >= Card->m_Labels->Count) {
		// ラベル追加

		if (label > 0) {
			// ラベル追加
			BackupSub(MLText.AddLabel);
			AddLabel = label;
		}
	}
	else {
		// 既存ラベルの編集
		if (label == 0) {
			// ラベル削除
			BackupSub(MLText.DeleteLabel);
			DelLabel = Card->m_Labels->GetLabel(index);
		}
		else {
			// ラベル変更
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
				}
				else if (AddLabel >= 0 && DelLabel >= 0) {
					// 変更
					if (Card->m_Labels->Contain(DelLabel)) {
						// 削除すべきラベルを持っている
						if (Card->m_Labels->Contain(AddLabel)) {
							// 追加するラベルを既に持っている→単純削除
							Card->m_Labels->DeleteLabel(DelLabel);
						}
						else {
							// 通常の変更
							for (int i2 = 0; i2 < Card->m_Labels->Count; i2++) {
								if (Card->m_Labels->GetLabel(i2) == DelLabel) {
									Card->m_Labels->SetLabel(i2, AddLabel);
									break;
								}
							}
						}
					}
					else {
						// 削除すべきラベルを持っていない→単純追加
						Card->m_Labels->AddLabel(label);
					}
				}
				else {
					// 削除
					Card->m_Labels->DeleteLabel(DelLabel);
				}
			}
		}
	}
	/*
	 TCard *Card = m_Document->GetCard(m_nTargetCard);

	 if (index >= Card->m_Labels->Count){
	 //ラベル追加

	 if (label > 0){
	 //ラベル追加
	 BackupSub(MLText.AddLabel);
	 Card->m_Labels->AddLabel(label);
	 }
	 }else{
	 //既存ラベルの編集
	 if (label == 0){
	 //ラベル削除
	 BackupSub(MLText.DeleteLabel);
	 Card->m_Labels->DeleteLabel(Card->m_Labels->GetLabel(index));
	 }else{
	 //ラベル変更
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

void __fastcall TFo_Main::MH_ContentsClick(TObject *Sender) {
	ExitFullScreen();

	UnicodeString S = UnicodeString(ExtractFilePath(ParamStr(0))) +
		UnicodeString("editor.exe");
	UnicodeString S2 = UnicodeString("\"") + UnicodeString
		(ExtractFilePath(ParamStr(0))) + UnicodeString("help.fip\"");
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

void __fastcall TFo_Main::MF_RecentFilesClick(TObject *Sender) {
	if (SaveCheck()) {
		LoadSub(SettingFile.m_RecentFiles[((TMenuItem*)Sender)->Tag]);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MF_RecentFoldersClick(TObject *Sender) {
	OD->FileName = SettingFile.m_RecentFolders[((TMenuItem*)Sender)->Tag] +
		"*.fip";
	MF_OpenClick(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Ed_LinkTitleChange(TObject *Sender) {
	if (Ed_LinkTitle->Tag == 0) {
		// 選択中のリンク名編集
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
		// 選択中のリンクの方向有り無し切り替え
		TLink *Link = m_Document->GetLinkByIndex(m_nCurrentLink);
		Link->m_bDirection = Ch_LinkDirection->Checked;
		m_Document->RefreshList();
		m_Document->RefreshLink();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_LinkDirectionInverseClick(TObject *Sender) {
	// 選択中のリンクの方向反転
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
	int index = ((TMenuItem*)Sender)->Tag;
	if (m_nTargetLink < 0) {
		// リンク非選択
		return;
	}
	else {
		TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);
		if (index >= Link->m_Labels->Count) {
			// ラベルなし
			PL_NoLinkLabel->Checked = true;
		}
		else {
			// ラベルあり
			((TMenuItem*)MI_LinkLabels->Items[Link->m_Labels->GetLabel(index) -
				1])->Checked = true;
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
	int label = ((TMenuItem*)Sender)->Tag;
	int index = Po_LinkLabel->Tag;

	if (index < 0) {
		// ツールラベル指定
		m_nToolLinkLabel = label;
		return;
	}

	if (m_nTargetLink < 0) {
		// リンク非選択
		return;
	}

	TLink *Link = m_Document->GetLinkByIndex(m_nTargetLink);

	if (index >= Link->m_Labels->Count) {
		// ラベル追加

		if (label > 0) {
			// ラベル追加
			BackupSub(MLText.AddLabel);
			Link->m_Labels->AddLabel(label);
		}
	}
	else {
		// 既存ラベルの編集
		if (label == 0) {
			// ラベル削除
			BackupSub(MLText.DeleteLabel);
			Link->m_Labels->DeleteLabel(Link->m_Labels->GetLabel(index));
		}
		else {
			// ラベル変更
			BackupSub(MLText.ChangeLabel);
			Link->m_Labels->SetLabel(index, label);
		}
	}
	m_Document->RefreshLabel();
	m_bRedrawRequested = true;
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

void __fastcall TFo_Main::MI_NewLinkLabelClick(TObject *Sender) {
	if (m_nTargetLink >= 0) {
		Fo_EditText = new TFo_EditText(this);
		Fo_EditText->La_Text->Caption = MLText.LabelName + ":";
		Fo_EditText->Caption = DeleteActionKey(MI_NewLinkLabel->Caption);
		Fo_EditText->ShowModal();
		if (Fo_EditText->ModalResult == mrOk &&
			Fo_EditText->Ed_Text->Text != "") {
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

void __fastcall TFo_Main::MA_RandomFlashClick(TObject *Sender) {
	m_nAnimation = 1;
	m_nAnimationCount = 0;
	PC_Client->ActivePage = TS_Browser;
	SettingView.m_bAnimationPaused = false;

	if (!m_DocBeforeAnimation) {
		m_DocBeforeAnimation = new TDocument(*m_Document);
	}
	m_nTargetCard = -1;
	m_nTargetLink = -1;
	m_Document->ClearCardSelection();

	// MM_Menu->Enabled = false;//アニメーション中の編集を避けるため
	m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
	m_bAnimationBak_Arrange = SB_Arrange->Down;
	m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
	m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;

	// SoftLinkでArrange
	SB_Arrange->Down = true;
	Bu_ArrangeType->Tag = 102;

	PB_Browser->Tag = 1;
	TB_Zoom->Position = -1000; // 全体表示
	SB_AutoScroll->Down = false;
	Sc_X->Position = 5000;
	Sc_Y->Position = 5000;
	/*
	 if (!SB_AutoScroll->Down){
	 Sc_X->Position = 5000;
	 Sc_Y->Position = 5000;
	 }
	 */
	PB_Browser->Tag = 0;

	Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
	Fo_RandomAnimation->TB_Zoom->Enabled = true;
	Fo_RandomAnimation->Show();
}

// ---------------------------------------------------------------------------
void TFo_Main::Animation_RandomCard() {
	if (!Fo_RandomAnimation->Visible) {
		// アニメーション終了
		m_nAnimation = 0;
		if (m_DocBeforeAnimation) {
			m_Document->CopyFrom(m_DocBeforeAnimation);
			delete m_DocBeforeAnimation;
			m_DocBeforeAnimation = NULL;
			SB_Arrange->Down = m_bAnimationBak_Arrange;
		}
		else {
			SB_Arrange->Down = true;
		}
		Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
		SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
		SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
		// MM_Menu->Enabled = true;//アニメーション中の編集を避けるため
		Redraw();
		return;
	}

	// Random Card
	if (m_nAnimationCount == 0) {
		m_Document->ClearCardSelection();

		SetCardAssign();

		// 全て表示
		for (int i = 0; i < m_Document->m_Cards->Count; i++)
			if (m_CardAssign[i] == i) {
				m_Document->GetCardByIndex(i)->m_bVisible = true;
			}
		for (int i = 0; i < m_Document->m_Links->Count; i++) {
			m_Document->GetLinkByIndex(i)->m_bVisible = true;
		}

		FilteringCard();

		// 表示になっている個数を数える
		int count = 0;
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			if (m_Document->GetCardByIndex(i)->m_bVisible) {
				count++;
			}
		}

		// 指定個数以下になるまで非表示にする
		while (count > SettingView.m_nAnimationRCCards) {
			int cardindex = rand() % m_Document->m_Cards->Count;
			TCard *Card = m_Document->GetCardByIndex(cardindex);
			if (Card->m_bVisible) {
				Card->m_bVisible = false;
				count--;
			}
		}

		// 適当なカードをアクティブにする（読み上げのため）
		// 表示になっている個数を数える
		if (count > 0) {
			int idx = rand() % count;
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				if (m_Document->GetCardByIndex(i)->m_bVisible) {
					if (idx == 0) {
						TCard *Card = m_Document->GetCardByIndex(i);
						m_nTargetCard = Card->m_nID;
						Card->m_bSelected = true;
						break;
					}
					idx--;
				}
			}
		}

	}
	m_nAnimationCount += Ti_Check->Interval;

	if (!SettingView.m_bAnimationPaused && m_nAnimationCount >= 3000 * pow(2.0,
		-(SettingView.m_nAnimationRCSpeed - 30) / 25.0)) {
		m_nAnimationCount = 0;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MA_RandomMapClick(TObject *Sender) {
	m_nAnimation = 5;
	m_nAnimationCount = 0;
	PC_Client->ActivePage = TS_Browser;
	SettingView.m_bAnimationPaused = false;

	if (!m_DocBeforeAnimation) {
		m_DocBeforeAnimation = new TDocument(*m_Document);
	}
	m_nTargetCard = -1;
	m_nTargetLink = -1;
	m_Document->ClearCardSelection();

	m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
	m_bAnimationBak_Arrange = SB_Arrange->Down;
	m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
	m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;

	// LinkでArrange
	SB_Arrange->Down = true;
	Bu_ArrangeType->Tag = 2;

	PB_Browser->Tag = 1;
	TB_Zoom->Position = -1000; // 全体表示
	SB_AutoScroll->Down = false;
	Sc_X->Position = 5000;
	Sc_Y->Position = 5000;
	PB_Browser->Tag = 0;

	Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
	Fo_RandomAnimation->TB_Zoom->Enabled = true;
	Fo_RandomAnimation->Show();
}

// ---------------------------------------------------------------------------
void TFo_Main::Animation_RandomMap() {
	if (!Fo_RandomAnimation->Visible) {
		// アニメーション終了
		m_nAnimation = 0;
		if (m_DocBeforeAnimation) {
			m_Document->CopyFrom(m_DocBeforeAnimation);
			delete m_DocBeforeAnimation;
			m_DocBeforeAnimation = NULL;
			SB_Arrange->Down = m_bAnimationBak_Arrange;
		}
		else {
			SB_Arrange->Down = true;
		}
		Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
		SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
		SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
		Redraw();
		return;
	}

	// Random Map
	if (m_nAnimationCount == 0) {
		// マップ更新タイミング

		// 表示中カードの数を数える
		SetCardAssign();
		int count = 0;
		for (int i = 0; i < m_Document->m_Cards->Count; i++)
			if (m_CardVisible[i] && m_CardAssign[i] == i) {
				count++;
			}

		// カード、リンクの表示状況バックアップ
		bool *CVBak = new bool[m_Document->m_Cards->Count];
		bool *CVNext = new bool[m_Document->m_Cards->Count];
		memcpy(CVBak, m_CardVisible, sizeof(bool) * m_Document->m_Cards->Count);
		memcpy(CVNext, m_CardVisible,
			sizeof(bool) * m_Document->m_Cards->Count);
		bool *LVBak = new bool[m_Document->m_Links->Count];
		bool *LVNext = new bool[m_Document->m_Links->Count];
		memcpy(LVBak, m_LinkVisible, sizeof(bool) * m_Document->m_Links->Count);
		memcpy(LVNext, m_LinkVisible,
			sizeof(bool) * m_Document->m_Links->Count);

		// 全て表示
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			m_Document->GetCardByIndex(i)->m_bVisible = true;
		}
		for (int i = 0; i < m_Document->m_Links->Count; i++) {
			m_Document->GetLinkByIndex(i)->m_bVisible = true;
		}

		// 表示対象のカード、リンクをフィルタリング
		FilteringCard();

		int inccardnum = 0;
		if (m_nTargetCard >= 0) {
			// カードが表示されているなら、そこからリンクを伸ばす
			for (int i = 0; i < m_Document->m_Links->Count && count <
				SettingView.m_nAnimationRCCards; i++) {
				TLink *Link = m_Document->GetLinkByIndex(i);
				if (Link->m_bVisible) { // 表示対象のリンク
					int fromidx = m_Document->SearchCardIndex(Link->m_nFromID);
					if (m_CardVisible[fromidx]) { // 表示中=表示対象
						int destidx =
							m_Document->SearchCardIndex(Link->m_nDestID);
						TCard *Dest = m_Document->GetCardByIndex(destidx);
						if (!m_CardVisible[destidx] && Dest->m_bVisible)
						{ // 表示されておらず、表示対象
							CVNext[destidx] = true;
							LVNext[i] = true;
							TCard *From = m_Document->GetCardByIndex(fromidx);
							Dest->m_fX = From->m_fX;
							Dest->m_fY = From->m_fY;
							if (m_CardAssign[destidx] == destidx) {
								count++;
								inccardnum++;
							}
						}
					}
				}
			}
		}
		if (m_nTargetCard < 0 || inccardnum == 0) {
			// カードが選択されていないか、カードが追加されなかった場合、新たにカードを1枚選択
			m_Document->ClearCardSelection();

			/*
			 //全て表示
			 for (int i = 0 ; i < m_Document->m_Cards->Count ; i++) if (m_CardAssign[i] == i){
			 m_Document->GetCardByIndex(i)->m_bVisible = true;
			 }

			 FilteringCard();
			 */

			// 表示になっている個数を数える
			int count = 0;
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				if (m_Document->GetCardByIndex(i)->m_bVisible) {
					count++;
				}
			}

			// 表示するIndexを決める
			int idx = rand() % count;
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				TCard *Card = m_Document->GetCardByIndex(i);
				if (Card->m_bVisible) {
					if (idx == 0) {
						m_nTargetCard = Card->m_nID;
						Card->m_bSelected = true;
						Card->m_fX = 0.5f;
						Card->m_fY = 0.5f;
					}
					else {
						Card->m_bVisible = false;
					}
					idx--;
				}
			}
		}
		else {
			// 新しいVisibleを反映
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				m_Document->GetCardByIndex(i)->m_bVisible = CVNext[i];
			}
			for (int i = 0; i < m_Document->m_Links->Count; i++) {
				m_Document->GetLinkByIndex(i)->m_bVisible = LVNext[i];
			}
		}

		delete[]LVNext;
		delete[]LVBak;
		delete[]CVNext;
		delete[]CVBak;

	}
	m_nAnimationCount += Ti_Check->Interval;

	if (!SettingView.m_bAnimationPaused && m_nAnimationCount >= 2000 * pow(2.0,
		-(SettingView.m_nAnimationRCSpeed - 30) / 25.0)) {
		m_nAnimationCount = 0;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MA_RandomScrollClick(TObject *Sender) {
	m_nAnimation = 2;
	m_nAnimationCount = 3000;
	PC_Client->ActivePage = TS_Browser;
	SettingView.m_bAnimationPaused = false;

	if (!m_DocBeforeAnimation) {
		m_DocBeforeAnimation = new TDocument(*m_Document);
	}
	m_nTargetCard = -1;
	m_nTargetLink = -1;
	m_Document->ClearCardSelection();

	m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
	m_bAnimationBak_Arrange = SB_Arrange->Down;
	m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
	m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;
	// MM_Menu->Enabled = false;//アニメーション中の編集を避けるため

	// Arrangeなし
	SB_Arrange->Down = false;

	PB_Browser->Tag = 1;
	TB_Zoom->Position = -2000; // 全体表示
	SB_AutoScroll->Down = false; // スクロールなし
	SB_AutoZoom->Down = false; // 自動ズームなし
	Sc_X->Position = 5000;
	Sc_Y->Position = 5000;
	/*
	 if (!SB_AutoScroll->Down){
	 Sc_X->Position = 5000;
	 Sc_Y->Position = 5000;
	 }
	 */
	PB_Browser->Tag = 0;

	for (int i = 0; i < MAXRANDOMCARDS; i++) {
		m_Animation_RC2Idxs[i] = -1;
	}

	Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
	Fo_RandomAnimation->TB_Zoom->Enabled = false;
	Fo_RandomAnimation->Show();
}

// ---------------------------------------------------------------------------
void TFo_Main::Animation_RandomCard2() {
	if (!Fo_RandomAnimation->Visible) {
		// アニメーション終了
		m_nAnimation = 0;
		if (m_DocBeforeAnimation) {
			m_Document->CopyFrom(m_DocBeforeAnimation);
			delete m_DocBeforeAnimation;
			m_DocBeforeAnimation = NULL;
			SB_Arrange->Down = m_bAnimationBak_Arrange;
		}
		else {
			SB_Arrange->Down = true;
		}
		Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
		SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
		SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
		// MM_Menu->Enabled = true;//アニメーション中の編集を避けるため
		Redraw();
		// SetCardVisible();
		return;
	}

	if (SettingView.m_bAnimationPaused) {
		// 一時停止中
		return;
	}

	// Random Card 2
	for (int i = 0; i < MAXRANDOMCARDS; i++) {
		if (i < SettingView.m_nAnimationRCCards) {
			// 表示数の範囲内

			if (m_Animation_RC2Idxs[i] < 0) {
				// カードが指定されていない

				SetCardAssign();

				// 表示可能なカードをフィルタリング
				for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
					if (m_CardAssign[i2] == i2) {
						m_Document->GetCardByIndex(i2)->m_bVisible = true;
					}
				for (int i = 0; i < m_Document->m_Links->Count; i++) {
					m_Document->GetLinkByIndex(i)->m_bVisible = true;
				}

				FilteringCard();

				// 既に表示中のカードもフィルタリング
				int count = 0;
				for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++) {
					TCard *Card = m_Document->GetCardByIndex(i2);
					if (Card->m_bVisible) {
						bool found = false;
						for (int i3 = 0;
						i3 < SettingView.m_nAnimationRCCards && !found; i3++) {
							if (m_Animation_RC2Idxs[i3] == i2) {
								found = true;
							}
						}

						if (found) {
							Card->m_bVisible = false;
						}
						else {
							count++;
						}
					}
				}

				if (count > 0) {
					// 表示可能なカードの中から適当な1枚を選択
					int idx = rand() % count;
					for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++) {
						if (m_Document->GetCardByIndex(i2)->m_bVisible) {
							if (idx-- == 0) {
								idx = i2;
								break;
							}
						}
					}

					if (idx >= 0) {
						m_Animation_RC2Idxs[i] = idx;
						TCard *Card =
							m_Document->GetCardByIndex(m_Animation_RC2Idxs[i]);

						/*
						 //読み上げ用
						 if (SettingView.m_bRead) if (m_AgentChar && m_TTS){
						 if (m_nAnimationCount >= 3000){
						 m_Document->ClearCardSelection();
						 m_nTargetCard = Card->m_nID;
						 Card->m_bSelected = true;
						 m_nAnimationCount = 0;
						 }
						 }
						 */

						// 初期表示座標、移動方向を決定
						int direction = rand() % 4; // 上下左右
						switch (direction / 2) {
						case 0: // 上下から
							Card->m_fX = (rand() % 2001) / 1000.0f - 0.5f;
							m_Animation_RC2AXs[i] =
								-(Card->m_fX - 0.5f) + 0.5f + (rand() % 10001)
								/ 10000.0f - 0.5f;
							if (direction == 0) {
								// 上
								Card->m_fY = 2.0f;
								m_Animation_RC2AYs[i] = -3.0f;
							}
							else {
								// 下
								Card->m_fY = -1.0f;
								m_Animation_RC2AYs[i] = 3.0f;
							}
							break;
						case 1: // 左右から
							Card->m_fY = (rand() % 2001) / 1000.0f - 0.5f;
							m_Animation_RC2AYs[i] =
								-(Card->m_fX - 0.5f) + 0.5f + (rand() % 10001)
								/ 10000.0f - 0.5f;
							if (direction == 3) {
								// 左
								Card->m_fX = 2.0f;
								m_Animation_RC2AXs[i] = -3.0f;
							}
							else {
								// 右
								Card->m_fX = -1.0f;
								m_Animation_RC2AXs[i] = 3.0f;
							}
							break;
						};

						// 移動速度決定（デフォルトで160~240フレーム）
						int frames = (160 + rand() % 81);
						m_Animation_RC2AXs[i] /= frames;
						m_Animation_RC2AYs[i] /= frames;
					}
				}
			}
		}
		else {
			// 表示数の範囲外
			m_Animation_RC2Idxs[i] = -1;
		}
	}

	// （全て非表示）
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		m_Document->GetCardByIndex(i)->m_bVisible = false;
	}

	// 移動表示中のカードのみ表示
	float speed = 0.5f / pow(2.0, -(SettingView.m_nAnimationRCSpeed - 30)
		/ 25.0);
	for (int i = 0; i < SettingView.m_nAnimationRCCards; i++) {
		if (m_Animation_RC2Idxs[i] >= 0) {
			// 移動表示中
			TCard *Card = m_Document->GetCardByIndex(m_Animation_RC2Idxs[i]);

			// 座標加算
			Card->m_fX += m_Animation_RC2AXs[i] * speed;
			Card->m_fY += m_Animation_RC2AYs[i] * speed;

			if (Card->m_fX < -1.0f || Card->m_fY < -1.0f || Card->m_fX >
				2.0f || Card->m_fY > 2.0f) {
				// 画面外にはみ出した
				m_Animation_RC2Idxs[i] = -1;
			}
			else {
				Card->m_bVisible = true;
			}
		}
	}

	m_nAnimationCount += Ti_Check->Interval;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MA_RandomJumpClick(TObject *Sender) {
	m_nAnimation = 4;
	m_nAnimationCount = 3000;
	PC_Client->ActivePage = TS_Browser;
	SettingView.m_bAnimationPaused = false;

	if (!m_DocBeforeAnimation) {
		m_DocBeforeAnimation = new TDocument(*m_Document);
	}
	m_nTargetCard = -1;
	m_nTargetLink = -1;
	m_Document->ClearCardSelection();

	m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
	m_bAnimationBak_Arrange = SB_Arrange->Down;
	m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
	m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;
	// MM_Menu->Enabled = false;//アニメーション中の編集を避けるため

	// Arrangeなし
	SB_Arrange->Down = false;

	PB_Browser->Tag = 1;
	TB_Zoom->Position = -2000; // 全体表示
	SB_AutoScroll->Down = false; // スクロールなし
	SB_AutoZoom->Down = false; // 自動ズームなし
	Sc_X->Position = 5000;
	Sc_Y->Position = 5000;
	PB_Browser->Tag = 0;

	for (int i = 0; i < MAXRANDOMCARDS; i++) {
		m_Animation_RC2Idxs[i] = -1;
	}

	Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
	Fo_RandomAnimation->TB_Zoom->Enabled = false;
	Fo_RandomAnimation->Show();
}

// ---------------------------------------------------------------------------
void TFo_Main::Animation_RandomJump() {
	if (!Fo_RandomAnimation->Visible) {
		// アニメーション終了
		m_nAnimation = 0;
		if (m_DocBeforeAnimation) {
			m_Document->CopyFrom(m_DocBeforeAnimation);
			delete m_DocBeforeAnimation;
			m_DocBeforeAnimation = NULL;
			SB_Arrange->Down = m_bAnimationBak_Arrange;
		}
		else {
			SB_Arrange->Down = true;
		}
		Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
		SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
		SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
		// MM_Menu->Enabled = true;//アニメーション中の編集を避けるため
		Redraw();
		// SetCardVisible();
		return;
	}

	if (SettingView.m_bAnimationPaused) {
		// 一時停止中
		return;
	}

	// Random Card 2
	for (int i = 0; i < MAXRANDOMCARDS; i++) {
		if (i < SettingView.m_nAnimationRCCards) {
			// 表示数の範囲内

			if (m_Animation_RC2Idxs[i] < 0) {
				// カードが指定されていない

				SetCardAssign();

				// 表示可能なカードをフィルタリング
				for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
					if (m_CardAssign[i2] == i2) {
						m_Document->GetCardByIndex(i2)->m_bVisible = true;
					}
				for (int i = 0; i < m_Document->m_Links->Count; i++) {
					m_Document->GetLinkByIndex(i)->m_bVisible = true;
				}

				FilteringCard();

				// 既に表示中のカードもフィルタリング
				int count = 0;
				for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++) {
					if (m_Document->GetCardByIndex(i2)->m_bVisible) {
						bool found = false;
						for (int i3 = 0;
						i3 < SettingView.m_nAnimationRCCards && !found; i3++) {
							if (m_Animation_RC2Idxs[i3] == i2) {
								found = true;
							}
						}

						if (found) {
							m_Document->GetCardByIndex(i2)->m_bVisible = false;
						}
						else {
							count++;
						}
					}
				}

				if (count > 0) {
					// 表示可能なカードの中から適当な1枚を選択
					int idx = rand() % count;
					for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++) {
						if (m_Document->GetCardByIndex(i2)->m_bVisible) {
							if (idx-- == 0) {
								idx = i2;
								break;
							}
						}
					}

					if (idx >= 0) {
						m_Animation_RC2Idxs[i] = idx;
						TCard *Card =
							m_Document->GetCardByIndex(m_Animation_RC2Idxs[i]);

						/*
						 //読み上げ用
						 if (SettingView.m_bRead) if (m_AgentChar && m_TTS){
						 if (m_nAnimationCount >= 3000){
						 m_Document->ClearCardSelection();
						 m_nTargetCard = Card->m_nID;
						 Card->m_bSelected = true;
						 m_nAnimationCount = 0;
						 }
						 }
						 */

						// 初期表示座標を決定
						Card->m_fY = 2.0f;
						Card->m_fX = (rand() % 2001) / 1000.0f - 0.5f;
						// 移動速度決定（デフォルトで160~240フレーム）
						int frames = (160 + rand() % 81);
						m_Animation_RC2AYs[i] =
							(-rand() % 15 - 27) * (1.0f / frames);
						m_Animation_RC2AXs[i] =
							(rand() % 61 - 30) * (0.1f / frames);

					}
				}
			}
		}
		else {
			// 表示数の範囲外
			m_Animation_RC2Idxs[i] = -1;
		}
	}

	// （全て非表示）
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		m_Document->GetCardByIndex(i)->m_bVisible = false;
	}

	// 移動表示中のカードのみ表示
	float speed = 0.3f / pow(2.0, -(SettingView.m_nAnimationRCSpeed - 30)
		/ 25.0);
	for (int i = 0; i < SettingView.m_nAnimationRCCards; i++) {
		if (m_Animation_RC2Idxs[i] >= 0) {
			// 移動表示中
			TCard *Card = m_Document->GetCardByIndex(m_Animation_RC2Idxs[i]);

			// 座標加算
			Card->m_fX += m_Animation_RC2AXs[i] * speed;
			Card->m_fY += m_Animation_RC2AYs[i] * speed;
			m_Animation_RC2AYs[i] += speed * 0.01f;

			if (Card->m_fY > 2.0f) {
				// 画面外にはみ出した
				m_Animation_RC2Idxs[i] = -1;
			}
			else {
				Card->m_bVisible = true;
			}
		}
	}

	m_nAnimationCount += Ti_Check->Interval;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MA_RandomTraceClick(TObject *Sender) {
	m_nAnimation = 3;
	m_nAnimationCount = 0;
	PC_Client->ActivePage = TS_Browser;
	SettingView.m_bAnimationPaused = false;

	m_nTargetLink = -1;
	m_nAnimation_LastCard = m_nTargetCard;

	if (!m_DocBeforeAnimation) {
		m_DocBeforeAnimation = new TDocument(*m_Document);
	}
	m_nAnimationBak_ArrangeType = Bu_ArrangeType->Tag;
	m_bAnimationBak_Arrange = SB_Arrange->Down;
	m_bAnimationBak_AutoScroll = SB_AutoScroll->Down;
	m_bAnimationBak_AutoZoom = SB_AutoZoom->Down;
	// MM_Menu->Enabled = false;//アニメーション中の編集を避けるため

	SB_AutoScroll->Down = true;
	SB_AutoZoom->Down = true;

	Fo_RandomAnimation->TB_Zoom->Position = TB_Zoom->Position;
	Fo_RandomAnimation->TB_Zoom->Enabled = true;
	Fo_RandomAnimation->Show();
}

// ---------------------------------------------------------------------------
void TFo_Main::Animation_RandomTrace() {
	if (!Fo_RandomAnimation->Visible) {
		// アニメーション終了
		m_nAnimation = 0;
		if (m_DocBeforeAnimation) {
			m_Document->CopyFrom(m_DocBeforeAnimation);
			delete m_DocBeforeAnimation;
			m_DocBeforeAnimation = NULL;
		}
		Bu_ArrangeType->Tag = m_nAnimationBak_ArrangeType;
		SB_Arrange->Down = m_bAnimationBak_Arrange;
		SB_AutoScroll->Down = m_bAnimationBak_AutoScroll;
		SB_AutoZoom->Down = m_bAnimationBak_AutoZoom;
		Redraw();
		// MM_Menu->Enabled = true;//アニメーション中の編集を避けるため
		return;
	}

	// Random Trace
	if (m_nAnimationCount == 0) {

		// 辿れるリンク数
		int count = 0;
		if (m_nTargetCard >= 0) {
			// 現在のカードから辿れるリンクを数える

			for (int il = 0; il < m_Document->m_Links->Count; il++)
				if (m_LinkVisible[il]) {
					TLink *Link = m_Document->GetLinkByIndex(il);
					if ((Link->m_nFromID == m_nTargetCard && Link->m_nDestID !=
						m_nAnimation_LastCard) ||
						(Link->m_nDestID == m_nTargetCard && Link->m_nFromID !=
						m_nAnimation_LastCard && !Link->m_bDirection)) {
						// 辿れるリンク
						count++;
					}
				}
		}

		int nextid = -1;
		if (count) {
			// 辿れるリンクがある

			int idx = rand() % count; // 辿るリンク決定

			for (int il = 0; il < m_Document->m_Links->Count; il++)
				if (m_LinkVisible[il]) {
					TLink *Link = m_Document->GetLinkByIndex(il);
					if ((Link->m_nFromID == m_nTargetCard && Link->m_nDestID !=
						m_nAnimation_LastCard) ||
						(Link->m_nDestID == m_nTargetCard && Link->m_nFromID !=
						m_nAnimation_LastCard && !Link->m_bDirection)) {
						// 辿れるリンク

						if (idx-- == 0) {
							if (Link->m_nFromID == m_nTargetCard) {
								nextid = Link->m_nDestID;
							}
							else {
								nextid = Link->m_nFromID;
							}
						}
					}
				}
		}

		if (m_nTargetCard < 0) {
			// 現在カード非選択

			// 表示中のカードから適当にカードを選択

			count = 0; // 表示中のカード数
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				if (m_Document->GetCardByIndex(i)->m_bVisible) {
					count++;
				}
			}

			int idx = rand() % count; // 表示カード決定
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				if (m_Document->GetCardByIndex(i)->m_bVisible) {
					if (idx-- == 0) {
						idx = i;
						break;
					}
				}
			}

			if (idx >= 0) {
				nextid = m_Document->GetCardByIndex(idx)->m_nID;
			}
		}

		m_nAnimation_LastCard = m_nTargetCard;
		m_nTargetCard = nextid;
		m_Document->ClearCardSelection();
		TCard *Card = m_Document->GetCard(m_nTargetCard);
		if (Card) {
			Card->m_bSelected = true;
		}
	}

	m_nAnimationCount += Ti_Check->Interval;

	if (!SettingView.m_bAnimationPaused && m_nAnimationCount >= 1000 * pow(2.0,
		-(SettingView.m_nAnimationRCSpeed - 30) / 25.0)) {
		m_nAnimationCount = 0;
	}
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
	}
	else {
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
	}
	else {
		TB_Zoom->SetFocus();
	}

	/*
	 if (MV_FullScreen->Checked){
	 //タスクバーを非表示にする
	 HWND hTrayWnd = FindWindow("Shell_TrayWnd", NULL);
	 ShowWindow(hTrayWnd, SW_HIDE);

	 //各アイテムを非表示にする
	 Pa_Card->Visible = false;
	 Sp_Left->Visible = false;
	 LB_List->Visible = false;
	 Sp_Left->Align = alNone;
	 LB_List->Align = alNone;
	 Pa_Client->Align = alNone;
	 Pa_Client->Align = alClient;

	 //ウインドウサイズ
	 WindowState = wsNormal;
	 Width = Screen->Width;
	 Height = Screen->Height;
	 Top = 0;
	 Left = 0;
	 ShowMessage(Pa_Client->Left);
	 ShowMessage(PC_Client->Left);
	 }else{
	 //タスクバーを表示
	 HWND hTrayWnd = FindWindow("Shell_TrayWnd", NULL);
	 ShowWindow(hTrayWnd, SW_SHOWNORMAL);

	 //各アイテムを表示
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
				S += m_Document->GetLabelByIndex(0,
					Card->m_Labels->GetLabel(i) - 1)->m_Name;
			}
			S += ret;
		}
		else {
			S += MLText.Labels + UnicodeString(" : 0");
			S += ret;
		}

		S += ret;

		// LinkNum
		int count = 0;
		for (int il = 0; il < m_Document->m_Links->Count; il++) {
			TLink *Link = m_Document->GetLinkByIndex(il);
			if (Link->m_nFromID == Card->m_nID ||
				Link->m_nDestID == Card->m_nID) {
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
		S += MLText.Created + UnicodeString(" : ") + DateTimeToStr
			(Card->m_fCreated);
		S += ret;
		S += MLText.Edited + UnicodeString(" : ") + DateTimeToStr
			(Card->m_fUpdated);
		S += ret;
		S += MLText.Viewed + UnicodeString(" : ") + DateTimeToStr
			(Card->m_fViewed);

		Application->MessageBox(UnicodeString(S).c_str(),
			UnicodeString(DeleteActionKey(PL_CardProperty->Caption))
			.c_str(), 0);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MH_CheckLatestClick(TObject *Sender) {
	ExitFullScreen();

	Fo_Information = new TFo_Information(this);
	Fo_Information->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MH_EnableSpecialMenuClick(TObject *Sender) {
	M_Special->Visible = true;
	/*
	 //リンク反転
	 for (int i = 0 ; i < m_Document->m_Links->Count ; i++){
	 TLink *Link = m_Document->GetLinkByIndex(i);
	 int bak = Link->m_nFromID;
	 Link->m_nFromID = Link->m_nDestID;
	 Link->m_nDestID = bak;
	 }
	 m_Document->RefreshLink();
	 // */

	/*
	 //カードタイトルを?に置き換える。
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
	SettingView.m_bLabelRectangleVisible =
		!SettingView.m_bLabelRectangleVisible;
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
		// カードが選択されていない
		return;
	}

	if (Button == btNext) {
		// 上方向
		for (int i = 1; i < LB_List->Count; i++) {
			TCard *Card = m_Document->GetCardByIndex(i);
			if (Card->m_bSelected) {
				TCard *Card2 = NULL;
				int swapindex;
				if (!TreeMode()) {
					// 通常
					Card2 = m_Document->GetCardByIndex(i - 1);
					swapindex = i - 1;
				}
				else {
					// 木構造
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
	}
	else if (Button == btPrev) {
		// 下方向
		for (int i = LB_List->Count - 2; i >= 0; i--) {
			TCard *Card = m_Document->GetCardByIndex(i);
			if (Card->m_bSelected) {
				TCard *Card2 = NULL;
				int swapindex;
				if (!TreeMode()) {
					// 通常
					Card2 = m_Document->GetCardByIndex(i + 1);
					swapindex = i + 1;
				}
				else {
					// 木構造
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
	 //上方向

	 if (cardindex <= 0){
	 //これ以上前にできない
	 return;
	 }

	 //一つ手前のカードと入れ替える
	 m_Document->SwapCard(cardindex, cardindex - 1);
	 LB_List->ItemIndex = cardindex - 1;
	 }else if (Button == btPrev){
	 //下方向

	 if (cardindex >= m_Document->m_Cards->Count - 1){
	 //これ以上後にできない
	 return;
	 }

	 //一つ後ろのカードと入れ替える
	 m_Document->SwapCard(cardindex, cardindex + 1);
	 LB_List->ItemIndex = cardindex + 1;
	 }
	 */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::RE_EditMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y) {
	if (Shift.Contains(ssCtrl)) {
		// Ctrl+クリック

		// 現在行を調べる
		int line = 0;
		int count = 0;
		while (line < RE_Edit->Lines->Count) {
			count += WideString(RE_Edit->Lines->Strings[line]).Length() + 2;
			if (RE_Edit->SelStart >= count) {
				line++;
			}
			else {
				break;
			}
		}
		// 現在行
		if (line < RE_Edit->Lines->Count) {
			UnicodeString Str = RE_Edit->Lines->Strings[line];
			if (IsFileNameOrURL(Str)) {
				// 開く
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
			delete(TMenuItem*)MI_ExtLink->Items[i];
		}
		delete MI_ExtLink;
		MI_ExtLink = NULL;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PM_ExtLinkClick(TObject *Sender) {
	ExitFullScreen();
	TMenuItem *MI = (TMenuItem*)Sender;
	OpenExtLink(MI->Caption);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_NewExtLinkClick(TObject *Sender) {
	if (OD_Ext->Execute()) {
		RE_Edit->Lines->Add(OD_Ext->FileName);
		/*
		 if (PC_Client->ActivePage == TS_Editor){
		 //エディター表示中
		 }else{
		 //エディター非表示
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

	// カット
	ME_CutClick(Sender);

	// カードの新規作成
	MI_NewCardClick(Sender);

	// カードの表示書き換え
	Ti_CheckTimer(Sender);

	// Paste
	RE_Edit->SetFocus();
	ME_PasteClick(Sender);

	m_bDoNotBackup = false;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFI_TxtFileClick(TObject *Sender) {
	if (OD_Txt->Execute()) {
		BackupSub(MF_Import->Caption + UnicodeString(" ") +
			MFI_TxtFile->Caption);

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
		// カードが無い
		return;
	}
	SD_Txt->FileName = ForFileName(m_Document->GetCardByIndex(0)->m_Title)
		+ ".txt";
	if (SD_Txt->Execute()) {
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			TCard *Card = m_Document->GetCardByIndex(i);
			TFileStream *FS =
				new TFileStream(ExtractFilePath(SD_Txt->FileName) +
				ForFileName(Card->m_Title) + ".txt", fmCreate);
			Card->m_Lines->SaveToStream(FS);
			delete FS;
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_HtmlFilesClick(TObject *Sender) {
	if (m_Document->m_Cards->Count == 0) {
		// カードが無い
		return;
	}
	UnicodeString DocumentTitle;
	if (m_Document->m_FN != "") {
		DocumentTitle = ExtractFileNameOnly(m_Document->m_FN);
	}
	SD_Html->FileName = "index.html";
	if (SD_Html->Execute()) {
		// index.html
		{
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
			}
			catch (...) {
				ShowMessage(MLText.FailedToSave + SD_Html->FileName +
					UnicodeString("."));
			}
			delete SL;
		}

		// cards.html
		{
			TStringList *SL = new TStringList();
			SL->Add("<html>");
			SL->Add("<head>");
			SL->Add(UnicodeString("<title>") + DocumentTitle + "</title>");
			SL->Add("<link rel=StyleSheet href=\"./style.css\">");

			SL->Add("</head>");
			SL->Add("<body>");
			SL->Add(UnicodeString("<font size=+2>") + DocumentTitle +
				"</font>");
			SL->Add("<hr>");
			for (int i = 0; i < m_Document->m_Cards->Count; i++) {
				TCard *Card = m_Document->GetCardByIndex(i);
				SL->Add(UnicodeString("<a href=\"./") + IntToStr(i + 1) +
					".html\" target=\"card\">" + Card->m_Title + "</a><br>");
			}
			SL->Add("<hr>");
			try {
				SL->SaveToFile(ExtractFilePath(SD_Html->FileName) +
					"cards.html");
			}
			catch (...) {
				ShowMessage(MLText.FailedToSave + ExtractFilePath
					(SD_Html->FileName) + "cards.html" + UnicodeString("."));
			}
			delete SL;
		}

		if (m_Document->m_FN != "") {
			DocumentTitle = ExtractFileNameOnly(m_Document->m_FN) + " - ";
		}
		// 各カードのファイル
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			TCard *Card = m_Document->GetCardByIndex(i);
			TStringList *SL = new TStringList();
			SL->Add("<html>");
			SL->Add("<head>");
			SL->Add(UnicodeString("<title>") + DocumentTitle + Card->m_Title +
				"</title>");
			SL->Add("<link rel=StyleSheet href=\"./style.css\">");

			SL->Add("</head>");
			SL->Add("<body>");
			SL->Add(UnicodeString("<font size=+2>") + DocumentTitle +
				Card->m_Title + "</font>");

			// 前のカード、次のカード
			SL->Add("<hr>");
			SL->Add(UnicodeString("<a href=\"./1.html\">[Top]</a> "));
			if (i > 0) {
				SL->Add(UnicodeString("<a href=\"./") + IntToStr(i - 1 + 1) +
					".html\">[Prev]</a> ");
			}
			else {
				SL->Add("[Prev] ");
			}
			if (i < m_Document->m_Cards->Count - 1) {
				SL->Add(UnicodeString("<a href=\"./") + IntToStr(i + 1 + 1) +
					".html\">[Next]</a> ");
			}
			else {
				SL->Add("[Next] ");
			}
			SL->Add(UnicodeString("<a href=\"./") +
				IntToStr(m_Document->m_Cards->Count) + UnicodeString
				(".html\">[End]</a> "));

			SL->Add(UnicodeString("(") + IntToStr(i + 1) + UnicodeString(" / ")
				+ IntToStr(m_Document->m_Cards->Count) + UnicodeString(")"));

			// リンク
			SL->Add("<hr>");
			for (int il = 0; il < m_Document->m_Links->Count; il++) {
				TLink *Link = m_Document->GetLinkByIndex(il);
				int dest = -1;
				if (Link->m_nFromID == Card->m_nID) {
					dest = Link->m_nDestID;
				}
				else if (Link->m_nDestID == Card->m_nID) {
					dest = Link->m_nFromID;
				}
				if (dest >= 0) {
					// 関係あるリンク
					int card2index = m_Document->SearchCardIndex(dest);
					TCard *Card2 = m_Document->GetCardByIndex(card2index);
					SL->Add(UnicodeString("<a href=\"./") +
						IntToStr(card2index + 1) + ".html\">[" +
						Card2->m_Title + "]</a> ");
				}
			}
			SL->Add("<hr>");

			// 本文
			SL->Add("<br>");
			for (int il = 0; il < Card->m_Lines->Count; il++) {
				if (IsFileNameOrURL(Card->m_Lines->Strings[il])) {
					// 外部リンク
					SL->Add(UnicodeString("<a href=\"") +
						Card->m_Lines->Strings[il] + UnicodeString("\">") +
						Card->m_Lines->Strings[il] + UnicodeString("</a><br>"));
				}
				else {
					// 普通の文章
					SL->Add(Card->m_Lines->Strings[il] + "<br>");
				}
			}
			SL->Add("<br>");

			// リンク（再）
			{
				int count = 0;
				for (int il = 0; il < m_Document->m_Links->Count; il++) {
					TLink *Link = m_Document->GetLinkByIndex(il);
					int dest = -1;
					if (Link->m_nFromID == Card->m_nID) {
						dest = Link->m_nDestID;
					}
					else if (Link->m_nDestID == Card->m_nID) {
						dest = Link->m_nFromID;
					}
					if (dest >= 0) {
						if (count++ == 0) {
							SL->Add("<hr>");
						}
						// 関係あるリンク
						int card2index = m_Document->SearchCardIndex(dest);
						TCard *Card2 = m_Document->GetCardByIndex(card2index);
						SL->Add(UnicodeString("<a href=\"./") +
							IntToStr(card2index + 1) + ".html\">[" +
							Card2->m_Title + "]</a> ");
					}
				}
			}

			SL->Add("<hr>");
			SL->Add("</body>");
			SL->Add("</html>");

			try {
				SL->SaveToFile(ExtractFilePath(SD_Html->FileName) +
					IntToStr(i + 1) + ".html");
			}
			catch (...) {
				ShowMessage(MLText.FailedToSave + ExtractFilePath
					(SD_Html->FileName) + IntToStr(i + 1) + ".html" +
					UnicodeString("."));
			}
			delete SL;
		}
	}
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
	}
	else if (m_nTargetCard >= 0 && PC_Client->ActivePage == TS_Browser) {
		// 親カードからリンクしたカードを作成
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
		// 改行後IMEが表示されないことがある問題対策（なぜか各行の先頭文字でカーソルが表示されなくなる）
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

	// カット
	ME_CutClick(Sender);

	TMemo *Me = new TMemo(this);
	Me->Parent = this;
	Me->WordWrap = false;
	Me->PasteFromClipboard();

	for (int i = 0; i < Me->Lines->Count; i++) {
		UnicodeString S = Trim(Me->Lines->Strings[i]);
		if (S != "") {
			// カードの新規作成
			MI_NewCardClick(Sender);

			// カードの表示書き換え
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

	// カードの座標をランダムにする
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
	BackupSub(MD_SortCard->Caption + UnicodeString(" ") + ((TMenuItem*)Sender)
		->Caption);
	m_Document->SortCards(-1, ((TMenuItem*)Sender)->Tag);
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
	bSearchRequest = 0x1 | 0x4; // カード名のみ検索
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
					(int)(100 * exp(UD_CardSize->Position / (8 / log(4))) +
					100) - 100;
			}
		}
		/*
		 TCard *Card = m_Document->GetCard(m_nTargetCard);
		 Card->m_nSize = (int)(100 * exp(UD_CardSize->Position / (8 / log(4))) + 100) - 100;
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
	m_fStatisticsPos = 0.5f; // グラフの立ち上がりを0.5に
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
	}
	else if (Me_EditorRelated->Focused()) {
		Me_EditorRelated->SelectAll();
	}
	else if (Ed_Title->Focused()) {
		Ed_Title->SelectAll();
	}
	else if (Ed_TitleB->Focused()) {
		Ed_TitleB->SelectAll();
	}
	else if (Ed_LinkTitle->Focused()) {
		Ed_LinkTitle->SelectAll();
	}
	else if (Ed_FindCard->Focused()) {
		Ed_FindCard->SelectAll();
	}
	else if (LB_List->Focused() || PC_Client->ActivePage == TS_Browser) {
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			m_Document->GetCardByIndex(i)->m_bSelected = true;
		}
	}
	else if (PC_Client->ActivePage == TS_Drawing) {
		m_Drawing->SelectAll();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MF_PrintImageClick(TObject *Sender) {
	PC_Client->ActivePage = TS_Browser;
	if (PD_Image->Execute()) {
		TPrinter *Prntr = Printer();

		// 印刷する大きさ決定
		int printwidth = Prntr->PageWidth * 0.95; // 最大用紙の95%の領域に印刷
		int printheight = Prntr->PageHeight * 0.95;

		if (printheight > (printwidth * PB_Browser->Height) / PB_Browser->Width)
		{
			printheight = (printwidth * PB_Browser->Height) / PB_Browser->Width;
		}
		if (printwidth > (printheight * PB_Browser->Width) / PB_Browser->Height)
		{
			printwidth = (printheight * PB_Browser->Width) / PB_Browser->Height;
		}

		// 印刷する範囲を決定
		RECT printr = Rect(Prntr->PageWidth / 2 - printwidth / 2,
			Prntr->PageHeight / 2 - printheight / 2,
			Prntr->PageWidth / 2 + printwidth / 2,
			Prntr->PageHeight / 2 + printheight / 2);

		// プリンタに描画
		for (int i = 0; i < PD_Image->Copies; i++) {
			HRGN MyRgn;
			MyRgn = ::CreateRectRgn(printr.left, printr.top, printr.right,
				printr.bottom);

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

// ---------------------------------------------------------------------------
void TFo_Main::ApplyLanguageSetting() {
	if (!FileExists(LanguageFileName())) {
		return;
	}
	TFastIni *Ini = new TFastIni(LanguageFileName());

	// メインウインドウ
	TS_Browser->Caption = Ini->ReadString("MainWnd", "TS_Browser",
		TS_Browser->Caption); // ブラウザ
	TS_Editor->Caption = Ini->ReadString("MainWnd", "TS_Editor",
		TS_Editor->Caption); // エディタ
	TS_Drawing->Caption = Ini->ReadString("MainWnd", "TS_Drawing",
		TS_Drawing->Caption); // 図
	TS_Statistics->Caption = Ini->ReadString("MainWnd", "TS_Statistics",
		TS_Statistics->Caption); // 統計
	La_Files->Caption = Ini->ReadString("MainWnd", "La_Files",
		La_Files->Caption); // ファイル
	La_Cards->Caption = Ini->ReadString("MainWnd", "La_Cards",
		La_Cards->Caption); // カード
	La_Card->Caption = Ini->ReadString("MainWnd", "La_Card",
		La_Card->Caption); // カード
	La_Title->Caption = Ini->ReadString("MainWnd", "La_Title",
		La_Title->Caption); // 名前
	La_Size->Caption = Ini->ReadString("MainWnd", "La_Size",
		La_Size->Caption); // サイズ
	La_Label->Caption = Ini->ReadString("MainWnd", "La_Label",
		La_Label->Caption); // ラベル
	La_Link->Caption = Ini->ReadString("MainWnd", "La_Link",
		La_Link->Caption); // リンク
	La_LinkTitle->Caption = Ini->ReadString("MainWnd", "La_Title",
		La_LinkTitle->Caption); // 名前
	Ch_LinkDirection->Caption = Ini->ReadString("MainWnd", "Ch_LinkDirection",
		Ch_LinkDirection->Caption); // 向き
	Bu_LinkDirectionInverse->Caption =
		Ini->ReadString("MainWnd", "Bu_LinkDirectionInverse",
		Bu_LinkDirectionInverse->Caption); // 反転
	La_LinkLabel->Caption = Ini->ReadString("MainWnd", "La_Label",
		La_LinkLabel->Caption); // ラベル
	La_Arrange->Caption = Ini->ReadString("MainWnd", "La_Arrange",
		La_Arrange->Caption); // 自動整理
	La_Zoom->Caption = Ini->ReadString("MainWnd", "La_Zoom",
		La_Zoom->Caption); // ズーム
	Bu_Shuffle->Caption = Ini->ReadString("MainWnd", "Bu_Shuffle",
		Bu_Shuffle->Caption); // シャッフル
	SB_AutoScroll->Caption = Ini->ReadString("MainWnd", "SB_AutoScroll",
		SB_AutoScroll->Caption); // オートスクロール
	SB_AutoZoom->Caption = Ini->ReadString("MainWnd", "SB_AutoZoom",
		SB_AutoZoom->Caption); // オートズーム
	Bu_FindCard->Caption = Ini->ReadString("MainWnd", "Bu_FindCard",
		Bu_FindCard->Caption); // 検索
	SB_EditorRelated->Caption = Ini->ReadString("MainWnd", "SB_EditorRelated",
		SB_EditorRelated->Caption); // 関連テキスト
	La_PenColor->Caption = Ini->ReadString("MainWnd", "La_PenColor",
		La_PenColor->Caption); // 色:
	La_PenColorDefault->Caption =
		Ini->ReadString("MainWnd", "La_PenColorDefault",
		La_PenColorDefault->Caption); // 自動
	La_DZoom->Caption = Ini->ReadString("MainWnd", "La_Zoom",
		La_DZoom->Caption); // ズーム
	La_StatisticsKey->Caption = Ini->ReadString("MainWnd", "La_StatisticsKey",
		La_StatisticsKey->Caption); // キー
	SB_StatisticsSort->Caption = Ini->ReadString("MainWnd", "SB_StatisticsSort",
		SB_StatisticsSort->Caption); // ソート
	La_GlobalSearchTop->Caption =
		Ini->ReadString("MainWnd", "La_GlobalSearchTop",
		La_GlobalSearchTop->Caption);

	// メインメニュー
	M_File->Caption = Ini->ReadString("MainWnd", "M_File", M_File->Caption);
	// ファイル(&F)
	MF_New->Caption = Ini->ReadString("MainWnd", "MF_New", MF_New->Caption);
	MF_Open->Caption = Ini->ReadString("MainWnd", "MF_Open",
		MF_Open->Caption); // 開く(&O)...
	MF_Save->Caption = Ini->ReadString("MainWnd", "MF_Save",
		MF_Save->Caption); // 上書き保存(&S)
	MF_SaveAs->Caption = Ini->ReadString("MainWnd", "MF_SaveAs",
		MF_SaveAs->Caption); // 名前をつけて保存(&A)...
	MF_RecentFiles->Caption = Ini->ReadString("MainWnd", "MF_RecentFiles",
		MF_RecentFiles->Caption); // 最近使ったファイル(&R)
	MF_RecentFolders->Caption = Ini->ReadString("MainWnd", "MF_RecentFolders",
		MF_RecentFolders->Caption); // 最近使ったフォルダ(&C)
	MF_Import->Caption = Ini->ReadString("MainWnd", "MF_Import",
		MF_Import->Caption); // インポート(&I)
	MFI_TxtFile->Caption = Ini->ReadString("MainWnd", "MFI_TxtFile",
		MFI_TxtFile->Caption); // テキストファイル(&T)...
	MFI_HierarchicalTextFile->Caption =
		Ini->ReadString("MainWnd", "MFI_HierarchicalTextFile",
		MFI_HierarchicalTextFile->Caption);
	MFI_HierarchicalTextFile2->Caption =
		Ini->ReadString("MainWnd", "MFI_HierarchicalTextFile2",
		MFI_HierarchicalTextFile2->Caption);
	MFI_TxtFilesinaFolder->Caption =
		Ini->ReadString("MainWnd", "MFI_TxtFilesinaFolder",
		MFI_TxtFilesinaFolder->Caption);
	MF_Export->Caption = Ini->ReadString("MainWnd", "MF_Export",
		MF_Export->Caption); // エクスポート(&E)
	MFE_TxtFileCardTitle->Caption =
		Ini->ReadString("MainWnd", "MFE_TxtFileCardTitle",
		MFE_TxtFileCardTitle->Caption); // テキストファイル（カードタイトル）(&C)...
	MFE_TxtFile->Caption = Ini->ReadString("MainWnd", "MFE_TxtFile",
		MFE_TxtFile->Caption); // テキストファイル(&T)...
	MFE_TxtFiles->Caption = Ini->ReadString("MainWnd", "MFE_TxtFiles",
		MFE_TxtFiles->Caption); // 複数のテキストファイル(&X)...
	MFE_HierarchicalTextFile->Caption =
		Ini->ReadString("MainWnd", "MFE_HierarchicalTextFile",
		MFE_HierarchicalTextFile->Caption);
	MFE_HtmlFiles->Caption = Ini->ReadString("MainWnd", "MFE_HtmlFiles",
		MFE_HtmlFiles->Caption); // Htmlファイル(&H)...
	MFE_BMPFile->Caption = Ini->ReadString("MainWnd", "MFE_BMPFile",
		MFE_BMPFile->Caption);
	MFE_JPEGFile->Caption = Ini->ReadString("MainWnd", "MFE_JPEGFile",
		MFE_JPEGFile->Caption);
	MFE_ClipboardCardTitle->Caption =
		Ini->ReadString("MainWnd", "MFE_ClipboardCardTitle",
		MFE_ClipboardCardTitle->Caption);
	MFE_ClipboardCardText->Caption =
		Ini->ReadString("MainWnd", "MFE_ClipboardCardText",
		MFE_ClipboardCardText->Caption);
	MFE_ClipboardBMP->Caption = Ini->ReadString("MainWnd", "MFE_ClipboardBMP",
		MFE_ClipboardBMP->Caption);
	MF_PrintImage->Caption = Ini->ReadString("MainWnd", "MF_PrintImage",
		MF_PrintImage->Caption); // ブラウザ表示を印刷(&P)...
	MF_Exit->Caption = Ini->ReadString("MainWnd", "MF_Exit",
		MF_Exit->Caption); // 終了(&X)
	M_Edit->Caption = Ini->ReadString("MainWnd", "M_Edit", M_Edit->Caption);
	// 編集(&E)
	ME_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
		ME_Undo->Caption); // 元に戻す(&U)
	ME_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo", ME_Redo->Caption);
	ME_Cut->Caption = Ini->ReadString("MainWnd", "ME_Cut", ME_Cut->Caption);
	// 切り取り(&T)
	ME_Copy->Caption = Ini->ReadString("MainWnd", "ME_Copy",
		ME_Copy->Caption); // コピー(&C)
	ME_Paste->Caption = Ini->ReadString("MainWnd", "ME_Paste",
		ME_Paste->Caption); // 貼り付け(&P)
	ME_Delete->Caption = Ini->ReadString("MainWnd", "ME_Delete",
		ME_Delete->Caption); // 削除(&D)
	ME_SelectAll->Caption = Ini->ReadString("MainWnd", "ME_SelectAll",
		ME_SelectAll->Caption); // 全て選択(&A)
	ME_Find->Caption = Ini->ReadString("MainWnd", "ME_Find",
		ME_Find->Caption); // 検索(&F)
	ME_Replace->Caption = Ini->ReadString("MainWnd", "ME_Replace",
		ME_Replace->Caption); // (&E)
	ME_FindNext->Caption = Ini->ReadString("MainWnd", "ME_FindNext",
		ME_FindNext->Caption); // 次を検索(&N)
	ME_FindPrevious->Caption = Ini->ReadString("MainWnd", "ME_FindPrevious",
		ME_FindPrevious->Caption); // 前を検索(&V)
	ME_WebSearch->Caption = Ini->ReadString("MainWnd", "ME_WebSearch",
		ME_WebSearch->Caption); // W
	MD_SortCard->Caption = Ini->ReadString("MainWnd", "MD_SortCard",
		MD_SortCard->Caption); // カードの並び替え(&S)
	MDS_Title->Caption = Ini->ReadString("MainWnd", "MDS_Title",
		MDS_Title->Caption); // タイトル(&T)
	MDS_Title_I->Caption = Ini->ReadString("MainWnd", "MDS_Title_I",
		MDS_Title_I->Caption); // タイトル（降順）(&T)
	MDS_Date_Created->Caption = Ini->ReadString("MainWnd", "MDS_Date_Created",
		MDS_Date_Created->Caption); // 作成日時(&C)
	MDS_Date_Created_I->Caption =
		Ini->ReadString("MainWnd", "MDS_Date_Created_I",
		MDS_Date_Created_I->Caption); // 作成日時（降順）(&R)
	MDS_Date_Edited->Caption = Ini->ReadString("MainWnd", "MDS_Date_Edited",
		MDS_Date_Edited->Caption); // 編集日時(&E)
	MDS_Date_Edited_I->Caption = Ini->ReadString("MainWnd", "MDS_Date_Edited_I",
		MDS_Date_Edited_I->Caption); // 編集日時（降順）(&D)
	MDS_Date_Viewed->Caption = Ini->ReadString("MainWnd", "MDS_Date_Viewed",
		MDS_Date_Viewed->Caption); // 閲覧日時(&V)
	MDS_Date_Viewed_I->Caption = Ini->ReadString("MainWnd", "MDS_Date_Viewed_I",
		MDS_Date_Viewed_I->Caption); // 閲覧日時（降順）(&W)
	MDS_Score->Caption = Ini->ReadString("MainWnd", "MDS_Score",
		MDS_Score->Caption); // スコア(&S)
	MDS_Score_I->Caption = Ini->ReadString("MainWnd", "MDS_Score_I",
		MDS_Score_I->Caption); // スコア（降順）(&O)
	MDS_Shuffle->Caption = Ini->ReadString("MainWnd", "MDS_Shuffle",
		MDS_Shuffle->Caption); // シャッフル(&H)
	ME_BatchConversion->Caption =
		Ini->ReadString("MainWnd", "ME_BatchConversion",
		ME_BatchConversion->Caption); // 一括変更(&B)
	MEC_AllCards->Caption = Ini->ReadString("MainWnd", "MEC_AllCards",
		MEC_AllCards->Caption); // 全てのカードの形(&C)
	MEC_AllLinks->Caption = Ini->ReadString("MainWnd", "MEC_AllLinks",
		MEC_AllLinks->Caption); // 全てのリンクの形(&L)
	MEC_AllLinksDirection->Caption =
		Ini->ReadString("MainWnd", "MEC_AllLinksDirection",
		MEC_AllLinksDirection->Caption); // 全てのリンクの向き(&D)
	ME_Label->Caption = Ini->ReadString("MainWnd", "ME_Label",
		ME_Label->Caption); // ラベルの編集(&L)...
	ME_LinkLabel->Caption = Ini->ReadString("MainWnd", "ME_LinkLabel",
		ME_LinkLabel->Caption); // リンクラベルの編集(&I)...
	M_Insert->Caption = Ini->ReadString("MainWnd", "M_Insert",
		M_Insert->Caption); // 挿入(&I)
	MI_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
		MI_NewCard->Caption); // 新規カード(&C)
	MI_NewLink->Caption = Ini->ReadString("MainWnd", "MI_NewLink",
		MI_NewLink->Caption); // 新規リンク(&L)...
	MI_NewExtLink->Caption = Ini->ReadString("MainWnd", "MI_NewExtLink",
		MI_NewExtLink->Caption); // 新規外部リンク(&E)...
	MI_NewLabel->Caption = Ini->ReadString("MainWnd", "MI_NewLabel",
		MI_NewLabel->Caption); // 新規ラベル(&A)...
	MI_NewLinkLabel->Caption = Ini->ReadString("MainWnd", "MI_NewLinkLabel",
		MI_NewLinkLabel->Caption); // 新規リンクラベル(&N)...
	MI_LinktoAllCardswithDesignatedLabel->Caption =
		Ini->ReadString("MainWnd", "MI_LinktoAllCardswithDesignatedLabel",
		MI_LinktoAllCardswithDesignatedLabel->Caption);
	MI_NewCardLinkstoAllCardswithDesignatedLabel->Caption =
		Ini->ReadString("MainWnd",
		"MI_NewCardLinkstoAllCardswithDesignatedLabel",
		MI_NewCardLinkstoAllCardswithDesignatedLabel->Caption);
	MI_AddDesignatedLabeltoAllDestinationCards->Caption =
		Ini->ReadString("MainWnd", "MI_AddDesignatedLabeltoAllDestinationCards",
		MI_AddDesignatedLabeltoAllDestinationCards->Caption);
	M_Animation->Caption = Ini->ReadString("MainWnd", "M_Animation",
		M_Animation->Caption); // アニメーション(&A)
	MA_RandomFlash->Caption = Ini->ReadString("MainWnd", "MA_RandomFlash",
		MA_RandomFlash->Caption); // ランダムカード(&C)
	MA_RandomScroll->Caption = Ini->ReadString("MainWnd", "MA_RandomScroll",
		MA_RandomScroll->Caption); // ランダムカード2(&2)
	MA_RandomJump->Caption = Ini->ReadString("MainWnd", "MA_RandomJump",
		MA_RandomJump->Caption); // ランダムカード(&C)
	MA_RandomMap->Caption = Ini->ReadString("MainWnd", "MA_RandomMap",
		MA_RandomMap->Caption); // ランダムカード2(&2)
	MA_RandomTrace->Caption = Ini->ReadString("MainWnd", "MA_RandomTrace",
		MA_RandomTrace->Caption); // ランダムトレース(&T)
	M_View->Caption = Ini->ReadString("MainWnd", "M_View", M_View->Caption);
	// 表示(&V)
	MV_Font->Caption = Ini->ReadString("MainWnd", "MV_Font",
		MV_Font->Caption); // フォント(&F)
	MVF_DefaultSize->Caption = Ini->ReadString("MainWnd", "MVF_DefaultSize",
		MVF_DefaultSize->Caption); // 標準サイズ(&D)
	MVF_Magnify->Caption = Ini->ReadString("MainWnd", "MVF_Magnify",
		MVF_Magnify->Caption); // 拡大(&M)
	MVF_Reduce->Caption = Ini->ReadString("MainWnd", "MVF_Reduce",
		MVF_Reduce->Caption); // 縮小(&R)
	MVF_ChangeFont->Caption = Ini->ReadString("MainWnd", "MVF_ChangeFont",
		MVF_ChangeFont->Caption); // フォントの変更(&C)
	MV_Card->Caption = Ini->ReadString("MainWnd", "MV_Card",
		MV_Card->Caption); // カード(&C)
	MVC_CardShadow->Caption = Ini->ReadString("MainWnd", "MVC_CardShadow",
		MVC_CardShadow->Caption);
	MVC_CardGradation->Caption = Ini->ReadString("MainWnd", "MVC_CardGradation",
		MVC_CardGradation->Caption);
	MVC_TickerVisible->Caption = Ini->ReadString("MainWnd", "MVC_TickerVisible",
		MVC_TickerVisible->Caption);
	MVC_Ticker1Line->Caption = Ini->ReadString("MainWnd", "MVC_Ticker1Line",
		MVC_Ticker1Line->Caption);
	MVC_Ticker2Lines->Caption = Ini->ReadString("MainWnd", "MVC_Ticker2Lines",
		MVC_Ticker2Lines->Caption);
	MVC_Image->Caption = Ini->ReadString("MainWnd", "MVC_Image",
		MVC_Image->Caption); // イメージ(&I)
	MVC_Video->Caption = Ini->ReadString("MainWnd", "MVC_Video",
		MVC_Video->Caption); // ビデオ(&V)
	MVC_Drawing->Caption = Ini->ReadString("MainWnd", "MVC_Drawing",
		MVC_Drawing->Caption); // 図(&D)
	MV_Link->Caption = Ini->ReadString("MainWnd", "MV_Link",
		MV_Link->Caption); // リンク(&L)
	MVL_Link->Caption = Ini->ReadString("MainWnd", "MVL_Link",
		MVL_Link->Caption); // リンク線(&L)
	MVL_LinkHemming->Caption = Ini->ReadString("MainWnd", "MVL_LinkHemming",
		MVL_LinkHemming->Caption);
	MVL_LinkShadow->Caption = Ini->ReadString("MainWnd", "MVL_LinkShadow",
		MVL_LinkShadow->Caption);
	MVL_LinkDirection->Caption = Ini->ReadString("MainWnd", "MVL_LinkDirection",
		MVL_LinkDirection->Caption); // リンクの方向(&D)
	MVL_LinkName->Caption = Ini->ReadString("MainWnd", "MVL_LinkName",
		MVL_LinkName->Caption); // リンク名(&N)
	MV_Label->Caption = Ini->ReadString("MainWnd", "MV_Label",
		MV_Label->Caption); // ラベル(&A)
	MVL_LabelCircle->Caption = Ini->ReadString("MainWnd", "MVL_LabelCircle",
		MVL_LabelCircle->Caption); // ラベルの円(&C)
	MVL_LabelRectangle->Caption =
		Ini->ReadString("MainWnd", "MVL_LabelRectangle",
		MVL_LabelRectangle->Caption); // ラベルの矩形(&R)
	MVL_LabelName->Caption = Ini->ReadString("MainWnd", "MVL_LabelName",
		MVL_LabelName->Caption); // ラベルの円(&N)
	MV_Text->Caption = Ini->ReadString("MainWnd", "MV_Text",
		MV_Text->Caption); // 本文テキスト(&T)
	MVT_Text->Caption = Ini->ReadString("MainWnd", "MVT_Text",
		MVT_Text->Caption); // 本文テキスト(&T)
	MVT_Centering->Caption = Ini->ReadString("MainWnd", "MVT_Centering",
		MVT_Centering->Caption); // センタリング(&C)
	MVT_WordWrap->Caption = Ini->ReadString("MainWnd", "MVT_WordWrap",
		MVT_WordWrap->Caption); // 右端で折り返す(&W)
	MVT_EditInBrowser->Caption = Ini->ReadString("MainWnd", "MVT_EditInBrowser",
		MVT_EditInBrowser->Caption);
	MVT_AlwaysShowEditor->Caption =
		Ini->ReadString("MainWnd", "MVT_AlwaysShowEditor",
		MVT_AlwaysShowEditor->Caption);
	MVT_UnderTheCard->Caption = Ini->ReadString("MainWnd", "MVT_UnderTheCard",
		MVT_UnderTheCard->Caption);
	MVT_BrowserRight->Caption = Ini->ReadString("MainWnd", "MVT_BrowserRight",
		MVT_BrowserRight->Caption);
	MVT_BrowserBottom->Caption = Ini->ReadString("MainWnd", "MVT_BrowserBottom",
		MVT_BrowserBottom->Caption);
	MV_Others->Caption = Ini->ReadString("MainWnd", "MV_Others",
		MV_Others->Caption); // その他(&O)
	MV_Score->Caption = Ini->ReadString("MainWnd", "MV_Score",
		MV_Score->Caption); // スコア(&S)
	MVS_Score->Caption = Ini->ReadString("MainWnd", "MVS_Score",
		MVS_Score->Caption); // スコア(&S)
	MVS_Authenticity->Caption = Ini->ReadString("MainWnd", "MVS_Authenticity",
		MVS_Authenticity->Caption); // 信頼度(&A)
	MVS_StartingPoint->Caption = Ini->ReadString("MainWnd", "MVS_StartingPoint",
		MVS_StartingPoint->Caption); // 開始地点(&S)
	MVS_Destination->Caption = Ini->ReadString("MainWnd", "MVS_Destination",
		MVS_Destination->Caption); // 目的地(&D)
	MVS_Links_Out->Caption = Ini->ReadString("MainWnd", "MVS_Links_Out",
		MVS_Links_Out->Caption); // リンク数（出力）(&O)
	MVS_Links_In->Caption = Ini->ReadString("MainWnd", "MVS_Links_In",
		MVS_Links_In->Caption); // リンク数（入力）(&I)
	MVS_Links_Total->Caption = Ini->ReadString("MainWnd", "MVS_Links_Total",
		MVS_Links_Total->Caption); // リンク数（合計）(&T)
	MVS_Links_InOut->Caption = Ini->ReadString("MainWnd", "MVS_Links_InOut",
		MVS_Links_InOut->Caption); // リンク数（入力-出力）(&L)
	MVS_TextLength->Caption = Ini->ReadString("MainWnd", "MVS_TextLength",
		MVS_TextLength->Caption); // 本文の長さ(&X)
	MV_Read->Caption = Ini->ReadString("MainWnd", "MV_Read",
		MV_Read->Caption); // 読み上げ(&R)
	MVR_Read->Caption = Ini->ReadString("MainWnd", "MVR_Read",
		MVR_Read->Caption); // 読み上げ(&R)
	MVR_ReadSetting->Caption = Ini->ReadString("MainWnd", "MVR_ReadSetting",
		MVR_ReadSetting->Caption); // 読み上げの設定(&E)
	MVO_CardList->Caption = Ini->ReadString("MainWnd", "MVO_CardList",
		MVO_CardList->Caption); //
	MVO_FileList->Caption = Ini->ReadString("MainWnd", "MVO_FileList",
		MVO_FileList->Caption); //
	MVO_Overview->Caption = Ini->ReadString("MainWnd", "MVO_Overview",
		MVO_Overview->Caption); // 全体像を表示(&O)
	MVO_ChangeFourgroundColor->Caption =
		Ini->ReadString("MainWnd", "MVO_ChangeFourgroundColor",
		MVO_ChangeFourgroundColor->Caption); // 前景色を変更(&F)...
	MVO_ChangeBackgroundColor->Caption =
		Ini->ReadString("MainWnd", "MVO_ChangeBackgroundColor",
		MVO_ChangeBackgroundColor->Caption); // 背景色を変更(&B)...
	MVO_WallPaper->Caption = Ini->ReadString("MainWnd", "MVO_WallPaper",
		MVO_WallPaper->Caption); // 壁紙(&W)...
	MVO_FixWallPaper->Caption = Ini->ReadString("MainWnd", "MVO_FixWallPaper",
		MVO_FixWallPaper->Caption); // 壁紙を固定(&I)
	MVO_TileWallPaper->Caption = Ini->ReadString("MainWnd", "MVO_TileWallPaper",
		MVO_TileWallPaper->Caption); // 壁紙を並べて表示(&T)
	MVO_AntiAliasing->Caption = Ini->ReadString("MainWnd", "MVO_AntiAliasing",
		MVO_AntiAliasing->Caption); // アンチエイリアス(&A)
	MVO_BGAnimation->Caption = Ini->ReadString("MainWnd", "MVO_BGAnimation",
		MVO_BGAnimation->Caption); // &C
	MVO_BGAnimationType->Caption =
		Ini->ReadString("MainWnd", "MVO_BGAnimationType",
		MVO_BGAnimationType->Caption); // &K
	MVO_NoScrollLag->Caption = Ini->ReadString("MainWnd", "MVO_NoScrollLag",
		MVO_NoScrollLag->Caption); // &N
	MVO_CursorAnimation->Caption =
		Ini->ReadString("MainWnd", "MVO_CursorAnimation",
		MVO_CursorAnimation->Caption); // &U
	MVOB_MovingLine->Caption = Ini->ReadString("MainWnd", "MVOB_MovingLine",
		MVOB_MovingLine->Caption); // &M
	MVOB_Bubble->Caption = Ini->ReadString("MainWnd", "MVOB_Bubble",
		MVOB_Bubble->Caption); // &B
	MVOB_Snow->Caption = Ini->ReadString("MainWnd", "MVOB_Snow",
		MVOB_Snow->Caption); // &S
	MVOB_CherryBlossom->Caption =
		Ini->ReadString("MainWnd", "MVOB_CherryBlossom",
		MVOB_CherryBlossom->Caption); // &C
	MV_ChangeLanguage->Caption = Ini->ReadString("MainWnd", "MV_ChangeLanguage",
		MV_ChangeLanguage->Caption); // 言語の変更(&H)
	MV_FullScreen->Caption = Ini->ReadString("MainWnd", "MV_FullScreen",
		MV_FullScreen->Caption); // フルスクリーン表示(&U)
	MV_StatusBar->Caption = Ini->ReadString("MainWnd", "MV_StatusBar",
		MV_StatusBar->Caption); // ステータスバー(&B)
	M_Help->Caption = Ini->ReadString("MainWnd", "M_Help", M_Help->Caption);
	// ヘルプ(&H)
	MH_Contents->Caption = Ini->ReadString("MainWnd", "MH_Contents",
		MH_Contents->Caption); // 目次(&C)...
	MH_FrieveSite->Caption = Ini->ReadString("MainWnd", "MH_FrieveSite",
		MH_FrieveSite->Caption); // Frieveのサイト(&F)...
	MH_FIPSite->Caption = Ini->ReadString("MainWnd", "MH_FIPSite",
		MH_FIPSite->Caption); // Frieve Editorのサイト(&O)...
	MH_CheckLatest->Caption = Ini->ReadString("MainWnd", "MH_CheckLatest",
		MH_CheckLatest->Caption); // 最新バージョンを確認(&L)...
	MH_About->Caption = Ini->ReadString("MainWnd", "MH_About",
		MH_About->Caption); // バージョン情報(&A)...

	// ポップアップメニュー（エディタ）
	PE_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
		PE_Undo->Caption); // 元に戻す(&U)
	PE_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo", PE_Redo->Caption);
	PE_Cut->Caption = Ini->ReadString("MainWnd", "ME_Cut", PE_Cut->Caption);
	// 切り取り(&T)
	PE_Copy->Caption = Ini->ReadString("MainWnd", "ME_Copy",
		PE_Copy->Caption); // コピー(&C)
	PE_Paste->Caption = Ini->ReadString("MainWnd", "ME_Paste",
		PE_Paste->Caption); // 貼り付け(&P)
	PE_Delete->Caption = Ini->ReadString("MainWnd", "ME_Delete",
		PE_Delete->Caption); // 削除(&D)
	PE_SelectAll->Caption = Ini->ReadString("MainWnd", "ME_SelectAll",
		PE_SelectAll->Caption); // 全て選択(&A)
	PE_CutToNewCard->Caption = Ini->ReadString("MainWnd", "PM_CutToNewCard",
		PE_CutToNewCard->Caption);
	PE_CutToNewCardTitle->Caption =
		Ini->ReadString("MainWnd", "PM_CutToNewCardTitle",
		PE_CutToNewCardTitle->Caption);
	PE_CutToNewCardTitleWithLink->Caption =
		Ini->ReadString("MainWnd", "PM_CutToNewCardTitleWithLink",
		PE_CutToNewCardTitleWithLink->Caption);

	// ポップアップメニュー（リスト）
	PL_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
		PL_NewCard->Caption); // 新規カード(&C)
	PL_DeleteCard->Caption = Ini->ReadString("MainWnd", "PM_DeleteCard",
		PL_DeleteCard->Caption);
	PL_CardProperty->Caption = Ini->ReadString("MainWnd", "PM_Property",
		PL_CardProperty->Caption);

	PI_NewLink->Caption = Ini->ReadString("MainWnd", "MI_NewLink",
		PI_NewLink->Caption);
	PI_DeleteLink->Caption = Ini->ReadString("MainWnd", "PM_DeleteLink",
		PI_DeleteLink->Caption);

	// ポップアップメニュー（ブラウザ）
	PBN_Select->Caption = Ini->ReadString("MainWnd", "PBN_Select",
		PBN_Select->Caption);
	PBN_Link->Caption = Ini->ReadString("MainWnd", "PBN_Link",
		PBN_Link->Caption);
	PBN_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
		PBN_Undo->Caption); // 元に戻す(&U)
	PBN_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo",
		PBN_Redo->Caption);
	PBN_Paste->Caption = Ini->ReadString("MainWnd", "ME_Paste",
		PBN_Paste->Caption); // 貼り付け(&P)
	PBN_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
		PBN_NewCard->Caption); // 新規カード(&C)

	PBC_Select->Caption = Ini->ReadString("MainWnd", "PBN_Select",
		PBC_Select->Caption);
	PBC_Link->Caption = Ini->ReadString("MainWnd", "PBN_Link",
		PBC_Link->Caption);
	PBC_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
		PBN_Undo->Caption); // 元に戻す(&U)
	PBC_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo",
		PBN_Redo->Caption);
	PBC_Cut->Caption = Ini->ReadString("MainWnd", "ME_Cut",
		PBC_Cut->Caption); // 切り取り(&T)
	PBC_Copy->Caption = Ini->ReadString("MainWnd", "ME_Copy",
		PBC_Copy->Caption); // コピー(&C)
	PBC_Paste->Caption = Ini->ReadString("MainWnd", "ME_Paste",
		PBC_Paste->Caption); // 貼り付け(&P)
	PBC_WebSearch->Caption = Ini->ReadString("MainWnd", "ME_WebSearch",
		ME_WebSearch->Caption); // W
	PBC_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
		PBC_NewCard->Caption); // 新規カード(&C)
	PBC_NewChildCard->Caption = Ini->ReadString("MainWnd", "PM_NewChildCard",
		PBC_NewChildCard->Caption);
	PBC_NewBrotherCard->Caption =
		Ini->ReadString("MainWnd", "PM_NewBrotherCard",
		PBC_NewBrotherCard->Caption);
	PBC_NewLink->Caption = Ini->ReadString("MainWnd", "MI_NewLink",
		PBC_NewLink->Caption);
	PBC_NewExtLink->Caption = Ini->ReadString("MainWnd", "MI_NewExtLink",
		PBC_NewExtLink->Caption);
	PBC_NewLabel->Caption = Ini->ReadString("MainWnd", "MI_NewLabel",
		PBC_NewLabel->Caption);
	PBC_DeleteCard->Caption = Ini->ReadString("MainWnd", "PM_DeleteCard",
		PBC_DeleteCard->Caption);
	PBC_SetAsDefault->Caption = Ini->ReadString("MainWnd", "PM_SetAsDefault",
		PBC_SetAsDefault->Caption);
	PBC_Property->Caption = Ini->ReadString("MainWnd", "PM_Property",
		PBC_Property->Caption);

	PBL_Select->Caption = Ini->ReadString("MainWnd", "PBN_Select",
		PBL_Select->Caption);
	PBL_Link->Caption = Ini->ReadString("MainWnd", "PBN_Link",
		PBL_Link->Caption);
	PBL_Undo->Caption = Ini->ReadString("MainWnd", "ME_Undo",
		PBL_Undo->Caption); // 元に戻す(&U)
	PBL_Redo->Caption = Ini->ReadString("MainWnd", "ME_Redo",
		PBL_Redo->Caption);
	PBL_WebSearch->Caption = Ini->ReadString("MainWnd", "ME_WebSearch",
		ME_WebSearch->Caption); // W
	PBL_NewCard->Caption = Ini->ReadString("MainWnd", "MI_NewCard",
		PBL_NewCard->Caption); // 新規カード(&C)
	PBL_InsertCard->Caption = Ini->ReadString("MainWnd", "PM_InsertCard",
		PBL_InsertCard->Caption);
	PBL_NewLinkLabel->Caption = Ini->ReadString("MainWnd", "MI_NewLinkLabel",
		PBL_NewLinkLabel->Caption);
	PBL_DeleteLink->Caption = Ini->ReadString("MainWnd", "PM_DeleteLink",
		PBL_DeleteLink->Caption);
	PBL_SetAsDefault->Caption = Ini->ReadString("MainWnd", "PM_SetAsDefault",
		PBL_SetAsDefault->Caption);

	// ポップアップメニュー（アレンジ）
	PAT_Normalize->Caption = Ini->ReadString("MainWnd", "PAT_Normalize",
		PAT_Normalize->Caption); // ノーマライズ(&N)
	PAT_Repulsion->Caption = Ini->ReadString("MainWnd", "PAT_Repulsion",
		PAT_Repulsion->Caption); // 反発(&R)
	PAT_Link->Caption = Ini->ReadString("MainWnd", "PAT_Link",
		PAT_Link->Caption); // リンク(&N)
	PAT_Label->Caption = Ini->ReadString("MainWnd", "PAT_Label",
		PAT_Label->Caption); // ラベル(&A)
	PAT_Index->Caption = Ini->ReadString("MainWnd", "PAT_Index",
		PAT_Index->Caption); // 順序(&I)
	PAT_SoftLink->Caption = Ini->ReadString("MainWnd", "PAT_SoftLink",
		PAT_SoftLink->Caption); // リンク(ソフト)(&K)
	PAT_SoftLabel->Caption = Ini->ReadString("MainWnd", "PAT_SoftLabel",
		PAT_SoftLabel->Caption); // ラベル(ソフト)(&B)
	PAT_SoftIndex->Caption = Ini->ReadString("MainWnd", "PAT_SoftIndex",
		PAT_SoftIndex->Caption); // 順序(ソフト)(&D)
	PAT_Matrix->Caption = Ini->ReadString("MainWnd", "PAT_Matrix",
		PAT_Matrix->Caption); // マトリクス(&M)
	PAT_MatrixIndex->Caption = Ini->ReadString("MainWnd", "PAT_MatrixIndex",
		PAT_MatrixIndex->Caption); // 順序(マトリクス)(&E)
	PAT_Similarity->Caption = Ini->ReadString("MainWnd", "PAT_Similarity",
		PAT_Similarity->Caption); // 類似度(&S)
	PAT_SimilaritySoft->Caption =
		Ini->ReadString("MainWnd", "PAT_SimilaritySoft",
		PAT_SimilaritySoft->Caption); // 類似度(ソフト)
	PAT_TreeRadial->Caption = Ini->ReadString("MainWnd", "PAT_TreeRadial",
		PAT_TreeRadial->Caption); // ツリー(放射状)(&T)

	// ポップアップメニュー（統計）
	PMSK_Label->Caption = Ini->ReadString("MainWnd", "PMSK_Label",
		PMSK_Label->Caption); // ラベル(&L)
	PMSK_NumberOfLink->Caption = Ini->ReadString("MainWnd", "PMSK_NumberOfLink",
		PMSK_NumberOfLink->Caption); // リンク数(&N)
	PMSK_CreatedDate->Caption = Ini->ReadString("MainWnd", "PMSK_CreatedDate",
		PMSK_CreatedDate->Caption); // 作成日時(&C)
	PMSK_EditedDate->Caption = Ini->ReadString("MainWnd", "PMSK_EditedDate",
		PMSK_EditedDate->Caption); // 編集日時(&E)
	PMSK_ViewedDate->Caption = Ini->ReadString("MainWnd", "PMSK_ViewedDate",
		PMSK_ViewedDate->Caption); // 閲覧日時(&V)
	PMSKL_Total->Caption = Ini->ReadString("MainWnd", "PMSKL_Total",
		PMSKL_Total->Caption); // 合計(&T)
	PMSKL_Source->Caption = Ini->ReadString("MainWnd", "PMSKL_Source",
		PMSKL_Source->Caption); // リンク元(&S)
	PMSKL_Destination->Caption = Ini->ReadString("MainWnd", "PMSKL_Destination",
		PMSKL_Destination->Caption); // リンク先(&D)
	PMSKC_Year->Caption = Ini->ReadString("MainWnd", "PMSK_Year",
		PMSKC_Year->Caption); // 年毎に集計(&Y)
	PMSKC_Month->Caption = Ini->ReadString("MainWnd", "PMSK_Month",
		PMSKC_Month->Caption); // 月毎に集計(&M)
	PMSKC_Day->Caption = Ini->ReadString("MainWnd", "PMSK_Day",
		PMSKC_Day->Caption); // 日毎に集計(&D)
	PMSKC_Week->Caption = Ini->ReadString("MainWnd", "PMSK_Week",
		PMSKC_Week->Caption); // 週毎に集計(&W)
	PMSKC_Hour->Caption = Ini->ReadString("MainWnd", "PMSK_Hour",
		PMSKC_Hour->Caption); // 時間毎に集計(&H)
	PMSKE_Year->Caption = Ini->ReadString("MainWnd", "PMSK_Year",
		PMSKE_Year->Caption); // 年毎に集計(&Y)
	PMSKE_Month->Caption = Ini->ReadString("MainWnd", "PMSK_Month",
		PMSKE_Month->Caption); // 月毎に集計(&M)
	PMSKE_Day->Caption = Ini->ReadString("MainWnd", "PMSK_Day",
		PMSKE_Day->Caption); // 日毎に集計(&D)
	PMSKE_Week->Caption = Ini->ReadString("MainWnd", "PMSK_Week",
		PMSKE_Week->Caption); // 週毎に集計(&W)
	PMSKE_Hour->Caption = Ini->ReadString("MainWnd", "PMSK_Hour",
		PMSKE_Hour->Caption); // 時間毎に集計(&H)
	PMSKV_Year->Caption = Ini->ReadString("MainWnd", "PMSK_Year",
		PMSKV_Year->Caption); // 年毎に集計(&Y)
	PMSKV_Month->Caption = Ini->ReadString("MainWnd", "PMSK_Month",
		PMSKV_Month->Caption); // 月毎に集計(&M)
	PMSKV_Day->Caption = Ini->ReadString("MainWnd", "PMSK_Day",
		PMSKV_Day->Caption); // 日毎に集計(&D)
	PMSKV_Week->Caption = Ini->ReadString("MainWnd", "PMSK_Week",
		PMSKV_Week->Caption); // 週毎に集計(&W)
	PMSKV_Hour->Caption = Ini->ReadString("MainWnd", "PMSK_Hour",
		PMSKV_Hour->Caption); // 時間毎に集計(&H)

	// その他のテキスト

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

	MLText.EditCardTitle = Ini->ReadString("Common", "EditCardTitle",
		MLText.EditCardTitle);
	MLText.EditCardText = Ini->ReadString("Common", "EditCardText",
		MLText.EditCardText);
	MLText.EditLinkTitle = Ini->ReadString("Common", "EditLinkTitle",
		MLText.EditLinkTitle);
	MLText.EditDrawing = Ini->ReadString("Common", "EditDrawing",
		MLText.EditDrawing);

	MLText.ChangeSize = Ini->ReadString("Common", "ChangeSize",
		MLText.ChangeSize);
	MLText.FixCardPosition = Ini->ReadString("Common", "FixCardPosition",
		MLText.FixCardPosition);
	MLText.SetAsRootCard = Ini->ReadString("Common", "SetAsRootCard",
		MLText.SetAsRootCard);

	MLText.EditLink = Ini->ReadString("Common", "EditLink", MLText.EditLink);

	MLText.AddLabel = Ini->ReadString("Common", "AddLabel", MLText.AddLabel);
	MLText.DeleteLabel = Ini->ReadString("Common", "DeleteLabel",
		MLText.DeleteLabel);
	MLText.ChangeLabel = Ini->ReadString("Common", "ChangeLabel",
		MLText.ChangeLabel);
	MLText.LabelName = Ini->ReadString("Common", "LabelName", MLText.LabelName);
	MLText.SelectLinkDestination =
		Ini->ReadString("Common", "SelectLinkDestination",
		MLText.SelectLinkDestination);
	MLText.SelectLabelOfDestination =
		Ini->ReadString("Common", "SelectLabelOfDestination",
		MLText.SelectLabelOfDestination);
	MLText.SelectLabel = Ini->ReadString("Common", "SelectLabel",
		MLText.SelectLabel);

	MLText.Color = Ini->ReadString("Common", "Color", MLText.Color);

	MLText.ArrangeCards = Ini->ReadString("Common", "ArrangeCards",
		MLText.ArrangeCards);

	MLText.ProcessingTextAnalysis =
		Ini->ReadString("Common", "ProcessingTextAnalysis",
		MLText.ProcessingTextAnalysis);

	MLText.Enable = Ini->ReadString("Common", "Enable", MLText.Enable); // 有効
	MLText.Fold = Ini->ReadString("Common", "Fold", MLText.Fold); // 折畳
	MLText.Show = Ini->ReadString("Common", "Show", MLText.Show); // 表示
	MLText.Hide = Ini->ReadString("Common", "Hide", MLText.Hide); // 隠蔽
	MLText.NoAssign = Ini->ReadString("Common", "NoAssign",
		MLText.NoAssign); // 未指定
	MLText.NoLabel = Ini->ReadString("Common", "NoLabel", MLText.NoLabel);
	// ラベルなし
	MLText.SelectTargetCard = Ini->ReadString("Common", "SelectTargetCard",
		MLText.SelectTargetCard); // リンク先カードの選択
	MLText.Old = Ini->ReadString("Common", "Old", MLText.Old); // 古い順
	MLText.New = Ini->ReadString("Common", "New", MLText.New); // 新しい順
	MLText.Near = Ini->ReadString("Common", "Near", MLText.Near); // 近い順
	MLText.Older = Ini->ReadString("Common", "Older", MLText.Older); // より古い
	MLText.Newer = Ini->ReadString("Common", "Newer", MLText.Newer); // より新しい
	MLText.SaveCheck = Ini->ReadString("Common", "SaveCheck", MLText.SaveCheck);
	MLText.FailedToSave = Ini->ReadString("Common", "FailedToSave",
		MLText.FailedToSave);
	MLText.NotFound = Ini->ReadString("Common", "NotFound", MLText.NotFound);
	MLText.AANotEnoughMemory = Ini->ReadString("Common", "AANotEnoughMemory",
		MLText.AANotEnoughMemory);
	delete Ini;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVL_LabelNameClick(TObject *Sender) {
	SettingView.m_bLabelNameVisible = !SettingView.m_bLabelNameVisible;
	m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MFE_TxtFileCardTitleClick(TObject *Sender) {
	if (SD_Txt->Execute()) {
		ExportCardText(0, true, SD_Txt->FileName);
		/*
		 TStringList *SL = new TStringList();

		 for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
		 TCard *Card = m_Document->GetCardByIndex(i);
		 SL->Add(Card->m_Title);
		 }

		 SL->SaveToFile(SD_Txt->FileName);
		 delete SL;
		 */
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MVC_EnglishClick(TObject *Sender) {
	MVC_English->Checked = true;
	SettingView.m_Language = "";
	ShowMessage("Please restart Frieve Editor.");
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::ChangeLanguageOnClick(TObject *Sender) {
	TMenuItem *MI = (TMenuItem*)Sender;
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
	BackupSub(MD_SortCard->Caption + UnicodeString(" ") + ((TMenuItem*)Sender)
		->Caption);
	m_Document->SortCards(4, ((TMenuItem*)Sender)->Tag);
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
	// 使われているのにロードされていない画像をロードし、使われていない画像を破棄する

	bool result = false;

	// 使用中フラグリセット
	for (int i = 0; i < m_ImageList->Count; i++) {
		GetImage(i)->m_bUsed = false;
	}

	// カードループ
	if (SettingView.m_bImageVisible)
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			TCard *Card = m_Document->GetCardByIndex(i);
			if (Card->m_ImageFN != "") { // Card->m_bVisible &&
				// イメージリストからイメージ検索
				TCardImage *Image = SearchImage(Card->m_ImageFN);
				if (Image) {
					// 見つかった！
					Image->m_bUsed = true;
				}
				else if (Card->m_bVisible) {
					// 見つからない、かつ表示中→ロード
					// Screen->Cursor = crHourGlass;
					if (!result || SettingView.m_bLoadImageatOnce) {
						m_ImageList->Add(new TCardImage(Card->m_ImageFN));
					}
					result = true;
				}
			}
		}

	// 未使用イメージの破棄
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
	// 使われているのにロードされていないビデオをロードし、使われていないビデオを破棄する

	bool result = false;

	// 使用中フラグリセット
	for (int i = 0; i < m_VideoList->Count; i++) {
		GetVideo(i)->m_bUsed = false;
	}

	// カードループ
	if (SettingView.m_bVideoVisible)
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			TCard *Card = m_Document->GetCardByIndex(i);
			if (Card->m_bVisible && Card->m_VideoFN != "") {
				// ビデオリストからビデオ検索
				TCardVideo *Video = SearchVideo(Card->m_VideoFN);
				if (Video) {
					// 見つかった！
					Video->m_bUsed = true;
				}
				else {
					// 見つからない→ロード
					Screen->Cursor = crHourGlass;
					if (!result || SettingView.m_bLoadImageatOnce) {
						m_VideoList->Add(new TCardVideo(Card->m_VideoFN));
					}
					result = true;
				}
			}
		}

	// 未使用ビデオの破棄
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
	TMenuItem *MI = (TMenuItem*)Sender;
	SettingView.m_nImageLimitation = MI->Tag;
	m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVR_ReadClick(TObject *Sender) {
	SettingView.m_bRead = !SettingView.m_bRead;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVR_ReadSettingClick(TObject *Sender) {
	/*
	 if (m_TTS){
	 m_TTS->GeneralDlg((long)Handle, NULL);
	 }
	 */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVR_ChangeAgentClick(TObject *Sender) {
	/*
	 if (OD_ACS->Execute()){
	 SettingView.m_ReadAgentFile = OD_ACS->FileName;
	 UnloadAgent();
	 LoadAgent();
	 }
	 */
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVT_WordWrapClick(TObject *Sender) {
	SettingView.m_bTextWordWrap = !SettingView.m_bTextWordWrap;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MVS_ScoreClickType(TObject *Sender) {
	SettingView.m_ScoreType = ((TMenuItem*)Sender)->Tag;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_TestClick(TObject *Sender) {
	int doccount = m_Document->m_Cards->Count;

	TWideStringList *WS = new TWideStringList();
	for (int i = 0; i < doccount; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		/*
		 WideString W = Card->m_Title;
		 for (int i2 = 0 ; i2 < Card->m_Lines->Count ; i2++){
		 W += " ";
		 W += Card->m_Lines->Strings[i2];
		 }
		 WS->Add(W);
		 */
		WS->Add(Card->m_Title + "\n" + Card->m_Lines->Text);
	}
	unsigned int t;
	t = timeGetTime();
	float fdummy;
	bool bdummy = false;
	TTextDecomposer *TD = new TTextDecomposer(WS, 10, 100, 0.0f, 0.0f, fdummy,
		bdummy);
	RE_Edit->Clear();
	RE_Edit->Lines->BeginUpdate();
	RE_Edit->Lines->Add(UnicodeString("Time elapsed : ") +
		IntToStr((int)(timeGetTime() - t)) + "ms");
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
	 for (int i2 = TD->m_Gram[i - 1]->From(idx) ; i2 <= TD->m_Gram[i - 1]->To(idx) ; i2++){
	 WS += TD->m_Gram[i]->Strings(i2) + ", ";
	 }
	 RE_Edit->Lines->Add(WS);
	 idx = rand() % (TD->m_Gram[i - 1]->To(idx) + 1 - TD->m_Gram[i - 1]->From(idx)) + TD->m_Gram[i - 1]->From(idx);
	 }
	 }
	 RE_Edit->Lines->Add("");
	 }

	 {
	 //Search動作チェック
	 for (int ii = 0 ; ii < TD->m_Gram[0]->Count ; ii++){
	 int idx = TD->m_Gram[0]->Search(0, TD->m_Gram[0]->Count - 1, TD->m_Gram[0]->Strings(ii));
	 if (idx != ii){
	 RE_Edit->Lines->Add(IntToStr(ii) + " : " + IntToStr(idx));
	 }
	 }
	 }

	 {
	 //Enabledがfalseの組み合わせ
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

	// Doc-単語出現確率格納用
	t = timeGetTime();
	TSMatrix *SM = new TSMatrix();

	// TDの結果からSMを埋める
	// ドキュメントループ
	for (int id = 0; id < doccount; id++) {
		// 文字ループ
		WideString doc = WideLowerCase(WS->Strings(id));
		int doclen = doc.Length();
		for (int ic = 0; ic < doclen; ic++) {
			int len = 1;
			int start = 0;
			int end = TD->m_Gram[0]->Count - 1;
			while (start >= 0) {
				TWSandValueList *WSVL = TD->m_Gram[len - 1];
				WideString c = doc.SubString(ic + 1, len);
				// 1文字単語からこの文字を検索
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
				}
				else {
					break;
				}
			}
		}
	}

	// 重複削除
	unsigned int t2 = timeGetTime();
	SM->Finalize(true);
	RE_Edit->Lines->Add(UnicodeString("SM : ") +
		IntToStr((int)(timeGetTime() - t)) + "ms  (Ex : " +
		IntToStr((int)(t2 - t)) + "ms  Fn : " +
		IntToStr((int)(timeGetTime() - t2)) + "ms)");
	RE_Edit->Lines->Add(UnicodeString("SM Items : ") + SM->m_Items->Count);
	RE_Edit->Lines->Add("");

	// 単純類似Matrix生成
	t = timeGetTime();
	FreeSimMatrix();
	SM->DeleteSameCol(true);
	// SM->MergeCol(500, 0.9f, 30);//500単語毎に0.9以上の類似度の単語をまとめる。単語が50以下になるまで繰り返す
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
	t2 = timeGetTime();
	m_SimMatrix->Finalize();
	RE_Edit->Lines->Add(UnicodeString("SM Items (after DeleteSameCol): ") +
		SM->m_Items->Count);
	RE_Edit->Lines->Add(UnicodeString("DM : ") +
		IntToStr((int)(timeGetTime() - t)) + "ms  (Cos : " +
		IntToStr((int)(t2 - t)) + "ms  Fn : " +
		IntToStr((int)(timeGetTime() - t2)) + "ms)");

	delete SM;

	delete TD;
	delete WS;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PAT_ArrangeTypeClick(TObject *Sender) {
	bool tree = Bu_ArrangeType->Tag >= 1000 && Bu_ArrangeType->Tag <= 1999;
	Bu_ArrangeType->Tag = ((TMenuItem*)Sender)->Tag;
	if (SB_Arrange->Down && tree != (Bu_ArrangeType->Tag >=
		1000 && Bu_ArrangeType->Tag <= 1999)) {
		m_Document->RefreshList();
	}
	PrepareArrange();
}

// ---------------------------------------------------------------------------
void TFo_Main::PrepareArrange() {
	if (SB_Arrange->Down) {
		switch (Bu_ArrangeType->Tag) {
			// case 200://200の際はIterationの中で毎回呼ぶ
		case 202: // Matrix(Link)
		case 203: // Matrix(Label)
			PrepareMatrixArrange(Bu_ArrangeType->Tag);
			break;
		case 500: // Similarity
		case 600: // Similarity(Soft)
			SB_ArrangeRefreshClick(this);
			break;
		case 700: // Matrix(Similarity)
			SB_ArrangeRefreshClick(this);
			PrepareMatrixArrange(Bu_ArrangeType->Tag);
			break;
		}

		if (Bu_ArrangeType->Tag >= 1000 && Bu_ArrangeType->Tag <= 1999) {
			// 階層表示
			m_Document->RefreshList();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::Bu_ArrangeTypeClick(TObject *Sender) {
	TPoint P;
	P.x = ((TControl*)Sender)->Left;
	P.y = ((TControl*)Sender)->Top;
	P = Pa_BrowserTop->ClientToScreen(P);
	PM_ArrangeType->Popup(P.x, P.y);
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshSimMatrix() {
	Refresh();
	Ti_Check->Enabled = false;

	Fo_Progress->PFunc = this;
	Fo_Progress->Left = Fo_Main->Left + Fo_Main->Width / 2;
	Fo_Progress->Top = Fo_Main->Top + Fo_Main->Height / 2;
	Fo_Progress->La_Progress->Caption = MLText.ProcessingTextAnalysis;
	Fo_Progress->ShowModal();
	/*
	 int doccount = m_Document->m_Cards->Count;

	 FreeSimMatrix();
	 TWideStringList *WS = new TWideStringList();
	 for (int i = 0 ; i < doccount ; i++){
	 TCard *Card = m_Document->GetCardByIndex(i);
	 WS->Add(Card->m_Title + "\n" + Card->m_Lines->Text);
	 }
	 TTextDecomposer *TD = new TTextDecomposer(WS, 10, 100);

	 //Doc-単語出現確率格納用
	 TSMatrix *SM = new TSMatrix();

	 //TDの結果からSMを埋める
	 //ドキュメントループ
	 for (int id = 0 ; id < doccount ; id++){
	 //文字ループ
	 WideString doc = WideLowerCase(WS->Strings(id));
	 int doclen = doc.Length();
	 for (int ic = 0 ; ic < doclen ; ic++){
	 int len = 1;
	 int start = 0;
	 int end = TD->m_Gram[0]->Count - 1;
	 while (start >= 0 && start <= end){
	 TWSandValueList *WSVL = TD->m_Gram[len - 1];
	 WideString c = doc.SubString(ic + 1, len);
	 //1文字単語からこの文字を検索
	 int idx = WSVL->Search(start, end, c);
	 if (idx >= 0){
	 int sn = WSVL->SN(idx);
	 if (sn >= 0){
	 SM->Add(sn, id, 1.0f / WSVL->Values(idx));
	 }
	 start = WSVL->From(idx);
	 end = WSVL->To(idx);
	 len++;
	 }else{
	 break;
	 }
	 }
	 }
	 }

	 //重複削除
	 SM->Finalize(true);

	 //単純類似Matrix生成
	 SM->DeleteSameCol(true);
	 //SM->MergeCol(500, 0.9f, 30);//500単語毎に0.9以上の類似度の単語をまとめる。単語が50以下になるまで繰り返す
	 SM->PrepareCosRow();
	 m_SimMatrix = new TDMatrix(doccount);
	 for (int i = 0 ; i < doccount ; i++){
	 TCard *Card = m_Document->GetCardByIndex(i);
	 m_SimMatrix->ID(i) = Card->m_nID;
	 for (int i2 = i + 1 ; i2 < doccount ; i2++){
	 float cos = SM->CosRow(i, i2);
	 m_SimMatrix->S(i, i2) = m_SimMatrix->S(i2, i) = cos;
	 }
	 }
	 m_SimMatrix->Finalize();

	 delete SM;

	 delete TD;
	 delete WS;
	 */
	Ti_Check->Enabled = true;
}

// ---------------------------------------------------------------------------
void TFo_Main::ProgressFunc() {
	// Progressからの呼び出し用
	RefreshSimMatrix_();
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshSimMatrix_() {
	Screen->Cursor = crHourGlass;

	FreeSimMatrix();
	int doccount = m_Document->m_Cards->Count;

	TWideStringList *WS = new TWideStringList();
	for (int i = 0; i < doccount; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		WS->Add(Card->m_Title + "\n" + Card->m_Lines->Text);
	}
	TTextDecomposer *TD = new TTextDecomposer(WS, 10, 100, 0.0f, 0.6f,
		Fo_Progress->fPos, Fo_Progress->m_bTerminated);

	// Doc-単語出現確率格納用
	TSMatrix *SM = new TSMatrix();

	// TDの結果からSMを埋める
	// ドキュメントループ
	for (int id = 0; id < doccount && !Fo_Progress->m_bTerminated; id++) {
		// 文字ループ
		WideString doc = WideLowerCase(WS->Strings(id));
		int doclen = doc.Length();
		for (int ic = 0; ic < doclen; ic++) {
			int len = 1;
			int start = 0;
			int end = TD->m_Gram[0]->Count - 1;
			while (start >= 0 && start <= end) {
				TWSandValueList *WSVL = TD->m_Gram[len - 1];
				WideString c = doc.SubString(ic + 1, len);
				// 1文字単語からこの文字を検索
				int idx = WSVL->Search(start, end, c);
				if (idx >= 0) {
					int sn = WSVL->SN(idx);
					if (sn >= 0) {
						SM->Add(sn, id, 1.0f / WSVL->Values(idx));
					}
					start = WSVL->From(idx);
					end = WSVL->To(idx);
					len++;
				}
				else {
					break;
				}
			}
		}
		Fo_Progress->fPos = 0.6f + (id + 1.0) * 0.15f / doccount;
		Application->ProcessMessages();
	}

	if (!Fo_Progress->m_bTerminated) {
		// 重複削除
		SM->Finalize(true, &Fo_Progress->fPos,
			&Fo_Progress->m_bTerminated, 0.05f);
	}

	if (!Fo_Progress->m_bTerminated) {
		// 単純類似Matrix生成
		SM->DeleteSameCol(true);
		// SM->MergeCol(500, 0.9f, 30);//500単語毎に0.9以上の類似度の単語をまとめる。単語が50以下になるまで繰り返す
		SM->PrepareCosRow();
		Fo_Progress->fPos = 0.95f;
		Application->ProcessMessages();
	}

	if (!Fo_Progress->m_bTerminated) {
		m_SimMatrix = new TDMatrix(doccount);
		for (int i = 0; i < doccount; i++) {
			TCard *Card = m_Document->GetCardByIndex(i);
			m_SimMatrix->ID(i) = Card->m_nID;
			for (int i2 = i + 1; i2 < doccount; i2++) {
				float cos = SM->CosRow(i, i2);
				m_SimMatrix->S(i, i2) = m_SimMatrix->S(i2, i) = cos;
			}
		}
		Fo_Progress->fPos = 1.05f;
		Application->ProcessMessages();

		m_SimMatrix->Finalize();
	}

	delete SM;

	delete TD;
	delete WS;

	Screen->Cursor = crDefault;
}

// ---------------------------------------------------------------------------
void TFo_Main::FreeSimMatrix() {
	if (m_SimMatrix) {
		delete m_SimMatrix;
		m_SimMatrix = NULL;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_ArrangeRefreshClick(TObject *Sender) {
	RefreshSimMatrix();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_ArrangeClick(TObject *Sender) {
	PrepareArrange();
	if (SB_Arrange->Down) {
		BackupSub(MLText.ArrangeCards);
	}
	else {
		// これまでTree表示していた
		bool tree = Bu_ArrangeType->Tag >= 1000 && Bu_ArrangeType->Tag <= 1999;
		if (tree) {
			m_Document->RefreshList();
		}
	}
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
		// 壁紙が指定されていれば消す
		SettingView.m_WallPaper = "";
		RefreshWallPaper();
	}
	else {
		OD_WallPaper->InitialDir = ExtractFilePath(ParamStr(0)) + "wallpaper";
		if (OD_WallPaper->Execute()) {
			/*
			 if (LowerCase(OD_WallPaper->FileName).Pos(LowerCase(ExtractFilePath(ParamStr(0)))) == 1){
			 SettingView.m_WallPaper = OD_WallPaper->FileName.SubString(ExtractFilePath(ParamStr(0)).Length() + 1, OD_WallPaper->FileName.Length());
			 }else{
			 SettingView.m_WallPaper = OD_WallPaper->FileName;
			 }
			 */
			SettingView.m_WallPaper = OD_WallPaper->FileName;
			RefreshWallPaper();
		}
	}
	m_bRedrawRequested = true;
}

// ---------------------------------------------------------------------------
void TFo_Main::RefreshWallPaper() {
	Im_Wall->Enabled = false;
	if (SettingView.m_WallPaper != "") {
		if (FileExists(SettingView.m_WallPaper)) {
			try {
				Im_Wall->Picture->LoadFromFile(SettingView.m_WallPaper);
				Im_Wall->Enabled = true;
			}
			catch (...) {
			}
		}
	}
}

// ---------------------------------------------------------------------------
float TFo_Main::MoveDistance(float sx, float sy, float dx, float dy,
	int direction) {
	// direction方向に動く際のs座標からd座標への距離
	switch (direction) {
	case 0: // 左
		if (sx > dx) {
			return (sx - dx) * (sx - dx) + (dy - sy) * (dy - sy) * 9.0f;
		}
		else {
			return -1.0f;
		}
	case 1: // 上
		if (sy > dy) {
			return (sy - dy) * (sy - dy) + (dx - sx) * (dx - sx) * 9.0f;
		}
		else {
			return -1.0f;
		}
	case 2: // 右
		if (sx < dx) {
			return (dx - sx) * (dx - sx) + (dy - sy) * (dy - sy) * 9.0f;
		}
		else {
			return -1.0f;
		}
	case 3: // 下
		if (sy < dy) {
			return (dy - sy) * (dy - sy) + (dx - sx) * (dx - sx) * 9.0f;
		}
		else {
			return -1.0f;
		}
	}
	return -1.0f;
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
	SettingView.m_nTickerLines = ((TMenuItem*)Sender)->Tag;
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
void TFo_Main::ExportBMP(int operation, UnicodeString Text) {
	// ブラウザ画像を保存orコピー(0=BMP, 1=JPEG, 2=コピー）
	PC_Client->ActivePage = TS_Browser;

	// バックグラウンドバッファ作成
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

		// 数倍のサイズのビットマップ作成
		Graphics::TBitmap *BMP2 = new Graphics::TBitmap();
		BMP2->Width = m_nBrowserWidth * SettingView.m_nAntiAliasX;
		BMP2->Height = m_nBrowserHeight * SettingView.m_nAntiAliasX;
		BMP2->PixelFormat = pf32bit;
		// BMPに描画
		m_fFontZoom = SettingView.m_nAntiAliasX;
		PaintSub(BMP2->Canvas);

		// アンチエイリアシング
		int **P2 = new int*[BMP2->Height];
		for (int iy = 0; iy < BMP2->Height; iy++) {
			P2[iy] = (int*)BMP2->ScanLine[iy];
		}
		for (int iy = 0; iy < BMP->Height; iy++) {
			int *PW = (int*)BMP->ScanLine[iy];
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
		delete[]P2;

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
	}
	else {
		// BMPに描画
		PaintSub(BMP->Canvas);
	}

	/*
	 //バックグラウンドバッファ作成
	 Graphics::TBitmap *BMP = new Graphics::TBitmap();
	 m_nBrowserWidth = PB_Browser->Width;
	 m_nBrowserHeight = PB_Browser->Height;
	 BMP->Width = m_nBrowserWidth;
	 BMP->Height = m_nBrowserHeight;

	 //BMPに描画
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
		// クリップボードにコピー
		Clipboard()->Assign(BMP);
		break;
	}

	// BMP破棄
	delete BMP;
}

// ---------------------------------------------------------------------------
void __fastcall TFo_Main::MFE_ClipboardBMPClick(TObject *Sender) {
	ExportBMP(2, "");
	/*
	 PC_Client->ActivePage = TS_Browser;

	 //バックグラウンドバッファ作成
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

	 //数倍のサイズのビットマップ作成
	 Graphics::TBitmap *BMP2 = new Graphics::TBitmap();
	 BMP2->Width = m_nBrowserWidth * SettingView.m_nAntiAliasX;
	 BMP2->Height = m_nBrowserHeight * SettingView.m_nAntiAliasX;
	 BMP2->PixelFormat = pf32bit;
	 //BMPに描画
	 m_fFontZoom = SettingView.m_nAntiAliasX;
	 PaintSub(BMP2->Canvas);

	 //アンチエイリアシング
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
	 //BMPに描画
	 PaintSub(BMP->Canvas);
	 }

	 //クリップボードにコピー
	 Clipboard()->Assign(BMP);

	 //BMP破棄
	 delete BMP;
	 */
}

// ---------------------------------------------------------------------------
void TFo_Main::ExportCardText(int operation, bool btitle, UnicodeString Text) {
	TStringList *SL = new TStringList();

	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		if (btitle) {
			SL->Add(Card->m_Title);
		}
		else {
			SL->Add(Card->m_Lines->Text);
		}
	}

	switch (operation) {
	case 0:
		try {
			SL->SaveToFile(Text);
		}
		catch (...) {
			ShowMessage(MLText.FailedToSave + SD_Txt->FileName +
				UnicodeString("."));
		}
		break;
	case 1:
		Clipboard()->SetTextBuf(SL->Text.c_str());
		break;
	}

	delete SL;
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
	// 階層テキスト読み込み
	if (OD_Txt->Execute()) {
		BackupSub(MF_Import->Caption + UnicodeString(" ") +
			MFI_HierarchicalTextFile->Caption);

		TStringList *SL = new TStringList();
		TStringList *SLK = new TStringList();
		for (int i = 0; i < OD_Txt->Files->Count; i++) {
			SL->LoadFromFile(OD_Txt->Files->Strings[i]);

			// 階層記号決定
			for (int il = 0; il < SL->Count; il++)
				if (SL->Strings[il] != "") {
					WideString W = SL->Strings[il];
					SLK->Add(W.SubString(1, 1));
				}
			SLK->Sort();

			UnicodeString K; // 階層記号
			if (SLK->Count) {
				UnicodeString Last; // 階層記号
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
					}
					else {
						count = 1;
						Last = SLK->Strings[i2];
					}
				}
			}
			SLK->Clear();

			TList *HList = new TList(); // 現在の各階層保存用

			// トップノード
			TCard *Top = m_Document->NewCard(m_Document->m_Cards->Count);
			Top->m_bTop = true;
			HList->Add(Top);
			Top->m_Title = ExtractFileNameOnly(OD_Txt->Files->Strings[i]);

			// 行ループ
			for (int il = 0; il < SL->Count; il++)
				if (Trim(SL->Strings[il]) != "") {
					TCard *Card =
						m_Document->NewCard(m_Document->m_Cards->Count);
					int level = 1;
					WideString W = SL->Strings[il];
					while (true) {
						if (W.SubString(1, 1) == K) {
							W = W.SubString(2, W.Length());
							level++;
						}
						else {
							break;
						}
					}
					Card->m_Title = W;

					// 階層管理
					// 自分以上の階層を削除
					for (int ih = HList->Count - 1; ih >= level; ih--) {
						HList->Delete(ih);
					}
					// 階層をスキップした場合（0->2など）、間をNULLで埋める
					for (int ih = HList->Count; ih < level; ih++) {
						HList->Add(NULL);
					}
					// 現在の階層に自分を書く
					HList->Add(Card);

					// 親カードを見つける
					TCard *Parent = NULL;
					int plevel = level - 1;
					while (plevel >= 0 && Parent == NULL) {
						if (plevel < HList->Count) {
							Parent = (TCard*)HList->Items[plevel];
						}
						if (!Parent) {
							plevel--;
						}
					}

					// 親カードからリンク
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

void TFo_Main::AddTxtsInFolder(UnicodeString TopDir, TCard *Parent) {
	TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
	Card->m_Title = ExtractFileName(TopDir);
	if (Parent) {
		TLink *Link = m_Document->NewLink();
		Link->m_nFromID = Parent->m_nID;
		Link->m_nDestID = Card->m_nID;
	}
	else {
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

void __fastcall TFo_Main::MFI_TxtFilesinaFolderClick(TObject *Sender) {
	UnicodeString Dir;
	if (SelectDirectory(L"", L"", Dir)) {
		BackupSub(MF_Import->Caption + UnicodeString(" ") +
			MFI_TxtFilesinaFolder->Caption);

		AddTxtsInFolder(Dir, NULL);
		/*
		 //ファイル列挙
		 TStringList *Files = new TStringList();
		 FileListCreator(Dir, Files, ".txt");
		 Files->Sort();

		 //フォルダ列挙
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

		 //フォルダカード作成
		 TList *DirCards = new TList();
		 for (int i = 0 ; i < Files->Count ; i++){
		 TCard *Card = m_Document->NewCard(m_Document->m_Cards->Count);
		 Card->m_Title = Dirs->Strings[i];

		 //このDirのあるフォルダを探す
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
		 //リンクを張る
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

		 //このDirのあるフォルダを探す
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
		 //リンクを張る
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
	SettingView.m_nAntiAliasX = ((TMenuItem*)Sender)->Tag;
	m_bRedrawRequested = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Bu_BrowserTestClick(TObject *Sender) {
	// Undo Redoテスト
	UnicodeString str;
	bool b = m_UndoRedo->GetCanUndo(str);
	if (b) {
		ShowMessage(str);
		m_UndoRedo->Undo(m_Document, m_nCurrentCard, RE_Edit->SelStart,
			RE_Edit->SelLength, &m_nNextCardID, &m_nNextSelStart,
			&m_nNextSelLength);
	}
	else {
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
	SettingView.m_nBGAnimationType = ((TMenuItem*)Sender)->Tag;
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
	// 選択されているカードで、一番最近触ったカードに移動
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
		// 選択設定
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			TCard *Card = m_Document->GetCardByIndex(i);
			Card->m_bSelected = LB_List->Selected[i];
		}

		TCard *Card = m_Document->GetCardByIndex(LB_List->ItemIndex);
		if (Card) {
			if (Card->m_bSelected) {
				m_nTargetCard = Card->m_nID;
				m_nTargetLink = -1;
			}
			else {
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
	m_nLastTimeOut = timeGetTime();
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
	SettingView.m_nEditInBrowserPos = ((TMenuItem*)Sender)->Tag;
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
	}
	else if (PC_Client->ActivePage == TS_Browser) {
		if (m_nTargetCard >= 0) {
			m_bTitleEditRequested = true;
		}
		else if (m_nTargetLink >= 0 && Ed_LinkTitle->Enabled) {
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
				UnicodeString("\\") + LB_FileList->Items->Strings
				[LB_FileList->ItemIndex] + UnicodeString(".fip"), false,
				// SoftLoad
				false); // Do not refresh recent
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::LB_FileListMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	m_bFileListDragging = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::LB_FileListMouseUp(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	m_bFileListDragging = false;
}

// ---------------------------------------------------------------------------
void TFo_Main::SetCardAssign() {
	// Paint時以外でLabelのFoldを考慮する必要がある際に呼ぶ
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		m_CardAssign[i] = i;
	}

	// カードループ
	// まず全てのカードのfold状態を調べる
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		Card->m_bLabelIsFold = m_Document->LabelIsFold(Card);
	}

	// カードループ
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		if (Card->m_bLabelIsFold) {
			// すべてfoldされている

			if (m_CardAssign[i] == i) {
				// 代表カード
				// カードループ
				// 同じラベルを持つカードをiの示すカードにアサインする
				for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
					if (i != i2) {
						TCard *Card2 = m_Document->GetCardByIndex(i2);

						if (m_Document->LabelIsSame(Card, Card2)) {
							// 同じラベル構造のカード

							// 最初に見つかった同じラベル構造のカードに座標をアサインする
							// 選択中でないカードは、一番裏（最初のほう）のカードにAssignされる
							m_CardAssign[i2] = i;
						}
					}
			} /* else{
			 //選択中のカードでもなく、代表カードでない

			 int idxbak = m_CardAssign[i];

			 //カードループ
			 //後に出てきたこのカードを代表カードに
			 for (int i2 = 0 ; i2 < m_Document->m_Cards->Count ; i2++) if (m_CardAssign[i2] == idxbak){
			 m_CardAssign[i2] = i;
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
	// バックグラウンドバッファ作成
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

	// 背景塗りつぶし
	B->Color = TColor(m_nBGColor);
	C->FillRect(Rect(0, 0, BMP->Width, BMP->Height));

	int itemcount = BMP->Height / m_GlobalSearchItemHeight;
	int largechange = itemcount;
	if (largechange < 1) {
		largechange = 1;
	}
	Sc_GlobalSearch->LargeChange = largechange;

	// 現在Index取得
	m_GlobalSearchCursorIndex = -1;
	for (int i = 0; i < m_GlobalSearchResult->Count; i++) {
		TCard *Card = m_Document->GetCard((int)m_GlobalSearchResult->Items[i]);
		if (Card) {
			if (m_nTargetCard == Card->m_nID) {
				m_GlobalSearchCursorIndex = i;
			}
		}
	}

	for (int i = 0; i < itemcount + 1; i++) {
		int Index = i + Sc_GlobalSearch->Position;

		if (Index < m_GlobalSearchResult->Count) {
			// 範囲内
			TCard *Card =
				m_Document->GetCard((int)m_GlobalSearchResult->Items[Index]);
			if (Card) {
				// 対象カードが存在

				// 枠を描画
				float sizex = 1.0f;
				TColor c = GetCardColor(Card, sizex);
				B->Style = bsSolid;
				// if (m_nTargetCard == Card->m_nID){
				if (Index == m_GlobalSearchCursorIndex) {
					P->Color = HalfColor(c, m_nBGColor, 0.33f);
					B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
					P->Width = 3;
				}
				else {
					P->Color = HalfColor(c, m_nBGColor, 0.75f);
					B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
					P->Width = 1;
				}

				int top = m_GlobalSearchItemHeight * i;
				C->RoundRect(0, top + 2, BMP->Width - 3,
					top + m_GlobalSearchItemHeight - 2,
					m_GlobalSearchItemHeight / 4, m_GlobalSearchItemHeight / 4);

				HRGN MyRgn;
				MyRgn = ::CreateRectRgn(textheight / 2 - 2, 0,
					BMP->Width - textheight / 2, BMP->Height);
				::SelectClipRgn(C->Handle, MyRgn);

				B->Style = bsClear;
				// カードタイトルを描画
				{
					int foundat = 0;
					WideString Title = DecodeES(Card->m_Title, " ");
					if (m_GlobalSearchOption & 0x1) {
						// タイトルから検索
						if (m_GlobalSearchOption & 0x10) {
							// 大文字と小文字を区別しない
							WideString Target = WideLowerCase(Title);
							foundat =
								Target.Pos
								(WideLowerCase(m_GlobalSearchKeyword));
						}
						else {
							// 大文字と小文字を区別
							foundat = Title.Pos(m_GlobalSearchKeyword);
						}
					}
					if (foundat == 0) {
						// タイトルを単純表示
						C->TextOut(textheight / 2 - 2,
							top + textheight / 2, Title);
					}
					else {
						// 見つかったところを強調表示

						WideString S1;

						// 左側を表示
						S1 = Title.SubString(1, foundat - 1);
						int widthsum = 0;
						C->TextOut(textheight / 2 - 2,
							top + textheight / 2, S1);
						widthsum += C->TextWidth(S1);

						// キーワードを表示
						S1 = Title.SubString(foundat,
							m_GlobalSearchKeyword.Length());
						F->Color = clRed;
						F->Style = TFontStyles() << fsBold;
						C->TextOut(textheight / 2 - 2 + widthsum,
							top + textheight / 2, S1);
						widthsum += C->TextWidth(S1);
						F->Color = TColor(m_nFGColor);
						F->Style = TFontStyles();

						// 右側を表示
						S1 = Title.SubString
							(foundat + m_GlobalSearchKeyword.Length(),
							Title.Length());
						C->TextOut(textheight / 2 - 2 + widthsum,
							top + textheight / 2, S1);
					}
				}

				{
					int foundat = 0;
					WideString Text = Card->m_Lines->Text;
					if (m_GlobalSearchOption & 0x2) {
						// 本文から検索
						if (m_GlobalSearchOption & 0x10) {
							WideString Target = WideLowerCase(Text);
							foundat =
								Target.Pos
								(WideLowerCase(m_GlobalSearchKeyword));
						}
						else {
							foundat = Text.Pos(m_GlobalSearchKeyword);
						}
					}

					int maxlen = BMP->Width / C->TextWidth(" ");
					if (foundat == 0) {
						// 本文を単純表示
						Text = Text.SubString(1, maxlen);
						C->TextOut(textheight / 2 - 2,
							top + textheight + textheight / 2, Text);
					}
					else {
						int start =
							foundat -
							(maxlen - m_GlobalSearchKeyword.Length()) / 2;
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
						while (C->TextWidth(Text) > BMP->Width - textheight / 2)
						{
							int leftlen = foundat - 1;
							int rightlen =
								Text.Length() - m_GlobalSearchKeyword.Length() -
								(foundat - 1);
							int delpos;
							if (leftlen < rightlen || foundat <= 1) {
								delpos = 1; // 後ろを削除
							}
							else {
								delpos = 2; // 頭を削除
							}
							Text = Text.SubString(delpos, Text.Length() - 1);
							if (delpos == 2) {
								foundat--;
							}
						}
						// C->TextOut(textheight / 2 - 2, top + textheight + textheight / 2, Text);
						WideString S1;

						// 左側を表示
						S1 = Text.SubString(1, foundat - 1);
						int widthsum = 0;
						C->TextOut(textheight / 2 - 2,
							top + textheight + textheight / 2, S1);
						widthsum += C->TextWidth(S1);

						// キーワードを表示
						S1 = Text.SubString(foundat,
							m_GlobalSearchKeyword.Length());
						F->Color = clRed;
						F->Style = TFontStyles() << fsBold;
						C->TextOut(textheight / 2 - 2 + widthsum,
							top + textheight + textheight / 2, S1);
						widthsum += C->TextWidth(S1);
						F->Color = TColor(m_nFGColor);
						F->Style = TFontStyles();

						// 右側を表示
						S1 = Text.SubString
							(foundat + m_GlobalSearchKeyword.Length(),
							Text.Length());
						C->TextOut(textheight / 2 - 2 + widthsum,
							top + textheight + textheight / 2, S1);
					}
				}::SelectClipRgn(C->Handle, NULL);
				::DeleteObject(MyRgn);
			}
		}
	}

	// 前景に描画
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
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	Sc_GlobalSearch->SetFocus();
	int Index = Y / m_GlobalSearchItemHeight + Sc_GlobalSearch->Position;
	if (Index >= 0 && Index < m_GlobalSearchResult->Count) {
		TCard *Card =
			m_Document->GetCard((int)m_GlobalSearchResult->Items[Index]);
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
			m_Document->GetCard((int)m_GlobalSearchResult->Items[targetindex]);
		m_Document->ClearCardSelection();
		Card->m_bSelected = true;
		m_nTargetCard = Card->m_nID;

		Sc_GlobalSearch->Tag = 1;
		if (targetindex < Sc_GlobalSearch->Position) {
			Sc_GlobalSearch->Position = targetindex;
		}
		else if (targetindex >= Sc_GlobalSearch->Position + itemcount - 1) {
			Sc_GlobalSearch->Position = targetindex - itemcount + 1;
		}
		Sc_GlobalSearch->Tag = 0;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PE_CutToNewCardTitleWithLinkClick(TObject *Sender) {
	BackupSub(PE_CutToNewCardTitleWithLink->Caption);

	m_bDoNotBackup = true;

	// カット
	ME_CutClick(Sender);

	TMemo *Me = new TMemo(this);
	Me->Parent = this;
	Me->WordWrap = false;
	Me->PasteFromClipboard();

	int from = m_nTargetCard;

	for (int i = 0; i < Me->Lines->Count; i++) {
		UnicodeString S = Trim(Me->Lines->Strings[i]);
		if (S != "") {
			// カードの新規作成
			MI_NewCardClick(Sender);

			// カードの表示書き換え
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
	// 階層テキスト読み込み
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

			TList *HList = new TList(); // 現在の各階層保存用

			// トップノード
			TCard *Top = m_Document->NewCard(m_Document->m_Cards->Count);
			Top->m_bTop = true;
			HList->Add(Top);
			Top->m_Title = ExtractFileNameOnly(OD_Txt->Files->Strings[i]);

			// 行ループ
			for (int il = 0; il < SL->Count; il++)
				if (Trim(SL->Strings[il]) != "") {
					TCard *Card =
						m_Document->NewCard(m_Document->m_Cards->Count);
					int level = 1;
					WideString W = SL->Strings[il];
					while (true) {
						if (W.SubString(1, 1) == K) {
							W = W.SubString(2, W.Length());
							level++;
						}
						else {
							break;
						}
					}
					Card->m_Title = W;

					// 階層管理
					// 自分以上の階層を削除
					for (int ih = HList->Count - 1; ih >= level; ih--) {
						HList->Delete(ih);
					}
					// 階層をスキップした場合（0->2など）、間をNULLで埋める
					for (int ih = HList->Count; ih < level; ih++) {
						HList->Add(NULL);
					}
					// 現在の階層に自分を書く
					HList->Add(Card);

					// 親カードを見つける
					TCard *Parent = NULL;
					int plevel = level - 1;
					while (plevel >= 0 && Parent == NULL) {
						if (plevel < HList->Count) {
							Parent = (TCard*)HList->Items[plevel];
						}
						if (!Parent) {
							plevel--;
						}
					}

					// 親カードからリンク
					if (Parent) {
						TLink *Link = m_Document->NewLink();
						Link->m_nFromID = Parent->m_nID;
						Link->m_nDestID = Card->m_nID;
					}

					// 本文設定
					while (il + 1 < SL->Count) {
						if (SL->Strings[il + 1].SubString(1, 1) != K) {
							Card->m_Lines->Add(SL->Strings[il + 1]);
							il++;
						}
						else {
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
	// PgUp,Downによるズーム
	switch (Key) {
	case 33: // PgUp
		{
			int pos = TB_Zoom->Position;
			pos -= (TB_Zoom->Max - TB_Zoom->Min) / 50;
			if (pos < TB_Zoom->Min) {
				pos = TB_Zoom->Min;
			}
			TB_Zoom->Position = pos;
		} Key = 0;
		break;
	case 34: // PgDn
		{
			int pos = TB_Zoom->Position;
			pos += (TB_Zoom->Max - TB_Zoom->Min) / 50;
			if (pos > TB_Zoom->Max) {
				pos = TB_Zoom->Max;
			}
			TB_Zoom->Position = pos;
		} Key = 0;
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
	// 関連テキストを移動
	if (SB_EditorRelated->Down && LB_Link->ItemIndex >= 0) {
		TLink *Link = m_Document->GetLinkByIndex
			((int)m_LinkIndexes->Items[LB_Link->ItemIndex]);
		if (Link) {
			int id;
			if (Link->m_nFromID == m_nCurrentCard) {
				id = Link->m_nDestID;
			}
			else if (Link->m_nDestID == m_nCurrentCard) {
				id = Link->m_nFromID;
			}
			TCard *Card = m_Document->GetCard(id);
			WideString Target = Me_EditorRelated->Text;
			WideString Key = "Title : " + Card->m_Title;
			int foundat = Target.Pos(Key);
			if (foundat >= 1) {
				// 見つかった
				Me_EditorRelated->HideSelection = false;
				Me_EditorRelated->SelStart = Target.Length() - 1;
				Me_EditorRelated->SelLength = 1;
				Me_EditorRelated->SelStart = foundat - 1;
				Me_EditorRelated->SelLength = Key.Length();
				// UnicodeString(SearchKeyword).Length();
			}
			else {
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
};

// ---------------------------------------------------------------------------
TSRectToCard::TSRectToCard() {
	m_Rect = Rect(0, 0, 0, 0);
	m_CardIDList = new TList();
}

// ---------------------------------------------------------------------------
TSRectToCard::~TSRectToCard() {
	delete m_CardIDList;
}

// ---------------------------------------------------------------------------
void TFo_Main::ClearStatisticsRectToCard() {
	for (int i = 0; i < m_StatisticsRectToCard->Count; i++) {
		delete(TSRectToCard *)m_StatisticsRectToCard->Items[i];
	}
	m_StatisticsRectToCard->Clear();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_StatisticsPaint(TObject *Sender) {
	// バックグラウンドバッファ作成
	Graphics::TBitmap *BMP = new Graphics::TBitmap();
	BMP->Width = PB_Statistics->Width;
	BMP->Height = PB_Statistics->Height;

	// 準備、初期化
	TCanvas *C = BMP->Canvas;
	TBrush *B = C->Brush;
	TPen *P = C->Pen;
	TFont *F = C->Font;
	F->Name = RE_Edit->Font->Name;
	F->Charset = RE_Edit->Font->Charset;
	F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);
	m_nFontHeight = C->TextHeight(" ");

	// 背景塗りつぶし
	B->Color = TColor(m_nBGColor);
	B->Style = bsSolid;
	C->FillRect(Rect(0, 0, BMP->Width, BMP->Height));

	ClearStatisticsRectToCard();

	{
		// 棒グラフ

		// 名前と値
		TStringList *Name = new TStringList();
		TList *Value = new TList();
		TList *Color = new TList();

		const int topmargin = m_nFontHeight * 2.5, leftmargin =
			m_nFontHeight / 2;

		switch (Bu_StatisticsKey->Tag) {
		case 0: {
				// ラベルの数
				const int labeltype = 0;
				for (int i = 0; i < m_Document->m_Labels[labeltype]->Count; i++)
				{
					TCardLabel *Label =
						m_Document->GetLabelByIndex(labeltype, i);
					if (Label->m_bEnable) {
						TSRectToCard *SRTC = new TSRectToCard();
						Name->Add(Label->m_Name);
						float count = 0.0f;
						for (int ic = 0; ic < m_Document->m_Cards->Count; ic++)
						{
							TCard *Card = m_Document->GetCardByIndex(ic);
							if (Card->m_Labels->Contain(i + 1)) {
								count += 1.0f;
								SRTC->m_CardIDList->Add((void*)Card->m_nID);
							}
						}
						Value->Add((void*)*(int*)&count);
						Color->Add((void*)Label->m_nColor);
						m_StatisticsRectToCard->Add(SRTC);
					}
				}
			} break;
		case 10000: // Total
		case 10100: // Source
		case 10200: // Destination
			{
				// リンクの数
				int *NumLinks = new int[m_Document->m_Cards->Count];
				memset(NumLinks, 0, sizeof(int) * m_Document->m_Cards->Count);

				// リンクループ
				for (int i = 0; i < m_Document->m_Links->Count; i++) {
					TLink *Link = m_Document->GetLinkByIndex(i);

					if (Bu_StatisticsKey->Tag == 10000 ||
						Bu_StatisticsKey->Tag == 10100) {
						// ソース側
						int idx = m_Document->SearchCardIndex(Link->m_nFromID);
						NumLinks[idx]++;
					}

					if (Bu_StatisticsKey->Tag == 10000 ||
						Bu_StatisticsKey->Tag == 10200) {
						// リンク先
						int idx = m_Document->SearchCardIndex(Link->m_nDestID);
						NumLinks[idx]++;
					}
				}

				// 最大リンク数算出
				int maxlinks = 0;
				for (int i = 0; i < m_Document->m_Cards->Count; i++) {
					if (maxlinks < NumLinks[i]) {
						maxlinks = NumLinks[i];
					}
				}

				// アイテム作成
				for (int i = 0; i <= maxlinks; i++) {
					TSRectToCard *SRTC = new TSRectToCard();
					Name->Add(IntToStr(i) + " Links");
					float count = 0.0f;
					for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
						if (NumLinks[ic] == i) {
							TCard *Card = m_Document->GetCardByIndex(ic);
							SRTC->m_CardIDList->Add((void*)Card->m_nID);
							count += 1.0f;
						}
					}
					Value->Add((void*)*(int*)&count);
					Color->Add((void*)GetColor(i * 1.0f / (maxlinks + 1),
						0, 255));
					m_StatisticsRectToCard->Add(SRTC);
				}

				delete[]NumLinks;
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

				// 要素を列挙
				switch (type2) {
				case 0:
				case 1:
				case 2: {
						// 全カードの日時を列挙
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
						// 全カードの日時をソート
						for (int i = 1; i < m_Document->m_Cards->Count; i++) {
							for (int i2 = i; i2 > 0; i2--) {
								if (dt[i2] > dt[i2 - 1]) {
									double t = dt[i2];
									dt[i2] = dt[i2 - 1];
									dt[i2 - 1] = t;
								}
								else {
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
								S = IntToStr(MonthOf(dt[i])) + "/" +
									IntToStr(YearOf(dt[i]));
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

						delete[]dt;
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
					Color->Add((void*)GetColor(i * 1.0f / Name->Count, 0, 255));
					Value->Add((void*)0);
					TSRectToCard *SRTC = new TSRectToCard();
					m_StatisticsRectToCard->Add(SRTC);
				}

				// 数を数える
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
								S = IntToStr(MonthOf(date)) + "/" +
									IntToStr(YearOf(date));
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

					int l1 = (int)Value->Items[index];
					float v1 = *(float*)&l1;
					v1 += 1.0f;
					Value->Items[index] = (void*)*(int*)&v1;

					TSRectToCard *SRTC =
						(TSRectToCard*)m_StatisticsRectToCard->Items[index];
					SRTC->m_CardIDList->Add((void*)Card->m_nID);
				}
			} break;
		}

		// ソート
		if (SB_StatisticsSort->Down) {
			// 挿入ソート
			for (int i = 1; i < Value->Count; i++) {
				for (int i2 = i; i2 > 0; i2--) {
					int l1 = (int)Value->Items[i2];
					float v1 = *(float*)&l1;
					int l2 = (int)Value->Items[i2 - 1];
					float v2 = *(float*)&l2;

					if (v1 > v2) {
						Value->Items[i2] = (void*)l2;
						Value->Items[i2 - 1] = (void*)l1;

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
					}
					else {
						break;
					}
				}
			}
		}

		// 棒グラフ描画

		// 最大値と文字のサイズ算出
		float max = 1.0f;
		int textwidth = 0;
		for (int i = 0; i < Name->Count; i++) {
			int tw = C->TextWidth(Name->Strings[i]);
			if (tw > textwidth) {
				textwidth = tw;
			}

			int l = (int)Value->Items[i];
			float v = *(float*)&l;
			if (v > max) {
				max = v;
			}
		}
		int lineheignt = m_nFontHeight * 2; // バーの間隔
		int hsize = Name->Count * lineheignt; // 全てのバーを表示した場合のグラフの高さ
		float hvisible = (BMP->Height - topmargin - leftmargin) / lineheignt;
		// 1画面にいくつバーが入るか
		if (hvisible * 2 >= Name->Count || Name->Count == 0) {
			Sc_StatisticsY->LargeChange = 10000;
		}
		else {
			Sc_StatisticsY->LargeChange =
				(10000 * hvisible) / (Name->Count - hvisible);
		}
		Sc_StatisticsY->SmallChange = Sc_StatisticsY->LargeChange / 10;
		float hinvisible = Name->Count - hvisible; // 画面に入りきらないバーの数
		int hoffset =
			hinvisible * Sc_StatisticsY->Position * 0.0001 * lineheignt;
		// 現在のスクロール位置
		if (hoffset < 0) {
			hoffset = 0;
		}
		int hend = hsize - hoffset; // 現在のグラフの縦サイズ
		if (hend > BMP->Height - topmargin - leftmargin) {
			hend = BMP->Height - topmargin - leftmargin;
		}
		if (hend < -leftmargin) {
			hend = -leftmargin;
		}
		int barwidth = BMP->Width - (textwidth + leftmargin * 5); // バーの長さ
		if (barwidth < 0) {
			barwidth = 0;
		}
		int shadowoffset = m_nFontHeight / 3; // 影のずれ量

		// ストライプ
		HRGN MyRgn, RgnBak;
		int rgnexist;
		MyRgn = ::CreateRectRgn(0, topmargin, BMP->Width,
			BMP->Height - leftmargin);
		RgnBak = ::CreateRectRgn(0, 0, 0, 0);
		rgnexist = ::GetClipRgn(C->Handle, RgnBak);
		if (rgnexist) {
			HRGN CmbRgn;
			CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
			::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
			::SelectClipRgn(C->Handle, CmbRgn);
			::DeleteObject(CmbRgn);
		}
		else {
			::SelectClipRgn(C->Handle, MyRgn);
		}
		B->Color = TColor(HalfColor(m_nFGColor, m_nBGColor, 0.95f));
		// 項目ループ
		for (int i = 1; i < Name->Count; i += 2) {
			int y = i * m_nFontHeight * 2 + topmargin - hoffset;
			B->Style = bsSolid;
			C->FillRect(Rect(leftmargin - 2, y - m_nFontHeight / 4,
				BMP->Width - leftmargin, y + m_nFontHeight * 1.5 +
				m_nFontHeight / 4));
		}
		if (rgnexist) {
			::SelectClipRgn(C->Handle, RgnBak);
		}
		else {
			::SelectClipRgn(C->Handle, NULL);
		}
		::DeleteObject(RgnBak);
		::DeleteObject(MyRgn);

		// 背景アニメーション
		if (SettingView.m_bBGAnimation) {
			BGAnimation(C);
		}

		// 線
		P->Color = TColor(m_nFGColor);
		P->Style = psSolid;
		P->Width = 3;
		C->MoveTo(BMP->Width - leftmargin, topmargin - leftmargin);
		C->LineTo(textwidth + leftmargin * 2, topmargin - leftmargin);
		C->LineTo(textwidth + leftmargin * 2, topmargin + hend);

		// 目盛り
		if (m_fStatisticsPos >= 1.0f) {
			P->Width = 1;
			P->Color = HalfColor(m_nFGColor, m_nBGColor, 0.5f);
			B->Style = bsClear;
			int order = 1;
			if (barwidth > 0) {
				while (barwidth / (max * 1.0 / order) < leftmargin * 8) {
					order *= 10;
				}
			}
			else {
				order = 100000;
			}
			F->Color = TColor(m_nFGColor);
			for (int i = 0; i <= max; i += order) {
				UnicodeString v = IntToStr(i);
				int x = leftmargin + textwidth + leftmargin * 2 +
					barwidth * i / max;
				C->TextOut(x - C->TextWidth(v) / 2, leftmargin, v);
				if (i > 0) {
					C->MoveTo(x, topmargin + 1);
					C->LineTo(x, topmargin + hend);
				}
			}
		}

		// 項目名と棒グラフ本体
		// HRGN MyRgn, RgnBak;
		// int rgnexist;
		MyRgn = ::CreateRectRgn(0, topmargin, BMP->Width,
			BMP->Height - leftmargin);
		RgnBak = ::CreateRectRgn(0, 0, 0, 0);
		rgnexist = ::GetClipRgn(C->Handle, RgnBak);
		if (rgnexist) {
			HRGN CmbRgn;
			CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
			::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
			::SelectClipRgn(C->Handle, CmbRgn);
			::DeleteObject(CmbRgn);
		}
		else {
			::SelectClipRgn(C->Handle, MyRgn);
		}
		// 項目ループ
		for (int i = 0; i < Name->Count; i++) {
			int y = i * m_nFontHeight * 2 + topmargin - hoffset;
			if (y < BMP->Height - leftmargin && y + m_nFontHeight * 2 >=
				topmargin) {
				// クリック可能な座標を設定
				TSRectToCard *SRTC =
					(TSRectToCard*)m_StatisticsRectToCard->Items[i];
				SRTC->m_Rect = Rect(0, y - m_nFontHeight / 4, BMP->Width,
					y + m_nFontHeight * 1.5 + m_nFontHeight / 4);
				SRTC->m_Name = Name->Strings[i];

				// 項目の名前表示
				B->Style = bsClear;
				F->Color = TColor(m_nFGColor);
				C->TextOut(leftmargin, y + m_nFontHeight / 4, Name->Strings[i]);

				int l = (int)Value->Items[i];
				float v = *(float*)&l;
				int x = leftmargin + textwidth + leftmargin * 2 +
					barwidth * v / max * m_fStatisticsPos;

				// 影
				B->Style = bsSolid;
				B->Color = HalfColor(HalfColor(m_nFGColor, m_nBGColor, 0.5f),
					0x0, 0.33f);
				C->FillRect(Rect(leftmargin + textwidth + leftmargin * 2 +
					shadowoffset, y + shadowoffset, x + shadowoffset,
					y + m_nFontHeight * 1.5 + shadowoffset));

				// バー本体
				B->Style = bsSolid;
				B->Color = TColor((int)Color->Items[i]);
				C->FillRect(Rect(leftmargin + textwidth + leftmargin * 2, y, x,
					y + m_nFontHeight * 1.5));

				if (m_fStatisticsPos >= 1.0f) {
					// 値
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
		}
		else {
			::SelectClipRgn(C->Handle, NULL);
		}
		::DeleteObject(RgnBak);
		::DeleteObject(MyRgn);

		delete Color;
		delete Value;
		delete Name;
	}

	// フォアグラウンドに描画
	PB_Statistics->Canvas->Draw(0, 0, BMP);

	// BMP破棄
	delete BMP;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sc_StatisticsYChange(TObject *Sender) {
	m_bRedrawRequested = true;
}

// ---------------------------------------------------------------------------
void TFo_Main::LinktoAllCardswithDesignatedLabel(TList *IDs) {
	// 現在カードから指定ラベルIDを持つカードすべてにラベルを貼る

	// カードループ
	for (int i = 0; i < m_Document->m_Cards->Count; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);

		if (Card->m_nID != m_nTargetCard) {
			// 自分自身ではない
			bool contain = false;
			for (int i2 = 0; i2 < Fo_Select->m_IDs->Count && !contain; i2++) {
				contain |= Card->m_Labels->Contain((int)IDs->Items[i2]);
			}
			if (contain) {
				// カードが該当ラベルを含んでいる

				// 既にリンクがはられているかどうかチェック
				bool linked = false;
				for (int i2 = 0;
				i2 < m_Document->m_Links->Count && !linked; i2++) {
					TLink *Link = m_Document->GetLinkByIndex(i2);
					linked |=
						Link->m_nFromID == m_nTargetCard && Link->m_nDestID ==
						Card->m_nID;
				}

				if (!linked) {
					// リンクが張られていない→新しいリンクを作成
					TLink *Link = m_Document->NewLink();
					Link->m_nFromID = m_nTargetCard;
					Link->m_nDestID = Card->m_nID;
				}
			}
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MI_LinktoAllCardswithDesignatedLabelClick
	(TObject *Sender) {
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

void __fastcall TFo_Main::MI_NewCardLinkstoAllCardswithDesignatedLabelClick
	(TObject *Sender) {
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
		// カードタイトルをラベル名に
		for (int i = 0; i < Fo_Select->m_IDs->Count; i++) {
			TCardLabel *Label = m_Document->GetLabelByIndex(0,
				(int)Fo_Select->m_IDs->Items[i] - 1);
			if (i > 0) {
				Card->m_Title += ", ";
			}
			Card->m_Title += Label->m_Name;
		}
		m_nTargetCard = Card->m_nID;
		m_nTargetLink = -1;

		// 座標を該当カードの中心に
		float x = 0.0f, y = 0.0f;
		int count = 0;
		// カードループ
		for (int i = 0; i < m_Document->m_Cards->Count; i++) {
			TCard *Card = m_Document->GetCardByIndex(i);

			if (Card->m_nID != m_nTargetCard) {
				// 自分自身ではない
				bool contain = false;
				for (int i2 = 0; i2 < Fo_Select->m_IDs->Count && !contain; i2++)
				{
					contain |=
						Card->m_Labels->Contain
						((int)Fo_Select->m_IDs->Items[i2]);
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

void __fastcall TFo_Main::MI_AddDesignatedLabeltoAllDestinationCardsClick
	(TObject *Sender) {
	Fo_Select = new TFo_Select(this);
	Fo_Select->m_Document = m_Document;
	Fo_Select->Caption = MLText.SelectLabel;
	Fo_Select->m_nType = 1;
	Fo_Select->m_bMultiSelect = true;
	Fo_Select->ShowModal();
	if (Fo_Select->ModalResult == mrOk) {
		BackupSub(MI_AddDesignatedLabeltoAllDestinationCards->Caption);

		// リンクループ
		for (int i = 0; i < m_Document->m_Links->Count; i++) {
			TLink *Link = m_Document->GetLinkByIndex(i);
			if (Link->m_nFromID == m_nTargetCard) {
				TCard *Card = m_Document->GetCard(Link->m_nDestID);
				for (int i2 = 0; i2 < Fo_Select->m_IDs->Count; i2++) {
					Card->m_Labels->AddLabel((int)Fo_Select->m_IDs->Items[i2]);
				}
			}
		}
	}
	Fo_Select->Release();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_StatisticsSortClick(TObject *Sender) {
	m_fStatisticsPos = 0.5f; // グラフの立ち上がりを0.5に
	PB_StatisticsPaint(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_StatisticsMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	// グラフ領域をクリックして関連カードを表示
	for (int i = 0; i < m_StatisticsRectToCard->Count; i++) {
		TSRectToCard *SRTC = (TSRectToCard*)m_StatisticsRectToCard->Items[i];
		if (SRTC->m_Rect.Left <= X && SRTC->m_Rect.Right >
			X && SRTC->m_Rect.Top <= Y && SRTC->m_Rect.Bottom > Y) {
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
				}
				else {
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
	P.x = ((TControl*)Sender)->Left;
	P.y = ((TControl*)Sender)->Top;
	P = Pa_BrowserTop->ClientToScreen(P);
	PM_StatisticsKey->Popup(P.x, P.y);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PM_StatisticKeyClick(TObject *Sender) {
	Bu_StatisticsKey->Tag = ((TMenuItem*)Sender)->Tag;

	m_fStatisticsPos = 0.5f; // グラフの立ち上がりを0.5に
	PB_StatisticsPaint(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_DrawingPaint(TObject *Sender) {
	// バックグラウンドバッファ作成
	Graphics::TBitmap *BMP = new Graphics::TBitmap();
	BMP->Width = PB_Drawing->Width;
	BMP->Height = PB_Drawing->Height;

	// 準備、初期化
	TCanvas *C = BMP->Canvas;
	TBrush *B = C->Brush;
	TPen *P = C->Pen;
	TFont *F = C->Font;
	F->Name = RE_Edit->Font->Name;
	F->Charset = RE_Edit->Font->Charset;
	F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);
	m_nFontHeight = C->TextHeight(" ");

	// 背景塗りつぶし
	B->Color = TColor(m_nBGColor);
	B->Style = bsSolid;
	C->FillRect(Rect(0, 0, BMP->Width, BMP->Height));

	// ガイド領域描画
	int sqrsize = BMP->Width; // 幅、高さのうち小さいほう
	int leftspace = 0;
	int topspace = 0;
	if (sqrsize > BMP->Height) {
		sqrsize = BMP->Height;
		leftspace = (BMP->Width - sqrsize) / 2;
	}
	else {
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

	// 図形描画
	P->Color = TColor(m_nFGColor);
	B->Color = TColor(m_nFGColor);
	P->Width = 3;
	m_Drawing->Draw(C, m_DrawingRect);

	// カードプレビュー表示
	/*
	 現在DrawCardがブラウザへの描画しか考慮していないためシンプルに実現できない
	 具体的には、m_CardXなどの情報が必要（PaintSub内で更新している）
	 これをプレビュー毎に作りなおさなければならない

	 int idx = m_Document->SearchCardIndex(m_nTargetCard);
	 if (idx >= 0) {
	 TCard *Card = m_Document->GetCardByIndex(idx);
	 if (Card) {
	 TColor HMColor = TColor(m_nBGColor);
	 float SizeX = Card->m_nSize;
	 if (m_Document->CountEnableLabel(Card)){
	 //ラベルあり（ラベルの色にする）
	 TColor c = GetCardColor(Card, SizeX);
	 P->Color = HalfColor(c, m_nBGColor, 0.33f);
	 B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
	 }else{
	 //ラベルなし
	 P->Color = HalfColor(m_nFGColor, m_nBGColor, 0.5f);
	 //B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor, 0.75f);
	 B->Color = HalfColor(P->Color, m_nBGColor, 0.875f);
	 }

	 Card->m_Color = P->Color;
	 DrawCard(C, Card, (int)SizeX, idx, HMColor, 0);
	 }
	 }
	 */

	// フォアグラウンドに描画
	PB_Drawing->Canvas->Draw(0, 0, BMP);

	// BMP破棄
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
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	TB_DZoom->SetFocus();
	if (Button == mbLeft) {
		m_Drawing->m_nPenColor = TColor(0x7fffffff);
		if (Sh_DColor->Pen->Width > 1) {
			m_Drawing->m_nPenColor = Sh_DColor->Brush->Color;
		}
		m_Drawing->SetTool(m_DrawingTool);
		m_Drawing->MouseDown((X - m_DrawingRect.left) * 1.0 /
			(m_DrawingRect.right - m_DrawingRect.left),
			(Y - m_DrawingRect.top) * 1.0 / (m_DrawingRect.bottom -
			m_DrawingRect.top), m_DrawingRect.right - m_DrawingRect.left,
			Shift);
		PB_DrawingPaint(Sender);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_DrawingMouseMove(TObject *Sender,
	TShiftState Shift, int X, int Y) {
	m_Drawing->MouseMove((X - m_DrawingRect.left) * 1.0 / (m_DrawingRect.right -
		m_DrawingRect.left), (Y - m_DrawingRect.top) * 1.0 /
		(m_DrawingRect.bottom - m_DrawingRect.top));
	PB_DrawingPaint(Sender);
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_DrawingMouseUp(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	if (Button == mbLeft) {
		m_Drawing->MouseUp((X - m_DrawingRect.left) * 1.0 /
			(m_DrawingRect.right - m_DrawingRect.left),
			(Y - m_DrawingRect.top) * 1.0 / (m_DrawingRect.bottom -
			m_DrawingRect.top));
		PB_DrawingPaint(Sender);
		ApplyDrawing();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::SB_DrawingToolClick(TObject *Sender) {
	m_DrawingTool = ((TSpeedButton*)Sender)->Tag;
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
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	Sh_DColorDefault->Pen->Width = 3;
	Sh_DColor->Pen->Width = 1;
	m_Drawing->ChangePenColor(TColor(0x7fffffff));
	PB_DrawingPaint(Sender);
	ApplyDrawing();
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::Sh_DColorMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
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
		// デフォルトのカード名を現在のカードのタイトルにする
		SD_Txt->FileName = Card->m_Title;
		if (SD_Txt->Execute()) {
			TStringList *SL = new TStringList();
			ExportHierarchicalText(SL, 1, ".", Card);
			try {
				SL->SaveToFile(SD_Txt->FileName);
			}
			catch (...) {
				ShowMessage(MLText.FailedToSave + SD_Txt->FileName +
					UnicodeString("."));
			}
			delete SL;
		}
	}
}

// ---------------------------------------------------------------------------
void TFo_Main::ExportHierarchicalText(TStringList *SL, int CurrentLevel,
	UnicodeString HChar, TCard *CurrentParent) {
	// 階層テキスト出力のためのサブルーチン。CurrentParentにぶら下がっているすべてのカードについて、その下の階層を出力させる。このルーチンは再帰的に呼ばれる
	for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
		TCard *Card = m_Document->GetCardByIndex(ic);
		if (Card) {
			if (Card->m_nParentID == CurrentParent->m_nID) {
				// 現在の親にぶら下がっているカード

				// カードタイトル出力
				UnicodeString S;
				for (int ih = 0; ih < CurrentLevel; ih++) {
					S += HChar;
				}
				SL->Add(S + Card->m_Title);

				// カードの内容出力
				for (int il = 0; il < Card->m_Lines->Count; il++) {
					SL->Add(Card->m_Lines->Strings[il]);
				}

				// 下位カードの内容出力
				ExportHierarchicalText(SL, CurrentLevel + 1, HChar, Card);
			}
		}
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
	Fo_Search->Ch_Backward->Checked = Sender == (TObject*)ME_FindPrevious;
	Fo_Search->Ch_GlobalSearch->Checked =
		Sender != (TObject*)ME_FindNext && Sender != (TObject*)ME_FindPrevious;
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::MS_ResetAllDatesClick(TObject *Sender) {
	// ヘルプ作成用。全日付を0に
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

	// 全文章を文字N-gramに分解
	TWideStringList *WS = new TWideStringList();
	for (int i = 0; i < doccount; i++) {
		TCard *Card = m_Document->GetCardByIndex(i);
		WS->Add(Card->m_Lines->Text);
	}

	float fdummy;
	bool bdummy = false;
	const int n = 16; // up to 16-gram
	TTextDecomposer *TD = new TTextDecomposer(WS, n, 0, 0.0f, 0.0f, fdummy,
		bdummy, 0x1);

	// 文字N-gramをファイルに出力
	FILE *F = fopen(AnsiString(SD_CSV->FileName).c_str(), "wt");
	fprintf(F, "String,Freq\n");

	int currentindex[n];
	for (int i = 0; i < TD->m_nMaxCombi; i++) {
		TD->m_Gram[i]->SortValue();
		currentindex[i] = TD->m_Gram[i]->Count - 1;
	}

	while (true) {
		// 一番頻度の多い文字列を探す
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
			// もう文字列がないので終了
			break;
		}

		// 一番頻度の多かった文字列から順に出力
		while (currentindex[maxindex] >= 0) {
			if (TD->m_Gram[maxindex]->Enabled(currentindex[maxindex])) {
				int v = TD->m_Gram[maxindex]->Values(currentindex[maxindex]);
				if (v == max) {
					AnsiString S =
						TD->m_Gram[maxindex]->Strings(currentindex[maxindex]);
					if (!(S.Pos("\"") || S.Pos("\n") || S.Pos("\r") ||
						S.Pos("\t"))) {
						// 書き出せない文字は入っていない
						fprintf(F, "%s,%d\n", S.c_str(), v);
					}
					currentindex[maxindex]--;
				}
				else {
					// 頻度が減ったら終わり
					break;
				}
			}
			else {
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
				}
				else {
					break;
				}
			}
			S2 += SplitStrBy(S, "\",");
		}
		else {
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

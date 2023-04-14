// ---------------------------------------------------------------------------

#include "setting.h"

// ---------------------------------------------------------------------------
TSettingView SettingView;
TSettingFile SettingFile;
TSetting2Function Setting2Function;

TMLText MLText;

// ---------------------------------------------------------------------------
TMLText::TMLText() {
	Cards = "Cards"; // カード
	Links = "Links"; // リンク
	Labels = "Labels"; // ラベル
	Line = "Line"; // 行
	Len = "Len"; // 文字
	Size = "Size"; // サイズ
	Title = "Title"; // 名前
	Created = "Created"; // 作成日時
	Edited = "Edited"; // 編集日時
	Viewed = "Viewed"; // 閲覧日時
	Hits = "Hits"; // 件

	EditCardTitle = "Edit Card Title";
	EditCardText = "Edit Card Text";
	EditLinkTitle = "Edit Link Title";
	EditDrawing = "Edit Drawing";

	ChangeSize = "Change Size"; // サイズの変更
	FixCardPosition = "Fix Card Position"; // カード位置の固定
	SetAsRootCard = "Set as Root Card"; // ルートカードに指定

	EditLink = "Edit Link"; // リンクの編集

	AddLabel = "Add Label"; // ラベルの追加
	DeleteLabel = "Delete Label"; // ラベルの削除
	ChangeLabel = "Change Label"; // ラベルの変更
	LabelName = "Label Name"; // ラベル名
	SelectLinkDestination = "Select Destination";
	SelectLabelOfDestination = "Select Label of Destination";
	SelectLabel = "Select Label";

	Color = "Color";

	ArrangeCards = "Arrange Cards";

	ProcessingTextAnalysis = "Processing text analysis...";

	Enable = "Enable"; // 有効
	Fold = "Fold"; // 折畳
	Show = "Show"; // 表示
	Hide = "Hide"; // 隠蔽
	NoAssign = "No Assign"; // 未指定
	NoLabel = "No Label"; // ラベルなし
	SelectTargetCard = "Select Target Card"; // リンク先カードの選択
	Old = "Old"; // 古い順
	New = "New"; // 新しい順
	Near = "Near"; // 近い順
	Older = "Older"; // より古い
	Newer = "Newer"; // より新しい
	SaveCheck = "This document has changed. Save?";
	FailedToSave = "Failed to save ";
	NotFound = " is not found.";
	AANotEnoughMemory = "Anti-Aliasing : Not enough memory";
}

// ---------------------------------------------------------------------------
UnicodeString LanguageFileName() {
	return ExtractFilePath(ParamStr(0)) + SettingView.m_Language + ".lng";
}

// ---------------------------------------------------------------------------
TSettingView::TSettingView() {
	m_Font = new TFont();

	m_bLinkShadow = false;
	m_bLinkHemming = false;
	m_bLinkVisible = true;
	m_bLinkDirectionVisible = true;
	m_bLinkNameVisible = true;

	m_bLabelCircleVisible = false;
	m_bLabelRectangleVisible = true;
	m_bLavelCRFocusedOnTop = false; // 普通はfalse
	m_bLabelFill = false;
	m_bLabelNameVisible = true;

	m_bFocusCursor = true;
	m_bFocusCursorAnimation = true;
	m_nCardDefaultShape = 2;
	m_nCardDefaultSize = 100;
	m_bCardDefaultFixed = false;
	m_bCardShadow = true;
	m_bCardGradation = true;
	m_bCardHemming = false;
	m_bImageVisible = true;
	m_bVideoVisible = true;
	m_bDrawingVisible = true;
	m_nImageLimitation = 80;
	m_bMagnifyFocused = false;

	m_bTextVisible = true;
	m_bTextCentering = false;
	m_bTextWordWrap = true;
	m_bEditInBrowser = true; // Browser上で本文編集
	m_bEditInBrowserAlways = false; // Browser上で本文編集（常に）
	m_nEditInBrowserPos = 0; // 編集位置。0=カードの下、1=ブラウザ右、2=ブラウザ下
	m_nEditorWidthInBrowser = 0; // Browser上で編集する際のWidth
	m_nEditorHeightInBrowser = 0; // Browser上で編集する際のHeight

	m_nSizeLimitation = 100;
	m_bSizeLimitation = false;

	m_bLinkLimitation = false;
	m_nLinkLimitation = 3;
	m_bLinkDirection = false;
	m_bLinkBackward = false;
	m_nLinkTarget = -1;

	m_bDateLimitation = false; // 日付による表示制限ON/OFF
	m_nDateLimitation = 0; // 日付による表示制限の大きさ（意味はDateLimitationTypeによる）
	m_DateLimitationType = 0; // 日付による制限の種類（Old, New, Near, Older, Newer）
	m_DateLimitationDateType = 0; // 日付の種類（Created, Edited, Viewed）

	m_bNoLabelVisible = true;
	m_bNoLinkLabelVisible = true;

	m_nAnimationRCCards = 10;
	m_nAnimationRCSpeed = 30;

	m_bAnimationPaused = false;

	m_bBGAnimation = false;
	m_nBGAnimationType = 0;
	m_bNoScrollLag = true;

	m_bScore = false;
	m_ScoreType = 0;

	m_bRead = false;
	m_ReadAgentFile = "";

	m_nFourgroundColor = 0x000000;
	m_nBackgroundColor = 0xffffff;

	m_bAntiAlias = false;
	m_nAntiAliasX = 2;

	m_WallPaper = "";
	m_bFixWallPaper = true;
	m_bTileWallPaper = false;

	m_bCardList = true; // カードリスト
	m_bFileList = true; // ファイルリスト

	m_bOverview = true; // 全体図

	m_bTickerVisible = false;
	m_nTickerLines = 1;

	m_Language = "";

	m_nFileListWidth = 145;
	m_nCardListWidth = 185;
	m_nLinkListHeight = 105;
	m_bEditorRelatedVisible = true;
	m_nEditorRelatedWidth = 321;
	m_nGlobalSearchWidth = 256;

	m_bStatusBar = true;

	m_bLoadImageatOnce = false;
	m_bRedrawInBackground = false;

	m_nSpecialPaint = 0;
	m_SpecialCaption = "";
}

// ---------------------------------------------------------------------------
TSettingView::~TSettingView() {
	delete m_Font;
}

// ---------------------------------------------------------------------------
void TSettingView::WriteToIni(TIniFile *Ini, UnicodeString Section) {
	Ini->WriteString(Section, "FontName", m_Font->Name);
	Ini->WriteInteger(Section, "FontCharSet", m_Font->Charset);
	Ini->WriteInteger(Section, "FontSize", m_Font->Size);

	Ini->WriteInteger(Section, "LinkDefaultShape", m_nLinkDefaultShape);
	Ini->WriteBool(Section, "LinkHemming", m_bLinkHemming);
	Ini->WriteBool(Section, "LinkShadow", m_bLinkShadow);
	Ini->WriteBool(Section, "LinkVisible", m_bLinkVisible);
	Ini->WriteBool(Section, "LinkDirectionVisible", m_bLinkDirectionVisible);
	Ini->WriteBool(Section, "LinkNameVisible", m_bLinkNameVisible);

	Ini->WriteBool(Section, "LabelCircleVisible", m_bLabelCircleVisible);
	Ini->WriteBool(Section, "LabelRectangleVisible", m_bLabelRectangleVisible);
	Ini->WriteBool(Section, "LabelNameVisible", m_bLabelNameVisible);
	Ini->WriteBool(Section, "LabelFill", m_bLabelFill);

	Ini->WriteBool(Section, "FocusCursor", m_bFocusCursor);
	Ini->WriteBool(Section, "FocusCursorAnimation", m_bFocusCursorAnimation);
	Ini->WriteInteger(Section, "CardDefaultShape", m_nCardDefaultShape);
	Ini->WriteInteger(Section, "CardDefaultSize", m_nCardDefaultSize);
	Ini->WriteBool(Section, "CardDefaultFixed", m_bCardDefaultFixed);
	Ini->WriteBool(Section, "CardHemming", m_bCardHemming);
	Ini->WriteBool(Section, "CardShadow", m_bCardShadow);
	Ini->WriteBool(Section, "CardGradation", m_bCardGradation);
	Ini->WriteBool(Section, "ImageVisible", m_bImageVisible);
	Ini->WriteBool(Section, "VideoVisible", m_bVideoVisible);
	Ini->WriteBool(Section, "DrawingVisible", m_bDrawingVisible);
	Ini->WriteInteger(Section, "ImageLimitation", m_nImageLimitation);
	Ini->WriteBool(Section, "MagnifyFocused", m_bMagnifyFocused);

	Ini->WriteBool(Section, "TextVisible", m_bTextVisible);
	Ini->WriteBool(Section, "TextCentering", m_bTextCentering);
	Ini->WriteBool(Section, "TextWordWrap", m_bTextWordWrap);
	Ini->WriteBool(Section, "EditInBrowser", m_bEditInBrowser);
	Ini->WriteBool(Section, "EditInBrowserAlways", m_bEditInBrowserAlways);
	Ini->WriteInteger(Section, "EditInBrowserPos", m_nEditInBrowserPos);
	Ini->WriteInteger(Section, "EditorWidthInBrowser", m_nEditorWidthInBrowser);
	Ini->WriteInteger(Section, "EditorHeightInBrowser",
		m_nEditorHeightInBrowser);

	Ini->WriteBool(Section, "Score", m_bScore);
	Ini->WriteInteger(Section, "ScoreType", m_ScoreType);

	Ini->WriteBool(Section, "Read", m_bRead);
	Ini->WriteString(Section, "ReadAgentFile", m_ReadAgentFile);

	Ini->WriteBool(Section, "CardList", m_bCardList);
	Ini->WriteBool(Section, "FileList", m_bFileList);

	Ini->WriteInteger(Section, "FourgroundColor", m_nFourgroundColor);
	Ini->WriteInteger(Section, "BackgroundColor", m_nBackgroundColor);

	Ini->WriteBool(Section, "AntiAlias", m_bAntiAlias);
	Ini->WriteInteger(Section, "AntiAliasX", m_nAntiAliasX);

	Ini->WriteString(Section, "WallPaper", m_WallPaper);
	Ini->WriteBool(Section, "FixWallPaper", m_bFixWallPaper);
	Ini->WriteBool(Section, "TileWallPaper", m_bTileWallPaper);

	Ini->WriteBool(Section, "BGAnimation", m_bBGAnimation);
	Ini->WriteInteger(Section, "BGAnimationType", m_nBGAnimationType);
	Ini->WriteBool(Section, "NoScrollLag", m_bNoScrollLag);

	Ini->WriteBool(Section, "Overview", m_bOverview);
	Ini->WriteBool(Section, "TickerVisible", m_bTickerVisible);
	Ini->WriteInteger(Section, "TickerLines", m_nTickerLines);

	Ini->WriteString(Section, "Language", m_Language);

	Ini->WriteInteger(Section, "FileListWidth", m_nFileListWidth);
	Ini->WriteInteger(Section, "CardListWidth", m_nCardListWidth);
	Ini->WriteInteger(Section, "LinkListHeight", m_nLinkListHeight);
	Ini->WriteBool(Section, "EditorRelatedVisible", m_bEditorRelatedVisible);
	Ini->WriteInteger(Section, "EditorRelatedWidth", m_nEditorRelatedWidth);
	Ini->WriteInteger(Section, "GlobalSearchWidth", m_nGlobalSearchWidth);

	Ini->WriteBool(Section, "StatusBar", m_bStatusBar);

	Ini->WriteBool(Section, "LoadImageatOnce", m_bLoadImageatOnce);
	Ini->WriteBool(Section, "RedrawInBackground", m_bRedrawInBackground);
}

// ---------------------------------------------------------------------------
void TSettingView::ReadFromIni(TIniFile *Ini, UnicodeString Section) {
	m_Font->Name = Ini->ReadString(Section, "FontName", m_Font->Name);
	m_Font->Charset = Ini->ReadInteger(Section, "FontCharSet", m_Font->Charset);
	m_Font->Size = Ini->ReadInteger(Section, "FontSize", m_Font->Size);

	m_nLinkDefaultShape = Ini->ReadInteger(Section, "LinkDefaultShape",
		m_nLinkDefaultShape);
	m_bLinkShadow = Ini->ReadBool(Section, "LinkShadow", m_bLinkShadow);
	m_bLinkHemming = Ini->ReadBool(Section, "LinkHemming", m_bLinkHemming);
	m_bLinkVisible = Ini->ReadBool(Section, "LinkVisible", m_bLinkVisible);
	m_bLinkDirectionVisible = Ini->ReadBool(Section, "LinkDirectionVisible",
		m_bLinkDirectionVisible);
	m_bLinkNameVisible = Ini->ReadBool(Section, "LinkNameVisible",
		m_bLinkNameVisible);

	m_bLabelCircleVisible = Ini->ReadBool(Section, "LabelCircleVisible",
		m_bLabelCircleVisible);
	m_bLabelRectangleVisible = Ini->ReadBool(Section, "LabelRectangleVisible",
		m_bLabelRectangleVisible);
	m_bLavelCRFocusedOnTop = Ini->ReadBool(Section, "LavelCRFocusedOnTop",
		m_bLavelCRFocusedOnTop);
	m_bLabelNameVisible = Ini->ReadBool(Section, "LabelNameVisible",
		m_bLabelNameVisible);
	m_bLabelFill = Ini->ReadBool(Section, "LabelFill", m_bLabelFill);

	m_bFocusCursor = Ini->ReadBool(Section, "FocusCursor", m_bFocusCursor);
	m_bFocusCursorAnimation = Ini->ReadBool(Section, "FocusCursorAnimation",
		m_bFocusCursorAnimation);
	m_nCardDefaultShape = Ini->ReadInteger(Section, "CardDefaultShape",
		m_nCardDefaultShape);
	m_nCardDefaultSize = Ini->ReadInteger(Section, "CardDefaultSize",
		m_nCardDefaultSize);
	m_bCardDefaultFixed = Ini->ReadBool(Section, "CardDefaultFixed",
		m_bCardDefaultFixed);
	m_bCardShadow = Ini->ReadBool(Section, "CardShadow", m_bCardShadow);
	m_bCardHemming = Ini->ReadBool(Section, "CardHemming", m_bCardHemming);
	m_bCardGradation = Ini->ReadBool(Section, "CardGradation",
		m_bCardGradation);
	m_bImageVisible = Ini->ReadBool(Section, "ImageVisible", m_bImageVisible);
	m_bVideoVisible = Ini->ReadBool(Section, "VideoVisible", m_bVideoVisible);
	m_bDrawingVisible = Ini->ReadBool(Section, "DrawingVisible",
		m_bDrawingVisible);
	m_nImageLimitation = Ini->ReadInteger(Section, "ImageLimitation",
		m_nImageLimitation);
	m_bMagnifyFocused = Ini->ReadBool(Section, "MagnifyFocused",
		m_bMagnifyFocused);

	m_bTextVisible = Ini->ReadBool(Section, "TextVisible", m_bTextVisible);
	m_bTextCentering = Ini->ReadBool(Section, "TextCentering",
		m_bTextCentering);
	m_bTextWordWrap = Ini->ReadBool(Section, "TextWordWrap", m_bTextWordWrap);
	m_bEditInBrowser = Ini->ReadBool(Section, "EditInBrowser",
		m_bEditInBrowser);
	m_bEditInBrowserAlways = Ini->ReadBool(Section, "EditInBrowserAlways",
		m_bEditInBrowserAlways);
	m_nEditInBrowserPos = Ini->ReadInteger(Section, "EditInBrowserPos",
		m_nEditInBrowserPos);
	m_nEditorWidthInBrowser = Ini->ReadInteger(Section, "EditorWidthInBrowser",
		m_nEditorWidthInBrowser);
	m_nEditorHeightInBrowser =
		Ini->ReadInteger(Section, "EditorHeightInBrowser",
		m_nEditorHeightInBrowser);

	m_bScore = Ini->ReadBool(Section, "Score", m_bScore);
	m_ScoreType = Ini->ReadInteger(Section, "ScoreType", m_ScoreType);

	m_bRead = Ini->ReadBool(Section, "Read", m_bRead);
	m_ReadAgentFile = Ini->ReadString(Section, "ReadAgentFile",
		m_ReadAgentFile);

	m_bCardList = Ini->ReadBool(Section, "CardList", m_bCardList);
	m_bFileList = Ini->ReadBool(Section, "FileList", m_bFileList);

	m_nFourgroundColor = Ini->ReadInteger(Section, "FourgroundColor",
		m_nFourgroundColor);
	m_nBackgroundColor = Ini->ReadInteger(Section, "BackgroundColor",
		m_nBackgroundColor);

	m_bAntiAlias = Ini->ReadBool(Section, "AntiAlias", m_bAntiAlias);
	m_nAntiAliasX = Ini->ReadInteger(Section, "AntiAliasX", m_nAntiAliasX);

	m_WallPaper = Ini->ReadString(Section, "WallPaper", m_WallPaper);
	m_bFixWallPaper = Ini->ReadBool(Section, "FixWallPaper", m_bFixWallPaper);
	m_bTileWallPaper = Ini->ReadBool(Section, "TileWallPaper",
		m_bTileWallPaper);

	m_bBGAnimation = Ini->ReadBool(Section, "BGAnimation", m_bBGAnimation);
	m_nBGAnimationType = Ini->ReadInteger(Section, "BGAnimationType",
		m_nBGAnimationType);
	m_bNoScrollLag = Ini->ReadBool(Section, "NoScrollLag", m_bNoScrollLag);

	m_bOverview = Ini->ReadBool(Section, "Overview", m_bOverview);
	m_bTickerVisible = Ini->ReadBool(Section, "TickerVisible",
		m_bTickerVisible);
	m_nTickerLines = Ini->ReadInteger(Section, "TickerLines", m_nTickerLines);

	m_Language = Ini->ReadString(Section, "Language", m_Language);
	if (m_Language == "Japanese" && m_Font->Charset != SHIFTJIS_CHARSET) {
		// 文字化け対策。日本語を選択している場合は自動で文字セットをSHIFTJISにする
		m_Font->Charset = SHIFTJIS_CHARSET;
	}

	m_nFileListWidth = Ini->ReadInteger(Section, "FileListWidth",
		m_nFileListWidth);
	m_nCardListWidth = Ini->ReadInteger(Section, "CardListWidth",
		m_nCardListWidth);
	m_nLinkListHeight = Ini->ReadInteger(Section, "LinkListHeight",
		m_nLinkListHeight);
	m_bEditorRelatedVisible = Ini->ReadBool(Section, "EditorRelatedVisible",
		m_bEditorRelatedVisible);
	m_nEditorRelatedWidth = Ini->ReadInteger(Section, "EditorRelatedWidth",
		m_nEditorRelatedWidth);
	m_nGlobalSearchWidth = Ini->ReadInteger(Section, "GlobalSearchWidth",
		m_nGlobalSearchWidth);

	m_bStatusBar = Ini->ReadBool(Section, "StatusBar", m_bStatusBar);

	m_bLoadImageatOnce = Ini->ReadBool(Section, "LoadImageatOnce",
		m_bLoadImageatOnce);
	m_bRedrawInBackground = Ini->ReadBool(Section, "RedrawInBackground",
		m_bRedrawInBackground);

	m_nSpecialPaint = Ini->ReadInteger(Section, "SpecialPaint",
		m_nSpecialPaint);
	m_SpecialCaption = Ini->ReadString(Section, "SpecialCaption",
		m_SpecialCaption);
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
TSettingFile::TSettingFile() {
	m_bCheckNew = true;

	m_nUndoTimes = 10;

	m_PlugIn = "";
	m_hPlugIn = NULL;
	fepLoadPlugIn = NULL;
	fepTimeOut = NULL;
	fepUnloadPlugIn = NULL;

	m_WebSearch = "";

	m_GPTAPIKey = "";
}

// ---------------------------------------------------------------------------
TSettingFile::~TSettingFile() {
	if (m_hPlugIn) {
		if (fepUnloadPlugIn) {
			fepUnloadPlugIn();
		}
		fepLoadPlugIn = NULL;
		fepTimeOut = NULL;
		fepOperation = NULL;
		fepUnloadPlugIn = NULL;
		FreeLibrary(m_hPlugIn);
	}
}

// ---------------------------------------------------------------------------
void TSettingFile::WriteToIni(TIniFile *Ini, UnicodeString Section) {
	Ini->WriteBool(Section, "CheckNew", m_bCheckNew);

	for (int i = 0; i < 10; i++) {
		Ini->WriteString(Section, UnicodeString("History") + i,
			m_RecentFiles[i]);
	}
	for (int i = 0; i < 10; i++) {
		Ini->WriteString(Section, UnicodeString("HistoryF") + i,
			m_RecentFolders[i]);
	}

	Ini->WriteInteger(Section, "UndoTimes", m_nUndoTimes);

	Ini->WriteString(Section, "WebSearch", m_WebSearch);

	Ini->WriteString(Section, "GPTAPIKey", m_GPTAPIKey);
}

// ---------------------------------------------------------------------------
void TSettingFile::ReadFromIni(TIniFile *Ini, UnicodeString Section) {
	m_bCheckNew = Ini->ReadBool(Section, "CheckNew", m_bCheckNew);

	for (int i = 0; i < 10; i++) {
		m_RecentFiles[i] =
			Ini->ReadString(Section, UnicodeString("History") + i, "");
	}
	for (int i = 0; i < 10; i++) {
		m_RecentFolders[i] =
			Ini->ReadString(Section, UnicodeString("HistoryF") + i, "");
	}

	m_nUndoTimes = Ini->ReadInteger(Section, "UndoTimes", m_nUndoTimes);

	m_PlugIn = Ini->ReadString(Section, "PlugIn", m_PlugIn);
	if (m_PlugIn != "") {
		if (FileExists(m_PlugIn)) {
			m_hPlugIn = LoadLibrary(m_PlugIn.c_str());
			if (!m_hPlugIn) {
				ShowMessage(GetLastError());
			}

			fepLoadPlugIn = (void __stdcall(*)(void))GetProcAddress(m_hPlugIn,
				"fepLoadPlugIn");
			fepTimeOut = (void __stdcall(*)(IDocCtrl * Doc))GetProcAddress
				(m_hPlugIn, "fepTimeOut");
			fepOperation = (bool __stdcall(*)(IDocCtrl*, wchar_t *))
				GetProcAddress(m_hPlugIn, "fepOperation");
			fepUnloadPlugIn = (void __stdcall(*)(void))GetProcAddress(m_hPlugIn,
				"fepUnloadPlugIn");

			if (fepLoadPlugIn) {
				fepLoadPlugIn();
			}
		}
	}

	m_WebSearch = Ini->ReadString(Section, "WebSearch", m_WebSearch);

	m_GPTAPIKey = Ini->ReadString(Section, "GPTAPIKey", m_GPTAPIKey);
}

// ---------------------------------------------------------------------------
TSetting2Function::TSetting2Function() {
	m_WebSearch = new TStringList();
	m_GPT = new TStringList();
}

// ---------------------------------------------------------------------------
TSetting2Function::~TSetting2Function() {
	delete m_WebSearch;
    delete m_GPT;
}

// ---------------------------------------------------------------------------
void TSetting2Function::WriteToIni(TIniFile *Ini) {
}

// ---------------------------------------------------------------------------
void TSetting2Function::ReadFromIni(TIniFile *Ini) {
	Ini->ReadSectionValues("WebSearch", m_WebSearch);
	m_GPTSystemPrompt = Ini->ReadString("GPTSystem", "SystemPrompt", "");
	Ini->ReadSectionValues("GPT", m_GPT);
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)

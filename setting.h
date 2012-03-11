//---------------------------------------------------------------------------

#ifndef settingH
#define settingH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <IniFiles.hpp>
#include "document.h"
#include "utils.h"
//---------------------------------------------------------------------------

class TSettingView
{
public:
    //生成、破棄
    TSettingView();
    virtual ~TSettingView();
public:
    //読み書き
    void WriteToIni(TIniFile *Ini, AnsiString Section);
    void ReadFromIni(TFastIni *Ini, AnsiString Section);
public:
    //設定

    //Font
    TFont *m_Font;

    //Link
    int m_nLinkDefaultShape;
    bool m_bLinkShadow;
    bool m_bLinkHemming;
    bool m_bLinkVisible;
    bool m_bLinkDirectionVisible;
    bool m_bLinkNameVisible;

    //Label
    bool m_bLabelCircleVisible;
    bool m_bLabelRectangleVisible;
    bool m_bLavelCRFocusedOnTop;//フォーカスされた。普通は非使用。読み込み専用。
    bool m_bLabelFill;
    bool m_bLabelNameVisible;

    //Card
    bool m_bFocusCursor;
    bool m_bFocusCursorAnimation;
    int m_nCardDefaultShape;
    int m_nCardDefaultSize;
    bool m_bCardDefaultFixed;
    bool m_bTickerVisible;//カードの一番下にTicker Marquee表示
    int m_nTickerLines;
    bool m_bCardShadow;
    bool m_bCardHemming;
    bool m_bCardGradation;
    bool m_bImageVisible;
    bool m_bVideoVisible;
    bool m_bDrawingVisible;
	int m_nImageLimitation;
    bool m_bMagnifyFocused;//フォーカスされたカードを拡大

    //Text
    bool m_bTextVisible;
    bool m_bTextCentering;
    bool m_bTextWordWrap;

    bool m_bEditInBrowser;//Browser上で本文編集
    bool m_bEditInBrowserAlways;//Browser上で本文編集（常に）
    int m_nEditInBrowserPos;//編集位置。0=カードの下、1=ブラウザ右、2=ブラウザ下
    int m_nEditorWidthInBrowser;//Browser上で編集する際のWidth
    int m_nEditorHeightInBrowser;//Browser上で編集する際のHeight

    //Size Limitation
    int m_nSizeLimitation;//サイズいくつ以上のカードを表示するか。デフォルトは100（標準サイズ以上で表示）
    bool m_bSizeLimitation;//サイズ制限の有無。デフォルトはなし

    //Link Limitation
    int m_bLinkLimitation;//リンク制限ON･OFF
    int m_nLinkLimitation;//何リンク辿って表示するか。デフォルトは0（全表示）
    bool m_bLinkDirection;//リンク方向の制限。デフォルトはなし
    bool m_bLinkBackward;//逆方向リンク制限
    int m_nLinkTarget;//リンクターゲットカードのID

    //Date Limitation
    bool m_bDateLimitation;//日付による表示制限ON/OFF
    int m_nDateLimitation;//日付による表示制限の大きさ（意味はDateLimitationTypeによる）
    int m_DateLimitationType;//日付による制限の種類（Old, New, Near, Older, Newer）
    int m_DateLimitationDateType;//日付の種類（Created, Edited, Viewed）

    //CardVisible
    bool m_bNoLabelVisible;
    bool m_bNoLinkLabelVisible;

    //アニメーション
    bool m_bAnimationPaused;//ポーズ

    int m_nAnimationRCCards;//RandomCardで表示するカード数
    int m_nAnimationRCSpeed;//RandomCardのアニメーション速度

    //背景アニメーション
    bool m_bBGAnimation;
    int m_nBGAnimationType;
    bool m_bNoScrollLag;

    //Score
    bool m_bScore;//カードのスコア
    int m_ScoreType;//0=Authenticity, 1=Starting Point, 2=Destination

    //Read
    bool m_bRead;//読み上げ
    AnsiString m_ReadAgentFile;//エージェント

    //Others
    bool m_bCardList;
    bool m_bFileList;

    int m_nFourgroundColor;
    int m_nBackgroundColor;

    bool m_bAntiAlias;
    int m_nAntiAliasX;

    AnsiString m_WallPaper;
    bool m_bFixWallPaper;
    bool m_bTileWallPaper;

    bool m_bOverview;//全体図

    //言語
    AnsiString m_Language;

    //画面配置
    int m_nFileListWidth;
    int m_nCardListWidth;
    int m_nLinkListHeight;
    bool m_bEditorRelatedVisible;
    int m_nEditorRelatedWidth;
    int m_nGlobalSearchWidth;

    //Statusバー
    bool m_bStatusBar;

    //メニューにないもの
    bool m_bLoadImageatOnce;//サムネイルなどの画像を一度に全てロード
    bool m_bRedrawInBackground;//バックグラウンド状態でも画面を更新

    //一時
    int m_nSpecialPaint;
    AnsiString m_SpecialCaption;
};
//---------------------------------------------------------------------------
class TSettingFile
{
public:
    //生成、破棄
    TSettingFile();
    virtual ~TSettingFile();
public:
    //読み書き
    void WriteToIni(TIniFile *Ini, AnsiString Section);
    void ReadFromIni(TFastIni *Ini, AnsiString Section);
public:
    //設定

    bool m_bCheckNew;

    AnsiString m_RecentFiles[10];
    AnsiString m_RecentFolders[10];

    int m_nUndoTimes;

    AnsiString m_WebSearch;//デフォルトのWebSearchのKey

    //プラグイン
    AnsiString m_PlugIn;
    void *m_hPlugIn;//プラグインハンドル（LoadLibrary）
    void __stdcall (*fepLoadPlugIn)(void);
    void __stdcall (*fepTimeOut)(IDocCtrl *Doc);
    bool __stdcall (*fepOperation)(IDocCtrl *Doc, char *operation);
    void __stdcall (*fepUnloadPlugIn)(void);
};
//---------------------------------------------------------------------------
class TSetting2Function
{
public:
    //生成、破棄
    TSetting2Function();
    virtual ~TSetting2Function();
public:
    //読み書き
    void WriteToIni(TIniFile *Ini);
    void ReadFromIni(TFastIni *Ini);
public:
    //設定

    TStringList *m_WebSearch;//ネット検索設定
};
//---------------------------------------------------------------------------
extern TSettingView SettingView;
extern TSettingFile SettingFile;
extern TSetting2Function Setting2Function;

class TMLText
{
public:
	TMLText();
public:
	AnsiString Cards;//カード
	AnsiString Links;//リンク
	AnsiString Labels;//ラベル
	AnsiString Line;//行
	AnsiString Len;//文字
	AnsiString Size;//サイズ
	AnsiString Title;//名前
	AnsiString Created;//作成日時
	AnsiString Edited;//編集日時
	AnsiString Viewed;//閲覧日時
	AnsiString Hits;//件

	AnsiString EditCardTitle;
    AnsiString EditCardText;
	AnsiString EditLinkTitle;
	AnsiString EditDrawing;

    AnsiString ChangeSize;//サイズの変更
    AnsiString FixCardPosition;//カード位置の固定
    AnsiString SetAsRootCard;//ルートカード指定

    AnsiString EditLink;//リンクの編集

    AnsiString AddLabel;//ラベルの追加
    AnsiString DeleteLabel;//ラベルの削除
	AnsiString ChangeLabel;//ラベルの変更
	AnsiString LabelName;
	AnsiString SelectLinkDestination;
	AnsiString SelectLabelOfDestination;
	AnsiString SelectLabel;

	AnsiString Color;

	AnsiString ArrangeCards;

	AnsiString ProcessingTextAnalysis;

	AnsiString Enable;//有効
	AnsiString Fold;//折畳
	AnsiString Show;//表示
	AnsiString Hide;//隠蔽
	AnsiString NoAssign;//未指定
	AnsiString NoLabel;//ラベルなし
	AnsiString SelectTargetCard;//リンク先カードの選択
	AnsiString Old;//古い順
	AnsiString New;//新しい順
	AnsiString Near;//近い順
	AnsiString Older;//より古い
	AnsiString Newer;//より新しい
	AnsiString SaveCheck;//データは変更されています。保存しますか？
	AnsiString FailedToSave;
	AnsiString NotFound;
	AnsiString AANotEnoughMemory;
};

extern TMLText MLText;

AnsiString LanguageFileName();
//---------------------------------------------------------------------------
#endif

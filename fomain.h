//---------------------------------------------------------------------------

#ifndef fomainH
#define fomainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "document.h"
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>
#include <IniFiles.hpp>
#include <ImgList.hpp>
#include <Jpeg.hpp>
/*
//#include "C:\My Documents\RAD Studio\5.0\Imports\HTTSLib_OCX.h"
#include <OleCtrls.hpp>
#include "AgentObjects_OCX.h"
#include "AgentObjects_TLB.h"
#include "ACTIVEVOICEPROJECTLib_OCX.h"
#include <OleServer.hpp>
*/
#include "textproc.h"
#include "foprogress.h"
#include <Graphics.hpp>
#include "dsdecoder.h"
#include "drawing.h"
#include <XPMan.hpp>
//---------------------------------------------------------------------------

#define MAXRANDOMCARDS 100

#define BGANIMATIONMAXOBJECTS 1024
#define BGANIMATIONOBJECTSIZE 16
//何ms編集がなかったらバックアップするか
#define UNDOBACKUPSPAN 2000
//---------------------------------------------------------------------------
class TEdit2 : public TMemo
{
public:
    __fastcall virtual TEdit2(Classes::TComponent* AOwner) : TMemo(AOwner){};
protected:
    virtual void __fastcall CreateParams(TCreateParams &Params);
};
//---------------------------------------------------------------------------

class TCardImage
{
public:
    TCardImage(AnsiString FN);//コンストラクタ。作られた瞬間イメージを読み込む
    virtual ~TCardImage();
public:
    AnsiString m_FN;//イメージのファイル名
    bool m_bExist;//読み込み成功しているか
    bool m_bUsed;//使用されているか（更新チェック時に外部から使用）
    TGraphic *m_Image;//イメージ
private:
    Graphics::TBitmap *m_BMP;//BMPイメージ
    TJPEGImage *m_Jpg;//Jpgイメージ
};

//---------------------------------------------------------------------------

class TCardVideo
{
public:
    TCardVideo(AnsiString FN);//コンストラクタ。作られた瞬間読み込む
    virtual ~TCardVideo();
public:
    AnsiString m_FN;//ファイル名
    bool m_bExist;//読み込み成功しているか
    bool m_bUsed;//使用されているか（更新チェック時に外部から使用）
	TDSDecoder *m_Video;//ビデオ
};

//---------------------------------------------------------------------------

class TFo_Main : public TForm, IProgress
{
__published:	// IDE 管理のコンポーネント
    TPanel *Pa_List;
    TSplitter *Sp_Left;
    TTimer *Ti_Check;
    TPopupMenu *PM_List;
    TMenuItem *PL_NewCard;
    TMenuItem *PL_DeleteCard;
    TMainMenu *MM_Menu;
    TMenuItem *M_File;
    TMenuItem *MF_New;
    TMenuItem *MF_Open;
    TMenuItem *MF_Save;
    TMenuItem *MF_SaveAs;
    TMenuItem *N1;
    TMenuItem *MF_Exit;
    TOpenDialog *OD;
    TSaveDialog *SD;
    TMenuItem *M_Edit;
    TMenuItem *ME_Undo;
    TMenuItem *N2;
    TMenuItem *ME_Copy;
    TMenuItem *ME_Cut;
    TMenuItem *ME_Paste;
    TMenuItem *ME_Delete;
    TMenuItem *M_Insert;
    TMenuItem *MI_NewLink;
    TPopupMenu *PM_Link;
    TMenuItem *PI_NewLink;
    TMenuItem *PI_DeleteLink;
    TPanel *Pa_Client;
    TPageControl *PC_Client;
    TTabSheet *TS_Browser;
    TPanel *Pa_BrowserBottom;
    TScrollBar *Sc_X;
    TPanel *Pa_BrowserBottomRight;
    TScrollBar *Sc_Y;
    TTabSheet *TS_Editor;
    TPanel *Pa_Editor;
    TSplitter *Splitter2;
    TListBox *LB_Link;
    TMemo *RE_Edit;
    TPanel *Pa_Card;
    TLabel *La_Title;
	TLabel *La_Label;
    TEdit *Ed_Title;
    TPanel *Pa_Top_;
    TLabel *Label2;
    TEdit *Ed_Size;
    TUpDown *UD_Size;
	TLabel *La_Card;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *La_Created;
    TLabel *La_Edited;
    TLabel *La_Viewed;
    TMenuItem *M_View;
    TMenuItem *MV_Link;
    TMenuItem *MV_Font;
    TMenuItem *MVL_Link;
    TMenuItem *MVL_LinkDirection;
    TMenuItem *MVL_LinkName;
    TMenuItem *MVF_Magnify;
    TMenuItem *MVF_DefaultSize;
    TMenuItem *MVF_Reduce;
    TMenuItem *MI_NewCard;
    TMenuItem *N3;
    TMenuItem *ME_Find;
    TMenuItem *ME_FindNext;
    TMenuItem *M_Help;
    TMenuItem *MH_About;
    TMenuItem *MD_SortCard;
    TMenuItem *MDS_Title;
    TMenuItem *MDS_Date_Created;
    TMenuItem *MDS_Date_Edited;
    TMenuItem *MDS_Date_Viewed;
    TMenuItem *MI_NewLabel;
    TButton *Bu_Label0;
    TPopupMenu *Po_Label;
    TMenuItem *PL_NoLabel;
    TMenuItem *N4;
    TMenuItem *N5;
    TMenuItem *ME_Label;
	TMenuItem *MH_EnableSpecialMenu;
    TMenuItem *MH_Contents;
    TMenuItem *N6;
    TMenuItem *MH_FrieveSite;
    TMenuItem *MH_FIPSite;
    TMenuItem *MDS_Title_I;
    TMenuItem *MDS_Date_Created_I;
    TMenuItem *MDS_Date_Edited_I;
    TMenuItem *MDS_Date_Viewed_I;
    TMenuItem *N7;
    TMenuItem *MF_RecentFiles;
    TMenuItem *MF_RecentFolders;
    TPanel *Pa_Link;
	TLabel *La_LinkTitle;
	TLabel *La_LinkLabel;
	TLabel *La_Link;
    TLabel *Label11;
    TLabel *Label12;
    TLabel *Label13;
    TLabel *Label14;
    TLabel *Label15;
    TLabel *Label16;
    TEdit *Ed_LinkTitle;
    TButton *Bu_LinkLabel0;
    TCheckBox *Ch_LinkDirection;
    TButton *Bu_LinkDirectionInverse;
    TMenuItem *ME_LinkLabel;
    TPopupMenu *Po_LinkLabel;
    TMenuItem *PL_NoLinkLabel;
    TMenuItem *MenuItem2;
    TMenuItem *N8;
    TMenuItem *MI_NewLinkLabel;
    TMenuItem *M_Animation;
    TMenuItem *MA_RandomFlash;
    TMenuItem *MA_RandomTrace;
    TMenuItem *MA_RandomScroll;
    TMenuItem *MV_FullScreen;
    TMenuItem *N9;
    TListBox *LB_List;
    TPanel *Pa_ListTop;
	TLabel *La_Cards;
    TMenuItem *N10;
    TMenuItem *PL_CardProperty;
    TMenuItem *MH_CheckLatest;
    TPanel *Pa_ListBottom;
    TMenuItem *MV_Label;
    TMenuItem *MVL_LabelCircle;
    TUpDown *UD_Sort;
    TPopupMenu *Po_ExtLink;
    TMenuItem *MI_NewExtLink;
    TOpenDialog *OD_Ext;
    TMenuItem *MV_Text;
    TMenuItem *MVT_Text;
    TMenuItem *MVT_Centering;
    TPopupMenu *PM_Editor;
    TMenuItem *PE_Undo;
    TMenuItem *N11;
    TMenuItem *PE_Cut;
    TMenuItem *PE_Copy;
    TMenuItem *PE_Paste;
    TMenuItem *PE_Delete;
    TMenuItem *PE_Sep1;
    TMenuItem *PE_CutToNewCard;
    TMenuItem *N13;
    TMenuItem *MF_Import;
    TMenuItem *MF_Export;
    TMenuItem *MFI_TxtFile;
    TMenuItem *MFE_TxtFile;
    TOpenDialog *OD_Txt;
    TSaveDialog *SD_Txt;
    TMenuItem *MFE_TxtFiles;
    TSaveDialog *SD_Html;
    TMenuItem *MFE_HtmlFiles;
    TButton *Bu_Enter;
    TMenuItem *PE_CutToNewCardTitle;
    TLabel *La_Count;
    TMenuItem *MDS_Shuffle;
    TSpeedButton *SB_Shape;
    TImageList *IL_Shape;
	TLabel *La_Size;
    TEdit *Ed_CardSize;
    TUpDown *UD_CardSize;
    TSpeedButton *SB_LinkShape;
    TStatusBar *SB_Status;
    TMenuItem *N14;
    TMenuItem *MV_StatusBar;
    TMenuItem *ME_SelectAll;
    TPrintDialog *PD_Text;
    TMenuItem *N15;
    TMenuItem *MF_PrintImage;
    TMenuItem *MF_PrintText;
    TPrintDialog *PD_Image;
    TMenuItem *MV_Others;
    TMenuItem *MVO_ChangeBackgroundColor;
    TColorDialog *CD;
    TMenuItem *MVO_ChangeFourgroundColor;
    TMenuItem *MV_ChangeLanguage;
    TMenuItem *MVL_LabelName;
    TMenuItem *MFE_TxtFileCardTitle;
    TMenuItem *MVC_English;
    TMenuItem *MVS_Score;
    TMenuItem *MDS_Score;
    TMenuItem *MDS_Score_I;
    TMenuItem *MV_Card;
    TMenuItem *MVC_Image;
    TMenuItem *MVI_80;
    TMenuItem *MVI_160;
    TMenuItem *MVI_320;
    TMenuItem *MVI_40;
    TMenuItem *MVI_32;
    TMenuItem *MVI_64;
    TMenuItem *MVI_120;
    TMenuItem *MVI_240;
    TMenuItem *MV_Score;
    TMenuItem *N17;
    TMenuItem *MV_Read;
    TMenuItem *MVR_Read;
    TMenuItem *MVR_ReadSetting;
    TOpenDialog *OD_ACS;
    TMenuItem *MVR_ChangeAgent;
    TMenuItem *MVT_WordWrap;
    TMenuItem *MVS_Authenticity;
    TMenuItem *MVS_StartingPoint;
    TMenuItem *MVS_Destination;
    TMenuItem *MVS_Links_In;
    TMenuItem *MVS_Links_Out;
    TMenuItem *MVS_Links_Total;
    TMenuItem *MVS_Links_InOut;
    TButton *Bu_Test;
    TPopupMenu *PM_ArrangeType;
    TMenuItem *PAT_Normalize;
    TMenuItem *PAT_Repulsion;
    TMenuItem *PAT_Link;
    TMenuItem *PAT_Label;
    TMenuItem *PAT_Index;
    TMenuItem *PAT_Similarity;
    TMenuItem *MVL_LabelRectangle;
    TMenuItem *N19;
    TMenuItem *PAT_Matrix;
    TSpeedButton *SB_Fix;
    TImage *Im_Wall;
    TMenuItem *N20;
    TMenuItem *MVO_WallPaper;
    TMenuItem *MVO_TileWallPaper;
    TMenuItem *MVO_FixWallPaper;
    TOpenDialog *OD_WallPaper;
    TMenuItem *N21;
    TMenuItem *MVF_ChangeFont;
    TFontDialog *FD_Font;
    TMenuItem *MVO_Overview;
    TMenuItem *N22;
    TMenuItem *PAT_SoftLink;
    TMenuItem *PAT_SoftLabel;
    TMenuItem *PAT_SoftIndex;
    TMenuItem *PAT_SimilaritySoft;
    TMenuItem *N18;
    TMenuItem *N23;
    TMenuItem *PAT_LinkMatrix;
    TMenuItem *PAT_LabelMatrix;
    TMenuItem *PAT_SimilarityMatrix;
    TMenuItem *MV_Browser;
    TMenuItem *MV_Editor;
    TMenuItem *MVC_Video;
    TMenuItem *MVC_CardShadow;
    TMenuItem *MVL_LinkHemming;
    TMenuItem *MVL_LinkShadow;
    TMenuItem *N24;
    TMenuItem *N25;
    TMenuItem *MVC_TickerVisible;
    TMenuItem *MVC_Ticker1Line;
    TMenuItem *MVC_Ticker2Lines;
    TMenuItem *MVC_CardGradation;
    TPopupMenu *PM_BNoSelect;
    TPopupMenu *PM_BCardSelect;
    TPopupMenu *PM_BLinkSelect;
    TMenuItem *PBN_NewCard;
    TMenuItem *PBC_NewCard;
    TMenuItem *PBC_NewLink;
    TMenuItem *PBC_NewExtLink;
    TMenuItem *PBC_NewLabel;
    TMenuItem *N16;
    TMenuItem *PBC_SetAsDefault;
    TMenuItem *N26;
    TMenuItem *PBC_DeleteCard;
    TMenuItem *N27;
    TMenuItem *PBC_Property;
    TMenuItem *PBL_NewCard;
    TMenuItem *PBL_InsertCard;
    TMenuItem *PBL_NewLinkLabel;
    TMenuItem *N28;
    TMenuItem *PBL_SetAsDefault;
    TMenuItem *N29;
    TMenuItem *PBL_DeleteLink;
    TMenuItem *PBC_NewChildCard;
    TMenuItem *PBC_NewBrotherCard;
    TImageList *IL_LinkShape;
    TMenuItem *ME_BatchConversion;
    TMenuItem *MEC_AllCards;
    TMenuItem *MEC_AllLinks;
    TMenuItem *N30;
    TMenuItem *N31;
    TMenuItem *MFE_ClipboardCardTitle;
    TMenuItem *MFE_ClipboardCardText;
    TMenuItem *MFE_ClipboardBMP;
    TMenuItem *N32;
    TMenuItem *MFE_BMPFile;
    TMenuItem *MFE_JPEGFile;
    TSaveDialog *SD_BMP;
    TSaveDialog *SD_JPEG;
    TMenuItem *MFI_HierarchicalTextFile;
    TMenuItem *MFI_TxtFilesinaFolder;
    TMenuItem *N33;
    TMenuItem *MVO_AntiAliasing;
    TMenuItem *MVO_AA2x;
    TMenuItem *MVO_AA4x;
    TButton *Bu_BrowserTest;
    TMenuItem *N34;
    TMenuItem *MVO_BGAnimation;
    TMenuItem *MVO_BGAnimationType;
    TMenuItem *MVOB_Bubble;
    TMenuItem *MVOB_MovingLine;
    TMenuItem *PE_SelectAll;
    TMenuItem *ME_Redo;
    TMenuItem *PE_Redo;
    TMenuItem *N35;
    TMenuItem *MVT_EditInBrowser;
    TMenuItem *MVT_AlwaysShowEditor;
    TMenuItem *MVT_UnderTheCard;
    TMenuItem *MVT_BrowserRight;
    TMenuItem *MVT_BrowserBottom;
    TMenuItem *MV_EditInBrowser;
    TMenuItem *MV_EditTitle;
    TMenuItem *PBN_Undo;
    TMenuItem *PBN_Redo;
    TMenuItem *N36;
    TMenuItem *N37;
    TMenuItem *PBN_Paste;
    TMenuItem *N38;
    TMenuItem *PBN_Select;
    TMenuItem *PBN_Link;
    TMenuItem *PBC_Select;
    TMenuItem *PBC_Link;
    TMenuItem *N39;
    TMenuItem *PBC_Undo;
    TMenuItem *PBC_Redo;
    TMenuItem *N40;
    TMenuItem *PBC_Paste;
    TMenuItem *N41;
    TMenuItem *PBC_Cut;
    TMenuItem *PBC_Copy;
    TMenuItem *PBL_Select;
    TMenuItem *PBL_Link;
    TMenuItem *N42;
    TMenuItem *PBL_Undo;
    TMenuItem *PBL_Redo;
    TMenuItem *N43;
    TPanel *Pa_Files;
    TListBox *LB_FileList;
	TPanel *Pa_FilesTop;
    TLabel *La_Files;
	TPanel *Pa_FilesBottom;
    TSplitter *Sp_Left2;
    TMenuItem *MA_RandomJump;
    TMenuItem *MA_RandomMap;
    TMenuItem *MVO_CursorAnimation;
    TMenuItem *MEC_AllLinksDirection;
    TMenuItem *N44;
    TMenuItem *PAT_TreeRadial;
    TSpeedButton *SB_Top;
    TMenuItem *N45;
    TMenuItem *MVO_CardList;
    TMenuItem *MVO_FileList;
    TMenuItem *N46;
    TMenuItem *ME_WebSearch;
    TMenuItem *N12;
    TMenuItem *PBC_WebSearch;
    TMenuItem *N47;
    TMenuItem *PBL_WebSearch;
    TMenuItem *MVOB_Snow;
    TMenuItem *MVOB_CherryBlossom;
    TPanel *Pa_EditorTop;
    TSplitter *Sp_EditorRelated;
    TSpeedButton *SB_EditorRelated;
    TPanel *Pa_EditorRelated;
    TMemo *Me_EditorRelated;
    TPanel *Pa_EditorRelatedTop;
    TPanel *Pa_GlobalSearch;
    TPanel *Pa_GlobalSearchTop;
    TLabel *La_GlobalSearchTop;
    TPanel *Pa_SearchResultBottom;
    TSplitter *Sp_GlobalSearch;
    TSpeedButton *SB_CloseGlobalSearch;
    TLabel *La_SearchResultKeyword;
    TScrollBar *Sc_GlobalSearch;
    TPaintBox *PB_GlobalSearch;
    TLabel *La_GlobalSearchCount;
    TMenuItem *ME_FindPrevious;
    TMenuItem *PE_CutToNewCardTitleWithLink;
    TMenuItem *MVO_NoScrollLag;
    TMenuItem *MFI_HierarchicalTextFile2;
    TMenuItem *MVS_TextLength;
    TMenuItem *PAT_MatrixIndex;
    TSplitter *Sp_BrowserRight;
    TSplitter *Sp_BrowserBottom;
    TTabSheet *TS_Statistics;
	TPanel *Pa_StatisticsTop;
    TPaintBox *PB_Statistics;
    TScrollBar *Sc_StatisticsY;
    TMenuItem *N48;
    TMenuItem *MI_NewCardLinkstoAllCardswithDesignatedLabel;
    TMenuItem *MI_LinktoAllCardswithDesignatedLabel;
    TMenuItem *MI_AddDesignatedLabeltoAllDestinationCards;
    TSpeedButton *SB_StatisticsSort;
    TSpeedButton *Bu_StatisticsKey;
	TLabel *La_StatisticsKey;
    TPopupMenu *PM_StatisticsKey;
	TMenuItem *PMSK_Label;
    TMenuItem *PMSK_NumberOfLink;
	TMenuItem *PMSK_CreatedDate;
    TMenuItem *PMSK_EditedDate;
    TMenuItem *PMSK_ViewedDate;
    TMenuItem *PMSKC_Year;
    TMenuItem *PMSKC_Month;
    TMenuItem *PMSKC_Day;
    TMenuItem *PMSKC_Week;
    TMenuItem *PMSKC_Hour;
    TMenuItem *PMSKE_Year;
    TMenuItem *PMSKE_Month;
    TMenuItem *PMSKE_Day;
    TMenuItem *PMSKE_Week;
    TMenuItem *PMSKE_Hour;
    TMenuItem *PMSKV_Year;
    TMenuItem *PMSKV_Month;
    TMenuItem *PMSKV_Day;
    TMenuItem *PMSKV_Week;
    TMenuItem *PMSKV_Hour;
    TMenuItem *PMSKL_Total;
    TMenuItem *PMSKL_Source;
	TMenuItem *PMSKL_Destination;
    TTabSheet *TS_Drawing;
    TPaintBox *PB_Drawing;
	TPanel *Pa_BrowserTop;
	TPanel *Pa_DrawingTop;
	TLabel *La_DZoom;
	TTrackBar *TB_DZoom;
	TLabel *La_Zoom;
	TSpeedButton *SB_Cursor;
	TSpeedButton *SB_Line;
	TSpeedButton *SB_View;
	TSpeedButton *SB_ToolLabel;
	TSpeedButton *SB_ToolLinkLabel;
	TSpeedButton *SB_Arrange;
	TSpeedButton *SB_AutoScroll;
	TSpeedButton *SB_AutoZoom;
	TSpeedButton *SB_ArrangeRefresh;
	TLabel *La_Arrange;
	TSpeedButton *Bu_ArrangeType;
	TTrackBar *TB_Zoom;
	TComboBox *Co_Arrange;
	TButton *Bu_Shuffle;
	TEdit *Ed_FindCard;
	TButton *Bu_FindCard;
	TSpeedButton *SB_DLine;
	TSpeedButton *SB_FreeHand;
	TSpeedButton *SB_DCursor;
	TSpeedButton *SB_Circle;
	TSpeedButton *SB_Text;
	TSpeedButton *SB_Box;
	TXPManifest *XPMan;
	TPanel *Pa_Browser;
	TPaintBox *PB_Browser;
	TLabel *La_PenColor;
	TShape *Sh_DColorDefault;
	TLabel *La_PenColorDefault;
	TShape *Sh_DColor;
	TBitBtn *Bu_DColor;
	TColorDialog *CD_Color;
	TPopupMenu *PM_Drawing;
	TMenuItem *PD_Undo;
	TMenuItem *PD_Redo;
	TMenuItem *MenuItem4;
	TMenuItem *PD_Cut;
	TMenuItem *PD_Copy;
	TMenuItem *PD_Paste;
	TMenuItem *PD_Delete;
	TMenuItem *PD_SelectAll;
	TMenuItem *MVC_Drawing;
	TMenuItem *MFE_HierarchicalTextFile;
	TMenuItem *ME_Replace;
	TMenuItem *M_Special;
	TMenuItem *MS_ResetAllDates;
	TMenuItem *MS_Demo;
	TMenuItem *MS_OutputWordNgram;
	TSaveDialog *SD_CSV;
	TMenuItem *MS_TextAnalysisTest;
	TMenuItem *MS_ImportCSV;
	TOpenDialog *OD_CSV;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall Ti_CheckTimer(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Ed_TitleChange(TObject *Sender);
    void __fastcall RE_EditChange(TObject *Sender);
    void __fastcall PL_DeleteCardClick(TObject *Sender);
    void __fastcall MF_NewClick(TObject *Sender);
    void __fastcall MF_SaveClick(TObject *Sender);
    void __fastcall MF_SaveAsClick(TObject *Sender);
    void __fastcall MF_OpenClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall MF_ExitClick(TObject *Sender);
    void __fastcall ME_UndoClick(TObject *Sender);
    void __fastcall ME_CutClick(TObject *Sender);
    void __fastcall ME_CopyClick(TObject *Sender);
    void __fastcall ME_PasteClick(TObject *Sender);
    void __fastcall ME_DeleteClick(TObject *Sender);
    void __fastcall MI_NewLinkClick(TObject *Sender);
    void __fastcall LB_LinkDblClick(TObject *Sender);
    void __fastcall PI_DeleteLinkClick(TObject *Sender);
    void __fastcall PB_BrowserPaint(TObject *Sender);
    void __fastcall LB_ListDblClick(TObject *Sender);
    void __fastcall PB_BrowserMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall PB_BrowserDblClick(TObject *Sender);
    void __fastcall PB_BrowserMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall PB_BrowserMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall Sc_YChange(TObject *Sender);
    void __fastcall Ed_SizeChange(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall MVF_DefaultSizeClick(TObject *Sender);
    void __fastcall MVF_MagnifyClick(TObject *Sender);
    void __fastcall MVF_ReduceClick(TObject *Sender);
    void __fastcall MVL_LinkClick(TObject *Sender);
    void __fastcall MVL_LinkDirectionClick(TObject *Sender);
    void __fastcall MVL_LinkNameClick(TObject *Sender);
    void __fastcall MI_NewCardClick(TObject *Sender);
    void __fastcall ME_FindClick(TObject *Sender);
    void __fastcall ME_FindNextClick(TObject *Sender);
    void __fastcall MH_AboutClick(TObject *Sender);
    void __fastcall MDS_TitleClick(TObject *Sender);
    void __fastcall MDS_Date_CreatedClick(TObject *Sender);
    void __fastcall MDS_Date_EditedClick(TObject *Sender);
    void __fastcall MDS_Date_ViewedClick(TObject *Sender);
    void __fastcall MI_NewLabelClick(TObject *Sender);
    void __fastcall Bu_Label0Click(TObject *Sender);
    void __fastcall PL_Click(TObject *Sender);
    void __fastcall ME_LabelClick(TObject *Sender);
    void __fastcall TS_BrowserShow(TObject *Sender);
    void __fastcall MH_ContentsClick(TObject *Sender);
    void __fastcall MH_FrieveSiteClick(TObject *Sender);
    void __fastcall MH_FIPSiteClick(TObject *Sender);
    void __fastcall MF_RecentFilesClick(TObject *Sender);
    void __fastcall MF_RecentFoldersClick(TObject *Sender);
    void __fastcall Ed_LinkTitleChange(TObject *Sender);
    void __fastcall Ch_LinkDirectionClick(TObject *Sender);
    void __fastcall Bu_LinkDirectionInverseClick(TObject *Sender);
    void __fastcall ME_LinkLabelClick(TObject *Sender);
    void __fastcall Bu_LinkLabel0Click(TObject *Sender);
    void __fastcall PL_NoLinkLabelClick(TObject *Sender);
    void __fastcall SB_ViewClick(TObject *Sender);
    void __fastcall MI_NewLinkLabelClick(TObject *Sender);
    void __fastcall MA_RandomFlashClick(TObject *Sender);
    void __fastcall MA_RandomScrollClick(TObject *Sender);
    void __fastcall MA_RandomTraceClick(TObject *Sender);
    void __fastcall MV_FullScreenClick(TObject *Sender);
    void __fastcall SB_ToolLabelClick(TObject *Sender);
    void __fastcall SB_ToolLinkLabelClick(TObject *Sender);
    void __fastcall PL_CardPropertyClick(TObject *Sender);
    void __fastcall MH_CheckLatestClick(TObject *Sender);
    void __fastcall MH_EnableSpecialMenuClick(TObject *Sender);
    void __fastcall MVL_LabelCircleClick(TObject *Sender);
    void __fastcall Pa_ListTopResize(TObject *Sender);
    void __fastcall UD_SortClick(TObject *Sender, TUDBtnType Button);
    void __fastcall RE_EditMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall PM_ExtLinkClick(TObject *Sender);
    void __fastcall MI_NewExtLinkClick(TObject *Sender);
    void __fastcall UD_SortExit(TObject *Sender);
    void __fastcall MVT_TextClick(TObject *Sender);
    void __fastcall MVT_CenteringClick(TObject *Sender);
    void __fastcall PE_CutToNewCardClick(TObject *Sender);
    void __fastcall MFI_TxtFileClick(TObject *Sender);
    void __fastcall MFE_TxtFileClick(TObject *Sender);
    void __fastcall MFE_TxtFilesClick(TObject *Sender);
    void __fastcall MFE_HtmlFilesClick(TObject *Sender);
    void __fastcall Ed_TitleBExit(TObject *Sender);
    void __fastcall Ed_TitleBEnter(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall Bu_EnterClick(TObject *Sender);
    void __fastcall Ed_TitleBKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall PE_CutToNewCardTitleClick(TObject *Sender);
    void __fastcall TB_ZoomChange(TObject *Sender);
    void __fastcall Bu_ShuffleClick(TObject *Sender);
    void __fastcall MDS_ShuffleClick(TObject *Sender);
    void __fastcall MH_DemoClick(TObject *Sender);
    void __fastcall Ed_FindCardEnter(TObject *Sender);
    void __fastcall Ed_FindCardExit(TObject *Sender);
    void __fastcall Bu_FindCardClick(TObject *Sender);
    void __fastcall Ed_FindCardChange(TObject *Sender);
    void __fastcall SB_ShapeClick(TObject *Sender);
    void __fastcall Ed_CardSizeChange(TObject *Sender);
    void __fastcall PC_ClientChange(TObject *Sender);
    void __fastcall MV_StatusBarClick(TObject *Sender);
    void __fastcall RE_EditKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall RE_EditMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall ME_SelectAllClick(TObject *Sender);
    void __fastcall MF_PrintImageClick(TObject *Sender);
    void __fastcall MF_PrintTextClick(TObject *Sender);
    void __fastcall MVO_ChangeBackgroundColorClick(TObject *Sender);
    void __fastcall MVO_ChangeFourgroundColorClick(TObject *Sender);
    void __fastcall MVL_LabelNameClick(TObject *Sender);
    void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall MFE_TxtFileCardTitleClick(TObject *Sender);
    void __fastcall MVC_EnglishClick(TObject *Sender);
    void __fastcall MVS_ScoreClick(TObject *Sender);
    void __fastcall MDS_ScoreClick(TObject *Sender);
    void __fastcall MVC_ImageClick(TObject *Sender);
    void __fastcall MVI_ImageLimitationClick(TObject *Sender);
    void __fastcall MVR_ReadClick(TObject *Sender);
    void __fastcall MVR_ReadSettingClick(TObject *Sender);
    void __fastcall MVR_ChangeAgentClick(TObject *Sender);
    void __fastcall MVT_WordWrapClick(TObject *Sender);
    void __fastcall MVS_ScoreClickType(TObject *Sender);
    void __fastcall Bu_TestClick(TObject *Sender);
    void __fastcall PAT_ArrangeTypeClick(TObject *Sender);
    void __fastcall Bu_ArrangeTypeClick(TObject *Sender);
    void __fastcall SB_ArrangeRefreshClick(TObject *Sender);
    void __fastcall SB_ArrangeClick(TObject *Sender);
    void __fastcall MVL_LabelRectangleClick(TObject *Sender);
    void __fastcall SB_FixClick(TObject *Sender);
    void __fastcall MVO_FixWallPaperClick(TObject *Sender);
    void __fastcall MVO_TileWallPaperClick(TObject *Sender);
    void __fastcall MVO_WallPaperClick(TObject *Sender);
    void __fastcall MVF_ChangeFontClick(TObject *Sender);
    void __fastcall MVO_OverviewClick(TObject *Sender);
    void __fastcall SB_AutoScrollClick(TObject *Sender);
    void __fastcall MV_BrowserClick(TObject *Sender);
    void __fastcall MV_EditorClick(TObject *Sender);
    void __fastcall MVC_VideoClick(TObject *Sender);
    void __fastcall MVL_LinkHemmingClick(TObject *Sender);
    void __fastcall MVL_LinkShadowClick(TObject *Sender);
    void __fastcall MVC_CardShadowClick(TObject *Sender);
    void __fastcall MVC_TickerVisibleClick(TObject *Sender);
    void __fastcall MVC_TickerLineClick(TObject *Sender);
    void __fastcall MVC_CardGradationClick(TObject *Sender);
    void __fastcall PBN_NewCardClick(TObject *Sender);
    void __fastcall PBL_DeleteLinkClick(TObject *Sender);
    void __fastcall PBL_InsertCardClick(TObject *Sender);
    void __fastcall PBC_NewChildCardClick(TObject *Sender);
    void __fastcall PBC_NewBrotherCardClick(TObject *Sender);
    void __fastcall PBC_SetAsDefaultClick(TObject *Sender);
    void __fastcall SB_LinkShapeClick(TObject *Sender);
    void __fastcall PBL_SetAsDefaultClick(TObject *Sender);
    void __fastcall MEC_AllCardsClick(TObject *Sender);
    void __fastcall MEC_AllLinksClick(TObject *Sender);
    void __fastcall MFE_ClipboardBMPClick(TObject *Sender);
    void __fastcall MFE_ClipboardCardTitleClick(TObject *Sender);
    void __fastcall MFE_ClipboardCardTextClick(TObject *Sender);
    void __fastcall MFE_BMPFileClick(TObject *Sender);
    void __fastcall MFE_JPEGFileClick(TObject *Sender);
    void __fastcall MFI_HierarchicalTextFileClick(TObject *Sender);
    void __fastcall MFI_TxtFilesinaFolderClick(TObject *Sender);
    void __fastcall MVO_AntiAliasingClick(TObject *Sender);
    void __fastcall MVO_AAClick(TObject *Sender);
    void __fastcall Bu_BrowserTestClick(TObject *Sender);
    void __fastcall MVO_BGAnimationClick(TObject *Sender);
    void __fastcall MVOB_BGAnimationTypeClick(TObject *Sender);
    void __fastcall ME_RedoClick(TObject *Sender);
    void __fastcall LB_ListClick(TObject *Sender);
    void __fastcall LB_ListMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall LB_ListMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall RE_EditExit(TObject *Sender);
    void __fastcall RE_EditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall MVT_BrowserPosClick(TObject *Sender);
    void __fastcall MVT_AlwaysShowEditorClick(TObject *Sender);
    void __fastcall MVT_EditInBrowserClick(TObject *Sender);
    void __fastcall MV_EditInBrowserClick(TObject *Sender);
    void __fastcall MV_EditTitleClick(TObject *Sender);
    void __fastcall PBN_SelectClick(TObject *Sender);
    void __fastcall PBN_LinkClick(TObject *Sender);
    void __fastcall LB_FileListClick(TObject *Sender);
    void __fastcall LB_FileListDblClick(TObject *Sender);
    void __fastcall LB_FileListMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall LB_FileListMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall MA_RandomJumpClick(TObject *Sender);
    void __fastcall MA_RandomMapClick(TObject *Sender);
    void __fastcall MVO_CursorAnimationClick(TObject *Sender);
    void __fastcall MEC_AllLinksDirectionClick(TObject *Sender);
    void __fastcall SB_TopClick(TObject *Sender);
    void __fastcall MVO_CardListClick(TObject *Sender);
    void __fastcall MVO_FileListClick(TObject *Sender);
    void __fastcall SB_EditorRelatedClick(TObject *Sender);
    void __fastcall SB_CloseGlobalSearchClick(TObject *Sender);
    void __fastcall PB_GlobalSearchPaint(TObject *Sender);
    void __fastcall Sc_GlobalSearchChange(TObject *Sender);
    void __fastcall PB_GlobalSearchMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall Sc_GlobalSearchKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ME_FindPreviousClick(TObject *Sender);
    void __fastcall PE_CutToNewCardTitleWithLinkClick(TObject *Sender);
    void __fastcall MVO_NoScrollLagClick(TObject *Sender);
    void __fastcall MFI_HierarchicalTextFile2Click(TObject *Sender);
    void __fastcall TB_ZoomKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall Sp_BrowserRightMoved(TObject *Sender);
    void __fastcall Sp_BrowserBottomMoved(TObject *Sender);
    void __fastcall LB_LinkClick(TObject *Sender);
    void __fastcall RE_EditEnter(TObject *Sender);
    void __fastcall PB_StatisticsPaint(TObject *Sender);
    void __fastcall Sc_StatisticsYChange(TObject *Sender);
    void __fastcall MI_LinktoAllCardswithDesignatedLabelClick(
          TObject *Sender);
    void __fastcall MI_NewCardLinkstoAllCardswithDesignatedLabelClick(
          TObject *Sender);
    void __fastcall MI_AddDesignatedLabeltoAllDestinationCardsClick(
          TObject *Sender);
    void __fastcall SB_StatisticsSortClick(TObject *Sender);
    void __fastcall PB_StatisticsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall Bu_StatisticsKeyClick(TObject *Sender);
    void __fastcall PM_StatisticKeyClick(TObject *Sender);
    void __fastcall PB_DrawingPaint(TObject *Sender);
    void __fastcall PB_DrawingMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall PB_DrawingMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall PB_DrawingMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall SB_DrawingToolClick(TObject *Sender);
	void __fastcall Bu_DColorClick(TObject *Sender);
	void __fastcall Sh_DColorDefaultMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall Sh_DColorMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall MVC_DrawingClick(TObject *Sender);
	void __fastcall MFE_HierarchicalTextFileClick(TObject *Sender);
	void __fastcall ME_ReplaceClick(TObject *Sender);
	void __fastcall MS_ResetAllDatesClick(TObject *Sender);
	void __fastcall MS_OutputWordNgramClick(TObject *Sender);
	void __fastcall MS_TextAnalysisTestClick(TObject *Sender);
	void __fastcall MS_ImportCSVClick(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
    __fastcall TFo_Main(TComponent* Owner);

public:
    //ドキュメント
    TDocument *m_Document;
    //Undoバッファ
    TUndoRedo *m_UndoRedo;
public:
    //表示更新用
    int m_nRefreshListCount;
    int m_nRefreshLinkCount;
    int m_nRefreshLabelCount;
    int m_nTargetCard;//編集中のカード
    int m_nTargetLink;//編集中のリンク
    int m_nTargetCard2;//リンク先カード
    int m_nCurrentCard;//表示中のカード（TargetCardと異なったら画面更新）
    int m_nCurrentLink;//表示中のリンク（TargetLinkと異なったら画面更新）

    void RefreshLinks();//リンク表示更新
    TList *m_LinkIndexes;//表示中のリンクIndex

    //ラベル
    void RefreshLabel();//ラベル更新
    TButton *Bu_Label[MAXLABELS];
    TList *MI_Labels;
    TButton *Bu_LinkLabel[MAXLABELS];
    TList *MI_LinkLabels;
    void FreeMILabels();

    int m_nToolLabel;//ラベル付けボタンで付けるラベル
    int m_nToolLinkLabel;//ラベル付けボタンで付けるラベル

    //ブラウザ表示
    int m_nBrowserWidth, m_nBrowserHeight;
    int m_nBGColor, m_nFGColor;
    float m_fFontZoom;
    int m_nXOffset, m_nYOffset;//プリンタ表示時のオフセット

    int m_nTmpCardsCount;//以下の一時変数格納場所のサイズ（数が変わっていたらPaintSubでメモリ再確保）
    int m_nTmpLinksCount;//以下の一時変数格納場所のサイズ（数が変わっていたらPaintSubでメモリ再確保）
    bool *m_CardVisible;
    bool *m_LinkVisible;
    AnsiString *m_CardTitle;//一時カードタイトル（TitleはFoldされていると変わる）
    bool *m_CardRelated;
    int *m_CardAssign;//Fold用。これの示す番号のカードとして移動する
    int *m_CardShape;//カードの形（一時。Foldされているときは多数決で決まる）
    //カードのサイズも本当はFoldされている平均にするべき！？
    float *m_CardX;//表示する実座標（画面サイズは考慮しない）
    float *m_CardY;//表示する実座標（画面サイズは考慮しない）
    int *m_CardWidth;//幅（実座標。Pixel）
    int *m_CardHeight;//高さ（実座標。Pixel）
    int m_nFontHeight;
    float m_fZoomSD;//Zoomする標準偏差。0.21が標準。小さくすると広い範囲を表示
    float m_fTickerSpeed;//Tickerの移動量
    unsigned int m_nLastTimeOut;
    bool m_bRedrawRequested; 

    //フレームレート計算
    float m_fFPS;
    int m_nFPSCount;
    unsigned int m_nLastFPSTime;

    //アレンジ
    TDMatrix *m_SimMatrix;//カード毎の類似度Matrix
    void PrepareArrange();
    void RefreshSimMatrix();
    void ProgressFunc();//Progressからの呼び出し用
    void RefreshSimMatrix_();//Progressからの呼び出し用
    void FreeSimMatrix();
    void BrowserArrangeByNormalize(float ratio = 1.0f);//ノーマライズ
    void BrowserArrangeByRepulsion(float ratio = 1.0f);//反発力のみに従い座標更新
    void BrowserArrangeByLink(float ratio = 1.0f);//リンクに従い座標更新
    void BrowserArrangeByLabel(float ratio = 1.0f);//ラベルに従い座標更新
    void BrowserArrangeByIndex(float ratio = 1.0f);//カード順に従い座標更新
    void BrowserArrangeByDate(int dateindex, float ratio = 1.0f);//日付に従い座標更新
    void BrowserArrangeByFold(float ratio = 1.0f);//折りたたまれたラベルに従い座標更新
    void BrowserArrangeBySimilarity(float ratio = 1.0f);//類似度に従い座標更新
    void BrowserArrangeByMatrix(int type, float ratio = 1.0f);//格子状に配置
    void BrowserArrangeByTree(int type, float ratio = 1.0f);//階層表示

    //アレンジ（格子状配置）
    int m_nMatrixWidth;//格子状配置の際の幅
    int m_nMatrixHeight;//格子状配置の際の高さ
    void PrepareMatrixArrange(int type);//格子サイズを決め、全カードを格子に配置。typeはArrangeType
    void PrepareMatrixArrange_AssignToMatrix(int i, int *tmatrix, float minx, float maxx, float miny, float maxy);//i番目のカードを、tmatrixで示すマトリクスに格納
    void PrepareMatrixArrange_AssignToMatrix2(int i, int *tmatrix, float minx, float maxx, float miny, float maxy);//i番目のカードを、tmatrixで示すマトリクスに格納（円状から）

    //AutoScroll
    float m_fBrowserScrollRatio;//スクロール量
    float m_fBrowserScrollLastD;//最後のターゲットまでの距離
    int m_nScrollTargetX;//Overviewからのスクロール要求
    int m_nScrollTargetY;
    bool BrowserAutoScroll();//現在カードが中心に来るように移動。返り値は座標が更新されたかどうか

    //アニメーション
    TDocument *m_DocBeforeAnimation;//アニメーション前のドキュメントをバックアップ
    int m_nAnimation;//アニメーション中かどうか。0=アニメーション無し、1=RandomCard、2=RandomCard2、3=RandomTrace
    int m_nAnimationCount;//アニメーションの進行状況（使い方はアニメーションによる）
    //アニメーション中の各種バックアップ
    int m_nAnimationBak_ArrangeType;
    bool m_bAnimationBak_Arrange;
    bool m_bAnimationBak_AutoScroll;
    bool m_bAnimationBak_AutoZoom;

    void Animation_RandomCard();

    int m_Animation_RC2Idxs[MAXRANDOMCARDS];
    float m_Animation_RC2AXs[MAXRANDOMCARDS];
    float m_Animation_RC2AYs[MAXRANDOMCARDS];
    void Animation_RandomCard2();
    void Animation_RandomJump();

    void Animation_RandomMap();

    int m_nAnimation_LastCard;
    void Animation_RandomTrace();

    //タイトル入力
    TEdit2 *Ed_TitleB;
    void SetEdTitleBPos();
    void CloseEditBox();

    //Browserで本文編集
    void SetTextEditPos();
    void CloseTextEditBox();
private:
    //ブラウザ表示サブ
    void BrowserArrange_Initialize(float *CardX, float *CardY, bool *Norm);
    void BrowserArrange_Link(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio);//リンクのあるカードの近くへ
    void BrowserArrange_Repulsion(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio);//他のカードの遠くへ
    void BrowserArrange_LabelPrepare();//各ラベルの中心座標計算
    void BrowserArrange_Label(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio);//同じラベルの近く、違うラベルの遠くへ
    void BrowserArrange_Index(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio);//indexが近いものほど近くに
    void BrowserArrange_Date(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio, int dateindex);//日付が近いものほど近くに
    void BrowserArrange_Similarity(int i, TCard *Card, float *CardX, float *CardY, bool *Norm, float ratio, int *idxtable);//類似カードの近くへ
    void BrowserArrange_Memory(float *CardX, float *CardY, bool *Norm);
    void BrowserArrange_Normalize(float *CardX, float *CardY, bool *Norm);
public:
    //ファイル
    TIniFile *Ini;

    TMenuItem *MI_RecentFiles[10];
    TMenuItem *MI_RecentFolders[10];

    void LoadSub(AnsiString FN, bool bSoftLoad = false, bool bRefreshRecent = true);
    bool Save();//上書きもしくは名前をつけて保存
    bool SaveAs();//名前をつけて保存
    bool SaveCheck();//ファイルが変更されています。保存しますか？
    void RefreshRecent(AnsiString FN);
public:
    //操作用
    int m_bMDownBrowser;//1=通常ドラッグ、2=リンクのDest編集、3=リンクのFrom編集
    int m_bDblClicked;
    bool m_bTitleEditRequested;
    bool m_bTextEditRequested;
    unsigned int m_uMDownBrowserLast;
    bool m_bMDownBrowserMoved;//マウスボタンを押してから動いたかどうか
    int m_nMDownBrowserOffsetX;
    int m_nMDownBrowserOffsetY;
    int m_nMDownBrowserX;//マウスを押したときの座標
    int m_nMDownBrowserY;
    int m_nMDownTargetX;//リンク線先
    int m_nMDownTargetY;
    int m_nMDownBrowserScrollX;
    int m_nMDownBrowserScrollY;
    bool m_bShowRecent;//最近表示したカードを強調表示（スペースキー）

    bool CheckTreeButton(int index, int X, int Y);
private:
    //検索
    bool m_bSearching;

    void Search(int SearchRequest);
    //0x1カードタイトル、0x2本文、0x4Browserから検索、0x8ダイアログから検索
    //0x10 大文字小文字を区別しない

    TList *m_GlobalSearchResult;//グローバル検索結果（ヒットしたカードIDのリスト）
    int m_GlobalSearchItemHeight;
    WideString m_GlobalSearchKeyword;
    int m_GlobalSearchOption;
    int m_GlobalSearchCursorIndex;
    void GlobalSearch(int SearchRequest);//ヒットした結果をm_GlobalSearchResultに入れる
    int ReplaceAll(int SearchRequest);//すべて置換。置換に成功した数を返す

    TList *MI_WebSearch;
    WideString GetSelText();//現在選択中のテキストを得る
    void __fastcall ME_WebSearchClick(TObject *Sender);
private:
    //Utils
    void DispPosToCardPos(float dx, float dy, float &cx, float &cy);
    void CardPosToDispPos(float cx, float cy, float &dx, float &dy);
    void FilteringCard();//Labelで表示するカードを選別する
    void RefreshLaStatus();//ステータスラベルの更新
    void RefreshWallPaper();
    void RefreshFileList();

    void MoveToSelectedAndRecentCard(); //選択されているカードで、一番最近触ったカードに移動
    float MoveDistance(float sx, float sy, float dx, float dy, int direction);//direction方向に動く際のs座標からd座標への距離
    void LinkLimitationSub(int targetcard, int targetlink, int linkdepth, int *Score, bool linkdirection, bool linkbackward);

    void BrowserNewCard();
    void BrowserNewChildCard(bool binverse);
    void BrowserNewBrotherCard();
    void BrowserInsertCardToLink();

    void SetFont();//SettingView.m_Fontの名前、Charsetを適用

    void AddTxtsInFolder(AnsiString TopDir, TCard *Parent);

    void ExitFullScreen();

    void SetCardAssign();//Paint時以外でLabelのFoldを考慮する必要がある際に呼ぶ

	bool TreeMode();//階層表示モードかどうかの判定

    void LinktoAllCardswithDesignatedLabel(TList *IDs);//現在カードから指定ラベルIDを持つカードすべてにラベルを貼る

    //Undo、Redo用
    unsigned int m_nLastModified;//最後にテキストを編集した時間（Undo用にバックアップする際使用）
    int m_nNextCardID, m_nNextSelStart, m_nNextSelLength;//Undo,Redoによるエディタのカーソル位置の移動
    int m_nLastSelLength;
    bool m_bDoNotBackup;//複合編集のため、細かい編集中のUndo用バックアップを禁止
    void BackupSub(AnsiString Action);
    void TextEditBackupSub(AnsiString Action, int CardID = -1, int SelStart = 0, int SelLength = 0);//テキスト編集前にUndo用バックアップを行う

    //保存用
    void ExportBMP(int operation, AnsiString Text);//ブラウザ画像を保存orコピー(0=BMP, 1=JPEG, 2=コピー）
	void ExportCardText(int operation, bool btitle, AnsiString Text);//カード本文を保存orコピー
	void ExportHierarchicalText(TStringList *SL, int CurrentLevel, AnsiString HChar, TCard *CurrentParent);//階層テキスト出力のためのサブルーチン。CurrentParentにぶら下がっているすべてのカードについて、その下の階層を出力させる。このルーチンは再帰的に呼ばれる
private:
    //描画用
    void CalcCardSize(TCanvas *C, TCard *Card, int Size, int cardindex);
    void DrawLabelCircleRect(TCanvas *C, bool drawtoporder, bool drawothers);
    void DrawFocusCursor(TCanvas *C, float fx, float fy, float fw, float fh);
    void DrawCard(TCanvas *C, TCard *Card, int Size, int cardindex, TColor HMColor, int option);//0x1で影描画,0x2で縁取り
    void DrawLink(TCanvas *C, TLink *Link, int card1index, int card2index, TColor HMColor, int option);//0x1で影描画,0x2で縁取り
    void DrawLink2(TCanvas *C, TLink *Link, int X1, int Y1, int X2, int Y2, TColor HMColor, int option);
	void DrawPatternLine(TCanvas *C, int Pattern, int X1, int Y1, int X2, int Y2, int penwidth);
    void DrawCurvedLine(TCanvas *C, int Pattern, int X1, int Y1, int X2, int Y2, int penwidth, int direction);//曲線描画
	void SetCardVisible(bool bFoldTree = true);//表示するカード決定
    void Redraw();//全体再描画
    void PaintSub(TCanvas *C);
    TColor GetCardColor(TCard *Card, float &SizeX);//ラベルによるカードの基本色を取得

    //overview座標
    int m_nOVWidth;
    int m_nOVXOffset;
    int m_nOVHeight;
    int m_nOVYOffset;

    //Focusを示すCursorの位置（0動き始め～100到達）
    int m_nFocusCursorPos;
    int m_nLastTarget;//直前のターゲットカード
private:
    //外部リンク
    void OpenExtLink(AnsiString S);
    TList *MI_ExtLink;
    void FreeMIExtLink();

public:
    //言語選択
    void __fastcall ChangeLanguageOnClick(TObject *Sender);
public:
    //Score
    float m_fMinScore;
    float m_fMaxScore;
    void IterScore();
    void IterScore_Authenticity();
    void IterScore_StartingPoint();
    void IterScore_Destination();
    void IterScore_Links_Out();
    void IterScore_Links_In();
    void IterScore_Links_Total();
    void IterScore_Links_InOut();
    void IterScore_TextLength();
public:
    //イメージリスト
	TList *m_ImageList;
	bool UpdateImageList();//帰り値は変更があったらtrue
	inline TCardImage *GetImage(int index);
	TCardImage *SearchImage(AnsiString FN);
public:
	//ビデオサムネイルリスト
	TList *m_VideoList;
	bool UpdateVideoList();//帰り値は変更があったらtrue
	inline TCardVideo *GetVideo(int index);
	TCardVideo *SearchVideo(AnsiString FN);
public:
	//背景アニメーション
	int m_BGAnimationBuf[BGANIMATIONOBJECTSIZE * BGANIMATIONMAXOBJECTS];//16int×256オブジェクト分
	float m_fBGAnimationSpeed;//何秒分アニメーションするか
	void BGAnimation(TCanvas *C);
	void BGAnimation_MovingVHLine(TCanvas *C);
	void BGAnimation_Bubble(TCanvas *C);
	void BGAnimation_Snow(TCanvas *C);
	void BGAnimation_CherryBlossom(TCanvas *C);
public:
	//描画
	TDrawing *m_Drawing;//現在描画中の絵
	TRect m_DrawingRect;//描画領域
	int m_DrawingTool;
	void ApplyDrawing();//絵が編集されていたらUndoを作成し、編集結果をカードのデータに反映
public:
    //統計
    float m_fStatisticsPos;//滑らかにグラフを立ち上げる係数（0.0～1.0）
	TList *m_StatisticsRectToCard;//範囲選択したときに見せるカードのリスト
    void ClearStatisticsRectToCard();
public:
    /*
    //読み上げ
	TAgent *m_Agent;
    IAgentCtlCharacter *m_AgentChar;
    TDirectSS *m_TTS;
    void LoadAgent();
    void UnloadAgent();
    */
public:
    //ファイルドロップ
    BEGIN_MESSAGE_MAP
      VCL_MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFiles)
      //VCL_MESSAGE_HANDLER(WM_ERASEBKGND, TWMEraseBkgnd, WMEraseBkgnd)
    END_MESSAGE_MAP(TForm);

    void __fastcall WMDropFiles(TWMDropFiles &mes);
    void __fastcall WMEraseBkgnd(TWMEraseBkgnd &msg);
public:
    //多言語対応
    void ApplyLanguageSetting();
public:
    //連続読み込み（編集中ファイルに更新があったらすぐ読み込み）
    bool m_bContinuousLoad;
    int m_nCLFileAge;//連続読み込みするファイルのタイムスタンプ
private:
    //デモ
    AnsiString m_DemoString;
    TStringList *m_DemoStrings;
    int m_nDemoIndex;
private:
    //整合性を取るため
    bool m_bSkipAutoZoom;
    bool m_bFileListDragging;//FileList使用中に消さないため
private:
    //一時
    void PaintSub_GUC(TCanvas *C);
};
//---------------------------------------------------------------------------
extern PACKAGE TFo_Main *Fo_Main;
//---------------------------------------------------------------------------
#endif

// ---------------------------------------------------------------------------

#ifndef fomainH
#define fomainH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
// ---------------------------------------------------------------------------
#include "document.h"
#include "foprogress.h"
#include "textproc.h"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Graphics.hpp>
#include <ImgList.hpp>
#include <IniFiles.hpp>
#include <Jpeg.hpp>
#include <Menus.hpp>
#include <XPMan.hpp>
// #include "dsdecoder.h"
#include "drawing.h"
#include <System.ImageList.hpp>
#include <Vcl.WinXCtrls.hpp>
// #include "C:\My Documents\RAD Studio\5.0\Imports\HTTSLib_OCX.h"
/*
 #include "ACTIVEVOICEPROJECTLib_OCX.h"
 #include "AgentObjects_OCX.h"
 #include "AgentObjects_TLB.h"
 #include <OleCtrls.hpp>
 #include <OleServer.hpp>
 */
// ---------------------------------------------------------------------------

#define MAXRANDOMCARDS 100

#define BGANIMATIONMAXOBJECTS 1024
#define BGANIMATIONOBJECTSIZE 16
// Ms of no edit before backup
#define UNDOBACKUPSPAN 2000

// ---------------------------------------------------------------------------
class TEdit2 : public TMemo {
public:
  __fastcall virtual TEdit2(Classes::TComponent *AOwner) : TMemo(AOwner){};

protected:
  virtual void __fastcall CreateParams(TCreateParams &Params);
};
// ---------------------------------------------------------------------------

class TCardImage {
public:
  TCardImage(UnicodeString FN); // Constructor. Loads image when created
  virtual ~TCardImage();

  UnicodeString m_FN; // Image file name
  bool m_bExist;      // Load succeeded
  bool m_bUsed;       // In use (from outside for update check)
  TGraphic *m_Image;  // Image

private:
  TCardImage(const TCardImage &source);
  TCardImage &operator=(const TCardImage &source);

private:
  Graphics::TBitmap *m_BMP; // BMP image
  TJPEGImage *m_Jpg;        // JPG image
};

// ---------------------------------------------------------------------------

class TCardVideo {
public:
  TCardVideo(UnicodeString FN); // Constructor. Loads when created
  virtual ~TCardVideo();

  UnicodeString m_FN; // File name
  bool m_bExist;      // Load succeeded
  bool m_bUsed;       // In use (from outside for update check)
  // TDSDecoder *m_Video;// Video
  void *m_Video;

private:
  TCardVideo(const TCardVideo &source);
  TCardVideo &operator=(const TCardVideo &source);
};

// ---------------------------------------------------------------------------

class TFo_Main : public TForm, IProgress {
__published: // IDE-managed components
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
  TPanel *Pa_Card;
  TPanel *Pa_Link;
  TPanel *Pa_Top;
  TPopupMenu *PM_GPT;
  TMenuItem *ME_GPT;
  TMenuItem *Setting1;
  TMenuItem *MS_GPTAPIKey;

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
  void __fastcall PB_BrowserMouseDown(TObject *Sender, TMouseButton Button,
                                      TShiftState Shift, int X, int Y);
  void __fastcall PB_BrowserDblClick(TObject *Sender);
  void __fastcall PB_BrowserMouseUp(TObject *Sender, TMouseButton Button,
                                    TShiftState Shift, int X, int Y);
  void __fastcall PB_BrowserMouseMove(TObject *Sender, TShiftState Shift, int X,
                                      int Y);
  void __fastcall Sc_YChange(TObject *Sender);
  void __fastcall Ed_SizeChange(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
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
  void __fastcall RE_EditKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall RE_EditMouseUp(TObject *Sender, TMouseButton Button,
                                 TShiftState Shift, int X, int Y);
  void __fastcall ME_SelectAllClick(TObject *Sender);
  void __fastcall MF_PrintImageClick(TObject *Sender);
  void __fastcall MF_PrintTextClick(TObject *Sender);
  void __fastcall MVO_ChangeBackgroundColorClick(TObject *Sender);
  void __fastcall MVO_ChangeFourgroundColorClick(TObject *Sender);
  void __fastcall MVL_LabelNameClick(TObject *Sender);
  void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
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
  void __fastcall RE_EditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall MVT_BrowserPosClick(TObject *Sender);
  void __fastcall MVT_AlwaysShowEditorClick(TObject *Sender);
  void __fastcall MVT_EditInBrowserClick(TObject *Sender);
  void __fastcall MV_EditInBrowserClick(TObject *Sender);
  void __fastcall MV_EditTitleClick(TObject *Sender);
  void __fastcall PBN_SelectClick(TObject *Sender);
  void __fastcall PBN_LinkClick(TObject *Sender);
  void __fastcall LB_FileListClick(TObject *Sender);
  void __fastcall LB_FileListDblClick(TObject *Sender);
  void __fastcall LB_FileListMouseDown(TObject *Sender, TMouseButton Button,
                                       TShiftState Shift, int X, int Y);
  void __fastcall LB_FileListMouseUp(TObject *Sender, TMouseButton Button,
                                     TShiftState Shift, int X, int Y);
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
  void __fastcall PB_GlobalSearchMouseDown(TObject *Sender, TMouseButton Button,
                                           TShiftState Shift, int X, int Y);
  void __fastcall Sc_GlobalSearchKeyDown(TObject *Sender, WORD &Key,
                                         TShiftState Shift);
  void __fastcall ME_FindPreviousClick(TObject *Sender);
  void __fastcall PE_CutToNewCardTitleWithLinkClick(TObject *Sender);
  void __fastcall MVO_NoScrollLagClick(TObject *Sender);
  void __fastcall MFI_HierarchicalTextFile2Click(TObject *Sender);
  void __fastcall TB_ZoomKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall Sp_BrowserRightMoved(TObject *Sender);
  void __fastcall Sp_BrowserBottomMoved(TObject *Sender);
  void __fastcall LB_LinkClick(TObject *Sender);
  void __fastcall RE_EditEnter(TObject *Sender);
  void __fastcall PB_StatisticsPaint(TObject *Sender);
  void __fastcall Sc_StatisticsYChange(TObject *Sender);
  void __fastcall MI_LinktoAllCardswithDesignatedLabelClick(TObject *Sender);
  void __fastcall MI_NewCardLinkstoAllCardswithDesignatedLabelClick(
      TObject *Sender);
  void __fastcall MI_AddDesignatedLabeltoAllDestinationCardsClick(
      TObject *Sender);
  void __fastcall SB_StatisticsSortClick(TObject *Sender);
  void __fastcall PB_StatisticsMouseDown(TObject *Sender, TMouseButton Button,
                                         TShiftState Shift, int X, int Y);
  void __fastcall Bu_StatisticsKeyClick(TObject *Sender);
  void __fastcall PM_StatisticKeyClick(TObject *Sender);
  void __fastcall PB_DrawingPaint(TObject *Sender);
  void __fastcall PB_DrawingMouseDown(TObject *Sender, TMouseButton Button,
                                      TShiftState Shift, int X, int Y);
  void __fastcall PB_DrawingMouseMove(TObject *Sender, TShiftState Shift, int X,
                                      int Y);
  void __fastcall PB_DrawingMouseUp(TObject *Sender, TMouseButton Button,
                                    TShiftState Shift, int X, int Y);
  void __fastcall SB_DrawingToolClick(TObject *Sender);
  void __fastcall Bu_DColorClick(TObject *Sender);
  void __fastcall Sh_DColorDefaultMouseDown(TObject *Sender,
                                            TMouseButton Button,
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
  void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift,
                                 int WheelDelta, TPoint &MousePos,
                                 bool &Handled);
  void __fastcall ME_GPTClick(TObject *Sender);
  void __fastcall MS_GPTAPIKeyClick(TObject *Sender);

private: // User declarations
public:  // User declarations
  __fastcall TFo_Main(TComponent *Owner);

  // Document
  TDocument *m_Document;
  // Undo buffer
  TUndoRedo *m_UndoRedo;
  // Display refresh
  int m_nRefreshListCount;
  int m_nRefreshLinkCount;
  int m_nRefreshLabelCount;
  int m_nTargetCard;  // Card being edited
  int m_nTargetLink;  // Link being edited
  int m_nTargetCard2; // Link destination card
  int m_nCurrentCard; // Card displayed (refresh if differs from TargetCard)
  int m_nCurrentLink; // Link displayed (refresh if differs from TargetLink)

  void RefreshLinks(); // Link display refresh

  TList *m_LinkIndexes; // Link index displayed

  // Label
  void RefreshLabel(); // Label refresh

  TButton *Bu_Label[MAXLABELS];
  TList *MI_Labels;
  TButton *Bu_LinkLabel[MAXLABELS];
  TList *MI_LinkLabels;

  void FreeMILabels();

  int m_nToolLabel;     // Label from label button
  int m_nToolLinkLabel; // Label from label button

  // Browser display
  int m_nBrowserWidth, m_nBrowserHeight;
  int m_nBGColor, m_nFGColor;
  float m_fFontZoom;
  int m_nXOffset, m_nYOffset; // Offset for printer display

  int m_nTmpCardsCount; // Temp var storage size (realloc in PaintSub if count
                        // changes)
  int m_nTmpLinksCount; // Temp var storage size (realloc in PaintSub if count
                        // changes)
  bool *m_CardVisible;
  bool *m_LinkVisible;
  UnicodeString *m_CardTitle; // Temp card title (Title changes when Fold)
  bool *m_CardRelated;
  int *m_CardAssign; // For Fold. Move as card of this index
  int *m_CardShape;  // Card shape (temp. Majority when Fold)
  // Card size should be Fold average!?
  float *m_CardX;    // Display coords (ignore screen size)
  float *m_CardY;    // Display coords (ignore screen size)
  int *m_CardWidth;  // Width (real coords, Pixel)
  int *m_CardHeight; // Height (real coords, Pixel)
  int m_nFontHeight;
  float m_fZoomSD;      // Zoom std dev. 0.21 default. Smaller=wider view
  float m_fTickerSpeed; // Ticker movement
  unsigned int m_nLastTimeOut;
  bool m_bRedrawRequested;

  // Frame rate calculation
  float m_fFPS;
  int m_nFPSCount;
  unsigned int m_nLastFPSTime;

  // Arrange
  TDMatrix *m_SimMatrix; // Similarity matrix per card

  void PrepareArrange();
  void RefreshSimMatrix();
  void ProgressFunc();      // For call from Progress
  void RefreshSimMatrix_(); // For call from Progress
  void FreeSimMatrix();
  void BrowserArrangeByNormalize(float ratio = 1.0f); // Normalize
  void BrowserArrangeByRepulsion(
      float ratio = 1.0f); // Update coords by repulsion only
  void BrowserArrangeByLink(float ratio = 1.0f);  // Update coords by link
  void BrowserArrangeByLabel(float ratio = 1.0f); // Update coords by label
  void BrowserArrangeByIndex(float ratio = 1.0f); // Update coords by card order
  void BrowserArrangeByDate(int dateindex,
                            float ratio = 1.0f); // Update coords by date
  void
  BrowserArrangeByFold(float ratio = 1.0f); // Update coords by folded label
  void
  BrowserArrangeBySimilarity(float ratio = 1.0f); // Update coords by similarity
  void BrowserArrangeByMatrix(int type, float ratio = 1.0f); // Arrange in grid
  void BrowserArrangeByTree(int type, float ratio = 1.0f);   // Hierarchy view

  // Arrange (grid layout)
  int m_nMatrixWidth;  // Width for grid layout
  int m_nMatrixHeight; // Height for grid layout

  void PrepareMatrixArrange(
      int type); // Set grid size, place all cards. type=ArrangeType
  void PrepareMatrixArrange_AssignToMatrix(
      int i, int *tmatrix, float minx, float maxx, float miny,
      float maxy); // Store i-th card in matrix by tmatrix
  void PrepareMatrixArrange_AssignToMatrix2(int i, int *tmatrix, float minx,
                                            float maxx, float miny, float maxy);
  // Store i-th card in matrix by tmatrix (from circle)

  // AutoScroll
  float m_fBrowserScrollRatio; // Scroll amount
  float m_fBrowserScrollLastD; // Distance to last target
  int m_nScrollTargetX;        // Scroll request from Overview
  int m_nScrollTargetY;

  bool BrowserAutoScroll(); // Move so current card centers. Returns if coords
                            // updated

  // Animation
  TDocument *m_DocBeforeAnimation; // Backup document before animation
  int m_nAnimation;
  // Whether animating. 0=none, 1=RandomCard, 2=RandomCard2, 3=RandomTrace
  int m_nAnimationCount; // Animation progress (usage varies by animation)
  // Various backups during animation
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

  // Title input
  TEdit2 *Ed_TitleB;

  void SetEdTitleBPos();
  void CloseEditBox();

  // Edit body in Browser
  void SetTextEditPos();
  void CloseTextEditBox();

private:
  // Browser display area
  void BrowserArrange_Initialize(float *CardX, float *CardY, bool *Norm);
  void BrowserArrange_Link(int i, TCard *Card, float *CardX, float *CardY,
                           bool *Norm, float ratio); // Toward linked cards
  void BrowserArrange_Repulsion(int i, TCard *Card, float *CardX, float *CardY,
                                bool *Norm,
                                float ratio); // Away from other cards
  void BrowserArrange_LabelPrepare();         // Calc center coords per label
  void BrowserArrange_Label(int i, TCard *Card, float *CardX, float *CardY,
                            bool *Norm,
                            float ratio); // Near same label, far from different
  void BrowserArrange_Index(int i, TCard *Card, float *CardX, float *CardY,
                            bool *Norm, float ratio); // Closer by index
  void BrowserArrange_Date(int i, TCard *Card, float *CardX, float *CardY,
                           bool *Norm, float ratio,
                           int dateindex); // Closer by date
  void BrowserArrange_Similarity(int i, TCard *Card, float *CardX, float *CardY,
                                 bool *Norm, float ratio,
                                 int *idxtable); // Toward similar cards
  void BrowserArrange_Memory(float *CardX, float *CardY, bool *Norm);
  void BrowserArrange_Normalize(float *CardX, float *CardY, bool *Norm);

public:
  // File
  TIniFile *Ini;

  TMenuItem *MI_RecentFiles[10];
  TMenuItem *MI_RecentFolders[10];

  void LoadSub(UnicodeString FN, bool bSoftLoad = false,
               bool bRefreshRecent = true);
  bool Save();      // Save (overwrite or save as)
  bool SaveAs();    // Save as
  bool SaveCheck(); // File changed. Save?
  void RefreshRecent(UnicodeString FN);

  // Operations
  int m_bMDownBrowser; // 1=Normal drag, 2=Link Dest edit, 3=Link From edit
  int m_bDblClicked;
  bool m_bTitleEditRequested;
  bool m_bTextEditRequested;
  unsigned int m_uMDownBrowserLast;
  bool m_bMDownBrowserMoved; // Moved after mouse down
  int m_nMDownBrowserOffsetX;
  int m_nMDownBrowserOffsetY;
  int m_nMDownBrowserX; // Coords when mouse pressed
  int m_nMDownBrowserY;
  int m_nMDownTargetX; // Link line end
  int m_nMDownTargetY;
  int m_nMDownBrowserScrollX;
  int m_nMDownBrowserScrollY;
  bool m_bShowRecent; // Highlight recent cards (Space)

  bool CheckTreeButton(int index, int X, int Y);

private:
  // Search
  bool m_bSearching;

  void Search(int SearchRequest);
  // 0x1 title, 0x2 body, 0x4 from Browser, 0x8 from dialog
  // 0x10 case insensitive

  TList *m_GlobalSearchResult; // Global search result (hit card IDs)
  int m_GlobalSearchItemHeight;
  WideString m_GlobalSearchKeyword;
  int m_GlobalSearchOption;
  int m_GlobalSearchCursorIndex;

  void GlobalSearch(int SearchRequest); // Put hits in m_GlobalSearchResult
  int ReplaceAll(int SearchRequest);    // Replace all. Returns count

  TList *MI_WebSearch;
  TList *MI_GPT;

  WideString GetSelText(); // Get selected text
  void __fastcall ME_WebSearchClick(TObject *Sender);
  // Utils
  void DispPosToCardPos(float dx, float dy, float &cx, float &cy);
  void CardPosToDispPos(float cx, float cy, float &dx, float &dy);
  void FilteringCard();   // Filter cards by Label
  void RefreshLaStatus(); // Status label refresh
  void RefreshWallPaper();
  void RefreshFileList();

  void MoveToSelectedAndRecentCard(); // Move to most recent of selected
  float MoveDistance(float sx, float sy, float dx, float dy,
                     int direction); // Distance s to d when moving in direction
  void LinkLimitationSub(int targetcard, int targetlink, int linkdepth,
                         int *Score, bool linkdirection, bool linkbackward);

  void BrowserNewCard();
  void BrowserNewChildCard(bool binverse);
  void BrowserNewBrotherCard();
  void BrowserInsertCardToLink();

  void SetFont(); // Apply SettingView.m_Font name, Charset

  void AddTxtsInFolder(UnicodeString TopDir, TCard *Parent);

  void ExitFullScreen();

  void SetCardAssign(); // Call when Fold needed outside Paint

  bool TreeMode(); // Check if hierarchy view mode

  void LinktoAllCardswithDesignatedLabel(TList *IDs);
  // Apply label to all with designated ID from current

  // For Undo, Redo
  unsigned int m_nLastModified; // Last text edit time (for Undo backup)
  int m_nNextCardID, m_nNextSelStart, m_nNextSelLength;
  // Cursor move by Undo/Redo
  int m_nLastSelLength;
  bool m_bDoNotBackup; // Disable Undo backup during compound edit

  void BackupSub(UnicodeString Action);
  void TextEditBackupSub(UnicodeString Action, int CardID = -1,
                         int SelStart = 0,
                         int SelLength = 0); // Undo backup before text edit

  // For save
  void ExportBMP(int operation, UnicodeString Text);
  // Save or copy browser image (0=BMP, 1=JPEG, 2=copy)
  void ExportCardText(int operation, bool btitle, UnicodeString Text);
  // Save or copy card body
  void ExportHierarchicalText(TStringList *SL, int CurrentLevel,
                              UnicodeString HChar, TCard *CurrentParent);
  // Sub for hierarchy text. Recursively outputs children of CurrentParent
  // For drawing
  void CalcCardSize(TCanvas *C, TCard *Card, int Size, int cardindex);
  void DrawLabelCircleRect(TCanvas *C, bool drawtoporder, bool drawothers);
  void DrawFocusCursor(TCanvas *C, float fx, float fy, float fw, float fh);
  void DrawCard(TCanvas *C, TCard *Card, int Size, int cardindex,
                TColor HMColor, int option); // 0x1 shadow, 0x2 outline
  void DrawLink(TCanvas *C, TLink *Link, int card1index, int card2index,
                TColor HMColor, int option); // 0x1 shadow, 0x2 outline
  void DrawLink2(TCanvas *C, TLink *Link, int X1, int Y1, int X2, int Y2,
                 TColor HMColor, int option);
  void DrawPatternLine(TCanvas *C, int Pattern, int X1, int Y1, int X2, int Y2,
                       int penwidth);
  void DrawCurvedLine(TCanvas *C, int Pattern, int X1, int Y1, int X2, int Y2,
                      int penwidth, int direction); // Curve drawing
  void SetCardVisible(bool bFoldTree = true); // Determine cards to display
  void Redraw();                              // Full redraw
  void PaintSub(TCanvas *C);
  TColor GetCardColor(TCard *Card,
                      float &SizeX); // Get card base color by label

  // Overview coordinates
  int m_nOVWidth;
  int m_nOVXOffset;
  int m_nOVHeight;
  int m_nOVYOffset;

  // Focus cursor (0 start~100 reach)
  int m_nFocusCursorPos;
  int m_nLastTarget; // Previous target card

  // External links
  void OpenExtLink(UnicodeString S);

  TList *MI_ExtLink;

  void FreeMIExtLink();

public:
  // Language selection
  void __fastcall ChangeLanguageOnClick(TObject *Sender);

  // Score
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

  // Image list
  TList *m_ImageList;

  bool UpdateImageList(); // Returns true if changed
  inline TCardImage *GetImage(int index);
  TCardImage *SearchImage(UnicodeString FN);

  // Video thumbnail list
  TList *m_VideoList;

  bool UpdateVideoList(); // Returns true if changed
  inline TCardVideo *GetVideo(int index);
  TCardVideo *SearchVideo(UnicodeString FN);

  // Background animation
  int m_BGAnimationBuf[BGANIMATIONOBJECTSIZE * BGANIMATIONMAXOBJECTS];
  // 16 int x 256 objects
  float m_fBGAnimationSpeed; // Animation duration in seconds

  void BGAnimation(TCanvas *C);
  void BGAnimation_MovingVHLine(TCanvas *C);
  void BGAnimation_Bubble(TCanvas *C);
  void BGAnimation_Snow(TCanvas *C);
  void BGAnimation_CherryBlossom(TCanvas *C);

  // Drawing
  TDrawing *m_Drawing; // Drawing in progress
  TRect m_DrawingRect; // Drawing area
  int m_DrawingTool;

  void ApplyDrawing(); // Create Undo if edited, apply to card

  // Statistics
  float m_fStatisticsPos;        // Smooth graph rise factor (0.0~1.0)
  TList *m_StatisticsRectToCard; // Cards shown on range selection

  void ClearStatisticsRectToCard();
  /*
  // Text-to-speech
  TAgent *m_Agent;
  IAgentCtlCharacter *m_AgentChar;
  TDirectSS *m_TTS;
  void LoadAgent();
  void UnloadAgent();
  */
  // File drop
  BEGIN_MESSAGE_MAP VCL_MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFiles)
      // VCL_MESSAGE_HANDLER(WM_ERASEBKGND, TWMEraseBkgnd, WMEraseBkgnd)
      END_MESSAGE_MAP(TForm);

  void __fastcall WMDropFiles(TWMDropFiles &mes);
  void __fastcall WMEraseBkgnd(TWMEraseBkgnd &msg);
  // Multi-language
  void ApplyLanguageSetting();

  // Continuous load (reload when file updated)
  bool m_bContinuousLoad;
  int m_nCLFileAge; // Timestamp for continuous load

private:
  // Demo
  UnicodeString m_DemoString;
  TStringList *m_DemoStrings;
  int m_nDemoIndex;
  // For consistency
  bool m_bSkipAutoZoom;
  bool m_bFileListDragging; // Don't delete while FileList in use
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_Main *Fo_Main;
// ---------------------------------------------------------------------------
#endif

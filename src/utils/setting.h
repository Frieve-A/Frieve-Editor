// ---------------------------------------------------------------------------

#ifndef settingH
#define settingH
// ---------------------------------------------------------------------------
#include "document.h"
#include "utils.h"
#include <IniFiles.hpp>
#include <vcl.h>
// ---------------------------------------------------------------------------

class TSettingView {
public:
  // Constructor
  TSettingView();
  virtual ~TSettingView();
  // Read/write
  void WriteToIni(TIniFile *Ini, UnicodeString Section);
  void ReadFromIni(TIniFile *Ini, UnicodeString Section);
  // Members

  // Font
  TFont *m_Font;

  // Link
  int m_nLinkDefaultShape;
  bool m_bLinkShadow;
  bool m_bLinkHemming;
  bool m_bLinkVisible;
  bool m_bLinkDirectionVisible;
  bool m_bLinkNameVisible;

  // Label
  bool m_bLabelCircleVisible;
  bool m_bLabelRectangleVisible;
  bool m_bLavelCRFocusedOnTop; // Focused label on top (circle/rect overlap)
  bool m_bLabelFill;
  bool m_bLabelNameVisible;

  // Card
  bool m_bFocusCursor;
  bool m_bFocusCursorAnimation;
  int m_nCardDefaultShape;
  int m_nCardDefaultSize;
  bool m_bCardDefaultFixed;
  bool m_bTickerVisible; // Show Ticker Marquee below card
  int m_nTickerLines;
  bool m_bCardShadow;
  bool m_bCardHemming;
  bool m_bCardGradation;
  bool m_bImageVisible;
  bool m_bVideoVisible;
  bool m_bDrawingVisible;
  int m_nImageLimitation;
  bool m_bMagnifyFocused; // Magnify focused card

  // Text
  bool m_bTextVisible;
  bool m_bTextCentering;
  bool m_bTextWordWrap;

  bool m_bEditInBrowser;       // Edit body in browser
  bool m_bEditInBrowserAlways; // Always edit body in browser
  int m_nEditInBrowserPos; // Edit position: 0=below card, 1=right of browser,
                           // 2=below browser
  int m_nEditorWidthInBrowser;  // Editor width when editing in browser
  int m_nEditorHeightInBrowser; // Editor height when editing in browser

  // Size Limitation
  int m_nSizeLimitation;  // Max size of cards to display (100 = show all)
  bool m_bSizeLimitation; // Size limitation on/off

  // Link Limitation
  int m_bLinkLimitation; // Link limitation ON/OFF
  int m_nLinkLimitation; // Max links to display (0 = show all)
  bool m_bLinkDirection; // Link direction limitation
  bool m_bLinkBackward;  // Backward link limitation
  int m_nLinkTarget;     // Link target card ID

  // Date Limitation
  bool m_bDateLimitation;   // Display limit by date ON/OFF
  int m_nDateLimitation;    // Date limit magnitude (see DateLimitationType)
  int m_DateLimitationType; // Date limit type (Old, New, Near, Older, Newer)
  int m_DateLimitationDateType; // Date type (Created, Edited, Viewed)

  // CardVisible
  bool m_bNoLabelVisible;
  bool m_bNoLinkLabelVisible;

  // Animation
  bool m_bAnimationPaused; // Paused

  int m_nAnimationRCCards; // Number of cards in RandomCard
  int m_nAnimationRCSpeed; // RandomCard animation speed

  // Background animation
  bool m_bBGAnimation;
  int m_nBGAnimationType;
  bool m_bNoScrollLag;

  // Score
  bool m_bScore;   // Card score
  int m_ScoreType; // 0=Authenticity, 1=Starting Point, 2=Destination

  // Read
  bool m_bRead;                  // Read file
  int m_nReadSpeed;              // Read speed
  UnicodeString m_ReadAgentFile; // Agent file

  // Others
  bool m_bCardList;
  bool m_bFileList;

  int m_nFourgroundColor;
  int m_nBackgroundColor;

  bool m_bAntiAlias;
  int m_nAntiAliasX;

  UnicodeString m_WallPaper;
  bool m_bFixWallPaper;
  bool m_bTileWallPaper;

  bool m_bOverview; // Overview

  // Language
  UnicodeString m_Language;

  // Layout
  int m_nFileListWidth;
  int m_nCardListWidth;
  int m_nLinkListHeight;
  bool m_bEditorRelatedVisible;
  int m_nEditorRelatedWidth;
  int m_nGlobalSearchWidth;

  // Status bar
  bool m_bStatusBar;

  // Performance
  bool m_bLoadImageatOnce;    // Load all images at once
  bool m_bRedrawInBackground; // Redraw in background thread

  // Coordinates
  int m_nSpecialPaint;
  UnicodeString m_SpecialCaption;

private:
  TSettingView(const TSettingView &source);
  TSettingView &operator=(const TSettingView &source);
};

// ---------------------------------------------------------------------------
class TSettingFile {
public:
  // Constructor
  TSettingFile();
  virtual ~TSettingFile();
  // Read/write
  void WriteToIni(TIniFile *Ini, UnicodeString Section);
  void ReadFromIni(TIniFile *Ini, UnicodeString Section);
  // Members

  bool m_bCheckNew;

  UnicodeString m_RecentFiles[10];
  UnicodeString m_RecentFolders[10];

  int m_nUndoTimes;

  UnicodeString m_WebSearch; // WebSearch key

  // Plugin
  UnicodeString m_PlugIn;
  HINSTANCE m_hPlugIn; // Plugin handle (LoadLibrary)

  // GPT
  UnicodeString m_GPTAPIKey;

  void __stdcall (*fepLoadPlugIn)(void);
  void __stdcall (*fepTimeOut)(IDocCtrl *Doc);
  bool __stdcall (*fepOperation)(IDocCtrl *Doc, wchar_t *operation);
  void __stdcall (*fepUnloadPlugIn)(void);

private:
  TSettingFile(const TSettingFile &source);
  TSettingFile &operator=(const TSettingFile &source);
};

// ---------------------------------------------------------------------------
class TSetting2Function {
public:
  // Constructor
  TSetting2Function();
  virtual ~TSetting2Function();
  // Read/write
  void WriteToIni(TIniFile *Ini);
  void ReadFromIni(TIniFile *Ini);
  // Members
  TStringList *m_WebSearch;        // Web search settings
  TStringList *m_GPT;              // GPT settings
  UnicodeString m_GPTModel;        // GPT model
  UnicodeString m_GPTSystemPrompt; // GPT system prompt
private:
  TSetting2Function(const TSetting2Function &source);
  TSetting2Function &operator=(const TSetting2Function &source);
};

// ---------------------------------------------------------------------------
extern TSettingView SettingView;
extern TSettingFile SettingFile;
extern TSetting2Function Setting2Function;

class TMLText {
public:
  TMLText();

  UnicodeString Cards;   // Cards
  UnicodeString Links;   // Links
  UnicodeString Labels;  // Labels
  UnicodeString Line;    // Line
  UnicodeString Len;     // Characters
  UnicodeString Size;    // Size
  UnicodeString Title;   // Title
  UnicodeString Created; // Created
  UnicodeString Edited;  // Edited
  UnicodeString Viewed;  // Viewed
  UnicodeString Hits;    // Hits

  UnicodeString EditCardTitle;
  UnicodeString EditCardText;
  UnicodeString EditLinkTitle;
  UnicodeString EditDrawing;

  UnicodeString ChangeSize;      // Change size
  UnicodeString FixCardPosition; // Fix card position
  UnicodeString SetAsRootCard;   // Set as root card

  UnicodeString EditLink; // Edit link

  UnicodeString AddLabel;    // Add label
  UnicodeString DeleteLabel; // Delete label
  UnicodeString ChangeLabel; // Change label
  UnicodeString LabelName;
  UnicodeString SelectLinkDestination;
  UnicodeString SelectLabelOfDestination;
  UnicodeString SelectLabel;

  UnicodeString Color;

  UnicodeString ArrangeCards;

  UnicodeString ProcessingTextAnalysis;

  UnicodeString Enable;           // Enable
  UnicodeString Fold;             // Fold
  UnicodeString Show;             // Show
  UnicodeString Hide;             // Hide
  UnicodeString NoAssign;         // No assign
  UnicodeString NoLabel;          // No label
  UnicodeString SelectTargetCard; // Select link target card
  UnicodeString Old;              // Old order
  UnicodeString New;              // New order
  UnicodeString Near;             // Near order
  UnicodeString Older;            // Older
  UnicodeString Newer;            // Newer
  UnicodeString SaveCheck;        // Document changed. Save?
  UnicodeString FailedToSave;
  UnicodeString NotFound;
  UnicodeString AANotEnoughMemory;

private:
  TMLText(const TMLText &source);
  TMLText &operator=(const TMLText &source);
};

extern TMLText MLText;

UnicodeString LanguageFileName();
// ---------------------------------------------------------------------------
#endif

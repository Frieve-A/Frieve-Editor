// ---------------------------------------------------------------------------

#ifndef fosearchH
#define fosearchH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>

// ---------------------------------------------------------------------------
class TFo_Search : public TForm {
__published: // IDE-managed components
	TLabel *La_Keyword;
	TButton *Bu_FindNext;
	TButton *Bu_Cancel;
	TCheckBox *Ch_CardTitle;
	TCheckBox *Ch_Text;
	TPopupMenu *Po_Search;
	TMenuItem *PM_Cut;
	TMenuItem *PM_Copy;
	TMenuItem *PM_Paste;
	TMenuItem *PM_Undo;
	TMenuItem *PM_SelectAll;
	TCheckBox *Ch_MatchCase;
	TCheckBox *Ch_GlobalSearch;
	TCheckBox *Ch_Backward;
	TComboBox *Co_Keyword;
	TEdit *Ed_Keyword;
	TCheckBox *Ch_InvisibleCard;
	TLabel *La_ReplaceTo;
	TEdit *Ed_ReplaceTo;

	void __fastcall Bu_CancelClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Ed_KeywordChange(TObject *Sender);
	void __fastcall Bu_FindNextClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall PM_UndoClick(TObject *Sender);
	void __fastcall PM_CutClick(TObject *Sender);
	void __fastcall PM_CopyClick(TObject *Sender);
	void __fastcall PM_PasteClick(TObject *Sender);
	void __fastcall PM_SelectAllClick(TObject *Sender);
	void __fastcall Co_KeywordChange(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

private: // User declarations
public: // User declarations
	__fastcall TFo_Search(TComponent* Owner);
	// Multi-language support
	void ApplyLanguageSetting();

	int bReplace; // Replace mode
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_Search *Fo_Search;
extern WideString SearchKeyword; // Search keyword
extern WideString ReplaceKeyword; // Replace keyword
extern int bSearchRequest;
// Search request: 0x1=CardTitle, 0x2=Text, 0x8=from search dialog, 0x4=from browser Find button, 0x40=Global Search, 0x80=backward, 0x100=replace
extern int bLastSearchRequest;
// Search request: 0x1=CardTitle, 0x2=Text, 0x8=from search dialog, 0x4=from browser Find button, 0x40=Global Search
// ---------------------------------------------------------------------------
#endif

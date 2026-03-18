// ---------------------------------------------------------------------------

#ifndef foshapeH
#define foshapeH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

// ---------------------------------------------------------------------------
class TFo_Shape : public TForm {
__published: // IDE-managed components
	TPanel *Pa;
	TPaintBox *PB;

	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall PBPaint(TObject *Sender);
	void __fastcall PBMouseMove(TObject *Sender, TShiftState Shift,
		int X, int Y);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall PBMouseDown(TObject *Sender, TMouseButton Button,
		TShiftState Shift, int X, int Y);

private: // User declarations
public: // User declarations
	__fastcall TFo_Shape(TComponent* Owner);

	TImageList *IL_Shape;
	int m_nMode; // 0-3: card, link, all cards, all links
	int m_nItemIndex;
	bool m_bIgnoreChange; // Prevent index change at display moment
	bool m_bSelected;
};

// ---------------------------------------------------------------------------
extern PACKAGE TFo_Shape *Fo_Shape;
// ---------------------------------------------------------------------------
#endif

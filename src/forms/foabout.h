// ----------------------------------------------------------------------------
#ifndef foaboutH
#define foaboutH
// ----------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

// ----------------------------------------------------------------------------
class TFo_About : public TForm {
__published:
	TButton *Bu_OK;
	TImage *ProgramIcon;
	TLabel *ProductName;
	TLabel *Copyright;
	TLabel *Comments;
	TLabel *Label1;

	void __fastcall FormShow(TObject *Sender);

private:
public:
	virtual __fastcall TFo_About(TComponent* AOwner);
	// ‘½Œ¾Œê‘Î‰ž
	void ApplyLanguageSetting();
};

// ----------------------------------------------------------------------------
extern PACKAGE TFo_About *Fo_About;
// ----------------------------------------------------------------------------
#endif

//----------------------------------------------------------------------------
#ifndef foaboutH
#define foaboutH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
//----------------------------------------------------------------------------
class TFo_About : public TForm
{
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
public:
    //‘½Œ¾Œê‘Î‰ž
    void ApplyLanguageSetting();
};
//----------------------------------------------------------------------------
extern PACKAGE TFo_About *Fo_About;
//----------------------------------------------------------------------------
#endif    

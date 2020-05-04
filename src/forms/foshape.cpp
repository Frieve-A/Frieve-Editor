//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "foshape.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_Shape *Fo_Shape;

const int itemwidth = 41;
const int itemheight = 21;
//---------------------------------------------------------------------------
__fastcall TFo_Shape::TFo_Shape(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFo_Shape::FormDeactivate(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFo_Shape::FormShow(TObject *Sender)
{
    Pa->Width = itemwidth + 2;
    Pa->Height = itemheight * IL_Shape->Count + 2;
    ClientWidth = Pa->Width;
    ClientHeight = Pa->Height;
}
//---------------------------------------------------------------------------
void __fastcall TFo_Shape::PBPaint(TObject *Sender)
{
    Graphics::TBitmap *BMP = new Graphics::TBitmap();
    BMP->Width = PB->Width;
    BMP->Height = PB->Height;

    TCanvas *C = BMP->Canvas;
    TBrush *B = C->Brush;
    TPen *P = C->Pen;

    B->Color = clBtnFace;
    C->FillRect(Rect(0, 0, BMP->Width, BMP->Height));

    Graphics::TBitmap *BMP2 = new Graphics::TBitmap();
    BMP2->Width = IL_Shape->Width;
    BMP2->Height = IL_Shape->Height;
    TCanvas *C2 = BMP2->Canvas;
    TBrush *B2 = C2->Brush;
    B2->Color = clBtnFace;

    for (int i = 0 ; i < IL_Shape->Count ; i++){
        C2->FillRect(Rect(0, 0, BMP2->Width, BMP2->Height));
        IL_Shape->GetBitmap(i, BMP2);
        C->Draw(
            (itemwidth - BMP2->Width) / 2,
            (itemheight - BMP2->Height) / 2 + i * itemheight,
            BMP2);

        if (i == m_nItemIndex){
            B->Style = bsClear;
            P->Color = TColor(0xff0000);
            C->Rectangle(0, i * itemheight, BMP->Width, (i + 1) * itemheight);
        }
    }

    delete BMP2;


    PB->Canvas->Draw(0, 0, BMP);
    delete BMP;
}
//---------------------------------------------------------------------------
void __fastcall TFo_Shape::PBMouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
    int newindex = Y / itemheight;
    if (newindex != m_nItemIndex && !m_bIgnoreChange){
        m_nItemIndex = newindex;
        PBPaint(Sender);
    }else{
        m_bIgnoreChange = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFo_Shape::FormCreate(TObject *Sender)
{
    m_bSelected = false;
}
//---------------------------------------------------------------------------

void __fastcall TFo_Shape::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key == 27){
        Close();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFo_Shape::PBMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    m_bSelected = true;
    Close();
}
//---------------------------------------------------------------------------


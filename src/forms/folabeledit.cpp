//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "folabeledit.h"
#include "foedittext.h"
#include "utils.h"
#include "setting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_LabelEdit *Fo_LabelEdit;
//---------------------------------------------------------------------------
void TFo_LabelEdit::ApplyLanguageSetting()
{
	if (!FileExists(LanguageFileName())){
		return;
	}
	TFastIni *Ini = new TFastIni(LanguageFileName());

	Bu_OK->Caption = Ini->ReadString("Dialog", "Bu_OK", Bu_OK->Caption);
	Bu_New->Caption = Ini->ReadString("LabelEdit", "Bu_New", Bu_New->Caption);
	Bu_Delete->Caption = Ini->ReadString("LabelEdit", "Bu_Delete", Bu_Delete->Caption);

	delete Ini;
}
//---------------------------------------------------------------------
__fastcall TFo_LabelEdit::TFo_LabelEdit(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFo_LabelEdit::FormShow(TObject *Sender)
{
	ApplyLanguageSetting();

	Refresh();
    Ti_Check->Enabled = true;
}
//---------------------------------------------------------------------------
void TFo_LabelEdit::Refresh()
{
    SG_Labels->RowCount = m_Document->m_Labels[m_LType]->Count + 1;
    /*
    if (SG_Labels->RowCount > 1){
        SG_Labels->FixedRows = 1;
	}
    */
    SG_Labels->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TFo_LabelEdit::SG_LabelsDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
    TCanvas *Canvas = SG_Labels->Canvas;

    //背景
    Canvas->Brush->Style = bsSolid;
    Canvas->Brush->Color = clWhite;
    if (ARow == 0){
        Canvas->Brush->Color = clBtnFace;
	}else{
    }
    Canvas->FillRect(Rect);

    if (ARow > 0 && ACol == 1){
        //Color
        Canvas->Brush->Color = (TColor)m_Document->GetLabelByIndex(m_LType, ARow - 1)->m_nColor;
        Canvas->FillRect(::Rect(Rect.Left + 2, Rect.Top + 2, Rect.Right - 2, Rect.Bottom - 2));
	}
    
    //枠
    Canvas->Brush->Style = bsClear;
    if (State.Contains(gdFocused) && ARow > 0){
        Canvas->Pen->Color = clBlue;
        Canvas->Rectangle(Rect);
    }

    //字
    WideString S;
    if (ARow == 0){
        //タイトル行
		if (ACol == 0){
			S = MLText.LabelName;
		}else{
            S = MLText.Color;
        }
    }else{
        //データ行
        if (ACol == 0){
			S = m_Document->GetLabelByIndex(m_LType, ARow - 1)->m_Name;
        }else{
            S = "";
        }
    }

	while (Canvas->TextWidth(S)>Rect.Width()-4){
		S = S.SubString(1, S.Length()-1);
    }
    Canvas->Font->Color = clBlack;
	Canvas->TextOut(Rect.left+2, Rect.top+2, S);
}
//---------------------------------------------------------------------------

void __fastcall TFo_LabelEdit::SG_LabelsSelectCell(TObject *Sender,
	  int ACol, int ARow, bool &CanSelect)
{
    if (ARow == 0){
        //タイトル行
        return;
    }

    m_nLastCol = ACol;
    m_nLastRow = ARow;
}
//---------------------------------------------------------------------------


void __fastcall TFo_LabelEdit::FormCreate(TObject *Sender)
{
    m_bEdited = false;
    m_nLastCol = 0;
    m_nLastRow = 0;
}
//---------------------------------------------------------------------------


void __fastcall TFo_LabelEdit::SG_LabelsDblClick(TObject *Sender)
{
    int ACol = m_nLastCol;
    int ARow = m_nLastRow;
    if (ARow == 0){
        return;
	}
	if (ACol == 0){
		//ラベル名
		Fo_EditText = new TFo_EditText(this);
		Fo_EditText->La_Text->Caption = MLText.LabelName + ":";
		Fo_EditText->Ed_Text->Text = m_Document->GetLabelByIndex(m_LType, ARow - 1)->m_Name;
		Fo_EditText->Caption = Caption;
		Fo_EditText->ShowModal();
		if (Fo_EditText->ModalResult == mrOk && Fo_EditText->Ed_Text->Text != ""){
			BackupSub();
			TCardLabel *Label = m_Document->GetLabelByIndex(m_LType, ARow - 1);
			Label->m_Name = Fo_EditText->Ed_Text->Text;
			m_Document->RefreshLabel();
		}
		Fo_EditText->Release();
	}else{
		//ラベル色
		CD->Color = (TColor)m_Document->GetLabelByIndex(m_LType, ARow - 1)->m_nColor;
		if (CD->Execute()){
			BackupSub();
			m_Document->GetLabelByIndex(m_LType, ARow - 1)->m_nColor = (int)CD->Color;
			m_Document->RefreshLabel();
		}
	}

	m_nLastCol = 0;
	m_nLastRow = 0;
}
//---------------------------------------------------------------------------

void __fastcall TFo_LabelEdit::Bu_DeleteClick(TObject *Sender)
{
	int ARow = SG_Labels->Selection.Top;
	if (ARow > 0 && ARow == m_nLastRow){
		BackupSub();
		m_Document->DeleteLabelByIndex(m_LType, ARow - 1);
		Refresh();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_LabelEdit::Bu_NewClick(TObject *Sender)
{
	Fo_EditText = new TFo_EditText(this);
	Fo_EditText->La_Text->Caption = MLText.LabelName + ":";
	Fo_EditText->Caption = DeleteActionKey(Bu_New->Caption);
	Fo_EditText->ShowModal();
	if (Fo_EditText->ModalResult == mrOk && Fo_EditText->Ed_Text->Text != ""){
		BackupSub();
		TCardLabel *Label = m_Document->NewLabel(m_LType);
		Label->m_Name = Fo_EditText->Ed_Text->Text;

		Label->m_nColor = GetColor(rand() % 10001 / 10000.0, 0, 255);
		Refresh();
	}
	Fo_EditText->Release();
}
//---------------------------------------------------------------------------


void __fastcall TFo_LabelEdit::FormClose(TObject *Sender,
	  TCloseAction &Action)
{
	Ti_Check->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TFo_LabelEdit::Ti_CheckTimer(TObject *Sender)
{
    if (Bu_Delete->Enabled != SG_Labels->Selection.Top > 0){
        Bu_Delete->Enabled = SG_Labels->Selection.Top > 0;
    }
}
//---------------------------------------------------------------------------
void TFo_LabelEdit::BackupSub()
{
    if (!m_bEdited){
        m_UndoRedo->Backup(m_Document, DeleteActionKey(Caption).c_str());
    }
    m_bEdited = true;
}
//---------------------------------------------------------------------------


void __fastcall TFo_LabelEdit::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (Key == 27) {
        Close();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <math.h>

#include "foview.h"
#include "setting.h"
#include "utils.h"
#include "foselectcard.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_View *Fo_View;
//---------------------------------------------------------------------------
void TFo_View::ApplyLanguageSetting()
{
	if (!FileExists(LanguageFileName())){
		return;
	}
	TFastIni *Ini = new TFastIni(LanguageFileName());

	Caption = Ini->ReadString("View", "Caption", Caption);
	TS_Label->Caption = Ini->ReadString("View", "TS_Label", TS_Label->Caption);//ラベル
	La_Label->Caption = Ini->ReadString("View", "La_Label", La_Label->Caption);//ラベル:
	La_LabelFold->Caption = Ini->ReadString("View", "La_Fold", La_LabelFold->Caption);//折畳:
	La_LabelShow->Caption = Ini->ReadString("View", "La_Show", La_LabelShow->Caption);//表示:
	La_LabelHide->Caption = Ini->ReadString("View", "La_Hide", La_LabelHide->Caption);//隠蔽:
	La_LinkLabel->Caption = Ini->ReadString("View", "La_LinkLabel", La_LinkLabel->Caption);//リンクラベル:
	La_LinkLabelShow->Caption = Ini->ReadString("View", "La_Show", La_LinkLabelShow->Caption);//表示:
	La_LinkLabelHide->Caption = Ini->ReadString("View", "La_Hide", La_LinkLabelHide->Caption);//隠蔽:
	TS_Limitation->Caption = Ini->ReadString("View", "TS_Limitation", TS_Limitation->Caption);//表示制限
	Ch_SizeLimitation->Caption = Ini->ReadString("View", "Ch_SizeLimitation", Ch_SizeLimitation->Caption);//サイズで表示制限
	La_Size->Caption = Ini->ReadString("View", "La_Size", La_Size->Caption);//サイズ
	Ch_LinkLimitation->Caption = Ini->ReadString("View", "Ch_LinkLimitation", Ch_LinkLimitation->Caption);//リンクで表示制限
	La_Links->Caption = Ini->ReadString("View", "La_Links", La_Links->Caption);//リンク
	Ch_LinkDirection->Caption = Ini->ReadString("View", "Ch_LinkDirection", Ch_LinkDirection->Caption);//向き
	RB_LinkForward->Caption = Ini->ReadString("View", "RB_LinkForward", RB_LinkForward->Caption);//前向き
	RB_LinkBackward->Caption = Ini->ReadString("View", "RB_LinkBackward", RB_LinkBackward->Caption);//後向き
	La_LinkTarget->Caption = Ini->ReadString("View", "La_LinkTarget", La_LinkTarget->Caption);//リンク先:
	Ch_DateLimitation->Caption = Ini->ReadString("View", "Ch_DateLimitation", Ch_DateLimitation->Caption);//日時で表示制限
	La_DateType->Caption = Ini->ReadString("View", "La_DateType", La_DateType->Caption);//日時の種類:
	La_DateLimitationType->Caption = Ini->ReadString("View", "La_DateLimitationType", La_DateLimitationType->Caption);//日時制限方法:
	PT_NoAssign->Caption = Ini->ReadString("View", "PT_NoAssign", PT_NoAssign->Caption);
	PT_Select->Caption = Ini->ReadString("View", "PT_Select", PT_Select->Caption);

	Co_DateType->Items->Strings[0] = MLText.Created;
	Co_DateType->Items->Strings[1] = MLText.Edited;
	Co_DateType->Items->Strings[2] = MLText.Viewed;

	Co_DateLimitationType->Items->Strings[0] = MLText.Old;
	Co_DateLimitationType->Items->Strings[1] = MLText.New;
	Co_DateLimitationType->Items->Strings[2] = MLText.Near;
	Co_DateLimitationType->Items->Strings[3] = MLText.Older;
	Co_DateLimitationType->Items->Strings[4] = MLText.Newer;

	delete Ini;
}
//---------------------------------------------------------------------------
__fastcall TFo_View::TFo_View(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFo_View::FormDeactivate(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFo_View::FormShow(TObject *Sender)
{
	ApplyLanguageSetting();
    SettingToGUI();
}
//---------------------------------------------------------------------------
void TFo_View::SettingToGUI()
{
    Ch_SizeLimitation->Checked = SettingView.m_bSizeLimitation;
    TB_SizeLimitation->Position = (int)(log(SettingView.m_nSizeLimitation / 100.0) * (8 / log(4)) + 100.5) - 100;
    TB_SizeLimitationChange(NULL);

    Ch_LinkLimitation->Checked = SettingView.m_bLinkLimitation;
    TB_LinkLimitation->Position = SettingView.m_nLinkLimitation;
    TB_LinkLimitationChange(NULL);
    Ch_LinkDirection->Checked = SettingView.m_bLinkDirection;
    if (SettingView.m_bLinkBackward){
        RB_LinkBackward->Checked = true;
    }else{
        RB_LinkForward->Checked = true;
    }
    RefreshLinkTarget();

    Ch_DateLimitation->Checked = SettingView.m_bDateLimitation;
    Co_DateLimitationType->ItemIndex = SettingView.m_DateLimitationType;
    Co_DateType->ItemIndex = SettingView.m_DateLimitationDateType;
    TB_DateLimitation->Position = SettingView.m_nDateLimitation;
    TB_LinkLimitationChange(NULL);

    //Label

    SG_Labels->RowCount = 1 + 1 + m_Document->m_Labels[0]->Count;//説明行＋No Label＋実際のラベル
	SG_Labels->Cells[1][0] = MLText.Enable;
	SG_Labels->Cells[2][0] = MLText.Fold;
	SG_Labels->Cells[3][0] = MLText.Show;
	SG_Labels->Cells[4][0] = MLText.Hide;
	SG_Labels->Cells[5][0] = MLText.Size;
	SG_Labels->Cells[0][0] = MLText.LabelName;
	SG_Labels->Cells[0][1] = MLText.NoLabel;

	for (int i = 0 ; i < m_Document->m_Labels[0]->Count ; i++){
		SG_Labels->Cells[0][i + 2] = m_Document->GetLabelByIndex(0, i)->m_Name;
	}
	SG_Labels->Invalidate();

	//LinkLabel

	SG_LinkLabels->RowCount = 1 + 1 + m_Document->m_Labels[1]->Count;//説明行＋No Label＋実際のラベル
	SG_LinkLabels->Cells[1][0] = MLText.Show;
	SG_LinkLabels->Cells[2][0] = MLText.Hide;
	SG_LinkLabels->Cells[0][0] = MLText.LabelName;
	SG_LinkLabels->Cells[0][1] = MLText.NoLabel;

	for (int i = 0 ; i < m_Document->m_Labels[1]->Count ; i++){
		SG_LinkLabels->Cells[0][i + 2] = m_Document->GetLabelByIndex(1, i)->m_Name;
	}
	SG_LinkLabels->Invalidate();

}
//---------------------------------------------------------------------------
void __fastcall TFo_View::TB_LinkLimitationChange(TObject *Sender)
{
	SettingView.m_nLinkLimitation = TB_LinkLimitation->Position;
	if (TB_LinkLimitation->Position < TB_LinkLimitation->Max){
		La_Links->Caption =
            IntToStr(TB_LinkLimitation->Position) + " " + MLText.Links;
    }else{
        /*
        //制限無し
        La_Links->Caption = "None";
        */
		La_Links->Caption =
			AnsiString("- ") + MLText.Links;
	}

	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFo_View::Bu_LabelShowAllClick(TObject *Sender)
{
	int ltype = ((TButton*)Sender)->Tag / 2;
	bool check = !(((TButton*)Sender)->Tag % 2);
	bool changed = false;
	for (int i = 0 ; i < m_Document->m_Labels[ltype]->Count ; i++){
		if (m_Document->GetLabelByIndex(ltype, i)->m_bShow != check){
            m_Document->GetLabelByIndex(ltype, i)->m_bShow = check;
            changed = true;
        }
    }

    if (changed){
        m_Document->RefreshList();
        m_Document->RefreshLabel();
        SettingToGUI();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFo_View::Bu_LabelHideAllClick(TObject *Sender)
{
    int ltype = ((TButton*)Sender)->Tag / 2;
    bool check = !(((TButton*)Sender)->Tag % 2);
    bool changed = false;
    for (int i = 0 ; i < m_Document->m_Labels[ltype]->Count ; i++){
        if (m_Document->GetLabelByIndex(ltype, i)->m_bHide != check){
            m_Document->GetLabelByIndex(ltype, i)->m_bHide = check;
            changed = true;
        }
    }

    if (changed){
        m_Document->RefreshList();
        m_Document->RefreshLabel();
        SettingToGUI();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFo_View::Bu_LabelFoldAllClick(TObject *Sender)
{
    int ltype = ((TButton*)Sender)->Tag / 2;
    bool check = !(((TButton*)Sender)->Tag % 2);
	bool changed = false;
    for (int i = 0 ; i < m_Document->m_Labels[ltype]->Count ; i++){
        if (m_Document->GetLabelByIndex(ltype, i)->m_bFold != check){
            m_Document->GetLabelByIndex(ltype, i)->m_bFold = check;
            changed = true;
        }
    }

    if (changed){
        m_Document->RefreshList();
        m_Document->RefreshLabel();
        SettingToGUI();
    }
}
//---------------------------------------------------------------------------


void __fastcall TFo_View::Ch_LinkDirectionClick(TObject *Sender)
{
    SettingView.m_bLinkDirection = Ch_LinkDirection->Checked;
    if (Sender){
        m_bRefreshRequest = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::SG_LabelsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TStringGrid *SG = (TStringGrid*)Sender;
    int type = SG->Tag;

    bool changed = false;
    int ACol, ARow;
    SG->MouseToCell(X, Y, ACol, ARow);
    RECT r = SG->CellRect(ACol, ARow);
	if (SG->Cells[ACol][0] == MLText.Fold){
		if (ARow > 1){
			m_Document->GetLabelByIndex(type, ARow - 2)->m_bFold = !m_Document->GetLabelByIndex(type, ARow - 2)->m_bFold;
			changed = true;
		}
	}else if (SG->Cells[ACol][0] == MLText.Enable){
		if (ARow > 1){
			m_Document->GetLabelByIndex(type, ARow - 2)->m_bEnable = !m_Document->GetLabelByIndex(type, ARow - 2)->m_bEnable;
			changed = true;
		}
	}else if (SG->Cells[ACol][0] == MLText.Show){
		if (ARow > 1){
			m_Document->GetLabelByIndex(type, ARow - 2)->m_bShow = !m_Document->GetLabelByIndex(type, ARow - 2)->m_bShow;
			changed = true;
		}else{
			if (type == 0){
				SettingView.m_bNoLabelVisible = !SettingView.m_bNoLabelVisible;
			}else{
				SettingView.m_bNoLinkLabelVisible = !SettingView.m_bNoLinkLabelVisible;
			}
			changed = true;

		}
	}else if (SG->Cells[ACol][0] == MLText.Hide){
		if (ARow > 1){
			m_Document->GetLabelByIndex(type, ARow - 2)->m_bHide = !m_Document->GetLabelByIndex(type, ARow - 2)->m_bHide;
			changed = true;
		}
	}else if (SG->Cells[ACol][0] == MLText.Size){
		if (ARow > 1){
			if (Y >= r.top + 2 && Y < r.bottom - 2 && X >= r.left + 2 && X < r.right - 2){
				int size = (int)(log(m_Document->GetLabelByIndex(type, ARow - 2)->m_nSize / 100.0) * (8 / log(4)) + 100.5) - 100;//8で4倍
				if (X < r.left + 13){
					//- button
					if (size > -8){
						size--;
						m_Document->GetLabelByIndex(type, ARow - 2)->m_nSize = (int)(100 * exp(size / (8 / log(4))) + 100) - 100;
						changed = true;
					}
				}else if (X >= r.right - 13){
					//+ button
					if (size < 8){
						size++;
						m_Document->GetLabelByIndex(type, ARow - 2)->m_nSize = (int)(100 * exp(size / (8 / log(4))) + 100) - 100;
						changed = true;
					}
				}
			}
		}
	}

	if (changed){
		m_Document->RefreshList();
		m_Document->RefreshLabel();
		SettingToGUI();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::SG_LabelsDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
    TStringGrid *SG = (TStringGrid*)Sender;
    int type = SG->Tag;

    TCanvas *Canvas = SG->Canvas;
    Canvas->Pen->Width = 1;
    Canvas->Pen->Style = psSolid;

    //背景
    Canvas->Brush->Style = bsSolid;
    Canvas->Brush->Color = clWhite;
	if (ARow == 0){
        Canvas->Brush->Color = clBtnFace;
    }
    Canvas->FillRect(Rect);

    //枠
    if (State.Contains(gdFocused) && ARow > 0 && ACol > 0){
        Canvas->Brush->Style = bsClear;
        Canvas->Pen->Color = clBlue;
        Canvas->Rectangle(Rect);
    }else if (ACol == 0 && ARow > 0){
        Canvas->Brush->Style = bsSolid;
        if (ARow >= 2){
            Canvas->Pen->Color = (TColor)m_Document->GetLabelByIndex(type, ARow - 2)->m_nColor;
            Canvas->Brush->Color = (TColor)HalfColor(m_Document->GetLabelByIndex(type, ARow - 2)->m_nColor, 0xffffff, 0.5f);
        }else{
            Canvas->Pen->Color = (TColor)0xc0c0c0;
            Canvas->Brush->Color = (TColor)0xe0e0e0;
        }
        Canvas->Rectangle(Rect);
        //Canvas->Rectangle(::Rect(Rect.left + 1, Rect.top + 1, Rect.right - 1, Rect.Bottom - 1));
    }


    if (ARow == 0 || ACol == 0){
        //字
        WideString S = SG->Cells[ACol][ARow];
        while (Canvas->TextWidth(S)>Rect.Width()-4){
            S = S.SubString(1, S.Length()-1);
        }
        Canvas->Brush->Style = bsClear;
        Canvas->Font->Color = clBlack;
        Canvas->TextOutA(Rect.left+2, Rect.top+2, S);
	}else if ((
		SG->Cells[ACol][0] == MLText.Fold ||
		SG->Cells[ACol][0] == MLText.Enable ||
		SG->Cells[ACol][0] == MLText.Show ||
		SG->Cells[ACol][0] == MLText.Hide)
		  || ARow == 1){
		int checked = -1;
		if (SG->Cells[ACol][0] == MLText.Fold){
			if (ARow > 1){
				checked = m_Document->GetLabelByIndex(type, ARow - 2)->m_bFold;
			}
		}else if (SG->Cells[ACol][0] == MLText.Enable){
			if (ARow > 1){
				checked = m_Document->GetLabelByIndex(type, ARow - 2)->m_bEnable;
			}
		}else if (SG->Cells[ACol][0] == MLText.Show){
			if (ARow > 1){
				checked = m_Document->GetLabelByIndex(type, ARow - 2)->m_bShow;
			}else{
				if (type == 0){
					checked = SettingView.m_bNoLabelVisible;
				}else{
					checked = SettingView.m_bNoLinkLabelVisible;
				}
			}
		}else if (SG->Cells[ACol][0] == MLText.Hide){
			if (ARow > 1){
				checked = m_Document->GetLabelByIndex(type, ARow - 2)->m_bHide;
			}
		}
		switch (checked){
		case -1:
			//Canvas->Pen->Style = psClear;
			Canvas->Pen->Color = TColor(0xe0e0e0);
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = TColor(0xe0e0e0);
			Canvas->Rectangle(Rect);
			break;
		case 1:
			Canvas->Pen->Color = clBlack;
			Canvas->Pen->Width = 3;
			Canvas->MoveTo(Rect.left + 5, Rect.top + 5);
			Canvas->LineTo(Rect.left + 8, Rect.bottom - 5);
			Canvas->LineTo(Rect.left + 17, Rect.top + 5);
			break;
		}
	}else if (SG->Cells[ACol][0] == MLText.Size && ARow > 1){
		//+-ボタン
		Canvas->Brush->Style = bsSolid;
		Canvas->Brush->Color = clSilver;
		Canvas->Pen->Style = psSolid;
		Canvas->Pen->Color = clGray;
		Canvas->Rectangle(Rect.left + 2, Rect.top + 2, Rect.left + 13, Rect.bottom - 2);
		Canvas->Rectangle(Rect.right - 13, Rect.top + 2, Rect.right - 2, Rect.bottom - 2);
		Canvas->Brush->Style = bsClear;
		Canvas->Font->Color = clBlack;
		Canvas->TextOutA(Rect.left + 2 + 3, Rect.top + 2, "-");
		Canvas->TextOutA(Rect.right - 13 + 3, Rect.top + 2, "+");


		//Size
		WideString S = IntToStr((int)(log(m_Document->GetLabelByIndex(type, ARow - 2)->m_nSize / 100.0) * (8 / log(4)) + 100.5) - 100);
		Canvas->TextOutA((Rect.left + Rect.right - Canvas->TextWidth(S)) / 2, Rect.top+2, S);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::Ch_SizeLimitationClick(TObject *Sender)
{
	SettingView.m_bSizeLimitation = Ch_SizeLimitation->Checked;
	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::TB_SizeLimitationChange(TObject *Sender)
{
	SettingView.m_nSizeLimitation = (int)(100 * exp(TB_SizeLimitation->Position / (8 / log(4))) + 100) - 100;
	La_Size->Caption = IntToStr(TB_SizeLimitation->Position);

	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::Ch_DateLimitationClick(TObject *Sender)
{
	SettingView.m_bDateLimitation = Ch_DateLimitation->Checked;
	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::TB_DateLimitationChange(TObject *Sender)
{
	SettingView.m_nDateLimitation = TB_DateLimitation->Position;
	//La_Size->Caption = IntToStr(TB_DateLimitation->Position);

	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------



void __fastcall TFo_View::Co_DateTypeChange(TObject *Sender)
{
	SettingView.m_DateLimitationDateType = Co_DateType->ItemIndex;

	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::Co_DateLimitationTypeChange(TObject *Sender)
{
	SettingView.m_DateLimitationType = Co_DateLimitationType->ItemIndex;

	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::SG_LabelsKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if (Key == 32){
		TRect R = SG_Labels->CellRect(SG_Labels->Col, SG_Labels->Row);

		SG_LabelsMouseDown(Sender, mbLeft, Shift, R.Left + 1, R.Top + 1);
	}
}
//---------------------------------------------------------------------------


void __fastcall TFo_View::RB_LinkForwardClick(TObject *Sender)
{
	SettingView.m_bLinkBackward = RB_LinkBackward->Checked;
	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::Ch_LinkLimitationClick(TObject *Sender)
{
	SettingView.m_bLinkLimitation = Ch_LinkLimitation->Checked;
	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::FormCreate(TObject *Sender)
{
	PC_View->ActivePage = TS_Label;
}
//---------------------------------------------------------------------------
void TFo_View::RefreshLinkTarget()
{
	if (SettingView.m_nLinkTarget < 0){
		Bu_LinkTarget->Caption = MLText.NoAssign;
	}else{
		TCard *Card = m_Document->GetCard(SettingView.m_nLinkTarget);
		Bu_LinkTarget->Caption = DecodeES(Card->m_Title, "");
	}
}
//---------------------------------------------------------------------------
void __fastcall TFo_View::PT_NoAssignClick(TObject *Sender)
{
	SettingView.m_nLinkTarget = -1;
	RefreshLinkTarget();
	if (Sender){
		m_bRefreshRequest = true;
	}
}
//---------------------------------------------------------------------------


void __fastcall TFo_View::PT_SelectClick(TObject *Sender)
{
	Fo_Select = new TFo_Select(this);
	Fo_Select->Caption = MLText.SelectTargetCard;
	Fo_Select->m_Document = m_Document;
	Fo_Select->ShowModal();
	if (Fo_Select->ModalResult == mrOk){
		SettingView.m_nLinkTarget = Fo_Select->m_nID;
		RefreshLinkTarget();
		if (Sender){
			m_bRefreshRequest = true;
		}
	}
	Fo_Select->Release();
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::Bu_LinkTargetClick(TObject *Sender)
{
	TPoint P;
	P.x = Bu_LinkTarget->Left;
	P.y = Bu_LinkTarget->Top;
	P = GB_LinkLimitation->ClientToScreen(P);
	PM_LinkTarget->Popup(P.x, P.y);
}
//---------------------------------------------------------------------------

void __fastcall TFo_View::FormKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if (Key == 27){
		Close();
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "foselectcard.h"
#include "setting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_Select *Fo_Select;
//---------------------------------------------------------------------------
void TFo_Select::ApplyLanguageSetting()
{
	if (!FileExists(LanguageFileName())){
		return;
	}
	TFastIni *Ini = new TFastIni(LanguageFileName());

	Bu_OK->Caption = Ini->ReadString("Dialog", "Bu_OK", Bu_OK->Caption);
	Bu_Cancel->Caption = Ini->ReadString("Dialog", "Bu_Cancel", Bu_Cancel->Caption);

	delete Ini;
}
//---------------------------------------------------------------------------
__fastcall TFo_Select::TFo_Select(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFo_Select::FormShow(TObject *Sender)
{
	ApplyLanguageSetting();
	
    LB_Items->MultiSelect = m_bMultiSelect;
    LB_Items->Items->BeginUpdate();
    switch (m_nType){
    case 0:
        for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
            TCard *Card = m_Document->GetCardByIndex(i);
            LB_Items->Items->Add(DecodeES(Card->m_Title, " "));
        }
        break;
    case 1:
    case 2:
        for (int i = 0 ; i < m_Document->m_Labels[m_nType - 1]->Count ; i++){
            TCardLabel *Label = m_Document->GetLabelByIndex(m_nType - 1, i);
            LB_Items->Items->Add(DecodeES(Label->m_Name, " "));
        }
        break;
	}
    LB_Items->Items->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TFo_Select::Bu_OKClick(TObject *Sender)
{
    m_nID = -1;
    switch (m_nType){
    case 0://Card
        if (!LB_Items->MultiSelect){
            TCard *Card = m_Document->GetCardByIndex(LB_Items->ItemIndex);
            if (Card){
                m_nID = Card->m_nID;
                m_IDs->Add((void*)m_nID);
            }
        }else{
            for (int i = 0 ; i < m_Document->m_Cards->Count && i < LB_Items->Count ; i++){
                if (LB_Items->Selected[i]){
                    TCard *Card = m_Document->GetCardByIndex(LB_Items->ItemIndex);
                    if (Card){
                        m_IDs->Add((void*)Card->m_nID);
                    }
                }
            }
        }
        break;
    case 1://Label
    case 2://Link Label
        if (!LB_Items->MultiSelect){
            m_nID = LB_Items->ItemIndex + 1;
            m_IDs->Add((void*)m_nID);
        }else{
            for (int i = 0 ; i < m_Document->m_Labels[m_nType - 1]->Count && i < LB_Items->Count ; i++){
                if (LB_Items->Selected[i]){
                    m_IDs->Add((void*)(i + 1));
				}
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFo_Select::LB_ItemsClick(TObject *Sender)
{
    if (!LB_Items->MultiSelect){
        Bu_OK->Enabled = true;
    }else{
        Bu_OK->Enabled = LB_Items->SelCount > 0;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFo_Select::LB_ItemsDblClick(TObject *Sender)
{
    if (LB_Items->MultiSelect && LB_Items->SelCount == 0){
        return;
    }
    ModalResult = mrOk;
    Bu_OKClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFo_Select::FormCreate(TObject *Sender)
{
    m_nType = 0;//カード。1=ラベル
    m_bMultiSelect = false;

    m_IDs = new TList();
}
//---------------------------------------------------------------------------

void __fastcall TFo_Select::FormDestroy(TObject *Sender)
{
    delete m_IDs;
}
//---------------------------------------------------------------------------


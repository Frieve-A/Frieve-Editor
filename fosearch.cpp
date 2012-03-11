//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <clipbrd.hpp>
#include "fosearch.h"
#include "setting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFo_Search *Fo_Search;
WideString SearchKeyword;//検索キーワード
WideString ReplaceKeyword;//置換先キーワード
int bSearchRequest = 0x30;//検索要求
int bLastSearchRequest = 0x30;//大文字小文字区別なし、非表示カードも対象
//---------------------------------------------------------------------------
void TFo_Search::ApplyLanguageSetting()
{
	if (!FileExists(LanguageFileName())){
		return;
	}
	TFastIni *Ini = new TFastIni(LanguageFileName());

	if (bReplace) {
		AnsiString Replace = "&Replace";
		Replace = Ini->ReadString("Search", "Replace", Replace);
		Caption = DeleteActionKey(Replace);
		Bu_FindNext->Caption = Replace;

		AnsiString ReplaceAll = "Replace All";
		ReplaceAll = Ini->ReadString("Search", "ReplaceAll", ReplaceAll);
		Ch_GlobalSearch->Caption = ReplaceAll;

		La_ReplaceTo->Caption = Ini->ReadString("Search", "La_ReplaceTo", La_ReplaceTo->Caption);
	}else{
		Caption = Ini->ReadString("Search", "Caption", Caption);
		Ch_GlobalSearch->Caption = Ini->ReadString("Search", "Ch_GlobalSearch", Ch_GlobalSearch->Caption);
		Bu_FindNext->Caption = Ini->ReadString("Search", "Bu_FindNext", Bu_FindNext->Caption);
	}
	La_Keyword->Caption = Ini->ReadString("Search", "La_Keyword", La_Keyword->Caption);
	Ch_CardTitle->Caption = Ini->ReadString("Search", "Ch_CardTitle", Ch_CardTitle->Caption);
	Ch_Text->Caption = Ini->ReadString("Search", "Ch_Text", Ch_Text->Caption);
	Ch_MatchCase->Caption = Ini->ReadString("Search", "Ch_MatchCase", Ch_MatchCase->Caption);
	Ch_InvisibleCard->Caption = Ini->ReadString("Search", "Ch_InvisibleCard", Ch_InvisibleCard->Caption);
	Ch_Backward->Caption = Ini->ReadString("Search", "Ch_Backward", Ch_Backward->Caption);

	Bu_Cancel->Caption = Ini->ReadString("Dialog", "Bu_Cancel", Bu_Cancel->Caption);

	delete Ini;
}
//---------------------------------------------------------------------------
__fastcall TFo_Search::TFo_Search(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFo_Search::Bu_CancelClick(TObject *Sender)
{
    Close();    
}
//---------------------------------------------------------------------------
void __fastcall TFo_Search::FormShow(TObject *Sender)
{
	if (bReplace) {
		Caption = "Replace";
		Bu_FindNext->Caption = "&Replace";
		Ch_GlobalSearch->Caption = "Replace All";

		La_ReplaceTo->Visible = true;
		Ed_ReplaceTo->Visible = true;
		Ed_ReplaceTo->Text = "";
	}else{
		Caption = "Search";
		Ch_GlobalSearch->Caption = "Global Search";
		Bu_FindNext->Caption = "&Find Next";

		La_ReplaceTo->Visible = false;
		Ed_ReplaceTo->Visible = false;
	}

	ApplyLanguageSetting();

	Co_Keyword->Text = SearchKeyword;
	Co_Keyword->SetFocus();
	Co_Keyword->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TFo_Search::Ed_KeywordChange(TObject *Sender)
{
	if (Ed_Keyword->Tag == 0){
		SearchKeyword = Ed_Keyword->Text;
		Co_Keyword->Tag = 1;
		Co_Keyword->Text = Ed_Keyword->Text;
		Co_Keyword->Tag = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFo_Search::Bu_FindNextClick(TObject *Sender)
{
	if (Ch_CardTitle->Checked){
        bSearchRequest |= 0x1;//カードタイトル検索
    }
	if (Ch_Text->Checked){
        bSearchRequest |= 0x2;//本文検索
    }
    if (bSearchRequest){
        bSearchRequest |= 0x8;//検索後このWindowを再表示
    }
    if (!Ch_MatchCase->Checked){//チェックされてないとき
        bSearchRequest |= 0x10;//大文字と小文字を区別しない（0で区別）
    }
    if (Ch_InvisibleCard->Checked){
        bSearchRequest |= 0x20;//非表示カードを検索
    }
    if (Ch_GlobalSearch->Checked){
        bSearchRequest |= 0x40;//グローバル検索
	}
    if (Ch_Backward->Checked){
        bSearchRequest |= 0x80;//後方検索
    }
	if (bReplace){
		bSearchRequest |= 0x100;//置換
		ReplaceKeyword = Ed_ReplaceTo->Text;
	}
	bLastSearchRequest = bSearchRequest;
}
//---------------------------------------------------------------------------
void __fastcall TFo_Search::FormCreate(TObject *Sender)
{
	bSearchRequest = 0;
	bReplace = false;
}
//---------------------------------------------------------------------------
void __fastcall TFo_Search::PM_UndoClick(TObject *Sender)
{
    Ed_Keyword->SelStart = Co_Keyword->SelStart;
    Ed_Keyword->SelLength = Co_Keyword->SelLength;
    Ed_Keyword->Undo();
    Co_Keyword->SelStart = Ed_Keyword->SelStart;
    Co_Keyword->SelLength = Ed_Keyword->SelLength;
}
//---------------------------------------------------------------------------

void __fastcall TFo_Search::PM_CutClick(TObject *Sender)
{
    Ed_Keyword->SelStart = Co_Keyword->SelStart;
    Ed_Keyword->SelLength = Co_Keyword->SelLength;
    Ed_Keyword->CutToClipboard();
    Co_Keyword->SelStart = Ed_Keyword->SelStart;
    Co_Keyword->SelLength = Ed_Keyword->SelLength;
}
//---------------------------------------------------------------------------

void __fastcall TFo_Search::PM_CopyClick(TObject *Sender)
{
	Ed_Keyword->SelStart = Co_Keyword->SelStart;
    Ed_Keyword->SelLength = Co_Keyword->SelLength;
    Ed_Keyword->CopyToClipboard();
    Co_Keyword->SelStart = Ed_Keyword->SelStart;
    Co_Keyword->SelLength = Ed_Keyword->SelLength;
    //Clipboard()->SetTextBuf(Co_Keyword->Text.SubString(Co_Keyword->SelStart + 1, Co_Keyword->SelLength).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFo_Search::PM_PasteClick(TObject *Sender)
{
    Ed_Keyword->SelStart = Co_Keyword->SelStart;
    Ed_Keyword->SelLength = Co_Keyword->SelLength;
    Ed_Keyword->PasteFromClipboard();
    Co_Keyword->SelStart = Ed_Keyword->SelStart;
	Co_Keyword->SelLength = Ed_Keyword->SelLength;
}
//---------------------------------------------------------------------------

void __fastcall TFo_Search::PM_SelectAllClick(TObject *Sender)
{
    Co_Keyword->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TFo_Search::Co_KeywordChange(TObject *Sender)
{
    if (Co_Keyword->Tag == 0){
        SearchKeyword = Co_Keyword->Text;
        Ed_Keyword->Tag = 1;
        Ed_Keyword->Text = Co_Keyword->Text;
        Ed_Keyword->Tag = 0;
    }
}
//---------------------------------------------------------------------------


void __fastcall TFo_Search::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    /*
    if (Key == 8){
        //BS
        Key = 0;
        Caption = "BS";
    }else{
        Caption = Key;
    }
    */
}
//---------------------------------------------------------------------------


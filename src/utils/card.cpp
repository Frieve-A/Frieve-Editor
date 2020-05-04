//---------------------------------------------------------------------------

#include "card.h"
#include "utils.h"
#include "setting.h"

//---------------------------------------------------------------------------
WideString EncodeES(WideString S)
{
    //エスケープシーケンスのエンコード（13→\n、\→\\）
    S = ReplaceText(S, "\\", "\\\\");
    return ReplaceText(S, "\r\n", "\\n");
}
//---------------------------------------------------------------------------
WideString DecodeES(WideString S, WideString Ret)
{
    //エスケープシーケンスのデコード（13←\n、\←\\）
    WideString result;
    while (true){
        int p = S.Pos("\\");
        if (p){
            result += S.SubString(1, p - 1);
            WideString S1 = S.SubString(p + 1, 1);
            if (S1 == WideString("n")){
                result += Ret;
                S = S.SubString(p + 2, S.Length());
            }else if (S1 == WideString("\\")){
                result += S1;
                S = S.SubString(p + 2, S.Length());
            }else{
                result += "\\";
                S = S.SubString(p + 1, S.Length());
            }
        }else{
            result += S;
            break;
        }
    }
    return result;
}
//---------------------------------------------------------------------------

TCard::TCard()
{
    m_Lines = new TStringList();
	m_Labels = new TLabelList();
	m_Drawing = new TDrawing();
	m_nID = 0;
	m_Title = "No title";

	//形、サイズ
	/*
	m_nShape = 2;//RoundRect
	m_nSize = 100;
	m_bFixed = false;
	*/
	m_nShape = SettingView.m_nCardDefaultShape;
	m_nSize = SettingView.m_nCardDefaultSize;
	m_bFixed = SettingView.m_bCardDefaultFixed;
	m_bTop = false;
	m_bFold = false;

	//座標
	m_fX = rand() % 10000 * 0.0001f;
	m_fY = rand() % 10000 * 0.0001f;
	m_bVisible = true;

	//日付
	m_fCreated = Now();
	m_fUpdated = Now();
	m_fViewed = Now();


	//保存されないデータ
	m_bGetFocus = true;

	m_nLevel = 0;

	m_nCreatedOrder = 0;
	m_nUpdatedOrder = 0;
	m_nViewedOrder = 0;

	m_fScore = 1.0;

	m_ImageFN = "";

	m_nMatrixX = 0;//格子状に配置した際の座標
	m_nMatrixY = 0;
	m_fMatrixSpeed = 0.0f;//格子間を移動する速度（～1.0）

	m_fTickerPos = 0;
	m_bVisibleBak = false;

	m_bLabelIsFold = false;

	m_Color = TColor(0);

	m_bSelected = false;
	m_nSelected = 0;

	m_nParentID = -1;
	m_bHasChild = false;
	m_nTmp = 0;
}
//---------------------------------------------------------------------------
TCard::TCard(TCard &Card)
{
	m_Lines = new TStringList();
	m_Labels = new TLabelList();
	m_Drawing = new TDrawing(*Card.m_Drawing);

	m_Lines->Text = Card.m_Lines->Text;
	for (int i = 0 ; i < Card.m_Labels->Count ; i++){
		m_Labels->AddLabel(Card.m_Labels->GetLabel(i));
	}

	//データ
	m_nID = Card.m_nID;
	m_Title = Card.m_Title;//タイトル
	m_bVisible = Card.m_bVisible;//表示、非表示
	m_nShape = Card.m_nShape;//形
	m_nSize = Card.m_nSize;//サイズ（100分率）
	m_bTop = Card.m_bTop;
	m_bFixed = Card.m_bFixed;//位置固定
	m_bFold = Card.m_bFold;//階層表示時に折りたたみ

	//ネット共有されないデータ（追加の際は、Document->SoftLoadを修正する必要あり）
	m_fX = Card.m_fX;//座標
	m_fY = Card.m_fY;//座標
	m_fCreated = Card.m_fCreated;//日付
	m_fUpdated = Card.m_fUpdated;//日付
	m_fViewed = Card.m_fViewed;//日付

	//保存されないデータ
	m_bGetFocus = Card.m_bGetFocus;//カーソル移動でフォーカスを得るかどうか

	m_nLevel = Card.m_nLevel;

	m_nCreatedOrder = Card.m_nCreatedOrder;//日付をソート順で0.0～100.0に正規化したもの
	m_nUpdatedOrder = Card.m_nUpdatedOrder;//日付をソート順で0.0～100.0に正規化したもの
	m_nViewedOrder = Card.m_nViewedOrder;//日付をソート順で0.0～100.0に正規化したもの
	m_fScore = Card.m_fScore;//スコア
	m_ImageFN = Card.m_ImageFN;//画像ファイル名（本文で最初に出てきた画像ファイル）
	m_VideoFN = Card.m_VideoFN;//ビデオファイル名（本文で最初に出てきた動画ファイル）

	m_nMatrixX = Card.m_nMatrixX;//格子状に配置した際の座標
	m_nMatrixY = Card.m_nMatrixY;//格子状に配置した際の座標
	m_fMatrixSpeed = Card.m_fMatrixSpeed;//格子間を移動する速度（～1.0）

    m_bLabelIsFold = Card.m_bLabelIsFold;//このカードの全てのラベルが折りたたまれているかどうか
    m_Color = Card.m_Color;//描画時の色

    m_fTickerPos = Card.m_fTickerPos;//Ticker。右端から何ドット左にスクロールしているか

    m_bVisibleBak = Card.m_bVisibleBak;//Visible情報のバックアップ（Visibleの変化を捉える）

    m_bSelected = Card.m_bSelected;
    m_nSelected = Card.m_nSelected;

    m_nParentID = Card.m_nParentID;
    m_bHasChild = Card.m_bHasChild;
    m_nTmp = Card.m_nSelected;
}
//---------------------------------------------------------------------------
TCard::~TCard()
{
    delete m_Labels;
	delete m_Lines;
	delete m_Drawing;
}
//---------------------------------------------------------------------------
void TCard::CheckImageFN()
{
    //本文に現れた最初のイメージファイル名をこのカードのイメージとする
    m_ImageFN = "";
    m_VideoFN = "";
    for (int i = 0 ; i < m_Lines->Count ; i++){
		if (IsFileNameOrURL(m_Lines->Strings[i])){
            UnicodeString Ext = LowerCase(ExtractFileExt(m_Lines->Strings[i]));
            if (Ext == ".bmp" || Ext == ".jpg" || Ext == ".jpeg"){
                m_ImageFN = m_Lines->Strings[i];
            }
            if (Ext == ".avi" || Ext == ".mpg" || Ext == ".mpeg" || Ext == ".wmv"){
                m_VideoFN = m_Lines->Strings[i];
            }
        }
    }
}
//---------------------------------------------------------------------------
void TCard::LoadFromString(TStringList *SL, int &line, int version)
{
    m_Lines->Clear();

    if (line >= SL->Count){
        //読み込むラインがない
    }else{
        int LineNum = StrToIntDef(SL->Strings[line++], 0);
        while (LineNum-- && line < SL->Count){
            m_Lines->Add(SL->Strings[line++]);
        }
    }

    while (m_Lines->Count){
        UnicodeString S = m_Lines->Strings[0];
        m_Lines->Delete(0);

        if (S == "-"){
            //ヘッダ終わり
            break;
        }else if (S.SubString(1, 6) == "Title:"){
            m_Title = S.SubString(7, S.Length());
		}else if (S.SubString(1, 6) == "Label:"){
			m_Labels->Decode(S.SubString(7, S.Length()));
		}else if (S.SubString(1, 8) == "Drawing:"){
			delete m_Drawing;
			m_Drawing = new TDrawing(S.SubString(9, S.Length()));
			m_Drawing->ClearSelection();
		}else if (S.SubString(1, 8) == "Visible:"){
			m_bVisible = StrToIntDef(S.SubString(9, S.Length()), true);
		}else if (S.SubString(1, 6) == "Shape:"){
			m_nShape = StrToIntDef(S.SubString(7, S.Length()), SettingView.m_nCardDefaultShape);
		}else if (S.SubString(1, 5) == "Size:"){
			m_nSize = StrToIntDef(S.SubString(6, S.Length()), SettingView.m_nCardDefaultSize);
		}else if (S.SubString(1, 4) == "Top:"){
			m_bTop = StrToIntDef(S.SubString(5, S.Length()), false);
		}else if (S.SubString(1, 5) == "Fold:"){
			m_bFold = StrToIntDef(S.SubString(6, S.Length()), false);
		}else if (S.SubString(1, 2) == "X:"){
			m_fX = StrToFloatDef(S.SubString(3, S.Length()), 0.5);
		}else if (S.SubString(1, 2) == "Y:"){
			m_fY = StrToFloatDef(S.SubString(3, S.Length()), 0.5);
		}else if (S.SubString(1, 6) == "Fixed:"){
			m_bFixed = StrToIntDef(S.SubString(7, S.Length()), SettingView.m_bCardDefaultFixed);
		}else if (S.SubString(1, 8) == "Created:"){
			m_fCreated = StrToDateTimeDef(S.SubString(9, S.Length()), 0.0);
		}else if (S.SubString(1, 8) == "Updated:"){
            m_fUpdated = StrToDateTimeDef(S.SubString(9, S.Length()), 0.0);
        }else if (S.SubString(1, 7) == "Viewed:"){
            m_fViewed = StrToDateTimeDef(S.SubString(8, S.Length()), 0.0);
        }
    }

    if (version <= 4){
        m_Title = ReplaceText(m_Title, "\\", "\\\\");
    }

    CheckImageFN();
}
//---------------------------------------------------------------------------
void TCard::SaveToString(TStringList *SL)
{
    TStringList *SL2 = new TStringList();
    SL2->Assign(m_Lines);

    //ヘッダの区切り
    SL2->Insert(0, "-");
    //ヘッダ
    SL2->Insert(0, UnicodeString("Viewed:") + DateTimeToStr(m_fViewed));
    SL2->Insert(0, UnicodeString("Updated:") + DateTimeToStr(m_fUpdated));
    SL2->Insert(0, UnicodeString("Created:") + DateTimeToStr(m_fCreated));

	SL2->Insert(0, UnicodeString("Visible:") + IntToStr(m_bVisible));
    SL2->Insert(0, UnicodeString("Shape:") + IntToStr(m_nShape));
    SL2->Insert(0, UnicodeString("Size:") + IntToStr(m_nSize));
	if (m_bFold){
		SL2->Insert(0, UnicodeString("Fold:") + IntToStr(m_bFold));
	}
	if (m_bTop){
		SL2->Insert(0, UnicodeString("Top:") + IntToStr(m_bTop));
	}
	SL2->Insert(0, UnicodeString("Y:") + FloatToStr(m_fY));
	SL2->Insert(0, UnicodeString("X:") + FloatToStr(m_fX));
	SL2->Insert(0, UnicodeString("Fixed:") + IntToStr(m_bFixed));

	if (m_Drawing->Count) {
		SL2->Insert(0, UnicodeString("Drawing:") + m_Drawing->Encode(true));
	}
	if (m_Labels->Count) {
		SL2->Insert(0, UnicodeString("Label:") + m_Labels->Encode());
	}
	SL2->Insert(0, UnicodeString("Title:") + m_Title);

    SL2->Insert(0, SL2->Count);

    for (int i = 0 ; i < SL2->Count ; i ++){
        SL->Add(SL2->Strings[i]);
    }

    delete SL2;
}
//---------------------------------------------------------------------------
void TCard::LoadFromFile(UnicodeString FN)
{
	//互換用

	m_Lines->LoadFromFile(FN);

	while (m_Lines->Count){
        UnicodeString S = m_Lines->Strings[0];
        m_Lines->Delete(0);

        if (S == "-"){
            //ヘッダ終わり
            break;
        }else if (S.SubString(1, 6) == "Title:"){
            m_Title = S.SubString(7, S.Length());
        }else if (S.SubString(1, 5) == "Type:"){//互換用
			m_Labels->Decode(S.SubString(6, S.Length()));
        }else if (S.SubString(1, 6) == "Label:"){
            m_Labels->Decode(S.SubString(7, S.Length()));
        }else if (S.SubString(1, 2) == "X:"){
            m_fX = StrToFloatDef(S.SubString(3, S.Length()), 0.5);
        }else if (S.SubString(1, 2) == "Y:"){
            m_fY = StrToFloatDef(S.SubString(3, S.Length()), 0.5);
        }else if (S.SubString(1, 8) == "Created:"){
			m_fCreated = StrToDateTimeDef(S.SubString(9, S.Length()), 0.0);
		}else if (S.SubString(1, 8) == "Updated:"){
			m_fUpdated = StrToDateTimeDef(S.SubString(9, S.Length()), 0.0);
		}else if (S.SubString(1, 7) == "Viewed:"){
			m_fViewed = StrToDateTimeDef(S.SubString(8, S.Length()), 0.0);
		}
	}

	m_Title = ReplaceText(m_Title, "\\", "\\\\");

	CheckImageFN();
}
//---------------------------------------------------------------------------
void TCard::SaveToFile(UnicodeString FN)
{
	//互換用

	TStringList *SL = new TStringList();
	SL->Assign(m_Lines);

	//ヘッダの区切り
	SL->Insert(0, "-");
	//ヘッダ
	SL->Insert(0, UnicodeString("Viewed:") + DateTimeToStr(m_fViewed));
	SL->Insert(0, UnicodeString("Updated:") + DateTimeToStr(m_fUpdated));
	SL->Insert(0, UnicodeString("Created:") + DateTimeToStr(m_fCreated));

	SL->Insert(0, UnicodeString("Y:") + FloatToStr(m_fY));
	SL->Insert(0, UnicodeString("X:") + FloatToStr(m_fX));

	SL->Insert(0, UnicodeString("Label:") + m_Labels->Encode());

	SL->Insert(0, UnicodeString("Title:") + m_Title);

    SL->SaveToFile(FN);
    delete SL;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#include "card.h"
#include "utils.h"
#include "setting.h"

//---------------------------------------------------------------------------
WideString EncodeES(WideString S)
{
    //�G�X�P�[�v�V�[�P���X�̃G���R�[�h�i13��\n�A\��\\�j
    S = ReplaceText(S, "\\", "\\\\");
    return ReplaceText(S, "\r\n", "\\n");
}
//---------------------------------------------------------------------------
WideString DecodeES(WideString S, WideString Ret)
{
    //�G�X�P�[�v�V�[�P���X�̃f�R�[�h�i13��\n�A\��\\�j
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

	//�`�A�T�C�Y
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

	//���W
	m_fX = rand() % 10000 * 0.0001f;
	m_fY = rand() % 10000 * 0.0001f;
	m_bVisible = true;

	//���t
	m_fCreated = Now();
	m_fUpdated = Now();
	m_fViewed = Now();


	//�ۑ�����Ȃ��f�[�^
	m_bGetFocus = true;

	m_nLevel = 0;

	m_nCreatedOrder = 0;
	m_nUpdatedOrder = 0;
	m_nViewedOrder = 0;

	m_fScore = 1.0;

	m_ImageFN = "";

	m_nMatrixX = 0;//�i�q��ɔz�u�����ۂ̍��W
	m_nMatrixY = 0;
	m_fMatrixSpeed = 0.0f;//�i�q�Ԃ��ړ����鑬�x�i�`1.0�j

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

	//�f�[�^
	m_nID = Card.m_nID;
	m_Title = Card.m_Title;//�^�C�g��
	m_bVisible = Card.m_bVisible;//�\���A��\��
	m_nShape = Card.m_nShape;//�`
	m_nSize = Card.m_nSize;//�T�C�Y�i100�����j
	m_bTop = Card.m_bTop;
	m_bFixed = Card.m_bFixed;//�ʒu�Œ�
	m_bFold = Card.m_bFold;//�K�w�\�����ɐ܂肽����

	//�l�b�g���L����Ȃ��f�[�^�i�ǉ��̍ۂ́ADocument->SoftLoad���C������K�v����j
	m_fX = Card.m_fX;//���W
	m_fY = Card.m_fY;//���W
	m_fCreated = Card.m_fCreated;//���t
	m_fUpdated = Card.m_fUpdated;//���t
	m_fViewed = Card.m_fViewed;//���t

	//�ۑ�����Ȃ��f�[�^
	m_bGetFocus = Card.m_bGetFocus;//�J�[�\���ړ��Ńt�H�[�J�X�𓾂邩�ǂ���

	m_nLevel = Card.m_nLevel;

	m_nCreatedOrder = Card.m_nCreatedOrder;//���t���\�[�g����0.0�`100.0�ɐ��K����������
	m_nUpdatedOrder = Card.m_nUpdatedOrder;//���t���\�[�g����0.0�`100.0�ɐ��K����������
	m_nViewedOrder = Card.m_nViewedOrder;//���t���\�[�g����0.0�`100.0�ɐ��K����������
	m_fScore = Card.m_fScore;//�X�R�A
	m_ImageFN = Card.m_ImageFN;//�摜�t�@�C�����i�{���ōŏ��ɏo�Ă����摜�t�@�C���j
	m_VideoFN = Card.m_VideoFN;//�r�f�I�t�@�C�����i�{���ōŏ��ɏo�Ă�������t�@�C���j

	m_nMatrixX = Card.m_nMatrixX;//�i�q��ɔz�u�����ۂ̍��W
	m_nMatrixY = Card.m_nMatrixY;//�i�q��ɔz�u�����ۂ̍��W
	m_fMatrixSpeed = Card.m_fMatrixSpeed;//�i�q�Ԃ��ړ����鑬�x�i�`1.0�j

    m_bLabelIsFold = Card.m_bLabelIsFold;//���̃J�[�h�̑S�Ẵ��x�����܂肽���܂�Ă��邩�ǂ���
    m_Color = Card.m_Color;//�`�掞�̐F

    m_fTickerPos = Card.m_fTickerPos;//Ticker�B�E�[���牽�h�b�g���ɃX�N���[�����Ă��邩

    m_bVisibleBak = Card.m_bVisibleBak;//Visible���̃o�b�N�A�b�v�iVisible�̕ω��𑨂���j

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
    //�{���Ɍ��ꂽ�ŏ��̃C���[�W�t�@�C���������̃J�[�h�̃C���[�W�Ƃ���
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
        //�ǂݍ��ރ��C�����Ȃ�
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
            //�w�b�_�I���
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

    //�w�b�_�̋�؂�
    SL2->Insert(0, "-");
    //�w�b�_
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
	//�݊��p

	m_Lines->LoadFromFile(FN);

	while (m_Lines->Count){
        UnicodeString S = m_Lines->Strings[0];
        m_Lines->Delete(0);

        if (S == "-"){
            //�w�b�_�I���
            break;
        }else if (S.SubString(1, 6) == "Title:"){
            m_Title = S.SubString(7, S.Length());
        }else if (S.SubString(1, 5) == "Type:"){//�݊��p
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
	//�݊��p

	TStringList *SL = new TStringList();
	SL->Assign(m_Lines);

	//�w�b�_�̋�؂�
	SL->Insert(0, "-");
	//�w�b�_
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

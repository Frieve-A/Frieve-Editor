//---------------------------------------------------------------------------

#pragma hdrstop
                                                                           
#include <IniFiles.hpp>
#include <math.h>
#include <clipbrd.hpp>

#include "document.h"
#include "utils.h"

//���N�G�X�g
int bReqArrange = -1;
int nReqArrangeMode = -1;
int bReqAutoScroll = -1;
int bReqAutoZoom = -1;
int bReqFullScreen = -1;
int bReqExit = -1;
float fReqZoom = -1000.0f, fReqX = -1000.0f, fReqY = -1000.0f;//�Y�[���ƍ��W
int nReqTargetCard = -1000;//�^�[�Q�b�g�J�[�h�ύX
int bReqSizeLimitation = -1, bReqLinkLimitation = -1, bReqDateLimitation = -1;//�\��������ON/OFF
int nReqSizeLimitation = -1;
int nReqLinkLimitation = -1, bReqLinkDirection = -1, bReqLinkBackward = -1, nReqLinkTarget = -2;
int nReqDateLimitation = -1, ReqDateLimitationDateType = -1, ReqDateLimitationType = -1;
int nReqKeyDown = -1;//����̃L�[������������������
//---------------------------------------------------------------------------
int __fastcall Func_CompCardRandom(void * Item1, void * Item2)
{
    return (rand() & 2 * 2) - 1;
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCard(void * Item1, void * Item2)
{
    return stricmp(((TCard *)Item1)->m_Title.c_str(), ((TCard *)Item2)->m_Title.c_str());
}
//---------------------------------------------------------------------------
int __fastcall Func_CompLabelTouched(void * Item1, void * Item2)
{
    float f = ((TCardLabel *)Item1)->m_fTouched - ((TCardLabel *)Item2)->m_fTouched;
    if (f > 0.0f){
        return 1;
    }else if (f == 0.0f){
        return 0;
    }else{
        return -1;
    }
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCardCreated(void * Item1, void * Item2)
{
    float f = ((TCard *)Item1)->m_fCreated - ((TCard *)Item2)->m_fCreated;
    if (f > 0.0f){
        return 1;
    }else if (f == 0.0f){
        return 0;
    }else{
        return -1;
    }
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCardEdited(void * Item1, void * Item2)
{
    float f = ((TCard *)Item1)->m_fUpdated - ((TCard *)Item2)->m_fUpdated;
    if (f > 0.0f){
        return 1;
    }else if (f == 0.0f){
        return 0;
    }else{
        return -1;
    }
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCardViewed(void * Item1, void * Item2)
{
    float f = ((TCard *)Item1)->m_fViewed - ((TCard *)Item2)->m_fViewed;
    if (f > 0.0f){
        return 1;
    }else if (f == 0.0f){
        return 0;
    }else{
        return -1;
    }
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCardScore(void * Item1, void * Item2)
{
    float f = ((TCard *)Item1)->m_fScore - ((TCard *)Item2)->m_fScore;
    if (f > 0.0f){
        return 1;
    }else if (f == 0.0f){
        return 0;
    }else{
        return -1;
    }
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCardI(void * Item1, void * Item2)
{
    return -stricmp(((TCard *)Item1)->m_Title.c_str(), ((TCard *)Item2)->m_Title.c_str());
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCardCreatedI(void * Item1, void * Item2)
{
    float f = ((TCard *)Item1)->m_fCreated - ((TCard *)Item2)->m_fCreated;
    if (f > 0.0f){
        return -1;
    }else if (f == 0.0f){
        return 0;
    }else{
        return 1;
    }
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCardEditedI(void * Item1, void * Item2)
{
    float f = ((TCard *)Item1)->m_fUpdated - ((TCard *)Item2)->m_fUpdated;
    if (f > 0.0f){
        return -1;
    }else if (f == 0.0f){
        return 0;
    }else{
        return 1;
    }
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCardViewedI(void * Item1, void * Item2)
{
    float f = ((TCard *)Item1)->m_fViewed - ((TCard *)Item2)->m_fViewed;
    if (f > 0.0f){
        return -1;
    }else if (f == 0.0f){
        return 0;
    }else{
        return 1;
    }
}
//---------------------------------------------------------------------------
int __fastcall Func_CompCardScoreI(void * Item1, void * Item2)
{
    float f = ((TCard *)Item1)->m_fScore - ((TCard *)Item2)->m_fScore;
    if (f > 0.0f){
        return -1;
    }else if (f == 0.0f){
        return 0;
    }else{
        return 1;
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//TUndoRedo�n
//---------------------------------------------------------------------------
TUndoRedoData::TUndoRedoData(TDocument *Doc, AnsiString Name, int CardID, int SelStart, int SelLength) :
    m_Doc(new TDocument(*Doc)),
    m_Name(Name),
	m_nCardID(CardID),
    m_nSelStart(SelStart),
	m_nSelLength(SelLength)
{
}
//---------------------------------------------------------------------------
TUndoRedoData::~TUndoRedoData()
{
    delete m_Doc;
}
//---------------------------------------------------------------------------
inline TUndoRedoData *TUndoRedo::UndoData(int index)
{
    return (TUndoRedoData*)m_Undos->Items[index];
}
//---------------------------------------------------------------------------
inline TUndoRedoData *TUndoRedo::RedoData(int index)
{
    return (TUndoRedoData*)m_Redos->Items[index];
}
//---------------------------------------------------------------------------
TUndoRedo::TUndoRedo(int maxundo) :
	m_Undos(new TList()),
	m_Redos(new TList()),
	m_nMaxUndo(maxundo),
	m_bChanged(false)
{
}
//---------------------------------------------------------------------------
TUndoRedo::~TUndoRedo()
{
	ClearRedos();
	ClearUndos();
	delete m_Redos;
	delete m_Undos;
}
//---------------------------------------------------------------------------
void TUndoRedo::ClearUndos()
{
	for (int i = 0 ; i < m_Undos->Count ; i++){
		delete UndoData(i);
	}
	m_Undos->Clear();
}
//---------------------------------------------------------------------------
void TUndoRedo::ClearRedos()
{
	for (int i = 0 ; i < m_Redos->Count ; i++){
		delete RedoData(i);
	}
	m_Redos->Clear();
}
//---------------------------------------------------------------------------
void TUndoRedo::Backup(TDocument *Doc, char *editname, int CardID, int SelStart, int SelLength)
{
	ClearRedos();
	m_Undos->Insert(0, new TUndoRedoData(Doc, editname, CardID, SelStart, SelLength));
	while (m_Undos->Count > m_nMaxUndo){
		delete UndoData(m_Undos->Count - 1);
		m_Undos->Delete(m_Undos->Count - 1);
	}
}
//---------------------------------------------------------------------------
void TUndoRedo::Undo(TDocument *Doc, int CardID, int SelStart, int SelLength, int *NextCardID, int *NextSelStart, int *NextSelLength)
{
	TUndoRedoData *Data = UndoData(0);
	AnsiString UndoName = Data->m_Name;
	*NextCardID = Data->m_nCardID;
	*NextSelStart = Data->m_nSelStart;
	*NextSelLength = Data->m_nSelLength;

	//���̃f�[�^��Redo�ɐς�
	m_Redos->Insert(0, new TUndoRedoData(Doc, UndoName, CardID, SelStart, SelLength));

	//Undo
	Doc->CopyFrom(Data->m_Doc);

	//�g����Undo���폜
	delete Data;
	m_Undos->Delete(0);

	m_bChanged = true;
}
//---------------------------------------------------------------------------
void TUndoRedo::Redo(TDocument *Doc, int CardID, int SelStart, int SelLength, int *NextCardID, int *NextSelStart, int *NextSelLength)
{
	TUndoRedoData *Data = RedoData(0);
	AnsiString RedoName = Data->m_Name;
	*NextCardID = Data->m_nCardID;
	*NextSelStart = Data->m_nSelStart;
	*NextSelLength = Data->m_nSelLength;

	//���̃f�[�^��Undo�ɐς�
	m_Undos->Insert(0, new TUndoRedoData(Doc, RedoName, CardID, SelStart, SelLength));

	//Redo
	Doc->CopyFrom(Data->m_Doc);

	//�g����Redo���폜
	delete Data;
	m_Redos->Delete(0);

	m_bChanged = true;
}
//---------------------------------------------------------------------------
bool TUndoRedo::GetCanUndo(char **editname)
{
	if (m_Undos->Count){
		*editname = UndoData(0)->m_Name.c_str();
		return true;
	}else{
		*editname = NULL;
		return false;
    }
}
//---------------------------------------------------------------------------
bool TUndoRedo::GetCanRedo(char **editname)
{
    if (m_Redos->Count){
        *editname = RedoData(0)->m_Name.c_str();
        return true;
    }else{
        *editname = NULL;
        return false;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//TDocument
//---------------------------------------------------------------------------
void TDocument::CreateCardIDToIndex()
{
    if (m_CardIDToIndex == NULL){
        m_CardIDToIndex = new int[m_nMaxCardID];
        for (int i = 0 ; i < m_nMaxCardID ; i++){
            m_CardIDToIndex[i] = -1;
        }
        for (int i = 0 ; i < m_Cards->Count ; i++){
            TCard *Card = GetCardByIndex(i);
            m_CardIDToIndex[Card->m_nID] = i;
        }
    }
}
//---------------------------------------------------------------------------
void TDocument::FreeCardIDToIndex()
{
    if (m_CardIDToIndex != NULL){
        delete[] m_CardIDToIndex;
        m_CardIDToIndex = NULL;
    }
}
//---------------------------------------------------------------------------
TCard *TDocument::NewCard(int insertindex)
{
    CreateCardIDToIndex();
    TCard *Card = new TCard();
    int id = -1;
    for (int i = 0 ; i < m_nMaxCardID ; i++){
        if (m_CardIDToIndex[i] < 0){
            id = i;
            break;
        }
    }

    if (id >= 0){
        Card->m_nID = id;
    }else{
        Card->m_nID = m_nMaxCardID ++;
    }
    m_Cards->Insert(insertindex, Card);

    RefreshList();
    return Card;
}
//---------------------------------------------------------------------------
void TDocument::SortCards(int sorttype, bool inverse)
{
    if (!inverse){
        switch (sorttype){
        case -1:
            m_Cards->Sort(Func_CompCardRandom);
            break;
        case 0:
            m_Cards->Sort(Func_CompCard);
            break;
        case 1:
            m_Cards->Sort(Func_CompCardCreated);
            break;
        case 2:
            m_Cards->Sort(Func_CompCardEdited);
            break;
        case 3:
            m_Cards->Sort(Func_CompCardViewed);
            break;
        case 4:
            m_Cards->Sort(Func_CompCardScore);
            break;
        }
    }else{
        switch (sorttype){
        case 0:
            m_Cards->Sort(Func_CompCardI);
            break;
        case 1:
            m_Cards->Sort(Func_CompCardCreatedI);
            break;
        case 2:
            m_Cards->Sort(Func_CompCardEditedI);
            break;
        case 3:
            m_Cards->Sort(Func_CompCardViewedI);
            break;
        case 4:
            m_Cards->Sort(Func_CompCardScoreI);
            break;
        }
    }
    RefreshList();
}
//---------------------------------------------------------------------------
TCard* TDocument::GetCardByIndex_(int Index)
{
    return (TCard *)m_Cards->Items[Index];
}
//---------------------------------------------------------------------------
TCard* TDocument::GetCardByIndex(int Index)
{
    return (TCard *)m_Cards->Items[Index];
}
//---------------------------------------------------------------------------
void TDocument::SetCardTitle(TCard *Card, char *szChar)
{
    Card->m_Title = szChar;
    RefreshList();
}
//---------------------------------------------------------------------------
void TDocument::SetCardText(TCard *Card, char *szChar)
{
    Card->m_Lines->Clear();
    Card->m_Lines->Text = szChar;
    Card->m_fUpdated = Now();
    Card->CheckImageFN();
    m_bChanged = true;
}
/*
void TDocument::SetCardText(int nID, char *szChar)
{
    TCard *Card = GetCard(nID);
    //Card->m_Lines->Assign(SL);
    Card->m_Lines->Clear();
    Card->m_Lines->Text = szChar;
    Card->m_fUpdated = Now();
    Card->CheckImageFN();
    m_bChanged = true;
}
*/
//---------------------------------------------------------------------------
TCard* TDocument::GetCard(int nID)
{
    int index = SearchCardIndex(nID);
    if (index >= 0){
        return GetCardByIndex_(index);
    }else{
        return NULL;
    }
}
//---------------------------------------------------------------------------
void TDocument::DeleteCard(int nID)
{
    int index = SearchCardIndex(nID);
    if (index >= 0){
        delete (TCard *)m_Cards->Items[index];
        m_Cards->Delete(index);
        RefreshList();
        for (int i = m_Links->Count - 1; i >= 0 ; i--){
            TLink *Link = GetLinkByIndex(i);
            if (Link->m_nFromID == nID || Link->m_nDestID == nID){
                DeleteLinkByIndex(i);
            }
        }
    }
}
//---------------------------------------------------------------------------
int TDocument::SearchCardIndex(int nID)
{
    if (nID >= 0 && nID < m_nMaxCardID){
        CreateCardIDToIndex();
        return m_CardIDToIndex[nID];
    }else{
        return -1;
    }
    /*
    for (int i = 0 ; i < m_Cards->Count ; i++){
        if (GetCardByIndex_(i)->m_nID == nID){
            return i;
        }
    }
    return -1;
    */
}
//---------------------------------------------------------------------------
void TDocument::ClearCards()
{
    while(m_Cards->Count){
        delete (TCard *)m_Cards->Items[0];
        m_Cards->Delete(0);
    }
    RefreshList();
}
//---------------------------------------------------------------------------
void TDocument::ClearCardSelection()
{
    for (int i = 0 ; i < m_Cards->Count ; i++){
        TCard *Card = GetCardByIndex(i);
        Card->m_bSelected = false;
    }
}
//---------------------------------------------------------------------------
void TDocument::SwapCard(int idx1, int idx2)
{
    void *bak = m_Cards->Items[idx1];
    m_Cards->Items[idx1] = m_Cards->Items[idx2];
    m_Cards->Items[idx2] = bak;
    RefreshList();
}
//---------------------------------------------------------------------------
void TDocument::RefreshDateOrder_Label()
{
    //m_fTouchedOrder�Ȃǂɐ������l����

    //�G�������Ƀ\�[�g
    {
        TList *List = new TList();
        for (int il = 0 ; il < m_Labels[0]->Count ; il++){
            List->Add(GetLabelByIndex(0, il));
        }
        List->Sort(Func_CompLabelTouched);
        for (int i = 0 ; i < List->Count ; i++){
            ((TCardLabel*)List->Items[i])->m_nTouchedOrder = i;
        }
        delete List;
    }
}
//---------------------------------------------------------------------------
void TDocument::RefreshDateOrder()
{
    //m_fCreatedOrder�Ȃǂɐ������l����
    //����́A�J�[�h�̊e���t���Ń\�[�g���A���ʂ�0.0�`100.0�ɐ��K����������
    //Date Limitation�i���t�ɂ��\�������j�ȂǂŎg�p

    //DateCreated
    {
        TList *List = new TList();
        for (int i = 0 ; i < m_Cards->Count ; i++){
            List->Add(GetCardByIndex(i));
        }
        List->Sort(Func_CompCardCreated);
        for (int i = 0 ; i < List->Count ; i++){
            ((TCard*)List->Items[i])->m_nCreatedOrder = i;
        }
        delete List;
    }

    //DateUpdated
    {
        TList *List = new TList();
        for (int i = 0 ; i < m_Cards->Count ; i++){
            List->Add(GetCardByIndex(i));
        }
        List->Sort(Func_CompCardEdited);
        for (int i = 0 ; i < List->Count ; i++){
            ((TCard*)List->Items[i])->m_nUpdatedOrder = i;
        }
        delete List;
    }

    //DateViewed
    {
        TList *List = new TList();
        for (int i = 0 ; i < m_Cards->Count ; i++){
            List->Add(GetCardByIndex(i));
        }
        List->Sort(Func_CompCardViewed);
        for (int i = 0 ; i < List->Count ; i++){
            ((TCard*)List->Items[i])->m_nViewedOrder = i;
        }
        delete List;
    }
}
//---------------------------------------------------------------------------
int TDocument::SearchParent(int CardID, bool bChild, bool bFocus)
{
    //�e�J�[�h��{��
    int ParentID = -1;
    double ParentDate = 0.0;
    //�����N���[�v
    for (int il = 0 ; il < m_Links->Count ; il++){
        TLink *Link = GetLinkByIndex(il);
        if (((Link->m_nDestID == CardID && !bChild) ||
             (Link->m_nFromID == CardID && bChild))
             && Link->m_bDirection
             && Link->m_bVisible){
            //���݂̃J�[�h�փ����N��\���Ă���J�[�h
            TCard *ParentCard;
            if (!bChild){
                ParentCard = GetCard(Link->m_nFromID);
            }else{
                ParentCard = GetCard(Link->m_nDestID);
            }
            if (ParentCard->m_nID != CardID && ParentCard->m_bVisible &&
                (ParentID == -1 || ParentCard->m_fViewed > ParentDate) &&
                (ParentCard->m_bGetFocus || !bFocus)){

                //�e�J�[�h���������Ă��Ȃ����A��ԍŋߌ���ꂽ�e�J�[�h

                //�e�J�[�h���X�V
                ParentID = ParentCard->m_nID;
                ParentDate = ParentCard->m_fViewed;
            }
        }
    }

    return ParentID;
}
//---------------------------------------------------------------------------
int TDocument::SearchBrother(int CurrentID, int ParentID, bool bInverse, bool bChild, bool bFocus)
{
    //�e�J�[�h��{��
    int CardID = -1;
    float MinD = 0.0f;

    //�����N���[�v
    TCard *ParentCard = GetCard(ParentID);
    TCard *CurrentCard = GetCard(CurrentID);
    if (ParentCard && CurrentCard){
        float xd0 = ParentCard->m_fX - CurrentCard->m_fX;
        float yd0 = ParentCard->m_fY - CurrentCard->m_fY;
        float rad0 = 0.0f;
        if (xd0 != 0.0f || yd0 != 0.0f){
            rad0 = atan2(yd0, xd0);
        }

        for (int il = 0 ; il < m_Links->Count ; il++){
            TLink *Link = GetLinkByIndex(il);
            if (((Link->m_nDestID == ParentID && bChild) ||
                 (Link->m_nFromID == ParentID && !bChild))
                 && Link->m_bDirection
                 && Link->m_bVisible){
                //�e�J�[�h���烊���N��\���Ă���J�[�h�i�Z��j
                TCard *Card;
                if (bChild){
                    Card = GetCard(Link->m_nFromID);
                }else{
                    Card = GetCard(Link->m_nDestID);
                }
                if (Card->m_bVisible && Card->m_nID != CurrentID && (Card->m_bGetFocus || !bFocus)){
                    //�p�x���v�Z
                    float xd = ParentCard->m_fX - Card->m_fX;
                    float yd = ParentCard->m_fY - Card->m_fY;
                    float rad = 0.0f;
                    if (xd != 0.0f || yd != 0.0f){
                        rad = atan2(yd, xd);
                    }
                    rad = rad0 - rad;
                    while (rad < 0.0f){
                        rad += 2 * M_PI;
                    }

                    if (CardID == -1 || ((MinD < rad) == bInverse)){
                        //����܂łň�ԋ߂��J�[�h

                        //�J�[�h���X�V
                        CardID = Card->m_nID;
                        MinD = rad;
                    }
                }
            }
        }
    }

    return CardID;
}
//---------------------------------------------------------------------------
int TDocument::SearchLast(int CardID, bool bFocus)
{
    //���O�ɕ\�����Ă����J�[�h��{��
    //�e�J�[�h��{��
    int LastID = -1;
    double LastDate = 0.0;
    //�����N���[�v
    for (int i = 0 ; i < m_Cards->Count ; i++){
        TCard *Card = GetCardByIndex(i);
        if (Card->m_bVisible && Card->m_nID != CardID && (LastID == -1 || Card->m_fViewed > LastDate) && (Card->m_bGetFocus || !bFocus)){
            //�J�[�h���������Ă��Ȃ����A��ԍŋߌ���ꂽ�J�[�h

            //�J�[�h���X�V
            LastID = Card->m_nID;
            LastDate = Card->m_fViewed;
        }
    }

    return LastID;
}
//---------------------------------------------------------------------------
void TDocument::RefreshCardLevel()
{
    //Card->m_bTop�ƃ����N�ɏ]���ĊK�w���x���ݒ�A�J�[�h�\�[�g

    //�K�w���Z�b�g
    void **orderbak = new void*[m_Cards->Count];
    for (int i = 0 ; i < m_Cards->Count ; i++){
        TCard *Card = GetCardByIndex(i);
        orderbak[i] = Card;
        Card->m_nLevel = 0;
        Card->m_nParentID = -1;//�e�J�[�hID�Ƃ��Ďg�p
        Card->m_bHasChild = false;
    }

    TList *RCard = GetRelatedCard(true, true);//�J�[�h�Ƀ����N����Ă���J�[�h���

    //�K�w���[�v
    bool changed = true;
    int level = 1;
    while (changed){
        changed = false;
        //�J�[�h���[�v
        for (int i = 0 ; i < m_Cards->Count ; i++){
            TCard *Card = GetCardByIndex(i);
            if (!Card->m_bTop && Card->m_nLevel == 0 && Card->m_nParentID == -1){
                //�K�w���ݒ肳��Ă��Ȃ��J�[�h
                //���̃J�[�h�ւ̃����N�𒲂ׂ�
                int count = RelatedCardNum(RCard, i);
                if (count){
                    //���̃J�[�h�֒���ꂽ�����N������

                    if (Card->m_Title == "�}���`���x��"){
                        count = count;
                    }

                    //���݂̃J�[�h����߂�����ɐe�J�[�h��T��
                    int startindex = 0;
                    for (int il = 0 ; il < count ; il++){
                        int index = RelatedIndex(RCard, i, il);
                        if (index < i){
                            //Index���O�̃J�[�h
                            startindex = il;
                        }else{
                            break;
                        }
                    }

                    //�K�w�ݒ�
                    bool changed1 = false;
                    int il = startindex;
                    int count2 = count;
                    do{
                        int index = RelatedIndex(RCard, i, il);
                        TCard *Card2 = GetCardByIndex(index);

                        if (i != index && (Card2->m_bTop || (Card2->m_nLevel > 0 && Card2->m_nLevel == level - 1))){
                            //�e�J�[�h�ɂȂ蓾��

                            //�e�J�[�h�ɂ���
                            Card->m_nLevel = level;
                            Card->m_nParentID = Card2->m_nID;
                            Card2->m_bHasChild = true;
                            changed1 = true;
                            break;
                        }

                        il = (il + count - 1) % count;//1��O��
                        count2--;
                    }while (count2);//�����N��S�ĕ]������܂�

                    changed |= changed1;
                }
            }
        }
        level++;
    }
    int maxlevel = level;

    FreeRelatedCard(RCard);

    //���ёւ�
    FreeCardIDToIndex();

    //Top�m�[�h���ŏ��Ɏ����Ă���
    for (int i = 1 ; i < m_Cards->Count ; i++){
        TCard *Card = GetCardByIndex(i);
        if (Card->m_bTop){
            int index = i;
            while (index > 0){
                TCard *Card2 = GetCardByIndex(index - 1);//1�O�̃J�[�h
                if (!Card2->m_bTop){
                    m_Cards->Items[index - 1] = Card;
                    m_Cards->Items[index] = Card2;
                    index--;
                }else{
                    break;
                }
            }
        }
    }

    //�K�w���[�v
    level = 0;
    int moved = true;
    while (moved || level <= maxlevel){
        moved = false;
        //�e�m�[�h���[�v
        //�J�[�h���[�v
        for (int i = 0 ; i < m_Cards->Count ; i++){
            TCard *Card = GetCardByIndex(i);
            if ((Card->m_bTop && level == 0) || (Card->m_nLevel == level && Card->m_nLevel > 0)){
                //�e�J�[�h
                int insindex = i + 1;//�e�J�[�h�̎��Ɏq�J�[�h�������Ă���

                //�J�[�h���[�v�i�q�j
                for (int i2 = 0 ; i2 < m_Cards->Count ; i2++){
                    TCard *Card2 = GetCardByIndex(i2);
                    if (Card2->m_nParentID == Card->m_nID){
                        //�e�J�[�h�̎q
                        if (i2 < insindex){
                            //�}���ʒu����O
                            //���肦�Ȃ��͂��H
                            i2 = i2;
                        }else if (i2 > insindex){
                            //�}���ʒu����끁�Ԃ̃J�[�hinsindex<=x<i2������1�����炷
                            for (int i3 = i2 ; i3 > insindex ; i3--){
                                m_Cards->Items[i3] = m_Cards->Items[i3 - 1];
                            }
                            m_Cards->Items[insindex] = Card2;
                            insindex++;
                            moved = true;
                            i2--;
                            i++;
                        }else{
                            //�������ʒu���ʒu�̈ړ��Ȃ�
                            insindex++;
                        }
                    }
                }
            }
        }

        level++;
    }

    //Index�ύX�`�F�b�N
    changed = false;
    for (int i = 0 ; i < m_Cards->Count && !changed ; i++){
        changed |= orderbak[i] != m_Cards->Items[i];
    }
    if (changed){
        RefreshList();
    }

    delete[] orderbak;
}
//---------------------------------------------------------------------------
void TDocument::AddLabelToCard(TCard *Card, int label)
{
    Card->m_Labels->AddLabel(label);
}
//---------------------------------------------------------------------------
void TDocument::DeleteLabelFromCard(TCard *Card, int label)
{
    Card->m_Labels->DeleteLabel(label);
}
//---------------------------------------------------------------------------
TList *TDocument::GetRelatedCard(bool bInverse, bool bVisibleOnly)
{
    //RCard�ɁA�e�J�[�h���璣��ꂽ�����N��̃J�[�h��Index���ɓ����
    TList *RCard = new TList();
    if (m_Cards->Count){
        for (int i = 0 ; i < m_Cards->Count ; i++){
            RCard->Add(new TList());
        }

        bool *matrix = new bool[m_Cards->Count * m_Cards->Count];
        memset(matrix, 0, sizeof(bool) * m_Cards->Count * m_Cards->Count);

        for (int il = 0 ; il < m_Links->Count ; il++){
            TLink *Link = GetLinkByIndex(il);
            if ((Link->m_bVisible || !bVisibleOnly) && Link->m_bDirection){
                int indexfrom = SearchCardIndex(Link->m_nFromID);
                TCard *From = GetCardByIndex(indexfrom);
                int indexdest = SearchCardIndex(Link->m_nDestID);
                TCard *Dest = GetCardByIndex(indexdest);
                if ((From->m_bVisible && Dest->m_bVisible) || !bVisibleOnly){
                    if (bInverse){
                        //�t�����i�e�J�[�h�փ����N�𒣂��Ă���J�[�h��Index������j
                        matrix[indexdest * m_Cards->Count + indexfrom] = true;
                    }else{
                        //������
                        matrix[indexfrom * m_Cards->Count + indexdest] = true;
                    }
                }
            }
        }

        for (int ifrom = 0 ; ifrom < m_Cards->Count ; ifrom++){
            for (int idest = 0 ; idest < m_Cards->Count ; idest++){
                if (matrix[ifrom * m_Cards->Count + idest]){
                    ((TList *)RCard->Items[ifrom])->Add((void*)idest);
                }
            }
		}
		delete[] matrix;
    }
    return RCard;
}
//---------------------------------------------------------------------------
int TDocument::RelatedCardNum(TList *RCard, int cardindex)
{
    return ((TList *)RCard->Items[cardindex])->Count;
}
//---------------------------------------------------------------------------
int TDocument::RelatedIndex(TList *RCard, int cardindex, int index)
{
    return (int)((TList *)RCard->Items[cardindex])->Items[index];
}
//---------------------------------------------------------------------------
void TDocument::FreeRelatedCard(TList *RCard)
{
    //RCard�̔j��
    for (int i = 0 ; i < m_Cards->Count ; i++){
        delete (TList *)RCard->Items[i];
    }
    delete RCard;
}
//---------------------------------------------------------------------------
TDocument::TDocument()
{
    InitDocument();
}
//---------------------------------------------------------------------------
void TDocument::InitDocument()
{
    m_nRefreshListCount = 0;
    m_nRefreshLinkCount = 0;
    m_nRefreshLabelCount = 0;

    m_nMaxCardID = 0;
    m_CardIDToIndex = NULL;
    m_nCardID = -1;
    m_FN = "";

    m_Cards = new TList();
    TCard *Card = new TCard();
    Card->m_fX = 0.5;
    Card->m_fY = 0.5;
    m_Cards->Add(Card);
    m_nMaxCardID++;
    RefreshList();

    m_Links = new TList();
    RefreshLink();

    m_Labels[0] = new TList();
    InitLabel(0);

    m_Labels[1] = new TList();
    InitLabel(1);

    RefreshLabel();

    m_bChanged = false;
    m_bReadOnly = false;
    m_nDefaultView = -1;

    m_nCheckCount = 0;
}
//---------------------------------------------------------------------------
TDocument::TDocument(TDocument &Doc)
{
    //������
    InitDocument();

    CopyFrom(&Doc);
}
//---------------------------------------------------------------------------
void TDocument::CopyFrom(TDocument *Doc)
{
    ClearCards();
    ClearLinks();
    ClearLabels(0);
    ClearLabels(1);

    //�J�[�h�R�s�[
    for (int i = 0 ; i < Doc->m_Cards->Count ; i++){
        TCard *Card = Doc->GetCardByIndex(i);
        m_Cards->Add(new TCard(*Card));
        if (Card->m_nID >= m_nMaxCardID){
            m_nMaxCardID = Card->m_nID + 1;
        }
    }

    //�����N�R�s�[
    for (int i = 0 ; i < Doc->m_Links->Count ; i++){
        m_Links->Add(new TLink(*Doc->GetLinkByIndex(i)));
    }

    //���x���R�s�[
    for (int il = 0 ; il < 2 ; il++){
        for (int i = 0 ; i < Doc->m_Labels[il]->Count ; i++){
            m_Labels[il]->Add(new TCardLabel(*Doc->GetLabelByIndex(il, i)));
        }
    }

    //��ʍX�V�J�E���^
    m_nCheckCount = Doc->m_nCheckCount;
    //�\���X�V�p
    m_nRefreshListCount = Doc->m_nRefreshListCount;
    m_nRefreshLinkCount = Doc->m_nRefreshLinkCount;
    m_nRefreshLabelCount = Doc->m_nRefreshLabelCount;

    //���̑�
    m_bChanged = Doc->m_bChanged;
    m_FN = Doc->m_FN;
    m_bReadOnly = Doc->m_bReadOnly;

    m_nCardID = Doc->m_nCardID;//���ݕ\�����̃J�[�h�i�ۑ��p�j
    m_nDefaultView = Doc->m_nDefaultView;//�ǂݍ��񂾏u�Ԃǂ̉�ʂ�\�����邩�i-1=�Ȃ��A0=Browser�A1=Editor�j
}
//---------------------------------------------------------------------------
TDocument::~TDocument()
{
    FreeCardIDToIndex();

    ClearLabels(1);
    ClearLabels(0);
    delete m_Labels[1];
    delete m_Labels[0];

    ClearLinks();
    delete m_Links;
    ClearCards();
    delete m_Cards;
}
//---------------------------------------------------------------------------
void TDocument::RefreshList()
{
    FreeCardIDToIndex();

    m_nRefreshListCount++;
    m_bChanged = true;
}
/*
#include <mmsystem.hpp>
    unsigned int tgt = timeGetTime();
    ShowMessage(timeGetTime() - tgt);
*/
//---------------------------------------------------------------------------
bool TDocument::LoadFromString(TStringList *SL, AnsiString FN)
{
    bool result = true;

    //������
    ClearCards();
    ClearLinks();
    ClearLabels(0);
    ClearLabels(1);

    int line = 1;
    while (line < SL->Count){
        if (SL->Strings[line++] == "[CardData]"){
            break;
        }
    }

    TStringList *SL2 = new TStringList();
    for (int i = 0 ; i < line - 1 ; i++){
        SL2->Add(SL->Strings[i]);
    }
    TFastIni *Ini = new TFastIni(SL2);

    int version = Ini->ReadInteger("Global", "Version", 0);

    if (version <= 0){
        delete Ini;
        delete SL2;
        if (FN != ""){
            return Load_Old(FN);
        }else{
            return false;
        }
    }

    //�ǂݎ���p
    m_bReadOnly = Ini->ReadBool("Global", "ReadOnly", 0);
    m_nDefaultView = Ini->ReadInteger("Global", "DefaultView", -1);

    //����v��
	bReqArrange = Ini->ReadInteger("Global", "Arrange", bReqArrange);//�A�����W��ON/OFF
    nReqArrangeMode = Ini->ReadInteger("Global", "ArrangeMode", nReqArrangeMode);//0���珇��Repulsion�ALink�ALabel�AIndex
    bReqAutoScroll = Ini->ReadInteger("Global", "AutoScroll", bReqAutoScroll);//�I�[�g�X�N���[��
    bReqAutoZoom = Ini->ReadInteger("Global", "AutoZoom", bReqAutoZoom);//�I�[�g�Y�[��
    bReqFullScreen = Ini->ReadInteger("Global", "FullScreen", bReqFullScreen);//�t���X�N���[��
    bReqExit = Ini->ReadInteger("Global", "Exit", bReqExit);//�I��
    fReqZoom = Ini->ReadFloat("Global", "Zoom", fReqZoom);
    fReqX = Ini->ReadFloat("Global", "X", fReqX);
    fReqY = Ini->ReadFloat("Global", "Y", fReqY);
    nReqTargetCard = Ini->ReadInteger("Global", "TargetCard", nReqTargetCard);
    bReqSizeLimitation = Ini->ReadInteger("Global", "SizeLimitation", bReqSizeLimitation);
    bReqLinkLimitation = Ini->ReadInteger("Global", "LinkLimitation", bReqLinkLimitation);
	bReqDateLimitation = Ini->ReadInteger("Global", "DateLimitation", bReqDateLimitation);
	nReqSizeLimitation = Ini->ReadInteger("Global", "SizeLimitation", nReqSizeLimitation);
	nReqLinkLimitation = Ini->ReadInteger("Global", "LinkLimitation", nReqLinkLimitation);
	bReqLinkDirection = Ini->ReadInteger("Global", "LinkDirection", bReqLinkDirection);
	bReqLinkBackward = Ini->ReadInteger("Global", "LinkBackward", bReqLinkBackward);
	nReqLinkTarget = Ini->ReadInteger("Global", "LinkTarget", nReqLinkTarget);
	nReqDateLimitation = Ini->ReadInteger("Global", "DateLimitation", nReqDateLimitation);
	ReqDateLimitationDateType = Ini->ReadInteger("Global", "DateLimitationDateType", ReqDateLimitationDateType);
	ReqDateLimitationType = Ini->ReadInteger("Global", "DateLimitationType", ReqDateLimitationType);

	//�J�[�h��ID��ǂݍ���
    int cardnum = Ini->ReadInteger("Card", "Num", 0);
	m_nCardID = Ini->ReadInteger("Card", "CardID", -1);

    int maxid = 0;
    for (int i = 0 ; i < cardnum ; i++){
        //TCard *Card = NewCard(m_Cards->Count);
        TCard *Card = new TCard();
        m_Cards->Add(Card);
        Card->m_nID = Ini->ReadInteger("Card", IntToStr(i), 0);
        if (Card->m_nID > maxid){
            maxid = Card->m_nID;
        }
    }

    m_nMaxCardID = maxid + 1;

    //�����N�ǂݍ���
    int linknum = Ini->ReadInteger("Link", "Num", 0);
    for (int i = 0 ; i < linknum ; i++){
        TLink *Link = NewLink();
        if (version < 6){
            Link->Decode_005(Ini->ReadString("Link", IntToStr(i), ""));
        }else if (version < 7){
            Link->Decode_006(Ini->ReadString("Link", IntToStr(i), ""));
        }else{
            Link->Decode(Ini->ReadString("Link", IntToStr(i), ""));
        }
    }

    //���x���ǂݍ���
    int labelnum = Ini->ReadInteger("Label", "Num", -1);
    if (labelnum < 0){
        InitLabel(0);
    }else{
        for (int i = 0 ; i < labelnum ; i++){
            TCardLabel *Label = NewLabel(0);
            switch(version){
            case 0:
            case 1:
                Label->Decode_001(Ini->ReadString("Label", IntToStr(i), ""));
                break;
            case 2:
                Label->Decode_002(Ini->ReadString("Label", IntToStr(i), ""));
                break;
            case 3:
                Label->Decode_003(Ini->ReadString("Label", IntToStr(i), ""));
                break;
            case 4:
            case 5:
            case 6:
                Label->Decode_006(Ini->ReadString("Label", IntToStr(i), ""));
                break;
            default:
                Label->Decode(Ini->ReadString("Label", IntToStr(i), ""));
            }
        }
    }

    //���x���ǂݍ���
    labelnum = Ini->ReadInteger("LinkLabel", "Num", -1);
    if (labelnum < 0){
        InitLabel(1);
    }else{
        for (int i = 0 ; i < labelnum ; i++){
            TCardLabel *Label = NewLabel(1);
            switch(version){
            case 0:
            case 1:
                Label->Decode_001(Ini->ReadString("LinkLabel", IntToStr(i), ""));
                break;
            case 2:
                Label->Decode_002(Ini->ReadString("LinkLabel", IntToStr(i), ""));
                break;
            case 3:
                Label->Decode_003(Ini->ReadString("LinkLabel", IntToStr(i), ""));
                break;
            case 4:
            case 5:
            case 6:
                Label->Decode_006(Ini->ReadString("LinkLabel", IntToStr(i), ""));
                break;
            default:
                Label->Decode(Ini->ReadString("LinkLabel", IntToStr(i), ""));
            }
        }
    }

    delete Ini;
    delete SL2;

    for (int i = 0 ; i < cardnum ; i++){
        TCard *Card = GetCardByIndex_(i);
        Card->LoadFromString(SL, line, version);
    }

    return result;
}
//---------------------------------------------------------------------------
bool TDocument::SoftLoadFromString(TStringList *SL, AnsiString FN)
{
    //20070804����_continuousload.fip�݂̂ŗL���ȋ@�\

    //���x���A�����N���x���A�����N�͂��̂܂ܓǂݍ���
    //�J�[�h�́A���t�ƈʒu�͌��̒l��ێ�

    bool result = true;
    ClearLinks();

    TDocument *Tmp = new TDocument();
    result &= Tmp->LoadFromString(SL, FN);

    //�J�[�h���W�A���t�̈��p��
    for (int i = 0 ; i < m_Cards->Count ; i++){
        TCard *Card = GetCardByIndex(i);
        TCard *Card2 = Tmp->GetCard(Card->m_nID);
        if (Card2){
            Card2->m_fX = Card->m_fX;
            Card2->m_fY = Card->m_fY;
            Card2->m_fCreated = Card->m_fCreated;
            Card2->m_fUpdated = Card->m_fUpdated;
            Card2->m_fViewed = Card->m_fViewed;
        }
    }
    //�J�[�h�R�s�[
    ClearCards();
    int maxid = 0;
    for (int i = 0 ; i < Tmp->m_Cards->Count ; i++){
        TCard *Card2 = Tmp->GetCardByIndex(i);
        //TCard *Card = NewCard(m_Cards->Count);
        TCard *Card = new TCard();
        m_Cards->Add(Card);
        Card->m_nID = Card2->m_nID;
        TStringList *SL2 = new TStringList();
        Card2->SaveToString(SL2);
        int line = 0;
        Card->LoadFromString(SL2, line, FileVersion);

        if (Card->m_nID > maxid){
            maxid = Card->m_nID;
        }
    }

    m_nMaxCardID = maxid + 1;

    //�����N�R�s�[
    for (int i = 0 ; i < Tmp->m_Links->Count ; i++){
        TLink *Link = NewLink();
        Link->Decode(Tmp->GetLinkByIndex(i)->Encode());
    }

    //���x���p�����[�^�̈��p��
    for (int il = 0 ; il < 2 ; il++){
        for (int i = 0 ; i < Tmp->m_Labels[il]->Count ; i++){
            TCardLabel *Label2 = Tmp->GetLabelByIndex(il, i);
            TCardLabel *Label = GetLabel(il, Label2->m_Name);
            if (Label){
                Label2->m_bEnable = Label->m_bEnable;
                Label2->m_bFold = Label->m_bFold;
                Label2->m_bShow = Label->m_bShow;
                Label2->m_bHide = Label->m_bHide;
            }
        }
    }

    //���x���R�s�[
    ClearLabels(0);
    ClearLabels(1);
    for (int il = 0 ; il < 2 ; il++){
        for (int i = 0 ; i < Tmp->m_Labels[il]->Count ; i++){
            TCardLabel *Label = NewLabel(il);
            Label->Decode(Tmp->GetLabelByIndex(il, i)->Encode());
        }
    }

    delete Tmp;

    return result;
}
//---------------------------------------------------------------------------
bool TDocument::SaveToString(TStringList *SL)
{
    bool result = true;

    SL->Add("[Global]");
    SL->Add(AnsiString("Version=") + IntToStr(FileVersion));

	//�\�����
	SL->Add(AnsiString("Arrange=") + IntToStr(bReqArrange));//�A�����W��ON/OFF
	SL->Add(AnsiString("ArrangeMode=") + IntToStr(nReqArrangeMode));//0���珇��Repulsion�ALink�ALabel�AIndex
	SL->Add(AnsiString("AutoScroll=") + IntToStr(bReqAutoScroll));//�I�[�g�X�N���[��
	SL->Add(AnsiString("AutoZoom=") + IntToStr(bReqAutoZoom));//�I�[�g�Y�[��
	SL->Add(AnsiString("FullScreen=") + IntToStr(bReqFullScreen));//�t���X�N���[��
	SL->Add(AnsiString("Exit=") + IntToStr(bReqExit));//�I��
	SL->Add(AnsiString("Zoom=") + FloatToStr(fReqZoom));
	SL->Add(AnsiString("X=") + FloatToStr(fReqX));
	SL->Add(AnsiString("Y=") + FloatToStr(fReqY));
	SL->Add(AnsiString("TargetCard=") + IntToStr(nReqTargetCard));
	SL->Add(AnsiString("SizeLimitation=") + IntToStr(bReqSizeLimitation));
	SL->Add(AnsiString("LinkLimitation=") + IntToStr(bReqLinkLimitation));
	SL->Add(AnsiString("DateLimitation=") + IntToStr(bReqDateLimitation));
	SL->Add(AnsiString("SizeLimitation=") + IntToStr(nReqSizeLimitation));
	SL->Add(AnsiString("LinkLimitation=") + IntToStr(nReqLinkLimitation));
	SL->Add(AnsiString("LinkDirection=") + IntToStr(bReqLinkDirection));
	SL->Add(AnsiString("LinkBackward=") + IntToStr(bReqLinkBackward));
	SL->Add(AnsiString("LinkTarget=") + IntToStr(nReqLinkTarget));
	SL->Add(AnsiString("DateLimitation=") + IntToStr(nReqDateLimitation));
	SL->Add(AnsiString("DateLimitationDateType=") + IntToStr(ReqDateLimitationDateType));
	SL->Add(AnsiString("DateLimitationType=") + IntToStr(ReqDateLimitationType));

	//�J�[�h��ID��ۑ�
    SL->Add("[Card]");
    SL->Add(AnsiString("CardID=") + m_nCardID);
    SL->Add(AnsiString("Num=") + m_Cards->Count);
    for (int i = 0 ; i < m_Cards->Count ; i++){
        SL->Add(IntToStr(i) + AnsiString("=") + GetCardByIndex_(i)->m_nID);
    }

    //�����N�ۑ�
    SL->Add("[Link]");
    SL->Add(AnsiString("Num=") + m_Links->Count);
    for (int i = 0 ; i < m_Links->Count ; i++){
        SL->Add(IntToStr(i) + AnsiString("=") + GetLinkByIndex(i)->Encode());
    }

    //���x���ۑ�
    SL->Add("[Label]");
    SL->Add(AnsiString("Num=") + m_Labels[0]->Count);
    for (int i = 0 ; i < m_Labels[0]->Count ; i++){
        SL->Add(IntToStr(i) + AnsiString("=") + GetLabelByIndex(0, i)->Encode());
    }

    //���x���ۑ�
    SL->Add("[LinkLabel]");
    SL->Add(AnsiString("Num=") + m_Labels[1]->Count);
    for (int i = 0 ; i < m_Labels[1]->Count ; i++){
        SL->Add(IntToStr(i) + AnsiString("=") + GetLabelByIndex(1, i)->Encode());
    }

    //�e�J�[�h��ۑ�
    SL->Add("[CardData]");
    for (int i = 0 ; i < m_Cards->Count ; i++){
        TCard *Card = GetCardByIndex_(i);
        Card->SaveToString(SL);
    }

    return result;
}
//---------------------------------------------------------------------------
bool TDocument::Load(AnsiString FN, bool bSoftLoad)
{
    bool result = true;

    m_FN = FN;
    if (!FileExists(FN)){
        return false;
    }

    TStringList *SL = new TStringList();
    try{
        SL->LoadFromFile(m_FN);
        if (bSoftLoad){
            SoftLoadFromString(SL, m_FN);
        }else{
            LoadFromString(SL, m_FN);
        }
        m_bChanged = false;
    }
    catch(...){
        result = false;
    }
    delete SL;

    //�t�@�C�������Ă����ꍇ�̏����i�K�v�Ȃ����A�ꉞ�j
    if (SearchCardIndex(m_nCardID) == -1){
        m_nCardID = -1;
    }

    return result;
}
//---------------------------------------------------------------------------
bool TDocument::Save()
{
    bool result;

    //Global�f�[�^�ۑ�
    TStringList *SL = new TStringList();

    result = SaveToString(SL);

	try{
		SL->SaveToFile(m_FN);
	}
	catch(...){
        result = false;
    }

    delete SL;

    if (result){
        m_bChanged = false;
    }
    return result;
}
//---------------------------------------------------------------------------
bool TDocument::Load_Old(AnsiString FN)
{
    m_FN = FN;

    //������
    ClearCards();
    ClearLinks();

    ClearLabels(0);
    ClearLabels(1);

    //�e�J�[�h�ۑ��p�t�H���_
    AnsiString Dir = m_FN.SubString(1, m_FN.Length() - ExtractFileExt(m_FN).Length());

    //�J�[�h��ID��ǂݍ���
    TIniFile *Ini = new TIniFile(m_FN);
    int cardnum = Ini->ReadInteger("Card", "Num", 0);
    m_nCardID = -1;

    int maxid = 0;
    for (int i = 0 ; i < cardnum ; i++){
        //TCard *Card = NewCard(m_Cards->Count);
        TCard *Card = new TCard();
        m_Cards->Add(Card);
        Card->m_nID = Ini->ReadInteger("Card", IntToStr(i), 0);
        if (Card->m_nID > maxid){
            maxid = Card->m_nID;
        }
        Card->LoadFromFile(Dir + "\\" + IntToDigit(Card->m_nID, 8) + ".txt");
    }

    m_nMaxCardID = maxid + 1;

    //�����N�ǂݍ���
    int linknum = Ini->ReadInteger("Link", "Num", 0);
    for (int i = 0 ; i < linknum ; i++){
        TLink *Link = NewLink();
        Link->Decode(Ini->ReadString("Link", IntToStr(i), ""));
    }

    //���x���ǂݍ���
    int labelnum = Ini->ReadInteger("Label", "Num", -1);
    if (labelnum < 0){
        InitLabel(0);
    }else{
        for (int i = 0 ; i < labelnum ; i++){
            TCardLabel *Label = NewLabel(0);
            Label->Decode_Old(Ini->ReadString("Label", IntToStr(i), ""));
        }
    }

    delete Ini;

    m_bChanged = false;
    return true;
}
//---------------------------------------------------------------------------
bool TDocument::Save_Old()
{
    TIniFile *Ini = new TIniFile(m_FN);

    //�J�[�h��ID��ۑ�
    Ini->WriteInteger("Card", "Num", m_Cards->Count);
    for (int i = 0 ; i < m_Cards->Count ; i++){
        Ini->WriteInteger("Card", IntToStr(i), GetCardByIndex_(i)->m_nID);
    }

    //�����N�ۑ�
    Ini->WriteInteger("Link", "Num", m_Links->Count);
    for (int i = 0 ; i < m_Links->Count ; i++){
        Ini->WriteString("Link", IntToStr(i), GetLinkByIndex(i)->Encode());
    }

    //���x���ۑ�
    Ini->WriteInteger("Label", "Num", m_Labels[0]->Count);
    for (int i = 0 ; i < m_Labels[0]->Count ; i++){
        Ini->WriteString("Label", IntToStr(i), GetLabelByIndex(0, i)->Encode());
    }

    delete Ini;

    //�e�J�[�h�ۑ��p�t�H���_�쐬
    AnsiString Dir = m_FN.SubString(1, m_FN.Length() - ExtractFileExt(m_FN).Length());
    if (!DirectoryExists(Dir)){
        MkDir(Dir);
    }

    //�e�J�[�h��ۑ�
    for (int i = 0 ; i < m_Cards->Count ; i++){
        TCard *Card = GetCardByIndex_(i);
        Card->SaveToFile(Dir + "\\" + IntToDigit(Card->m_nID, 8) + ".txt");
    }

    m_bChanged = false;
    return true;
}
//---------------------------------------------------------------------------
void TDocument::ClearLinks()
{
    for (int i = 0 ; i < m_Links->Count ; i++){
        delete GetLinkByIndex(i);
    }
    m_Links->Clear();
}
//---------------------------------------------------------------------------
void TDocument::AddLabelToLink(TLink *Link, int label)
{
    Link->m_Labels->AddLabel(label);
}
//---------------------------------------------------------------------------
void TDocument::DeleteLabelFromLink(TLink *Link, int label)
{
    Link->m_Labels->DeleteLabel(label);
}
//---------------------------------------------------------------------------
TLink *TDocument::GetLinkByIndex(int index)
{
    if (index >= 0 && index < m_Links->Count){
        return (TLink *)(m_Links->Items[index]);
    }
    return NULL;
}
//---------------------------------------------------------------------------
void TDocument::SetLinkName(TLink *Link, char *szChar)
{
    Link->m_Name = szChar;
}
//---------------------------------------------------------------------------
void TDocument::DeleteLinkByIndex(int index)
{
    delete GetLinkByIndex(index);
    m_Links->Delete(index);
    m_nRefreshLinkCount++;
    m_bChanged = true;
}
//---------------------------------------------------------------------------
void TDocument::RefreshLink()
{
    m_nRefreshLinkCount++;
    m_bChanged = true;
}
//---------------------------------------------------------------------------
TLink *TDocument::NewLink()
{
    TLink *Link = new TLink();
    m_Links->Add(Link);
    RefreshLink();
    return Link;
}
//---------------------------------------------------------------------------
void TDocument::InitLabel(int ltype)
{
    ClearLabels(ltype);

    if (ltype == 0){
        for (int i = 0 ; i < 3 ; i++){
            NewLabel(ltype);
        }

        GetLabelByIndex(0, 0)->m_Name = "Problem";
        GetLabelByIndex(0, 0)->m_nColor = 0xff0000;

        GetLabelByIndex(0, 1)->m_Name = "Solution";
        GetLabelByIndex(0, 1)->m_nColor = 0x00ff00;

        GetLabelByIndex(0, 2)->m_Name = "Result";
        GetLabelByIndex(0, 2)->m_nColor = 0x0000ff;
    }
}
//---------------------------------------------------------------------------
void TDocument::ClearLabels(int ltype)
{
    for (int i = 0 ; i < m_Labels[ltype]->Count ; i++){
        delete GetLabelByIndex(ltype, i);
    }
    m_Labels[ltype]->Clear();
}
//---------------------------------------------------------------------------
bool TDocument::LabelIsFold(TCard *Card)
{
    //���x�����[�v
    bool fold = CountEnableLabel(Card) > 0;
    for (int il = 0 ; il < Card->m_Labels->Count && fold; il++){
        TCardLabel *Label = GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1);
        if (Label->m_bEnable){
            fold &= Label->m_bFold;
        }
    }

    return fold;
}
//---------------------------------------------------------------------------
int TDocument::CountEnableLabel(TCard *Card)
{
    //�J�[�h�̗L���ȃ��x�����𓾂�

    //���x�����[�v
    int count = 0;
    for (int il = 0 ; il < Card->m_Labels->Count; il++){
        TCardLabel *Label = GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1);
        count += Label->m_bEnable;
        /*
        if (Label->m_bEnable){
            count++;
        }
        */
    }

    return count;
}
//---------------------------------------------------------------------------
bool TDocument::LabelIsSame(TCard *Card1, TCard *Card2)
{
    //2�̃J�[�h�̃��x�������������ׂ�

    if (CountEnableLabel(Card1) != CountEnableLabel(Card2)){
        return false;
    }
    for (int i = 0 ; i < Card1->m_Labels->Count ; i++){
        int labelindex = Card1->m_Labels->GetLabel(i);
        TCardLabel *Label = GetLabelByIndex(0, labelindex - 1);
        if (Label->m_bEnable){
            if (!Card2->m_Labels->Contain(labelindex)){
                return false;
            }
        }
    }
    return true;
}
//---------------------------------------------------------------------------
TCardLabel *TDocument::GetLabel(int ltype, AnsiString S)
{
    for (int i = 0 ; i < m_Labels[ltype]->Count ; i++){
        TCardLabel *Label = GetLabelByIndex(ltype, i);
        if (Label->m_Name == S){
            return Label;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
TCardLabel *TDocument::GetLabelByIndex(int ltype, int index)
{
    return (TCardLabel *)(m_Labels[ltype]->Items[index]);
}
//---------------------------------------------------------------------------
void TDocument::SetLabelName(TCardLabel *Label, char *szChar)
{
    Label->m_Name = szChar;
}
//---------------------------------------------------------------------------
void TDocument::DeleteLabelByIndex(int ltype, int index)
{
    if (ltype == 0){
        //�J�[�h���x��
        for (int i = 0 ; i < m_Cards->Count ; i++){
            TCard *Card = GetCardByIndex_(i);

            for (int il = 0 ; il < Card->m_Labels->Count ; il++){
                if (Card->m_Labels->GetLabel(il) > index + 1){
                    //�������郉�x�����傫��Index�̃��x����Index������炷
                    Card->m_Labels->SetLabel(il, Card->m_Labels->GetLabel(il) - 1);
                }else if (Card->m_Labels->GetLabel(il) == index + 1){
                    //�������郉�x���̂��Ă���J�[�h�̃��x�����N���A
                    Card->m_Labels->DeleteLabel(index + 1);
                    il--;
                }
            }
        }
    }else{
        //�����N���x��
        for (int i = 0 ; i < m_Links->Count ; i++){
            TLink *Link = GetLinkByIndex(i);

            for (int il = 0 ; il < Link->m_Labels->Count ; il++){
                if (Link->m_Labels->GetLabel(il) > index + 1){
                    //�������郉�x�����傫��Index�̃��x����Index������炷
                    Link->m_Labels->SetLabel(il, Link->m_Labels->GetLabel(il) - 1);
                }else if (Link->m_Labels->GetLabel(il) == index + 1){
                    //�������郉�x���̂��Ă���J�[�h�̃��x�����N���A
                    Link->m_Labels->DeleteLabel(index + 1);
                    il--;
                }
            }
        }
    }

    delete GetLabelByIndex(ltype, index);
    m_Labels[ltype]->Delete(index);
    m_nRefreshLabelCount++;
    m_bChanged = true;
}
//---------------------------------------------------------------------------
void TDocument::RefreshLabel()
{
    m_nRefreshLabelCount++;
    m_bChanged = true;
}
//---------------------------------------------------------------------------
TCardLabel *TDocument::NewLabel(int ltype)
{
    TCardLabel *Label = new TCardLabel();
    m_Labels[ltype]->Add(Label);
    RefreshLabel();
    return Label;
}
//---------------------------------------------------------------------------
int TDocument::Request(char *Type, int Value, float fValue, void *option)
{
    int result = 1;
    AnsiString T = Type;
    if (T == "Arrange"){
        bReqArrange = Value;
        result = 0;
    }else if (T == "ArrangeMode"){
        nReqArrangeMode = Value;
        result = 0;
    }else if (T == "AutoScroll"){
        bReqAutoScroll = Value;
        result = 0;
    }else if (T == "AutoZoom"){
        bReqAutoZoom = Value;
        result = 0;
    }else if (T == "FullScreen"){
        bReqFullScreen = Value;
        result = 0;
    }else if (T == "Exit"){
        bReqExit = Value;
        result = 0;
    }else if (T == "Zoom"){
        fReqZoom = fValue;
        result = 0;
    }else if (T == "X"){
        fReqX = fValue;
        result = 0;
    }else if (T == "Y"){
        fReqY = fValue;
        result = 0;
    }else if (T == "TargetCard"){
        nReqTargetCard = Value;
        result = 0;
    }else if (T == "DateLimitation"){
        bReqDateLimitation = Value;
        result = 0;
    }else if (T == "LinkLimitation"){
        bReqLinkLimitation = Value;
        result = 0;
    }else if (T == "SizeLimitation"){
        bReqSizeLimitation = Value;
        result = 0;
    }else if (T == "KeyDown"){
        nReqKeyDown = Value;
        result = 0;
    }

    return result;//0�Ő���
}
//---------------------------------------------------------------------------
int TDocument::GetCheckCount()
{
    //1���ʂ��X�V����邽�тɃC���N�������g
    return m_nCheckCount;
}
//---------------------------------------------------------------------------
int TDocument::GetCardID()
{
    //���݃t�H�[�J�X���̃J�[�hID
    return m_nCardID;
}
//---------------------------------------------------------------------------
int TDocument::CardCount()
{
    return m_Cards->Count;
}
//---------------------------------------------------------------------------
int TDocument::LabelCount(int ltype)
{
    return m_Labels[ltype]->Count;
}
//---------------------------------------------------------------------------
int TDocument::LinkCount()
{
    return m_Links->Count;
}
//---------------------------------------------------------------------------
void TDocument::CopyToClipboard()
{
    //�R�s�[�����h�L�������g���쐬
    TDocument *D2 = new TDocument(*this);

    //�I������Ă��Ȃ��J�[�h���폜�i�֘A�J�[�h�������ɍ폜�����j
    for (int i = D2->m_Cards->Count - 1 ; i >= 0 ; i--){
        TCard *Card = D2->GetCardByIndex(i);
        if (!Card->m_bSelected){
            D2->DeleteCard(Card->m_nID);
        }
    }

    //�g�p����Ă��Ȃ��J�[�h���x�����폜
    //�g�p����Ă��郉�x����
    bool *LabelUsed;
    LabelUsed = new bool[D2->m_Labels[0]->Count];
    memset(LabelUsed, 0, sizeof(bool) * D2->m_Labels[0]->Count);
    for (int i = 0 ; i < D2->m_Cards->Count ; i++){
        TCard *Card = D2->GetCardByIndex(i);
        for (int il = 0 ; il < Card->m_Labels->Count ; il++){
            LabelUsed[Card->m_Labels->GetLabel(il) - 1] = true;
        }
    }
    //�g�p����Ă��Ȃ����x���̍폜
    for (int il = D2->m_Labels[0]->Count - 1 ; il >= 0 ; il--){
        if (!LabelUsed[il]){
            D2->DeleteLabelByIndex(0, il);
        }
    }
    delete[] LabelUsed;

    //�g�p����Ă��Ȃ������N���x�����폜
    //�g�p����Ă��郊���N���x����
    LabelUsed = new bool[D2->m_Labels[1]->Count];
    memset(LabelUsed, 0, sizeof(bool) * D2->m_Labels[1]->Count);
    for (int i = 0 ; i < D2->m_Links->Count ; i++){
        TLink *Link = D2->GetLinkByIndex(i);
        for (int il = 0 ; il < Link->m_Labels->Count ; il++){
            LabelUsed[Link->m_Labels->GetLabel(il) - 1] = true;
        }
    }
    //�g�p����Ă��Ȃ����x���̍폜
    for (int il = D2->m_Labels[1]->Count - 1 ; il >= 0 ; il--){
        if (!LabelUsed[il]){
            D2->DeleteLabelByIndex(1, il);
        }
    }
    delete[] LabelUsed;

    //�N���b�v�{�[�h�ɃR�s�[
    TStringList *SL = new TStringList();
    D2->SaveToString(SL);
    Clipboard()->SetTextBuf(SL->Text.c_str());
    delete SL;

    delete D2;
}
//---------------------------------------------------------------------------
void TDocument::PasteFromClipboard(float fSpan)
{
    if (!Clipboard()->HasFormat(CF_TEXT)){
        return;
    }
    TStringList *SL = new TStringList();
    SL->Text = Clipboard()->AsText;
    TDocument *D2 = new TDocument();
    D2->LoadFromString(SL, "");

    //���x���̑Ή������Ȃ���ǉ�
    int *labelassign[2];
    for (int lt = 0 ; lt < 2 ; lt++){
        labelassign[lt] = new int[D2->m_Labels[lt]->Count];
        for (int il = 0 ; il < D2->m_Labels[lt]->Count ; il++){
            TCardLabel *Label = D2->GetLabelByIndex(lt, il);
            bool found = false;
            for (int il2 = 0 ; il2 < m_Labels[lt]->Count && !found ; il2++){
                TCardLabel *Label2 = GetLabelByIndex(lt, il2);

                if (Label->m_Name == Label2->m_Name){
                    labelassign[lt][il] = il2;
                    found = true;
                }
            }
            if (!found){
                m_Labels[lt]->Add(new TCardLabel(*Label));
                labelassign[lt][il] = m_Labels[lt]->Count - 1;
            }
        }
    }

    //�J�[�hID��U��Ȃ����Ȃ���ǉ�
    int *cardassign = new int[D2->m_nMaxCardID];
    for (int ic = 0 ; ic < D2->m_Cards->Count ; ic++){
        TCard *Card = new TCard(*D2->GetCardByIndex(ic));

        cardassign[Card->m_nID] = m_nMaxCardID;
        Card->m_nID = m_nMaxCardID++;

        //���x���u��
        for (int il = 0 ; il < Card->m_Labels->Count ; il++){
            Card->m_Labels->SetLabel(il, labelassign[0][Card->m_Labels->GetLabel(il) - 1] + 1);
        }

        for (int ic2 = 0 ; ic2 < m_Cards->Count ; ic2++){
            TCard *Card2 = GetCardByIndex(ic2);
            if (fabs(Card2->m_fX - Card->m_fX) < 0.0000001f &&
                fabs(Card2->m_fY - Card->m_fY) < 0.0000001f){
                Card->m_fX += fSpan;
                Card->m_fY += fSpan;
                ic2 = -1;
            }
        }

        m_Cards->Add(Card);
    }

    //�����N��ǉ�
    for (int il = 0 ; il < D2->m_Links->Count ; il++){
        TLink *Link = new TLink(*D2->GetLinkByIndex(il));
        Link->m_nFromID = cardassign[Link->m_nFromID];
        Link->m_nDestID = cardassign[Link->m_nDestID];

        //���x���u��
        for (int il = 0 ; il < Link->m_Labels->Count ; il++){
            Link->m_Labels->SetLabel(il, labelassign[1][Link->m_Labels->GetLabel(il) - 1] + 1);
        }

        m_Links->Add(Link);
    }

    //�j��
    delete[] cardassign;
    for (int lt = 0 ; lt < 2 ; lt++){
        delete[] labelassign[lt];
    }

    delete D2;
    delete SL;

    RefreshList();
    RefreshLink();
    RefreshLabel();
}
//---------------------------------------------------------------------------
extern int bReqArrange;//�A�����W��ON/OFF
extern int nReqArrangeMode;//0���珇��Repulsion�ALink�ALabel�AIndex
extern int bReqAutoScroll;//�I�[�g�X�N���[��
extern int bReqAutoZoom;//�I�[�g�Y�[��
extern int bReqFullScreen;//�t���X�N���[��
extern int bReqExit;//�I��
extern float fReqZoom, fReqX, fReqY;//�Y�[���ƍ��W

#pragma package(smart_init)

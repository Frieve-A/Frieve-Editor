// ---------------------------------------------------------------------------

#ifndef documentH
#define documentH
// ---------------------------------------------------------------------------
#include <vcl.h>
// ---------------------------------------------------------------------------
#include "card.h"
#include "label.h"
// ---------------------------------------------------------------------------
/*
 help�Ɉȉ���ǉ�

 ReadOnly=1
 DefaultView=1
 */

#define AppTitle "Frieve Editor"
#define AppVersion 140
#define FileVersion 007
#define AppURL "http://www.frieve.com/"
#define SoftURL "http://www.frieve.com/feditor/"
#define InfoURL "http://www.frieve.com/feditor/info.html"

class IDocCtrl {
public:
	virtual int Request(char *Type, int Value, float fValue, void *option) = 0;
	virtual TCard *NewCard(int insertindex) = 0;
	virtual void SortCards(int sorttype, bool inverse) = 0;
	virtual TCard* GetCardByIndex(int nIndex) = 0;
	virtual TCard* GetCard(int nID) = 0;
	virtual void SetCardTitle(TCard *Card, UnicodeString S) = 0;
	virtual void SetCardText(TCard *Card, UnicodeString S) = 0;
	// virtual void SetCardText(int nID, UnicodeString S) = 0;
	virtual void DeleteCard(int nID) = 0;
	virtual int SearchCardIndex(int nID) = 0;
	virtual void ClearCards() = 0;
	virtual void ClearCardSelection() = 0;
	virtual void SwapCard(int idx1, int idx2) = 0;
	virtual int SearchParent(int CardID, bool bChild, bool bFocus) = 0;
	// �e�i�������͎q�j�J�[�h��{��
	virtual int SearchBrother(int CurrentID, int ParentID, bool bInverse,
		bool bChild, bool bFocus) = 0; // ���v���i�������͋t����j�ɌZ��m�[�h��T��
	virtual int SearchLast(int CardID, bool bFocus) = 0; // ���O�ɕ\�����Ă����J�[�h��{��
	virtual void RefreshCardLevel() = 0; // Card->m_bTop�ƃ����N�ɏ]���ĊK�w���x���ݒ�A�J�[�h�\�[�g

	virtual void AddLabelToCard(TCard* Card, int label) = 0;
	virtual void DeleteLabelFromCard(TCard* Card, int label) = 0;
	// �f�[�^�A�N�Z�X�i�����N�j
	virtual TLink *NewLink() = 0;
	virtual TLink *GetLinkByIndex(int index) = 0;
	virtual void SetLinkName(TLink *Link, UnicodeString S) = 0;
	virtual void DeleteLinkByIndex(int index) = 0;
	virtual void ClearLinks() = 0;

	virtual void AddLabelToLink(TLink* Link, int label) = 0;
	virtual void DeleteLabelFromLink(TLink* Link, int label) = 0;

	// �f�[�^�A�N�Z�X�i���x���j
	virtual TCardLabel *NewLabel(int ltype) = 0;
	virtual TCardLabel *GetLabelByIndex(int ltype, int index) = 0;
	virtual TCardLabel *GetLabel(int ltype, UnicodeString S) = 0;
	virtual void SetLabelName(TCardLabel *Label, UnicodeString S) = 0;
	virtual void DeleteLabelByIndex(int ltype, int index) = 0;
	virtual void InitLabel(int ltype) = 0;
	virtual void ClearLabels(int ltype) = 0;

	virtual bool LabelIsFold(TCard *Card) = 0; // �J�[�h�̃��x�����S�Đ܂肽���܂�Ă��邩���ׂ�
	virtual int CountEnableLabel(TCard *Card) = 0; // �J�[�h�̗L���ȃ��x�����𓾂�
	virtual bool LabelIsSame(TCard *Card1, TCard *Card2) = 0;
	// 2�̃J�[�h�̃��x�������������ׂ�
	// �\���X�V�p
	virtual void RefreshList() = 0;
	virtual void RefreshLink() = 0;
	virtual void RefreshLabel() = 0;
	// �t�@�C��
	virtual bool Load(UnicodeString FN, bool bSoftLoad = false) = 0;
	// SoftLoad�ł́A���f�[�^�̍��W���ێ�
	virtual bool Save() = 0;
	virtual bool LoadFromString(TStringList *SL, UnicodeString FN) = 0;
	virtual bool SoftLoadFromString(TStringList *SL, UnicodeString FN) = 0;
	virtual bool SaveToString(TStringList *SL) = 0;
	// ���擾
	virtual int GetCheckCount() = 0; // 1���ʂ��X�V����邽�тɃC���N�������g
	virtual int GetCardID() = 0; // ���݃t�H�[�J�X���̃J�[�hID
	virtual int CardCount() = 0; // �J�[�h��
	virtual int LabelCount(int ltype) = 0; // ���x����
	virtual int LinkCount() = 0; // �����N��
	// �N���b�v�{�[�h
	virtual void CopyToClipboard() = 0;
	virtual void PasteFromClipboard(float fSpan) = 0;
};

class TDocument : public IDocCtrl {
public:
	// �쐬�A�j��
	void InitDocument();
	TDocument();
	void CopyFrom(TDocument *Doc);
	TDocument(TDocument &Doc);
	virtual ~TDocument();

	// �R�s�[�Ώۃf�[�^
	// �f�[�^
	TList *m_Cards;

	// �����N�f�[�^
	TList *m_Links;

	// ���x���f�[�^
	TList *m_Labels[2];

	// ��ʍX�V�J�E���^
	int m_nCheckCount;
	// �\���X�V�p
	int m_nRefreshListCount;
	int m_nRefreshLinkCount;
	int m_nRefreshLabelCount;

	// ���̑�
	bool m_bChanged;
	UnicodeString m_FN;
	bool m_bReadOnly;

	int m_nCardID; // ���ݕ\�����̃J�[�h�i�ۑ��p�j
	int m_nDefaultView; // �ǂݍ��񂾏u�Ԃǂ̉�ʂ�\�����邩�i-1=�Ȃ��A0=Browser�A1=Editor�j
	// �f�[�^�A�N�Z�X
	int m_nMaxCardID;
	int* m_CardIDToIndex;

	void CreateCardIDToIndex();
	void FreeCardIDToIndex();

	TCard *NewCard(int insertindex);
	void SortCards(int sorttype, bool inverse);
	TCard* GetCardByIndex(int nIndex);
	TCard* GetCard(int nID);
	void SetCardTitle(TCard *Card, UnicodeString S);
	void SetCardText(TCard *Card, UnicodeString S);
	// void SetCardText(int nID, char *szChar);
	void DeleteCard(int nID);
	int SearchCardIndex(int nID);
	void ClearCards();
	void ClearCardSelection();
	void SwapCard(int idx1, int idx2);
	void RefreshDateOrder(); // m_fCreatedOrder�Ȃǂɐ������l����
	void RefreshDateOrder_Label(); // m_fTouchedOrder�Ȃǂɐ������l����
	int SearchParent(int CardID, bool bChild, bool bFocus); // �e�i�������͎q�j�J�[�h��{��
	int SearchBrother(int CurrentID, int ParentID, bool bInverse, bool bChild,
		bool bFocus); // ���v���i�������͋t����j�ɌZ��m�[�h��T��
	int SearchLast(int CardID, bool bFocus); // ���O�ɕ\�����Ă����J�[�h��{��
	void RefreshCardLevel(); // Card->m_bTop�ƃ����N�ɏ]���ĊK�w���x���ݒ�A�J�[�h�\�[�g

	void AddLabelToCard(TCard* Card, int label);
	void DeleteLabelFromCard(TCard* Card, int label);

	TList *GetRelatedCard(bool bInverse, bool bVisibleOnly);
	// �e�J�[�h���璣��ꂽ�����N��̃J�[�h��Index���ɓ����
	int RelatedCardNum(TList *RCard, int cardindex);
	int RelatedIndex(TList *RCard, int cardindex, int index);
	void FreeRelatedCard(TList *RCard); // RCard�̔j��

private:
	inline TCard* GetCardByIndex_(int nIndex);

public:
	// �f�[�^�A�N�Z�X�i�����N�j
	TLink *NewLink();
	inline TLink *GetLinkByIndex(int index);
	void SetLinkName(TLink *Link, UnicodeString S);
	void DeleteLinkByIndex(int index);
	void ClearLinks();

	void AddLabelToLink(TLink* Link, int label);
	void DeleteLabelFromLink(TLink* Link, int label);

	// �f�[�^�A�N�Z�X�i���x���j
	TCardLabel *NewLabel(int ltype);
	inline TCardLabel *GetLabelByIndex(int ltype, int index);
	TCardLabel *GetLabel(int ltype, UnicodeString S);
	void SetLabelName(TCardLabel *Label, UnicodeString S);
	void DeleteLabelByIndex(int ltype, int index);
	void InitLabel(int ltype);
	void ClearLabels(int ltype);

	bool LabelIsFold(TCard *Card); // �J�[�h�̃��x�����S�Đ܂肽���܂�Ă��邩���ׂ�
	int CountEnableLabel(TCard *Card); // �J�[�h�̗L���ȃ��x�����𓾂�
	bool LabelIsSame(TCard *Card1, TCard *Card2); // 2�̃J�[�h�̃��x�������������ׂ�

	// �\���X�V�p
	void RefreshList();
	void RefreshLink();
	void RefreshLabel();
	// �t�@�C��
	bool Load(UnicodeString FN, bool bSoftLoad = false);
	// SoftLoad�ł́A���f�[�^�̍��W���ێ�
	bool Save();
	bool LoadFromString(TStringList *SL, UnicodeString FN);
	bool SoftLoadFromString(TStringList *SL, UnicodeString FN);
	bool SaveToString(TStringList *SL);

	// �t�H���_�������Ă������̌݊��p
	bool Load_Old(UnicodeString FN);
	bool Save_Old();
	int Request(char *Type, int Value, float fValue, void *option);

	// ���擾
	int GetCheckCount(); // 1���ʂ��X�V����邽�тɃC���N�������g
	int GetCardID(); // ���݃t�H�[�J�X���̃J�[�hID
	int CardCount(); // �J�[�h��
	int LabelCount(int ltype); // ���x����
	int LinkCount(); // �����N��
	// �N���b�v�{�[�h
	void CopyToClipboard();
	void PasteFromClipboard(float fSpan);
};

class TUndoRedoData {
public:
	UnicodeString m_Name;
	TDocument *m_Doc;
	int m_nCardID; // �ҏW���̃J�[�hID
	int m_nSelStart; // �G�f�B�^��̃J�[�\���ʒu
	int m_nSelLength;

	TUndoRedoData(TDocument *Doc, UnicodeString Name, int m_nCardID = -1,
		int SelStart = 0, int SelLength = 0);
	virtual ~TUndoRedoData();
};

class TUndoRedo {
private:
	int m_nMaxUndo;
	TList *m_Undos;
	TList *m_Redos;

	inline TUndoRedoData *UndoData(int index);
	inline TUndoRedoData *RedoData(int index);

public:
	TUndoRedo(int maxundo);
	virtual ~TUndoRedo();
	// Undo�ARedo
	void ClearUndos();
	void ClearRedos();
	void Backup(TDocument *Doc, UnicodeString editname, int CardID = -1,
		int SelStart = 0, int SelLength = 0);
	void Undo(TDocument *Doc, int CardID, int SelStart, int SelLength,
		int *NextCardID, int *NextSelStart, int *NextSelLength);
	void Redo(TDocument *Doc, int CardID, int SelStart, int SelLength,
		int *NextCardID, int *NextSelStart, int *NextSelLength);
	bool GetCanUndo(UnicodeString &editname);
	bool GetCanRedo(UnicodeString &editname);

	int m_bChanged; // Undo�ARedo���s��ꂽ���Ƃ�����
};

// ---------------------------------------------------------------------------
// �h�L�������g����̑��샊�N�G�X�g(-1�Ń��N�G�X�g�Ȃ�)
extern int bReqArrange; // �A�����W��ON/OFF
extern int nReqArrangeMode; // 0���珇��Repulsion�ALink�ALabel�AIndex
extern int bReqAutoScroll; // �I�[�g�X�N���[��
extern int bReqAutoZoom; // �I�[�g�Y�[��
extern int bReqFullScreen; // �t���X�N���[��
extern int bReqExit; // �I��
extern float fReqZoom, fReqX, fReqY; // �Y�[���ƍ��W
extern int nReqTargetCard; // �^�[�Q�b�g�J�[�h�ύX
extern int bReqSizeLimitation, bReqLinkLimitation, bReqDateLimitation;
// �\��������ON/OFF
extern int nReqSizeLimitation;
extern int nReqLinkLimitation, bReqLinkDirection, bReqLinkBackward,
	nReqLinkTarget;
extern int nReqDateLimitation, ReqDateLimitationDateType, ReqDateLimitationType;
extern int nReqKeyDown; // ����̃L�[������������������
#endif

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
 Add to help:

 ReadOnly=1
 DefaultView=1
 */

#define AppTitle "Frieve Editor"
#define AppVersion 200
#define FileVersion 8
#define AppURL "https://www.frieve.com/"
#define SoftURL "https://www.frieve.com/software/frieve-editor"
#define ReleaseURL "https://github.com/Frieve-A/Frieve-Editor/releases"

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
	// Search parent (or child) card
	virtual int SearchBrother(int CurrentID, int ParentID, bool bInverse,
		bool bChild, bool bFocus) = 0; // Find sibling nodes clockwise (or counter)
	virtual int SearchLast(int CardID, bool bFocus) = 0; // Find previously displayed card
	virtual void RefreshCardLevel() = 0; // Set hierarchy level from Card->m_bTop and links, sort cards

	virtual void AddLabelToCard(TCard* Card, int label) = 0;
	virtual void DeleteLabelFromCard(TCard* Card, int label) = 0;
	// Data access (links)
	virtual TLink *NewLink() = 0;
	virtual TLink *GetLinkByIndex(int index) = 0;
	virtual void SetLinkName(TLink *Link, UnicodeString S) = 0;
	virtual void DeleteLinkByIndex(int index) = 0;
	virtual void ClearLinks() = 0;

	virtual void AddLabelToLink(TLink* Link, int label) = 0;
	virtual void DeleteLabelFromLink(TLink* Link, int label) = 0;

	// Data access (labels)
	virtual TCardLabel *NewLabel(int ltype) = 0;
	virtual TCardLabel *GetLabelByIndex(int ltype, int index) = 0;
	virtual TCardLabel *GetLabel(int ltype, UnicodeString S) = 0;
	virtual void SetLabelName(TCardLabel *Label, UnicodeString S) = 0;
	virtual void DeleteLabelByIndex(int ltype, int index) = 0;
	virtual void InitLabel(int ltype) = 0;
	virtual void ClearLabels(int ltype) = 0;

	virtual bool LabelIsFold(TCard *Card) = 0; // Check if all labels are folded
	virtual int CountEnableLabel(TCard *Card) = 0; // Get enabled label count
	virtual bool LabelIsSame(TCard *Card1, TCard *Card2) = 0;
	// Check if two cards have same labels
	// Display refresh
	virtual void RefreshList() = 0;
	virtual void RefreshLink() = 0;
	virtual void RefreshLabel() = 0;
	// File
	virtual bool Load(UnicodeString FN, bool bSoftLoad = false) = 0;
	// SoftLoad preserves original coordinates
	virtual bool Save() = 0;
	virtual bool LoadFromString(TStringList *SL, UnicodeString FN) = 0;
	virtual bool SoftLoadFromString(TStringList *SL, UnicodeString FN) = 0;
	virtual bool SaveToString(TStringList *SL) = 0;
	// Info access
	virtual int GetCheckCount() = 0; // Incremented each display refresh
	virtual int GetCardID() = 0; // Currently focused card ID
	virtual int CardCount() = 0; // Card count
	virtual int LabelCount(int ltype) = 0; // Label count
	virtual int LinkCount() = 0; // Link count
	// Clipboard
	virtual void CopyToClipboard() = 0;
	virtual void PasteFromClipboard(float fSpan) = 0;
};

class TDocument : public IDocCtrl {
public:
	// Create, destroy
	void InitDocument();
	TDocument();
	void CopyFrom(TDocument *Doc);
	TDocument(TDocument &Doc);
	virtual ~TDocument();

	// Copy target data
	// Data
	TList *m_Cards;

	// Link data
	TList *m_Links;

	// Label data
	TList *m_Labels[2];

	// Display update counter
	int m_nCheckCount;
	// Display refresh
	int m_nRefreshListCount;
	int m_nRefreshLinkCount;
	int m_nRefreshLabelCount;

	// Others
	bool m_bChanged;
	UnicodeString m_FN;
	bool m_bReadOnly;

	int m_nCardID; // Currently displayed card (for save)
	int m_nDefaultView; // Which view on load (-1=none, 0=Browser, 1=Editor)
	// Data access
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
	void RefreshDateOrder(); // Assign correct values to m_fCreatedOrder etc
	void RefreshDateOrder_Label(); // Assign correct values to m_fTouchedOrder etc
	int SearchParent(int CardID, bool bChild, bool bFocus); // Search parent (or child) card
	int SearchBrother(int CurrentID, int ParentID, bool bInverse, bool bChild,
		bool bFocus); // Find sibling nodes clockwise (or counter)
	int SearchLast(int CardID, bool bFocus); // Find previously displayed card
	void RefreshCardLevel(); // Set hierarchy level from Card->m_bTop and links, sort cards

	void AddLabelToCard(TCard* Card, int label);
	void DeleteLabelFromCard(TCard* Card, int label);

	TList *GetRelatedCard(bool bInverse, bool bVisibleOnly);
	// Put link destination cards in Index order from each card
	int RelatedCardNum(TList *RCard, int cardindex);
	int RelatedIndex(TList *RCard, int cardindex, int index);
	void FreeRelatedCard(TList *RCard); // Free RCard

private:
	inline TCard* GetCardByIndex_(int nIndex);

public:
	// Data access (links)
	TLink *NewLink();
	inline TLink *GetLinkByIndex(int index);
	void SetLinkName(TLink *Link, UnicodeString S);
	void DeleteLinkByIndex(int index);
	void ClearLinks();

	void AddLabelToLink(TLink* Link, int label);
	void DeleteLabelFromLink(TLink* Link, int label);

	// Data access (labels)
	TCardLabel *NewLabel(int ltype);
	inline TCardLabel *GetLabelByIndex(int ltype, int index);
	TCardLabel *GetLabel(int ltype, UnicodeString S);
	void SetLabelName(TCardLabel *Label, UnicodeString S);
	void DeleteLabelByIndex(int ltype, int index);
	void InitLabel(int ltype);
	void ClearLabels(int ltype);

	bool LabelIsFold(TCard *Card); // Check if all labels are folded
	int CountEnableLabel(TCard *Card); // Get enabled label count
	bool LabelIsSame(TCard *Card1, TCard *Card2); // Check if two cards have same labels

	// Display refresh
	void RefreshList();
	void RefreshLink();
	void RefreshLabel();
	// File
	bool Load(UnicodeString FN, bool bSoftLoad = false);
	// SoftLoad preserves original coordinates
	bool Save();
	bool LoadFromString(TStringList *SL, UnicodeString FN);
	bool SoftLoadFromString(TStringList *SL, UnicodeString FN);
	bool SaveToString(TStringList *SL);

	// Compatibility for old folder structure
	bool Load_Old(UnicodeString FN);
	bool Save_Old();
	int Request(char *Type, int Value, float fValue, void *option);

	// Info access
	int GetCheckCount(); // Incremented each display refresh
	int GetCardID(); // Currently focused card ID
	int CardCount(); // Card count
	int LabelCount(int ltype); // Label count
	int LinkCount(); // Link count
	// Clipboard
	void CopyToClipboard();
	void PasteFromClipboard(float fSpan);
};

class TUndoRedoData {
public:
	UnicodeString m_Name;
	TDocument *m_Doc;
	int m_nCardID; // Card ID being edited
	int m_nSelStart; // Cursor position in editor
	int m_nSelLength;

	TUndoRedoData(TDocument *Doc, UnicodeString Name, int m_nCardID = -1,
		int SelStart = 0, int SelLength = 0);
	virtual ~TUndoRedoData();
private:
	TUndoRedoData(const TUndoRedoData &source);
	TUndoRedoData& operator=(const TUndoRedoData &source);
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
	// Undo, Redo
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

	int m_bChanged; // Indicates Undo/Redo was performed
private:
	TUndoRedo(const TUndoRedo &source);
	TUndoRedo& operator=(const TUndoRedo &source);
};

// ---------------------------------------------------------------------------
// Document operation requests (-1 = no request)
extern int bReqArrange; // Arrange ON/OFF
extern int nReqArrangeMode; // 0=Repulsion, Link, Label, Index
extern int bReqAutoScroll; // Auto scroll
extern int bReqAutoZoom; // Auto zoom
extern int bReqFullScreen; // Full screen
extern int bReqExit; // Exit
extern float fReqZoom, fReqX, fReqY; // Zoom and position
extern int nReqTargetCard; // Target card change
extern int bReqSizeLimitation, bReqLinkLimitation, bReqDateLimitation;
// Display limit ON/OFF
extern int nReqSizeLimitation;
extern int nReqLinkLimitation, bReqLinkDirection, bReqLinkBackward,
	nReqLinkTarget;
extern int nReqDateLimitation, ReqDateLimitationDateType, ReqDateLimitationType;
extern int nReqKeyDown; // Process specified key press
#endif

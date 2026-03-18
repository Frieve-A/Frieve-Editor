// ---------------------------------------------------------------------------

#ifndef cardH
#define cardH
// ---------------------------------------------------------------------------
#include <vcl.h>
// ---------------------------------------------------------------------------
#include "link.h"
#include "label.h"
#include "drawing.h"
// ---------------------------------------------------------------------------
WideString EncodeES(WideString S); // Encode escape sequence (13->\\n, \->\\\\)
WideString DecodeES(WideString S, WideString Ret = "\r\n");
// Decode escape sequence (13<-\\n, \<-\\\\)

class TCard {
public:
	// When adding data, also edit initialization and copy constructor

	// Data
	int m_nID;
	TLabelList *m_Labels; // Labels
	UnicodeString m_Title; // Title
	TStringList *m_Lines; // Body text
	TDrawing *m_Drawing; // Drawing
	int m_bVisible; // Visible, hidden
	int m_nShape; // Shape
	int m_nSize; // Size (100-based ratio)
	int m_bTop; // Top of hierarchy. false by default
	int m_bFixed; // Position fixed
	int m_bFold; // Folded. false by default

	// Not synced in network share (edit Document->SoftLoad when adding)
	double m_fX, m_fY; // Coordinates
	double m_fCreated, m_fUpdated, m_fViewed; // Dates
	// Not persisted
	int m_bGetFocus; // Whether to gain focus on cursor move

	int m_nLevel; // Hierarchy level

	int m_nCreatedOrder, m_nUpdatedOrder, m_nViewedOrder;
	// Date normalized to 0.0~100.0 by sort order
	double m_fScore; // Score
	UnicodeString m_ImageFN; // First image file in body
	UnicodeString m_VideoFN; // First video file in body

	int m_nMatrixX, m_nMatrixY; // Coordinates when arranged in grid
	float m_fMatrixSpeed; // Speed between grid cells (~1.0)

	int m_bLabelIsFold; // Whether all labels of this card are folded
	TColor m_Color; // Color for drawing

	float m_fTickerPos; // Ticker: dots scrolled from right edge

	int m_bVisibleBak; // Backup of Visible (detect Visible changes)

	int m_bSelected;
	int m_nSelected; // For selection animation (0=unselected, 100=selected)

	int m_nParentID; // Parent card ID in hierarchy view
	int m_bHasChild; // Whether has child cards in hierarchy view

	int m_nTmp; // Temporary. Beware of reuse

	// Construction, destruction
	TCard();
	TCard(TCard &Card);
	virtual ~TCard();
	// Util
	void CheckImageFN(); // Check image/video file names referenced in body
	// Load/Save
	void LoadFromString(TStringList *SL, int &line, int version);
	void SaveToString(TStringList *SL);

	// Compatibility
	void LoadFromFile(UnicodeString FN);
	void SaveToFile(UnicodeString FN);

private:
	TCard& operator=(const TCard &source);
};
// ---------------------------------------------------------------------------
#endif

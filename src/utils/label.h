// ---------------------------------------------------------------------------

#ifndef labelH
#define labelH
// ---------------------------------------------------------------------------
#include <vcl.h>
// ---------------------------------------------------------------------------
#define MAXLABELS 16

// ---------------------------------------------------------------------------
class TCardLabel {
public:
	// Label
	UnicodeString m_Name;
	int m_nColor;

	int m_nSize; // Size

	// Update Document->SoftLoad when adding fields
	int m_bEnable; // Enable
	int m_bShow; // Show
	int m_bHide; // Hide
	int m_bFold; // Fold
	// Coordinates
	float m_fX, m_fY; // Label position
	int m_bFixed; // Position fixed

	double m_fTouched; // Last touched datetime
	int m_nTouchedOrder; // Touch order

	// Constructor
	TCardLabel();
	TCardLabel(UnicodeString S);
	void InitLabel();
	// TCardLabel(TCardLabel &Label);
	virtual ~TCardLabel();
	// Read/write
	void Decode(UnicodeString S);
	void Decode_006(UnicodeString S); // Before backward compatibility
	void Decode_003(UnicodeString S); // Before Enabled addition
	void Decode_002(UnicodeString S); // Before Size addition
	void Decode_001(UnicodeString S);
	void Decode_Old(UnicodeString S);
	UnicodeString Encode();
};

// ---------------------------------------------------------------------------
class TLabelList : public TList {
public:
	// Accessors
	int GetLabel(int index);
	void SetLabel(int index, int labelid);
	void AddLabel(int labelid);
	void DeleteLabel(int labelid);
	bool Contain(int labelid);
	// Read/write
	UnicodeString Encode();
	void Decode(UnicodeString S);
};
// ---------------------------------------------------------------------------
#endif

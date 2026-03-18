// ---------------------------------------------------------------------------

#ifndef linkH
#define linkH
// ---------------------------------------------------------------------------
#include "label.h"
#include <vcl.h>

// ---------------------------------------------------------------------------
class TLink {
public:
  // Data
  int m_nFromID;        // Link source
  int m_nDestID;        // Link destination
  int m_bDirection;     // Direction visibility
  int m_nShape;         // Direction
  TLabelList *m_Labels; // Labels
  UnicodeString m_Name; // Link name

  // Transient
  bool m_bVisible;
  TColor m_Color;

  // Construction, destruction
  void InitLink();
  TLink();
  TLink(UnicodeString S);
  TLink(TLink &Link);
  virtual ~TLink();
  // Read/write
  void Decode(UnicodeString S);
  void Decode_005(UnicodeString S); // Before LinkShape addition
  void Decode_006(UnicodeString S); // Before backward compatibility
  UnicodeString Encode();
};
#endif

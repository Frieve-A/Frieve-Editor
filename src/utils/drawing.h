// ---------------------------------------------------------------------------

#ifndef drawingH
#define drawingH
// ---------------------------------------------------------------------------
#include <vcl.h>

// ---------------------------------------------------------------------------
class TDrawingItem : public TList {
public:
  TDrawingItem();
  TDrawingItem(TDrawingItem &Source);
  TDrawingItem(UnicodeString S); // Decode from string
  __fastcall virtual ~TDrawingItem();

  UnicodeString Encode(); // Encode to string

private:
  void UpdateFreeHandRect(float x, float y);
  void FinishFreeHandRect();

  float m_fDragStartX, m_fDragStartY; // Drag start coordinates
  float m_DragRect[4]; // Drawing coords, used temporarily during drag

public:
  int m_nType; // Element type: 1=FreeHand, 2=Line, 3=Rect, 4=Circle, 5=Text
  TColor m_nPenColor;   // Line color. 0x7fffffff=FGColor, -1=none
  TColor m_nBrushColor; // Brush color
  float m_Rect[4]; // Drawing coords. FreeHand: bounding rect. Line: start/end.
                   // Text: top-left

  bool m_bSelected; // Selected
  bool m_bExist;    // Visible/displayable

  void SetType(int nType);
  bool MouseDown(float x, float y); // Returns true if edited
  bool MouseMove(float x, float y);
  bool MouseUp(float x, float y);

  int CheckDrag(float x, float y, int size); // Check if drag can start
  bool Drag(float x, float y, int dragmode);

  void Draw(TCanvas *C, TRect &R);

  int SetPenColor(TColor C); // Change color. Returns true on success
  int SetBrushColor(TColor C);
};

// ---------------------------------------------------------------------------
class TDrawing : public TList {
public:
  TDrawing();
  TDrawing(TDrawing &Source); // Copy constructor
  TDrawing(UnicodeString S);  // Decode from string
  __fastcall virtual ~TDrawing();

  UnicodeString Encode(bool bAll = false);
  // Encode to string. bAll=false: selected only. bAll=true: all items

private:
  TDrawingItem *m_TmpItem;

  void FreeTmpItem();
  TDrawingItem *DItem(int index);

public:
  int m_nTool; // Current tool: 0=Select, 1=FreeHand, 2=Line, 3=Rect, 4=Circle,
               // 5=Text
  bool m_bDrawRequest;
  int m_nDragMode;          // 0x10~0x13=corner drag, 0x20=move
  int m_nDragIndex;         // Item index being corner-dragged
  float m_fDragX, m_fDragY; // Drag start coordinates
  TColor m_nPenColor;       // Line color. 0x7fffffff=FGColor, -1=none
  TColor m_nBrushColor;     // Brush color

  bool m_bModified; // Edited

  void SetTool(int nToolID);
  void MouseDown(float x, float y, int size, TShiftState Shift);
  void MouseMove(float x, float y);
  void MouseUp(float x, float y);

  void Draw(TCanvas *C, TRect &R);

  void SelectRect(float x1, float y1, float x2, float y2);
  void ClearSelection();
  void SelectAll();
  int Selected(); // Return count of selected elements

  // Edit operations
  void Cut();
  void Copy();
  void Paste();
  void DDelete();

  void ChangePenColor(TColor C);
  void ChangeBrushColor(TColor C);
};
// ---------------------------------------------------------------------------
#endif

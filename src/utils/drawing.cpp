// ---------------------------------------------------------------------------

#pragma hdrstop

#include "drawing.h"
#include <clipbrd.hpp>
#include <math.h>

// ---------------------------------------------------------------------------
TDrawingItem::TDrawingItem()
    : TList(), m_nType(0), m_bSelected(false), m_bExist(false),
      m_nPenColor(TColor(0x7fffffff)), m_nBrushColor(TColor(-1)) {
  m_Rect[0] = 0.0f;
  m_Rect[1] = 0.0f;
  m_Rect[2] = 0.0f;
  m_Rect[3] = 0.0f;
}

// ---------------------------------------------------------------------------
TDrawingItem::TDrawingItem(TDrawingItem &Source)
    : TList(), m_nType(Source.m_nType), m_bSelected(Source.m_bSelected),
      m_bExist(Source.m_bExist), m_nPenColor(Source.m_nPenColor),
      m_nBrushColor(Source.m_nBrushColor) {
  memcpy(m_Rect, Source.m_Rect, sizeof(m_Rect));
  for (int i = 0; i < Source.Count; i++) {
    Add(Source.Items[i]);
  }
}

// ---------------------------------------------------------------------------
__fastcall TDrawingItem::~TDrawingItem() {}

// ---------------------------------------------------------------------------
TDrawingItem::TDrawingItem(UnicodeString S)
    : TList(), m_bSelected(true), m_bExist(true) {
  // Decode from string
  m_nType = StrToIntDef(UnicodeString("0x") + S.SubString(1, 8), 0);
  m_nPenColor = (TColor)StrToIntDef(UnicodeString("0x") + S.SubString(9, 8), 0);
  m_nBrushColor =
      (TColor)StrToIntDef(UnicodeString("0x") + S.SubString(17, 8), 0);
  int count = StrToIntDef(UnicodeString("0x") + S.SubString(25, 8), 0);

  int pos = 33;
  for (int i = 0; i < 4; i++) {
    int l = StrToIntDef(UnicodeString("0x") + S.SubString(pos, 8), 0);
    pos += 8;
    m_Rect[i] = *(float *)&l;
  }
  for (int i = 0; i < count; i++) {
    Add((void *)(intptr_t)StrToIntDef(UnicodeString("0x") + S.SubString(pos, 8), 0));
    pos += 8;
  }
}

// ---------------------------------------------------------------------------
UnicodeString TDrawingItem::Encode() {
  // Encode to string
  UnicodeString result;

  result += IntToHex(m_nType, 8) + IntToHex(m_nPenColor, 8) +
            IntToHex(m_nBrushColor, 8) + IntToHex(Count, 8);
  for (int i = 0; i < 4; i++) {
    result += IntToHex(*(int *)&m_Rect[i], 8);
  }
  for (int i = 0; i < Count; i++) {
    result += IntToHex((int)(intptr_t)Items[i], 8);
  }
  return result;
}

// ---------------------------------------------------------------------------
void TDrawingItem::UpdateFreeHandRect(float x, float y) {
  int ix = *(int *)&x, iy = *(int *)&y;
  Add((void *)(intptr_t)ix);
  Add((void *)(intptr_t)iy);
  m_Rect[0] = m_Rect[1] = 0.0f;
  m_Rect[2] = m_Rect[3] = 1.0f;
}

// ---------------------------------------------------------------------------
void TDrawingItem::FinishFreeHandRect() {
  for (int i = 0; i < Count / 2; i++) {
    int ix = (int)(intptr_t)Items[i * 2];
    float x = *(float *)&ix;
    int iy = (int)(intptr_t)Items[i * 2 + 1];
    float y = *(float *)&iy;
    if (i == 0) {
      // First time
      m_Rect[0] = m_Rect[2] = x;
      m_Rect[1] = m_Rect[3] = y;
    } else {
      // Update rect
      if (m_Rect[0] > x) {
        m_Rect[0] = x;
      }
      if (m_Rect[1] > y) {
        m_Rect[1] = y;
      }
      if (m_Rect[2] < x) {
        m_Rect[2] = x;
      }
      if (m_Rect[3] < y) {
        m_Rect[3] = y;
      }
    }
  }

  // Normalize
  float w = m_Rect[2] - m_Rect[0];
  float h = m_Rect[3] - m_Rect[1];
  if (w == 0.0f) {
    w = 1.0f;
  }
  if (h == 0.0f) {
    h = 1.0f;
  }

  for (int i = 0; i < Count / 2; i++) {
    int ix = (int)(intptr_t)Items[i * 2];
    float x = *(float *)&ix;
    int iy = (int)(intptr_t)Items[i * 2 + 1];
    float y = *(float *)&iy;

    x = (x - m_Rect[0]) / w;
    y = (y - m_Rect[1]) / h;

    ix = *(int *)&x;
    iy = *(int *)&y;
    Items[i * 2] = (void *)(intptr_t)ix;
    Items[i * 2 + 1] = (void *)(intptr_t)iy;
  }
}

// ---------------------------------------------------------------------------
void TDrawingItem::SetType(int nType) { m_nType = nType; }

// ---------------------------------------------------------------------------
bool TDrawingItem::MouseDown(float x, float y) {
  bool result = false; // Not edited

  switch (m_nType) {
  case 1: // FreeHand
    UpdateFreeHandRect(x, y);
    break;
  default:
    m_Rect[0] = x;
    m_Rect[1] = y;
    result = true;
    break;
  }

  return result;
}

// ---------------------------------------------------------------------------
bool TDrawingItem::MouseMove(float x, float y) {
  bool result = false; // Not edited

  switch (m_nType) {
  case 1: // FreeHand
    UpdateFreeHandRect(x, y);
    break;
  default:
    m_Rect[2] = x;
    m_Rect[3] = y;
    result = true;
    break;
  }
  m_bExist = true;

  return result;
}

// ---------------------------------------------------------------------------
bool TDrawingItem::MouseUp(float x, float y) {
  bool result = false; // Not edited

  switch (m_nType) {
  case 1: // FreeHand
    FinishFreeHandRect();
    break;
  }

  return result;
}
// ---------------------------------------------------------------------------

float LineToPoint(float x0, float y0, float x1, float y1, float px, float py) {
  float dx = x1 - x0;
  float dy = y1 - y0;
  float a = dx * dx + dy * dy;
  if (a == 0.0f) {
    return sqrt((x0 - px) * (x0 - px) + (y0 - py) * (y0 - py));
  }
  float b = dx * (x0 - px) + dy * (y0 - py);
  float t = -(b / a);
  if (t < 0.0f) {
    t = 0.0f;
  }
  if (t > 1.0f) {
    t = 1.0f;
  }
  float x = t * dx + x0;
  float y = t * dy + y0;
  return sqrt((x - px) * (x - px) + (y - py) * (y - py));
}

int TDrawingItem::CheckDrag(float x, float y, int size) {
  // Check if drag can start. Returns drag type

  m_fDragStartX = x;
  m_fDragStartY = y;
  memcpy(m_DragRect, m_Rect, sizeof(m_Rect));

  // Convert to screen coords
  int rx = x * size;
  int ry = y * size;

  // Four corners
  for (int i = 0; i < 4; i++) {
    if (m_nType != 2 || i == 0 || i == 3) {
      if (rx >= size * m_Rect[(i % 2) * 2] - 4 &&
          ry >= size * m_Rect[(i / 2) * 2 + 1] - 4 &&
          rx < size * m_Rect[(i % 2) * 2] + 4 &&
          ry < size * m_Rect[(i / 2) * 2 + 1] + 4) {

        return 0x10 | i;
      }
    }
  }

  // Translation
  switch (m_nType) {
  case 1: // FreeHand
  {
    float w2 = m_Rect[2] - m_Rect[0];
    float h2 = m_Rect[3] - m_Rect[1];
    int result = 0;
    if (w2 != 0.0f && h2 != 0.0f) {
      float *p = new float[Count];
      for (int i = 0; i < Count / 2; i++) {
        int ix = (int)(intptr_t)Items[i * 2];
        float fx = *(float *)&ix;
        int iy = (int)(intptr_t)Items[i * 2 + 1];
        float fy = *(float *)&iy;

        p[i * 2] = m_Rect[0] + fx * w2;
        p[i * 2 + 1] = m_Rect[1] + fy * h2;
      }

      for (int i = 1; i < Count / 2; i++) {
        if (LineToPoint(size * p[(i - 1) * 2], size * p[(i - 1) * 2 + 1],
                        size * p[i * 2], size * p[i * 2 + 1], rx, ry) <= 4) {
          result = 0x01;
        }
      }

      delete[] p;
    }
    if (result) {
      return result;
    }
  } break;
  case 2: // Line
    if (LineToPoint(size * m_Rect[0], size * m_Rect[1], size * m_Rect[2],
                    size * m_Rect[3], rx, ry) <= 4) {
      return 0x01;
    }
    break;
  case 3: // Box
    if (LineToPoint(size * m_Rect[0], size * m_Rect[1], size * m_Rect[0],
                    size * m_Rect[3], rx, ry) <= 4 ||
        LineToPoint(size * m_Rect[0], size * m_Rect[1], size * m_Rect[2],
                    size * m_Rect[1], rx, ry) <= 4 ||
        LineToPoint(size * m_Rect[2], size * m_Rect[1], size * m_Rect[2],
                    size * m_Rect[3], rx, ry) <= 4 ||
        LineToPoint(size * m_Rect[0], size * m_Rect[3], size * m_Rect[2],
                    size * m_Rect[3], rx, ry) <= 4) {
      return 0x01;
    }
    break;
  case 4: // Circle
  {
    if (m_Rect[3] - m_Rect[1] == 0.0f || m_Rect[2] - m_Rect[0] == 0.0f) {
      // Ellipse is line-like
      if (LineToPoint(size * m_Rect[0], size * m_Rect[1], size * m_Rect[2],
                      size * m_Rect[3], rx, ry) <= 4) {
        return 0x01;
      }
    } else {
      double xx = (m_Rect[3] - m_Rect[1]) / (m_Rect[2] - m_Rect[0]);
      // X scale factor to make ellipse circular
      double cx = (m_Rect[2] + m_Rect[0]) * 0.5 * xx; // Circle center X
      double cy = (m_Rect[3] + m_Rect[1]) * 0.5;      // Circle center Y
      double dx = x * xx; // Cursor position as circle
      double dist = sqrt((dx - cx) * (dx - cx) + (y - cy) * (y - cy));
      // Distance from center to cursor
      double r = fabs(m_Rect[3] - m_Rect[1]) * 0.5; // Circle radius
      double rd =
          r / dist; // Point on circumference at rd from center toward cursor
      double ccx = (cx * (1.0 - rd) + dx * rd) / xx; // Circumference X
      double ccy = cy * (1.0 - rd) + y * rd;         // Circumference Y
      if (sqrt((x - ccx) * (x - ccx) * size * size +
               (y - ccy) * (y - ccy) * size * size) <= 6) {
        // Drag if cursor close to circumference
        return 0x01;
      }
    }
  } break;
  }

  return 0; // Cannot drag
}

// ---------------------------------------------------------------------------
bool TDrawingItem::Drag(float x, float y, int dragmode) {
  bool result = false;

  if (dragmode == 0x01) {
    // Translation
    m_Rect[0] = m_DragRect[0] + x - m_fDragStartX;
    m_Rect[2] = m_DragRect[2] + x - m_fDragStartX;
    m_Rect[1] = m_DragRect[1] + y - m_fDragStartY;
    m_Rect[3] = m_DragRect[3] + y - m_fDragStartY;
    result = true;
  } else if (dragmode >= 0x10 && dragmode < 0x14) {
    // Corner drag
    m_Rect[((dragmode & 0x3) % 2) * 2] = x;
    m_Rect[((dragmode & 0x3) / 2) * 2 + 1] = y;
    result = true;
  }

  return result;
}

// ---------------------------------------------------------------------------
void TDrawingItem::Draw(TCanvas *C, TRect &R) {
  if (!m_bExist) {
    // Not enough info to draw yet
    return;
  }

  int w = R.right - R.left;
  if (w < 1) {
    w = 1;
  }
  int h = R.bottom - R.top;
  if (h < 1) {
    h = 1;
  }

  TPen *P = C->Pen;
  TColor pcbak = P->Color;
  int pwbak = P->Width;
  if (m_nPenColor >= 0) {
    if (m_nPenColor <= 0xffffff) {
      P->Color = m_nPenColor;
    }
    P->Style = psSolid;
  } else {
    P->Style = psClear;
  }

  TBrush *B = C->Brush;
  TColor bcbak = B->Color;
  if (m_nBrushColor >= 0) {
    if (m_nBrushColor <= 0xffffff) {
      B->Color = m_nBrushColor;
    }
    B->Style = bsSolid;
  } else {
    B->Style = bsClear;
  }

  switch (m_nType) {
  case 0: // Select
    P->Width = 1;
    P->Style = psDot;
    P->Color = TColor(0xffffff);
    P->Mode = pmXor;
    B->Style = bsClear;
    C->Rectangle(R.left + w * m_Rect[0], R.top + h * m_Rect[1],
                 R.left + w * m_Rect[2], R.top + h * m_Rect[3]);
    break;
  case 1: // FreeHand
  {
    /*
     float xmin = 0.0f, xmax = 1.0f, ymin = 0.0f, ymax = 1.0f;
     if (m_Rect[0] != 0.0f || m_Rect[1] != 0.0f || m_Rect[2] != 1.0f ||
     m_Rect[3] != 1.0f){ for (int i = 0 ; i < Count / 2 ; i++){ int ix =
     (int)Items[i * 2]; float x = *(float*)&ix; int iy = (int)Items[i * 2 + 1];
     float y = *(float*)&iy;
     if (i == 0 || x < xmin){
     xmin = x;
     }
     if (i == 0 || y < ymin){
     ymin = y;
     }
     if (i == 0 || x > xmax){
     xmax = x;
     }
     if (i == 0 || y > ymax){
     ymax = y;
     }
     }
     }

     // Prepare TPoint
     float w2 = m_Rect[2] - m_Rect[0];
     float h2 = m_Rect[3] - m_Rect[1];
     if (w2 >= 0.0f && h2 >= 0.0f && xmin != xmax && ymin != ymax){
     TPoint *p = new TPoint[Count / 2];
     for (int i = 0 ; i < Count / 2 ; i++){
     int ix = (int)Items[i * 2];
     float x = *(float*)&ix;
     int iy = (int)Items[i * 2 + 1];
     float y = *(float*)&iy;

     p[i].x = R.Left + w * (m_Rect[0] + (x - xmin) / (xmax - xmin) * w2);
     p[i].y = R.Top + h * (m_Rect[1] + (y - ymin) / (ymax - ymin) * h2);
     }

     for (int i = 0 ; i < Count / 2 ; i++){
     if (i == 0){
     C->MoveTo(p[i].x, p[i].y);
     }else{
     C->LineTo(p[i].x, p[i].y);
     }
     }

     delete[] p;
     }
     */
    // Prepare TPoint
    float w2 = m_Rect[2] - m_Rect[0];
    float h2 = m_Rect[3] - m_Rect[1];
    if (w2 == 0.0f) {
      w2 = 0.1f;
    }
    if (h2 == 0.0f) {
      h2 = 0.1f;
    }
    TPoint *p = new TPoint[Count / 2];
    for (int i = 0; i < Count / 2; i++) {
      int ix = (int)(intptr_t)Items[i * 2];
      float x = *(float *)&ix;
      int iy = (int)(intptr_t)Items[i * 2 + 1];
      float y = *(float *)&iy;

      p[i].x = R.Left + w * (m_Rect[0] + x * w2);
      p[i].y = R.Top + h * (m_Rect[1] + y * h2);
    }

    for (int i = 0; i < Count / 2; i++) {
      if (i == 0) {
        C->MoveTo(p[i].x, p[i].y);
      } else {
        C->LineTo(p[i].x, p[i].y);
      }
    }

    delete[] p;
  } break;
  case 2: // Line
    C->MoveTo(R.left + w * m_Rect[0], R.top + h * m_Rect[1]);
    C->LineTo(R.left + w * m_Rect[2], R.top + h * m_Rect[3]);
    break;
  case 3: // Box
    C->Rectangle(R.left + w * m_Rect[0], R.top + h * m_Rect[1],
                 R.left + w * m_Rect[2], R.top + h * m_Rect[3]);
    break;
  case 4: // Circle
    C->Ellipse(R.left + w * m_Rect[0], R.top + h * m_Rect[1],
               R.left + w * m_Rect[2], R.top + h * m_Rect[3]);
    break;
  }

  P->Color = pcbak;
  B->Color = bcbak;

  // Draw selection rect
  if (m_bSelected) {
    P->Style = psSolid;
    P->Width = 1;
    B->Style = bsClear;
    for (int i = 0; i < 4; i++) {
      if (m_nType != 2 || i == 0 || i == 3) {
        C->Rectangle(R.left + w * m_Rect[(i % 2) * 2] - 4,
                     R.top + h * m_Rect[(i / 2) * 2 + 1] - 4,
                     R.left + w * m_Rect[(i % 2) * 2] + 4,
                     R.top + h * m_Rect[(i / 2) * 2 + 1] + 4);
      }
    }
  }
  P->Width = pwbak;
}

// ---------------------------------------------------------------------------
int TDrawingItem::SetPenColor(TColor C) {
  // Change color. Returns true on success
  switch (m_nType) {
  case 1:
  case 2:
  case 3:
  case 4:
    if (m_nPenColor != C) {
      m_nPenColor = C;
      return true;
    }
    break;
  }
  return false;
}

// ---------------------------------------------------------------------------
int TDrawingItem::SetBrushColor(TColor C) {
  switch (m_nType) {
  case 3:
  case 4:
    if (m_nBrushColor != C) {
      m_nBrushColor = C;
      return true;
    }
    break;
  }
  return false;
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
TDrawing::TDrawing()
    : TList(), m_nTool(0), m_TmpItem(NULL), m_nDragMode(0), m_nDragIndex(0),
      m_bDrawRequest(true), m_bModified(false), m_nPenColor(TColor(0x7fffffff)),
      m_nBrushColor(TColor(-1)) {}

// ---------------------------------------------------------------------------
TDrawing::TDrawing(TDrawing &Source)
    : TList(), m_nTool(0), m_TmpItem(NULL), m_nDragMode(0), m_nDragIndex(0),
      m_bDrawRequest(true), m_bModified(false), m_nPenColor(TColor(0x7fffffff)),
      m_nBrushColor(TColor(-1)) {
  for (int i = 0; i < Source.Count; i++) {
    Add(new TDrawingItem(*Source.DItem(i)));
  }
  ClearSelection();
}

// ---------------------------------------------------------------------------
TDrawing::TDrawing(UnicodeString S)
    : TList(), m_nTool(0), m_TmpItem(NULL), m_nDragMode(0), m_nDragIndex(0),
      m_bDrawRequest(true), m_bModified(false), m_nPenColor(TColor(0x7fffffff)),
      m_nBrushColor(TColor(-1)) {
  // Decode from string
  int count = StrToIntDef(UnicodeString("0x") + S.SubString(1, 8), 0);
  int pos = 9;
  for (int i = 0; i < count; i++) {
    int len = StrToIntDef(UnicodeString("0x") + S.SubString(pos, 8), 0);
    pos += 8;
    Add(new TDrawingItem(S.SubString(pos, len)));
    pos += len;
  }
}

// ---------------------------------------------------------------------------
UnicodeString TDrawing::Encode(bool bAll) {
  // Encode to string
  UnicodeString result;
  int count = 0;
  for (int i = 0; i < Count; i++) {
    if (bAll || DItem(i)->m_bSelected) {
      // Item to encode
      UnicodeString S = DItem(i)->Encode();
      result += IntToHex(S.Length(), 8) + S;
      count++;
    }
  }

  return IntToHex(count, 8) + result;
}

// ---------------------------------------------------------------------------
__fastcall TDrawing::~TDrawing() {
  FreeTmpItem();
  for (int i = 0; i < Count; i++) {
    delete DItem(i);
  }
}

// ---------------------------------------------------------------------------
void TDrawing::FreeTmpItem() {
  if (m_TmpItem) {
    delete m_TmpItem;
  }
  m_TmpItem = NULL;
}

// ---------------------------------------------------------------------------
TDrawingItem *TDrawing::DItem(int index) {
  return (TDrawingItem *)Items[index];
}

// ---------------------------------------------------------------------------
void TDrawing::SetTool(int nToolID) { m_nTool = nToolID; }

// ---------------------------------------------------------------------------
void TDrawing::MouseDown(float x, float y, int size, TShiftState Shift) {
  m_nDragMode = 0;
  FreeTmpItem();

  if (m_nTool == 0 && !Shift.Contains(ssShift)) {
    // Selection operation

    // Check drag start
    // Loop from front to back
    for (int i = Count - 1; i >= 0; i--) {
      // Call for all items once (to init m_DragRect)
      DItem(i)->CheckDrag(x, y, size);
    }
    for (int i = Count - 1; i >= 0; i--) {
      m_nDragMode = DItem(i)->CheckDrag(x, y, size);
      if (m_nDragMode) {
        // Drag started
        if (!Shift.Contains(ssCtrl)) {
          if (!DItem(i)->m_bSelected) {
            ClearSelection();
            DItem(i)->m_bSelected = true;
          }
        } else {
          // While holding Ctrl
          DItem(i)->m_bSelected = !DItem(i)->m_bSelected;
        }
        m_nDragIndex = i;
        break;
      }
    }
    if (m_nDragMode) {
      // Drag started
      m_bDrawRequest = true;
      m_fDragX = x;
      m_fDragY = y;
      return;
    }
  }
  if (!Shift.Contains(ssCtrl)) {
    ClearSelection();
  }

  m_TmpItem = new TDrawingItem();
  m_TmpItem->SetType(m_nTool);
  m_TmpItem->m_nPenColor = m_nPenColor;
  m_TmpItem->m_nBrushColor = m_nBrushColor;
  m_TmpItem->MouseDown(x, y);
  /*
   if (m_nTool == 0){
   // Selection operation
   }else{
   ClearSelection();
   m_TmpItem = new TDrawingItem();
   m_TmpItem->SetType(m_nTool);
   m_TmpItem->MouseDown(x, y);
   }
   */
  m_bDrawRequest = true;
}

// ---------------------------------------------------------------------------
void TDrawing::MouseMove(float x, float y) {
  if (m_nDragMode) {
    if (m_nDragMode == 0x01) {
      for (int i = 0; i < Count; i++)
        if (DItem(i)->m_bSelected) {
          m_bModified |= DItem(i)->Drag(x, y, m_nDragMode);
        }
    } else if (m_nDragMode >= 0x10 && m_nDragMode < 0x14) {
      m_bModified |= DItem(m_nDragIndex)->Drag(x, y, m_nDragMode);
    }
  }

  if (m_TmpItem) {
    m_TmpItem->MouseMove(x, y);
    m_bDrawRequest = true;
  }
  /*
   if (m_nTool == 0){
   // Selection operation
   }else{
   if (m_TmpItem){
   m_TmpItem->MouseMove(x, y);
   }
   }
   */
}

// ---------------------------------------------------------------------------
void TDrawing::MouseUp(float x, float y) {
  m_nDragMode = 0;
  if (m_TmpItem) {
    if (m_nTool == 0) {
      // Selection operation
      if (m_TmpItem->m_bExist) {
        SelectRect(m_TmpItem->m_Rect[0], m_TmpItem->m_Rect[1],
                   m_TmpItem->m_Rect[2], m_TmpItem->m_Rect[3]);
      }
    } else {
      m_TmpItem->MouseUp(x, y);

      if (m_TmpItem->m_bExist) {
        // Add new item
        m_TmpItem->m_bSelected = true;
        Add(m_TmpItem);
        m_bModified = true;
        m_TmpItem = NULL;
      }
    }
    FreeTmpItem();
    m_bDrawRequest = true;
  }
}

// ---------------------------------------------------------------------------
void TDrawing::Draw(TCanvas *C, TRect &R) {
  for (int i = 0; i < Count; i++) {
    DItem(i)->Draw(C, R);
  }
  if (m_TmpItem) {
    m_TmpItem->Draw(C, R);
  }
}

// ---------------------------------------------------------------------------
void TDrawing::SelectRect(float x1, float y1, float x2, float y2) {
  float x1_ = x1 < x2 ? x1 : x2;
  float x2_ = x1 >= x2 ? x1 : x2;
  float y1_ = y1 < y2 ? y1 : y2;
  float y2_ = y1 >= y2 ? y1 : y2;
  for (int i = 0; i < Count; i++) {
    TDrawingItem *DI = DItem(i);
    DI->m_bSelected = true;
    for (int i2 = 0; i2 < 2; i2++) {
      DI->m_bSelected &= DI->m_Rect[i2 * 2] >= x1_ && DI->m_Rect[i2 * 2] < x2_;
      DI->m_bSelected &=
          DI->m_Rect[i2 * 2 + 1] >= y1_ && DI->m_Rect[i2 * 2 + 1] < y2_;
    }
  }
  m_bDrawRequest = true;
}

// ---------------------------------------------------------------------------
void TDrawing::ClearSelection() {
  for (int i = 0; i < Count; i++) {
    DItem(i)->m_bSelected = false;
  }
  m_bDrawRequest = true;
}

// ---------------------------------------------------------------------------
void TDrawing::SelectAll() {
  for (int i = 0; i < Count; i++) {
    DItem(i)->m_bSelected = true;
  }
  m_bDrawRequest = true;
}

// ---------------------------------------------------------------------------
int TDrawing::Selected() {
  int result = 0;
  for (int i = 0; i < Count; i++) {
    if (DItem(i)->m_bSelected) {
      result++;
    }
  }
  return result;
}

// ---------------------------------------------------------------------------
void TDrawing::Cut() {
  Copy();
  DDelete();
}

// ---------------------------------------------------------------------------
void TDrawing::Copy() {
  Clipboard()->SetTextBuf((UnicodeString("Drawing:") + Encode()).c_str());
}

// ---------------------------------------------------------------------------
void TDrawing::Paste() {
  if (!Clipboard()->HasFormat(CF_TEXT)) {
    return;
  }
  UnicodeString S = Clipboard()->AsText;
  if (S.SubString(1, 8) != "Drawing:") {
    return;
  }

  ClearSelection();
  TDrawing *Source = new TDrawing(S.SubString(9, S.Length()));
  int existingnum = Count;
  for (int i = 0; i < Source->Count; i++) {
    TDrawingItem *Item = Source->DItem(i);

    // Update coord
    bool found = true;
    while (found) { // Loop while objects at same coord exist
      found = false;
      for (int i2 = 0; i2 < existingnum; i2++) {
        TDrawingItem *Item2 = DItem(i2);
        bool same = true;
        for (int i3 = 0; i3 < 4 && same; i3++) {
          same &= fabs(Item->m_Rect[i3] - Item2->m_Rect[i3]) < 0.001f;
        }

        if (same) {
          for (int i3 = 0; i3 < 4; i3++) {
            Item->m_Rect[i3] += 1.0f / 32;
          }
          found = true;
          break;
        }
      }
    }

    Add(new TDrawingItem(*Item));
    m_bModified = true;
    m_bDrawRequest = true;
  }
  delete Source;
}

// ---------------------------------------------------------------------------
void TDrawing::DDelete() {
  for (int i = Count - 1; i >= 0; i--) {
    if (DItem(i)->m_bSelected) {
      delete DItem(i);
      Delete(i);
    }
  }
  m_bModified = true;
  m_bDrawRequest = true;
}

// ---------------------------------------------------------------------------
void TDrawing::ChangePenColor(TColor C) {
  bool changed = false;
  for (int i = 0; i < Count; i++) {
    if (DItem(i)->m_bSelected) {
      changed |= DItem(i)->SetPenColor(C);
    }
  }
  if (changed) {
    m_bModified = true;
    m_bDrawRequest = true;
  }
}

// ---------------------------------------------------------------------------
void TDrawing::ChangeBrushColor(TColor C) {
  bool changed = false;
  for (int i = 0; i < Count; i++) {
    if (DItem(i)->m_bSelected) {
      changed |= DItem(i)->SetBrushColor(C);
    }
  }
  if (changed) {
    m_bModified = true;
    m_bDrawRequest = true;
  }
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)

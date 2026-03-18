// ---------------------------------------------------------------------------
// Browser drawing and painting (split from fomain.cpp)

#include <vcl.h>
#pragma hdrstop
#include <math.h>

#define OVERVIEWSIZE 0.75f

#include "fomain.h"
#include "fomain_paint.h"
#include "fofullscreen.h"
#include "setting.h"
#include "utils.h"

#pragma package(smart_init)

// ---------------------------------------------------------------------------
void TFo_Main::CalcCardSize(TCanvas *C, TCard *Card, int Size, int cardindex) {
  // Y center calc
  int Space = (m_nFontHeight * Size) / 100;
  if (Space < 1) {
    Space = 1;
  }

  TFont *F = C->Font;

  int imagex = 100; // Image scale
  bool Drawing = false;
  TCardVideo *Video = NULL;
  TCardImage *Image = NULL;
  if (Card) {
    Drawing = SettingView.m_bDrawingVisible && (Card->m_Drawing->Count > 0);
    if (SettingView.m_bVideoVisible && Card->m_VideoFN != "") {
      Video = SearchVideo(Card->m_VideoFN);
    }
    if (SettingView.m_bImageVisible && !Video && Card->m_ImageFN != "") {
      Image = SearchImage(Card->m_ImageFN);
    }
  }

  TStringList *Title = new TStringList();
  WideString S = DecodeES(m_CardTitle[cardindex]);
  int lines = CountStr(S, "\n") + 1;
  if (lines <= 1) {
    Title->Add(S);
  } else {
    while (S != WideString("")) {
      Title->Add(SplitStrBy(S, "\n"));
    }
  }

  int lineheight = (m_nFontHeight * Size) / 100;
  int tickerheight = 0;
  bool ticker = false;
  if (SettingView.m_bTickerVisible && Card->m_Lines->Count > 0) {
    for (int il = 0;
         il < SettingView.m_nTickerLines && il < Card->m_Lines->Count; il++) {
      ticker |= Trim(Card->m_Lines->Strings[il]) != "";
    }
    if (ticker) {
      tickerheight = lineheight * 0.8f;
    }
  }
  m_CardHeight[cardindex] = lineheight * lines + tickerheight + Space;

  if (Drawing) {
    imagex = abs((m_nFontHeight * Size * F->Size) /
                 (RE_Edit->Font->Height * SettingView.m_Font->Size));
    m_CardHeight[cardindex] +=
        SettingView.m_nImageLimitation * 0.75 * imagex * 0.01;
  }

  /*
   VIDEOINFOHEADER *vi = NULL;
   if (Video){
   vi = Video->m_Video->GetVideoInfoHeader();
   if (Video->m_bExist && vi){
   imagex = abs((m_nFontHeight * Size * F->Size) / (RE_Edit->Font->Height *
   SettingView.m_Font->Size)); if (vi->bmiHeader.biHeight >
   vi->bmiHeader.biWidth){
   // Y coordinate
   if (vi->bmiHeader.biHeight > SettingView.m_nImageLimitation){
   imagex = (imagex * SettingView.m_nImageLimitation) / vi->bmiHeader.biHeight;
   }
   }else{
   // X coordinate
   if (vi->bmiHeader.biWidth > SettingView.m_nImageLimitation){
   imagex = (imagex * SettingView.m_nImageLimitation) / vi->bmiHeader.biWidth;
   }
   }
   m_CardHeight[cardindex] += (vi->bmiHeader.biHeight * imagex) / 100;
   }else{
   Video = NULL;
   }
   }
   */
  if (Image) {
    if (Image->m_bExist) {
      imagex = abs((m_nFontHeight * Size * F->Size) /
                   (RE_Edit->Font->Height * SettingView.m_Font->Size));
      if (Image->m_Image->Height > Image->m_Image->Width) {
        // Y coordinate
        if (Image->m_Image->Height > SettingView.m_nImageLimitation) {
          imagex = (imagex * SettingView.m_nImageLimitation) /
                   Image->m_Image->Height;
        }
      } else {
        // X coordinate
        if (Image->m_Image->Width > SettingView.m_nImageLimitation) {
          imagex =
              (imagex * SettingView.m_nImageLimitation) / Image->m_Image->Width;
        }
      }
      m_CardHeight[cardindex] += (Image->m_Image->Height * imagex) / 100;
    } else {
      Image = NULL;
    }
  }

  // Center calc
  int OrgFontHeight = F->Height;
  F->Height = (F->Height * Size) / 100;
  if (F->Height == 0) {
    F->Height = -1;
  }

  // X center calc
  m_CardWidth[cardindex] = C->TextWidth(Title->Strings[0]) + Space;
  for (int i = 1; i < Title->Count; i++) {
    int w = C->TextWidth(Title->Strings[i]) + Space;
    if (w > m_CardWidth[cardindex]) {
      m_CardWidth[cardindex] = w;
    }
  }
  if (Drawing)
    if (m_CardWidth[cardindex] <
        (SettingView.m_nImageLimitation * imagex) / 100 + Space) {
      m_CardWidth[cardindex] =
          (SettingView.m_nImageLimitation * imagex) / 100 + Space;
    }
  /*
   if (Video) if (m_CardWidth[cardindex] < (vi->bmiHeader.biWidth * imagex) /
   100 + Space){ m_CardWidth[cardindex] = (vi->bmiHeader.biWidth * imagex) / 100
   + Space;
   }
   */
  if (Image)
    if (m_CardWidth[cardindex] <
        (Image->m_Image->Width * imagex) / 100 + Space) {
      m_CardWidth[cardindex] = (Image->m_Image->Width * imagex) / 100 + Space;
    }
  F->Height = OrgFontHeight;
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawCard(TCanvas *C, TCard *Card, int Size, int cardindex,
                        TColor HMColor, int option) {
  // Y center calc
  int Space = (m_nFontHeight * Size) / 100;
  if (Space < 1) {
    Space = 1;
  }

  TBrush *B = C->Brush;
  TPen *P = C->Pen;
  TFont *F = C->Font;

  TColor PColor = P->Color; // Color
  TColor BColor = B->Color;

  int imagex = 100; // Image scale
  bool Drawing = false;
  TCardVideo *Video = NULL;
  TCardImage *Image = NULL;
  if (Card) {
    Drawing = SettingView.m_bDrawingVisible && (Card->m_Drawing->Count > 0);
    if (SettingView.m_bVideoVisible && !Drawing && Card->m_VideoFN != "") {
      Video = SearchVideo(Card->m_VideoFN);
    }
    if (SettingView.m_bImageVisible && !Drawing && !Video &&
        Card->m_ImageFN != "") {
      Image = SearchImage(Card->m_ImageFN);
    }
  }

  TStringList *Title = new TStringList();
  WideString S = DecodeES(m_CardTitle[cardindex]);
  int lines = CountStr(S, "\n") + 1;
  if (lines <= 1) {
    Title->Add(S);
  } else {
    while (S != WideString("")) {
      Title->Add(SplitStrBy(S, "\n"));
    }
  }

  int lineheight = (m_nFontHeight * Size) / 100;
  int tickerheight = 0;
  bool ticker = false;
  if (SettingView.m_bTickerVisible && Card->m_Lines->Count > 0) {
    for (int il = 0;
         il < SettingView.m_nTickerLines && il < Card->m_Lines->Count; il++) {
      ticker |= Trim(Card->m_Lines->Strings[il]) != "";
    }
    if (ticker) {
      tickerheight = lineheight * 0.8f;
    }
  }
  m_CardHeight[cardindex] = lineheight * lines + tickerheight + Space;

  if (Drawing) {
    imagex = abs((m_nFontHeight * Size * F->Size) /
                 (RE_Edit->Font->Height * SettingView.m_Font->Size));
    m_CardHeight[cardindex] +=
        SettingView.m_nImageLimitation * 0.75 * imagex * 0.01;
  }

  /*
   VIDEOINFOHEADER *vi = NULL;
   if (Video){
   vi = Video->m_Video->GetVideoInfoHeader();
   if (Video->m_bExist && vi){
   imagex = abs((m_nFontHeight * Size * F->Size) / (RE_Edit->Font->Height *
   SettingView.m_Font->Size)); if (vi->bmiHeader.biHeight >
   vi->bmiHeader.biWidth){
   // Y coordinate
   if (vi->bmiHeader.biHeight > SettingView.m_nImageLimitation){
   imagex = (imagex * SettingView.m_nImageLimitation) / vi->bmiHeader.biHeight;
   }
   }else{
   // X coordinate
   if (vi->bmiHeader.biWidth > SettingView.m_nImageLimitation){
   imagex = (imagex * SettingView.m_nImageLimitation) / vi->bmiHeader.biWidth;
   }
   }
   m_CardHeight[cardindex] += (vi->bmiHeader.biHeight * imagex) / 100;
   }else{
   Video = NULL;
   }
   }
   */
  if (Image) {
    if (Image->m_bExist) {
      imagex = abs((m_nFontHeight * Size * F->Size) /
                   (RE_Edit->Font->Height * SettingView.m_Font->Size));
      if (Image->m_Image->Height > Image->m_Image->Width) {
        // Y coordinate
        if (Image->m_Image->Height > SettingView.m_nImageLimitation) {
          imagex = (imagex * SettingView.m_nImageLimitation) /
                   Image->m_Image->Height;
        }
      } else {
        // X coordinate
        if (Image->m_Image->Width > SettingView.m_nImageLimitation) {
          imagex =
              (imagex * SettingView.m_nImageLimitation) / Image->m_Image->Width;
        }
      }
      m_CardHeight[cardindex] += (Image->m_Image->Height * imagex) / 100;
    } else {
      Image = NULL;
    }
  }

  // Center calc
  int top = m_CardY[cardindex] - m_CardHeight[cardindex] / 2;
  int bottom = m_CardY[cardindex] + m_CardHeight[cardindex] / 2;
  int bottom2 = bottom;
  if (option & 0x1) {
    bottom2 += m_nFontHeight / 2;
  }

  {
    int OrgFontHeight = F->Height;
    F->Height = (F->Height * Size) / 100;
    if (F->Height == 0) {
      F->Height = -1;
    }

    // X center calc
    m_CardWidth[cardindex] = C->TextWidth(Title->Strings[0]) + Space;
    for (int i = 1; i < Title->Count; i++) {
      int w = C->TextWidth(Title->Strings[i]) + Space;
      if (w > m_CardWidth[cardindex]) {
        m_CardWidth[cardindex] = w;
      }
    }
    if (Drawing)
      if (m_CardWidth[cardindex] <
          (SettingView.m_nImageLimitation * imagex) / 100 + Space) {
        m_CardWidth[cardindex] =
            (SettingView.m_nImageLimitation * imagex) / 100 + Space;
      }
    /*
     if (Video) if (m_CardWidth[cardindex] < (vi->bmiHeader.biWidth * imagex) /
     100 + Space){ m_CardWidth[cardindex] = (vi->bmiHeader.biWidth * imagex) /
     100 + Space;
     }
     */
    if (Image)
      if (m_CardWidth[cardindex] <
          (Image->m_Image->Width * imagex) / 100 + Space) {
        m_CardWidth[cardindex] = (Image->m_Image->Width * imagex) / 100 + Space;
      }
    int left = m_CardX[cardindex] - m_CardWidth[cardindex] / 2;
    int right = m_CardX[cardindex] + m_CardWidth[cardindex] / 2;
    int right2 = right;
    if (option & 0x1) {
      right2 += m_nFontHeight / 2;
    }

    if (!((top < 0 && bottom2 < 0) ||
          (top > m_nBrowserHeight && bottom2 > m_nBrowserHeight)) &&
        !((left < 0 && right2 < 0) ||
          (left > m_nBrowserWidth && right2 > m_nBrowserWidth))) {
      // Search?

      // Text Drawing
      B->Style = bsSolid;
      int drawtimes = 1;
      if (SettingView.m_bCardGradation && !(option & 0x1)) {
        // Before refresh: setting, then draw
        drawtimes++;
      }
      while (drawtimes) {
        HRGN MyRgn, RgnBak;
        int rgnexist;

        if (SettingView.m_bCardGradation && !(option & 0x1)) {
          // MyRgn = ::CreateRectRgn(m_nXOffset + left + Space / 2, Y,
          // m_nXOffset + right - Space / 2, Y + tickerheight);
          switch (drawtimes) {
          case 2:
            // Update
            MyRgn = ::CreateRectRgn(m_nXOffset, m_nYOffset + m_CardY[cardindex],
                                    m_nXOffset + m_nBrowserWidth,
                                    m_nYOffset + m_nBrowserHeight);
            break;
          case 1:
            // Search
            // B->Color = BrightColor(B->Color, 1.25f);
            B->Color = HalfColor(B->Color, 0xffffff, 0.25f);
            MyRgn = ::CreateRectRgn(m_nXOffset, m_nYOffset,
                                    m_nXOffset + m_nBrowserWidth,
                                    m_nYOffset + m_CardY[cardindex]);
            break;
          }

          RgnBak = ::CreateRectRgn(0, 0, 0, 0);
          rgnexist = ::GetClipRgn(C->Handle, RgnBak);
          if (rgnexist) {
            HRGN CmbRgn;
            CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
            ::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
            ::SelectClipRgn(C->Handle, CmbRgn);
            ::DeleteObject(CmbRgn);
          } else {
            ::SelectClipRgn(C->Handle, MyRgn);
          }
        }
        switch (m_CardShape[cardindex]) {
        case 0: // No drawing
          break;
        case 1: // Rect
          C->Rectangle(m_nXOffset + left, m_nYOffset + top, m_nXOffset + right,
                       m_nYOffset + bottom);
          break;
        case 3: // RRect2
          C->RoundRect(m_nXOffset + left, m_nYOffset + top, m_nXOffset + right,
                       m_nYOffset + bottom, m_CardHeight[cardindex],
                       m_CardHeight[cardindex]);
          break;
        case 4: // Ellipse
          C->Ellipse(m_nXOffset + left, m_nYOffset + top, m_nXOffset + right,
                     m_nYOffset + bottom);
          break;
        case 5: // Dia
        {
          TPoint ps[4];
          // Top
          ps[0].x = m_nXOffset + m_CardX[cardindex];
          ps[0].y = m_nYOffset + top;
          // Right
          ps[1].x = m_nXOffset + right;
          ps[1].y = m_nYOffset + m_CardY[cardindex];
          // Bottom
          ps[2].x = m_nXOffset + m_CardX[cardindex];
          ps[2].y = m_nYOffset + bottom;
          // Left
          ps[3].x = m_nXOffset + left;
          ps[3].y = m_nYOffset + m_CardY[cardindex];
          C->Polygon(ps, 3);
        } break;
        case 6: // Hex
        {
          TPoint ps[6];
          // Top left
          ps[0].x = m_nXOffset + left + m_CardHeight[cardindex] / 3;
          ps[0].y = m_nYOffset + top;
          // Top right
          ps[1].x = m_nXOffset + right - m_CardHeight[cardindex] / 3;
          ps[1].y = m_nYOffset + top;
          // Right
          ps[2].x = m_nXOffset + right;
          ps[2].y = m_nYOffset + m_CardY[cardindex];
          // Bottom right
          ps[3].x = m_nXOffset + right - m_CardHeight[cardindex] / 3;
          ps[3].y = m_nYOffset + bottom;
          // Bottom left
          ps[4].x = m_nXOffset + left + m_CardHeight[cardindex] / 3;
          ps[4].y = m_nYOffset + bottom;
          // Left
          ps[5].x = m_nXOffset + left;
          ps[5].y = m_nYOffset + m_CardY[cardindex];
          C->Polygon(ps, 5);
        } break;
        case 7: // Trapezoid top
        {
          TPoint ps[4];
          // Top left
          ps[0].x = m_nXOffset + left + m_CardHeight[cardindex] / 4;
          ps[0].y = m_nYOffset + top;
          // Top right
          ps[1].x = m_nXOffset + right - m_CardHeight[cardindex] / 4;
          ps[1].y = m_nYOffset + top;
          // Bottom right
          ps[2].x = m_nXOffset + right;
          ps[2].y = m_nYOffset + bottom;
          // Bottom left
          ps[3].x = m_nXOffset + left;
          ps[3].y = m_nYOffset + bottom;
          C->Polygon(ps, 3);
        } break;
        case 8: // Trapezoid bottom
        {
          TPoint ps[4];
          // Top left
          ps[0].x = m_nXOffset + left;
          ps[0].y = m_nYOffset + top;
          // Top right
          ps[1].x = m_nXOffset + right;
          ps[1].y = m_nYOffset + top;
          // Bottom right
          ps[2].x = m_nXOffset + right - m_CardHeight[cardindex] / 4;
          ps[2].y = m_nYOffset + bottom;
          // Bottom left
          ps[3].x = m_nXOffset + left + m_CardHeight[cardindex] / 4;
          ps[3].y = m_nYOffset + bottom;
          C->Polygon(ps, 3);
        } break;
        case 9: // Small rect
          C->Rectangle(m_nXOffset + (left + right) / 2 - Space,
                       m_nYOffset + (top + bottom) / 2 - Space,
                       m_nXOffset + (left + right) / 2 + Space,
                       m_nYOffset + (top + bottom) / 2 + Space);
          break;
        case 10: // Small circle
          C->Ellipse(m_nXOffset + (left + right) / 2 - Space,
                     m_nYOffset + (top + bottom) / 2 - Space,
                     m_nXOffset + (left + right) / 2 + Space,
                     m_nYOffset + (top + bottom) / 2 + Space);
          break;
        case 11: // Small triangle (up)
        {
          TPoint ps[3];
          // Top
          ps[0].x = m_nXOffset + (left + right) / 2;
          ps[0].y = m_nYOffset + (top + bottom) / 2 - Space;
          // Bottom right
          ps[1].x = m_nXOffset + (left + right) / 2 + Space * (1.7320508 / 2);
          ps[1].y = m_nYOffset + (top + bottom) / 2 + Space / 2;
          // Bottom left
          ps[2].x = m_nXOffset + (left + right) / 2 - Space * (1.7320508 / 2);
          ps[2].y = m_nYOffset + (top + bottom) / 2 + Space / 2;
          C->Polygon(ps, 2);
        } break;
        case 12: // Small triangle (down)
        {
          TPoint ps[3];
          // Bottom
          ps[0].x = m_nXOffset + (left + right) / 2;
          ps[0].y = m_nYOffset + (top + bottom) / 2 + Space;
          // Top right
          ps[1].x = m_nXOffset + (left + right) / 2 + Space * (1.7320508 / 2);
          ps[1].y = m_nYOffset + (top + bottom) / 2 - Space / 2;
          // Top left
          ps[2].x = m_nXOffset + (left + right) / 2 - Space * (1.7320508 / 2);
          ps[2].y = m_nYOffset + (top + bottom) / 2 - Space / 2;
          C->Polygon(ps, 2);
        } break;
        case 13: // Dia
        {
          TPoint ps[4];
          // Top
          ps[0].x = m_nXOffset + (left + right) / 2;
          ps[0].y = m_nYOffset + (top + bottom) / 2 - Space;
          // Right
          ps[1].x = m_nXOffset + (left + right) / 2 + Space;
          ps[1].y = m_nYOffset + (top + bottom) / 2;
          // Bottom
          ps[2].x = m_nXOffset + (left + right) / 2;
          ps[2].y = m_nYOffset + (top + bottom) / 2 + Space;
          // Left
          ps[3].x = m_nXOffset + (left + right) / 2 - Space;
          ps[3].y = m_nYOffset + (top + bottom) / 2;
          C->Polygon(ps, 3);
        } break;
        case 14: // Hex
        {
          TPoint ps[6];
          ps[0].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 6 * 1) * Space;
          ps[0].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 6 * 1) * Space;
          ps[1].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 6 * 3) * Space;
          ps[1].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 6 * 3) * Space;
          ps[2].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 6 * 5) * Space;
          ps[2].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 6 * 5) * Space;
          ps[3].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 6 * 7) * Space;
          ps[3].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 6 * 7) * Space;
          ps[4].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 6 * 9) * Space;
          ps[4].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 6 * 9) * Space;
          ps[5].x =
              m_nXOffset + (left + right) / 2 + sin(M_PI / 6 * 11) * Space;
          ps[5].y =
              m_nYOffset + (top + bottom) / 2 - cos(M_PI / 6 * 11) * Space;
          C->Polygon(ps, 5);
        } break;
        case 15: // Star
        {
          int c =
              (Space * sin(M_PI * (18.0 / 360.0))) / sin(M_PI * (54.0 / 360.0));
          TPoint ps[10];
          ps[0].x = m_nXOffset + (left + right) / 2 + sin(0.0) * Space;
          ps[0].y = m_nYOffset + (top + bottom) / 2 - cos(0.0) * Space;
          ps[1].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 5 * 1) * c;
          ps[1].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 5 * 1) * c;
          ps[2].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 5 * 2) * Space;
          ps[2].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 5 * 2) * Space;
          ps[3].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 5 * 3) * c;
          ps[3].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 5 * 3) * c;
          ps[4].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 5 * 4) * Space;
          ps[4].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 5 * 4) * Space;
          ps[5].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 5 * 5) * c;
          ps[5].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 5 * 5) * c;
          ps[6].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 5 * 6) * Space;
          ps[6].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 5 * 6) * Space;
          ps[7].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 5 * 7) * c;
          ps[7].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 5 * 7) * c;
          ps[8].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 5 * 8) * Space;
          ps[8].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 5 * 8) * Space;
          ps[9].x = m_nXOffset + (left + right) / 2 + sin(M_PI / 5 * 9) * c;
          ps[9].y = m_nYOffset + (top + bottom) / 2 - cos(M_PI / 5 * 9) * c;
          C->Polygon(ps, 9);
        } break;
        default:
          C->RoundRect(m_nXOffset + left, m_nYOffset + top, m_nXOffset + right,
                       m_nYOffset + bottom, (m_nFontHeight * 2) / 3,
                       (m_nFontHeight * 2) / 3);
        }
        drawtimes--;
        if (SettingView.m_bCardGradation && !(option & 0x1)) {
          if (rgnexist) {
            ::SelectClipRgn(C->Handle, RgnBak);
          } else {
            ::SelectClipRgn(C->Handle, NULL);
          }
          ::DeleteObject(RgnBak);
          ::DeleteObject(MyRgn);
        }
      }

      // Fixed card display
      if (Card->m_bFixed) {
        P->Style = psSolid;
        P->Width = m_fFontZoom;
        B->Style = bsSolid;
        if ((option & 0x1) == 0) {
          // Not shadow
          P->Color = HalfColor(0x0000ff, B->Color, 0.5f);
          B->Color = TColor(0x0000ff);
        }
        C->Ellipse(m_nXOffset + left + Space / 2, m_nYOffset + top + Space / 2,
                   m_nXOffset + left + Space / 4, m_nYOffset + top + Space / 4);
        C->Ellipse(m_nXOffset + right - Space / 2, m_nYOffset + top + Space / 2,
                   m_nXOffset + right - Space / 4,
                   m_nYOffset + top + Space / 4);
        C->Ellipse(
            m_nXOffset + left + Space / 2, m_nYOffset + bottom - Space / 2,
            m_nXOffset + left + Space / 4, m_nYOffset + bottom - Space / 4);
        C->Ellipse(
            m_nXOffset + right - Space / 2, m_nYOffset + bottom - Space / 2,
            m_nXOffset + right - Space / 4, m_nYOffset + bottom - Space / 4);
      }

      // Drawing
      if (Drawing) {
        if ((option & 0x1) == 0) {
          HRGN MyRgn, RgnBak;
          MyRgn = ::CreateRectRgn(
              m_nXOffset + left + Space / 2, m_nYOffset + top + Space / 2,
              m_nXOffset + left + Space / 2 +
                  (SettingView.m_nImageLimitation * imagex) / 100,
              m_nYOffset + top + Space / 2 +
                  (SettingView.m_nImageLimitation * imagex) / 100 * 0.75);
          RgnBak = ::CreateRectRgn(0, 0, 0, 0);
          int exist = ::GetClipRgn(C->Handle, RgnBak);
          if (exist) {
            HRGN CmbRgn;
            CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
            ::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
            ::SelectClipRgn(C->Handle, CmbRgn);
            ::DeleteObject(CmbRgn);
          } else {
            ::SelectClipRgn(C->Handle, MyRgn);
          }

          TRect R =
              Rect(m_nXOffset + left + Space / 2,
                   m_nYOffset + top + Space / 2 -
                       (SettingView.m_nImageLimitation * imagex) / 100 * 0.125,
                   m_nXOffset + left + Space / 2 +
                       (SettingView.m_nImageLimitation * imagex) / 100,
                   m_nYOffset + top + Space / 2 +
                       (SettingView.m_nImageLimitation * imagex) / 100 * 0.875);

          Card->m_Drawing->Draw(C, R);

          if (exist) {
            ::SelectClipRgn(C->Handle, RgnBak);
          } else {
            ::SelectClipRgn(C->Handle, NULL);
          }
          ::DeleteObject(RgnBak);
          ::DeleteObject(MyRgn);
        } /* else{
         // Shadow
         B->Style = bsSolid;
         C->FillRect(
         Rect(
         m_nXOffset + left + Space / 2,
         m_nYOffset + top + Space / 2,
         m_nXOffset + left + Space / 2 + SettingView.m_nImageLimitation,
         m_nYOffset + top + Space / 2 + SettingView.m_nImageLimitation * 0.75
         ));
         }
         */
      }

      // Video Drawing
      /*
       if (Video){
       if ((option & 0x1) == 0){
       // Video draw
       Graphics::TBitmap *BMP = new Graphics::TBitmap();
       Video->m_Video->GetBMP(BMP);
       if (imagex == 100){
       C->Draw(
       m_nXOffset + left + Space / 2,
       m_nYOffset + top + Space / 2,
       BMP
       );
       }else{
       C->StretchDraw(
       Rect(
       m_nXOffset + left + Space / 2,
       m_nYOffset + top + Space / 2,
       m_nXOffset + left + Space / 2 + (imagex * vi->bmiHeader.biWidth) / 100,
       m_nYOffset + top + Space / 2 + (imagex * vi->bmiHeader.biHeight) / 100
       ),
       BMP
       );
       }
       delete BMP;
       }else{
       // Shadow
       B->Style = bsSolid;
       C->FillRect(
       Rect(
       m_nXOffset + left + Space / 2,
       m_nYOffset + top + Space / 2,
       m_nXOffset + left + Space / 2 + (imagex * vi->bmiHeader.biWidth) / 100,
       m_nYOffset + top + Space / 2 + (imagex * vi->bmiHeader.biHeight) / 100
       ));
       }
       }
       */

      // Image Drawing
      if (Image) {
        if ((option & 0x1) == 0) {
          if (imagex == 100) {
            C->Draw(m_nXOffset + left + Space / 2, m_nYOffset + top + Space / 2,
                    Image->m_Image);
          } else {
            C->StretchDraw(Rect(m_nXOffset + left + Space / 2,
                                m_nYOffset + top + Space / 2,
                                m_nXOffset + left + Space / 2 +
                                    (imagex * Image->m_Image->Width) / 100,
                                m_nYOffset + top + Space / 2 +
                                    (imagex * Image->m_Image->Height) / 100),
                           Image->m_Image);
          }
        } else {
          // Shadow
          B->Style = bsSolid;
          C->FillRect(Rect(m_nXOffset + left + Space / 2,
                           m_nYOffset + top + Space / 2,
                           m_nXOffset + left + Space / 2 +
                               (imagex * Image->m_Image->Width) / 100,
                           m_nYOffset + top + Space / 2 +
                               (imagex * Image->m_Image->Height) / 100));
        }
      }

      // Text Drawing
      B->Style = bsClear;
      TColor fcbak = F->Color;
      {
        float X = m_nXOffset + left + Space / 2;
        float Y =
            m_nYOffset + bottom - Space / 2 - lineheight * lines - tickerheight;
        if (m_CardShape[cardindex] >= 9 && m_CardShape[cardindex] <= 15) {
          /*
           // Shadowed
           {
           int d;
           d = Space / 16;
           if (d < 1){
           d = 1;
           }
           TColor bkcolor = F->Color;
           F->Color = (TColor)SettingView.m_nBackgroundColor;
           for (int i = 0 ; i < Title->Count ; i++){
           C->TextOut(X + d, Y + i * lineheight + d, Title->Strings[i]);
           }
           F->Color = bkcolor;
           }
           */
          // *
          // Outline
          BeginPath(C->Handle);
          for (int i = 0; i < Title->Count; i++) {
            C->TextOut(X, Y + i * lineheight, Title->Strings[i]);
          }
          EndPath(C->Handle);
          P->Color = F->Color;
          P->Width = (abs(F->Height) / 8);
          if (P->Width < 4) {
            P->Width = 4;
          }
          StrokeAndFillPath(C->Handle);
          if ((option & 0x1) == 0) {
            // Not shadow
            F->Color = (TColor)SettingView.m_nBackgroundColor;
          }
          // */
          /*
           {
           int d;
           d = Space / 16;
           if (d < 1){
           d = 1;
           }
           for (int i = 0 ; i < Title->Count ; i++){
           C->TextOut(X - d, Y + i * lineheight, Title->Strings[i]);
           C->TextOut(X, Y + i * lineheight - d, Title->Strings[i]);
           C->TextOut(X + d, Y + i * lineheight, Title->Strings[i]);
           C->TextOut(X, Y + i * lineheight + d, Title->Strings[i]);
           }
           d = Space / 22.6272;
           if (d < 1){
           d = 1;
           }
           for (int i = 0 ; i < Title->Count ; i++){
           C->TextOut(X - d, Y + i * lineheight - d, Title->Strings[i]);
           C->TextOut(X + d, Y + i * lineheight - d, Title->Strings[i]);
           C->TextOut(X + d, Y + i * lineheight + d, Title->Strings[i]);
           C->TextOut(X - d, Y + i * lineheight + d, Title->Strings[i]);
           }
           F->Color = (TColor)SettingView.m_nBackgroundColor;
           }
           */
          for (int i = 0; i < Title->Count; i++) {
            C->TextOut(X, Y + i * lineheight, Title->Strings[i]);
          }
        } else if (m_CardShape[cardindex] >= 3 || ((option & 0x1) == 0)) {
          for (int i = 0; i < Title->Count; i++) {
            C->TextOut(X, Y + i * lineheight, Title->Strings[i]);
            // WideString W = Title->Strings[i];
            // TextOutW(C->Handle, X, Y + i * lineheight, W, W.Length());
          }
        }
      }
      F->Color = fcbak;

      // TickerDisplay
      if (SettingView.m_bTickerVisible && tickerheight > 0 && ticker) {
        int FontHeightBak = F->Height;

        F->Height *= 0.8f;
        float X = m_nXOffset + right - Space / 2;
        float Y = m_nYOffset + bottom - Space / 2 - tickerheight;
        UnicodeString S;
        for (int il = 0;
             il < SettingView.m_nTickerLines && il < Card->m_Lines->Count;
             il++) {
          S += Card->m_Lines->Strings[il];
          S += " ";
        }
        int tickerwidth = C->TextWidth(S);
        if (Card->m_fTickerPos * m_nFontHeight >
            tickerwidth + m_CardWidth[cardindex] - Space) {
          Card->m_fTickerPos = 0.0f;
        }
        Card->m_fTickerPos += m_fTickerSpeed;

        /*
         HRGN MyRgn, RgnBak;
         MyRgn = ::CreateRectRgn(m_nXOffset + left + Space / 2, Y, m_nXOffset +
         right - Space / 2, Y + tickerheight); RgnBak = ::CreateRectRgn(0, 0, 0,
         0); int exist = ::GetClipRgn(C->Handle, RgnBak); if (exist){ HRGN
         CmbRgn; CmbRgn = ::CreateRectRgn(0, 0, 0, 0);
         ::CombineRgn(CmbRgn, MyRgn, RgnBak, RGN_AND);
         ::SelectClipRgn(C->Handle, CmbRgn);
         ::DeleteObject(CmbRgn);
         }else{
         ::SelectClipRgn(C->Handle, MyRgn);
         }
         C->TextOut(X - Card->m_fTickerPos * m_nFontHeight, Y, S);
         if (exist){
         ::SelectClipRgn(C->Handle, RgnBak);
         }else{
         ::SelectClipRgn(C->Handle, NULL);
         }
         ::DeleteObject(RgnBak);
         ::DeleteObject(MyRgn);
         // */

        if (m_CardShape[cardindex] >= 3 || ((option & 0x1) == 0)) {
          C->TextRect(Rect(m_nXOffset + left + Space / 2, Y,
                           m_nXOffset + right - Space / 2, Y + tickerheight),
                      X - Card->m_fTickerPos * m_nFontHeight, Y, S);
        }
        F->Height = FontHeightBak;
      }

      // ScoreDrawing
      if (SettingView.m_bScore) {
        if (m_CardShape[cardindex] >= 3 || ((option & 0x1) == 0)) {
          if (Card) {
            float Y = m_nYOffset + bottom - Space / 2;
            float score = 0.0f;
            if (Card->m_fScore > 0.0f && m_fMaxScore - m_fMinScore > 0.0f) {
              score = (log(Card->m_fScore) - m_fMinScore) /
                      (m_fMaxScore - m_fMinScore) * 5.0f;
            }
            B->Style = bsSolid;
            if ((option & 0x1) == 0) {
              // Not shadow
              B->Color = clRed;
            } else {
              B->Color = P->Color;
            }
            C->FillRect(Rect(m_nXOffset + left + Space / 2, Y,
                             m_nXOffset + left + Space / 2 + Space * score,
                             Y + Space / 4));
          }
        }
      }

      // FoldbuttonDrawing
      if (Card)
        if ((option & 0x1) == 0 && TreeMode() && Card->m_bHasChild) {
          P->Style = psSolid;
          B->Style = bsSolid;
          P->Color = PColor;
          P->Width = m_fFontZoom;
          B->Color = HalfColor(BColor, 0xffffff, 0.5f);
          int fleft = m_nXOffset + right - m_nFontHeight;
          int ftop = m_nYOffset + bottom - m_nFontHeight / 2;
          int fsize = (m_nFontHeight * 2) / 3;
          fsize = fsize / 2 * 2 + 1;
          if (fsize < 13) {
            fsize = 13;
          }
          int fright = m_nXOffset + right - m_nFontHeight + fsize;
          int fbottom = m_nYOffset + bottom - m_nFontHeight / 2 + fsize;
          C->Rectangle(fleft, ftop, fright, fbottom);
          P->Color = TColor(m_nFGColor);
          C->MoveTo(fleft + fsize / 4, (ftop + fbottom) / 2);
          C->LineTo(fright - fsize / 4, (ftop + fbottom) / 2);
          if (Card->m_bFold) {
            C->MoveTo((fleft + fright) / 2, ftop + fsize / 4);
            C->LineTo((fleft + fright) / 2, fbottom - fsize / 4);
          }
        }
    }
    F->Height = OrgFontHeight;
  }

  delete Title;

  // FocusCursor
  if (Card && (option & 0x1) == 0) {
    // Card exists and not shadow
    if (Card->m_nSelected && Card->m_nID != m_nLastTarget &&
        Card->m_nID != m_nTargetCard) {
      // During selection animation, not selected card etc.
      int pos = m_nFocusCursorPos;
      float br = 0.0f;
      if (pos % 100 >= 50 && Card->m_nSelected == 100) {
        br = 0.33f;
      }

      // Position and size determination
      float ratio = Card->m_nSelected * 0.01f;
      float fx, fy, fw, fh;
      float addsize; // 0 to max 1 (lighter as larger)
      fx = m_CardX[cardindex];
      fy = m_CardY[cardindex];
      addsize = 1.0f - ratio;
      fw = m_CardWidth[cardindex];
      fh = m_CardHeight[cardindex];

      fw += m_nFontHeight * (addsize * 10.0f + 1);
      fh += m_nFontHeight * (addsize * 10.0f + 1);

      P->Color = HalfColor(m_nFGColor, m_nBGColor, addsize + br);

      DrawFocusCursor(C, fx, fy, fw, fh);
    }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawFocusCursor(TCanvas *C, float fx, float fy, float fw,
                               float fh) {
  TPen *P = C->Pen;

  int space = (m_nFontHeight * 2) / 3;
  TColor color = P->Color;

  for (int i = 0; i < 2; i++) {
    if (i == 0) {
      P->Color = TColor(m_nBGColor);
      P->Width = m_fFontZoom * 5;
    } else {
      P->Color = color;
      P->Width = m_fFontZoom * 3;
    }
    // Top left
    C->MoveTo(m_nXOffset + fx - fw * 0.5f + space, m_nYOffset + fy - fh * 0.5f);
    C->LineTo(m_nXOffset + fx - fw * 0.5f, m_nYOffset + fy - fh * 0.5f);
    C->LineTo(m_nXOffset + fx - fw * 0.5f, m_nYOffset + fy - fh * 0.5f + space);

    // Top right
    C->MoveTo(m_nXOffset + fx + fw * 0.5f - space, m_nYOffset + fy - fh * 0.5f);
    C->LineTo(m_nXOffset + fx + fw * 0.5f, m_nYOffset + fy - fh * 0.5f);
    C->LineTo(m_nXOffset + fx + fw * 0.5f, m_nYOffset + fy - fh * 0.5f + space);

    // Bottom left
    C->MoveTo(m_nXOffset + fx - fw * 0.5f + space, m_nYOffset + fy + fh * 0.5f);
    C->LineTo(m_nXOffset + fx - fw * 0.5f, m_nYOffset + fy + fh * 0.5f);
    C->LineTo(m_nXOffset + fx - fw * 0.5f, m_nYOffset + fy + fh * 0.5f - space);

    // Bottom right
    C->MoveTo(m_nXOffset + fx + fw * 0.5f - space, m_nYOffset + fy + fh * 0.5f);
    C->LineTo(m_nXOffset + fx + fw * 0.5f, m_nYOffset + fy + fh * 0.5f);
    C->LineTo(m_nXOffset + fx + fw * 0.5f, m_nYOffset + fy + fh * 0.5f - space);
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawPatternLine(TCanvas *C, int Pattern, int X1, int Y1, int X2,
                               int Y2, int penwidth) {
  // Line length
  float xd = X2 - X1;
  float yd = Y2 - Y1;
  float len = sqrt(xd * xd + yd * yd);
  if (len == 0.0f) {
    return;
  }
  float xstep = xd / len;
  float ystep = yd / len;

  int Pat[16];
  int PatNum;
  switch (Pattern) {
  case 0: // 3-3-dotted line
    Pat[0] = 3;
    Pat[1] = 3;
    PatNum = 2;
    break;
  case 1: // 9-3-dashed line
    Pat[0] = 9;
    Pat[1] = 3;
    PatNum = 2;
    break;
  case 2: // 9-3-3-dashed line
    Pat[0] = 9;
    Pat[1] = 3;
    Pat[2] = 3;
    Pat[3] = 3;
    PatNum = 4;
    break;
  case 3: // 9-3-3-dashed line
    Pat[0] = 9;
    Pat[1] = 3;
    Pat[2] = 3;
    Pat[3] = 3;
    Pat[4] = 3;
    Pat[5] = 3;
    PatNum = 6;
    break;
  }

  for (int i = 0; i < PatNum; i++) {
    Pat[i] *= penwidth;
  }

  float pos = 0.0f;
  int ppos = 0;
  while (pos < len - 1) {
    C->MoveTo(X1 + xstep * pos, Y1 + ystep * pos);
    pos += Pat[ppos++];
    if (pos > len) {
      pos = len;
    }
    C->LineTo(X1 + xstep * pos, Y1 + ystep * pos);
    pos += Pat[ppos++];
    if (ppos >= PatNum) {
      ppos = 0;
    }
  }
}

void CurvePosToXY(float pos, float &x0, float &y0) {
  /*
   x0 = sin(pos) * 0.5;
   if (pos > M_PI * 0.5) {
   x0 = 1.0f - x0;
   }
   y0 = (sin(pos - M_PI * 0.5) + 1.0) * 0.5;
   */
  x0 = sqrt(fabs(sin(pos))) * 0.5;
  if (pos > M_PI * 0.5) {
    x0 = 1.0f - x0;
  }
  y0 = (sin(pos - M_PI * 0.5) + 1.0) * 0.5;
}

void TFo_Main::DrawCurvedLine(TCanvas *C, int Pattern, int X1, int Y1, int X2,
                              int Y2, int penwidth, int direction) {
  // Curve drawing
  // Line length
  float xd = X2 - X1;
  float yd = Y2 - Y1;
  float len = sqrt(xd * xd + yd * yd) * sqrt(2);
  if (len == 0.0f) {
    return;
  }
  float radstep = M_PI / len;

  int Pat[16];
  int PatNum;
  switch (Pattern) {
  case 0: // 3-3-dotted line
    Pat[0] = 3;
    Pat[1] = 3;
    PatNum = 2;
    break;
  case 1: // 9-3-dashed line
    Pat[0] = 9;
    Pat[1] = 3;
    PatNum = 2;
    break;
  case 2: // 9-3-3-dashed line
    Pat[0] = 9;
    Pat[1] = 3;
    Pat[2] = 3;
    Pat[3] = 3;
    PatNum = 4;
    break;
  case 3: // 9-3-3-dashed line
    Pat[0] = 9;
    Pat[1] = 3;
    Pat[2] = 3;
    Pat[3] = 3;
    Pat[4] = 3;
    Pat[5] = 3;
    PatNum = 6;
    break;
  default:
    Pat[0] = 2;
    PatNum = 1;
  }

  TList *Ps = NULL;
  if (Pattern >= 4) {
    Ps = new TList();
  }

  for (int i = 0; i < PatNum; i++) {
    Pat[i] *= penwidth;
  }

  float pos = 0.0f; // < PI
  int ppos = 0;
  while (pos < M_PI) {
    float lastx, lasty;
    {
      float x0, y0;
      CurvePosToXY(pos, x0, y0);

      lastx = xd * x0;
      lasty = yd * y0;
      if (Pattern < 4) {
        C->MoveTo(X1 + lastx, Y1 + lasty);
      } else {
        int ix = *(int *)&lastx, iy = *(int *)&lasty;
        Ps->Add((void *)(intptr_t)ix);
        Ps->Add((void *)(intptr_t)iy);
      }
    }

    while (pos < M_PI) {
      pos += radstep;
      float x0, y0;
      CurvePosToXY(pos, x0, y0);

      if ((lastx - xd * x0) * (lastx - xd * x0) +
              (lasty - yd * y0) * (lasty - yd * y0) >
          Pat[ppos] * Pat[ppos]) {
        break;
      }
    }
    ppos++;
    // pos += Pat[ppos++] * radstep;
    if (pos > M_PI) {
      pos = M_PI;
    }
    {
      float x0, y0;
      CurvePosToXY(pos, x0, y0);

      lastx = xd * x0;
      lasty = yd * y0;
      if (Pattern < 4) {
        C->LineTo(X1 + lastx, Y1 + lasty);
      } else {
        int ix = *(int *)&lastx, iy = *(int *)&lasty;
        Ps->Add((void *)(intptr_t)ix);
        Ps->Add((void *)(intptr_t)iy);
      }
    }
    if (ppos < PatNum) {
      while (pos < M_PI) {
        pos += radstep;
        float x0 = sin(pos) * 0.5;
        if (pos > M_PI * 0.5) {
          x0 = 1.0f - x0;
        }
        float y0 = (sin(pos - M_PI * 0.5) + 1.0) * 0.5;
        if ((lastx - xd * x0) * (lastx - xd * x0) +
                (lasty - yd * y0) * (lasty - yd * y0) >
            Pat[ppos] * Pat[ppos]) {
          break;
        }
      }
      ppos++;
      // pos += Pat[ppos++] * radstep;
    }
    if (ppos >= PatNum) {
      ppos = 0;
    }
  }

  if (Ps) {
    int count = Ps->Count / 2;
    TPoint *ps = new TPoint[count * 2];
    TPen *P = C->Pen;
    TBrush *B = C->Brush;
    P->Style = psClear;
    B->Style = bsSolid;
    B->Color = P->Color;
    for (int i2 = 0; i2 < 2; i2++) {
      float rad = 0.0f;
      for (int i = 0; i < count; i++) {
        int l;
        l = (int)(intptr_t)Ps->Items[i * 2];
        float X1_ = *(float *)&l;
        l = (int)(intptr_t)Ps->Items[i * 2 + 1];
        float Y1_ = *(float *)&l;
        if (i < count - 1) {
          l = (int)(intptr_t)Ps->Items[(i + 1) * 2];
          float X2_ = *(float *)&l;
          l = (int)(intptr_t)Ps->Items[(i + 1) * 2 + 1];
          float Y2_ = *(float *)&l;
          if (Y2_ != Y1_ || X2_ != X1_) {
            rad = atan2(Y2_ - Y1_, X2_ - X1_);
          }
        }
        float size = m_nFontHeight / 6.0f;
        if (direction & 0x1) {
          size = ((count - i) * m_nFontHeight) / (3.0f * count) - i2 * 0.5;
        }
        if (direction & 0x2) {
          size += P->Width * 0.33f;
        }
        if (i2) {
          ps[i].x = X1 + X1_ - cos(rad + M_PI * 0.5) * size;
          ps[i].y = Y1 + Y1_ - sin(rad + M_PI * 0.5) * size;
        } else {
          ps[i].x = X1 + X1_ - cos(rad - M_PI * 0.5) * size;
          ps[i].y = Y1 + Y1_ - sin(rad - M_PI * 0.5) * size;
        }
        ps[count * 2 - 1 - i].x = X1 + X1_;
        ps[count * 2 - 1 - i].y = Y1 + Y1_;
      }
      C->Polygon(ps, count * 2 - 1);
      B->Color = HalfColor(B->Color, 0xffffff, 0.25f);
    }
    delete[] ps;
    delete Ps;
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawLink2(TCanvas *C, TLink *Link, int X1, int Y1, int X2,
                         int Y2, TColor HMColor, int option) {
  // Link drawing

  if (!SettingView.m_bLinkVisible) {
    // Don't draw
    return;
  }

  TPen *P = C->Pen;
  TBrush *B = C->Brush;
  int penwidth = P->Width;

  int shape = SettingView.m_nLinkDefaultShape;
  if (Link) {
    shape = Link->m_nShape;
  }

  float rad = 0.0;
  if (Y2 != Y1 || X2 != X1) {
    if (shape < 6) {
      // Straight line
      rad = atan2(Y2 - Y1, X2 - X1);
    } else {
      // Curve
      rad = atan2(pow(abs(Y2 - Y1), 1.73) * (-1 + (Y2 > Y1) * 2), X2 - X1);
    }
  }

  bool barrow;
  if (Link) {
    barrow = Link->m_bDirection;
  } else {
    barrow = true;
  }
  barrow &= SettingView.m_bLinkDirectionVisible;

  switch (shape) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
    // Normal arrow
    {
      P->Style = psSolid;
      // P->Style = psDashDot;

      if (option & 0x2) {
        // Outline
        TColor pcbak = P->Color;
        int pwbak = P->Width;
        P->Color = TColor(HMColor); // InverseColor(pcbak);
        P->Width = C->Pen->Width * 3;
        // int FontHeightBak = m_nFontHeight;

        switch (shape) {
        case 1:
        case 2:
        case 3:
        case 4:
          DrawPatternLine(C, shape - 1, X1, Y1, X2, Y2, penwidth);
          break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
          DrawCurvedLine(C, shape - 7, X1, Y1, X2, Y2, penwidth, barrow | 0x2);
          break;
        default:
          C->MoveTo(X1, Y1);
          C->LineTo(X2, Y2);
          break;
        }

        // Link outline
        if (barrow && shape < 11) {
          float mX = (X1 + X2) / 2;
          float mY = (Y1 + Y2) / 2;
          if (Y2 != Y1 || X2 != X1) {
            C->MoveTo(mX - cos(rad + 0.1 * 2 * M_PI) * (m_nFontHeight * 2) / 3,
                      mY - sin(rad + 0.1 * 2 * M_PI) * (m_nFontHeight * 2) / 3);
            C->LineTo(mX, mY);
            C->MoveTo(mX - cos(rad - 0.1 * 2 * M_PI) * (m_nFontHeight * 2) / 3,
                      mY - sin(rad - 0.1 * 2 * M_PI) * (m_nFontHeight * 2) / 3);
            C->LineTo(mX, mY);
          }
        }
        P->Color = pcbak;
        P->Width = pwbak;
      }

      switch (shape) {
      case 1:
      case 2:
      case 3:
      case 4:
        DrawPatternLine(C, shape - 1, X1, Y1, X2, Y2, penwidth);
        break;
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
        DrawCurvedLine(C, shape - 7, X1, Y1, X2, Y2, penwidth, barrow);
        break;
      default:
        C->MoveTo(X1, Y1);
        C->LineTo(X2, Y2);
        break;
      }

      // Link direction
      if (barrow && shape < 11) {
        float mX = (X1 + X2) / 2;
        float mY = (Y1 + Y2) / 2;
        if (Y2 != Y1 || X2 != X1) {
          C->MoveTo(mX - cos(rad + 0.1 * 2 * M_PI) * (m_nFontHeight * 2) / 3,
                    mY - sin(rad + 0.1 * 2 * M_PI) * (m_nFontHeight * 2) / 3);
          C->LineTo(mX, mY);
          C->MoveTo(mX - cos(rad - 0.1 * 2 * M_PI) * (m_nFontHeight * 2) / 3,
                    mY - sin(rad - 0.1 * 2 * M_PI) * (m_nFontHeight * 2) / 3);
          C->LineTo(mX, mY);
        }
      }
    }
    break;
  case 5:
    // Wedge shape
    {
      if (option & 0x2) {
        // Outline
        TColor pcbak = P->Color;
        P->Color = TColor(HMColor); // InverseColor(pcbak);
        P->Style = psSolid;

        if (barrow) {
          TPoint ps[3];
          ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 3;
          ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 3;
          ps[1].x = X1 - cos(rad - M_PI * 0.5) * m_nFontHeight / 3;
          ps[1].y = Y1 - sin(rad - M_PI * 0.5) * m_nFontHeight / 3;

          ps[2].x = X2;
          ps[2].y = Y2;
          C->Polygon(ps, 2);
        } else {
          TPoint ps[4];
          ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[3].x = X2 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[3].y = Y2 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;

          ps[1].x = X1 - cos(rad - M_PI * 0.5) * m_nFontHeight / 6;
          ps[1].y = Y1 - sin(rad - M_PI * 0.5) * m_nFontHeight / 6;

          ps[2].x = X2 - cos(rad - M_PI * 0.5) * m_nFontHeight / 6;
          ps[2].y = Y2 - sin(rad - M_PI * 0.5) * m_nFontHeight / 6;
          C->Polygon(ps, 3);
        }

        P->Color = pcbak;
      }

      B->Color = P->Color;
      B->Style = bsSolid;
      P->Style = psClear;
      if (barrow) {
        TPoint ps[3];
        if (SettingView.m_bCardGradation) {
          ps[0].x = X1;
          ps[0].y = Y1;
        } else {
          ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 3;
          ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 3;
        }
        ps[1].x = X1 - cos(rad - M_PI * 0.5) * m_nFontHeight / 3;
        ps[1].y = Y1 - sin(rad - M_PI * 0.5) * m_nFontHeight / 3;

        ps[2].x = X2;
        ps[2].y = Y2;
        C->Polygon(ps, 2);
      } else {
        TPoint ps[4];
        if (SettingView.m_bCardGradation) {
          ps[0].x = X1;
          ps[0].y = Y1;
          ps[3].x = X2;
          ps[3].y = Y2;
        } else {
          ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[3].x = X2 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[3].y = Y2 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
        }

        ps[1].x = X1 - cos(rad - M_PI * 0.5) * m_nFontHeight / 6;
        ps[1].y = Y1 - sin(rad - M_PI * 0.5) * m_nFontHeight / 6;

        ps[2].x = X2 - cos(rad - M_PI * 0.5) * m_nFontHeight / 6;
        ps[2].y = Y2 - sin(rad - M_PI * 0.5) * m_nFontHeight / 6;
        C->Polygon(ps, 3);
      }

      if (SettingView.m_bCardGradation) {
        B->Color = HalfColor(P->Color, 0xffffff, 0.25f);
        if (barrow) {
          TPoint ps[3];
          ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 3;
          ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 3;
          ps[1].x = X1;
          ps[1].y = Y1;

          ps[2].x = X2;
          ps[2].y = Y2;
          C->Polygon(ps, 2);
        } else {
          TPoint ps[4];
          ps[0].x = X1 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[0].y = Y1 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[1].x = X1;
          ps[1].y = Y1;

          ps[3].x = X2 - cos(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[3].y = Y2 - sin(rad + M_PI * 0.5) * m_nFontHeight / 6;
          ps[2].x = X2;
          ps[2].y = Y2;
          C->Polygon(ps, 3);
        }
      }
    }
  }

  // Link name
  if (SettingView.m_bLinkNameVisible && Link) {
    if (Link->m_Name != "") {
      TBrush *B = C->Brush;
      B->Style = bsClear;

      float mX = (X1 + X2) / 2;
      float mY = (Y1 + Y2) / 2;

      UnicodeString S = Link->m_Name;
      if (mX > m_nBrowserWidth / 2) {
        C->TextOut(mX + m_nFontHeight / 2, mY - m_nFontHeight / 2, S);
      } else {
        C->TextOut(mX - m_nFontHeight / 2 - C->TextWidth(S),
                   mY - m_nFontHeight / 2, S);
      }
    }
  }
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawLink(TCanvas *C, TLink *Link, int card1index, int card2index,
                        TColor HMColor, int option) {
  // Link drawing

  // Link line
  float X1 = m_CardX[card1index];
  float Y1 = m_CardY[card1index];
  float X2 = m_CardX[card2index];
  float Y2 = m_CardY[card2index];

  if (!((X1 < 0 && X2 < 0) || (Y1 < 0 && Y2 < 0) ||
        (X1 > m_nBrowserWidth && X2 > m_nBrowserWidth) ||
        (Y1 > m_nBrowserHeight && Y2 > m_nBrowserHeight))) {
    X1 += m_nXOffset;
    Y1 += m_nYOffset;
    X2 += m_nXOffset;
    Y2 += m_nYOffset;

    DrawLink2(C, Link, X1, Y1, X2, Y2, HMColor, option);
  }
}
// ---------------------------------------------------------------------------

void __fastcall TFo_Main::PB_BrowserPaint(TObject *Sender) {
  if (PB_Browser->Tag) {
    return;
  }

  HRGN RgnBak = ::CreateRectRgn(0, 0, 0, 0);
  int rgnexist;

  bool err = false; // Error by AA

  // Size determination
  if (Fo_FullScreen->Visible) {
    if (Fo_FullScreen->Width > 0 && Fo_FullScreen->Height > 0) {
      m_nBrowserWidth = Fo_FullScreen->Width;
      m_nBrowserHeight = Fo_FullScreen->Height;
    }
    rgnexist = ::GetClipRgn(Fo_FullScreen->Canvas->Handle, RgnBak);
  } else {
    if (PB_Browser->Width > 0 && PB_Browser->Height > 0) {
      m_nBrowserWidth = PB_Browser->Width;
      m_nBrowserHeight = PB_Browser->Height;
    }
    rgnexist = ::GetClipRgn(PB_Browser->Canvas->Handle, RgnBak);
  }
  m_nXOffset = 0;
  m_nYOffset = 0;

  // Background buffer creation
  Graphics::TBitmap *BMP = new Graphics::TBitmap();
  BMP->Width = m_nBrowserWidth;
  BMP->Height = m_nBrowserHeight;

  m_fFontZoom = 1.0f;
  m_nBGColor = SettingView.m_nBackgroundColor;
  m_nFGColor = SettingView.m_nFourgroundColor;

  if (SettingView.m_bAntiAlias) {
    int countshift = 1;
    while ((1 << countshift) < SettingView.m_nAntiAliasX) {
      countshift++;
    }
    int count = SettingView.m_nAntiAliasX;
    int countshift2 = countshift + countshift;

    BMP->PixelFormat = pf32bit;

    m_nBrowserWidth = m_nBrowserWidth << countshift;
    m_nBrowserHeight = m_nBrowserHeight << countshift;

    // Create bitmap of multiple size
    Graphics::TBitmap *BMP2 = new Graphics::TBitmap();
    try {
      BMP2->Width = m_nBrowserWidth << countshift;
      BMP2->Height = m_nBrowserHeight << countshift;
      BMP2->PixelFormat = pf32bit;
    } catch (...) {
      SettingView.m_bAntiAlias = false;
      ShowMessage(MLText.AANotEnoughMemory);
      Redraw();
      err = true;
    }

    if (!err) {
      // Draw to BMP
      m_fFontZoom = SettingView.m_nAntiAliasX;
      PaintSub(BMP2->Canvas);
      /*
       Graphics::TBitmap *BMP3 = new Graphics::TBitmap();
       BMP3->Width = BMP2->Width;
       BMP3->Height = BMP2->Height;
       BMP3->PixelFormat = pf32bit;
       BMP3->Canvas->Draw(0, 0, BMP2);
       */
      // Anti-aliasing
      int **P2 = new int *[BMP2->Height];
      for (int iy = 0; iy < BMP2->Height; iy++) {
        P2[iy] = (int *)BMP2->ScanLine[iy];
      }
      for (int iy = 0; iy < BMP->Height; iy++) {
        int *PW = (int *)BMP->ScanLine[iy];
        for (int ix = 0; ix < BMP->Width; ix++) {
          int r = 0, g = 0, b = 0;
          for (int iy2 = 0; iy2 < count; iy2++) {
            int *PR = P2[(iy << countshift) + iy2] + (ix << countshift);
            int *PRE = PR + count;
            while (PR < PRE) {
              int c = *PR++;
              r += c & 0xff;
              g += c & 0xff00;
              b += c & 0xff0000;
            }
          }
          r = r >> countshift2;
          g = (g >> countshift2) & 0xff00;
          b = (b >> countshift2) & 0xff0000;
          *PW++ = r | g | b;
        }
      }
      delete[] P2;

      m_nBrowserWidth = BMP->Width;
      m_nBrowserHeight = BMP->Height;

      float x = 1.0f / count;
      for (int i = 0; i < m_Document->m_Cards->Count; i++) {
        m_CardX[i] *= x;
        m_CardY[i] *= x;
        m_CardWidth[i] *= x;
        m_CardHeight[i] *= x;
      }

      m_nFontHeight /= count;
      m_nOVWidth /= count;
      m_nOVXOffset /= count;
      m_nOVHeight /= count;
      m_nOVYOffset /= count;
      // delete BMP3;
      m_fFontZoom = 1.0f;
    }

    delete BMP2;
  } else {
    BMP->Width = m_nBrowserWidth;
    BMP->Height = m_nBrowserHeight;

    if (rgnexist) {
      ::SelectClipRgn(BMP->Canvas->Handle, RgnBak);
    }
    // Draw to BMP
    PaintSub(BMP->Canvas);
  }

  // Copy BMP to foreground
  if (!err) {
    if (Fo_FullScreen->Visible) {
      Fo_FullScreen->Canvas->Draw(0, 0, BMP);
    } else {
      PB_Browser->Canvas->Draw(0, 0, BMP);
    }
  }

  // Discard BMP
  delete BMP;

  ::DeleteObject(RgnBak);
}

// ---------------------------------------------------------------------------
void TFo_Main::DrawLabelCircleRect(TCanvas *C, bool drawtoporder,
                                   bool drawothers) {
  TBrush *B = C->Brush;
  TPen *P = C->Pen;
  TFont *F = C->Font;
  if (m_Document->m_Labels[0]->Count > 0 &&
      (SettingView.m_bLabelCircleVisible ||
       SettingView.m_bLabelRectangleVisible)) {
    // Specify each label's Date
    for (int il = 0; il < m_Document->m_Labels[0]->Count; il++) {
      TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
      Label->m_fTouched = 0.0;
    }
    // Card loop
    for (int i = 0; i < m_Document->m_Cards->Count; i++)
      if (m_CardVisible[i]) {
        TCard *Card = m_Document->GetCardByIndex(i);
        // Card label loop
        for (int il = 0; il < Card->m_Labels->Count; il++) {
          TCardLabel *Label =
              m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1);
          if (Label->m_fTouched < Card->m_fViewed) {
            Label->m_fTouched = Card->m_fViewed;
          }
        }
      }
    // For display
    m_Document->RefreshDateOrder_Label();

    int *draworder = new int[m_Document->m_Labels[0]->Count];
    memset(draworder, 0, sizeof(int) * m_Document->m_Labels[0]->Count);
    for (int i = 0; i < m_Document->m_Labels[0]->Count; i++) {
      int vo = m_Document->GetLabelByIndex(0, i)->m_nTouchedOrder;
      draworder[vo] = i;
    }

    bool first = true;
    if (SettingView.m_bLabelCircleVisible) {
      for (int i = 0; i < m_Document->m_Labels[0]->Count; i++) {
        int il = draworder[i];
        TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
        if (Label->m_bEnable && !Label->m_bFold) {
          // Label that is Enable and not Fold

          if ((first && drawtoporder) || (!first && drawothers)) {
            // Search
            float Mean[2], StDev[2], Cov = 0.0f;
            int count = 0;
            for (int i = 0; i < 2; i++) {
              Mean[i] = 0.0f;
              StDev[i] = 0.0f;
            }

            for (int i = 0; i < m_Document->m_Cards->Count; i++)
              if (m_CardVisible[i]) {
                TCard *Card = m_Document->GetCardByIndex(i);
                if (Card->m_Labels->Contain(il + 1)) {
                  Mean[0] += m_CardX[i];
                  Mean[1] += m_CardY[i];
                  count++;
                }
              }
            if (count) {
              for (int i = 0; i < 2; i++) {
                Mean[i] /= count;
              }

              // Variance, covariance
              for (int i = 0; i < m_Document->m_Cards->Count; i++)
                if (m_CardVisible[i]) {
                  TCard *Card = m_Document->GetCardByIndex(i);
                  if (Card->m_Labels->Contain(il + 1)) {
                    StDev[0] += (m_CardX[i] - Mean[0]) * (m_CardX[i] - Mean[0]);
                    StDev[1] += (m_CardY[i] - Mean[1]) * (m_CardY[i] - Mean[1]);
                    Cov += (m_CardX[i] - Mean[0]) * (m_CardY[i] - Mean[1]);
                  }
                }

              if (StDev[0] > 0.0f && StDev[1] > 0.0f) {
                for (int i = 0; i < 2; i++) {
                  StDev[i] = sqrt(StDev[i] / count);
                }
                Cov = Cov / count;

                if (StDev[0] < StDev[1] * 0.25) {
                  StDev[0] = StDev[1] * 0.25;
                }
                if (StDev[1] < StDev[0] * 0.25) {
                  StDev[1] = StDev[0] * 0.25;
                }

                // float correl = Cov / (StDev[0] * StDev[1]);

                P->Width = (int)(5 * m_fFontZoom);
                P->Color = HalfColor((TColor)Label->m_nColor, m_nBGColor, 0.5f);

                // Fill
                if (SettingView.m_bLabelFill &&
                    (Bu_ArrangeType->Tag % 100 == 3) &&
                    Bu_ArrangeType->Tag < 500) {
                  B->Style = bsSolid;
                  B->Color =
                      HalfColor((TColor)Label->m_nColor, m_nBGColor, 0.9f);
                } else {
                  B->Style = bsClear;
                }

                C->Ellipse((Mean[0] - StDev[0] * 2.1f) + m_nXOffset,
                           (Mean[1] - StDev[1] * 2.1f) + m_nYOffset,
                           (Mean[0] + StDev[0] * 2.1f) + m_nXOffset,
                           (Mean[1] + StDev[1] * 2.1f) + m_nYOffset);

                if (SettingView.m_bLabelNameVisible) {
                  F->Color = (TColor)Label->m_nColor;
                  F->Height =
                      (RE_Edit->Font->Height * Label->m_nSize * m_fFontZoom) *
                      0.01;
                  F->Style = TFontStyles() << fsBold;
                  C->TextOut(Mean[0] + m_nXOffset -
                                 C->TextWidth(Label->m_Name) * 0.5,
                             (Mean[1] - StDev[1] * 2.1f) + m_nYOffset +
                                 C->TextHeight(Label->m_Name) * 0.5,
                             Label->m_Name);
                }
              }
            }
          }
          first = false;
        }
      }
    } else if (SettingView.m_bLabelRectangleVisible) {
      // Rectangle enclosing labels

      for (int i = 0; i < m_Document->m_Labels[0]->Count; i++) {
        int il = draworder[i];
        TCardLabel *Label = m_Document->GetLabelByIndex(0, il);
        if (Label->m_bEnable && !Label->m_bFold) {
          // Label that is Enable and not Fold

          if ((first && drawtoporder) || (!first && drawothers)) {
            // Coordinates
            float MinX, MinY, MaxX, MaxY, OrgMinY;
            int count = 0;

            for (int i = 0; i < m_Document->m_Cards->Count; i++)
              if (m_CardVisible[i]) {
                TCard *Card = m_Document->GetCardByIndex(i);
                if (Card->m_Labels->Contain(il + 1)) {
                  if (count == 0 || MinX > m_CardX[i] - (m_CardWidth[i] >> 1)) {
                    MinX = m_CardX[i] - (m_CardWidth[i] >> 1);
                  }
                  if (count == 0 || OrgMinY > Card->m_fY) {
                    OrgMinY = Card->m_fY;
                  }
                  if (count == 0 ||
                      MinY > m_CardY[i] - (m_CardHeight[i] >> 1)) {
                    MinY = m_CardY[i] - (m_CardHeight[i] >> 1);
                  }
                  if (count == 0 || MaxX < m_CardX[i] + (m_CardWidth[i] >> 1)) {
                    MaxX = m_CardX[i] + (m_CardWidth[i] >> 1);
                  }
                  if (count == 0 ||
                      MaxY < m_CardY[i] + (m_CardHeight[i] >> 1)) {
                    MaxY = m_CardY[i] + (m_CardHeight[i] >> 1);
                  }
                  count++;
                }
              }
            if (count) {
              P->Width = (int)(5 * m_fFontZoom);
              P->Color = HalfColor((TColor)Label->m_nColor, m_nBGColor, 0.5f);
              B->Style = bsClear;

              // Fill
              if (SettingView.m_bLabelFill &&
                  (Bu_ArrangeType->Tag % 100 == 3) &&
                  Bu_ArrangeType->Tag < 500) {
                B->Style = bsSolid;
                B->Color = HalfColor((TColor)Label->m_nColor, m_nBGColor, 0.9f);
              } else {
                B->Style = bsClear;
              }

              int space = m_nFontHeight / 2;

              C->RoundRect(MinX + m_nXOffset - space, MinY + m_nYOffset - space,
                           MaxX + m_nXOffset + space, MaxY + m_nYOffset + space,
                           space, space);

              if (SettingView.m_bLabelNameVisible) {
                F->Color = (TColor)Label->m_nColor;
                F->Height =
                    (RE_Edit->Font->Height * Label->m_nSize * m_fFontZoom) *
                    0.01;
                F->Style = TFontStyles() << fsBold;
                B->Style = bsClear;
                if (OrgMinY < 0.4f) {
                  C->TextOut(((MinX + MaxX) / 2) + m_nXOffset -
                                 C->TextWidth(Label->m_Name) * 0.5,
                             MinY - space - space -
                                 C->TextHeight(Label->m_Name) + m_nYOffset,
                             Label->m_Name);
                } else {
                  C->TextOut(((MinX + MaxX) / 2) + m_nXOffset -
                                 C->TextWidth(Label->m_Name) * 0.5,
                             MaxY + space + space + m_nYOffset, Label->m_Name);
                }
              }
            }
          }
          first = false;
        }
      }
    }

    delete[] draworder;
  }
}

// ---------------------------------------------------------------------------
TColor TFo_Main::GetCardColor(TCard *Card, float &SizeX) {
  // Get card base color by label
  TColor c = (TColor)0;
  int count = 0;
  for (int il = 0; il < Card->m_Labels->Count; il++) {
    TCardLabel *Label =
        m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1);
    if (Label->m_bEnable) {
      c = HalfColor(c, (TColor)Label->m_nColor, 1.0f / (count + 1));
      SizeX *= m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1)
                   ->m_nSize /
               100.0f;
      count++;
    }
  }

  return c;
}

// ---------------------------------------------------------------------------
void TFo_Main::PaintSub(TCanvas *C) {
  SetCardVisible();

  // DeZoom
  float Zoom = pow(2, TB_Zoom->Position / 2000.0f);
  Sc_X->LargeChange = 10000 / Zoom;
  Sc_Y->LargeChange = 10000 / Zoom;
  Sc_X->SmallChange = 1000 / Zoom;
  Sc_Y->SmallChange = 1000 / Zoom;

  // State variable preparation
  if (m_nTmpCardsCount != m_Document->m_Cards->Count) {
    m_nTmpCardsCount = m_Document->m_Cards->Count;
    if (m_CardX != NULL) {
      delete[] m_CardVisible;
      delete[] m_LinkVisible;
      delete[] m_CardTitle;
      delete[] m_CardRelated;
      delete[] m_CardAssign;
      delete[] m_CardShape;
      delete[] m_CardX;
      delete[] m_CardY;
      delete[] m_CardWidth;
      delete[] m_CardHeight;
    }
    m_CardVisible = new bool[m_Document->m_Cards->Count];
    m_LinkVisible = new bool[m_Document->m_Links->Count];
    m_CardTitle = new UnicodeString[m_Document->m_Cards->Count];
    m_CardX = new float[m_Document->m_Cards->Count];
    m_CardY = new float[m_Document->m_Cards->Count];
    m_CardWidth = new int[m_Document->m_Cards->Count];
    m_CardHeight = new int[m_Document->m_Cards->Count];
    m_CardRelated = new bool[m_Document->m_Cards->Count]; // Related card
    m_CardAssign = new int[m_Document->m_Cards->Count];
    m_CardShape = new int[m_Document->m_Cards->Count];
  }
  // State variable preparation
  if (m_nTmpLinksCount != m_Document->m_Links->Count) {
    m_nTmpLinksCount = m_Document->m_Links->Count;
    if (m_LinkVisible != NULL) {
      delete[] m_LinkVisible;
    }
    m_LinkVisible = new bool[m_Document->m_Links->Count];
  }
  memset(m_CardRelated, 0, sizeof(bool) * m_Document->m_Cards->Count);
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    m_CardAssign[i] = i;
  }

  // Get each card's coordinates and normalize
  // float maxx = 0.5f, minx = 0.5f, maxy = 0.5f, miny = 0.5f;
  float addx =
      (Zoom * (0.05f - Sc_X->Position * 0.0001f) + 0.5f) * m_nBrowserWidth;
  float addy =
      (Zoom * (0.05f - Sc_Y->Position * 0.0001f) + 0.5f) * m_nBrowserHeight;
  float mpyx = Zoom * 0.9f * m_nBrowserWidth;
  float mpyy = Zoom * 0.9f * m_nBrowserHeight;
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);
    m_CardVisible[i] = Card->m_bVisible;
    m_CardTitle[i] = Card->m_Title;

    /*
     m_CardX[i] = (((x * 0.9f + 0.05f) - (Sc_X->Position - 5000.0f) * 0.0001f) -
     0.5f) * Zoom + 0.5f; m_CardX[i] = (x * 0.9f + 0.05f - Sc_X->Position *
     0.0001f) * Zoom + 0.5f; m_CardX[i] = Zoom * x * 0.9f + Zoom * 0.05f - Zoom
     * Sc_X->Position * 0.0001f + 0.5f; m_CardX[i] = Zoom * 0.9f * x + Zoom *
     (0.05f - Sc_X->Position * 0.0001f) + 0.5f;
     */
    m_CardX[i] = mpyx * Card->m_fX + addx;
    m_CardY[i] = mpyy * Card->m_fY + addy;

    /*
     float scrollx = (Sc_X->Position - 5000.0f) * 0.0001f;
     float x = Card->m_fX;
     x = x * 0.9f + 0.05f;
     x -= scrollx;
     x = (x - 0.5f) * Zoom + 0.5f;
     m_CardX[i] = x;

     float scrolly = (Sc_Y->Position - 5000.0f) * 0.0001f;
     float y = Card->m_fY;
     y = y * 0.9f + 0.05f;
     y -= scrolly;
     y = (y - 0.5f) * Zoom + 0.5f;
     m_CardY[i] = y;
     */

    m_CardShape[i] = Card->m_nShape;
  }

  // TCanvas *C = BMP->Canvas;
  TBrush *B = C->Brush;
  TPen *P = C->Pen;
  TFont *F = C->Font;
  F->Name = RE_Edit->Font->Name;
  F->Charset = RE_Edit->Font->Charset;
  F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);
  m_nFontHeight = C->TextHeight(" ");

  // Background fill
  if (fabs(m_fFontZoom - 1.0) < 0.1 &&
      !(Im_Wall->Enabled && SettingView.m_bFixWallPaper)) {
    B->Color = TColor(m_nBGColor);
    B->Style = bsSolid;
    C->FillRect(Rect(0, 0, m_nBrowserWidth, m_nBrowserHeight));
  }

  // Wallpaper display
  if (Im_Wall->Enabled) {
    if (SettingView.m_bFixWallPaper) {
      // Fixed display
      C->StretchDraw(Rect(m_nXOffset, m_nYOffset, m_nXOffset + m_nBrowserWidth,
                          m_nYOffset + m_nBrowserHeight),
                     Im_Wall->Picture->Graphic);
    } else {
      // Scroll display
      int startx = 0, starty = 0, endx = 0, endy = 0;
      if (SettingView.m_bTileWallPaper) {
        startx = -4;
        endx = 4;
        starty = -4;
        endy = 4;
      }
      for (int iy = starty; iy <= endy; iy++) {
        int top =
            ((iy - (Sc_Y->Position - 5000.0f) * 0.0001f - 0.5f) * Zoom + 0.5f) *
            m_nBrowserHeight;
        int bottom =
            ((iy + 1 - (Sc_Y->Position - 5000.0f) * 0.0001f - 0.5f) * Zoom +
             0.5f) *
            m_nBrowserHeight;
        if (bottom >= 0 && top < m_nBrowserHeight) {
          for (int ix = startx; ix <= endx; ix++) {
            int left =
                ((ix - (Sc_X->Position - 5000.0f) * 0.0001f - 0.5f) * Zoom +
                 0.5f) *
                m_nBrowserWidth;
            int right =
                ((ix + 1 - (Sc_X->Position - 5000.0f) * 0.0001f - 0.5f) * Zoom +
                 0.5f) *
                m_nBrowserWidth;
            if (right >= 0 && left < m_nBrowserWidth) {
              C->StretchDraw(Rect(left + m_nXOffset, top + m_nYOffset,
                                  right + m_nXOffset, bottom + m_nYOffset),
                             Im_Wall->Picture->Graphic);
            }
          }
        }
      }
    }
  }

  // Background animation
  if (SettingView.m_bBGAnimation) {
    BGAnimation(C);
  }

  if (SettingView.m_bTextVisible && m_nTargetCard >= 0) {
    // Card body drawing
    F->Color = (TColor)HalfColor(m_nBGColor, m_nFGColor, 0.33);
    B->Style = bsClear;
    int height = C->TextHeight(" ") * 1.3;
    int x = (m_nFontHeight * 2) / 3;
    int y = (m_nFontHeight * 2) / 3;
    if (SettingView.m_bTextCentering) {
      int maxwidth = 0;
      for (int i = 0;
           i < RE_Edit->Lines->Count && y + i * height < m_nBrowserHeight;
           i++) {
        int width = C->TextWidth(RE_Edit->Lines->Strings[i]);
        if (width > maxwidth) {
          maxwidth = width;
        }
      }
      x = (m_nBrowserWidth - maxwidth) >> 1;
      if (x < (m_nFontHeight * 2) / 3) {
        x = (m_nFontHeight * 2) / 3;
      }
      int maxheight = height * RE_Edit->Lines->Count;
      y = (m_nBrowserHeight - maxheight) >> 1;
      if (y < (m_nFontHeight * 2) / 3) {
        y = (m_nFontHeight * 2) / 3;
      }
    }
    for (int i = 0;
         i < RE_Edit->Lines->Count && y + i * height < m_nBrowserHeight; i++) {
      C->TextOut(x + m_nXOffset, y + height * i + m_nYOffset,
                 RE_Edit->Lines->Strings[i]);
    }
  }

  int *LinkCard1Index = new int[m_Document->m_Links->Count];
  int *LinkCard2Index = new int[m_Document->m_Links->Count];

  // LinkVisiblesetting
  for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++) {
    TLink *Link = m_Document->GetLinkByIndex(i2);

    LinkCard1Index[i2] = m_Document->SearchCardIndex(Link->m_nFromID);
    LinkCard2Index[i2] = m_Document->SearchCardIndex(Link->m_nDestID);

    bool visible = Link->m_bVisible;

    if (visible) {
      if (LinkCard1Index[i2] >= 0 && LinkCard2Index[i2] >= 0) {
        visible &= m_CardVisible[LinkCard1Index[i2]] &&
                   m_CardVisible[LinkCard2Index[i2]];
      } else {
        visible = false;
      }
    }

    m_LinkVisible[i2] = visible;
    Link->m_bVisible = visible;
  }

  // Score calc. LinkVisible needed at this position
  if (SettingView.m_bScore) {
    IterScore();
  }

  // Link relation setting
  for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
    if (m_LinkVisible[i2]) {
      TLink *Link = m_Document->GetLinkByIndex(i2);

      if (Link->m_nFromID != m_nTargetCard &&
          Link->m_nDestID != m_nTargetCard && i2 != m_nTargetLink) {
        // Unrelated link
      } else {
        m_CardRelated[LinkCard1Index[i2]] = true;
        m_CardRelated[LinkCard2Index[i2]] = true;
      }
    }
  if (m_nTargetCard2 >= 0) {
    int card1index = m_Document->SearchCardIndex(m_nTargetCard2);
    m_CardRelated[card1index] = true;
  }
  if (SettingView.m_bLinkLimitation && SettingView.m_nLinkTarget >= 0) {
    int card1index = m_Document->SearchCardIndex(SettingView.m_nLinkTarget);
    m_CardRelated[card1index] = true;
  }

  // Label fold: (cards with folded labels - if any label attached, set all
  // Related True; skip since related links shown later) If all-fold-label card
  // is selected: same-label cards and linked cards are all Related Lower
  // repulsion of folded cards If all labels on card are Fold: use label name as
  // card title Card loop: first check fold state of all cards Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);
      Card->m_bLabelIsFold = m_Document->LabelIsFold(Card);
    }

  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i]) {
      TCard *Card = m_Document->GetCardByIndex(i);
      if (Card->m_bLabelIsFold) {
        // All labels folded
        // Card title to label name
        UnicodeString S = "";
        for (int il = 0; il < Card->m_Labels->Count; il++) {
          TCardLabel *Label =
              m_Document->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1);
          if (Label->m_bEnable) {
            if (S != "") {
              S += ", ";
            }
            S += Label->m_Name;
          }
        }
        m_CardTitle[i] = S;

        if (Card->m_nID == m_nTargetCard) {
          // Selected card

          // Card loop: set all same-label cards Related
          m_CardAssign[i] = i;
          for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
            if (i != i2 && m_CardVisible[i2]) {
              TCard *Card2 = m_Document->GetCardByIndex(i2);

              if (m_Document->LabelIsSame(Card, Card2)) {
                // Same label structure card
                // Assign coords to last found same-label card
                m_CardAssign[i2] = i;

                m_CardRelated[i2] = true;

                // Set all cards linked from here Related
                for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
                  if (m_LinkVisible[i2]) {
                    TLink *Link = m_Document->GetLinkByIndex(i2);
                    if (Card2->m_nID == Link->m_nFromID) {
                      int idx = LinkCard2Index[i2];
                      if (m_CardVisible[idx]) {
                        m_CardRelated[idx] = true;
                      }
                    } else if (Card2->m_nID == Link->m_nDestID) {
                      int idx = LinkCard1Index[i2];
                      if (m_CardVisible[idx]) {
                        m_CardRelated[idx] = true;
                      }
                    }
                  }
              }
            }
        } else if (m_CardAssign[i] == i) {
          // Not selected card, representative card
          // Card loop: assign same-label cards to this card
          // Assign coords to first found same-label card; non-selected gets
          // back card
          for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
            if (i != i2 && m_CardVisible[i2]) {
              TCard *Card2 = m_Document->GetCardByIndex(i2);

              if (m_Document->LabelIsSame(Card, Card2)) {
                // Same label structure card
                // Assign coords to first found same-label card; non-selected
                // gets back card
                m_CardAssign[i2] = i;
              }
            }
        } else {
          // Neither selected nor representative

          int idxbak = m_CardAssign[i];

          // Card loop: make this card representative
          for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
            if (m_CardVisible[i2] && m_CardAssign[i2] == idxbak) {
              m_CardAssign[i2] = i;
            }
        }
      }
    }

  // If any Related card has Related True, set all Related cards True
  // Card loop
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i] && m_CardRelated[i]) {
      // Related card
      TCard *Card = m_Document->GetCardByIndex(i);

      if (Card->m_bLabelIsFold) {
        // All labels folded

        // Card loop: set all related cards Related
        for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
          if (m_CardAssign[i] == m_CardAssign[i2]) {
            m_CardRelated[i2] = true;
          }
      }
    }

  // Take Shape majority among Folded
  int *shapevote = new int[IL_Shape->Count];
  for (int i = 0; i < m_Document->m_Cards->Count; i++)
    if (m_CardVisible[i] && m_CardAssign[i] == i) {
      // Display target, representative card
      TCard *Card = m_Document->GetCardByIndex(i);

      if (Card->m_bLabelIsFold) {
        // Folded

        // Vote start
        memset(shapevote, 0, sizeof(int) * IL_Shape->Count);

        for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
          if (m_CardVisible[i] && m_CardAssign[i2] == i) {
            TCard *Card2 = m_Document->GetCardByIndex(i2);
            if (Card2->m_nShape < IL_Shape->Count) {
              shapevote[Card2->m_nShape]++;
            }
          }

        // Get shape with most votes
        int maxindex = 2;
        int max = 0;
        for (int i2 = 0; i2 < IL_Shape->Count; i2++) {
          if (shapevote[i2] > max) {
            max = shapevote[i2];
            maxindex = i2;
          }
        }

        // Apply
        for (int i2 = 0; i2 < m_Document->m_Cards->Count; i2++)
          if (m_CardVisible[i] && m_CardAssign[i2] == i) {
            m_CardShape[i2] = maxindex;
          }
      }
    }
  delete[] shapevote;

  int *draworder = new int[m_Document->m_Cards->Count];
  memset(draworder, 0, sizeof(int) * m_Document->m_Cards->Count);
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    int vo = m_Document->GetCardByIndex(i)->m_nViewedOrder;
    draworder[vo] = i;
  }

  // Search (half-mix color)
  // TColor HMColor = HalfColor(m_nBGColor, m_nFGColor, 0.125);
  TColor HMColor = TColor(m_nBGColor);

  // Pre-calc card size as needed
  // Shadow drawing
  if ((m_Document->m_Labels[0]->Count > 0 &&
       SettingView.m_bLabelRectangleVisible) || // Rectangle enclosing labels
      SettingView.m_bCardShadow) {
    for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
      int i = draworder[ic];
      if (m_CardVisible[i] && m_CardAssign[i] == i) {
        TCard *Card = m_Document->GetCardByIndex(i);

        if ((ic >= m_Document->m_Cards->Count - 3 && m_bShowRecent) ||
            m_nTargetCard == Card->m_nID) {
          // Emphasize recently displayed cards
          F->Style = TFontStyles() << fsBold;
        } else {
          F->Style = TFontStyles();
        }

        if (m_CardRelated[i]) {
          // Emphasize recently displayed cards
          P->Width = (int)(3 * m_fFontZoom);
        } else {
          P->Width = (int)(1 * m_fFontZoom);
        }

        float SizeX = Card->m_nSize;
        if (m_Document->CountEnableLabel(Card)) {
          // Has label
          for (int il = 0; il < Card->m_Labels->Count; il++) {
            TCardLabel *Label = m_Document->GetLabelByIndex(
                0, Card->m_Labels->GetLabel(il) - 1);
            if (Label->m_bEnable) {
              SizeX *=
                  m_Document
                      ->GetLabelByIndex(0, Card->m_Labels->GetLabel(il) - 1)
                      ->m_nSize /
                  100.0f;
            }
          }
        }
        if (SettingView.m_bMagnifyFocused && Card->m_nID == m_nTargetCard) {
          SizeX *= 1.2;
        }

        if (SettingView.m_bCardShadow) {
          int xoffsetbak = m_nXOffset;
          int yoffsetbak = m_nYOffset;
          m_nXOffset += m_nFontHeight / 2;
          m_nYOffset += m_nFontHeight / 2;

          // Shadow color (darkened mid-tone)
          P->Color =
              HalfColor(HalfColor(m_nFGColor, m_nBGColor, 0.5f), 0x0, 0.33f);
          B->Color = P->Color;
          B->Style = bsSolid;
          F->Color = P->Color;
          DrawCard(C, Card, (int)SizeX, i, HMColor, 0x1);

          m_nXOffset = xoffsetbak;
          m_nYOffset = yoffsetbak;
        } else {
          CalcCardSize(C, Card, (int)SizeX, i);
        }
      }
    }
  }

  int linkoption = 0;
  if (SettingView.m_bLinkShadow) {
    linkoption |= 0x1;
  }
  if (SettingView.m_bLinkHemming) {
    linkoption |= 0x2;
  }

  // Link shadow
  if (SettingView.m_bLinkShadow) {
    P->Style = psSolid;
    P->Width = (int)(1 * m_fFontZoom);
    F->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.25);

    int xoffsetbak = m_nXOffset;
    int yoffsetbak = m_nYOffset;
    m_nXOffset += m_nFontHeight / 2;
    m_nYOffset += m_nFontHeight / 2;

    for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
      if (m_LinkVisible[i2]) {
        TLink *Link = m_Document->GetLinkByIndex(i2);

        P->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.5);
        Link->m_Color = P->Color;
        DrawLink(C, Link, LinkCard1Index[i2], LinkCard2Index[i2], HMColor, 0x1);
      }

    m_nXOffset = xoffsetbak;
    m_nYOffset = yoffsetbak;
  }

  // Circle/rect enclosing labels
  DrawLabelCircleRect(C, !SettingView.m_bLavelCRFocusedOnTop, true);

  // Restore font
  F->Style = TFontStyles();
  F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);

  // Link drawing (unrelated)
  P->Width = (int)(3 * m_fFontZoom);
  F->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.5);

  for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
    if (m_LinkVisible[i2]) {
      TLink *Link = m_Document->GetLinkByIndex(i2);

      if (!(m_CardRelated[LinkCard1Index[i2]] &&
            m_CardRelated[LinkCard2Index[i2]])) {
        // Unrelated link

        if (Link->m_Labels->Count == 0) {
          P->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.25);
        } else {
          TColor c = (TColor)0;
          for (int il = 0; il < Link->m_Labels->Count; il++) {
            TCardLabel *Label = m_Document->GetLabelByIndex(
                1, Link->m_Labels->GetLabel(il) - 1);
            c = HalfColor(c, (TColor)Label->m_nColor, 1.0f / (il + 1));
          }
          P->Color = c;
        }

        if ((Bu_ArrangeType->Tag % 100 > 2 && Bu_ArrangeType->Tag <= 499) ||
            (Bu_ArrangeType->Tag >= 500 && Bu_ArrangeType->Tag <= 999)) {
          // Arranged link
          P->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
        } else if (Bu_ArrangeType->Tag >= 1000 && Bu_ArrangeType->Tag <= 1999) {
          // Hierarchy display
          TCard *From = m_Document->GetCard(Link->m_nFromID);
          TCard *Dest = m_Document->GetCard(Link->m_nDestID);
          if (From->m_nParentID != Dest->m_nID &&
              Dest->m_nParentID != From->m_nID) {
            P->Color = HalfColor(P->Color, m_nBGColor, 0.75f);
          }
        }

        // Unrelated link
        Link->m_Color = P->Color;
        DrawLink(C, Link, LinkCard1Index[i2], LinkCard2Index[i2], HMColor,
                 linkoption);
      }
    }

  // Card drawing (unrelated)
  P->Style = psSolid;
  B->Style = bsSolid;
  F->Color = (TColor)m_nFGColor;
  for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
    int i = draworder[ic];
    // int i = ic;
    TCard *Card = m_Document->GetCardByIndex(i);

    if (!m_CardRelated[i] && m_CardAssign[i] == i && Card->m_bVisible &&
        Card->m_nID != m_nTargetCard) {
      // Unrelated card
      if (ic >= m_Document->m_Cards->Count - 3 && m_bShowRecent) {
        // Emphasize recently displayed cards
        F->Style = TFontStyles() << fsBold;
        P->Width = (int)(3 * m_fFontZoom);
      } else {
        F->Style = TFontStyles();
        P->Width = (int)(1 * m_fFontZoom);
      }

      float SizeX = Card->m_nSize;
      if (m_Document->CountEnableLabel(Card)) {
        // Has label
        TColor c = GetCardColor(Card, SizeX);
        P->Color = HalfColor(c, m_nBGColor, 0.33f);
        B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
      } else {
        // No label
        P->Color = HalfColor(m_nFGColor, m_nBGColor, 0.5f);
        // B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor,
        // 0.75f);
        B->Color = HalfColor(P->Color, m_nBGColor, 0.875f);
      }

      Card->m_Color = P->Color;
      DrawCard(C, Card, (int)SizeX, i, HMColor, 0);
    }
  }

  // Link drawing (label exists)
  P->Style = psSolid;
  P->Width = (int)(3 * m_fFontZoom);
  F->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.25);
  F->Style = TFontStyles();
  for (int i2 = 0; i2 < m_Document->m_Links->Count; i2++)
    if (m_LinkVisible[i2] && m_nTargetLink != i2) {

      if (m_CardRelated[LinkCard1Index[i2]] &&
          m_CardRelated[LinkCard2Index[i2]]) {

        TLink *Link = m_Document->GetLinkByIndex(i2);

        if (Link->m_Labels->Count == 0) {
          P->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor,
                                       0.125); // (TColor)m_nFGColor;
        } else {
          TColor c = (TColor)0;
          for (int il = 0; il < Link->m_Labels->Count; il++) {
            TCardLabel *Label = m_Document->GetLabelByIndex(
                1, Link->m_Labels->GetLabel(il) - 1);
            c = HalfColor(c, (TColor)Label->m_nColor, 1.0f / (il + 1));
          }
          P->Color = c;
        }

        if ((Bu_ArrangeType->Tag % 100 > 2 && Bu_ArrangeType->Tag <= 499) ||
            (Bu_ArrangeType->Tag >= 500 && Bu_ArrangeType->Tag <= 999)) {
          // Dim link color when links are less important in Arrange
          P->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
        }

        // Related link
        Link->m_Color = P->Color;
        TColor HMColor2 = HalfColor(m_nFGColor, m_nBGColor, 0.5);
        // HalfColor(HMColor, m_nFGColor, 0.5)
        DrawLink(C, Link, LinkCard1Index[i2], LinkCard2Index[i2], HMColor2,
                 linkoption | 0x02);
      }
    }

  P->Color = (TColor)m_nFGColor;
  if (m_bMDownBrowser == 1 && m_nTargetCard >= 0 &&
      (SB_Line->Down || SB_ToolLinkLabel->Down)) {
    // Link add
    if (SB_ToolLinkLabel->Down && m_nToolLinkLabel > 0) {
      // Link label exists
      TCardLabel *Label = m_Document->GetLabelByIndex(1, m_nToolLinkLabel - 1);
      P->Color = (TColor)Label->m_nColor;
    }

    if (m_nTargetCard2 >= 0) {
      // Link add (has destination)
      int card1index = m_Document->SearchCardIndex(m_nTargetCard);
      int card2index = m_Document->SearchCardIndex(m_nTargetCard2);

      DrawLink(C, NULL, card1index, card2index, HMColor, linkoption);
    } else if (m_bMDownBrowser && m_nTargetCard >= 0) {
      // Link add (no destination)
      int card1index = m_Document->SearchCardIndex(m_nTargetCard);

      // Link add
      float X1 = m_CardX[card1index];
      float Y1 = m_CardY[card1index];
      DrawLink2(C, NULL, X1, Y1, m_nMDownTargetX, m_nMDownTargetY, HMColor,
                linkoption);
    }
  }

  // Link drawing (during operation)
  P->Color = (TColor)m_nFGColor;
  P->Style = psSolid;
  P->Width = (int)(3 * m_fFontZoom);
  F->Color = (TColor)m_nFGColor;
  if (m_nTargetLink >= 0) {
    // Selected
    int i2 = m_nTargetLink;
    TLink *Link = m_Document->GetLinkByIndex(i2);
    if (Link) {
      // Color by label
      if (Link->m_Labels->Count == 0) {
        P->Color = (TColor)m_nFGColor;
      } else {
        TColor c = (TColor)0;
        for (int il = 0; il < Link->m_Labels->Count; il++) {
          c = HalfColor(
              c,
              (TColor)m_Document
                  ->GetLabelByIndex(1, Link->m_Labels->GetLabel(il) - 1)
                  ->m_nColor,
              1.0f / (il + 1));
        }
        P->Color = c;
      }

      Link->m_Color = P->Color;
      if (!m_bMDownBrowserMoved || m_bMDownBrowser <= 1) {
        // Not dragging
        int pos = m_nFocusCursorPos;
        float br = 0.0f;
        if (pos % 100 < 50) {
          br = 0.4f;
        } else {
        }
        TColor HMColor2 = HalfColor(m_nFGColor, m_nBGColor, br);
        // HalfColor(HMColor, m_nFGColor, 0.5)
        DrawLink(C, Link, LinkCard1Index[m_nTargetLink],
                 LinkCard2Index[m_nTargetLink], HMColor2, linkoption | 0x2);
      } else
        switch (m_bMDownBrowser) {
        case 2:
          // Dest changing
          if (m_nTargetCard2 >= 0) {
            // Has destination
            int index = m_Document->SearchCardIndex(m_nTargetCard2);
            DrawLink(C, Link, LinkCard1Index[m_nTargetLink], index, HMColor,
                     linkoption);
          } else {
            // No destination
            float X1 = m_CardX[LinkCard1Index[m_nTargetLink]];
            float Y1 = m_CardY[LinkCard1Index[m_nTargetLink]];
            DrawLink2(C, Link, X1, Y1, m_nMDownTargetX, m_nMDownTargetY,
                      HMColor, linkoption);
          }
          break;
        case 3:
          // From changing
          if (m_nTargetCard2 >= 0) {
            // Has destination
            int index = m_Document->SearchCardIndex(m_nTargetCard2);
            DrawLink(C, Link, index, LinkCard2Index[m_nTargetLink], HMColor,
                     linkoption);
          } else {
            // No destination
            float X1 = m_CardX[LinkCard2Index[m_nTargetLink]];
            float Y1 = m_CardY[LinkCard2Index[m_nTargetLink]];
            DrawLink2(C, Link, m_nMDownTargetX, m_nMDownTargetY, X1, Y1,
                      HMColor, linkoption);
          }
          break;
        }
    }
  }

  delete[] LinkCard1Index;
  delete[] LinkCard2Index;

  // Card drawing (related exists)
  // for (int i = 0 ; i < m_Document->m_Cards->Count ; i++){
  P->Style = psSolid;
  B->Style = bsSolid;
  for (int ic = 0; ic < m_Document->m_Cards->Count; ic++) {
    int i = draworder[ic];
    TCard *Card = m_Document->GetCardByIndex(i);

    if (m_CardRelated[i] && m_CardAssign[i] == i &&
        Card->m_nID != m_nTargetCard && Card->m_bVisible) {
      // Related card
      P->Width = (int)(3 * m_fFontZoom);
      if (ic >= m_Document->m_Cards->Count - 3 && m_bShowRecent) {
        // Emphasize recently displayed cards
        F->Style = TFontStyles() << fsBold;
      } else {
        F->Style = TFontStyles();
      }
      float SizeX = Card->m_nSize;
      if (m_Document->CountEnableLabel(Card)) {
        // Has label
        TColor c = GetCardColor(Card, SizeX);
        P->Color = HalfColor(c, m_nBGColor, 0.25f);
        B->Color = HalfColor(P->Color, m_nBGColor, 0.5f);
      } else {
        P->Color = (TColor)HalfColor(m_nFGColor, m_nBGColor, 0.25f);
        // B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor,
        // 0.875f);
        B->Color = HalfColor(P->Color, m_nBGColor, 0.9375f);
      }

      Card->m_Color = P->Color;
      DrawCard(C, Card, (int)SizeX, i, HMColor, 0);
    }
  }

  // Label circle/rect (usually unused option)
  /*
   if (SettingView.m_bLavelCRFocusedOnTop){
   DrawLabelCircleRect(C, m_fFontZoom, m_nFGColor, m_nBGColor, true, false);

   // Restore font
   F->Style = TFontStyles();
   F->Height = (int)(RE_Edit->Font->Height * m_fFontZoom);

   // Redisplay cards with same label as card being edited
   }
   */

  delete[] draworder;

  // Card drawing (card being edited)
  for (int i = 0; i < m_Document->m_Cards->Count; i++) {
    TCard *Card = m_Document->GetCardByIndex(i);

    if (Card->m_nID == m_nTargetCard) {
      // Card being edited
      F->Style = TFontStyles() << fsBold;

      P->Width = (int)(3 * m_fFontZoom);
      float SizeX = Card->m_nSize;
      if (m_Document->CountEnableLabel(Card)) {
        // Has label
        TColor c = GetCardColor(Card, SizeX);
        P->Color = c;
        B->Color = HalfColor(P->Color, m_nBGColor, 0.33f);
      } else {
        P->Color = (TColor)m_nFGColor;
        // B->Color = TColor(m_nBGColor);//HalfColor(P->Color, m_nBGColor,
        // 0.875f);
        B->Color = HalfColor(P->Color, m_nBGColor, 0.96875);
      }

      if (SettingView.m_bMagnifyFocused) {
        SizeX *= 1.2;
      }

      Card->m_Color = P->Color;
      DrawCard(C, Card, (int)SizeX, i, HMColor, 0);
    }
  }

  // Focus cursor
  if (SettingView.m_bFocusCursor &&
      (m_nTargetCard >= 0 || m_nLastTarget >= 0)) {
    int current = m_Document->SearchCardIndex(m_nTargetCard);
    int last = m_Document->SearchCardIndex(m_nLastTarget);
    if (current >= 0 || last >= 0) {
      int pos = m_nFocusCursorPos;
      float br = 0.0f;
      if (pos > 100) {
        // Blink process
        if (pos % 100 < 50) {
          // pos = 100;
        } else {
          // pos = -1;
          br = 0.33f;
        }
        pos = 100;
      }

      if (pos >= 0) {
        // Position and size determination
        float ratio = pos * 0.01f;
        float fx, fy, fw, fh;
        float addsize; // 0 to max 10 (lighter as larger)
        if (last >= 0 && current >= 0) {
          // Current and previous card exist
          fx = m_CardX[last] * (1.0f - ratio) + m_CardX[current] * ratio;
          fy = m_CardY[last] * (1.0f - ratio) + m_CardY[current] * ratio;
          addsize = 0.5f - fabs(0.5f - ratio);
          fw =
              m_CardWidth[last] * (1.0f - ratio) + m_CardWidth[current] * ratio;
          fh = m_CardHeight[last] * (1.0f - ratio) +
               m_CardHeight[current] * ratio;
        } else if (last >= 0) {
          // Only previous card
          fx = m_CardX[last];
          fy = m_CardY[last];
          addsize = ratio;
          fw = m_CardWidth[last];
          fh = m_CardHeight[last];
        } else {
          // Only current card
          fx = m_CardX[current];
          fy = m_CardY[current];
          addsize = 1.0f - ratio;
          fw = m_CardWidth[current];
          fh = m_CardHeight[current];
        }
        fw += m_nFontHeight * (addsize * 10.0f + 1);
        fh += m_nFontHeight * (addsize * 10.0f + 1);

        P->Color = HalfColor(m_nFGColor, m_nBGColor, addsize + br);

        P->Style = psSolid;
        if (addsize < 0.99f) {
          DrawFocusCursor(C, fx, fy, fw, fh);
        }
      }
    }
  }

  // Range selection
  if (m_bMDownBrowser == 5) {
    B->Style = bsClear;
    P->Width = m_fFontZoom * 3;
    P->Color = TColor(m_nFGColor);
    C->RoundRect(m_nXOffset + m_nMDownBrowserX * m_fFontZoom,
                 m_nYOffset + m_nMDownBrowserY * m_fFontZoom,
                 m_nXOffset + m_nMDownTargetX * m_fFontZoom,
                 m_nYOffset + m_nMDownTargetY * m_fFontZoom, m_nFontHeight,
                 m_nFontHeight);
  }

  // Overview display
  if (SettingView.m_bOverview) {
    m_nOVWidth = m_nBrowserWidth / 6;
    m_nOVXOffset = m_nXOffset + m_nBrowserWidth - m_nOVWidth;
    m_nOVHeight = m_nBrowserHeight / 6;
    m_nOVYOffset = m_nYOffset + m_nBrowserHeight - m_nOVHeight;
    P->Color = HalfColor(m_nFGColor, m_nBGColor, 0.80f);
    B->Color = HalfColor(m_nFGColor, m_nBGColor, 0.95f);
    B->Style = bsSolid;
    P->Width = m_fFontZoom;
    P->Style = psSolid;
    C->Rectangle(m_nOVXOffset, m_nOVYOffset, m_nOVXOffset + m_nOVWidth,
                 m_nOVYOffset + m_nOVHeight);

    HRGN MyRgn;
    MyRgn =
        ::CreateRectRgn(m_nOVXOffset, m_nOVYOffset, m_nOVXOffset + m_nOVWidth,
                        m_nOVYOffset + m_nOVHeight);
    ::SelectClipRgn(C->Handle, MyRgn);

    for (int i = 0; i < m_Document->m_Cards->Count; i++)
      if (m_CardVisible[i]) {
        TCard *Card = m_Document->GetCardByIndex(i);
        int x = m_nOVXOffset +
                ((Card->m_fX - 0.5f) * OVERVIEWSIZE + 0.5f) * m_nOVWidth;
        int y = m_nOVYOffset +
                ((Card->m_fY - 0.5f) * OVERVIEWSIZE + 0.5f) * m_nOVHeight;
        P->Color = Card->m_Color;
        B->Color = Card->m_Color;
        C->FillRect(Rect(x - 2 * m_fFontZoom, y - 1 * m_fFontZoom,
                         x + 2 * m_fFontZoom, y + 1 * m_fFontZoom));
      }
    float left, top, right, bottom;
    DispPosToCardPos(0, 0, left, top);
    DispPosToCardPos(m_nBrowserWidth - 1, m_nBrowserHeight - 1, right, bottom);
    P->Color = clRed;
    B->Style = bsClear;
    left = m_nOVXOffset + ((left - 0.5f) * OVERVIEWSIZE + 0.5f) * m_nOVWidth;
    top = m_nOVYOffset + ((top - 0.5f) * OVERVIEWSIZE + 0.5f) * m_nOVHeight;
    right = m_nOVXOffset + ((right - 0.5f) * OVERVIEWSIZE + 0.5f) * m_nOVWidth;
    bottom =
        m_nOVYOffset + ((bottom - 0.5f) * OVERVIEWSIZE + 0.5f) * m_nOVHeight;
    C->Rectangle(left, top, right, bottom);

    ::DeleteObject(MyRgn);
  }

  // Demo string / special text drawing
  F->Height = Canvas->Font->Height * 2;
  B->Style = bsClear;
  F->Style = TFontStyles() << fsBold;
  F->Color = (TColor)SettingView.m_nFourgroundColor;

  // Demo String
  if (m_DemoString != "") {
    C->TextOut(8, m_nBrowserHeight - 8 - C->TextHeight(m_DemoString),
               m_DemoString);
  }

  // FPS
  m_nFPSCount++;
  unsigned int tgt = GetTickCount();
  if (tgt > m_nLastFPSTime + 1000) {
    unsigned int d = tgt - m_nLastFPSTime; // Elapsed ms
    m_fFPS = (m_nFPSCount * 1000.0f) / d;
    m_nFPSCount = 0;
    m_nLastFPSTime = tgt;
  }
}

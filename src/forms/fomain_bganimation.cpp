// ---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <math.h>
#include <math.hpp>
#include "fomain.h"
// #include "fomain_bganimation.h"
#include "utils.h"
#include "setting.h"

// ---------------------------------------------------------------------------
void TFo_Main::BGAnimation(TCanvas *C) {
	switch (SettingView.m_nBGAnimationType) {
	case 0:
		BGAnimation_MovingVHLine(C);
		break;
	case 1:
		BGAnimation_Bubble(C);
		break;
	case 2:
		BGAnimation_Snow(C);
		break;
	case 3:
		BGAnimation_CherryBlossom(C);
		break;
	}
}

// ---------------------------------------------------------------------------
typedef struct MovingVHLineData_ {
	bool bExist; // 割り当てあり
	float fPos; // 座標
	int fWidth; // 線の太さ
	TColor Color; // 線の色
	float fSpeed; // 1フレームで動く幅
	int nType; // 0=上から下、1=下から上、2=左から右、3=右から左
} MovingVHLineData;

// ---------------------------------------------------------------------------
void TFo_Main::BGAnimation_MovingVHLine(TCanvas *C) {
	TPen *P = C->Pen;
	P->Style = psSolid;

	int emptyindex = -1;
	for (int i = 0; i < 32 && i < BGANIMATIONMAXOBJECTS; i++) {
		MovingVHLineData *Data =
			(MovingVHLineData*)&m_BGAnimationBuf[i * BGANIMATIONOBJECTSIZE];
		if (Data->bExist) {
			Data->fPos += Data->fSpeed * m_fBGAnimationSpeed;
			P->Width = Data->fWidth * m_fFontZoom;
			P->Color = Data->Color;
			if (Data->fPos < 0.0f || Data->fPos > 1.0f) {
				Data->bExist = false;
			}
			else {
				switch (Data->nType) {
				case 0:
				case 1:
					C->MoveTo(m_nXOffset,
						m_nYOffset + Data->fPos * m_nBrowserHeight);
					C->LineTo(m_nXOffset + m_nBrowserWidth,
						m_nYOffset + Data->fPos * m_nBrowserHeight);
					break;
				case 2:
				case 3:
					C->MoveTo(m_nXOffset + Data->fPos * m_nBrowserWidth,
						m_nYOffset);
					C->LineTo(m_nXOffset + Data->fPos * m_nBrowserWidth,
						m_nYOffset + m_nBrowserHeight);
					break;
				}
			}
		}
		else if (emptyindex < 0) {
			emptyindex = i;
		}
	}
	if (emptyindex >= 0 && rand() % 2) {
		MovingVHLineData *Data =
			(MovingVHLineData*)&m_BGAnimationBuf
			[emptyindex * BGANIMATIONOBJECTSIZE];
		Data->bExist = true;
		Data->fSpeed = (rand() % 9000 + 1000) * 0.0001f * 0.05f;
		Data->fWidth = rand() % 3000 * 0.001f + 1; // 1～3ドット
		Data->Color =
			HalfColor(HalfColor((rand() % 256) | ((rand() % 256) << 8) |
			((rand() % 256) << 16), m_nFGColor, 0.33f), m_nBGColor, 0.875f);
		Data->nType = rand() % (m_nBrowserWidth + m_nBrowserHeight);
		Data->nType = (Data->nType < m_nBrowserWidth) * 2 + rand() % 2;
		if (Data->nType % 2) {
			Data->fPos = 1.0f;
			Data->fSpeed = -Data->fSpeed;
		}
		else {
			Data->fPos = 0.0f;
		}
	}
}

// ---------------------------------------------------------------------------
typedef struct BubbleData_ {
	bool bExist; // 割り当てあり
	float fX, fY; // 座標
	int fSize; // 泡の大きさ
	TColor Color; // 泡の色（微妙に変化させる？）
	float fSpeed; // 1フレームで動く大きさ
} BubbleData;

// ---------------------------------------------------------------------------
void TFo_Main::BGAnimation_Bubble(TCanvas *C) {
	TPen *P = C->Pen;
	TBrush *B = C->Brush;
	P->Width = m_fFontZoom;
	P->Style = psSolid;
	B->Style = bsClear;

	int emptyindex = -1;
	for (int i = 0; i < 256 && i < BGANIMATIONMAXOBJECTS; i++) {
		BubbleData *Data =
			(BubbleData*)&m_BGAnimationBuf[i * BGANIMATIONOBJECTSIZE];
		if (Data->bExist) {
			Data->fY -= Data->fSpeed * m_fBGAnimationSpeed;
			Data->fX +=
				(rand() % 201 - 100)
				* 0.003 * Data->fSpeed * m_fBGAnimationSpeed;
			// Data->Color = HalfColor(Data->Color, HalfColor(HalfColor((rand() % 256) | ((rand() % 256) << 8) | ((rand() % 256) << 16),m_nFGColor, 0.33f), m_nBGColor, 0.875f), 0.1f);
			P->Color = Data->Color;
			if (Data->fX < -0.1f || Data->fX > 1.1f || Data->fY < -0.1f) {
				Data->bExist = false;
			}
			else {
				C->Ellipse(m_nXOffset + Data->fX * m_nBrowserWidth -
					Data->fSize * m_fFontZoom,
					m_nYOffset + Data->fY * m_nBrowserHeight -
					Data->fSize * m_fFontZoom,
					m_nXOffset + Data->fX * m_nBrowserWidth +
					Data->fSize * m_fFontZoom,
					m_nYOffset + Data->fY * m_nBrowserHeight +
					Data->fSize * m_fFontZoom);
			}
		}
		else if (emptyindex < 0) {
			emptyindex = i;
		}
	}
	if (emptyindex >= 0 && rand()) {
		BubbleData *Data =
			(BubbleData*)&m_BGAnimationBuf[emptyindex * BGANIMATIONOBJECTSIZE];
		Data->bExist = true;
		Data->fSize = (rand() % 4 + 1);
		Data->fSpeed = (rand() % 7000 + 3000) * 0.0001f * 0.1f * Data->fSize;
		Data->Color =
			HalfColor(HalfColor((rand() % 128) | ((rand() % 256) << 8) |
			((rand() % 256) << 16), m_nFGColor, 0.33f), m_nBGColor, 0.5f);
		Data->fX = (rand() % 10001) * 0.0001f;
		Data->fY = 1.0f;
	}
}

// ---------------------------------------------------------------------------
typedef struct SnowData_ {
	bool bExist; // 割り当てあり
	float fX, fY; // 座標
	float aX; // X座標増分
	int fSize; // 大きさ
	TColor Color; // 雪の色
	TColor Color2; // 雪の色（薄い）
	float fSpeed; // 1フレームで動く大きさ
} SnowData;

// ---------------------------------------------------------------------------
void TFo_Main::BGAnimation_Snow(TCanvas *C) {
	TPen *P = C->Pen;
	TBrush *B = C->Brush;
	P->Width = m_fFontZoom;
	P->Style = psSolid;
	B->Style = bsSolid;

	int emptyindex = -1;
	static float aXall = 0.0f;
	aXall += (rand() % 201 - 100) * 0.0007 * 0.1f * m_fBGAnimationSpeed;
	aXall *= 0.9f;
	static float xYall = 0.0f;
	xYall += (rand() % 101) * 0.002f;
	xYall *= 0.9f;
	for (int i = 0; i < 256 && i < BGANIMATIONMAXOBJECTS; i++) {
		SnowData *Data =
			(SnowData*)&m_BGAnimationBuf[i * BGANIMATIONOBJECTSIZE];
		if (Data->bExist) {
			Data->fY += Data->fSpeed * 0.4f * m_fBGAnimationSpeed * xYall;
			Data->aX +=
				(rand() % 201 - 100)
				* 0.003 * Data->fSpeed * 0.13f * m_fBGAnimationSpeed;
			Data->aX *= 0.9f;
			Data->fX += Data->aX + aXall * (Data->fSize + 1) * (rand() % 101)
				* 0.01f;
			B->Color = Data->Color;
			P->Color = Data->Color2;
			if (Data->fX < -0.1f || Data->fX > 1.1f || Data->fY > 1.1f) {
				Data->bExist = false;
			}
			else {
				C->Ellipse(m_nXOffset + Data->fX * m_nBrowserWidth -
					Data->fSize * m_fFontZoom,
					m_nYOffset + Data->fY * m_nBrowserHeight -
					Data->fSize * m_fFontZoom,
					m_nXOffset + Data->fX * m_nBrowserWidth +
					Data->fSize * m_fFontZoom,
					m_nYOffset + Data->fY * m_nBrowserHeight +
					Data->fSize * m_fFontZoom);
			}
		}
		else if (emptyindex < 0) {
			emptyindex = i;
		}
	}
	if (emptyindex >= 0 && rand()) {
		SnowData *Data =
			(SnowData*)&m_BGAnimationBuf[emptyindex * BGANIMATIONOBJECTSIZE];
		Data->bExist = true;
		Data->fSize = (rand() % 3 + 1);
		Data->fSpeed = (rand() % 3000 + 7000) * 0.0001f * 0.1f *
			(Data->fSize + 1);
		Data->Color = HalfColor(HalfColor(m_nBGColor, 0x808080, 0.5f),
			0xffffff, 0.4f);
		Data->Color2 = HalfColor(Data->Color, m_nBGColor, 0.5f);
		Data->fX = (rand() % 10001) * 0.0001f;
		Data->aX = 0.0f;
		Data->fY = 0.0f;
	}
}

// ---------------------------------------------------------------------------
typedef struct CBData_ {
	bool bExist; // 割り当てあり
	float fX, fY; // 座標
	float fT1, fT2, fT3; // 回転角
	float fAT1, fAT2, fAT3; // 角度加速度
	float fAX, fAY; // 座標加速度
	int fSize; // 大きさ
	TColor Color, Color2; // 色
	float fSpeed; // 1フレームで動く大きさ
} CBData;

// ---------------------------------------------------------------------------
void TFo_Main::BGAnimation_CherryBlossom(TCanvas *C) {
	TPen *P = C->Pen;
	TBrush *B = C->Brush;
	P->Width = m_fFontZoom;
	P->Style = psSolid;
	B->Style = bsSolid;

	const float CBShape[] = {0.0f, 1.0f, // 下
		-0.8f, 0.2f, // 左
		-0.3f, -1.0f, // 上左
		0.0f, -0.7f, // 上
		0.3f, -1.0f, // 上左
		0.8f, 0.2f // 左
	};

	int emptyindex = -1;
	static float aXall = 0.0f;
	aXall += (rand() % 201 - 100) * 0.0007 * 0.1f * m_fBGAnimationSpeed;
	aXall *= 0.9f;
	static float xYall = 0.0f;
	xYall += (rand() % 101) * 0.01f;
	xYall *= 0.9f;
	for (int i = 0; i < 256 && i < BGANIMATIONMAXOBJECTS; i++) {
		CBData *Data = (CBData*)&m_BGAnimationBuf[i * BGANIMATIONOBJECTSIZE];
		if (Data->bExist) {
			// 角度加速度加算
			Data->fAT1 = Data->fAT1 * 0.9f + (rand() % 201 - 100)
				* 0.01f * 0.1f;
			Data->fAT2 = Data->fAT2 * 0.9f + (rand() % 201 - 100)
				* 0.01f * 0.1f;
			Data->fAT3 = Data->fAT3 * 0.9f + (rand() % 201 - 100)
				* 0.01f * 0.1f;
			// 角度加速
			Data->fT1 += Data->fAT1 * m_fBGAnimationSpeed * 30;
			Data->fT2 += Data->fAT2 * m_fBGAnimationSpeed * 30;
			Data->fT3 += Data->fAT3 * m_fBGAnimationSpeed * 30;
			// 加速度加算
			Data->fAX = Data->fAX * 0.85f + sin(Data->fT1) * sin(Data->fT2);
			Data->fAY = Data->fAY * 0.85f +
				fabs(sin(Data->fT1) * cos(Data->fT2)) + 0.1f;
			float c = sqrt(fabs(cos(Data->fT3) * cos(Data->fT2))); // 色の濃さ
			// float c = fabs(cos(Data->fT3) * cos(Data->fT2)) * 0.3f + 0.7f;//色の濃さ
			// 加速
			Data->fY +=
				Data->fAY * 0.03f * Data->fSpeed * xYall * m_fBGAnimationSpeed;
			Data->fX += Data->fAX * 0.03f * Data->fSpeed * m_fBGAnimationSpeed +
				aXall * (Data->fSize + 1) * (rand() % 101) * 0.01f;

			if (Data->fX < -0.1f || Data->fX > 1.1f || Data->fY > 1.1f) {
				Data->bExist = false;
			}
			else if (Data->fSize < 8) {
				B->Color = HalfColor(m_nBGColor, Data->Color, c);
				P->Color = B->Color;
				C->Ellipse(m_nXOffset + Data->fX * m_nBrowserWidth -
					Data->fSize * m_fFontZoom,
					m_nYOffset + Data->fY * m_nBrowserHeight -
					Data->fSize * m_fFontZoom,
					m_nXOffset + Data->fX * m_nBrowserWidth +
					Data->fSize * m_fFontZoom,
					m_nYOffset + Data->fY * m_nBrowserHeight +
					Data->fSize * m_fFontZoom);
			}
			else {
				B->Color = Data->Color;
				P->Color = Data->Color;
				TPoint P[6];
				for (int ip = 0; ip < 6; ip++) {
					// float fx = CBShape[ip * 2];
					// float fy = CBShape[ip * 2 + 1];
					float ct2 = cos(Data->fT2);
					ct2 = Sign(ct2) * sqrt(sqrt(fabs(ct2)));
					float st2 = sin(Data->fT2);
					st2 = Sign(st2) * sqrt(sqrt(fabs(st2)));
					float ct3 = cos(Data->fT3);
					ct3 = Sign(ct3) * sqrt(sqrt(fabs(ct3)));
					float st3 = sin(Data->fT3);
					st3 = Sign(st3) * sqrt(sqrt(fabs(st3)));
					float fx =
						(CBShape[ip * 2] * cos(Data->fT1) -
						CBShape[ip * 2 + 1] * sin(Data->fT1)) * ct3 +
						(CBShape[ip * 2] * sin(Data->fT1) -
						CBShape[ip * 2 + 1] * cos(Data->fT1)) * st2 * st3;
					float fy =
						(CBShape[ip * 2] * sin(Data->fT1) -
						CBShape[ip * 2 + 1] * cos(Data->fT1)) * ct2;
					P[ip].x = m_nXOffset + Data->fX * m_nBrowserWidth +
						Data->fSize * fx * m_fFontZoom;
					P[ip].y = m_nYOffset + Data->fY * m_nBrowserHeight +
						Data->fSize * fy * m_fFontZoom;
				}
				C->Polygon(P, 5);
			}
		}
		else if (emptyindex < 0) {
			emptyindex = i;
		}
	}
	if (emptyindex >= 0 && rand()) {
		CBData *Data =
			(CBData*)&m_BGAnimationBuf[emptyindex * BGANIMATIONOBJECTSIZE];
		Data->bExist = true;
		Data->fSize = (rand() % 3 + 1);
		Data->fSpeed = (rand() % 3000 + 7000) * 0.0001f * 0.1f *
			(Data->fSize + 1);
		Data->Color = HalfColor(TColor(0xc890ff), TColor(0xe8d0ff),
			(rand() % 101) * 0.01f);
		Data->fX = (rand() % 10001) * 0.0001f;
		Data->fY = 0.0f;
		Data->fAX = 0.0f;
		Data->fAY = 1.0f;
		Data->fT1 = (rand() % 10001) * 0.0001f * 2 * M_PI;
		Data->fT2 = (rand() % 10001) * 0.0001f * 2 * M_PI;
		Data->fT3 = (rand() % 10001) * 0.0001f * 2 * M_PI;
		Data->fAT1 = 0.0f;
		Data->fAT2 = 0.0f;
		Data->fAT3 = 0.0f;
	}
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)

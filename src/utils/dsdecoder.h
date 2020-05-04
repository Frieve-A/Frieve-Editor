// ---------------------------------------------------------------------------

#ifndef dsdecoderH
#define dsdecoderH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <dshow.h>
#include <qedit.h>

/*
 #include <windows.h>
 #include <iostream.h>
 #include <control.h>
 #include <mediaobj.h>
 #include <strmif.h>
 #include <dmodshow.h>
 #include <objbase.h>
 */
// ---------------------------------------------------------------------------
class ICBBMP {
public:
	virtual void CBBMP(Graphics::TBitmap *BMP) = 0;
};

// ---------------------------------------------------------------------------
class ICBWAVE {
public:
	virtual void CBWAVE(double time, void *buf, int len) = 0;
};

class TDSDecoder : public TThread, ISampleGrabberCB {
public: // ユーザー宣言
	__fastcall TDSDecoder(bool CreateSuspended);
	void __fastcall Execute();

	// 設定
	UnicodeString m_FN;
	ICBBMP *CBBMP;
	ICBWAVE *CBWAVE;
	bool m_bGrub; // データぶっこ抜き
	bool m_bUnsync;
	bool m_bLoop; // 最後まで再生したら自動的に先頭に戻る
	double m_fLoopStartPos, m_fLoopEndPos; // ループ箇所(ms)

	// 状態
	bool m_bExecuted; // スレッドが実行に入っている
	bool m_bPrepared; // 準備済み
	bool m_bWaitToPlay; // 再生待ちする
	bool m_bFinished; // 再生終了フラグ

	// 操作
	double GetLength();
	double GetPosition();
	void SetPosition(double ms);
	void WaitPrepared();
	WAVEFORMATEX *GetWaveFormatEx();
	VIDEOINFOHEADER *GetVideoInfoHeader();

	void GetBMP(Graphics::TBitmap *BMP);

	IGraphBuilder *pGraphBuilder;
	IMediaControl *pMediaControl;
	IMediaFilter *pMediaFilter;
	IMediaSeeking *pMediaSeeking;
	ISampleGrabber *pSampleGrabber;
	VIDEOINFOHEADER *pVideoInfoHeader;
	WAVEFORMATEX *pWaveFormatEx;
	AM_MEDIA_TYPE am_media_type;

	STDMETHODIMP_(ULONG) AddRef() {
		return 2;
	}

	STDMETHODIMP_(ULONG) Release() {
		return 1;
	}

	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv) {

		if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {

			// *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
			*ppv = (void *)this;
			return NOERROR;

		}

		return E_NOINTERFACE;
	}

	HRESULT __stdcall SampleCB(double SampleTime, IMediaSample *pSample);

	HRESULT __stdcall BufferCB(double SampleTime, BYTE *pBuffer,
		long BufferLen);

};

#endif

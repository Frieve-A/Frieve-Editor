// ---------------------------------------------------------------------------

#include "dsdecoder.h"

// ---------------------------------------------------------------------------
IPin *GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir) {
	bool found = false;
	IEnumPins *pEnum;
	IPin *pPin;

	if (pFilter) {
		pFilter->EnumPins(&pEnum);
		while (pEnum->Next(1, &pPin, 0) == S_OK) {
			PIN_DIRECTION pindir;
			pPin->QueryDirection(&pindir);
			if ((found = (PinDir == pindir)) != 0) {
				break;
			}
			pPin->Release();
		}
		pEnum->Release();
		if (found) {
			return pPin;
		}
	}
	return NULL;
}

const GUID IID_ISampleGrabberCB = {
	0x0579154A, 0x2B53, 0x4994, {0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85}
};

const GUID IID_ISampleGrabber = {
	0x6B652FFF, 0x11FE, 0x4FCE, {0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F}
};

const GUID CLSID_SampleGrabber = {
	0xC1F400A0, 0x3F08, 0x11d3, {0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37}
};

const GUID CLSID_NullRenderer = {
	0xC1F400A4, 0x3F08, 0x11D3, {0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37}
};

const GUID MEDIATYPE_Video = {
	0x73646976, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}
};
const GUID MEDIATYPE_Audio = {
	0x73647561, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}
};
const GUID MEDIASUBTYPE_RGB24 = {
	0xE436EB7D, 0x524F, 0x11CE, {0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70}
};
const GUID MEDIASUBTYPE_RGB32 = {
	0xE436EB7E, 0x524F, 0x11CE, {0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70}
};
const GUID MEDIASUBTYPE_PCM = {
	0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}
};
const GUID MEDIASUBTYPE_WAVE = {
	0xE436EB8B, 0x524F, 0x11CE, {0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70}
};
const GUID FORMAT_VideoInfo = {
	0x05589F80, 0xC356, 0x11CE, {0xBF, 0x01, 0x00, 0xAA, 0x00, 0x55, 0x59, 0x5A}
};
const GUID FORMAT_VideoInfo2 = {
	0xF72A76A0, 0xEB0A, 0x11D0, {0xAC, 0xE4, 0x00, 0x00, 0xC0, 0xCC, 0x16, 0xBA}
};
const GUID FORMAT_WaveFormatEx = {
	0x05589F81, 0xC356, 0x11CE, {0xBF, 0x01, 0x00, 0xAA, 0x00, 0x55, 0x59, 0x5A}
};
const GUID TIME_FORMAT_MEDIA_TIME = {
	0x7B785574, 0x8C82, 0x11CF, {0xBC, 0x0C, 0x00, 0xAA, 0x00, 0xAC, 0x74, 0xF6}
};

HRESULT __stdcall TDSDecoder::SampleCB(double SampleTime, IMediaSample *pSample)
{
	return S_OK;
}

HRESULT __stdcall TDSDecoder::BufferCB(double SampleTime, BYTE *pBuffer,
	long nBufferSize) {
	if (CBWAVE) {
		CBWAVE->CBWAVE(SampleTime, pBuffer, nBufferSize);
	}
	else {
		// 現在表示されている映像を静止画として取得
		pSampleGrabber->GetCurrentBuffer(&nBufferSize, (long*)pBuffer);
		void *DIBPtr = NULL;
		HBITMAP BitmapHandle =
			CreateDIBSection(0, (BITMAPINFO*)&pVideoInfoHeader->bmiHeader,
			DIB_RGB_COLORS, &DIBPtr, 0, 0);
		memcpy(DIBPtr, pBuffer, nBufferSize);

		Graphics::TBitmap *BMP = new Graphics::TBitmap();
		BMP->Handle = BitmapHandle;
		CBBMP->CBBMP(BMP);
		delete BMP;
	}

	return S_OK;
}

double TDSDecoder::GetLength() {
	if (m_bPrepared && pMediaSeeking) {
		LONGLONG Duration;
		if (pMediaSeeking->GetDuration(&Duration) == NOERROR) {
			return Duration / 10000;
			/*
			 TGUID TimeFormat;
			 pMediaSeeking->GetTimeFormat(&TimeFormat);

			 if (TimeFormat.Data1 == TIME_FORMAT_MEDIA_TIME.Data1){
			 return Duration / 10000;
			 }
			 */
		}
	}
	return 0.0;
}

double TDSDecoder::GetPosition() {
	LONGLONG Current = 0, StopPos = 0;
	if (m_bPrepared && pMediaSeeking) {
		pMediaSeeking->GetPositions(&Current, &StopPos);
	}
	return Current / 10000;
	/*
	 TGUID TimeFormat;
	 pMediaSeeking->GetTimeFormat(&TimeFormat);

	 if (TimeFormat.Data1 == TIME_FORMAT_MEDIA_TIME.Data1){
	 return Current / 10000;
	 }
	 return 0.0;
	 */
}

void TDSDecoder::SetPosition(double ms) {
	LONGLONG EndPos, CurrentPos = 0;
	EndPos = 0;
	/*
	 TGUID TimeFormat;
	 pMediaSeeking->GetTimeFormat(&TimeFormat);

	 if (TimeFormat.Data1 == TIME_FORMAT_MEDIA_TIME.Data1){
	 CurrentPos = (LONGLONG)(ms * 10000);
	 }
	 */
	CurrentPos = (LONGLONG)(ms * 10000);
	if (m_bPrepared && pMediaSeeking) {
		pMediaSeeking->SetPositions(&CurrentPos, AM_SEEKING_AbsolutePositioning,
			&EndPos, AM_SEEKING_NoPositioning);
	}
}

void TDSDecoder::GetBMP(Graphics::TBitmap *BMP) {
	// キャプチャ
	// バッファを用意
	long nBufferSize = am_media_type.lSampleSize;

	if (nBufferSize > 0 && pSampleGrabber) {
		void *pBuffer = new char[nBufferSize];

		// 現在表示されている映像を静止画として取得
		pSampleGrabber->GetCurrentBuffer(&nBufferSize, (long *)pBuffer);
		void *DIBPtr = NULL;
		HBITMAP BitmapHandle =
			CreateDIBSection(0, (BITMAPINFO*)&pVideoInfoHeader->bmiHeader,
			DIB_RGB_COLORS, &DIBPtr, 0, 0);
		memcpy(DIBPtr, pBuffer, nBufferSize);

		BMP->Handle = BitmapHandle;

		delete[]pBuffer;
	}
}

void TDSDecoder::WaitPrepared() {
	while (!m_bPrepared) {
		Sleep(1);
	}
}

WAVEFORMATEX *TDSDecoder::GetWaveFormatEx() {
	if (!m_bExecuted) {
		return NULL;
	}
	else {
		WaitPrepared();
		return pWaveFormatEx;
	}
}

VIDEOINFOHEADER *TDSDecoder::GetVideoInfoHeader() {
	if (!m_bExecuted) {
		return NULL;
	}
	else {
		WaitPrepared();
		return pVideoInfoHeader;
	}
}

__fastcall TDSDecoder::TDSDecoder(bool CreateSuspended)
	: TThread(CreateSuspended), CBBMP(NULL), CBWAVE(NULL), m_bGrub(false),
	m_bUnsync(false), m_bLoop(false), m_fLoopStartPos(0.0), m_fLoopEndPos(-1.0),

	m_bExecuted(false), m_bPrepared(false), // 再生が準備されたか（フォーマットが取得できたか）
	m_bWaitToPlay(false), // 再生を開始を待つ。Falseにした瞬間から再生開始
	m_bFinished(false), // 再生が終わっているかどうか

	pGraphBuilder(NULL), pMediaControl(NULL), pMediaFilter(NULL),
	pMediaSeeking(NULL), pSampleGrabber(NULL), pVideoInfoHeader(NULL),
	pWaveFormatEx(NULL) {
}

void __fastcall TDSDecoder::Execute() {
	m_bExecuted = true;
	// COMを初期化
	CoInitialize(NULL);

	// FilterGraphを生成
	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder,
		(LPVOID *)&pGraphBuilder);

	// GraphBuilderの各種インターフェース取得
	if (pGraphBuilder) {
		pGraphBuilder->QueryInterface(IID_IMediaControl,
			(LPVOID *)&pMediaControl);
		pGraphBuilder->QueryInterface(IID_IMediaFilter,
			(LPVOID *)&pMediaFilter);
		pGraphBuilder->QueryInterface(IID_IMediaSeeking,
			(LPVOID *)&pMediaSeeking);
	}

	// ソースを生成
	IBaseFilter *pSourceFilter = NULL;
	if (pGraphBuilder) {
		pGraphBuilder->AddSourceFilter(m_FN.c_str(), L"SourceFilter",
			&pSourceFilter);
	}

	// SampleGrabber(Filter)を生成
	IBaseFilter *pSampleGrabberFilter;
	// AM_MEDIA_TYPE am_media_type;
	CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter,
		(LPVOID *)&pSampleGrabberFilter);

	// ISampleGrabberインターフェース取得
	if (pSampleGrabberFilter) {
		pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber,
			(LPVOID *)&pSampleGrabber);
	}

	// SampleGrabberのフォーマット
	ZeroMemory(&am_media_type, sizeof(am_media_type));
	if (CBWAVE) {
		am_media_type.majortype = MEDIATYPE_Audio;
		am_media_type.subtype = MEDIASUBTYPE_PCM;
		am_media_type.formattype = FORMAT_WaveFormatEx;
	}
	else {
		am_media_type.majortype = MEDIATYPE_Video;
		am_media_type.subtype = MEDIASUBTYPE_RGB24;
		am_media_type.formattype = FORMAT_VideoInfo;
	}
	if (pSampleGrabber) {
		pSampleGrabber->SetMediaType(&am_media_type);
	}

	// GraphにSampleGrabber Filterを追加
	if (pGraphBuilder && pSampleGrabberFilter && m_bGrub) {
		pGraphBuilder->AddFilter(pSampleGrabberFilter, L"Sample Grabber");
	}

	// NULL Renderer生成
	IBaseFilter *pFilter_NULLRenderer = NULL;
	CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID *)&pFilter_NULLRenderer);
	if (pGraphBuilder && m_bGrub) {
		pGraphBuilder->AddFilter(pFilter_NULLRenderer, L"Renderer");
	}

	// 接続
	if (pGraphBuilder) {
		IPin *pPinOut; // = NULL;
		IPin *pPinIn; // = NULL;

		// Source->SampleGrabber
		pPinOut = GetPin(pSourceFilter, PINDIR_OUTPUT);
		if (m_bGrub) {
			pPinIn = GetPin(pSampleGrabberFilter, PINDIR_INPUT);
			pGraphBuilder->Connect(pPinOut, pPinIn);

			// SampleGrabber->NULLレンダラ
			pPinOut = GetPin(pSampleGrabberFilter, PINDIR_OUTPUT);
			pPinIn = GetPin(pFilter_NULLRenderer, PINDIR_INPUT);
			pGraphBuilder->Connect(pPinOut, pPinIn);
		}
		else {
			pGraphBuilder->Render(pPinOut);
		}
	}

	// 再生
	if (pSampleGrabber) {
		pSampleGrabber->GetConnectedMediaType(&am_media_type);
		pVideoInfoHeader = (VIDEOINFOHEADER*)am_media_type.pbFormat;
		pWaveFormatEx = (WAVEFORMATEX*)am_media_type.pbFormat;
		pSampleGrabber->SetBufferSamples(m_bGrub);
		if (m_bGrub && (CBBMP || CBWAVE)) {
			pSampleGrabber->SetCallback(this, 1);
		}
	}

	// Unsync
	if (pMediaFilter && m_bUnsync) {
		pMediaFilter->SetSyncSource(NULL);
	}

	m_bPrepared = true;
	// m_bWaitToPlayがクリアされるのを待つ
	while (!Terminated && m_bWaitToPlay) {
		Sleep(1);
	}

	// 再生開始
	if (pMediaControl && !Terminated) {
		if (m_fLoopStartPos > 0.0) {
			SetPosition(m_fLoopStartPos);
		}
		pMediaControl->Run();
	}

	while (!Terminated) {
		// 再生終了待ち
		Sleep(1);

		double len = GetLength();
		double pos = GetPosition();

		if (m_bLoop) {
			if (len > 0.0 && ((len < pos + 1) || (m_fLoopEndPos >
				m_fLoopStartPos && m_fLoopEndPos < pos + 1))) {
				if (m_fLoopStartPos > 0.0) {
					SetPosition(m_fLoopStartPos);
				}
				else {
					SetPosition(0.0);
				}
			}
		}

		m_bFinished = !m_bLoop && ((len < pos + 1) || m_fLoopEndPos >
			m_fLoopStartPos && m_fLoopEndPos < pos + 1);
	}

	// 資源を解放
	if (pMediaControl) {
		pMediaControl->Stop();
	}
	if (pFilter_NULLRenderer) {
		pFilter_NULLRenderer->Release();
	}
	if (pSampleGrabber) {
		pSampleGrabber->Release();
	}
	if (pSampleGrabberFilter) {
		pSampleGrabberFilter->Release();
	}
	if (pSourceFilter) {
		pSourceFilter->Release();
	}
	if (pMediaSeeking) {
		pMediaSeeking->Release();
	}
	if (pMediaFilter) {
		pMediaFilter->Release();
	}
	if (pMediaControl) {
		pMediaControl->Release();
	}
	if (pGraphBuilder) {
		pGraphBuilder->Release();
	}

	// COM終了
	CoUninitialize();

	// 状態初期化
	m_bExecuted = false;
	m_bPrepared = false; // 再生が準備されたか（フォーマットが取得できたか）
	m_bWaitToPlay = false; // 再生を開始を待つ。Falseにした瞬間から再生開始
	m_bFinished = false; // 再生が終わっているかどうか

	pGraphBuilder = NULL;
	pMediaControl = NULL;
	pMediaFilter = NULL;
	pMediaSeeking = NULL;
	pSampleGrabber = NULL;
	pVideoInfoHeader = NULL;
	pWaveFormatEx = NULL;
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)

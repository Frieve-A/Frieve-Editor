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
public: // ���[�U�[�錾
	__fastcall TDSDecoder(bool CreateSuspended);
	void __fastcall Execute();

	// �ݒ�
	UnicodeString m_FN;
	ICBBMP *CBBMP;
	ICBWAVE *CBWAVE;
	bool m_bGrub; // �f�[�^�Ԃ�������
	bool m_bUnsync;
	bool m_bLoop; // �Ō�܂ōĐ������玩���I�ɐ擪�ɖ߂�
	double m_fLoopStartPos, m_fLoopEndPos; // ���[�v�ӏ�(ms)

	// ���
	bool m_bExecuted; // �X���b�h�����s�ɓ����Ă���
	bool m_bPrepared; // �����ς�
	bool m_bWaitToPlay; // �Đ��҂�����
	bool m_bFinished; // �Đ��I���t���O

	// ����
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

// ---------------------------------------------------------------------------

#ifndef dsdecoderH
#define dsdecoderH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
#include <dshow.h>
#include <qedit.h>

/*
 #include <control.h>
 #include <dmodshow.h>
 #include <iostream.h>
 #include <mediaobj.h>
 #include <objbase.h>
 #include <strmif.h>
 #include <windows.h>
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
public:
  __fastcall TDSDecoder(bool CreateSuspended);
  void __fastcall Execute();

  // Settings
  UnicodeString m_FN;
  ICBBMP *CBBMP;
  ICBWAVE *CBWAVE;
  bool m_bGrub; // Extract raw data
  bool m_bUnsync;
  bool m_bLoop; // Loop back to start when playback reaches end
  double m_fLoopStartPos, m_fLoopEndPos; // Loop range (ms)

  // State
  bool m_bExecuted;   // Thread has started executing
  bool m_bPrepared;   // Prepared
  bool m_bWaitToPlay; // Waiting to play
  bool m_bFinished;   // Playback finished flag

  // Operations
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

  STDMETHODIMP_(ULONG) AddRef() { return 2; }

  STDMETHODIMP_(ULONG) Release() { return 1; }

  STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {

    if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {

      // *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
      *ppv = (void *)this;
      return NOERROR;
    }

    return E_NOINTERFACE;
  }

  HRESULT __stdcall SampleCB(double SampleTime, IMediaSample *pSample);

  HRESULT __stdcall BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen);
};

#endif

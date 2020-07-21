#pragma once

#include <windows.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mferror.h>

#include <mutex>
#include <collection.h>
#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.media.h>
#include <windows.media.capture.h>
#include <windows.media.mediaproperties.h>
#include <wrl\implements.h>
#include <wrl\ftm.h>
#include <ppltasks.h>

#include "LinkList.h"

using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Media;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;


namespace libmswinrtvid {
	ref class MSWinRTCapHelper;
	class MSWinRTMediaSink;


	class CritSec
	{
	public:
		CRITICAL_SECTION m_criticalSection;

		CritSec()
		{
			InitializeCriticalSectionEx(&m_criticalSection, 100, 0);
		}

		~CritSec()
		{
			DeleteCriticalSection(&m_criticalSection);
		}

		_Acquires_lock_(m_criticalSection) void Lock()
		{
			EnterCriticalSection(&m_criticalSection);
		}

		_Releases_lock_(m_criticalSection) void Unlock()
		{
			LeaveCriticalSection(&m_criticalSection);
		}
	};


	class AutoLock
	{
	public:
		_Acquires_lock_(m_pCriticalSection) AutoLock(CritSec& crit)
		{
			m_pCriticalSection = &crit;
			m_pCriticalSection->Lock();
		}

		_Releases_lock_(m_pCriticalSection) ~AutoLock()
		{
			m_pCriticalSection->Unlock();
		}

	private:
		CritSec *m_pCriticalSection;
	};


	interface DECLSPEC_UUID("3AC82233-933C-43a9-AF3D-ADC94EABF406") DECLSPEC_NOVTABLE IMarker : public IUnknown
	{
		IFACEMETHOD(GetMarkerType) (MFSTREAMSINK_MARKER_TYPE *pType) = 0;
	IFACEMETHOD(GetMarkerValue) (PROPVARIANT *pvar) = 0;
	IFACEMETHOD(GetContext) (PROPVARIANT *pvar) = 0;
	};


	template<class T>
	class AsyncCallback : public IMFAsyncCallback
	{
	public:
		typedef HRESULT(T::*InvokeFn)(IMFAsyncResult *pAsyncResult);

		AsyncCallback(T *pParent, InvokeFn fn) : m_pParent(pParent), m_pInvokeFn(fn)
		{
		}

		// IUnknown
		STDMETHODIMP_(ULONG) AddRef() {
			// Delegate to parent class.
			return m_pParent->AddRef();
		}
		STDMETHODIMP_(ULONG) Release() {
			// Delegate to parent class.
			return m_pParent->Release();
		}
		STDMETHODIMP QueryInterface(REFIID iid, void** ppv)
		{
			if (!ppv)
			{
				return E_POINTER;
			}
			if (iid == __uuidof(IUnknown))
			{
				*ppv = static_cast<IUnknown*>(static_cast<IMFAsyncCallback*>(this));
			}
			else if (iid == __uuidof(IMFAsyncCallback))
			{
				*ppv = static_cast<IMFAsyncCallback*>(this);
			}
			else
			{
				*ppv = NULL;
				return E_NOINTERFACE;
			}
			AddRef();
			return S_OK;
		}


		// IMFAsyncCallback methods
		STDMETHODIMP GetParameters(DWORD*, DWORD*)
		{
			// Implementation of this method is optional.
			return E_NOTIMPL;
		}

		STDMETHODIMP Invoke(IMFAsyncResult* pAsyncResult)
		{
			return (m_pParent->*m_pInvokeFn)(pAsyncResult);
		}

		T *m_pParent;
		InvokeFn m_pInvokeFn;
	};


	class MSWinRTMarker : public IMarker
	{
	public:
		static HRESULT Create(MFSTREAMSINK_MARKER_TYPE eMarkerType, const PROPVARIANT *pvarMarkerValue, const PROPVARIANT *pvarContextValue, IMarker **ppMarker);

		// IUnknown methods.
		IFACEMETHOD(QueryInterface) (REFIID riid, void **ppv);
		IFACEMETHOD_(ULONG, AddRef) ();
		IFACEMETHOD_(ULONG, Release) ();

		IFACEMETHOD(GetMarkerType) (MFSTREAMSINK_MARKER_TYPE *pType);
		IFACEMETHOD(GetMarkerValue) (PROPVARIANT *pvar);
		IFACEMETHOD(GetContext) (PROPVARIANT *pvar);

	protected:
		MFSTREAMSINK_MARKER_TYPE _eMarkerType;
		PROPVARIANT _varMarkerValue;
		PROPVARIANT _varContextValue;

	private:
		long    _cRef;

		MSWinRTMarker(MFSTREAMSINK_MARKER_TYPE eMarkerType);
		virtual ~MSWinRTMarker();
	};


	class MSWinRTStreamSink : public IMFStreamSink, public IMFMediaTypeHandler
	{
	public:
		// State enum: Defines the current state of the stream.
		enum State
		{
			State_TypeNotSet = 0,    // No media type is set
			State_Ready,             // Media type is set, Start has never been called.
			State_Started,
			State_Stopped,
			State_Paused,
			State_Count              // Number of states
		};

		// StreamOperation: Defines various operations that can be performed on the stream.
		enum StreamOperation
		{
			OpSetMediaType = 0,
			OpStart,
			OpRestart,
			OpPause,
			OpStop,
			OpProcessSample,
			OpPlaceMarker,

			Op_Count                // Number of operations
		};

		// MSWinRTAsyncOperation:
		// Used to queue asynchronous operations. When we call MFPutWorkItem, we use this
		// object for the callback state (pState). Then, when the callback is invoked,
		// we can use the object to determine which asynchronous operation to perform.

		class MSWinRTAsyncOperation : public IUnknown
		{
		public:
			MSWinRTAsyncOperation(StreamOperation op);

			StreamOperation m_op;   // The operation to perform.

									// IUnknown methods.
			STDMETHODIMP QueryInterface(REFIID iid, void **ppv);
			STDMETHODIMP_(ULONG) AddRef();
			STDMETHODIMP_(ULONG) Release();

		private:
			long _cRef;
			virtual ~MSWinRTAsyncOperation();
		};

	public:
		// IUnknown
		IFACEMETHOD(QueryInterface) (REFIID riid, void **ppv);
		IFACEMETHOD_(ULONG, AddRef) ();
		IFACEMETHOD_(ULONG, Release) ();

		// IMFMediaEventGenerator
		IFACEMETHOD(BeginGetEvent) (IMFAsyncCallback *pCallback, IUnknown *punkState);
		IFACEMETHOD(EndGetEvent) (IMFAsyncResult *pResult, IMFMediaEvent **ppEvent);
		IFACEMETHOD(GetEvent) (DWORD dwFlags, IMFMediaEvent **ppEvent);
		IFACEMETHOD(QueueEvent) (MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, PROPVARIANT const *pvValue);

		// IMFStreamSink
		IFACEMETHOD(GetMediaSink) (IMFMediaSink **ppMediaSink);
		IFACEMETHOD(GetIdentifier) (DWORD *pdwIdentifier);
		IFACEMETHOD(GetMediaTypeHandler) (IMFMediaTypeHandler **ppHandler);
		IFACEMETHOD(ProcessSample) (IMFSample *pSample);
		IFACEMETHOD(PlaceMarker) (/* [in] */ MFSTREAMSINK_MARKER_TYPE eMarkerType, /* [in] */ PROPVARIANT const *pvarMarkerValue, /* [in] */ PROPVARIANT const *pvarContextValue);
		IFACEMETHOD(Flush)();

		// IMFMediaTypeHandler
		IFACEMETHOD(IsMediaTypeSupported) (IMFMediaType *pMediaType, IMFMediaType **ppMediaType);
		IFACEMETHOD(GetMediaTypeCount) (DWORD *pdwTypeCount);
		IFACEMETHOD(GetMediaTypeByIndex) (DWORD dwIndex, IMFMediaType **ppType);
		IFACEMETHOD(SetCurrentMediaType) (IMFMediaType *pMediaType);
		IFACEMETHOD(GetCurrentMediaType) (IMFMediaType **ppMediaType);
		IFACEMETHOD(GetMajorType) (GUID *pguidMajorType);

		// ValidStateMatrix: Defines a look-up table that says which operations
		// are valid from which states.
		static BOOL ValidStateMatrix[State_Count][Op_Count];


		MSWinRTStreamSink(DWORD dwIdentifier);
		virtual ~MSWinRTStreamSink();

		HRESULT Initialize(MSWinRTMediaSink *pParent);

		HRESULT CheckShutdown() const
		{
			if (_IsShutdown)
				return MF_E_SHUTDOWN;
			return S_OK;
		}

		HRESULT     Start(MFTIME start);
		HRESULT     Restart();
		HRESULT     Stop();
		HRESULT     Shutdown();

	private:
		HRESULT     ValidateOperation(StreamOperation op);
		HRESULT     QueueAsyncOperation(StreamOperation op);
		HRESULT     OnDispatchWorkItem(IMFAsyncResult *pAsyncResult);
		void        DispatchProcessSample(MSWinRTAsyncOperation *pOp);
		bool        DropSamplesFromQueue();
		bool        SendSampleFromQueue();
		bool        ProcessSamplesFromQueue(bool fFlush);
		void        ProcessFormatChange(IMFMediaType *pMediaType);
		HRESULT		PrepareSample(IMFSample *pSample);
		void        HandleError(HRESULT hr);

	private:
		long                        _cRef;                      // reference count
		CritSec                     _critSec;                   // critical section for thread safety

		DWORD                       _dwIdentifier;
		State                       _state;
		bool                        _IsShutdown;                // Flag to indicate if Shutdown() method was called.
		bool                        _fGetStartTimeFromSample;
		bool                        _fWaitingForFirstSample;
		bool                        _fFirstSampleAfterConnect;
		GUID                        _guiCurrentSubtype;
		UINT64						_guiCurrentFrameSize;

		DWORD                       _WorkQueueId;               // ID of the work queue for asynchronous operations.
		MFTIME                      _StartTime;                 // Presentation time when the clock started.

		ComPtr<IMFMediaSink>        _spSink;                    // Parent media sink
		MSWinRTMediaSink                  *_pParent;

		ComPtr<IMFMediaEventQueue>  _spEventQueue;              // Event queue
		ComPtr<IMFByteStream>       _spByteStream;              // Bytestream where we write the data.
		ComPtr<IMFMediaType>        _spCurrentType;
		ComPtr<IMFSample>           _spFirstVideoSample;

		ComPtrList<IUnknown>        _SampleQueue;               // Queue to hold samples and markers.
																// Applies to: ProcessSample, PlaceMarker

		AsyncCallback<MSWinRTStreamSink>  _WorkQueueCB;              // Callback for the work queue.

		ComPtr<IUnknown>            _spFTM;
	};


	class MSWinRTMediaSink
		: public Microsoft::WRL::RuntimeClass<
			Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
			ABI::Windows::Media::IMediaExtension,
			FtmBase,
			IMFMediaSink,
			IMFClockStateSink>
	{
		InspectableClass(L"libmswinrtvid::MSWinRTMediaSink", BaseTrust)

	public:
		MSWinRTMediaSink();
		~MSWinRTMediaSink();

		HRESULT RuntimeClassInitialize(Windows::Media::MediaProperties::IMediaEncodingProperties ^videoEncodingProperties);

		// IMediaExtension
		IFACEMETHOD(SetProperties) (ABI::Windows::Foundation::Collections::IPropertySet *pConfiguration) { return S_OK; }

		// IMFMediaSink methods
		IFACEMETHOD(GetCharacteristics) (DWORD *pdwCharacteristics);
		IFACEMETHOD(AddStreamSink) (/* [in] */ DWORD dwStreamSinkIdentifier, /* [in] */ IMFMediaType *pMediaType, /* [out] */ IMFStreamSink **ppStreamSink);
		IFACEMETHOD(RemoveStreamSink) (DWORD dwStreamSinkIdentifier);
		IFACEMETHOD(GetStreamSinkCount) (_Out_ DWORD *pcStreamSinkCount);
		IFACEMETHOD(GetStreamSinkByIndex) (DWORD dwIndex, _Outptr_ IMFStreamSink **ppStreamSink);
		IFACEMETHOD(GetStreamSinkById) (DWORD dwIdentifier, IMFStreamSink **ppStreamSink);
		IFACEMETHOD(SetPresentationClock) (IMFPresentationClock *pPresentationClock);
		IFACEMETHOD(GetPresentationClock) (IMFPresentationClock **ppPresentationClock);
		IFACEMETHOD(Shutdown) ();

		// IMFClockStateSink methods
		IFACEMETHOD(OnClockStart) (MFTIME hnsSystemTime, LONGLONG llClockStartOffset);
		IFACEMETHOD(OnClockStop) (MFTIME hnsSystemTime);
		IFACEMETHOD(OnClockPause) (MFTIME hnsSystemTime);
		IFACEMETHOD(OnClockRestart) (MFTIME hnsSystemTime);
		IFACEMETHOD(OnClockSetRate) (MFTIME hnsSystemTime, float flRate);

		LONGLONG GetStartTime() const { return _llStartTime; }

		void ReportEndOfStream();
		void SetCaptureFilter(MSWinRTCapHelper^ capture) { _capture = capture; }
		void OnSampleAvailable(BYTE *buf, DWORD bufLen, LONGLONG presentationTime);

	private:
		void HandleError(HRESULT hr);

		void SetVideoStreamProperties(_In_opt_ Windows::Media::MediaProperties::IMediaEncodingProperties ^mediaEncodingProperties);

		HRESULT CheckShutdown() const
		{
			if (_IsShutdown)
				return MF_E_SHUTDOWN;
			return S_OK;
		}

	private:
		ComPtr<IMFStreamSink> _stream;
		MSWinRTCapHelper^ _capture;

		long                            _cRef;                      // reference count
		CritSec							_critSec;                   // critical section for thread safety

		bool                            _IsShutdown;                // Flag to indicate if Shutdown() method was called.
		bool                            _IsConnected;
		LONGLONG                        _llStartTime;

		ComPtr<IMFPresentationClock>    _spClock;                   // Presentation clock.
		long                            _cStreamsEnded;
		String^                         _remoteUrl;

		DWORD                           _waitingConnectionId;
	};
}

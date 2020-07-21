#include "mswinrtmediasink.h"
#include "mswinrtcap.h"
#include <mediastreamer2/mscommon.h>

using namespace libmswinrtvid;
using namespace Windows::Foundation;


//#define MSWINRTMEDIASINK_DEBUG ms_message
#define MSWINRTMEDIASINK_DEBUG(...)


#define RETURN_HR(hr) { \
	if (FAILED(hr)) \
		ms_error("%s:%d -> 0x%x", __FUNCTION__, __LINE__, hr); \
	return hr; \
}


static void AddAttribute(_In_ GUID guidKey, _In_ IPropertyValue ^value, _In_ IMFAttributes *pAttr)
{
	HRESULT hr = S_OK;
	PropertyType type = value->Type;
	switch (type) {
	case PropertyType::UInt8Array:
		{
			Array<BYTE>^ arr;
			value->GetUInt8Array(&arr);
			hr = pAttr->SetBlob(guidKey, arr->Data, arr->Length);
		}
		break;
	case PropertyType::Double:
		hr = pAttr->SetDouble(guidKey, value->GetDouble());
		break;
	case PropertyType::Guid:
		hr = pAttr->SetGUID(guidKey, value->GetGuid());
		break;
	case PropertyType::String:
		hr = pAttr->SetString(guidKey, value->GetString()->Data());
		break;
	case PropertyType::UInt32:
		hr = pAttr->SetUINT32(guidKey, value->GetUInt32());
		break;
	case PropertyType::UInt64:
		hr = pAttr->SetUINT64(guidKey, value->GetUInt64());
		break;
	// ignore unknown values
	}

	if (FAILED(hr))
		throw ref new Exception(hr);
}

static void ConvertPropertiesToMediaType(_In_ IMediaEncodingProperties ^mep, _Outptr_ IMFMediaType **ppMT)
{
	if (mep == nullptr || ppMT == nullptr)
		throw ref new InvalidArgumentException();

	ComPtr<IMFMediaType> spMT;
	*ppMT = nullptr;
	HRESULT hr = MFCreateMediaType(&spMT);
	if (FAILED(hr))
		throw ref new Exception(hr);

	auto it = mep->Properties->First();
	while (it->HasCurrent) {
		auto currentValue = it->Current;
		AddAttribute(currentValue->Key, safe_cast<IPropertyValue^>(currentValue->Value), spMT.Get());
		it->MoveNext();
	}

	GUID guiMajorType = safe_cast<IPropertyValue^>(mep->Properties->Lookup(MF_MT_MAJOR_TYPE))->GetGuid();
	if (guiMajorType != MFMediaType_Video)
		throw ref new Exception(E_UNEXPECTED);

	*ppMT = spMT.Detach();
}




MSWinRTMarker::MSWinRTMarker(MFSTREAMSINK_MARKER_TYPE eMarkerType) : _cRef(1), _eMarkerType(eMarkerType)
{
	ZeroMemory(&_varMarkerValue, sizeof(_varMarkerValue));
	ZeroMemory(&_varContextValue, sizeof(_varContextValue));
}

MSWinRTMarker::~MSWinRTMarker()
{
	PropVariantClear(&_varMarkerValue);
	PropVariantClear(&_varContextValue);
}

HRESULT MSWinRTMarker::Create(MFSTREAMSINK_MARKER_TYPE eMarkerType, const PROPVARIANT *pvarMarkerValue, const PROPVARIANT *pvarContextValue, IMarker **ppMarker)
{
	if (ppMarker == nullptr)
		return E_POINTER;

	HRESULT hr = S_OK;
	ComPtr<MSWinRTMarker> spMarker;

	spMarker.Attach(new (std::nothrow) MSWinRTMarker(eMarkerType));
	if (spMarker == nullptr)
		hr = E_OUTOFMEMORY;
	// Copy the marker data.
	if (SUCCEEDED(hr)) {
		if (pvarMarkerValue)
			hr = PropVariantCopy(&spMarker->_varMarkerValue, pvarMarkerValue);
	}
	if (SUCCEEDED(hr)) {
		if (pvarContextValue)
			hr = PropVariantCopy(&spMarker->_varContextValue, pvarContextValue);
	}
	if (SUCCEEDED(hr))
		*ppMarker = spMarker.Detach();
	return hr;
}

// IUnknown methods.

IFACEMETHODIMP_(ULONG) MSWinRTMarker::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) MSWinRTMarker::Release()
{
	ULONG cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
		delete this;
	return cRef;
}

IFACEMETHODIMP MSWinRTMarker::QueryInterface(REFIID riid, void **ppv)
{
	if (ppv == nullptr)
		return E_POINTER;
	(*ppv) = nullptr;

	HRESULT hr = S_OK;
	if (riid == IID_IUnknown || riid == __uuidof(IMarker)) {
		(*ppv) = static_cast<IMarker*>(this);
		AddRef();
	} else {
		hr = E_NOINTERFACE;
	}
	return hr;
}

// IMarker methods.

IFACEMETHODIMP MSWinRTMarker::GetMarkerType(MFSTREAMSINK_MARKER_TYPE *pType)
{
	if (pType == NULL)
		return E_POINTER;
	*pType = _eMarkerType;
	return S_OK;
}

IFACEMETHODIMP MSWinRTMarker::GetMarkerValue(PROPVARIANT *pvar)
{
	if (pvar == NULL)
		return E_POINTER;
	return PropVariantCopy(pvar, &_varMarkerValue);

}

IFACEMETHODIMP MSWinRTMarker::GetContext(PROPVARIANT *pvar)
{
	if (pvar == NULL)
		return E_POINTER;
	return PropVariantCopy(pvar, &_varContextValue);
}





MSWinRTStreamSink::MSWinRTStreamSink(DWORD dwIdentifier)
	: _cRef(1)
	, _dwIdentifier(dwIdentifier)
	, _state(State_TypeNotSet)
	, _IsShutdown(false)
	, _fGetStartTimeFromSample(false)
	, _fWaitingForFirstSample(false)
	, _fFirstSampleAfterConnect(false)
	, _StartTime(0)
	, _WorkQueueId(0)
	, _pParent(nullptr)
#pragma warning(push)
#pragma warning(disable:4355)
	, _WorkQueueCB(this, &MSWinRTStreamSink::OnDispatchWorkItem)
#pragma warning(pop)
{
	ZeroMemory(&_guiCurrentSubtype, sizeof(_guiCurrentSubtype));
	_guiCurrentFrameSize = 0;
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink constructor");
}

MSWinRTStreamSink::~MSWinRTStreamSink()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink destructor");
}


MSWinRTStreamSink::MSWinRTAsyncOperation::MSWinRTAsyncOperation(StreamOperation op)
	: _cRef(1), m_op(op)
{
}

MSWinRTStreamSink::MSWinRTAsyncOperation::~MSWinRTAsyncOperation()
{
}

ULONG MSWinRTStreamSink::MSWinRTAsyncOperation::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

ULONG MSWinRTStreamSink::MSWinRTAsyncOperation::Release()
{
	ULONG cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0) {
		delete this;
	}
	return cRef;
}

HRESULT MSWinRTStreamSink::MSWinRTAsyncOperation::QueryInterface(REFIID iid, void **ppv)
{
	if (!ppv)
		return E_POINTER;
	if (iid == IID_IUnknown)
		*ppv = static_cast<IUnknown*>(this);
	else {
		*ppv = nullptr;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}



// IUnknown methods

IFACEMETHODIMP MSWinRTStreamSink::QueryInterface(REFIID riid, void **ppv)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::QueryInterface");
	if (ppv == nullptr)
		RETURN_HR(E_POINTER)
	(*ppv) = nullptr;

	HRESULT hr = S_OK;
	if (riid == IID_IUnknown ||	riid == IID_IMFStreamSink || riid == IID_IMFMediaEventGenerator) {
		(*ppv) = static_cast<IMFStreamSink*>(this);
		AddRef();
	} else if (riid == IID_IMFMediaTypeHandler) {
		(*ppv) = static_cast<IMFMediaTypeHandler*>(this);
		AddRef();
	} else {
		hr = E_NOINTERFACE;
	}

	if (FAILED(hr) && riid == IID_IMarshal) {
		if (_spFTM == nullptr) {
			AutoLock lock(_critSec);
			if (_spFTM == nullptr)
				hr = CoCreateFreeThreadedMarshaler(static_cast<IMFStreamSink*>(this), &_spFTM);
		}
		if (SUCCEEDED(hr)) {
			if (_spFTM == nullptr)
				hr = E_UNEXPECTED;
			else
				hr = _spFTM.Get()->QueryInterface(riid, ppv);
		}
	}

	RETURN_HR(hr)
}

IFACEMETHODIMP_(ULONG) MSWinRTStreamSink::AddRef()
{
	long cRef = InterlockedIncrement(&_cRef);
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::AddRef -> %d", cRef);
	return cRef;
}

IFACEMETHODIMP_(ULONG) MSWinRTStreamSink::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::Release -> %d", cRef);
	if (cRef == 0)
		delete this;
	return cRef;
}

// IMFMediaEventGenerator methods.
// Note: These methods call through to the event queue helper object.

IFACEMETHODIMP MSWinRTStreamSink::BeginGetEvent(IMFAsyncCallback *pCallback, IUnknown *punkState)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::BeginGetEvent");
	HRESULT hr = S_OK;
	AutoLock lock(_critSec);
	hr = CheckShutdown();
	if (SUCCEEDED(hr))
		hr = _spEventQueue->BeginGetEvent(pCallback, punkState);
	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTStreamSink::EndGetEvent(IMFAsyncResult *pResult, IMFMediaEvent **ppEvent)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::EndGetEvent");
	HRESULT hr = S_OK;
	AutoLock lock(_critSec);
	hr = CheckShutdown();
	if (SUCCEEDED(hr))
		hr = _spEventQueue->EndGetEvent(pResult, ppEvent);
	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTStreamSink::GetEvent(DWORD dwFlags, IMFMediaEvent **ppEvent)
{
	// NOTE:
	// GetEvent can block indefinitely, so we don't hold the lock.
	// This requires some juggling with the event queue pointer.

	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::GetEvent");
	HRESULT hr = S_OK;
	ComPtr<IMFMediaEventQueue> spQueue;

	{
		AutoLock lock(_critSec);
		hr = CheckShutdown();
		if (SUCCEEDED(hr))
			spQueue = _spEventQueue;
	}

	if (SUCCEEDED(hr))
		hr = spQueue->GetEvent(dwFlags, ppEvent);
	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTStreamSink::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, PROPVARIANT const *pvValue)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::QueueEvent");
	HRESULT hr = S_OK;
	AutoLock lock(_critSec);
	hr = CheckShutdown();
	if (SUCCEEDED(hr))
		hr = _spEventQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
	RETURN_HR(hr)
}

/// IMFStreamSink methods

IFACEMETHODIMP MSWinRTStreamSink::GetMediaSink(IMFMediaSink **ppMediaSink)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::GetMediaSink");
	if (ppMediaSink == nullptr)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr))
		_spSink.Get()->QueryInterface(IID_IMFMediaSink, (void**)ppMediaSink);
	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTStreamSink::GetIdentifier(DWORD *pdwIdentifier)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::GetIdentifier");
	if (pdwIdentifier == nullptr)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr)) {
		MSWINRTMEDIASINK_DEBUG("\t-> %d", _dwIdentifier);
		*pdwIdentifier = _dwIdentifier;
	}
	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTStreamSink::GetMediaTypeHandler(IMFMediaTypeHandler **ppHandler)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::GetMediaTypeHandler");
	if (ppHandler == nullptr)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	// This stream object acts as its own type handler, so we QI ourselves.
	if (SUCCEEDED(hr))
		hr = QueryInterface(IID_IMFMediaTypeHandler, (void**)ppHandler);
	RETURN_HR(hr)
}

// We received a sample from an upstream component
IFACEMETHODIMP MSWinRTStreamSink::ProcessSample(IMFSample *pSample)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::ProcessSample");
	if (pSample == nullptr)
		RETURN_HR(E_INVALIDARG)

	HRESULT hr = S_OK;
	AutoLock lock(_critSec);
	hr = CheckShutdown();
	// Validate the operation.
	if (SUCCEEDED(hr))
		hr = ValidateOperation(OpProcessSample);

	if (SUCCEEDED(hr) && _fWaitingForFirstSample) {
		_spFirstVideoSample = pSample;
		_fWaitingForFirstSample = false;
		hr = QueueEvent(MEStreamSinkRequestSample, GUID_NULL, hr, nullptr);
	} else if (SUCCEEDED(hr)) {
		// Add the sample to the sample queue.
		if (SUCCEEDED(hr))
			hr = _SampleQueue.InsertBack(pSample);

		// Unless we are paused, start an async operation to dispatch the next sample.
		if (SUCCEEDED(hr)) {
			if (_state != State_Paused) {
				// Queue the operation.
				hr = QueueAsyncOperation(OpProcessSample);
			}
		}
	}

	RETURN_HR(hr)
}

// The client can call PlaceMarker at any time. In response,
// we need to queue an MEStreamSinkMarker event, but not until
// *after *we have processed all samples that we have received
// up to this point.
//
// Also, in general you might need to handle specific marker
// types, although this sink does not.

IFACEMETHODIMP MSWinRTStreamSink::PlaceMarker(MFSTREAMSINK_MARKER_TYPE eMarkerType, const PROPVARIANT *pvarMarkerValue, const PROPVARIANT *pvarContextValue)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::PlaceMarker");
	HRESULT hr = S_OK;
	ComPtr<IMarker> spMarker;

	AutoLock lock(_critSec);
	hr = CheckShutdown();
	if (SUCCEEDED(hr))
		hr = ValidateOperation(OpPlaceMarker);
	if (SUCCEEDED(hr))
		hr = MSWinRTMarker::Create(eMarkerType, pvarMarkerValue, pvarContextValue, &spMarker);
	if (SUCCEEDED(hr))
		hr = _SampleQueue.InsertBack(spMarker.Get());

	// Unless we are paused, start an async operation to dispatch the next sample/marker.
	if (SUCCEEDED(hr)) {
		if (_state != State_Paused) {
			// Queue the operation.
			hr = QueueAsyncOperation(OpPlaceMarker); // Increments ref count on pOp.
		}
	}
	RETURN_HR(hr)
}

// Discards all samples that were not processed yet.
IFACEMETHODIMP MSWinRTStreamSink::Flush()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::Flush");
	HRESULT hr = S_OK;
	AutoLock lock(_critSec);
	try {
		hr = CheckShutdown();
		if (FAILED(hr)) {
			throw ref new Exception(hr);
		}

		// Note: Even though we are flushing data, we still need to send
		// any marker events that were queued.
		DropSamplesFromQueue();
	} catch (Exception ^exc) {
		hr = exc->HResult;
	}

	RETURN_HR(hr)
}


/// IMFMediaTypeHandler methods

// Check if a media type is supported.
IFACEMETHODIMP MSWinRTStreamSink::IsMediaTypeSupported(/* [in] */ IMFMediaType *pMediaType, /* [out] */ IMFMediaType **ppMediaType)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::IsMediaTypeSupported");
	if (pMediaType == nullptr)
		RETURN_HR(E_INVALIDARG)

	GUID majorType = GUID_NULL;
	UINT cbSize = 0;
	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr))
		hr = pMediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);

	// First make sure it's video type.
	if (SUCCEEDED(hr)) {
		if (majorType != MFMediaType_Video)
			hr = MF_E_INVALIDMEDIATYPE;
	}

	if (SUCCEEDED(hr) && _spCurrentType != nullptr) {
		GUID guiNewSubtype;
		if (FAILED(pMediaType->GetGUID(MF_MT_SUBTYPE, &guiNewSubtype)) || guiNewSubtype != _guiCurrentSubtype)
			hr = MF_E_INVALIDMEDIATYPE;
	}
	if (SUCCEEDED(hr)) {
		UINT64 frameSize = 0;
		if (FAILED(pMediaType->GetUINT64(MF_MT_FRAME_SIZE, &frameSize)) || frameSize != _guiCurrentFrameSize)
			hr = MF_E_INVALIDMEDIATYPE;
		else
			MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::IsMediaTypeSupported: %dx%d", (frameSize & 0xffffffff00000000) >> 32, frameSize & 0xffffffff);
	}

	// We don't return any "close match" types.
	if (ppMediaType)
		*ppMediaType = nullptr;

	RETURN_HR(hr)
}


// Return the number of preferred media types.
IFACEMETHODIMP MSWinRTStreamSink::GetMediaTypeCount(DWORD *pdwTypeCount)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::GetMediaTypeCount");
	if (pdwTypeCount == nullptr)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr)) {
		// We've got only one media type
		*pdwTypeCount = 1;
	}

	RETURN_HR(hr)
}


// Return a preferred media type by index.
IFACEMETHODIMP MSWinRTStreamSink::GetMediaTypeByIndex(/* [in] */ DWORD dwIndex, /* [out] */ IMFMediaType **ppType)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::GetMediaTypeByIndex");
	if (ppType == nullptr)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (dwIndex > 0)
		hr = MF_E_NO_MORE_TYPES;
	else {
		*ppType = _spCurrentType.Get();
		if (*ppType != nullptr)
			(*ppType)->AddRef();
	}

	RETURN_HR(hr)
}


// Set the current media type.
IFACEMETHODIMP MSWinRTStreamSink::SetCurrentMediaType(IMFMediaType *pMediaType)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::SetCurrentMediaType");
	HRESULT hr = S_OK;
	try {
		if (pMediaType == nullptr)
			throw ref new Exception(E_INVALIDARG);

		AutoLock lock(_critSec);
		hr = CheckShutdown();
		if (FAILED(hr)) {
			throw ref new Exception(hr);
		}

		// We don't allow format changes after streaming starts.
		hr = ValidateOperation(OpSetMediaType);
		if (FAILED(hr)) {
			throw ref new Exception(hr);
		}

		// We set media type already
		if (_state >= State_Ready) {
			hr = IsMediaTypeSupported(pMediaType, nullptr);
			if (FAILED(hr)) {
				throw ref new Exception(hr);
			}
		}

		hr = MFCreateMediaType(_spCurrentType.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			throw ref new Exception(hr);
		}
		hr = pMediaType->CopyAllItems(_spCurrentType.Get());
		if (FAILED(hr)) {
			throw ref new Exception(hr);
		}
		hr = _spCurrentType->GetGUID(MF_MT_SUBTYPE, &_guiCurrentSubtype);
		if (FAILED(hr)) {
			throw ref new Exception(hr);
		}
		hr = _spCurrentType->GetUINT64(MF_MT_FRAME_SIZE, &_guiCurrentFrameSize);
		if (FAILED(hr)) {
			throw ref new Exception(hr);
		}
		if (_state < State_Ready)
			_state = State_Ready;
		else if (_state > State_Ready) {
			ComPtr<IMFMediaType> spType;
			hr = MFCreateMediaType(&spType);
			if (FAILED(hr)) {
				throw ref new Exception(hr);
			}
			hr = pMediaType->CopyAllItems(spType.Get());
			if (FAILED(hr)) {
				throw ref new Exception(hr);
			}
			ProcessFormatChange(spType.Get());
		}
	} catch (Exception ^exc) {
		hr = exc->HResult;
	}

	RETURN_HR(hr)
}

// Return the current media type, if any.
IFACEMETHODIMP MSWinRTStreamSink::GetCurrentMediaType(IMFMediaType **ppMediaType)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::GetCurrentMediaType");
	if (ppMediaType == nullptr)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr)) {
		if (_spCurrentType == nullptr)
			hr = MF_E_NOT_INITIALIZED;
	}
	if (SUCCEEDED(hr)) {
		*ppMediaType = _spCurrentType.Get();
		(*ppMediaType)->AddRef();
	}

	RETURN_HR(hr)
}


// Return the major type GUID.
IFACEMETHODIMP MSWinRTStreamSink::GetMajorType(GUID *pguidMajorType)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::GetMajorType");
	if (pguidMajorType == nullptr)
		RETURN_HR(E_INVALIDARG)

	if (!_spCurrentType)
		return MF_E_NOT_INITIALIZED;

	*pguidMajorType = MFMediaType_Video;
	return S_OK;
}


// private methods
HRESULT MSWinRTStreamSink::Initialize(MSWinRTMediaSink *pParent)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::Initialize");
	// Create the event queue helper.
	HRESULT hr = MFCreateEventQueue(&_spEventQueue);

	// Allocate a new work queue for async operations.
	if (SUCCEEDED(hr))
		hr = MFAllocateSerialWorkQueue(MFASYNC_CALLBACK_QUEUE_STANDARD, &_WorkQueueId);

	if (SUCCEEDED(hr)) {
		_spSink = pParent;
		_pParent = pParent;
	}

	RETURN_HR(hr)
}


// Called when the presentation clock starts.
HRESULT MSWinRTStreamSink::Start(MFTIME start)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::Start");
	AutoLock lock(_critSec);
	HRESULT hr = ValidateOperation(OpStart);
	if (SUCCEEDED(hr)) {
		if (start != PRESENTATION_CURRENT_POSITION) {
			_StartTime = start;        // Cache the start time.
			_fGetStartTimeFromSample = false;
		} else {
			_fGetStartTimeFromSample = true;
		}
		_state = State_Started;
		_fWaitingForFirstSample = true;
		hr = QueueAsyncOperation(OpStart);
	}
	RETURN_HR(hr)
}

// Called when the presentation clock stops.
HRESULT MSWinRTStreamSink::Stop()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::Stop");
	AutoLock lock(_critSec);
	HRESULT hr = ValidateOperation(OpStop);
	if (SUCCEEDED(hr)) {
		_state = State_Stopped;
		hr = QueueAsyncOperation(OpStop);
	}
	RETURN_HR(hr)
}

// Called when the presentation clock restarts.
HRESULT MSWinRTStreamSink::Restart()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::Restart");
	AutoLock lock(_critSec);
	HRESULT hr = ValidateOperation(OpRestart);
	if (SUCCEEDED(hr)) {
		_state = State_Started;
		hr = QueueAsyncOperation(OpRestart);
	}
	RETURN_HR(hr)
}

// Class-static matrix of operations vs states.
// If an entry is TRUE, the operation is valid from that state.
BOOL MSWinRTStreamSink::ValidStateMatrix[MSWinRTStreamSink::State_Count][MSWinRTStreamSink::Op_Count] =
{
	// States:    Operations:
	//            SetType   Start     Restart   Pause     Stop      Sample    Marker   
	/* NotSet */  TRUE,     FALSE,    FALSE,    FALSE,    FALSE,    FALSE,    FALSE,

	/* Ready */   TRUE,     TRUE,     FALSE,    TRUE,     TRUE,     FALSE,    TRUE,

	/* Start */   TRUE,     TRUE,     FALSE,    TRUE,     TRUE,     TRUE,     TRUE,

	/* Pause */   TRUE,     TRUE,     TRUE,     TRUE,     TRUE,     TRUE,     TRUE,

	/* Stop */    TRUE,     TRUE,     FALSE,    FALSE,    TRUE,     FALSE,    TRUE,

};

// Checks if an operation is valid in the current state.
HRESULT MSWinRTStreamSink::ValidateOperation(StreamOperation op)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::ValidateOperation");
	if (ValidStateMatrix[_state][op])
		return S_OK;
	else if (_state == State_TypeNotSet)
		return MF_E_NOT_INITIALIZED;
	else
		return MF_E_INVALIDREQUEST;
}

// Shuts down the stream sink.
HRESULT MSWinRTStreamSink::Shutdown()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::Shutdown");
	AutoLock lock(_critSec);
	if (!_IsShutdown) {
		if (_spEventQueue)
			_spEventQueue->Shutdown();

		MFUnlockWorkQueue(_WorkQueueId);
		_SampleQueue.Clear();
		_spSink.Reset();
		_spEventQueue.Reset();
		_spByteStream.Reset();
		_spCurrentType.Reset();
		_IsShutdown = true;
	}
	return S_OK;
}

// Puts an async operation on the work queue.
HRESULT MSWinRTStreamSink::QueueAsyncOperation(StreamOperation op)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::QueueAsyncOperation");
	HRESULT hr = S_OK;
	ComPtr<MSWinRTAsyncOperation> spOp;
	spOp.Attach(new MSWinRTAsyncOperation(op)); // Created with ref count = 1
	if (!spOp)
		hr = E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
		hr = MFPutWorkItem2(_WorkQueueId, 0, &_WorkQueueCB, spOp.Get());
	RETURN_HR(hr)
}

HRESULT MSWinRTStreamSink::OnDispatchWorkItem(IMFAsyncResult *pAsyncResult)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::OnDispatchWorkItem");
	// Called by work queue thread. Need to hold the critical section.
	AutoLock lock(_critSec);

	try {
		ComPtr<IUnknown> spState;
		HRESULT hr = pAsyncResult->GetState(&spState);
		if (FAILED(hr)) {
			throw ref new Exception(hr);
		}

		// The state object is a CAsncOperation object.
		MSWinRTAsyncOperation *pOp = static_cast<MSWinRTAsyncOperation *>(spState.Get());
		StreamOperation op = pOp->m_op;
		bool fRequestMoreSamples = false;

		switch (op) {
		case OpStart:
		case OpRestart:
			// Send MEStreamSinkStarted.
			hr = QueueEvent(MEStreamSinkStarted, GUID_NULL, S_OK, nullptr);
			if (FAILED(hr)) {
				throw ref new Exception(hr);
			}

			// There might be samples queue from earlier (ie, while paused).
			fRequestMoreSamples = SendSampleFromQueue();
			if (fRequestMoreSamples) {
				// If false there is no samples in the queue now so request one
				hr = QueueEvent(MEStreamSinkRequestSample, GUID_NULL, S_OK, nullptr);
				if (FAILED(hr)) {
					throw ref new Exception(hr);
				}
			}
			break;

		case OpStop:
			// Drop samples from queue.
			DropSamplesFromQueue();

			// Send the event even if the previous call failed.
			hr = QueueEvent(MEStreamSinkStopped, GUID_NULL, S_OK, nullptr);
			if (FAILED(hr)) {
				throw ref new Exception(hr);
			}
			break;

		case OpPause:
			hr = QueueEvent(MEStreamSinkPaused, GUID_NULL, S_OK, nullptr);
			if (FAILED(hr)) {
				throw ref new Exception(hr);
			}
			break;

		case OpProcessSample:
		case OpPlaceMarker:
		case OpSetMediaType:
			DispatchProcessSample(pOp);
			break;
		}
	} catch (Exception ^exc) {
		HandleError(exc->HResult);
	}

	return S_OK;
}

// Complete a ProcessSample or PlaceMarker request.
void MSWinRTStreamSink::DispatchProcessSample(MSWinRTAsyncOperation *pOp)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::DispatchProcessSample");
	bool fRequestMoreSamples = SendSampleFromQueue();

	// Ask for another sample
	if (fRequestMoreSamples) {
		if (pOp->m_op == OpProcessSample) {
			HRESULT hr = QueueEvent(MEStreamSinkRequestSample, GUID_NULL, S_OK, nullptr);
			if (FAILED(hr))
				throw ref new Exception(hr);
		}
	}
}

// Drop samples in the queue
bool MSWinRTStreamSink::DropSamplesFromQueue()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::DropSamplesFromQueue");
	ProcessSamplesFromQueue(true);
	return true;
}

// Send sample from the queue
bool MSWinRTStreamSink::SendSampleFromQueue()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::SendSampleFromQueue");
	return ProcessSamplesFromQueue(false);
}

bool MSWinRTStreamSink::ProcessSamplesFromQueue(bool fFlush)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::ProcessSamplesFromQueue");
	bool fNeedMoreSamples = false;
	ComPtr<IUnknown> spunkSample;
	bool fSendSamples = true;
	bool fSendEOS = false;

	if (FAILED(_SampleQueue.RemoveFront(&spunkSample))) {
		fNeedMoreSamples = true;
		fSendSamples = false;
	}

	while (fSendSamples) {
		ComPtr<IMFSample> spSample;

		// Figure out if this is a marker or a sample.
		// If this is a sample, write it to the file.
		// Now handle the sample/marker appropriately.
		if (SUCCEEDED(spunkSample.As(&spSample))) {
			if (!fFlush) {
				// Prepare sample for sending
				if (FAILED(PrepareSample(spSample.Get()))) {
					fSendSamples = false;
				}
			}
		} else {
			ComPtr<IMarker> spMarker;
			// Check if it is a marker
			if (SUCCEEDED(spunkSample.As(&spMarker))) {
				MFSTREAMSINK_MARKER_TYPE markerType;
				PROPVARIANT var;
				HRESULT hr;
				PropVariantInit(&var);
				hr = spMarker->GetMarkerType(&markerType);
				if (FAILED(hr)) throw ref new Exception(hr);
				// Get the context data.
				hr = spMarker->GetContext(&var);
				if (FAILED(hr)) throw ref new Exception(hr);
				hr = QueueEvent(MEStreamSinkMarker, GUID_NULL, S_OK, &var);
				PropVariantClear(&var);
				if (FAILED(hr)) throw ref new Exception(hr);

				if (markerType == MFSTREAMSINK_MARKER_ENDOFSEGMENT) {
					fSendEOS = true;
				}
			}
#if 0 // TODO
			else {
				ComPtr<IMFMediaType> spType;
				HRESULT hr = spunkSample.As(&spType);
				if (FAILED(hr)) throw ref new Exception(hr);
				if (!fFlush) {
					spPacket = PrepareFormatChange(spType.Get());
				}
			}
#endif
		}

		if (fSendSamples) {
			if (FAILED(_SampleQueue.RemoveFront(spunkSample.ReleaseAndGetAddressOf()))) {
				fNeedMoreSamples = true;
				fSendSamples = false;
			}
		}
	}

	if (fSendEOS)
	{
		ComPtr<MSWinRTMediaSink> spParent = _pParent;
		concurrency::create_task([spParent]() {
			spParent->ReportEndOfStream();
		});
	}
	return fNeedMoreSamples;
}

// Processing format change
void MSWinRTStreamSink::ProcessFormatChange(IMFMediaType *pMediaType)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::ProcessFormatChange");
	// Add the media type to the sample queue.
	HRESULT hr = _SampleQueue.InsertBack(pMediaType);
	if (FAILED(hr))
		throw ref new Exception(hr);

	// Unless we are paused, start an async operation to dispatch the next sample.
	// Queue the operation.
	hr = QueueAsyncOperation(OpSetMediaType);
	if (FAILED(hr))
		throw ref new Exception(hr);
}

HRESULT MSWinRTStreamSink::PrepareSample(IMFSample *pSample)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::PrepareSample");
	LONGLONG llSampleTime;
	HRESULT hr = pSample->GetSampleTime(&llSampleTime);
	if (FAILED(hr))
		return hr;
	if (llSampleTime < 0)
		return hr;

	DWORD cBuffers = 0;
	hr = pSample->GetBufferCount(&cBuffers);
	if (SUCCEEDED(hr)) {
		for (DWORD nIndex = 0; nIndex < cBuffers; ++nIndex) {
			ComPtr<IMFMediaBuffer> spMediaBuffer;
			// Get buffer from the sample
			hr = pSample->GetBufferByIndex(nIndex, &spMediaBuffer);
			if (FAILED(hr)) break;
			BYTE *pBuffer = nullptr;
			DWORD currentLength = 0;
			hr = spMediaBuffer->Lock(&pBuffer, NULL, &currentLength);
			if (SUCCEEDED(hr)) {
				static_cast<MSWinRTMediaSink *>(_spSink.Get())->OnSampleAvailable(pBuffer, currentLength, llSampleTime);
				hr = spMediaBuffer->Unlock();
				if (FAILED(hr)) break;
			}
		}
	}

	RETURN_HR(hr);
}

void MSWinRTStreamSink::HandleError(HRESULT hr)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTStreamSink::HandleError");
	if (!_IsShutdown)
		QueueEvent(MEError, GUID_NULL, hr, nullptr);
}






MSWinRTMediaSink::MSWinRTMediaSink()
	: _cRef(1)
	, _IsShutdown(false)
	, _IsConnected(false)
	, _llStartTime(0)
	, _cStreamsEnded(0)
	, _waitingConnectionId(0)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink constructor");
}

MSWinRTMediaSink::~MSWinRTMediaSink()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink destructor");
}

HRESULT MSWinRTMediaSink::RuntimeClassInitialize(Windows::Media::MediaProperties::IMediaEncodingProperties ^videoEncodingProperties)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::RuntimeClassInitialize");
	try
	{
		SetVideoStreamProperties(videoEncodingProperties);
	}
	catch (Exception ^exc)
	{
		return exc->HResult;
	}

	return S_OK;
}

void MSWinRTMediaSink::SetVideoStreamProperties(_In_opt_ Windows::Media::MediaProperties::IMediaEncodingProperties ^mediaEncodingProperties)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::SetVideoStreamProperties");
	if (mediaEncodingProperties != nullptr)
	{
		ComPtr<IMFMediaType> spMediaType;
		ConvertPropertiesToMediaType(mediaEncodingProperties, &spMediaType);

		// Initialize the stream.
		HRESULT hr = S_OK;
		MSWinRTStreamSink *pStream = new MSWinRTStreamSink(0);
		if (pStream == nullptr)
			hr = E_OUTOFMEMORY;
		if (SUCCEEDED(hr)) {
			pStream->Initialize(this);
			hr = pStream->SetCurrentMediaType(spMediaType.Get());
		}
		if (SUCCEEDED(hr)) {
			_stream = pStream; // implicit AddRef
		}

		if (FAILED(hr))
			throw ref new Exception(hr);
	}
}

///  IMFMediaSink
IFACEMETHODIMP MSWinRTMediaSink::GetCharacteristics(DWORD *pdwCharacteristics)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::GetCharacteristics");
	if (pdwCharacteristics == NULL)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr)) {
		// Rateless sink with fixed number of streams (1).
		*pdwCharacteristics = MEDIASINK_RATELESS | MEDIASINK_FIXED_STREAMS;
	}
	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTMediaSink::AddStreamSink(/* [in] */ DWORD dwStreamSinkIdentifier, /* [in] */ IMFMediaType *pMediaType, /* [out] */ IMFStreamSink **ppStreamSink)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::AddStreamSink");
	RETURN_HR(E_NOTIMPL);
}

IFACEMETHODIMP MSWinRTMediaSink::RemoveStreamSink(DWORD dwStreamSinkIdentifier)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::RemoveStreamSink");
	RETURN_HR(E_NOTIMPL);
}

IFACEMETHODIMP MSWinRTMediaSink::GetStreamSinkCount(_Out_ DWORD *pcStreamSinkCount)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::GetStreamSinkCount");
	if (pcStreamSinkCount == NULL)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr))
		*pcStreamSinkCount = 1;

	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTMediaSink::GetStreamSinkByIndex(DWORD dwIndex, _Outptr_ IMFStreamSink **ppStreamSink)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::GetStreamSinkByIndex(dwIndex=%d)", dwIndex);
	if (ppStreamSink == NULL)
		RETURN_HR(E_INVALIDARG)

	ComPtr<IMFStreamSink> spStream;
	AutoLock lock(_critSec);
	if (dwIndex >= 1)
		return MF_E_INVALIDINDEX;

	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr)) {
		spStream = _stream;
		*ppStreamSink = spStream.Detach();
	}

	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTMediaSink::GetStreamSinkById(DWORD dwStreamSinkIdentifier, IMFStreamSink **ppStreamSink)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::GetStreamSinkById(dwStreamSinkIdentifier=%d)", dwStreamSinkIdentifier);
	if (ppStreamSink == NULL)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	ComPtr<IMFStreamSink> spResult;
	if (SUCCEEDED(hr)) {
		DWORD dwId;
		hr = _stream->GetIdentifier(&dwId);
		if (SUCCEEDED(hr)) {
			if (dwId == dwStreamSinkIdentifier)
				spResult = _stream; // implicit AddRef
			else
				hr = MF_E_INVALIDSTREAMNUMBER;
		} else {
			hr = MF_E_INVALIDSTREAMNUMBER;
		}
		if (SUCCEEDED(hr)) {
			*ppStreamSink = spResult.Detach();
		}
	}

	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTMediaSink::SetPresentationClock(IMFPresentationClock *pPresentationClock)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::SetPresentationClock");
	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();

	// If we already have a clock, remove ourselves from that clock's
	// state notifications.
	if (SUCCEEDED(hr)) {
		if (_spClock)
			hr = _spClock->RemoveClockStateSink(this);
	}

	// Register ourselves to get state notifications from the new clock.
	if (SUCCEEDED(hr)) {
		if (pPresentationClock)
			hr = pPresentationClock->AddClockStateSink(this);
	}

	if (SUCCEEDED(hr)) {
		// Release the pointer to the old clock.
		// Store the pointer to the new clock.
		_spClock = pPresentationClock;
	}

	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTMediaSink::GetPresentationClock(IMFPresentationClock **ppPresentationClock)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::GetPresentationClock");
	if (ppPresentationClock == NULL)
		RETURN_HR(E_INVALIDARG)

	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr)) {
		if (_spClock == NULL)
			hr = MF_E_NO_CLOCK; // There is no presentation clock.
		else {
			// Return the pointer to the caller.
			*ppPresentationClock = _spClock.Get();
			(*ppPresentationClock)->AddRef();
		}
	}

	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTMediaSink::Shutdown()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::Shutdown");
	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr)) {
		ComPtr<IMFStreamSink> spStream = _stream;
		static_cast<MSWinRTStreamSink *>(spStream.Get())->Shutdown();
		_stream = nullptr;
		_spClock.Reset();
		_IsShutdown = true;
	}
	return S_OK;
}

// IMFClockStateSink
IFACEMETHODIMP MSWinRTMediaSink::OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::OnClockStart");
	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr)) {
		MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::OnClockStart ts=%I64d\n", llClockStartOffset);
		_llStartTime = llClockStartOffset;
		hr = static_cast<MSWinRTStreamSink *>(_stream.Get())->Start(llClockStartOffset);
	}

	RETURN_HR(hr)
}

IFACEMETHODIMP MSWinRTMediaSink::OnClockStop(MFTIME hnsSystemTime)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::OnClockStop");
	AutoLock lock(_critSec);
	HRESULT hr = CheckShutdown();
	if (SUCCEEDED(hr)) {
		hr = static_cast<MSWinRTStreamSink *>(_stream.Get())->Stop();
	}

	RETURN_HR(hr)
}


IFACEMETHODIMP MSWinRTMediaSink::OnClockPause(MFTIME hnsSystemTime)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::OnClockPause");
	return MF_E_INVALID_STATE_TRANSITION;
}

IFACEMETHODIMP MSWinRTMediaSink::OnClockRestart(MFTIME hnsSystemTime)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::OnClockRestart");
	return MF_E_INVALID_STATE_TRANSITION;
}

IFACEMETHODIMP MSWinRTMediaSink::OnClockSetRate(/* [in] */ MFTIME hnsSystemTime, /* [in] */ float flRate)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::OnClockSetRate");
	return S_OK;
}

void MSWinRTMediaSink::ReportEndOfStream()
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::ReportEndOfStream");
	AutoLock lock(_critSec);
	++_cStreamsEnded;
}

void MSWinRTMediaSink::OnSampleAvailable(BYTE *buf, DWORD bufLen, LONGLONG presentationTime)
{
	MSWINRTMEDIASINK_DEBUG("MSWinRTMediaSink::OnSampleAvailable");
	if (_capture != nullptr) {
		_capture->OnSampleAvailable(buf, bufLen, presentationTime);
	}
}

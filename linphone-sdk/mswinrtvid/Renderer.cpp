/*
Renderer.cpp

mediastreamer2 library - modular sound and video processing and streaming
Windows Audio Session API sound card plugin for mediastreamer2
Copyright (C) 2010-2015 Belledonne Communications, Grenoble, France

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Renderer.h"
#include "ScopeLock.h"

#include <mediastreamer2/mscommon.h>

using namespace libmswinrtvid;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation::Collections;


MSWinRTExtensionManager^ MSWinRTExtensionManager::_instance = ref new MSWinRTExtensionManager();

MSWinRTExtensionManager::MSWinRTExtensionManager()
	: mMediaExtensionManager(nullptr), mExtensionManagerProperties(nullptr)
{
}

MSWinRTExtensionManager::~MSWinRTExtensionManager()
{
}

Platform::Boolean MSWinRTExtensionManager::Setup()
{
	if ((mMediaExtensionManager != nullptr) && (mExtensionManagerProperties != nullptr)) return true;

	using Windows::Foundation::ActivateInstance;
	HRESULT hr = ActivateInstance(HStringReference(RuntimeClass_Windows_Media_MediaExtensionManager).Get(), mMediaExtensionManager.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		ms_error("MSWinRTExtensionManager::Setup: Failed to create media extension manager %x", hr);
		return false;
	}
	ComPtr<IMap<HSTRING, IInspectable*>> props;
	hr = ActivateInstance(HStringReference(RuntimeClass_Windows_Foundation_Collections_PropertySet).Get(), props.ReleaseAndGetAddressOf());
	ComPtr<IPropertySet> propSet;
	props.As(&propSet);
	HStringReference clsid(L"MSWinRTVideo.SchemeHandler");
	HStringReference scheme(L"mswinrtvid:");
	hr = mMediaExtensionManager->RegisterSchemeHandlerWithSettings(clsid.Get(), scheme.Get(), propSet.Get());
	if (FAILED(hr)) {
		ms_error("MSWinRTExtensionManager::Setup: RegisterSchemeHandlerWithSettings failed %x", hr);
		return false;
	}
	mExtensionManagerProperties = props;
	return true;
}

Platform::Boolean MSWinRTExtensionManager::RegisterUrl(Platform::String^ url, Windows::Media::Core::MediaStreamSource^ source)
{
	boolean replaced;
	auto streamInspect = reinterpret_cast<IInspectable*>(source);
	return mExtensionManagerProperties->Insert(HStringReference(url->Data()).Get(), streamInspect, &replaced) == S_OK;
}

Platform::Boolean MSWinRTExtensionManager::UnregisterUrl(Platform::String^ url)
{
	return mExtensionManagerProperties->Remove(HStringReference(url->Data()).Get()) == S_OK;
}



MSWinRTRenderer::MSWinRTRenderer() :
	mMediaStreamSource(nullptr), mMediaEngine(nullptr), mUrl(nullptr),
	mForegroundProcess(nullptr), mMemoryMapping(nullptr), mSharedData(nullptr), mLock(nullptr), mShutdownEvent(nullptr), mEventAvailableEvent(nullptr)
{
}

MSWinRTRenderer::~MSWinRTRenderer()
{
	Close();
	mUrl = nullptr;
}

void MSWinRTRenderer::SetSwapChainPanel()
{
	Close();
	if (mSwapChainPanelName == nullptr) return;

	mMemoryMapping = OpenFileMappingFromApp(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, mSwapChainPanelName->Data());
	if ((mMemoryMapping == nullptr) || (mMemoryMapping == INVALID_HANDLE_VALUE)) {
		DWORD error = GetLastError();
		throw ref new Platform::COMException(HRESULT_FROM_WIN32(error));
	}
	mSharedData = (MSWinRTVideo::SharedData*)MapViewOfFileFromApp(mMemoryMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0LL, sizeof(*mSharedData));
	if (mSharedData == nullptr) {
		DWORD error = GetLastError();
		Close();
		throw ref new Platform::COMException(HRESULT_FROM_WIN32(error));
	}
	mSharedData->backgroundProcessId = GetCurrentProcessId();
	mForegroundProcess = OpenProcess(PROCESS_DUP_HANDLE, TRUE, mSharedData->foregroundProcessId);
	if ((mForegroundProcess == nullptr) || (mForegroundProcess == INVALID_HANDLE_VALUE)) {
		DWORD error = GetLastError();
		Close();
		throw ref new Platform::COMException(HRESULT_FROM_WIN32(error));
	}
	if (!DuplicateHandle(mForegroundProcess, mSharedData->foregroundLockMutex, GetCurrentProcess(), &mLock, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
		DWORD error = GetLastError();
		mLock = nullptr;
		Close();
		throw ref new Platform::COMException(HRESULT_FROM_WIN32(error));
	}
	if (!DuplicateHandle(mForegroundProcess, mSharedData->foregroundShutdownEvent, GetCurrentProcess(), &mShutdownEvent, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
		DWORD error = GetLastError();
		mLock = nullptr;
		Close();
		throw ref new Platform::COMException(HRESULT_FROM_WIN32(error));
	}
	if (!DuplicateHandle(mForegroundProcess, mSharedData->foregroundShutdownCompleteEvent, GetCurrentProcess(), &mShutdownCompleteEvent, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
		DWORD error = GetLastError();
		mLock = nullptr;
		Close();
		throw ref new Platform::COMException(HRESULT_FROM_WIN32(error));
	}
	if (!DuplicateHandle(mForegroundProcess, mSharedData->foregroundEventAvailableEvent, GetCurrentProcess(), &mEventAvailableEvent, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
		DWORD error = GetLastError();
		mLock = nullptr;
		Close();
		throw ref new Platform::COMException(HRESULT_FROM_WIN32(error));
	}
	if (!DuplicateHandle(mForegroundProcess, mSharedData->foregroundCommandAvailableEvent, GetCurrentProcess(), &mCommandAvailableEvent, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
		DWORD error = GetLastError();
		mLock = nullptr;
		Close();
		throw ref new Platform::COMException(HRESULT_FROM_WIN32(error));
	}
}

void MSWinRTRenderer::Close()
{
	if (mMediaEngineNotify != nullptr)
	{
		mMediaEngineNotify->SetCallback(nullptr);
		mMediaEngineNotify = nullptr;
	}
	if (mMediaEngine != nullptr)
	{
		mMediaEngine->Shutdown();
		mMediaEngine = nullptr;
	}
	if (mMediaEngineEx != nullptr)
	{
		mMediaEngineEx = nullptr;
	}
	if (mUrl != nullptr)
	{
		MSWinRTExtensionManager::Instance->UnregisterUrl(mUrl);
		mUrl = nullptr;
	}
	if (mMediaStreamSource != nullptr)
	{
		mMediaStreamSource->Stop();
		mMediaStreamSource = nullptr;
	}
	if (mDevice != nullptr) {
		mDevice = nullptr;
	}
	if (mDxGIManager != nullptr) {
		mDxGIManager = nullptr;
	}

	if (mSharedData != nullptr)
	{
		UnmapViewOfFile(mSharedData);
		mSharedData = nullptr;
	}
	if (mMemoryMapping != nullptr)
	{
		CloseHandle(mMemoryMapping);
		mMemoryMapping = nullptr;
	}
	if (mLock != nullptr)
	{
		CloseHandle(mLock);
		mLock = nullptr;
	}
	if (mShutdownEvent != nullptr)
	{
		CloseHandle(mShutdownEvent);
		mShutdownEvent = nullptr;
	}
	if (mEventAvailableEvent != nullptr)
	{
		CloseHandle(mEventAvailableEvent);
		mEventAvailableEvent = nullptr;
	}
	if (mForegroundProcess != nullptr)
	{
		CloseHandle(mForegroundProcess);
		mForegroundProcess = nullptr;
	}
	if (mShutdownCompleteEvent != nullptr)
	{
		SetEvent(mShutdownCompleteEvent);
		mShutdownCompleteEvent = nullptr;
	}
	if (mCommandAvailableEvent != nullptr)
	{
		CloseHandle(mCommandAvailableEvent);
		mCommandAvailableEvent = nullptr;
	}
	mSwapChainHandle.Close();
}

bool MSWinRTRenderer::Start()
{
	SetSwapChainPanel();
	mFrameWidth = mFrameHeight = mSwapChainPanelWidth = mSwapChainPanelHeight = 0;
	HRESULT hr = MSWinRTExtensionManager::Instance->Setup() ? S_OK : E_FAIL;
	if (FAILED(hr)) {
		SendErrorEvent(hr);
		return false;
	}
	hr = SetupDirectX();
	if (FAILED(hr)) {
		SendErrorEvent(hr);
		return false;
	}
	mMediaStreamSource = MediaStreamSource::CreateMediaSource();
	mUrl = "mswinrtvid://";
	GUID result;
	hr = CoCreateGuid(&result);
	if (FAILED(hr)) {
		SendErrorEvent(hr);
		return false;
	}
	Platform::Guid gd(result);
	mUrl += gd.ToString();
	hr = MSWinRTExtensionManager::Instance->RegisterUrl(mUrl, mMediaStreamSource->Source) ? S_OK : E_FAIL;
	if (FAILED(hr)) {
		SendErrorEvent(hr);
		return false;
	}
	BSTR sourceBSTR;
	sourceBSTR = SysAllocString(mUrl->Data());
	hr = mMediaEngine->SetSource(sourceBSTR);
	SysFreeString(sourceBSTR);
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::Start: Media engine SetSource failed %x", hr);
		SendErrorEvent(hr);
		return false;
	}
	hr = mMediaEngine->Load();
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::Start: Media engine Load failed %x", hr);
		SendErrorEvent(hr);
		return false;
	}
	return true;
}

void MSWinRTRenderer::Stop()
{
	Close();
}

void MSWinRTRenderer::Feed(Windows::Storage::Streams::IBuffer^ pBuffer, int width, int height)
{
	if ((mMediaStreamSource != nullptr) && (mSharedData != nullptr) && (mMediaEngineEx != nullptr)) {
		bool sizeChanged = false;
		HRESULT hr = mDevice->GetDeviceRemovedReason();
		if (FAILED(hr)) {
			ms_error("MSWinRTRenderer::Feed: Device lost %x", hr);
			Stop();
			Start();
		}
		if ((width != mFrameWidth) || (height != mFrameHeight)) {
			mFrameWidth = width;
			mFrameHeight = height;
			sizeChanged = true;
		}
		if ((mSharedData->width != mSwapChainPanelWidth) || (mSharedData->height != mSwapChainPanelHeight)) {
			mSwapChainPanelWidth = mSharedData->width;
			mSwapChainPanelHeight = mSharedData->height;
			sizeChanged = true;
		}
		if (sizeChanged) {
			MFVideoNormalizedRect srcSize = { 0.f, 0.f, (float)width, (float)height };
			RECT dstSize = { 0, 0, mSwapChainPanelWidth, mSwapChainPanelHeight };
			MFARGB backgroundColor = { 0, 0, 0, 0 };
			mMediaEngineEx->UpdateVideoStream(&srcSize, &dstSize, &backgroundColor);
		}

		mMediaStreamSource->Feed(pBuffer, width, height);
	}
}

HRESULT MSWinRTRenderer::SetupDirectX()
{
	mUseHardware = true;
	HRESULT hr = MFStartup(MF_VERSION);
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: MFStartup failed %x", hr);
		return hr;
	}
	hr = CreateDX11Device();
	if (FAILED(hr)) {
		return hr;
	}
	hr = MFCreateDXGIDeviceManager(&mResetToken, &mDxGIManager);
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: MFCreateDXGIDeviceManager failed %x", hr);
		return hr;
	}
	hr = mDxGIManager->ResetDevice(mDevice.Get(), mResetToken);
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: ResetDevice failed %x", hr);
		return hr;
	}
	ComPtr<IMFMediaEngineClassFactory> factory;
	hr = CoCreateInstanceBT(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: CoCreateInstance failed %x", hr);
		return hr;
	}
	ComPtr<IMFAttributes> attributes;
	hr = MFCreateAttributes(&attributes, 3);
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: MFCreateAttributes failed %x", hr);
		return hr;
	}
	hr = attributes->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, (IUnknown*)mDxGIManager.Get());
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: Set MF_MEDIA_ENGINE_DXGI_MANAGER attribute failed %x", hr);
		return hr;
	}
	mMediaEngineNotify = Make<MediaEngineNotify>();
	mMediaEngineNotify->SetCallback(this);
	hr = attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT_NV12);
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: Set MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT attribute failed %x", hr);
		return hr;
	}
	hr = attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, (IUnknown*)mMediaEngineNotify.Get());
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: Set MF_MEDIA_ENGINE_CALLBACK attribute failed %x", hr);
		return hr;
	}
	hr = factory->CreateInstance(MF_MEDIA_ENGINE_REAL_TIME_MODE, attributes.Get(), &mMediaEngine);
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: CreateInstance failed %x", hr);
		return hr;
	}
	hr = mMediaEngine.Get()->QueryInterface(__uuidof(IMFMediaEngineEx), (void**)&mMediaEngineEx);
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: QueryInterface failed %x", hr);
		return hr;
	}
	hr = mMediaEngineEx->EnableWindowlessSwapchainMode(TRUE);
	if (FAILED(hr)) {
		ms_error("MSWinRTRenderer::SetupDirectX: EnableWindowlessSwapchainMode failed %x", hr);
		return hr;
	}
	return S_OK;
}

//Returns true if this platform supports D3D11 (Libraries + hardware support)
bool MSWinRTRenderer::D3D11Supported()
{
	static const D3D_FEATURE_LEVEL levels[] = {
						   D3D_FEATURE_LEVEL_11_1,
						   D3D_FEATURE_LEVEL_11_0,
						   D3D_FEATURE_LEVEL_10_1,
						   D3D_FEATURE_LEVEL_10_0,
						   D3D_FEATURE_LEVEL_9_3,
						   D3D_FEATURE_LEVEL_9_2,
						   D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL FeatureLevel;
	HRESULT hr;
	UINT createFlag = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlag, levels, ARRAYSIZE(levels), D3D11_SDK_VERSION, &device, &FeatureLevel, nullptr);

	if (FAILED(hr)) {
		return false;
	}
	if (FeatureLevel != D3D_FEATURE_LEVEL_11_1 && FeatureLevel != D3D_FEATURE_LEVEL_11_0 && FeatureLevel != D3D_FEATURE_LEVEL_10_1) {
		return false;
	}
	return true;
}

HRESULT MSWinRTRenderer::CreateDX11Device()
{
	static const D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL FeatureLevel;
	HRESULT hr = S_OK;
	UINT createFlag = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;

	if (mUseHardware) {
		hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlag, levels, ARRAYSIZE(levels), D3D11_SDK_VERSION, &mDevice, &FeatureLevel, nullptr);
	}

	if (FAILED(hr)) {
		ms_warning("MSWinRTRenderer::CreateDX11Device: Failed to create hardware device, falling back to software %x", hr);
		mUseHardware = false;
	}

	if (!mUseHardware) {
		hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createFlag, levels, ARRAYSIZE(levels), D3D11_SDK_VERSION, &mDevice, &FeatureLevel, nullptr);
		if (FAILED(hr)) {
			ms_error("MSWinRTRenderer::CreateDX11Device: Failed to create WARP device %x", hr);
			return hr;
		}
	}

	if (mUseHardware) {
		ComPtr<ID3D10Multithread> multithread;
		hr = mDevice.Get()->QueryInterface(IID_PPV_ARGS(&multithread));
		if (FAILED(hr)) {
			ms_error("MSWinRTRenderer::CreateDX11Device: Failed to set hardware to multithreaded %x", hr);
			return hr;
		}
		multithread->SetMultithreadProtected(TRUE);
	}
	return hr;
}

void MSWinRTRenderer::SendSwapChainHandle(HANDLE swapChain)
{
	if ((swapChain == nullptr) || (swapChain == INVALID_HANDLE_VALUE)) return;

	ScopeLock lock(mLock);
	mSwapChainHandle.AssignHandle(swapChain, mSharedData->foregroundProcessId);
	HANDLE remoteHandle = mSwapChainHandle.GetRemoteHandle();
	if (remoteHandle != mSharedData->foregroundSwapChainHandle) {
		mSharedData->foregroundSwapChainHandle = remoteHandle;
		SetEvent(mEventAvailableEvent);
	}
}

void MSWinRTRenderer::SendErrorEvent(HRESULT hr)
{
	ScopeLock lock(mLock);
	if (mSharedData) mSharedData->error = hr;
	SetEvent(mEventAvailableEvent);
}

void MSWinRTRenderer::OnMediaEngineEvent(uint32 meEvent, uintptr_t param1, uint32 param2)
{
	HRESULT hr;
	HANDLE swapChainHandle;
	switch ((DWORD)meEvent) {
	case MF_MEDIA_ENGINE_EVENT_ERROR:
		ms_message("MSWinRTRenderer::OnMediaEngineEvent: Error");
		//SendErrorEvent((HRESULT)param2);
		break;
	case MF_MEDIA_ENGINE_EVENT_PLAYING:
	case MF_MEDIA_ENGINE_EVENT_FIRSTFRAMEREADY:
		mMediaEngineEx->GetVideoSwapchainHandle(&swapChainHandle);
		SendSwapChainHandle(swapChainHandle);
		break;
	case MF_MEDIA_ENGINE_EVENT_FORMATCHANGE:
		mMediaEngineEx->GetVideoSwapchainHandle(&swapChainHandle);
		ms_message("MSWinRTRenderer::OnMediaEngineEvent: Format change");
		SendSwapChainHandle(swapChainHandle);
		break;
	case MF_MEDIA_ENGINE_EVENT_CANPLAY:
		hr = mMediaEngine->Play();
		if (FAILED(hr)) {
			ms_error("MSWinRTRenderer::OnMediaEngineEvent: Error on play");
			SendErrorEvent(hr);
		}
		break;
	}
}

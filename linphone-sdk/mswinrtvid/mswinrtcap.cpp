/*
mswinrtcap.cpp

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


#include "mswinrtcap.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Storage;
using namespace libmswinrtvid;


bool MSWinRTCap::smInstantiated = false;
bctbx_list_t *MSWinRTCap::smCameras = NULL;


MSWinRTCapHelper::MSWinRTCapHelper() :
	mRotationKey({ 0xC380465D, 0x2271, 0x428C,{ 0x9B, 0x83, 0xEC, 0xEA, 0x3B, 0x4A, 0x85, 0xC1 } }),
	mDeviceOrientation(0), mAllocator(NULL)
{
	mInitializationCompleted = CreateEventEx(NULL, L"Local\\MSWinRTCapInitialization", 0, EVENT_ALL_ACCESS);
	if (!mInitializationCompleted) {
		ms_error("[MSWinRTCap] Could not create initialization event [%i]", GetLastError());
		return;
	}
	mStartCompleted = CreateEventEx(NULL, L"Local\\MSWinRTCapStart", 0, EVENT_ALL_ACCESS);
	if (!mStartCompleted) {
		ms_error("[MSWinRTCap] Could not create start event [%i]", GetLastError());
		return;
	}
	mStopCompleted = CreateEventEx(NULL, L"Local\\MSWinRTCapStop", 0, EVENT_ALL_ACCESS);
	if (!mStopCompleted) {
		ms_error("[MSWinRTCap] Could not create stop event [%i]", GetLastError());
		return;
	}

	ms_mutex_init(&mMutex, NULL);
	mAllocator = ms_yuv_buf_allocator_new();
	ms_queue_init(&mSamplesQueue);
}

MSWinRTCapHelper::~MSWinRTCapHelper()
{
	if (mCapture.Get() != nullptr) {
		mCapture->Failed -= mMediaCaptureFailedEventRegistrationToken;
	}
	if (mStopCompleted) {
		CloseHandle(mStopCompleted);
		mStopCompleted = NULL;
	}
	if (mStartCompleted) {
		CloseHandle(mStartCompleted);
		mStartCompleted = NULL;
	}
	if (mInitializationCompleted) {
		CloseHandle(mInitializationCompleted);
		mInitializationCompleted = NULL;
	}
	if (mAllocator != NULL) {
		ms_yuv_buf_allocator_free(mAllocator);
		mAllocator = NULL;
	}
	mEncodingProfile = nullptr;
	ms_mutex_destroy(&mMutex);
}

void MSWinRTCapHelper::OnCaptureFailed(MediaCapture^ sender, MediaCaptureFailedEventArgs^ errorEventArgs)
{
	errorEventArgs->Message;
}

bool MSWinRTCapHelper::Initialize(Platform::String^ DeviceId)
{
	bool isInitialized = false;
	mCapture = ref new MediaCapture();
	mMediaCaptureFailedEventRegistrationToken = mCapture->Failed += ref new MediaCaptureFailedEventHandler(this, &MSWinRTCapHelper::OnCaptureFailed);
	MediaCaptureInitializationSettings^ initSettings = ref new MediaCaptureInitializationSettings();
	initSettings->MediaCategory = MediaCategory::Communications;
	initSettings->VideoDeviceId = DeviceId;
	initSettings->StreamingCaptureMode = StreamingCaptureMode::Video;
	IAsyncAction^ initAction = mCapture->InitializeAsync(initSettings);
	initAction->Completed = ref new AsyncActionCompletedHandler([this, &isInitialized](IAsyncAction^ asyncInfo, Windows::Foundation::AsyncStatus asyncStatus) {
		switch (asyncStatus) {
		case Windows::Foundation::AsyncStatus::Completed:
			ms_message("[MSWinRTCap] InitializeAsync completed");
			isInitialized = true;
			break;
		case Windows::Foundation::AsyncStatus::Canceled:
			ms_warning("[MSWinRTCap] InitializeAsync has been canceled");
			break;
		case Windows::Foundation::AsyncStatus::Error:
			ms_error("[MSWinRTCap] InitializeAsync failed [0x%x]", asyncInfo->ErrorCode);
			break;
		default:
			break;
		}
		SetEvent(mInitializationCompleted);
	});

	WaitForSingleObjectEx(mInitializationCompleted, INFINITE, FALSE);
	return isInitialized;
}

bool MSWinRTCapHelper::StartCapture(MediaEncodingProfile^ EncodingProfile)
{
	bool isStarted = false;
	mEncodingProfile = EncodingProfile;
	MakeAndInitialize<MSWinRTMediaSink>(&mMediaSink, EncodingProfile->Video);
	static_cast<MSWinRTMediaSink *>(mMediaSink.Get())->SetCaptureFilter(this);
	ComPtr<IInspectable> spInspectable;
	HRESULT hr = mMediaSink.As(&spInspectable);
	if (FAILED(hr)) return false;
	IMediaExtension^ mediaExtension = safe_cast<IMediaExtension^>(reinterpret_cast<Object^>(spInspectable.Get()));
	IAsyncAction^ action = mCapture->StartRecordToCustomSinkAsync(EncodingProfile, mediaExtension);
	action->Completed = ref new AsyncActionCompletedHandler([this, &isStarted](IAsyncAction^ asyncAction, Windows::Foundation::AsyncStatus asyncStatus) {
		IAsyncAction^ capturePropertiesAction = nullptr;
		IMediaEncodingProperties^ props = nullptr;
		if (asyncStatus == Windows::Foundation::AsyncStatus::Completed) {
			ms_message("[MSWinRTCap] StartRecordToCustomSinkAsync completed");
			isStarted = true;
		} else {
			ms_error("[MSWinRTCap] StartRecordToCustomSinkAsync failed");
		}
		SetEvent(mStartCompleted);
	});
	WaitForSingleObjectEx(mStartCompleted, INFINITE, FALSE);
	return isStarted;
}

void MSWinRTCapHelper::StopCapture()
{
	static_cast<MSWinRTMediaSink *>(mMediaSink.Get())->SetCaptureFilter(nullptr);
	IAsyncAction^ action = mCapture->StopRecordAsync();
	action->Completed = ref new AsyncActionCompletedHandler([this](IAsyncAction^ asyncAction, Windows::Foundation::AsyncStatus asyncStatus) {
		static_cast<MSWinRTMediaSink *>(mMediaSink.Get())->Shutdown();
		mMediaSink = nullptr;
		if (asyncStatus == Windows::Foundation::AsyncStatus::Completed) {
			ms_message("[MSWinRTCap] StopRecordAsync completed");
		}
		else {
			ms_error("[MSWinRTCap] StopRecordAsync failed");
		}
		SetEvent(mStopCompleted);
	});
	WaitForSingleObjectEx(mStopCompleted, INFINITE, FALSE);
}

void MSWinRTCapHelper::OnSampleAvailable(BYTE *buf, DWORD bufLen, LONGLONG presentationTime)
{
	mblk_t *m;
	uint32_t timestamp = (uint32_t)((presentationTime / 10000LL) * 90LL);

	int w = mEncodingProfile->Video->Width;
	int h = mEncodingProfile->Video->Height;
	if ((mDeviceOrientation % 180) == 90) {
		w = mEncodingProfile->Video->Height;
		h = mEncodingProfile->Video->Width;
	}
	uint8_t *y = (uint8_t *)buf;
	uint8_t *cbcr = (uint8_t *)(buf + w * h);
	m = copy_ycbcrbiplanar_to_true_yuv_with_rotation(mAllocator, y, cbcr, mDeviceOrientation, w, h, mEncodingProfile->Video->Width, mEncodingProfile->Video->Width, TRUE);
	mblk_set_timestamp_info(m, timestamp);

	ms_mutex_lock(&mMutex);
	ms_queue_put(&mSamplesQueue, m);
	ms_mutex_unlock(&mMutex);
}

mblk_t * MSWinRTCapHelper::GetSample()
{
	ms_mutex_lock(&mMutex);
	mblk_t *m = ms_queue_get(&mSamplesQueue);
	ms_mutex_unlock(&mMutex);
	return m;
}

MSVideoSize MSWinRTCapHelper::SelectBestVideoSize(MSVideoSize vs)
{
	if ((CaptureDevice == nullptr) || (CaptureDevice->VideoDeviceController == nullptr)) {
		return vs;
	}

	MSVideoSize requestedSize;
	MSVideoSize bestFoundSize;
	MSVideoSize minSize = { 65536, 65536 };
	bestFoundSize.width = bestFoundSize.height = 0;
	requestedSize.width = vs.width;
	requestedSize.height = vs.height;

	IVectorView<IMediaEncodingProperties^>^ props = mCapture->VideoDeviceController->GetAvailableMediaStreamProperties(MediaStreamType::VideoRecord);
	for (unsigned int i = 0; i < props->Size; i++) {
		IMediaEncodingProperties^ encodingProp = props->GetAt(i);
		if (encodingProp->Type == L"Video") {
			IVideoEncodingProperties^ videoProp = static_cast<IVideoEncodingProperties^>(encodingProp);
			String^ subtype = videoProp->Subtype;
			if ((subtype == L"NV12") || (subtype == L"Unknown")) {
				MSVideoSize currentSize;
				currentSize.width = (int)videoProp->Width;
				currentSize.height = (int)videoProp->Height;
				ms_message("[MSWinRTCap] Seeing video size %ix%i", currentSize.width, currentSize.height);
				if (ms_video_size_greater_than(requestedSize, currentSize)) {
					if (ms_video_size_greater_than(currentSize, bestFoundSize)) {
						bestFoundSize = currentSize;
					}
				}
				if (ms_video_size_greater_than(minSize, currentSize)) {
					minSize = currentSize;
				}
			}
		}
	}

	if ((bestFoundSize.width == 0) && bestFoundSize.height == 0) {
		ms_warning("[MSWinRTCap] This camera does not support our video size, use requested size");
		return vs;
	}

	ms_message("[MSWinRTCap] Best video size is %ix%i", bestFoundSize.width, bestFoundSize.height);
	return bestFoundSize;
}


MSWinRTCap::MSWinRTCap()
	: mIsInitialized(false), mIsActivated(false), mIsStarted(false), mFps(15), mStartTime(0)
{
	if (smInstantiated) {
		ms_error("[MSWinRTCap] A video capture filter is already instantiated. A second one can not be created.");
		return;
	}

	mVideoSize.width = MS_VIDEO_SIZE_CIF_W;
	mVideoSize.height = MS_VIDEO_SIZE_CIF_H;
	mHelper = ref new MSWinRTCapHelper();
	smInstantiated = true;
}

MSWinRTCap::~MSWinRTCap()
{
	stop();
	deactivate();
	smInstantiated = false;
}


void MSWinRTCap::initialize()
{
	mIsInitialized = mHelper->Initialize(mDeviceId);
}

int MSWinRTCap::activate()
{
	if (!mIsInitialized) initialize();

	ms_average_fps_init(&mAvgFps, "[MSWinRTCap] fps=%f");
	ms_video_init_framerate_controller(&mFpsControl, mFps);
	configure();
	applyVideoSize();
	applyFps();
	mIsActivated = true;
	return 0;
}

int MSWinRTCap::deactivate()
{
	mIsActivated = false;
	mIsInitialized = false;
	return 0;
}

void MSWinRTCap::start()
{
	if (!mIsStarted && mIsActivated) {
		mIsStarted = mHelper->StartCapture(mEncodingProfile);
	}
}

void MSWinRTCap::stop()
{
	mblk_t *m;

	if (!mIsStarted) return;
	mHelper->StopCapture();

	// Free the samples that have not been sent yet
	while ((m = mHelper->GetSample()) != NULL) {
		freemsg(m);
	}
	mIsStarted = false;
}

int MSWinRTCap::feed(MSFilter *f)
{
	if (ms_video_capture_new_frame(&mFpsControl, f->ticker->time)) {
		mblk_t *im;
		
		// Send queued samples
		while ((im = mHelper->GetSample()) != NULL) {
			ms_queue_put(f->outputs[0], im);
			ms_average_fps_update(&mAvgFps, (uint32_t)f->ticker->time);
		}
	}

	return 0;
}


void MSWinRTCap::setFps(float fps)
{
	mFps = fps;
	ms_average_fps_init(&mAvgFps, "[MSWinRTCap] fps=%f");
	ms_video_init_framerate_controller(&mFpsControl, fps);
	applyFps();
}

float MSWinRTCap::getAverageFps()
{
	return ms_average_fps_get(&mAvgFps);
}

MSVideoSize MSWinRTCap::getVideoSize()
{
	MSVideoSize vs;
	if ((mHelper->DeviceOrientation % 180) == 90) {
		vs.width = mVideoSize.height;
		vs.height = mVideoSize.width;
	} else {
		vs = mVideoSize;
	}
	return vs;
}

void MSWinRTCap::setVideoSize(MSVideoSize vs)
{
	selectBestVideoSize(vs);
	applyVideoSize();
}

void MSWinRTCap::selectBestVideoSize(MSVideoSize vs)
{
	mVideoSize = mHelper->SelectBestVideoSize(vs);
}

void MSWinRTCap::setDeviceOrientation(int degrees)
{
	if (mFront) {
		mHelper->DeviceOrientation = degrees % 360;
	} else {
		mHelper->DeviceOrientation = (360 - degrees) % 360;
	}
}


void MSWinRTCap::applyFps()
{
	if (mEncodingProfile != nullptr) {
		mEncodingProfile->Video->FrameRate->Numerator = (unsigned int)mFps;
		mEncodingProfile->Video->FrameRate->Denominator = 1;
	}
}

void MSWinRTCap::applyVideoSize()
{
	if (mEncodingProfile != nullptr) {
		MSVideoSize vs = mVideoSize;
		mEncodingProfile->Video->Width = vs.width;
		mEncodingProfile->Video->Height = vs.height;
		mEncodingProfile->Video->PixelAspectRatio->Numerator = 1;
		mEncodingProfile->Video->PixelAspectRatio->Denominator = 1;
	}
}

void MSWinRTCap::configure()
{
	mEncodingProfile = ref new MediaEncodingProfile();
	mEncodingProfile->Audio = nullptr;
	mEncodingProfile->Container = nullptr;
	MSVideoSize vs = mVideoSize;
	mEncodingProfile->Video = VideoEncodingProperties::CreateUncompressed(MediaEncodingSubtypes::Nv12, vs.width, vs.height);
}

void MSWinRTCap::addCamera(MSWebCamManager *manager, MSWebCamDesc *desc, DeviceInformation^ DeviceInfo)
{
	char *idStr = NULL;
	char *nameStr = NULL;
	size_t returnlen;
	size_t inputlen = wcslen(DeviceInfo->Name->Data()) + 1;
	nameStr = (char *)ms_malloc(inputlen);
	if (!nameStr || wcstombs_s(&returnlen, nameStr, inputlen, DeviceInfo->Name->Data(), inputlen) != 0) {
		ms_error("MSWinRTCap: Cannot convert webcam name to multi-byte string.");
		goto error;
	}
	const wchar_t *id = DeviceInfo->Id->Data();
	inputlen = wcslen(id) + 1;
	idStr = (char *)ms_malloc(inputlen);
	if (!idStr || wcstombs_s(&returnlen, idStr, inputlen, DeviceInfo->Id->Data(), inputlen) != 0) {
		ms_error("MSWinRTCap: Cannot convert webcam id to multi-byte string.");
		goto error;
	}
	char *name = bctbx_strdup_printf("%s--%s", nameStr, idStr);

	MSWebCam *cam = ms_web_cam_new(desc);
	cam->name = name;
	WinRTWebcam *winrtwebcam = new WinRTWebcam();
	winrtwebcam->id_vector = new std::vector<wchar_t>(wcslen(id) + 1);
	wcscpy_s(&winrtwebcam->id_vector->front(), winrtwebcam->id_vector->size(), id);
	winrtwebcam->id = &winrtwebcam->id_vector->front();
	cam->data = winrtwebcam;
	if (DeviceInfo->EnclosureLocation != nullptr) {
		if (DeviceInfo->EnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Front) {
			winrtwebcam->external = FALSE;
			winrtwebcam->front = TRUE;
			smCameras = bctbx_list_append(smCameras, cam);
		} else {
			if (DeviceInfo->EnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Unknown) {
				winrtwebcam->external = TRUE;
				winrtwebcam->front = TRUE;
			} else {
				winrtwebcam->external = FALSE;
				winrtwebcam->front = FALSE;
			}
			smCameras = bctbx_list_prepend(smCameras, cam);
		}
	} else {
		winrtwebcam->external = TRUE;
		winrtwebcam->front = TRUE;
		smCameras = bctbx_list_prepend(smCameras, cam);
	}

error:
	if (nameStr) {
		ms_free(nameStr);
	}
	if (idStr) {
		ms_free(idStr);
	}
}

void MSWinRTCap::registerCameras(MSWebCamManager *manager)
{
	if (bctbx_list_size(smCameras) == 0) {
		ms_warning("[MSWinRTCap] No camera detected!");
	}
	for (size_t i = 0; i < bctbx_list_size(smCameras); i++) {
		ms_web_cam_manager_prepend_cam(manager, (MSWebCam *)bctbx_list_nth_data(smCameras, (int)i));
	}
	bctbx_list_free(smCameras);
	smCameras = NULL;
}

void MSWinRTCap::detectCameras(MSWebCamManager *manager, MSWebCamDesc *desc)
{
	HANDLE eventCompleted = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
	if (!eventCompleted) {
		ms_error("[MSWinRTCap] Could not create camera detection event [%i]", GetLastError());
		return;
	}
	IAsyncOperation<DeviceInformationCollection^>^ enumOperation = DeviceInformation::FindAllAsync(DeviceClass::VideoCapture);
	enumOperation->Completed = ref new AsyncOperationCompletedHandler<DeviceInformationCollection^>(
		[manager, desc, eventCompleted](IAsyncOperation<DeviceInformationCollection^>^ asyncOperation, Windows::Foundation::AsyncStatus asyncStatus) {
		if (asyncStatus == Windows::Foundation::AsyncStatus::Completed) {
			DeviceInformationCollection^ DeviceInfoCollection = asyncOperation->GetResults();
			if ((DeviceInfoCollection == nullptr) || (DeviceInfoCollection->Size == 0)) {
				ms_error("[MSWinRTCap] No webcam found");
			}
			else {
				try {
					for (unsigned int i = 0; i < DeviceInfoCollection->Size; i++) {
						addCamera(manager, desc, DeviceInfoCollection->GetAt(i));
					}
					registerCameras(manager);
				}
				catch (Platform::Exception^ e) {
					ms_error("[MSWinRTCap] Error of webcam detection");
				}
			}
		} else {
			ms_error("[MSWinRTCap] Cannot enumerate webcams");
		}
		SetEvent(eventCompleted);
	});
	WaitForSingleObjectEx(eventCompleted, INFINITE, FALSE);
}

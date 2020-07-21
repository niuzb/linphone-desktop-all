/*
mswinrtdis.cpp

mediastreamer2 library - modular sound and video processing and streaming
Windows Audio Session API sound card plugin for mediastreamer2
Copyright (C) 2010-2013 Belledonne Communications, Grenoble, France

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "mswinrtdis.h"
#include "VideoBuffer.h"

using namespace libmswinrtvid;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Media::Core;
using namespace Windows::Media::MediaProperties;


//#define MSWINRTDIS_DEBUG


static void _startMediaElement(Windows::UI::Xaml::Controls::MediaElement^ mediaElement, Windows::Media::Core::MediaStreamSource^ mediaStreamSource)
{
	ms_message("[MSWinRTDis] Play MediaElement");
	mediaElement->RealTimePlayback = true;
	mediaElement->SetMediaStreamSource(mediaStreamSource);
	mediaElement->Play();
}

static void _stopMediaElement(Windows::UI::Xaml::Controls::MediaElement^ mediaElement, MSWinRTDisSampleHandler^ sampleHandler)
{
	ms_message("[MSWinRTDis] Stop MediaElement");
	mediaElement->Stop();
}


MSWinRTDisSampleHandler::MSWinRTDisSampleHandler() :
	mSample(nullptr), mReferenceTime(0), mPixFmt(MS_YUV420P), mWidth(MS_VIDEO_SIZE_CIF_W), mHeight(MS_VIDEO_SIZE_CIF_H), mStarted(false)
{
	mDeferralQueue = ref new Platform::Collections::Vector<MSWinRTDisDeferral^>();
}

MSWinRTDisSampleHandler::~MSWinRTDisSampleHandler()
{
}

void MSWinRTDisSampleHandler::StartMediaElement()
{
	if (mMediaElement != nullptr) {
		VideoEncodingProperties^ videoEncodingProperties;
		videoEncodingProperties = VideoEncodingProperties::CreateUncompressed(MediaEncodingSubtypes::Nv12, this->Width, this->Height);
		VideoStreamDescriptor^ videoStreamDescriptor = ref new VideoStreamDescriptor(videoEncodingProperties);
		MediaStreamSource^ mediaStreamSource = ref new MediaStreamSource(videoStreamDescriptor);
		mediaStreamSource->SampleRequested += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Core::MediaStreamSource ^, Windows::Media::Core::MediaStreamSourceSampleRequestedEventArgs ^>(this, &MSWinRTDisSampleHandler::OnSampleRequested);
		Windows::UI::Xaml::Controls::MediaElement^ mediaElement = mMediaElement;
		bool inUIThread = mediaElement->Dispatcher->HasThreadAccess;
		mReferenceTime = 0;
		if (inUIThread) {
			// We are in the UI thread
			_startMediaElement(mediaElement, mediaStreamSource);
			mStarted = true;
		}
		else {
			// Ask the dispatcher to run this code in the UI thread
			mediaElement->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([mediaElement, mediaStreamSource, this]() {
				_startMediaElement(mediaElement, mediaStreamSource);
				mStarted = true;
			}));
		}
	}
}

void MSWinRTDisSampleHandler::StopMediaElement()
{
	if (mMediaElement != nullptr) {
		Windows::UI::Xaml::Controls::MediaElement^ mediaElement = mMediaElement;
		if (mediaElement->Dispatcher->HasThreadAccess) {
			// We are in the UI thread
			_stopMediaElement(mediaElement, this);
			mDeferralQueue->Clear();
			mStarted = false;
		}
		else {
			// Ask the dispatcher to run this code in the UI thread
			mediaElement->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([mediaElement, this]() {
				_stopMediaElement(mediaElement, this);
				mDeferralQueue->Clear();
				mStarted = false;
			}));
		}
	}
	else {
		mDeferralQueue->Clear();
		mStarted = false;
	}
}

void MSWinRTDisSampleHandler::Feed(Windows::Storage::Streams::IBuffer^ pBuffer)
{
	mMutex.lock();
	if (!mStarted) {
		StartMediaElement();
	}
	mSample = pBuffer;
	if (mDeferralQueue->Size > 0) {
#ifdef MSWINRTDIS_DEBUG
		ms_message("[MSWinRTDis] Feed answer deferral");
#endif
		MSWinRTDisDeferral^ deferral = mDeferralQueue->GetAt(0);
		mDeferralQueue->RemoveAt(0);
		AnswerSampleRequest(deferral->Request);
		deferral->Deferral->Complete();
	}
#ifdef MSWINRTDIS_DEBUG
	else {
		ms_message("[MSWinRTDis] Feed");
	}
#endif
	mMutex.unlock();
}

void MSWinRTDisSampleHandler::OnSampleRequested(Windows::Media::Core::MediaStreamSource^ sender, Windows::Media::Core::MediaStreamSourceSampleRequestedEventArgs^ args)
{
	MediaStreamSourceSampleRequest^ request = args->Request;
	VideoStreamDescriptor^ videoStreamDescriptor = dynamic_cast<VideoStreamDescriptor^>(request->StreamDescriptor);
	if (videoStreamDescriptor == nullptr) {
		ms_warning("[MSWinRTDis] OnSampleRequested not for a video stream!");
		return;
	}
	mMutex.lock();
	if (mSample == nullptr) {
#ifdef MSWINRTDIS_DEBUG
		ms_message("[MSWinRTDis] OnSampleRequested defer");
#endif
		mDeferralQueue->Append(ref new MSWinRTDisDeferral(request, request->GetDeferral()));
	} else {
#ifdef MSWINRTDIS_DEBUG
		ms_message("[MSWinRTDis] OnSampleRequested answer");
#endif
		AnswerSampleRequest(request);
	}
	mMutex.unlock();
}

void MSWinRTDisSampleHandler::AnswerSampleRequest(Windows::Media::Core::MediaStreamSourceSampleRequest^ sampleRequest)
{
	TimeSpan ts;
	UINT64 CurrentTime = GetTickCount64() * 10000LL;
	if (mReferenceTime == 0) {
		mReferenceTime = CurrentTime;
	}
	ts.Duration = CurrentTime - mReferenceTime;
	sampleRequest->Sample = MediaStreamSample::CreateFromBuffer(mSample, ts);
	mSample = nullptr;
}

void MSWinRTDisSampleHandler::RequestMediaElementRestart()
{
	mMutex.lock();
	ms_message("[MSWinRTDis] RequestMediaElementRestart");
	if (mReferenceTime != 0) {
		StopMediaElement();
		mReferenceTime = 0;
	}
	mMutex.unlock();
}



MSWinRTDis::MSWinRTDis()
	: mIsInitialized(false), mIsActivated(false), mIsStarted(false), mSampleHandler(nullptr)
{
	mSampleHandler = ref new MSWinRTDisSampleHandler();
	mIsInitialized = true;
}

MSWinRTDis::~MSWinRTDis()
{
	stop();
}

int MSWinRTDis::activate()
{
	if (!mIsInitialized) return -1;
	mIsActivated = true;
	return 0;
}

int MSWinRTDis::deactivate()
{
	mIsActivated = false;
	return 0;
}

void MSWinRTDis::start()
{
	if (!mIsStarted && mIsActivated) {
		mIsStarted = true;
	}
}

void MSWinRTDis::stop()
{
	if (mIsStarted) {
		mIsStarted = false;
		mSampleHandler->StopMediaElement();
	}
}

int MSWinRTDis::feed(MSFilter *f)
{
	if (mIsStarted) {
		mblk_t *im;
		mblk_t *om;

		if ((f->inputs[0] != NULL) && ((im = ms_queue_peek_last(f->inputs[0])) != NULL)) {
			int size = 0;
			MSPicture inbuf;
			MSPicture outbuf;
			if (ms_yuv_buf_init_from_mblk(&inbuf, im) == 0) {
				if ((inbuf.w != mSampleHandler->Width) || (inbuf.h != mSampleHandler->Height)) {
					mSampleHandler->Width = inbuf.w;
					mSampleHandler->Height = inbuf.h;
					mSampleHandler->RequestMediaElementRestart();
				}
				om = ms_yuv_buf_alloc(&outbuf, inbuf.w, inbuf.h);
				int ysize = inbuf.w * inbuf.h;
				int usize = ysize / 4;
				uint8_t *buffer = outbuf.planes[0];
				memcpy(buffer, inbuf.planes[0], ysize);
				for (int i = 0; i < usize; i++) {
					buffer[ysize + (i * 2)] = inbuf.planes[1][i];
					buffer[ysize + (i * 2) + 1] = inbuf.planes[2][i];
				}
				Microsoft::WRL::ComPtr<VideoBuffer> spVideoBuffer = NULL;
				Microsoft::WRL::MakeAndInitialize<VideoBuffer>(&spVideoBuffer, buffer, (int)msgdsize(om), om);
				mSampleHandler->Feed(VideoBuffer::GetIBuffer(spVideoBuffer));
			}
		}
	}

	if (f->inputs[0] != NULL) {
		ms_queue_flush(f->inputs[0]);
	}
	if (f->inputs[1] != NULL) {
		ms_queue_flush(f->inputs[1]);
	}

	return 0;
}

MSVideoSize MSWinRTDis::getVideoSize()
{
	MSVideoSize vs;
	vs.width = mSampleHandler->Width;
	vs.height = mSampleHandler->Height;
	return vs;
}

void MSWinRTDis::setVideoSize(MSVideoSize vs)
{
	mSampleHandler->Width = vs.width;
	mSampleHandler->Height = vs.height;
}

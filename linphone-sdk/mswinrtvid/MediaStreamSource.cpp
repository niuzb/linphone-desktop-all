/*
MediaStreamSource.cpp

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

#include "MediaStreamSource.h"
#include <mfapi.h>
#include <wrl.h>
#include <robuffer.h>

#include <mediastreamer2/msvideo.h>

using namespace Windows::Media::Core;
using namespace Windows::Media::MediaProperties;
using Microsoft::WRL::ComPtr;


libmswinrtvid::MediaStreamSource::MediaStreamSource()
	: mMediaStreamSource(nullptr), mTimeStamp(0LL), mInitialTimeStamp(0LL)
{
	mDeferralQueue = ref new Platform::Collections::Vector<SampleRequestDeferral^>();
}

libmswinrtvid::MediaStreamSource::~MediaStreamSource()
{
}

libmswinrtvid::MediaStreamSource^ libmswinrtvid::MediaStreamSource::CreateMediaSource()
{
	libmswinrtvid::MediaStreamSource^ streamState = ref new libmswinrtvid::MediaStreamSource();
	VideoEncodingProperties^ videoProperties = VideoEncodingProperties::CreateUncompressed(MediaEncodingSubtypes::Nv12, 40, 40);
	streamState->mVideoDesc = ref new VideoStreamDescriptor(videoProperties);
	streamState->mVideoDesc->EncodingProperties->Width = 40;
	streamState->mVideoDesc->EncodingProperties->Height = 40;
	streamState->mMediaStreamSource = ref new Windows::Media::Core::MediaStreamSource(streamState->mVideoDesc);
	streamState->mMediaStreamSource->SampleRequested += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Core::MediaStreamSource ^, MediaStreamSourceSampleRequestedEventArgs ^>(
		[streamState](Windows::Media::Core::MediaStreamSource^ sender, MediaStreamSourceSampleRequestedEventArgs^ args) {
			streamState->OnSampleRequested(sender, args);
		});
	return streamState;
}

void libmswinrtvid::MediaStreamSource::OnSampleRequested(Windows::Media::Core::MediaStreamSource ^sender, MediaStreamSourceSampleRequestedEventArgs ^args)
{
	MediaStreamSourceSampleRequest^ request = args->Request;
	if (request == nullptr) {
		return;
	}
	mMutex.lock();
	if (mSample == nullptr) {
		mDeferralQueue->Append(ref new SampleRequestDeferral(request, request->GetDeferral()));
	} else {
		AnswerSampleRequest(request);
	}
	mMutex.unlock();
}

void libmswinrtvid::MediaStreamSource::Feed(Windows::Storage::Streams::IBuffer^ pBuffer, int width, int height)
{
	mMutex.lock();
	mSample = ref new Sample(pBuffer, width, height);
	if (mDeferralQueue->Size > 0) {
		SampleRequestDeferral^ deferral = mDeferralQueue->GetAt(0);
		mDeferralQueue->RemoveAt(0);
		AnswerSampleRequest(deferral->Request);
		deferral->Deferral->Complete();
	}
	mMutex.unlock();
}

void libmswinrtvid::MediaStreamSource::Stop()
{
	mMediaStreamSource = nullptr;
	mVideoDesc = nullptr;
	mDeferralQueue = nullptr;
}

void libmswinrtvid::MediaStreamSource::AnswerSampleRequest(Windows::Media::Core::MediaStreamSourceSampleRequest^ sampleRequest)
{
	ComPtr<IMFMediaStreamSourceSampleRequest> spRequest;
	HRESULT hr = reinterpret_cast<IInspectable*>(sampleRequest)->QueryInterface(spRequest.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		ms_error("MediaStreamSource::AnswerSampleRequest: QueryInterface failed %x", hr);
		return;
	}
	ComPtr<IMFSample> spSample;
	hr = MFCreateSample(spSample.GetAddressOf());
	if (FAILED(hr)) {
		ms_error("MediaStreamSource::AnswerSampleRequest: MFCreateSample failed %x", hr);
		return;
	}
	LONGLONG timeStamp = GetTickCount64();
	if (mInitialTimeStamp == 0) {
		mInitialTimeStamp = timeStamp;
	}
	LONGLONG duration;
	if (mTimeStamp == 0LL)
	{
		duration = (LONGLONG)((1.0 / 30.0) * 1000 * 1000 * 10);
	}
	else
	{
		duration = (timeStamp - mTimeStamp) * 10000LL;
	}
	mTimeStamp = timeStamp;
	spSample->SetSampleDuration(duration);
	// Set frame 40ms into the future
	LONGLONG sampleTime = (mTimeStamp - mInitialTimeStamp + 40LL) * 10000LL;
	spSample->SetSampleTime(sampleTime);
	ComPtr<IMFMediaBuffer> mediaBuffer;
	if ((mVideoDesc->EncodingProperties->Width != mSample->Width) || (mVideoDesc->EncodingProperties->Height != mSample->Height)) {
		mVideoDesc->EncodingProperties->Width = mSample->Width;
		mVideoDesc->EncodingProperties->Height = mSample->Height;
	}
	hr = MFCreate2DMediaBuffer(mVideoDesc->EncodingProperties->Width, mVideoDesc->EncodingProperties->Height, 0x3231564E /* NV12 */, FALSE, mediaBuffer.GetAddressOf());
	if (FAILED(hr)) {
		ms_error("MediaStreamSource::AnswerSampleRequest: MFCreate2DMediaBuffer failed %x", hr);
		return;
	}
	spSample->AddBuffer(mediaBuffer.Get());
	RenderFrame(mediaBuffer.Get());
	hr = spRequest->SetSample(spSample.Get());
	if (FAILED(hr)) {
		ms_error("MediaStreamSource::AnswerSampleRequest: SetSample failed %x", hr);
	}
	mSample = nullptr;
}

void libmswinrtvid::MediaStreamSource::RenderFrame(IMFMediaBuffer* mediaBuffer)
{
	ComPtr<IMF2DBuffer2> imageBuffer;
	HRESULT hr = mediaBuffer->QueryInterface(imageBuffer.GetAddressOf());
	if (FAILED(hr)) {
		ms_error("MediaStreamSource::RenderFrame: mediaBuffer QueryInterface failed %x", hr);
		return;
	}

	ComPtr<Windows::Storage::Streams::IBufferByteAccess> sampleByteAccess;
	hr = reinterpret_cast<IInspectable*>(mSample->Buffer)->QueryInterface(IID_PPV_ARGS(&sampleByteAccess));
	if (FAILED(hr)) {
		ms_error("MediaStreamSource::RenderFrame: mSample->Buffer QueryInterface failed %x", hr);
		return;
	}

	BYTE* destRawData;
	BYTE* buffer;
	LONG pitch;
	DWORD destMediaBufferSize;
	hr = imageBuffer->Lock2DSize(MF2DBuffer_LockFlags_Write, &destRawData, &pitch, &buffer, &destMediaBufferSize);
	if (FAILED(hr)) {
		ms_error("MediaStreamSource::RenderFrame: Lock2DSize failed %x", hr);
		return;
	}

	BYTE* srcRawData = nullptr;
	sampleByteAccess->Buffer(&srcRawData);
	MSPicture src_pic;
	MSPicture dst_pic;
	ms_yuv_buf_init(&src_pic, mSample->Width, mSample->Height, mSample->Width, srcRawData);
	ms_yuv_buf_init(&dst_pic, mSample->Width, mSample->Height, pitch, destRawData);
	/* Copy Y plane */
	uint8_t *src_plane = src_pic.planes[0];
	uint8_t *dst_plane = dst_pic.planes[0];
	for (int i = 0; i < src_pic.h; ++i) {
		memcpy(dst_plane, src_plane, src_pic.w);
		src_plane += src_pic.w;
		dst_plane += pitch;
	}
	/* Copy U & V plane with interleaving */
	dst_plane = dst_pic.planes[1];
	for (int i = 0; i < src_pic.h / 2; i++) {
		for (int j = 0; j < src_pic.w / 2; j++) {
			dst_plane[i * pitch + j * 2] = src_pic.planes[1][i * (src_pic.w / 2) + j];
			dst_plane[i * pitch + j * 2 + 1] = src_pic.planes[2][i * (src_pic.w / 2) + j];
		}
	}
	imageBuffer->Unlock2D();
}

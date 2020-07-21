/*
MediaStreamSource.h

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

#pragma once

#include <Mfidl.h>
#include <mutex>
#include <collection.h>


namespace libmswinrtvid
{
	private ref class SampleRequestDeferral sealed
	{
	public:
		SampleRequestDeferral(Windows::Media::Core::MediaStreamSourceSampleRequest^ request, Windows::Media::Core::MediaStreamSourceSampleRequestDeferral^ deferral)
		{
			this->Request = request;
			this->Deferral = deferral;
		}

		property Windows::Media::Core::MediaStreamSourceSampleRequest^ Request
		{
			Windows::Media::Core::MediaStreamSourceSampleRequest^ get() { return mRequest; }
			void set(Windows::Media::Core::MediaStreamSourceSampleRequest^ value) { mRequest = value; }
		}

		property Windows::Media::Core::MediaStreamSourceSampleRequestDeferral^ Deferral
		{
			Windows::Media::Core::MediaStreamSourceSampleRequestDeferral^ get() { return mDeferral; }
			void set(Windows::Media::Core::MediaStreamSourceSampleRequestDeferral^ value) { mDeferral = value; }
		}

	private:
		~SampleRequestDeferral() {};

		Windows::Media::Core::MediaStreamSourceSampleRequest^ mRequest;
		Windows::Media::Core::MediaStreamSourceSampleRequestDeferral^ mDeferral;
	};

	private ref class Sample sealed
	{
	public:
		Sample(Windows::Storage::Streams::IBuffer^ buffer, int width, int height)
		{
			mBuffer = buffer;
			mWidth = width;
			mHeight = height;
		}

		property Windows::Storage::Streams::IBuffer^ Buffer
		{
			Windows::Storage::Streams::IBuffer^ get() { return mBuffer; }
		}

		property int Width
		{
			int get() { return mWidth; }
		}

		property int Height
		{
			int get() { return mHeight; }
		}

	private:
		~Sample() {};

		Windows::Storage::Streams::IBuffer^ mBuffer;
		int mWidth;
		int mHeight;
	};

	ref class MediaStreamSource sealed
	{
	public:
		static MediaStreamSource^ CreateMediaSource();

		void Feed(Windows::Storage::Streams::IBuffer^ pBuffer, int width, int height);
		void Stop();

		property Windows::Media::Core::MediaStreamSource^ Source
		{
			Windows::Media::Core::MediaStreamSource^ get() { return mMediaStreamSource; }
		}

	private:
		MediaStreamSource();
		~MediaStreamSource();

		void OnSampleRequested(Windows::Media::Core::MediaStreamSource ^sender, Windows::Media::Core::MediaStreamSourceSampleRequestedEventArgs ^args);
		void AnswerSampleRequest(Windows::Media::Core::MediaStreamSourceSampleRequest^ sampleRequest);
		void RenderFrame(IMFMediaBuffer* mediaBuffer);

		Windows::Media::Core::MediaStreamSource^ mMediaStreamSource;
		Windows::Media::Core::VideoStreamDescriptor^ mVideoDesc;
		Platform::Collections::Vector<SampleRequestDeferral^>^ mDeferralQueue;
		Sample^ mSample;
		uint64 mTimeStamp;
		uint64 mInitialTimeStamp;
		std::mutex mMutex;
	};
}

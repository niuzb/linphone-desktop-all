/*
mswinrtdis.h

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


#pragma once


#include "mswinrtvid.h"

#include <mediastreamer2/rfc3984.h>

#include <collection.h>
#include <ppltasks.h>
#include <mutex>
#include <robuffer.h>
#include <windows.storage.streams.h>


namespace libmswinrtvid
{
	class MSWinRTDis;

	private ref class MSWinRTDisDeferral sealed
	{
	public:
		MSWinRTDisDeferral(Windows::Media::Core::MediaStreamSourceSampleRequest^ request, Windows::Media::Core::MediaStreamSourceSampleRequestDeferral^ deferral)
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
		~MSWinRTDisDeferral() {};

		Windows::Media::Core::MediaStreamSourceSampleRequest^ mRequest;
		Windows::Media::Core::MediaStreamSourceSampleRequestDeferral^ mDeferral;
	};

	private ref class MSWinRTDisSampleHandler sealed
	{
	public:
		MSWinRTDisSampleHandler();
		virtual ~MSWinRTDisSampleHandler();
		void StartMediaElement();
		void StopMediaElement();
		void Feed(Windows::Storage::Streams::IBuffer^ pBuffer);
		void OnSampleRequested(Windows::Media::Core::MediaStreamSource ^sender, Windows::Media::Core::MediaStreamSourceSampleRequestedEventArgs ^args);
		void RequestMediaElementRestart();

		property unsigned int PixFmt
		{
			unsigned int get() { return mPixFmt; }
			void set(unsigned int value) { mPixFmt = (MSPixFmt)value; }
		}

		property Windows::UI::Xaml::Controls::MediaElement^ MediaElement
		{
			Windows::UI::Xaml::Controls::MediaElement^ get() { return mMediaElement; }
			void set(Windows::UI::Xaml::Controls::MediaElement^ value) { mMediaElement = value; }
		}

		property int Width
		{
			int get() { return mWidth; }
			void set(int value) { mWidth = value; }
		}

		property int Height
		{
			int get() { return mHeight; }
			void set(int value) { mHeight = value; }
		}

	private:
		void AnswerSampleRequest(Windows::Media::Core::MediaStreamSourceSampleRequest^ sampleRequest);

		Windows::Storage::Streams::IBuffer^ mSample;
		Platform::Collections::Vector<MSWinRTDisDeferral^>^ mDeferralQueue;
		Windows::UI::Xaml::Controls::MediaElement^ mMediaElement;
		UINT64 mReferenceTime;
		std::mutex mMutex;
		MSPixFmt mPixFmt;
		int mWidth;
		int mHeight;
		bool mStarted;
	};


	class MSWinRTDis {
	public:
		MSWinRTDis();
		virtual ~MSWinRTDis();

		int activate();
		int deactivate();
		bool isStarted() { return mIsStarted; }
		void start();
		void stop();
		int feed(MSFilter *f);
		MSVideoSize getVideoSize();
		void setVideoSize(MSVideoSize vs);
		void setMediaElement(Windows::UI::Xaml::Controls::MediaElement^ mediaElement) { mSampleHandler->MediaElement = mediaElement; }

	private:
		bool mIsInitialized;
		bool mIsActivated;
		bool mIsStarted;
		MSWinRTDisSampleHandler^ mSampleHandler;
		Windows::Media::Core::MediaStreamSource^ mMediaStreamSource;
	};
}

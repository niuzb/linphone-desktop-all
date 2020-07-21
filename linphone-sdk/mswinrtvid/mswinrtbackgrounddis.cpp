/*
mswinrtbackgrounddis.cpp

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

#include <wrl.h>

#include "mswinrtbackgrounddis.h"
#include "VideoBuffer.h"


using namespace libmswinrtvid;


MSWinRTBackgroundDis::MSWinRTBackgroundDis()
	: mIsActivated(false), mIsStarted(false)
{
	mRenderer = ref new MSWinRTRenderer();
}

MSWinRTBackgroundDis::~MSWinRTBackgroundDis()
{
	stop();
	mRenderer = nullptr;
}

int MSWinRTBackgroundDis::activate()
{
	mIsActivated = true;
	return 0;
}

int MSWinRTBackgroundDis::deactivate()
{
	mIsActivated = false;
	return 0;
}

void MSWinRTBackgroundDis::start()
{
	if (!mIsStarted && mIsActivated) {
		mIsStarted = mRenderer->Start();
	}
}

void MSWinRTBackgroundDis::stop()
{
	if (mIsStarted) {
		mRenderer->Stop();
		mIsStarted = false;
	}
}

int MSWinRTBackgroundDis::feed(MSFilter *f)
{
	if (mIsStarted) {
		mblk_t *im;

		if ((f->inputs[0] != NULL) && ((im = ms_queue_peek_last(f->inputs[0])) != NULL)) {
			MSPicture buf;
			if (ms_yuv_buf_init_from_mblk(&buf, im) == 0) {
				ms_queue_remove(f->inputs[0], im);
				Microsoft::WRL::ComPtr<VideoBuffer> spVideoBuffer = NULL;
				Microsoft::WRL::MakeAndInitialize<VideoBuffer>(&spVideoBuffer, buf.planes[0], (int)msgdsize(im), im);
				mRenderer->Feed(VideoBuffer::GetIBuffer(spVideoBuffer), buf.w, buf.h);
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

MSVideoSize MSWinRTBackgroundDis::getVideoSize()
{
	MSVideoSize vs;
	vs.width = mRenderer->FrameWidth;
	vs.height = mRenderer->FrameHeight;
	return vs;
}

void MSWinRTBackgroundDis::setSwapChainPanel(Platform::String ^swapChainPanelName)
{
	mRenderer->SwapChainPanelName = swapChainPanelName;
}

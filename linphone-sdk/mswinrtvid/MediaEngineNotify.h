/*
MediaEngineNotify.h

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

#include <wrl.h>
#include <mfmediaengine.h>


namespace libmswinrtvid
{
	public interface struct MediaEngineNotifyCallback
	{
		void OnMediaEngineEvent(uint32 meEvent, uintptr_t param1, uint32 param2) = 0;
	};


	class MediaEngineNotify :
		public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::ClassicCom>, IMFMediaEngineNotify>
	{
	public:
		// MediaEngineNotify
		void SetCallback(MediaEngineNotifyCallback^ callback);

		// IMFMediaEngineNotify
		IFACEMETHOD(EventNotify)(DWORD evt, DWORD_PTR param1, DWORD param2);

	private:
		virtual ~MediaEngineNotify();

		MediaEngineNotifyCallback^ _callback;
	};
}

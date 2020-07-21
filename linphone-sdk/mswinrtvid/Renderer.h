/*
Renderer.h

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

#include <collection.h>
#include <ppltasks.h>

#include <d3d11_2.h>
#include <d2d1_2.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mferror.h>
#include <Mfmediaengine.h>

#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.media.h>
#include <windows.media.capture.h>
#include <windows.media.mediaproperties.h>

#include <wrl.h>
#include <wrl\client.h>
#include <wrl\implements.h>
#include <wrl\ftm.h>
#include <wrl\event.h> 
#include <wrl\wrappers\corewrappers.h>
#include <wrl\module.h>

#include "MediaEngineNotify.h"
#include "MediaStreamSource.h"
#include "RemoteHandle.h"
#include "SharedData.h"


namespace libmswinrtvid
{
	public ref class MSWinRTExtensionManager sealed
	{
	public:
		Platform::Boolean Setup();
		Platform::Boolean RegisterUrl(Platform::String^ url, Windows::Media::Core::MediaStreamSource^ source);
		Platform::Boolean UnregisterUrl(Platform::String^ url);

		static property MSWinRTExtensionManager^ Instance
		{
			MSWinRTExtensionManager^ get() { return _instance; }
		}

	private:
		MSWinRTExtensionManager();
		~MSWinRTExtensionManager();

		static MSWinRTExtensionManager^ _instance;
		Microsoft::WRL::ComPtr<ABI::Windows::Media::IMediaExtensionManager> mMediaExtensionManager;
		Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Collections::IMap<HSTRING, IInspectable*>> mExtensionManagerProperties;
	};

	public ref class MSWinRTRenderer sealed
		: public MediaEngineNotifyCallback
	{
	public:
		MSWinRTRenderer();
		virtual ~MSWinRTRenderer();

		bool Start();
		void Stop();
		void Feed(Windows::Storage::Streams::IBuffer^ pBuffer, int width, int height);
		virtual void OnMediaEngineEvent(uint32 meEvent, uintptr_t param1, uint32 param2);
		static bool D3D11Supported();

		property int FrameWidth
		{
			int get() { return mFrameWidth; }
		}

		property int FrameHeight
		{
			int get() { return mFrameHeight; }
		}

		property Platform::String^ SwapChainPanelName
		{
			Platform::String^ get() { return mSwapChainPanelName; }
			void set(Platform::String^ value) { mSwapChainPanelName = value; }
		}

	private:
		void Close();
		void SetSwapChainPanel();
		HRESULT SetupDirectX();
		HRESULT CreateDX11Device();
		void SendSwapChainHandle(HANDLE swapChain);
		void SendErrorEvent(HRESULT hr);

		int mFrameWidth;
		int mFrameHeight;
		int mSwapChainPanelWidth;
		int mSwapChainPanelHeight;
		MediaStreamSource^ mMediaStreamSource;
		UINT mResetToken;

		HANDLE mMemoryMapping;
		HANDLE mForegroundProcess;
		HANDLE mLock;
		HANDLE mShutdownEvent;
		HANDLE mShutdownCompleteEvent;
		HANDLE mEventAvailableEvent;
		HANDLE mCommandAvailableEvent;
		RemoteHandle mSwapChainHandle;
		MSWinRTVideo::SharedData* mSharedData;
		bool mUseHardware;
		Platform::String^ mUrl;
		Platform::String^ mSwapChainPanelName;

		Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
		Microsoft::WRL::ComPtr<IMFDXGIDeviceManager> mDxGIManager;
		Microsoft::WRL::ComPtr<IMFMediaEngine> mMediaEngine;
		Microsoft::WRL::ComPtr<IMFMediaEngineEx> mMediaEngineEx;
		Microsoft::WRL::ComPtr<MediaEngineNotify> mMediaEngineNotify;
	};
}

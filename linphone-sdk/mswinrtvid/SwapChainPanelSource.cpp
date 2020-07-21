/*
SwapChainPanelSource.cpp

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

#include <collection.h>
#include <ppltasks.h>
#include <Windows.ui.xaml.media.dxinterop.h>
#include <crtdbg.h>

#include "SwapChainPanelSource.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace MSWinRTVideo;


SwapChainPanelSource::SwapChainPanelSource()
	: mSwapChainPanel(nullptr), mMemoryMapping(INVALID_HANDLE_VALUE), mSharedData(nullptr)
{
}

SwapChainPanelSource::~SwapChainPanelSource()
{
	Stop();
}

void SwapChainPanelSource::Start(Windows::UI::Xaml::Controls::SwapChainPanel^ swapChainPanel)
{
	Stop();

	mSwapChainPanel = swapChainPanel;

	HRESULT hr;
	if ((FAILED(hr = reinterpret_cast<IUnknown*>(swapChainPanel)->QueryInterface(IID_PPV_ARGS(&mNativeSwapChainPanel)))) || (!mNativeSwapChainPanel))
	{
		throw ref new COMException(hr);
	}

	// Setup shared memory between foreground and background tasks
	mMemoryMapping = CreateFileMappingFromApp(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE | SEC_COMMIT, sizeof(*mSharedData), mSwapChainPanel->Name->Data());
	if (mMemoryMapping == nullptr)
	{
		DWORD error = GetLastError();
		throw ref new  Platform::COMException(HRESULT_FROM_WIN32(error));
	}
	mSharedData = (SharedData *)MapViewOfFileFromApp(mMemoryMapping, FILE_MAP_ALL_ACCESS, 0, sizeof(*mSharedData));
	mSharedData->foregroundProcessId = GetCurrentProcessId();
	mSharedData->foregroundLockMutex = CreateMutex(nullptr, FALSE, nullptr);
	mSharedData->foregroundShutdownEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	mSharedData->foregroundShutdownCompleteEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	mSharedData->foregroundEventAvailableEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	mSharedData->foregroundCommandAvailableEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	GetEvents();

	if ((swapChainPanel->ActualWidth > 0.0f) && (swapChainPanel->ActualHeight > 0.0f))
	{
		mSharedData->width = (int)swapChainPanel->ActualWidth;
		mSharedData->height = (int)swapChainPanel->ActualHeight;
		SetEvent(mSharedData->foregroundCommandAvailableEvent);
	}
	mSwapChainPanel->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &SwapChainPanelSource::OnSizeChanged);
}

void SwapChainPanelSource::Stop()
{
	if (mSharedData != nullptr)
	{
		if (mSharedData->foregroundShutdownEvent != nullptr)
		{
			SetEvent(mSharedData->foregroundShutdownEvent);
		}
		if (mSharedData->foregroundShutdownCompleteEvent != nullptr)
		{
			SetEvent(mSharedData->foregroundShutdownCompleteEvent);
		}
		if (mSharedData->foregroundLockMutex != nullptr)
		{
			CloseHandle(mSharedData->foregroundLockMutex);
		}
		if (mSharedData->foregroundShutdownEvent != nullptr)
		{
			CloseHandle(mSharedData->foregroundShutdownEvent);
		}
		if (mSharedData->foregroundShutdownCompleteEvent != nullptr)
		{
			CloseHandle(mSharedData->foregroundShutdownCompleteEvent);
		}
		if (mSharedData->foregroundEventAvailableEvent != nullptr)
		{
			CloseHandle(mSharedData->foregroundEventAvailableEvent);
		}
		if (mSharedData->foregroundCommandAvailableEvent != nullptr)
		{
			CloseHandle(mSharedData->foregroundCommandAvailableEvent);
		}
		UnmapViewOfFile(mSharedData);
		mSharedData = nullptr;
	}
	if (mMemoryMapping != INVALID_HANDLE_VALUE)
	{
		CloseHandle(mMemoryMapping);
		mMemoryMapping = INVALID_HANDLE_VALUE;
	}
	mSwapChainPanel = nullptr;
	if (mNativeSwapChainPanel != nullptr)
	{
		mNativeSwapChainPanel->SetSwapChainHandle(nullptr);
	}
	mNativeSwapChainPanel.Reset();
	if (mCurrentSwapChainHandle != nullptr)
	{
		CloseHandle(mCurrentSwapChainHandle);
		mCurrentSwapChainHandle = nullptr;
	}
}

IAsyncAction^ SwapChainPanelSource::GetEvents()
{
	return concurrency::create_async([this]()
	{
		HANDLE evt[3];
		evt[0] = mSharedData->foregroundShutdownEvent;
		evt[1] = mSharedData->foregroundEventAvailableEvent;
		bool running = true;
		HRESULT hr = S_OK;
		HANDLE lastSwapChainHandleUsed = nullptr;
		HANDLE backgroundProcess = nullptr;
		HANDLE foregroundSwapChainHandle;
		while (running)
		{
			DWORD wait = WaitForMultipleObjects(2, evt, FALSE, INFINITE);
			switch (wait)
			{
			case WAIT_OBJECT_0:
				running = false;
				break;
			case WAIT_OBJECT_0 + 1:
				WaitForSingleObject(mSharedData->foregroundLockMutex, INFINITE);
				if (FAILED(mSharedData->error))
				{
					hr = mSharedData->error;
					running = false;
				}
				else if (mSharedData->foregroundSwapChainHandle != lastSwapChainHandleUsed)
				{
					lastSwapChainHandleUsed = mSharedData->foregroundSwapChainHandle;
					if (!DuplicateHandle(GetCurrentProcess(), mSharedData->foregroundSwapChainHandle, GetCurrentProcess(), &foregroundSwapChainHandle, 0, TRUE, DUPLICATE_SAME_ACCESS))
					{
						hr = HRESULT_FROM_WIN32(GetLastError());
						running = false;
						ReleaseMutex(mSharedData->foregroundLockMutex);
						break;
					}
					mSwapChainPanel->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, foregroundSwapChainHandle]()
					{
						if (mNativeSwapChainPanel == nullptr) return;
						HRESULT hr = mNativeSwapChainPanel->SetSwapChainHandle(foregroundSwapChainHandle);
						if (FAILED(hr))
						{
						}
						if (mCurrentSwapChainHandle != nullptr)
						{
							
							CloseHandle(mCurrentSwapChainHandle);
						}
						mCurrentSwapChainHandle = foregroundSwapChainHandle;
					}));
				}
				ReleaseMutex(mSharedData->foregroundLockMutex);
				break;
			}
		}
		if (backgroundProcess != nullptr)
		{
			CloseHandle(backgroundProcess);
		}
		if (FAILED(hr))
		{
			throw ref new COMException(HRESULT_FROM_WIN32(hr));
		}
	});
}

void SwapChainPanelSource::OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	if (mSharedData == nullptr) return;
	WaitForSingleObject(mSharedData->foregroundLockMutex, INFINITE);
	mSharedData->width = (int)e->NewSize.Width;
	mSharedData->height = (int)e->NewSize.Height;
	SetEvent(mSharedData->foregroundCommandAvailableEvent);
	ReleaseMutex(mSharedData->foregroundLockMutex);
}

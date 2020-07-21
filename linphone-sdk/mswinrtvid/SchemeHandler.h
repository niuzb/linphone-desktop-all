/*
SchemeHandler.h

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

#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.media.h>
#include <wrl.h>
#include <mfmediaengine.h>


namespace MSWinRTVideo
{
	class DECLSPEC_UUID("0D41D269-A26E-4726-972B-5DFE45E2DA2E") SchemeHandler
		: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>, ABI::Windows::Media::IMediaExtension, IMFSchemeHandler>
	{
		InspectableClass(L"MSWinRTVideo.SchemeHandler", BaseTrust)
	public:
		SchemeHandler();
		~SchemeHandler();

		// IMediaExtension
		IFACEMETHOD(SetProperties)(ABI::Windows::Foundation::Collections::IPropertySet *pConfiguration);

		// IMFSchemeHandler
		IFACEMETHOD(BeginCreateObject)(
			_In_ LPCWSTR pwszURL,
			_In_ DWORD dwFlags,
			_In_ IPropertyStore *pProps,
			_COM_Outptr_opt_  IUnknown **ppIUnknownCancelCookie,
			_In_ IMFAsyncCallback *pCallback,
			_In_ IUnknown *punkState);

		IFACEMETHOD(EndCreateObject)(
			_In_ IMFAsyncResult *pResult,
			_Out_  MF_OBJECT_TYPE *pObjectType,
			_Out_  IUnknown **ppObject);

		IFACEMETHOD(CancelObjectCreation)(_In_ IUnknown *pIUnknownCancelCookie);
	};

	CoCreatableClass(SchemeHandler);

	ActivatableClass(SchemeHandler)

	Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Collections::IPropertySet> mExtensionManagerProperties;
}

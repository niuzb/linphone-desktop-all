/*
RemoteHandle.cpp

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

#include "RemoteHandle.h"


libmswinrtvid::RemoteHandle::RemoteHandle()
	: mLocalHandle(INVALID_HANDLE_VALUE), mRemoteHandle(INVALID_HANDLE_VALUE), mProcessId(0), mProcessHandle(INVALID_HANDLE_VALUE)
{
}


libmswinrtvid::RemoteHandle::~RemoteHandle()
{
	Close();
	if (mProcessHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(mProcessHandle);
		mProcessHandle = INVALID_HANDLE_VALUE;
	}
}

libmswinrtvid::RemoteHandle& libmswinrtvid::RemoteHandle::AssignHandle(HANDLE localHandle, DWORD processId)
{
	if (localHandle != mLocalHandle) {
		HANDLE remoteHandle;
		HANDLE processHandle = mProcessHandle;
		if (processId != mProcessId) {
			processHandle = OpenProcess(PROCESS_DUP_HANDLE, TRUE, processId);
			if ((processHandle == nullptr) || (processHandle == INVALID_HANDLE_VALUE)) {
				throw std::exception();
			}
		}
		if (!DuplicateHandle(GetCurrentProcess(), localHandle, processHandle, &remoteHandle, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
			throw std::exception();
		}
		Close();
		if (processId != mProcessId) {
			if (mProcessHandle != INVALID_HANDLE_VALUE) {
				CloseHandle(mProcessHandle);
			}
			mProcessHandle = processHandle;
			mProcessId = processId;
		}
		mLocalHandle = localHandle;
		mRemoteHandle = remoteHandle;
	}
	return *this;
}

libmswinrtvid::RemoteHandle& libmswinrtvid::RemoteHandle::Close()
{
	if (mLocalHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(mLocalHandle);
		mLocalHandle = INVALID_HANDLE_VALUE;
	}
	if (mRemoteHandle != INVALID_HANDLE_VALUE) {
		DuplicateHandle(mProcessHandle, mRemoteHandle, nullptr, nullptr, 0, TRUE, DUPLICATE_CLOSE_SOURCE);
		mRemoteHandle = INVALID_HANDLE_VALUE;
	}
	return *this;
}

HANDLE libmswinrtvid::RemoteHandle::GetLocalHandle() const
{
	return mLocalHandle;
}
HANDLE libmswinrtvid::RemoteHandle::GetRemoteHandle() const
{
	return mRemoteHandle;
}

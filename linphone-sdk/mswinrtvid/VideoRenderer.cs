/*
VideoRenderer.cs
Copyright (C) 2015  Belledonne Communications, Grenoble, France
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

using Microsoft.Phone.Media;
using System;
using System.Diagnostics;
using System.Windows;
using Windows.Phone.Media.Capture;

using Mediastreamer2.WP8Video;

namespace Mediastreamer2
{
    namespace WP8Video
    {
        public class VideoRenderer : IVideoRenderer
        {
            /// <summary>
            /// Public constructor.
            /// </summary>
            public VideoRenderer()
            {
            }

            /// <summary>
            /// Gets the video sample dispatcher.
            /// </summary>
            public IVideoDispatcher Dispatcher
            {
                get
                {
                    return this.streamSource;
                }
            }

            /// <summary>
            /// Gets an Uri from a stream id.
            /// </summary>
            /// <param name="streamId">The id of the stream to get an Uri for</param>
            /// <returns>The Uri corresponding to the stream id</returns>
            public static Uri StreamUri(Int32 streamId)
            {
                return new Uri("ms-media-stream-id:MediaStreamer-" + streamId);
            }

            /// <summary>
            /// Gets an Uri from a camera device name.
            /// </summary>
            /// <param name="device">The name of the camera device to get an Uri for</param>
            /// <returns>The Uri corresponding to the camera device</returns>
            public static Uri CameraUri(String device)
            {
                if (device.EndsWith(CameraSensorLocation.Front.ToString()))
                {
                    return VideoRenderer.FrontFacingCameraStreamUri;
                }
                else if (device.EndsWith(CameraSensorLocation.Back.ToString()))
                {
                    return VideoRenderer.RearFacingCameraStreamUri;
                }
                return new Uri("");
            }

            /// <summary>
            /// Tells whether a camera display is to be mirrored.
            /// </summary>
            /// <param name="device">The name of the camera device</param>
            /// <returns>true if the camera display should be mirrored, false otherwise</returns>
            public static Boolean IsCameraMirrored(String device)
            {
                if (device.EndsWith(CameraSensorLocation.Front.ToString()))
                {
                    return true;
                }
                else if (device.EndsWith(CameraSensorLocation.Back.ToString()))
                {
                    return false;
                }
                return false;
            }

            #region Implementation of the IVideoRenderer interface

            public int GetNativeWindowId()
            {
                return GetHashCode();
            }

            public void Start(String format, int width, int height)
            {
                Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    if (this.isRendering)
                    {
                        return;
                    }

                    try
                    {
                        if (this.mediastreamer == null)
                        {
                            this.mediastreamer = MediaStreamerFactory.CreateMediaStreamer(GetHashCode());
                        }
                        this.streamSource = new VideoStreamSource(format, width, height);
                        this.mediastreamer.SetSource(this.streamSource);
                        this.isRendering = true;
                    }
                    catch (Exception e)
                    {
                        Debug.WriteLine("[VideoRenderer] VideoRenderer.Start() failed: " + e.Message);
                    }
                });
            }

            public void Stop()
            {
                Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    if (!this.isRendering)
                    {
                        return;
                    }

                    this.streamSource.Shutdown();
                    this.streamSource.Dispose();
                    this.streamSource = null;
                    this.mediastreamer.Dispose();
                    this.mediastreamer = null;
                    this.isRendering = false;
                });
            }

            public void ChangeFormat(String format, int width, int height)
            {
                Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    if (this.streamSource != null)
                    {
                        this.streamSource.ChangeFormat(format, width, height);
                    }
                });
            }

            public void FirstFrameReceived()
            {
                Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    if (this.streamSource != null)
                    {
                        this.streamSource.FirstFrameReceived();
                    }
                });
            }

            #endregion

            public static Uri FrontFacingCameraStreamUri = new Uri("ms-media-stream-id:camera-FrontFacing");
            public static Uri RearFacingCameraStreamUri = new Uri("ms-media-stream-id:camera-RearFacing");

            private bool isRendering;
            private MediaStreamer mediastreamer;
            private VideoStreamSource streamSource;
        }
    }
}

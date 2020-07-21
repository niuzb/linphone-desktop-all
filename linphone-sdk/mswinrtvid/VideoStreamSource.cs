/*
VideoStreamSource.cs
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

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Windows.Media;
using System.Threading;

using Mediastreamer2.WP8Video;

namespace Mediastreamer2
{
    namespace WP8Video
    {
        public class VideoStreamSource : MediaStreamSource, IDisposable, IVideoDispatcher
        {
            public class Sample
            {
                public Sample(Windows.Storage.Streams.IBuffer pBuffer, UInt64 hnsPresentationTime)
                {
                    this.buffer = pBuffer;
                    this.presentationTime = hnsPresentationTime;
                }

                public Windows.Storage.Streams.IBuffer buffer;
                public UInt64 presentationTime;
            }


            public VideoStreamSource(String format, int frameWidth, int frameHeight)
            {
                this.format = format;
                this.frameWidth = frameWidth;
                this.frameHeight = frameHeight;
                this.sampleQueue = new Queue<Sample>(VideoStreamSource.maxSampleQueueSize);
                this.outstandingSamplesCount = 0;
                this.shutdownEvent = new ManualResetEvent(false);
            }

            #region Implementation of the IDisposable interface
            public void Dispose()
            {
                if (!this.isDisposed)
                {
                    this.isDisposed = true;
                }
                GC.SuppressFinalize(this);
            }
            #endregion

            #region Implementation of the MediaStreamSource interface
            protected override void OpenMediaAsync()
            {
                lock(lockObj)
                {
                    // Create attributes telling that we want an infinite video in which we can't seek
                    this.sourceAttributes = new Dictionary<MediaSourceAttributesKeys, string>();
                    this.sourceAttributes[MediaSourceAttributesKeys.Duration] = TimeSpan.FromSeconds(0).Ticks.ToString(CultureInfo.InvariantCulture);
                    this.sourceAttributes[MediaSourceAttributesKeys.CanSeek] = false.ToString();
                    this.openAsyncPending = true;
                }
            }

            protected override void GetSampleAsync(MediaStreamType mediaStreamType)
            {
                if (mediaStreamType == MediaStreamType.Video)
                {
                    lock (lockObj)
                    {
                        this.outstandingSamplesCount++;
                        FeedSamples();
                    }
                }
            }

            protected override void CloseMedia()
            {
            }

            protected override void GetDiagnosticAsync(MediaStreamSourceDiagnosticKind diagnosticKind)
            {
                throw new NotImplementedException();
            }

            protected override void SwitchMediaStreamAsync(MediaStreamDescription mediaStreamDescription)
            {
                throw new NotImplementedException();
            }

            protected override void SeekAsync(long seekToTime)
            {
                ReportSeekCompleted(seekToTime);
            }
            #endregion

            public void Shutdown()
            {
                this.shutdownEvent.Set();
                lock (lockObj)
                {
                    if (this.outstandingSamplesCount > 0)
                    {
                        // ReportGetSampleCompleted must be called after GetSampleAsync to avoid memory leak. So, send
                        // an empty MediaStreamSample here.
                        MediaStreamSample msSample = new MediaStreamSample(this.streamDesc, null, 0, 0, 0, this.emptyAttributes);
                        ReportGetSampleCompleted(msSample);
                        this.outstandingSamplesCount = 0;
                    }
                }
            }

            public void ChangeFormat(String format, int width, int height)
            {
                this.format = format;
                this.frameWidth = width;
                this.frameHeight = height;
            }

            public void FirstFrameReceived()
            {
                if (this.openAsyncPending)
                {
                    ReportOpenAsyncCompleted();
                }
            }

            public void OnSampleReceived(Windows.Storage.Streams.IBuffer pBuffer, UInt64 hnsPresentationTime)
            {
                lock (lockObj)
                {
                    if (this.sampleQueue.Count >= VideoStreamSource.maxSampleQueueSize)
                    {
                        // The sample queue is full, discard the older sample
                        this.sampleQueue.Dequeue();
                    }

                    this.sampleQueue.Enqueue(new Sample(pBuffer, hnsPresentationTime));
                    FeedSamples();
                }
            }

            private void ReportOpenAsyncCompleted()
            {
                CreateMediaDescription();
                List<MediaStreamDescription> streams = new List<MediaStreamDescription>();
                streams.Add(this.streamDesc);
                ReportOpenMediaCompleted(sourceAttributes, streams);
                this.openAsyncPending = false;
            }

            private void CreateMediaDescription()
            {
                Dictionary<MediaStreamAttributeKeys, string> streamAttributes = new Dictionary<MediaStreamAttributeKeys, string>();
                streamAttributes[MediaStreamAttributeKeys.VideoFourCC] = this.format;
                streamAttributes[MediaStreamAttributeKeys.Width] = this.frameWidth.ToString();
                streamAttributes[MediaStreamAttributeKeys.Height] = this.frameHeight.ToString();
                this.streamDesc = new MediaStreamDescription(MediaStreamType.Video, streamAttributes);
            }

            private void FeedSamples()
            {
                if (!(this.shutdownEvent.WaitOne(0)))
                {
                    while (this.sampleQueue.Count > 0 && this.outstandingSamplesCount > 0)
                    {
                        Sample sample = this.sampleQueue.Dequeue();
                        Stream s = System.Runtime.InteropServices.WindowsRuntime.WindowsRuntimeBufferExtensions.AsStream(sample.buffer);
                        MediaStreamSample msSample = new MediaStreamSample(this.streamDesc, s, 0, s.Length, (long)sample.presentationTime, this.emptyAttributes);
                        ReportGetSampleCompleted(msSample);
                        this.outstandingSamplesCount--;
                    }
                }
            }


            private const int maxSampleQueueSize = 4;

            private bool isDisposed = false;
            private bool openAsyncPending = false;
            private String format;
            private int frameWidth;
            private int frameHeight;
            private int outstandingSamplesCount;
            private object lockObj = new object();
            private Queue<Sample> sampleQueue;
            private MediaStreamDescription streamDesc;
            private Dictionary<MediaSourceAttributesKeys, string> sourceAttributes;
            private Dictionary<MediaSampleAttributeKeys, string> emptyAttributes = new Dictionary<MediaSampleAttributeKeys, string>();
            private ManualResetEvent shutdownEvent;
        }
    }
}

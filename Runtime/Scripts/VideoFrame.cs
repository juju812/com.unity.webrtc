using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Unity.WebRTC
{
    public struct VideoFrame
    {
        /// <summary>
        /// 轨道id
        /// </summary>
        public string trackId;

        /// <summary>
        /// 数据指针
        /// </summary>
        public IntPtr buffer;

        /// <summary>
        /// 宽
        /// </summary>
        public int width;

        /// <summary>
        /// 高
        /// </summary>
        public int height;

        /// <summary>
        /// 时间戳
        /// </summary>
        public uint timestamp;

        public VideoFrame(string trackId, IntPtr buffer, int width, int height, uint timestamp)
        {
            this.trackId = trackId;
            this.buffer = buffer;
            this.width = width;
            this.height = height;
            this.timestamp = timestamp;
        }
    }
}

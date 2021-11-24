#pragma once
#include <mutex>
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "third_party/libyuv/include/libyuv.h"


// return I420 framebuffer to unity
typedef void (UNITY_INTERFACE_API* DelegateOnFrameCallback) (int renderer_id, void* i420_buffer, int width, int height, uint32_t timestamp);

namespace unity {
namespace webrtc {

    namespace webrtc = ::webrtc;

    class UnityVideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame>
    {
    public:
        UnityVideoRenderer(uint32_t id);
        ~UnityVideoRenderer();
        void OnFrame(const webrtc::VideoFrame &frame) override;

        void SetDelegateOnFrameCallback(DelegateOnFrameCallback delegateOnFrameCallback);
        uint32_t GetId();
        rtc::scoped_refptr<webrtc::VideoFrameBuffer> GetFrameBuffer();
        void SetFrameBuffer(rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer);

        // used in UnityRenderingExtEventUpdateTexture 
        std::vector<uint8_t> tempBuffer;
        // called on RenderThread
        void ConvertVideoFrameToTextureAndWriteToBuffer(int width, int height, libyuv::FourCC format);

    private:
        unsigned char* ConvertI420BufferToCharArray(webrtc::I420BufferInterface* i420_buffer, unsigned char* char_array);
        void OnFrameCallback(rtc::scoped_refptr<webrtc::I420BufferInterface> i420_buffer, int width, int height);

        uint32_t m_id;
        std::mutex m_mutex;
        rtc::scoped_refptr<webrtc::VideoFrameBuffer> m_frameBuffer;
        uint32_t m_timestamp;
        std::vector<uint8_t> m_i420buffer;
        DelegateOnFrameCallback m_delegateOnFrameCallback = nullptr;
    };

} // end namespace webrtc
} // end namespace unity

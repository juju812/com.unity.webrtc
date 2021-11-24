#include "pch.h"
#include "UnityVideoRenderer.h"

namespace unity {
namespace webrtc {

UnityVideoRenderer::UnityVideoRenderer(uint32_t id) : m_id(id)
{
    DebugLog("Create UnityVideoRenderer Id:%d", id);
}

UnityVideoRenderer::~UnityVideoRenderer()
{
    DebugLog("Destory UnityVideoRenderer Id:%d", m_id);
    {
        std::unique_lock<std::mutex> lock(m_mutex);
    }
}

void UnityVideoRenderer::OnFrame(const webrtc::VideoFrame &frame)
{
    rtc::scoped_refptr<webrtc::VideoFrameBuffer> frame_buffer = frame.video_frame_buffer();

    if (frame_buffer->type() == webrtc::VideoFrameBuffer::Type::kNative)
    {
        frame_buffer = frame_buffer->ToI420();
    }
    
    SetFrameBuffer(frame_buffer);
    m_timestamp = frame.timestamp();
}

uint32_t UnityVideoRenderer::GetId()
{
    return m_id;
}

rtc::scoped_refptr<webrtc::VideoFrameBuffer> UnityVideoRenderer::GetFrameBuffer()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!lock.owns_lock())
    {
        return nullptr;
    }

    return m_frameBuffer;
}

void UnityVideoRenderer::SetDelegateOnFrameCallback(DelegateOnFrameCallback delegateOnFrameCallback)
{
    m_delegateOnFrameCallback = delegateOnFrameCallback;
}

void UnityVideoRenderer::SetFrameBuffer(rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!lock.owns_lock())
    {
        return;
    }

    m_frameBuffer = buffer;
}

void UnityVideoRenderer::OnFrameCallback(rtc::scoped_refptr<webrtc::I420BufferInterface> i420_buffer, int width, int height)
{
    if (m_delegateOnFrameCallback != nullptr)
    {
        size_t size = width * height * 3 / 2;

        if (m_i420buffer.size() != size)
        {
            m_i420buffer.resize(size);
        }

        if (0 > libyuv::ConvertFromI420(
            i420_buffer->DataY(), i420_buffer->StrideY(), i420_buffer->DataU(),
            i420_buffer->StrideU(), i420_buffer->DataV(), i420_buffer->StrideV(),
            m_i420buffer.data(), 0, width, height, libyuv::FOURCC_I420))
        {
            RTC_LOG(LS_INFO) << "failed convert videoFrame to i420Buffer";
        }

        m_delegateOnFrameCallback(m_id, m_i420buffer.data(), width, height, m_timestamp);
    }
}

void UnityVideoRenderer::ConvertVideoFrameToTextureAndWriteToBuffer(int width, int height, libyuv::FourCC format)
{
    auto frame = GetFrameBuffer();
    if (frame == nullptr)
    {
        return;
    }

    rtc::scoped_refptr<webrtc::I420BufferInterface> i420_buffer;
    if (width == frame->width() && height == frame->height())
    {
        i420_buffer = frame->ToI420();
    }
    else
    {
        auto temp = webrtc::I420Buffer::Create(width, height);
        temp->ScaleFrom(*frame->ToI420());
        i420_buffer = temp;
    }

    size_t size = width * height * 4;
    if (tempBuffer.size() != size)
        tempBuffer.resize(size);

    if(0 > libyuv::ConvertFromI420(
        i420_buffer->DataY(), i420_buffer->StrideY(), i420_buffer->DataU(),
        i420_buffer->StrideU(), i420_buffer->DataV(), i420_buffer->StrideV(),
        tempBuffer.data(), 0, width, height, format))
    {
        RTC_LOG(LS_INFO) << "failed libyuv::ConvertFromI420";
    }

    OnFrameCallback(i420_buffer, width, height);
}
} // end namespace webrtc
} // end namespace unity

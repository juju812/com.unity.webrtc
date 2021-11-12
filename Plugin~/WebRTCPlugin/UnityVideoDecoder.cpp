#include "pch.h"
#include "UnityVideoDecoder.h"
#include "Codec/DecoderFactory.h"

#include "DummyVideoEncoder.h"
#include <libyuv.h>

namespace unity
{
    namespace webrtc
    {
        std::unique_ptr<UnityVideoDecoder> UnityVideoDecoder::Create()
        {
            return std::make_unique<UnityVideoDecoder>();
        }

        int32_t UnityVideoDecoder::InitDecode(const webrtc::VideoCodec* codec_settings, int32_t number_of_cores)
        {
            m_width = codec_settings->width;
            m_height = codec_settings->height;
            if (m_width <= 0 || m_height <= 0) {
                return WEBRTC_VIDEO_CODEC_ERROR;
            }
            m_decoder = DecoderFactory::GetInstance().Init(codec_settings->codecType, codec_settings->width, codec_settings->height);
            
            return WEBRTC_VIDEO_CODEC_OK;
        }

        int32_t UnityVideoDecoder::Decode(const webrtc::EncodedImage& input_image, bool missing_frames, int64_t render_time_ms)
        {
            if (key_frame_required_) {
                if (input_image._frameType != webrtc::VideoFrameType::kVideoFrameKey) {
                    return WEBRTC_VIDEO_CODEC_ERROR;
                }
                key_frame_required_ = false;
            }

            uint8_t* data = const_cast<uint8_t*>(input_image.data());
            size_t size = static_cast<size_t>(input_image.size());
            
            m_decoder->Decode(data, size, &m_ppFrame, &m_nFrameReturned, 0, &m_pTimestamp, m_pts);

            if (m_ppFrame != nullptr && m_nFrameReturned > 0) {
                /*std::vector<uint8_t> data = {};
                for (int i = 0; i < m_nFrameReturned; i++) {
                    data.push_back(*(m_ppFrame[i]));
                }
                const rtc::scoped_refptr<FrameBuffer> buffer =
                    new rtc::RefCountedObject<FrameBuffer>(
                        m_width, m_height, data, m_decoder->Id());
                webrtc::VideoFrame::Builder builder = webrtc::VideoFrame::Builder()
                    .set_video_frame_buffer(buffer)
                    .set_timestamp_us(0)
                    .set_timestamp_rtp(input_image.Timestamp())
                    .set_ntp_time_ms(input_image.NtpTimeMs());
                webrtc::VideoFrame videoFrame = builder.build();
                m_callback->Decoded(videoFrame);
                */

                rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer =
                    buffer_pool_.CreateI420Buffer(m_width, m_height);
                libyuv::NV12ToI420(
                    m_ppFrame[0], m_width, //m_decoder->GetDeviceFramePitch(),
                    m_ppFrame[0] + m_height * m_width,// * nvdec->GetDeviceFramePitch(),
                    m_width, //nvdec->GetDeviceFramePitch(),
                    i420_buffer->MutableDataY(),
                    i420_buffer->StrideY(), i420_buffer->MutableDataU(),
                    i420_buffer->StrideU(), i420_buffer->MutableDataV(),
                    i420_buffer->StrideV(), m_width, m_height);
                webrtc::VideoFrame decoded_image = webrtc::VideoFrame::Builder()
                    .set_video_frame_buffer(i420_buffer)
                    .set_timestamp_rtp(input_image.Timestamp())
                    .build();
                m_callback->Decoded(decoded_image);
            }
            return WEBRTC_VIDEO_CODEC_OK;
        }

        int32_t UnityVideoDecoder::RegisterDecodeCompleteCallback(webrtc::DecodedImageCallback* callback)
        {
            this->m_callback = callback;
            return WEBRTC_VIDEO_CODEC_OK;
        }

        int32_t UnityVideoDecoder::Release()
        {
            this->m_callback = nullptr;
            m_ppFrame = nullptr;
            m_nFrameReturned = 0;
            m_pts = 0;
            m_pTimestamp = nullptr;
            m_dpFrame = 0;
            m_decoder.release();
            buffer_pool_.Release();
            return WEBRTC_VIDEO_CODEC_OK;
        }
    }
}

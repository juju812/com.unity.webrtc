#include "pch.h"
#include "UnityVideoDecoder.h"
#include "Codec/DecoderFactory.h"

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
            m_decoder = DecoderFactory::GetInstance().Init(codec_settings->codecType, codec_settings->width, codec_settings->height);
            return WEBRTC_VIDEO_CODEC_OK;
        }
        int32_t UnityVideoDecoder::Decode(const webrtc::EncodedImage& input_image, bool missing_frames, int64_t render_time_ms)
        {
            webrtc::VideoFrame videoFrame = m_decoder->Decode(input_image.data(), input_image.size());
            m_callback->Decoded(videoFrame);
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
            return WEBRTC_VIDEO_CODEC_OK;
        }
    }
}

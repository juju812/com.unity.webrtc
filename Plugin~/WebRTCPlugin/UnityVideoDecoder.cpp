#include "pch.h"
#include "UnityVideoDecoder.h"

namespace unity
{
    namespace webrtc
    {
        UnityVideoDecoder::UnityVideoDecoder() {}

        int32_t UnityVideoDecoder::InitDecode(const webrtc::VideoCodec* codec_settings, int32_t number_of_cores)
        {

            return WEBRTC_VIDEO_CODEC_OK;
        }
        int32_t UnityVideoDecoder::Decode(const webrtc::EncodedImage& input_image, bool missing_frames, int64_t render_time_ms)
        {
            return WEBRTC_VIDEO_CODEC_OK;
        }

        int32_t UnityVideoDecoder::RegisterDecodeCompleteCallback(webrtc::DecodedImageCallback* callback)
        {
            this->callback = callback;
            return WEBRTC_VIDEO_CODEC_OK;
        }

        int32_t UnityVideoDecoder::Release()
        {
            this->callback = nullptr;
            return WEBRTC_VIDEO_CODEC_OK;
        }
    }
}

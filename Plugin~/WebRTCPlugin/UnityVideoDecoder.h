#include"Codec/IDecoder.h"
#include "common_video/include/video_frame_buffer_pool.h"

namespace unity
{
    namespace webrtc
    {
        namespace webrtc = ::webrtc;

        class UnityVideoDecoder : public webrtc::VideoDecoder
        {
        public:
            static std::unique_ptr<UnityVideoDecoder> Create();

            virtual int32_t InitDecode(const webrtc::VideoCodec* codec_settings, int32_t number_of_cores) override;

            virtual int32_t Decode(const webrtc::EncodedImage& input_image, bool missing_frames, int64_t render_time_ms) override;

            virtual int32_t RegisterDecodeCompleteCallback(webrtc::DecodedImageCallback* callback) override;

            virtual int32_t Release() override;

        private:
            webrtc::DecodedImageCallback* m_callback;
            std::unique_ptr<IDecoder> m_decoder;
            bool key_frame_required_ = true;
            webrtc::VideoFrameBufferPool buffer_pool_ = webrtc::VideoFrameBufferPool(false, 300);

            uint8_t** m_ppFrame;
            int m_nFrameReturned = 0;
            int64_t m_pts, * m_pTimestamp;
            CUdeviceptr m_dpFrame = 0;

            int m_width, m_height;
        };

    }
}

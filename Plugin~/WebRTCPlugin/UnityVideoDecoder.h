namespace unity
{
    namespace webrtc
    {
        namespace webrtc = ::webrtc;

        class UnityVideoDecoder : public webrtc::VideoDecoder
        {
            virtual int32_t InitDecode(const webrtc::VideoCodec* codec_settings, int32_t number_of_cores) override;

            virtual int32_t Decode(const webrtc::EncodedImage& input_image, bool missing_frames, int64_t render_time_ms) override;

            virtual int32_t RegisterDecodeCompleteCallback(webrtc::DecodedImageCallback* callback) override;

            virtual int32_t Release() override;

        private:
            webrtc::DecodedImageCallback* callback
        };

    }
}

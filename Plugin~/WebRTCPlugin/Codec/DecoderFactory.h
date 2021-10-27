#pragma once
#include <memory>

namespace unity
{
namespace webrtc
{
    namespace webrtc = ::webrtc;

    class IDecoder;
    class DecoderFactory
    {
    public:
        static DecoderFactory& GetInstance();
        static bool GetHardwareEncoderSupport();
        std::unique_ptr<IDecoder> Init(webrtc::VideoCodecType codecType, int width, int height);
    private:
        DecoderFactory() = default;
        DecoderFactory(DecoderFactory const&) = delete;
        DecoderFactory& operator=(DecoderFactory const&) = delete;
    };
}
}

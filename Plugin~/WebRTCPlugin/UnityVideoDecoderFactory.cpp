#include "pch.h"
#include "UnityVideoDecoder.h"
#include "UnityVideoDecoderFactory.h"

#include "absl/strings/match.h"

#if UNITY_OSX || UNITY_IOS
#import "sdk/objc/components/video_codec/RTCDefaultVideoDecoderFactory.h"
#import "sdk/objc/native/api/video_decoder_factory.h"
#elif UNITY_ANDROID
#include "Codec/AndroidCodec/android_codec_factory_helper.h"
#endif

namespace unity
{
namespace webrtc
{
    webrtc::VideoDecoderFactory* CreateDecoderFactory()
    {
#if UNITY_OSX || UNITY_IOS
        return webrtc::ObjCToNativeVideoDecoderFactory(
            [[RTCDefaultVideoDecoderFactory alloc] init]).release();
#elif UNITY_ANDROID
        return CreateAndroidDecoderFactory().release();
#else
        return new webrtc::InternalDecoderFactory();
#endif
    }

    UnityVideoDecoderFactory::UnityVideoDecoderFactory()
    : internal_decoder_factory_(CreateDecoderFactory())
    {
    }

    std::vector<webrtc::SdpVideoFormat> UnityVideoDecoderFactory::GetSupportedFormats() const
    {
#if CUDA_PLATFORM
        return { webrtc::CreateH264Format(
            webrtc::H264::kProfileConstrainedBaseline,
            webrtc::H264::kLevel5_1, "1") };
#else
        return internal_decoder_factory_->GetSupportedFormats();
#endif
    }

    std::unique_ptr<webrtc::VideoDecoder> UnityVideoDecoderFactory::CreateVideoDecoder(const webrtc::SdpVideoFormat & format)
    {

        if (absl::EqualsIgnoreCase(format.name, cricket::kAv1CodecName))
        {
            RTC_LOG(LS_INFO) << "AV1 codec is not supported";
            return nullptr;
        }
#if CUDA_PLATFORM
        if (absl::EqualsIgnoreCase(format.name, cricket::kH264CodecName)) {
            return UnityVideoDecoder::Create() ;
        }
        else
        {
            return internal_decoder_factory_->CreateVideoDecoder(format);
        }
#else
        return internal_decoder_factory_->CreateVideoDecoder(format);
#endif
    }

}  // namespace webrtc
}  // namespace unity

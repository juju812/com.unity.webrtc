#include "pch.h"
#include <cuda.h>
#include "DecoderFactory.h"
#include "NvCodec/NvDecoder.h"

namespace unity
{
namespace webrtc
{
    namespace webrtc = ::webrtc;
    static const cudaVideoCodec GetCudaVideoCodec(webrtc::VideoCodecType codecType)
    {
        static std::map<webrtc::VideoCodecType, cudaVideoCodec> codecTypeTrans = {
            std::make_pair(webrtc::kVideoCodecVP8, cudaVideoCodec_VP8),
            std::make_pair(webrtc::kVideoCodecVP9, cudaVideoCodec_VP9),
            std::make_pair(webrtc::kVideoCodecH264, cudaVideoCodec_H264),
        };
        if (codecTypeTrans.count(codecType) > 0)
        {
            return codecTypeTrans[codecType];
        }
        return cudaVideoCodec_NumCodecs;
    }

    DecoderFactory& DecoderFactory::GetInstance()
    {
        static DecoderFactory factory;
        return factory;
    }

    bool DecoderFactory::GetHardwareEncoderSupport()
    {
        return true;
    }

    std::unique_ptr<IDecoder> DecoderFactory::Init(webrtc::VideoCodecType codecType, int width, int height)
    {
        CUdevice cuDevice = 0;
        int iGpu = 0;
        cuDeviceGet(&cuDevice, iGpu);
        CUcontext cuContext = NULL;
        cuCtxCreate(&cuContext, CU_CTX_SCHED_AUTO, cuDevice);
        cudaVideoCodec cudaVideoCodec = GetCudaVideoCodec(codecType);
        std::unique_ptr<IDecoder> decoder = std::make_unique<NvDecoder>(cuContext, cudaVideoCodec, width, height);
        return decoder;
    }
}
}

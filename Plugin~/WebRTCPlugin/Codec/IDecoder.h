#pragma once
#include <nvcuvid.h>

namespace unity
{
namespace webrtc
{

    namespace webrtc = ::webrtc;
    class IDecoder
    {
    public:
        virtual ~IDecoder() {};
        virtual void SetDecoderId(const uint32_t id) { m_decoderId = id; }
        virtual uint32_t Id() const { return m_decoderId; }
        virtual bool Decode(const uint8_t* pData, int nSize, uint8_t*** pppFrame,
                            int* pnFrameReturned, uint32_t flags = 0, int64_t** ppTimestamp = NULL,
                            int64_t timestamp = 0, CUstream stream = 0) = 0;
    private:
        uint32_t m_decoderId;
    };
}
}

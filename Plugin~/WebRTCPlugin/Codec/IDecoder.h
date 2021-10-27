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
        virtual webrtc::VideoFrame Decode(const uint8_t* pData, int nSize) = 0;
    private:
        uint32_t m_decoderId;
    };
}
}

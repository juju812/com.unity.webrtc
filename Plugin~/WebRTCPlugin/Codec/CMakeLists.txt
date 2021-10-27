target_sources(WebRTCLib
  PRIVATE
    DecoderFactory.cpp
    DecoderFactory.h
    EncoderFactory.cpp
    EncoderFactory.h
    IDecoder.h
    IEncoder.h
)

add_subdirectory(SoftwareCodec)

if(Windows OR Linux)
  add_subdirectory(NvCodec)
endif()

if(Android)
  add_subdirectory(AndroidCodec)
endif()
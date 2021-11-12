/*
* Copyright 2017-2018 NVIDIA Corporation.  All rights reserved.
*
* Please refer to the NVIDIA end user license agreement (EULA) associated
* with this source code for terms and conditions that govern your use of
* this software. Any use, reproduction, disclosure, or distribution of
* this software and related documentation outside the terms of the EULA
* is strictly prohibited.
*
*/

#pragma once

#include <assert.h>
#include <stdint.h>
#include <mutex>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <string.h>
#include "nvcuvid.h"
#include "Codec/IDecoder.h"


#define NVDEC_THROW_ERROR( errorStr, errorCode )                                                         \
    do                                                                                                   \
    {                                                                                                    \
        throw NVDECException::makeNVDECException(errorStr, errorCode, __FUNCTION__, __FILE__, __LINE__); \
    } while (0)


#define NVDEC_API_CALL( cuvidAPI )                                                                                 \
    do                                                                                                             \
    {                                                                                                              \
        CUresult errorCode = cuvidAPI;                                                                             \
        if( errorCode != CUDA_SUCCESS)                                                                             \
        {                                                                                                          \
            std::ostringstream errorLog;                                                                           \
            errorLog << #cuvidAPI << " returned error " << errorCode;                                              \
            throw NVDECException::makeNVDECException(errorLog.str(), errorCode, __FUNCTION__, __FILE__, __LINE__); \
        }                                                                                                          \
    } while (0)

namespace unity
{
namespace webrtc
{
    /**
    * @brief Exception class for error reporting from the decode API.
    */
    
    class NVDECException : public std::exception
    {
    public:
        NVDECException(const std::string& errorStr, const CUresult errorCode)
            : m_errorString(errorStr), m_errorCode(errorCode) {}

        virtual ~NVDECException() throw() {}
        virtual const char* what() const throw() { return m_errorString.c_str(); }
        CUresult  getErrorCode() const { return m_errorCode; }
        const std::string& getErrorString() const { return m_errorString; }
        static NVDECException makeNVDECException(const std::string& errorStr, const CUresult errorCode,
            const std::string& functionName, const std::string& fileName, int lineNo);
    private:
        std::string m_errorString;
        CUresult m_errorCode;
    };

    inline NVDECException NVDECException::makeNVDECException(const std::string& errorStr, const CUresult errorCode, const std::string& functionName,
        const std::string& fileName, int lineNo)
    {
        std::ostringstream errorLog;
        errorLog << functionName << " : " << errorStr << " at " << fileName << ":" << lineNo << std::endl;
        NVDECException exception(errorLog.str(), errorCode);
        return exception;
    }

    struct Rect {
        int l, t, r, b;
    };

    struct Dim {
        int w, h;
    };

    class NvDecoder : public IDecoder{

    public:
        NvDecoder(CUcontext cuContext, cudaVideoCodec eCodec, int maxWidth = 0, int maxHeight = 0);
        ~NvDecoder();
        CUcontext GetContext() { return m_cuContext; }
        int GetWidth() { assert(m_nWidth); return m_nWidth; }
        int GetHeight() { assert(m_nLumaHeight); return m_nLumaHeight; }
        int GetChromaHeight() { assert(m_nChromaHeight); return m_nChromaHeight; }
        int GetNumChromaPlanes() { assert(m_nNumChromaPlanes); return m_nNumChromaPlanes; }
        int GetFrameSize() { assert(m_nWidth); return m_nWidth * (m_nLumaHeight + m_nChromaHeight * m_nNumChromaPlanes) * m_nBPP; }
        int GetDeviceFramePitch() { assert(m_nWidth); return m_nDeviceFramePitch ? (int)m_nDeviceFramePitch : m_nWidth * m_nBPP; }
        int GetBitDepth() { assert(m_nWidth); return m_nBitDepthMinus8 + 8; }
        int GetBPP() { assert(m_nWidth); return m_nBPP; }
        cudaVideoSurfaceFormat GetOutputFormat() { return m_eOutputFormat; }
        CUVIDEOFORMAT GetVideoFormatInfo() { assert(m_nWidth); return m_videoFormat; }
        //std::string GetVideoInfo() const { return m_videoInfo.str(); }
        bool Decode(const uint8_t* pData, int nSize, uint8_t*** pppFrame, int* pnFrameReturned, uint32_t flags = 0, int64_t** ppTimestamp = NULL, int64_t timestamp = 0, CUstream stream = 0);
        //bool Decode(const uint8_t* pData, int nSize, uint8_t*** pppFrame, int* pnFrameReturned, uint32_t flags = 0, int64_t** ppTimestamp = NULL, int64_t timestamp = 0, CUstream stream = 0);
        //bool DecodeLockFrame(const uint8_t* pData, int nSize, uint8_t*** pppFrame, int* pnFrameReturned, uint32_t flags = 0, int64_t** ppTimestamp = NULL, int64_t timestamp = 0, CUstream stream = 0);
        //void UnlockFrame(uint8_t** ppFrame, int nFrame);
        int setReconfigParams(const Rect* pCropRect, const Dim* pResizeDim);

    private:
        static int CUDAAPI HandleVideoSequenceProc(void* pUserData, CUVIDEOFORMAT* pVideoFormat) { return ((NvDecoder*)pUserData)->HandleVideoSequence(pVideoFormat); }
        static int CUDAAPI HandlePictureDecodeProc(void* pUserData, CUVIDPICPARAMS* pPicParams) { return ((NvDecoder*)pUserData)->HandlePictureDecode(pPicParams); }
        static int CUDAAPI HandlePictureDisplayProc(void* pUserData, CUVIDPARSERDISPINFO* pDispInfo) { return ((NvDecoder*)pUserData)->HandlePictureDisplay(pDispInfo); }
        int HandleVideoSequence(CUVIDEOFORMAT* pVideoFormat);
        int HandlePictureDecode(CUVIDPICPARAMS* pPicParams);
        int HandlePictureDisplay(CUVIDPARSERDISPINFO* pDispInfo);
        int ReconfigureDecoder(CUVIDEOFORMAT* pVideoFormat);

    private:
        //cuda context
        CUcontext m_cuContext = NULL;
        CUvideoctxlock m_ctxLock;
        std::mutex* m_pMutex;
        CUvideoparser m_hParser = NULL;
        CUvideodecoder m_hDecoder = NULL;
        //是否使用显卡内存
        bool m_bUseDeviceFrame;
        // dimension of the output
        unsigned int m_nWidth = 0, m_nLumaHeight = 0, m_nChromaHeight = 0;
        unsigned int m_nNumChromaPlanes = 0;
        // height of the mapped surface 
        int m_nSurfaceHeight = 0;
        int m_nSurfaceWidth = 0;
        cudaVideoCodec m_eCodec = cudaVideoCodec_NumCodecs;
        cudaVideoChromaFormat m_eChromaFormat;
        cudaVideoSurfaceFormat m_eOutputFormat;
        int m_nBitDepthMinus8 = 0;
        int m_nBPP = 1;
        CUVIDEOFORMAT m_videoFormat = {};
        Rect m_displayRect = {};
        // stock of frames
        std::vector<uint8_t*> m_vpFrame;
        // decoded frames for return
        std::vector<uint8_t*> m_vpFrameRet;
        // timestamps of decoded frames
        std::vector<int64_t> m_vTimestamp;
        int m_nDecodedFrame = 0, m_nDecodedFrameReturned = 0;
        int m_nDecodePicCnt = 0, m_nPicNumInDecodeOrder[32];
        bool m_bEndDecodeDone = false;
        std::mutex m_mtxVPFrame;
        int m_nFrameAlloc = 0;
        CUstream m_cuvidStream = 0;
        bool m_bDeviceFramePitched = false;
        //the width in bytes of the allocation ?
        size_t m_nDeviceFramePitch = 0;
        Rect m_cropRect = {};
        Dim m_resizeDim = {};

        std::ostringstream m_videoInfo;
        unsigned int m_nMaxWidth = 0, m_nMaxHeight = 0;
        bool m_bReconfigExternal = false;
        bool m_bReconfigExtPPChange = false;
    };
}
}

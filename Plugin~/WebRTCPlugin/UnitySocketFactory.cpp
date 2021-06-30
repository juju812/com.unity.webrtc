// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license
// information.

#include "pch.h"

#include "UnitySocketFactory.h"

namespace unity {
    namespace webrtc {


rtc::AsyncPacketSocket* UnitySocketFactory::CreateUdpSocket(const rtc::SocketAddress& local_address, uint16_t min_port,
            uint16_t max_port)
{
    if (min_port < m_minPort || max_port > m_maxPort) {
        DebugLog("Create udp socket cancelled, port out of range, expect port range is: %d -> %d, parameter port range is: %d -> %d",
            m_minPort, m_maxPort, min_port, max_port);
        return nullptr;
    }
    return BasicPacketSocketFactory::CreateUdpSocket(local_address, min_port, max_port);
}

}
} 


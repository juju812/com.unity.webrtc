// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "p2p/base/basic_packet_socket_factory.h"

#include "WebRTCPlugin.h"


namespace unity {
    namespace webrtc {
        using namespace webrtc;

        class UnitySocketFactory : public rtc::BasicPacketSocketFactory {
        public:
            UnitySocketFactory(uint16_t min_port, uint16_t max_port)
                : BasicPacketSocketFactory(), m_minPort(min_port), m_maxPort(max_port) {};
            UnitySocketFactory(rtc::Thread* thread, uint16_t min_port, uint16_t max_port)
                : BasicPacketSocketFactory(thread), m_minPort(min_port), m_maxPort(max_port) {};
            UnitySocketFactory(rtc::SocketFactory* socket_factory, uint16_t min_port, uint16_t max_port)
                : BasicPacketSocketFactory(socket_factory), m_minPort(min_port), m_maxPort(max_port) {};
            rtc::AsyncPacketSocket* CreateUdpSocket(const rtc::SocketAddress& local_address, uint16_t min_port,
                uint16_t max_port) override;

        private:
            uint16_t m_minPort;
            uint16_t m_maxPort;
        };

    }
}

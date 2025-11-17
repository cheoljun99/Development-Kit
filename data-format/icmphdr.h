#pragma once

#include <cstdint>
#include "iphdr.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#elif defined(__linux__)
    #include <arpa/inet.h>
#endif

#pragma pack(push, 1)
struct IcmpHdr {
    uint8_t type_;
    uint8_t code_;
    uint16_t checksum_;
    uint16_t unused_;
    uint16_t nextHopMTU_;

    uint8_t type() const { return type_; }
    uint8_t code() const { return code_; }
    uint16_t checksum() const { return ntohs(checksum_); }
    uint16_t unused() const { return ntohs(unused_); }
    uint16_t nextHopMTU() const { return ntohs(nextHopMTU_); }

    static uint16_t calc_checksum(IpHdr*iphdr,IcmpHdr*icmph) {// ICMP 체크섬 계산
        uint32_t ret = 0;
        icmph->checksum_ = 0;
        int16_t icmp_len = iphdr->totalLen() - (iphdr->hdrLen()*4);
        uint8_t* pword = reinterpret_cast<uint8_t*>(icmph);
        for (int i = 0; i < icmp_len - 1; i += 2) {// 16비트 단위로 더함
            ret += (pword[i] << 8) + pword[i + 1];
        }
        if (icmp_len & 1) ret += pword[icmp_len - 1] << 8;// 길이가 홀수일 경우 마지막 바이트 처리
        while (ret >> 16)// 캐리 처리
            ret = (ret & 0xFFFF) + (ret >> 16);
        return htons(static_cast<uint16_t>(~ret));
    }

    enum Type : uint8_t {
        DestinationUnreachable = 3
    };
};
#pragma pack(pop)

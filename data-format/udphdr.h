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
struct UdpHdr final {
	uint16_t sport_;   // Source Port
	uint16_t dport_;   // Destination Port
	uint16_t len_;     // Length (UDP header + payload)
	uint16_t checksum_;     // Checksum

	uint16_t sport() const { return ntohs(sport_); }
	uint16_t dport() const { return ntohs(dport_); }
	uint16_t len()   const { return ntohs(len_); }
	uint16_t checksum()   const { return ntohs(checksum_); }
	static uint16_t calc_checksum(IpHdr* iphdr, UdpHdr* udphdr) {// UDP 체크섬 계산
        udphdr->checksum_ = 0;
        uint32_t ret = 0;
        int udp_len = udphdr->len();
        int udp_payload_len = udp_len - sizeof(UdpHdr);
        const int pseudohdr_len = 12;
        uint8_t buf[pseudohdr_len] = {0};
        // pseudo-header
        uint32_t sip = ntohl(iphdr->sip_);
        uint32_t dip = ntohl(iphdr->dip_);
        buf[0] = (sip >> 24) & 0xFF;
        buf[1] = (sip >> 16) & 0xFF;
        buf[2] = (sip >> 8) & 0xFF;
        buf[3] = sip & 0xFF;
        buf[4] = (dip >> 24) & 0xFF;
        buf[5] = (dip >> 16) & 0xFF;
        buf[6] = (dip >> 8) & 0xFF;
        buf[7] = dip & 0xFF;
        buf[8]  = 0;
        buf[9]  = iphdr->proto_;
        buf[10] = (udp_len >> 8) & 0xFF;
        buf[11] = udp_len & 0xFF;
        for (int i = 0; i < pseudohdr_len-1; i += 2) {
            ret += (buf[i] << 8) + buf[i + 1];
        }
        if (pseudohdr_len & 1) { // 홀수 길이 처리
            ret += buf[pseudohdr_len - 1] << 8;
        }
        uint8_t* udphdr_pword = reinterpret_cast<uint8_t*>(udphdr);
        for(int i =0; i<static_cast<int>(sizeof(UdpHdr))-1;i+=2){
            ret +=(udphdr_pword[i] << 8) + udphdr_pword[i + 1];
        }
        if (sizeof(UdpHdr)& 1) { // 홀수 길이 처리
            ret += udphdr_pword[sizeof(UdpHdr)- 1] << 8;
        }
        uint8_t* udp_paylod_pword = reinterpret_cast<uint8_t*>(udphdr) + sizeof(UdpHdr);
        for(int i =0; i<udp_payload_len-1;i+=2){
            ret +=(udp_paylod_pword[i] << 8) + udp_paylod_pword[i + 1];
        }
        if (udp_payload_len & 1) ret += udp_paylod_pword[udp_payload_len - 1] << 8;
        while (ret >> 16)
            ret = (ret & 0xFFFF) + (ret >> 16);
        return htons((~ret) ? static_cast<uint16_t>(~ret) : 0xFFFF);
    }
    static bool verify_checksum(IpHdr* iphdr, UdpHdr* udphdr) {
        uint32_t ret = 0;
        int udp_len = udphdr->len();
        int udp_payload_len = udp_len - sizeof(UdpHdr);
        const int pseudohdr_len = 12;
        uint8_t buf[pseudohdr_len] = {0};
        uint32_t sip = ntohl(iphdr->sip_);
        uint32_t dip = ntohl(iphdr->dip_);
        buf[0] = (sip >> 24) & 0xFF;
        buf[1] = (sip >> 16) & 0xFF;
        buf[2] = (sip >> 8) & 0xFF;
        buf[3] = sip & 0xFF;
        buf[4] = (dip >> 24) & 0xFF;
        buf[5] = (dip >> 16) & 0xFF;
        buf[6] = (dip >> 8) & 0xFF;
        buf[7] = dip & 0xFF;
        buf[8]  = 0;
        buf[9]  = iphdr->proto_;
        buf[10] = (udp_len >> 8) & 0xFF;
        buf[11] = udp_len & 0xFF;
        for (int i = 0; i < pseudohdr_len - 1; i += 2) {
            ret += (buf[i] << 8) + buf[i + 1];
        }
        if (pseudohdr_len & 1) ret += buf[pseudohdr_len - 1] << 8;
        uint8_t* udphdr_pword = reinterpret_cast<uint8_t*>(udphdr);
        for (int i = 0; i < static_cast<int>(sizeof(UdpHdr)) - 1; i += 2) {
            ret += (udphdr_pword[i] << 8) + udphdr_pword[i + 1];
        }
        if (sizeof(UdpHdr) & 1) ret += udphdr_pword[sizeof(UdpHdr) - 1] << 8;
        uint8_t* udp_payload_pword = reinterpret_cast<uint8_t*>(udphdr) + sizeof(UdpHdr);
        for (int i = 0; i < udp_payload_len - 1; i += 2) {
            ret += (udp_payload_pword[i] << 8) + udp_payload_pword[i + 1];
        }
        if (udp_payload_len & 1) ret += udp_payload_pword[udp_payload_len - 1] << 8;
        while (ret >> 16) {
            ret = (ret & 0xFFFF) + (ret >> 16);
        }
        return (ret == 0xFFFF);
    }

};
typedef UdpHdr* PUdpHdr;
#pragma pack(pop)
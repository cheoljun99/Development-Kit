#pragma once

#include <cstdint>
#include "ip.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#elif defined(__linux__)
    #include <arpa/inet.h>
#endif

#pragma pack(push, 1)
// IPv4 헤더 구조체
struct IpHdr final {
	uint8_t verIhl_;       // version(4) + header length(4)
	uint8_t tos_;       // type of service
	uint16_t totalLen_;      // total length
	uint16_t id_;       // identification
	uint16_t fragsOff_;      // fragment offset field
	uint8_t ttl_;       // time to live
	uint8_t proto_;         // protocol
	uint16_t checksum_;      // checksum
	Ip sip_;           // source IP
	Ip dip_;           // destination IP

	uint8_t ver() const { return verIhl_ >> 4; }
	uint8_t hdrLen() const { return verIhl_ & 0x0F; }
	uint8_t tos() const { return tos_; }
    uint16_t totalLen() const { return ntohs(totalLen_); }
    uint16_t id() const { return ntohs(id_); }
	uint8_t flags() const {return ntohs(fragsOff_) >> 13;}
	uint16_t fragOffset() const {return ntohs(fragsOff_) & 0x1FFF;}
	uint8_t ttl() const { return ttl_; }
    uint8_t proto() const { return proto_; }
    uint16_t checksum() const { return ntohs(checksum_); }
	Ip sip() const { return Ip(ntohl(sip_)); }
	Ip dip() const { return Ip(ntohl(dip_)); }
	static uint16_t calc_checksum( IpHdr* iphdr) {
        uint32_t ret = 0;
        iphdr->checksum_=0;
        int iph_len = iphdr->hdrLen()*4;
        uint16_t *pword = reinterpret_cast<uint16_t *>(iphdr);
        for (int i = 0; i < iph_len / 2; ++i) {
            ret += ntohs(*pword);
            pword++;
        }
        while (ret >> 16)
            ret = (ret & 0xFFFF) + (ret >> 16);
        return htons(static_cast<uint16_t>(~ret));
    }

	enum Flag : uint16_t {
		RF = 0x8000,
		DF = 0x4000,
		MF = 0x2000
	};
};
typedef IpHdr* PIpHdr;

// IPv6 주소 타입 (128bit)
struct Ipv6Addr {
	uint8_t addr[16];
};

// IPv6 헤더 구조체
struct Ipv6Hdr final {
	uint32_t ver_tc_fl_;   // version(4), traffic class(8), flow label(20)

	uint16_t payload_len_; // payload length
	uint8_t next_header_;  // next header (protocol)
	uint8_t hop_limit_;    // hop limit (like TTL)
	Ipv6Addr sip_;         // source IPv6 address
	Ipv6Addr dip_;         // destination IPv6 address

	uint8_t ver() const { return (ntohl(ver_tc_fl_) >> 28) & 0x0F; }
    uint8_t trafficClass() const { return (ntohl(ver_tc_fl_) >> 20) & 0xFF; }
    uint32_t flowLabel() const { return ntohl(ver_tc_fl_) & 0xFFFFF; }
    uint16_t payloadLen() const { return ntohs(payload_len_); }
    uint8_t nextHeader() const { return next_header_; }
    uint8_t hopLimit() const { return hop_limit_; }
    Ipv6Addr sip() const { return sip_; }
    Ipv6Addr dip() const { return dip_; }
};
typedef Ipv6Hdr* PIpv6Hdr;
#pragma pack(pop)
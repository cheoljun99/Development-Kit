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
struct TcpHdr final {
	uint16_t sport_;     // Source port
	uint16_t dport_;     // Destination port
	uint32_t seq_;       // Sequence number
	uint32_t ack_;       // Acknowledgment number
	uint8_t dataOffRsvd_;    // Data offset (4 bits) + Reserved (4 bits)
	uint8_t flags_;       // Flags (CWR, ECE, URG, ACK, PSH, RST, SYN, FIN)
	uint16_t win_;        // Window size
	uint16_t checksum_;        // Checksum
	uint16_t urp_;        // Urgent pointer
	// options follow if data offset > 5 (not handled here)

	enum Flag : uint8_t {
		Fin = 0x01,
		Syn = 0x02,
		Rst = 0x04,
		Psh = 0x08,
		Ack = 0x10,
		Urg = 0x20,
		Ece = 0x40,
		Cwr = 0x80
	};
	
	uint16_t sport() const { return ntohs(sport_); }
	uint16_t dport() const { return ntohs(dport_); }
	uint32_t seq() const { return ntohl(seq_); }
	uint32_t ack() const { return ntohl(ack_); }
	uint8_t dataOffset() const { return (dataOffRsvd_ >> 4); }
	uint8_t reserved() const { return dataOffRsvd_ & 0x0F; }
	uint8_t flags() const { return flags_; }
	uint16_t win() const { return ntohs(win_); }
    uint16_t checksum() const { return ntohs(checksum_); }
    uint16_t urp() const { return ntohs(urp_); }
	static uint16_t calc_checksum(IpHdr* iphdr, TcpHdr* tcphdr) {// TCP 체크섬 계산
        tcphdr->checksum_ = 0;
        uint32_t ret = 0;
        int tcphdr_len = tcphdr->dataOffset() * 4;
        uint16_t tcp_paylod_len = iphdr->totalLen()-(iphdr->hdrLen()*4)-(tcphdr->dataOffset()*4);
        int tcp_len = tcphdr_len + tcp_paylod_len;
        const int pseudohdr_len = 12;
        uint8_t buf[pseudohdr_len] = {0};
        // pseudo-header
        uint32_t sip = iphdr->sip();
        uint32_t dip = iphdr->dip();
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
        buf[10] = (tcp_len >> 8) & 0xFF;
        buf[11] = tcp_len & 0xFF;
        for (int i = 0; i < pseudohdr_len-1; i += 2) {
            ret += (buf[i] << 8) + buf[i + 1];
        }
        if (pseudohdr_len & 1) { // 홀수 길이 처리
            ret += buf[pseudohdr_len - 1] << 8;
        }
        uint8_t* tcphdr_pword = reinterpret_cast<uint8_t*>(tcphdr);
        for(int i =0; i<tcphdr_len-1;i+=2){
            ret +=(tcphdr_pword[i] << 8) + tcphdr_pword[i + 1];
        }
        if (tcphdr_len & 1) { // 홀수 길이 처리
            ret += tcphdr_pword[tcphdr_len - 1] << 8;
        }
        uint8_t* tcp_payload_pword = reinterpret_cast<uint8_t*>(tcphdr) + tcphdr_len;
        for(int i =0; i<tcp_paylod_len-1;i+=2){
            ret +=(tcp_payload_pword[i] << 8) + tcp_payload_pword[i + 1];
        }
        if (tcp_paylod_len & 1) ret += tcp_payload_pword[tcp_paylod_len - 1] << 8;
        while (ret >> 16)
            ret = (ret & 0xFFFF) + (ret >> 16);
        return htons((~ret) ? static_cast<uint16_t>(~ret) : 0xFFFF);
    }
};
typedef TcpHdr* PTcpHdr;
#pragma pack(pop)

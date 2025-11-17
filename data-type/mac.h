#pragma once

#include <cstdint>
#include <cstring>
#include <string>

// ----------------------------------------------------------------------------
// Mac
// ----------------------------------------------------------------------------
struct Mac final {
	static constexpr int SIZE = 6;

	// constructor
	Mac() {}
	Mac(const Mac& r) { memcpy(this->mac_, r.mac_, SIZE); }
	Mac(const uint8_t* r) { memcpy(this->mac_, r, SIZE); }
	Mac(const std::string& r) {
		std::string s;
		for(char ch: r) {
			if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
				s += ch;
		}
		int res = sscanf(s.c_str(), "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx", &mac_[0], &mac_[1], &mac_[2], &mac_[3], &mac_[4], &mac_[5]);
		if (res != SIZE) {
			fprintf(stderr, "Mac::Mac sscanf return %d r=%s\n", res, r.c_str());
			return;
		}
	}

	// assign operator
	Mac& operator = (const Mac& r) { memcpy(this->mac_, r.mac_, SIZE); return *this; }

	// casting operator
	explicit operator uint8_t*() const { return const_cast<uint8_t*>(mac_); }
	explicit operator std::string() const {
		char buf[20]; // enough size
		sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac_[0], mac_[1], mac_[2], mac_[3], mac_[4], mac_[5]);
		return std::string(buf);
	}

	// comparison operator
	bool operator == (const Mac& r) const { return memcmp(mac_, r.mac_, SIZE) == 0; }
	bool operator != (const Mac& r) const { return memcmp(mac_, r.mac_, SIZE) != 0; }
	bool operator < (const Mac& r) const { return memcmp(mac_, r.mac_, SIZE) < 0; }
	bool operator > (const Mac& r) const { return memcmp(mac_, r.mac_, SIZE) > 0; }
	bool operator <= (const Mac& r) const { return memcmp(mac_, r.mac_, SIZE) <= 0; }
	bool operator >= (const Mac& r) const { return memcmp(mac_, r.mac_, SIZE) >= 0; }
	bool operator == (const uint8_t* r) const { return memcmp(mac_, r, SIZE) == 0; }

	void clear() {
		*this = nullMac();
	}

	bool isNull() const {
		return *this == nullMac();
	}

	bool isBroadcast() const { // FF:FF:FF:FF:FF:FF
		return *this == broadcastMac();
	}

	bool isMulticast() const { // 01:00:5E:0*
		return mac_[0] == 0x01 && mac_[1] == 0x00 && mac_[2] == 0x5E && (mac_[3] & 0x80) == 0x00;
	}

	static Mac randomMac() {
		Mac res;
		for (int i = 0; i < SIZE; i++)
			res.mac_[i] = uint8_t(rand() % 256);
		res.mac_[0] &= 0x7F;
		return res;
	}
	static Mac& nullMac() {
		static uint8_t _value[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		static Mac res(_value);
		return res;
	}
	static Mac& broadcastMac() {
		static uint8_t _value[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		static Mac res(_value);
		return res;
	}

public:
	uint8_t mac_[SIZE];
};

namespace std {
	template<>
	struct hash<Mac> {
		size_t operator() (const Mac& r) const {
			uint64_t h = 0;
            // MAC 주소 6바이트를 상위 2바이트는 0으로 두고, 하위 6바이트에 그대로 복사
            for (int i = 0; i < Mac::SIZE; i++) {
                h = (h << 8) | r.mac_[i];
            }
            return static_cast<size_t>(h); // size_t는 64비트 환경에서 그대로 사용 가능
		}
	};
}

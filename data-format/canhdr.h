#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct CanHdr final {
    uint32_t idFlag_; // CAN ID + EFF/RTR/ERR flags
    uint8_t  dlc_;    // DLC (0~8)
    uint8_t  res0_;   // reserved
    uint8_t  res1_;   // reserved
    uint8_t  res2_;   // reserved

    // CAN ID 추출 (11bit Standard or 29bit Extended)
    uint32_t can_id() const { return idFlag_ & 0x1FFFFFFF; }  // 29bit ID까지 지원
    bool is_extended() const { return idFlag_ & 0x80000000; } // EFF
    bool is_rtr() const { return idFlag_ & 0x40000000; }      // RTR
    bool is_error() const { return idFlag_ & 0x20000000; }    // ERR
    uint8_t get_dlc() const { return dlc_; }
};
typedef CanHdr* PCanHdr;
#pragma pack(pop)

#pragma pack(push, 1)
struct CanFdHdr final {
    uint32_t idFlag_; // CAN ID + EFF/RTR/ERR flags
    uint8_t  flags_; // CAN FD flags (BRS/EDL/ESI 등)
    uint8_t  dlc_; // DLC
    uint8_t  res0_; // reserved
    uint8_t  res1_; // reserved

    uint32_t can_id() const { return idFlag_ & 0x1FFFFFFF; }        // 29bit CAN ID
    bool is_extended() const { return idFlag_ & 0x80000000; }  // EFF
    bool is_rtr() const { return idFlag_ & 0x40000000; }        // RTR
    bool is_error() const { return idFlag_ & 0x20000000; }      // ERR
    uint8_t dlc() const { return dlc_; }
    uint8_t flags() const { return flags_; }
};   
typedef CanFdHdr* PCanFdHdr;
#pragma pack(pop)
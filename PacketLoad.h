#ifndef PRFC_PACKET_LOAD_H
#define PRFC_PACKET_LOAD_H

#include <cstdint>
#include <string>

// Six-dimensional packet key used by both RFC lookup implementations.
struct PacketLine {
    std::uint16_t first_src_ip;
    std::uint16_t last_src_ip;
    std::uint16_t first_dst_ip;
    std::uint16_t last_dst_ip;
    std::uint16_t src_port;
    std::uint16_t dst_port;

    // [src IP high, src IP low, dst IP high, dst IP low, src port, dst port]
    std::uint16_t dim[6];
};

// Parse one tab-separated packet trace record and add the modeled parse cost.
PacketLine Parse_PacketLine(std::string& fields, std::uint64_t& cycles);

#endif  // PRFC_PACKET_LOAD_H

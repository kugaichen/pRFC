#ifndef PRFC_FILTER_READER_H
#define PRFC_FILTER_READER_H

#include <cstdint>
#include <string>
#include <vector>

// ClassBench rule expanded into the six dimensions consumed by RFC.
struct FilterRule {
    std::string src_ip_addr;
    std::uint16_t first_src_ip;
    std::uint16_t last_src_ip;
    std::uint16_t src_ip_mask;
    std::string dst_ip_addr;
    std::uint16_t first_dst_ip;
    std::uint16_t last_dst_ip;
    std::uint16_t dst_ip_mask;
    std::uint16_t src_port_start;
    std::uint16_t src_port_end;
    std::uint16_t dst_port_start;
    std::uint16_t dst_port_end;
    std::uint16_t protocol;
    std::uint16_t protocol_mask;
    std::uint16_t flags;
    std::uint16_t flags_mask;
    std::uint32_t priority;

    // Inclusive [start, end] range for each packet dimension.
    std::uint16_t first_src_ip_start;
    std::uint16_t first_src_ip_end;
    std::uint16_t last_src_ip_start;
    std::uint16_t last_src_ip_end;
    std::uint16_t first_dst_ip_start;
    std::uint16_t first_dst_ip_end;
    std::uint16_t last_dst_ip_start;
    std::uint16_t last_dst_ip_end;
    std::uint16_t dim[6][2];
};

// Read valid ClassBench rules. Invalid records are reported and skipped.
std::vector<FilterRule> readFilterRule(const std::string& filepath);

#endif  // PRFC_FILTER_READER_H

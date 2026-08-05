#ifndef HARDWARESIMULATOR_H
#define HARDWARESIMULATOR_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <chrono>
#include <iomanip>  // Add for std::fixed and std::setprecision

// Define the hardware FSM clock cycle latency
constexpr std::uint64_t IDLE_DELAY = 1;                 // Latency for IDLE state
constexpr std::uint64_t LOAD_DATA_DELAY = 1;            // Latency for data loading
constexpr std::uint64_t PARSE_DELAY = 1;                // Latency for parsing
constexpr std::uint64_t LOOKUP_DELAY = 1;               // Latency for lookup operation
//improved
constexpr std::uint64_t SE_LOOKUP_DELAY = 1;            // Latency for SE_lookup operation
constexpr std::uint64_t SE_MATCH_DELAY = 1;             // Latency for SE_match operation

constexpr std::uint64_t WRITE_BACK_DELAY = 2;           // Latency for write-back operation
constexpr std::uint64_t NO_OPERATEION_DELAY = 1;        // Latency for no operation

// Define the hardware process packet data latency 
constexpr std::uint64_t PKT_FIELD_SPILT_DELAY = 1;      // Latency for splitting packet fields
constexpr std::uint64_t PKT_IP_PARSE_DELAY = 2;         // Latency for parsing IP address       
constexpr std::uint64_t PKT_PORT_PARSE_DELAY = 1;       // Latency for parsing port number

// Define the operation latency
constexpr std::uint64_t CYCLE_ADD = 1;                  // Latency for an addition operation 
constexpr std::uint64_t CYCLE_MUL = 3;                  // Latency for a multiplication operation
constexpr std::uint64_t CYCLE_MEMORY_ACCESS = 2;        // easy->1 ; complicated -> 2-3   // Latency for a memory access operation //2    ~//ddr 40-60
constexpr std::uint64_t CYCLE_BRANCH = 1;               // Latency for a branch operation


// Define the BRAM size
// detail information see wechat


// // FPGA芯片规格（Xilinx Virtex-7 690T）
// constexpr int VIRTEX7_BRAM_18KB = 240;      // 总BRAM块数（18KB/块）
// constexpr int BRAM_BLOCK_SIZE = 18432;       // 18*1024 bits
// constexpr int VIRTEX7_DSP48E = 160;          // 总DSP单元
// constexpr int VIRTEX7_FF = 128000;           // 总触发器数量
// constexpr int VIRTEX7_LUT = 64000;           // 总LUT数量

// // 添加BRAM计算参数（使用18KB BRAM块）
// constexpr int BITS_PER_PHASE0_ENTRY = 16;    // uint16_t
// constexpr int BITS_PER_PHASE1_ENTRY = 16;    // uint16_t
// constexpr int BITS_PER_FINAL_ENTRY = 32;     // int

// // 添加资源估算结构体
// struct FPGA_Resources {
//     int used_bram;
//     int used_dsp;
//     int used_ff;
//     int used_lut;
    
//     void print_report() const {
//         std::cout << "\nFPGA Resource Utilization (Virtex-7 690T):\n"
//                   << "-----------------------------------------\n"
//                   << "BRAM Blocks: " << used_bram << "/" << VIRTEX7_BRAM_18KB 
//                   << " (" << std::fixed << std::setprecision(2) 
//                   << (used_bram*100.0/VIRTEX7_BRAM_18KB) << "%)\n"
//                   << "DSP48E:       " << used_dsp << "/" << VIRTEX7_DSP48E
//                   << " (" << (used_dsp*100.0/VIRTEX7_DSP48E) << "%)\n"
//                   << "FlipFlops:    " << used_ff << "/" << VIRTEX7_FF
//                   << " (" << (used_ff*100.0/VIRTEX7_FF) << "%)\n"
//                   << "LUTs:        " << used_lut << "/" << VIRTEX7_LUT
//                   << " (" << (used_lut*100.0/VIRTEX7_LUT) << "%)\n";
//     }
// };

#endif
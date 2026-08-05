#include "PacketLoad.h"
#include "HardwareSimulator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <bitset>
#include <stdexcept>

// Parse a 32-bit integer to two 16-bit unsigned short 
void PKTintaddr_parse(const std::string& ipaddr_int32content, std::uint16_t& first16, std::uint16_t& last16, std::uint64_t& cycles){
    // converting to obtain two binary content
    unsigned long int_content = std::stoul(ipaddr_int32content);    //convert the 32-bit string to 32-bit integer 
    std::string binary_content = std::bitset<32>(int_content).to_string();
    std::string H_binary_content = binary_content.substr(0,16);
    std::string L_binary_content = binary_content.substr(16,16);
    cycles += PKT_IP_PARSE_DELAY;   //simulate binary conversion cost

    // converting to obtain two 16-bit unsigned short 
    std::bitset<16> H_bits(H_binary_content);
    std::bitset<16> L_bits(L_binary_content);
    unsigned long  H_value = H_bits.to_ulong();
    unsigned long  L_value = L_bits.to_ulong();
    first16 = static_cast<std::uint16_t>(H_value);
    last16 = static_cast<std::uint16_t>(L_value);  

}


// Function to parese a packet line of packet file 
PacketLine Parse_PacketLine(std::string& line, std::uint64_t& cycles){
    PacketLine packet{};
    std::vector<std::string> fields;

    // spilt the line into fields
    size_t pos = 0;
        while ((pos = line.find('\t')) != std::string::npos){
            fields.push_back(line.substr(0,pos));
            line.erase(0,pos+1);
        }
    fields.push_back(line);

    if (fields.size() < 4) {
        throw std::invalid_argument("packet record must contain at least four fields");
    }

    cycles += PKT_FIELD_SPILT_DELAY ;   // simulate field splitting cost Latency

    // Extract source ip address
    PKTintaddr_parse(fields[0], packet.first_src_ip, packet.last_src_ip, cycles);
    packet.dim[0] = packet.first_src_ip;
    packet.dim[1] = packet.last_src_ip;

    // Extract destination ip address
    PKTintaddr_parse(fields[1], packet.first_dst_ip, packet.last_dst_ip, cycles);
    packet.dim[2] = packet.first_dst_ip;
    packet.dim[3] = packet.last_dst_ip;

    // Extract source port 
    packet.src_port = static_cast<std::uint16_t>(std::stoi(fields[2]));
    packet.dim[4] = packet.src_port;

    // Extract destination port
    packet.dst_port = static_cast<std::uint16_t>(std::stoi(fields[3]));
    packet.dim[5] = packet.dst_port;


    return packet;
}

// // Function to load filter rules from a file 
/*
this function is loading filter rules to the vector->packets,where store the parsed_packets
the core procedure is "Parse_PacketLine", which parse the packets line by line.
*/

// std::vector<PacketLine> loadPacketLine(const std::string& filepath, std::uint64_t& cycle_count){
//     std::vector<PacketLine> packets;
//     std::ifstream file(filepath);
//     std::string line;

//     if (!file.is_open()){
//         std::cerr << "Error: Could not open file" << filepath << std::endl;
//         return packets;
//     }

//     std::uint64_t cycles = 0;

//     while (std::getline(file,line)){
//         cycles += 10;  //simulate line reading cost 
//         if (line.empty()) continue;

//         // spilt the line into fields
//         std::vector<std::string> fields;
//         size_t pos = 0;
//         while ((pos = line.find('\t')) != std::string::npos){
//             fields.push_back(line.substr(0,pos));
//             line.erase(0,pos+1);
//         }

//         cycles += 20;   //simulate field splitting cost
        
//         packets.push_back(Parse_PacketLine(fields,cycles));
//         // for(const auto& item : fields){
//             // std::cout << item << " ";
//         // }
//         // std::cout<< "" << std::endl;

//     } 

//     // Output results
//     std::cout << "Parsed " << packets.size() << " rules." << std::endl;
//     std::cout << "Estimated hardware cycles: " << cycles << std::endl;

//     cycle_count = cycle_count + cycles;
//     return packets;
// }

#include "FilterReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Convert an IP address string to a uint32_t
void IPintoUint16(const std::string& ip, std::uint16_t& first16, std::uint16_t& last16){
    std::uint32_t result = 0;
    std::istringstream iss(ip);
    std::string segment;    //store each segment of the IP address

    for (size_t i =0; i < 4; ++i){
        // Extract the segment between dots ('.')
        std::getline(iss, segment, '.');

        // Shift the current result left by 8 bits -> 32-bit integer
        result = (result << 8 ) | std::stoi(segment);
    }

    first16 = static_cast<std::uint16_t>((result >> 16) & 0xFFFF);   // Shift right by 16 bits and mask
    last16 = static_cast<std::uint16_t>(result & 0xFFFF); // Mask the lower 16 bits
}


void Decode_address_range(const std::string& ip, std::uint16_t& according_mask, 
    std::uint16_t& first_according_dim_start, std::uint16_t& first_according_dim_end,
    std::uint16_t& last_according_dim_start, std::uint16_t& last_according_dim_end){

    int address_parts[4] = {0};
    std::istringstream iss(ip);
    std::string segment;    //store each segment of the IP address

    for (size_t i =0; i < 4; ++i){
        // Extract the segment between dots ('.')
        std::getline(iss, segment, '.');

        // Shift the current result left by 8 bits -> 32-bit integer
        address_parts[i] = std::stoi(segment);
    }

    int mask = 32 - according_mask;
    int mask_8 = mask/8;
    int mask_less8 = mask % 8;

    unsigned short start_addr_range[4] = {0};
    for (int i=0; i<4; ++i){
        start_addr_range[i] = (i < 4 - mask_8) ? address_parts[i] : 0;
    }

    if (mask_less8 != 0 ){
        int mask_index = (1 << mask_less8) - 1;
        int mask_start = ~mask_index;
        start_addr_range[4- mask_8 - 1] &= mask_start;
    }

    unsigned short end_addr_range[4] = {0};
    for (int i=0; i<4; i++){
        end_addr_range[i] = (i < 4 - mask_8) ? address_parts[i] : 255;
    }

    if (mask_less8 != 0){
        int mask_index = (1 << mask_less8) - 1;
        end_addr_range[4 - mask_8 - 1] |= mask_index;
    }

    first_according_dim_start = (start_addr_range[0] << 8 | start_addr_range[1]);
    last_according_dim_start = (start_addr_range[2] << 8 | start_addr_range[3]);
    first_according_dim_end = (end_addr_range[0] << 8 | end_addr_range[1]);
    last_according_dim_end = (end_addr_range[2] << 8 | end_addr_range[3]); 

}


// Function to parse a Rule line of filter rule
FilterRule Parse_FilterRule(const std::vector<std::string>& fields, std::uint64_t priority){
    FilterRule rule;
    rule.priority = priority;

    // Extract source IP and mask
    size_t slash_pos = fields[0].find('/');
    rule.src_ip_addr = fields[0].substr(0, slash_pos);
    IPintoUint16(rule.src_ip_addr, rule.first_src_ip, rule.last_src_ip);
    std::string mask_str = fields[0].substr(slash_pos+1);
    rule.src_ip_mask = std::atoi(mask_str.c_str());

    // Extract destination IP and mask
    slash_pos = fields[1].find('/');
    rule.dst_ip_addr = fields[1].substr(0, slash_pos);
    IPintoUint16(rule.dst_ip_addr, rule.first_dst_ip, rule.last_dst_ip);
    mask_str = fields[1].substr(slash_pos+1);
    rule.dst_ip_mask = std::atoi(mask_str.c_str());

    // Extract source port range
    size_t colon_pos = fields[2].find(':');
    rule.src_port_start = std::stoul(fields[2].substr(0,colon_pos));
    rule.src_port_end = std::stoul(fields[2].substr(colon_pos+1));

    // Extract destination port range
    colon_pos = fields[3].find(':');
    rule.dst_port_start = std::stoul(fields[3].substr(0,colon_pos));
    rule.dst_port_end = std::stoul(fields[3].substr(colon_pos+1));


    // Extract protocol and protocol mask
    slash_pos = fields[4].find('/');
    rule.protocol = std::stoul(fields[4].substr(0, slash_pos), nullptr, 16);
    rule.protocol_mask = std::stoul(fields[4].substr(slash_pos + 1), nullptr, 16);


    // Extract flags and flags mask
    slash_pos = fields[5].find('/');
    rule.flags = std::stoul(fields[5].substr(0, slash_pos), nullptr, 16);
    rule.flags_mask = std::stoul(fields[5].substr(slash_pos + 1), nullptr, 16);
 

    // decode Ip address from start to end
    Decode_address_range(rule.src_ip_addr,rule.src_ip_mask,rule.dim[0][0],rule.dim[0][1],rule.dim[1][0],rule.dim[1][1]);
    Decode_address_range(rule.dst_ip_addr,rule.dst_ip_mask,rule.dim[2][0],rule.dim[2][1],rule.dim[3][0],rule.dim[3][1]);
    rule.dim[4][0] = rule.src_port_start;
    rule.dim[4][1] = rule.src_port_end;
    rule.dim[5][0] = rule.dst_port_start;
    rule.dim[5][1] = rule.dst_port_end;


    return rule;
}


// Function to read filter rules from a file 
std::vector<FilterRule> readFilterRule(const std::string& filepath){
    std::vector<FilterRule> rules;
    std::ifstream file(filepath);
    std::string line;

    if (!file.is_open()){
        std::cerr << "Error: Could not open file" << filepath << std::endl;
        return rules;
    }


    std::uint64_t priority = 0; // Priority based on line number

    while (std::getline(file, line)){

        // Skip empty lines and lines without '@'
        if (line.empty() || line[0] != '@') continue;
        line = line.substr(1);


        // Split the line into fields
        std::vector<std::string> fields;
        size_t pos = 0;
        while ((pos = line.find('\t')) != std::string::npos){
            fields.push_back(line.substr(0,pos));
            line.erase(0,pos+1);
        }
        fields.push_back(line);


        if (fields.size() < 6){
            std::cerr << "Error: Invalid line format: " << line << std::endl;
            continue;
        }


        // Parse the rule
        rules.push_back(Parse_FilterRule(fields, priority));
        priority ++;
    }

    std::cout << "Parsed " << rules.size() << " rules." << std::endl;
    return rules;

}

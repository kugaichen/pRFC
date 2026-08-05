#ifndef HARDWARE_STATE_MACHINE_H
#define HARDWARE_STATE_MACHINE_H

#include "PacketLoad.h"
#include "Hardware_RFC_Structure.h"
#include <vector>
#include <cstdint>
#include <thread>
#include <mutex>
#include <iostream>

// #define MAX_PARSED_PKT 1129

extern std::uint64_t cycle_count;           // simulate the clock 
extern std::uint64_t storage_overhead;      // simulate the storage overhead
extern std::uint64_t lookup_cycle_count;    // simulate the lookup clock

enum class FSMstate {
    IDLE,               // not performing any operation
    LOAD_DATA,          // loading data from an externel source
    PARSE,              // parsign the loading data 
    LOOKUP,             // writing results back
    WRITE_BACK,         // complete all operations 
    DONE
};

// Function to simulate the state machine for RFC
void Simulation_State_Machine(RFC_Chunks& rfc_classifier, std::string& line,int* parsed_packets, int& output_index);


// Function to simulate the hardware behavior by processing a file line by line 
/*
simulation the hardware behavior, input one line and then parse and lookup the line, then line by line
*/
void Simulation_Hardware_Behavior(RFC_Chunks& rfc_classifier, const std::string& filepath);

// Function to lookup packets
int Lookup(RFC_Chunks& rfc_classifer, PacketLine& packet);


#endif
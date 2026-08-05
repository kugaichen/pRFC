#ifndef HARDWARE_IMPROVED_STATEMACHINE
#define HARDWARE_IMPROVED_STATEMACHINE

#include "PacketLoad.h"
#include "Hardware_RFC_Structure.h"
#include "Hardware_ImprovedClassifierBuilding.h"
#include <vector>
#include <cstdint>
#include <thread>
#include <mutex>
#include <iostream>

extern std::uint64_t se_cycle_count;            // simulate the clock 
extern std::uint64_t se_storage_overhead;       // simulate the storage overhead


enum class SE_FSMstate {
    IDLE,
    LOAD_DATA,
    PARSE,
    SE_LOOKUP,
    SE_MATCH,
    WRITE_BACK,
    DONE
};

// Function to simulate the state machine for RFC_SE
void SE_Simulation_State_Machine(RFC_SE_Classifier& rfc_se_classifier, std::string& line,int* parsed_packets, int& output_index);


// Function to simulate the hardware behavior by processing a file line by line 
/*
simulation the hardware behavior, input one line and then parse and lookup the line, then line by line
*/
void SE_Simulation_Hardware_Behavior(RFC_SE_Classifier& rfc_se_classifier, const std::string& filepath);

// Function to lookup packets
int SE_Lookup(RFC_SE_Chunks& rfc_se_classifer, PacketLine& packet,std::uint64_t& se_lookup_cycle_count);

int Overall_Lookup(RFC_SE_Classifier& rfc_se_classifier, PacketLine& packet,std::uint64_t& se_lookup_cycle_count);


#endif

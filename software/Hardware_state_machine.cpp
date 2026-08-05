#include "Hardware_state_machine.h"
#include "PacketLoad.h"
#include "HardwareSimulator.h"

#include <fstream>

std::uint64_t lookup_cycle_count = 0;

std::uint64_t cycle_count = 0;
// mutex to protect cycle_count
std::mutex cycle_mutex;
// add cycles to the global cycle_count
void Add_Cycle(uint64_t cycles) {
    std::lock_guard<std::mutex> lock_cycle(cycle_mutex);
    cycle_count += cycles;
}


// Function to simulate the state machine for RFC
void Simulation_State_Machine(RFC_Chunks& rfc_classifier, std::string& line,int* parsed_packets, int& output_index){
    // size_t data_index = 0;
    // output_data.resize(input_data.size(), 0);

    // Initialize the state machine to the IDLE state
    FSMstate current_state = FSMstate::IDLE;
    PacketLine packet;
    int lookup_result;

    // Loop untile the state machine reaches the DONE state
    while(current_state != FSMstate::DONE){
        switch (current_state)
        {
        case FSMstate::IDLE:
            if (output_index < 3){
                std::cout << "State: IDLE" << std::endl;
            }
            // std::cout << "State: IDLE" << std::endl;
            Add_Cycle(IDLE_DELAY);   // Hardware Idle Latency
            current_state = FSMstate::LOAD_DATA;
            break;

        case FSMstate::LOAD_DATA:
            if (output_index < 3){
                std::cout << "State: LOAD_DATA" << std::endl;
            }
            Add_Cycle(LOAD_DATA_DELAY);   // Simulate the access latency of dual-port RAM Latency
            current_state = FSMstate::PARSE;
            break;

        case FSMstate::PARSE:
            if (output_index < 3){
                std::cout << "State: PARSE"<< " -> " ;
            }
            // If the line is not empty, parse it into a PacketLine 
            if (!line.empty()){
                // Parse the line into a PacketLine
                packet = Parse_PacketLine(line,cycle_count);     // Prase the line

                Add_Cycle(PARSE_DELAY);  // Simulate parsing Latency
                if (output_index < 3){
                    std::cout <<"Packet " << output_index << " : \"fitst src ip: " << packet.first_src_ip <<
                    ", last src ip: " << packet.last_src_ip <<
                    ", first destination ip: "<< packet.first_dst_ip <<
                    ", last destination ip: " << packet.last_dst_ip <<
                    ", Source Port: " << packet.src_port <<
                    ", Destination Port: " << packet.dst_port << "\"" 
                    << std::endl;
                }
                current_state = FSMstate::LOOKUP;

            }
            else{
                Add_Cycle(NO_OPERATEION_DELAY);   // Simulate non-operation Latency
                current_state = FSMstate::DONE;
            }     
            break;

        case FSMstate::LOOKUP:
            if (output_index < 3){
                std::cout << "State: LOOKUP" << std::endl;
            }
            // Lookup
            lookup_result = Lookup(rfc_classifier, packet);     // core function  -> lookup packet for the right rule 

            Add_Cycle(LOOKUP_DELAY);   // Simulate lookup Latency
      
            current_state = FSMstate::WRITE_BACK;
            break;

        case FSMstate::WRITE_BACK:
            if (output_index < 3){
                std::cout << "State: WRITE_BACK" << std::endl;
            }

            // Write_back
            *(parsed_packets+output_index -1) = lookup_result;     // core function  -> write back lookupresult for the right rule

            Add_Cycle(WRITE_BACK_DELAY);   // Simulate write back Latency
            current_state = FSMstate::DONE;
            if (output_index < 3){
                std::cout << "State: DONE" << std::endl;
            }
            break;
        // case FSMstate::DONE:
        //     if (output_index < 5){
        //         std::cout << "State: DONE" << std::endl;
        //     }

        //     output_index += 1;
        //     break;

        default:
            std::cerr << "Unknown state!" << std::endl;
            current_state = FSMstate::DONE;
            break;
        }
    }

    if (output_index < 3){
        std::cout << "State machine finished." << std::endl;
        std::cout << " " << std::endl;
    }
}


// Function to simulate the hardware behavior by processing a file line by line 
/*
simulation the hardware behavior, input one line and then parse and lookup the line, then line by line
*/
void Simulation_Hardware_Behavior(RFC_Chunks& rfc_classifier , const std::string& filepath){

    std::ifstream file(filepath);
    std::string line;
    std::vector<int> parsed_packets(MAX_PARSED_PKT, 0);
    int sequence = 1;

    // Check if the file was opened successfully
    if (!file.is_open()){
        std::cerr<<"Error: Could not open file" << filepath << std::endl;
        return;
    }

    // Process each line in the file
    while(std::getline(file,line)){
        if (sequence < 3){
            std::cout << "Processsing line " << sequence << std::endl;
        }

        // Simulate the state machine for the current line
        Simulation_State_Machine(rfc_classifier,line,parsed_packets.data(),sequence);     // using sequence to control the output informaiton 
        sequence += 1;
    }

    // Print the total number of packets parsed
    // int k = 1;
    // for (const auto& item : parsed_packets){
    //     if (k == 13) {
    //         k = 1;
    //         std::cout<<' '<< std::endl;
    //     }
    //     k++;
    //     std::cout << item << " ";
    // }
    // std::cout << "" << std::endl;

    // Save the parser packets lookup result 
    std::ofstream outFile("./lookupresult.txt", std::ios::binary);
    int flag = 0;
    for (const auto& item : parsed_packets){
        if (1){
            flag++;
            outFile << item << " ";
            if (flag == 29){
                flag = 0;
                outFile<< " " << std::endl;
            }
        }
    }



    std::cout << "\nParsed " << sequence -1  << " packets." << std::endl;
    // std::cout << "Estimated hardware storage (BRAM): " << storage_overhead / 1024.0 << " KB" << std::endl;
    
    std::cout << "Total Lookup clock cycles: "<< lookup_cycle_count << " cycles" << std::endl;
    std::cout << "Average Lookup clock cycles: "<< lookup_cycle_count / (sequence - 1) << " cycles" << std::endl;
    std::cout << "Total clock cycles: " << cycle_count << " cycles" <<std::endl;


}

// Function to lookup packets
int Lookup(RFC_Chunks& rfc_classifer, PacketLine& packet){
    int Lookup_result;
    std::uint16_t value_phase0[6];
    std::uint32_t key_phase1[2];
    std::uint32_t value_phase1[2];
    std::uint32_t key_final;

    for(size_t c0=0; c0<6; c0++){
        value_phase0[c0] = rfc_classifer.phase0_chunks[c0].item[packet.dim[c0]];
        lookup_cycle_count += CYCLE_MEMORY_ACCESS;
    }

    key_phase1[0] = value_phase0[0] * rfc_classifer.phase0_chunks[1].matched_EqIDTable->item_num * 
        rfc_classifer.phase0_chunks[5].matched_EqIDTable->item_num + 
        value_phase0[1] * rfc_classifer.phase0_chunks[5].matched_EqIDTable->item_num +
        value_phase0[5];

    lookup_cycle_count += (CYCLE_MEMORY_ACCESS * 2 + CYCLE_ADD * 2 + CYCLE_MUL * 2);
    
    key_phase1[1] = value_phase0[2] * rfc_classifer.phase0_chunks[3].matched_EqIDTable->item_num * 
        rfc_classifer.phase0_chunks[4].matched_EqIDTable->item_num +
        value_phase0[3] * rfc_classifer.phase0_chunks[4].matched_EqIDTable->item_num + 
        value_phase0[4];
    
    lookup_cycle_count += (CYCLE_MEMORY_ACCESS * 2 + CYCLE_ADD * 2 + CYCLE_MUL * 2);


    value_phase1[0] = rfc_classifer.phase1_chunks[0].item[key_phase1[0]];
    value_phase1[1] = rfc_classifer.phase1_chunks[1].item[key_phase1[1]];

    lookup_cycle_count += (CYCLE_MEMORY_ACCESS * 2);

    key_final = value_phase1[0] * rfc_classifer.phase1_chunks[1].matched_EqIDTable->item_num + value_phase1[1];
    lookup_cycle_count += (CYCLE_MEMORY_ACCESS + CYCLE_ADD + CYCLE_MUL);

    Lookup_result = rfc_classifer.final_chunks.item[key_final];
    lookup_cycle_count += CYCLE_MEMORY_ACCESS;

    return Lookup_result;
}

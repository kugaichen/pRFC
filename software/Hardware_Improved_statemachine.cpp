#include "Hardware_Improved_statemachine.h"
#include "PacketLoad.h"
#include "HardwareSimulator.h"

#include <fstream>

std::uint64_t se_lookup_cycle_count = 0;

std::uint64_t se_cycle_count = 0;
// mutex to protect cycle_count
// std::mutex se_cycle_mutex;
// add cycles to the global cycle_count
void SE_Add_Cycle(uint64_t cycles) {
    // std::lock_guard<std::mutex> lock_cycle(se_cycle_mutex);
    se_cycle_count += cycles;
}


// Function to simulate the state machine for RFC_SE
void SE_Simulation_State_Machine(RFC_SE_Classifier& rfc_se_classifier, std::string& line,int* parsed_packets, int& output_index){
    // Initialize teh state machine to the IDLE state
    SE_FSMstate current_state = SE_FSMstate::IDLE;
    PacketLine packet;
    int lookupresult;

    //Loop until the state machine reaches the DONE state
    while(current_state != SE_FSMstate::DONE){
        switch (current_state)
        {
            case SE_FSMstate::IDLE:
                if (output_index < 3){
                    std::cout << "State: IDLE" << std::endl;
                }
                SE_Add_Cycle(IDLE_DELAY);   // Hardware Idle Latency
                current_state = SE_FSMstate::LOAD_DATA;
                break;
        

            case SE_FSMstate::LOAD_DATA:
                if (output_index < 3){
                    std::cout << "State: LOAD_DATA" << std::endl;
                }
                SE_Add_Cycle(LOAD_DATA_DELAY);   // Simulate the access latency of dual-port RAM Latency
                current_state = SE_FSMstate::PARSE;
                break;

            case SE_FSMstate::PARSE:
                if (output_index < 3){
                std::cout << "State: PARSE"<< " -> " ;
                }
                // If the line is not empty, parse it into a PacketLine 
                if (!line.empty()){
                    // Parse the line into a PacketLine
                    packet = Parse_PacketLine(line,se_cycle_count);     // Prase the line

                    SE_Add_Cycle(PARSE_DELAY);  // Simulate parsing Latency
                    if (output_index < 3){
                        std::cout <<"Packet " << output_index << " : \"fitst src ip: " << packet.first_src_ip <<
                        ", last src ip: " << packet.last_src_ip <<
                        ", first destination ip: "<< packet.first_dst_ip <<
                        ", last destination ip: " << packet.last_dst_ip <<
                        ", Source Port: " << packet.src_port <<
                        ", Destination Port: " << packet.dst_port << "\"" 
                        << std::endl;
                    }
                    current_state = SE_FSMstate::SE_LOOKUP;

                }
                else{
                    SE_Add_Cycle(NO_OPERATEION_DELAY);   // Simulate non-operation Latency
                    current_state = SE_FSMstate::DONE;
                }     
                break;

            case SE_FSMstate::SE_LOOKUP:
                if (output_index < 3){
                std::cout << "State: SE_LOOKUP" << std::endl;
                }
                
                lookupresult = Overall_Lookup(rfc_se_classifier,packet,se_lookup_cycle_count);

                SE_Add_Cycle(SE_LOOKUP_DELAY);   // Simulate lookup Latency
      
                current_state = SE_FSMstate::SE_MATCH;
                break;
            
            case SE_FSMstate::SE_MATCH:
                if (output_index < 3){
                std::cout << "State: SE_MATCH" << std::endl;
                }
                // Match ok
                
                SE_Add_Cycle(SE_MATCH_DELAY);   // Simulate lookup Latency

                current_state = SE_FSMstate::WRITE_BACK;
                break;

            case SE_FSMstate::WRITE_BACK:
                if (output_index < 3){
                std::cout << "State: WRITE_BACK" << std::endl;
                }

                // Write_back
                *(parsed_packets+output_index -1) = lookupresult;     // core function  -> write back lookupresult for the right rule

                SE_Add_Cycle(WRITE_BACK_DELAY);   // Simulate write back Latency
                current_state = SE_FSMstate::DONE;
                if (output_index < 3){
                    std::cout << "State: DONE" << std::endl;
                }
                break;

            default:
                std::cerr << "Unknown state!" << std::endl;
                current_state = SE_FSMstate::DONE;
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
void SE_Simulation_Hardware_Behavior(RFC_SE_Classifier& rfc_se_classifier , const std::string& filepath){

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
        SE_Simulation_State_Machine(rfc_se_classifier,line,parsed_packets.data(),sequence);     // using sequence to control the output informaiton 
        sequence += 1;
    }
    //////////////////////////

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
    // std::ofstream outFile("./SE_lookupresult.txt", std::ios::binary);
    // int flag = 0;
    // for (const auto& item : parsed_packets){
    //     if (1){
    //         flag++;
    //         outFile << item << " ";
    //         if (flag == 29){
    //             flag = 0;
    //             outFile<< " " << std::endl;
    //         }
    //     }
    // }
    // auto resources = Calculate_FPGA_Resources(rfc_se_classifier);
    // resources.print_report();

    std::cout << "Classifier processing overhead: ";
    std::cout << "\nParsed " << sequence -1  << " packets." << std::endl;
    // std::cout << "Estimated hardware storage (BRAM): " << storage_overhead / 1024.0 << " KB" << std::endl;
    std::cout << "Total clock cycles: " << se_cycle_count << " cycles" <<std::endl;
    std::cout << "se_Lookup clock cycles: "<< se_lookup_cycle_count << " cycles" << std::endl;
    std::cout << "average se_Lookup clock cycyles: " << se_lookup_cycle_count / (sequence - 1) << " cycles" << std::endl;
    std::cout << "the lookup time in NetFPGA is: " << se_lookup_cycle_count * 6.0 / 1000000 << "ms" << std::endl;
    std::cout << "the average lookup time in NetFPGA is: " << se_lookup_cycle_count / (sequence - 1) * 6.0 / 1000000 << "ms" << std::endl;

}


int Overall_Lookup(RFC_SE_Classifier& rfc_se_classifier, PacketLine& packet,std::uint64_t& se_lookup_cycle_count){
    
    std::vector<int> lookupresult;
    int result = 65536;
    std::uint64_t se_lookup_cycle_count_L_L = 0;
    std::uint64_t se_lookup_cycle_count_L_H = 0;
    std::uint64_t se_lookup_cycle_count_H_L = 0;
    std::uint64_t se_lookup_cycle_count_H_H = 0;
    // Lookup
    lookupresult.push_back(SE_Lookup(rfc_se_classifier.rfc_classifier_L_L,packet,se_lookup_cycle_count_L_L));
    lookupresult.push_back(SE_Lookup(rfc_se_classifier.rfc_classifier_L_H,packet,se_lookup_cycle_count_L_H));
    lookupresult.push_back(SE_Lookup(rfc_se_classifier.rfc_classifier_H_L,packet,se_lookup_cycle_count_H_L));
    lookupresult.push_back(SE_Lookup(rfc_se_classifier.rfc_classifier_H_H,packet,se_lookup_cycle_count_H_H));

    std::uint64_t se_lookup_cycle = std::max({
        se_lookup_cycle_count_L_L,
        se_lookup_cycle_count_L_H,
        se_lookup_cycle_count_H_L,
        se_lookup_cycle_count_H_H
    });

    se_lookup_cycle_count = se_lookup_cycle_count + se_lookup_cycle;

    SE_Add_Cycle(se_lookup_cycle);

    for(const auto item:lookupresult){
        if(item < result && item != -1){
            result = item;
        }
    }

    if (result == 65536){
        return -1;
    }

    return result;
}


int SE_Lookup(RFC_SE_Chunks& rfc_se_chunks, PacketLine& packet,std::uint64_t& se_lookup_cycle_count){
    int lookup_result;
    std::uint16_t value_phase0[6];
    std::uint32_t key_phase1[2];
    std::uint32_t value_phase1[2];
    std::uint32_t key_final;

    se_lookup_cycle_count += CYCLE_ADD;
    se_lookup_cycle_count += CYCLE_MEMORY_ACCESS*2;
    for (size_t c0=0; c0<6; c0++){
        std::uint16_t key = packet.dim[c0];
        std::uint16_t real_key = key >> rfc_se_chunks.phase0_chunks[c0].factor;
        value_phase0[c0] = rfc_se_chunks.phase0_chunks[c0].item[real_key];
    }

    key_phase1[0] = value_phase0[0] * rfc_se_chunks.phase0_chunks[1].matched_EqIDTable->item_num *
    rfc_se_chunks.phase0_chunks[5].matched_EqIDTable->item_num + 
    value_phase0[1] * rfc_se_chunks.phase0_chunks[5].matched_EqIDTable->item_num +
    value_phase0[5];

    key_phase1[1] = value_phase0[2] * rfc_se_chunks.phase0_chunks[3].matched_EqIDTable->item_num *
    rfc_se_chunks.phase0_chunks[4].matched_EqIDTable->item_num + 
    value_phase0[3] * rfc_se_chunks.phase0_chunks[4].matched_EqIDTable->item_num +
    value_phase0[4];

    se_lookup_cycle_count += (CYCLE_MEMORY_ACCESS + CYCLE_ADD + CYCLE_MUL );

    value_phase1[0] = rfc_se_chunks.phase1_chunks[0].item[key_phase1[0]];
    value_phase1[1] = rfc_se_chunks.phase1_chunks[1].item[key_phase1[1]];
    

    key_final = value_phase1[0] * rfc_se_chunks.phase1_chunks[1].matched_EqIDTable->item_num + value_phase1[1];
    se_lookup_cycle_count += (CYCLE_MEMORY_ACCESS + CYCLE_ADD + CYCLE_MUL);

    lookup_result = rfc_se_chunks.final_chunks.item[key_final];

    return lookup_result;
    
}

#include "Hardware_RFC_ClassifierBuilding.h"
#include <iostream>
#include <chrono>

// the number of cycles for building the rfc classifier
std::uint64_t rfc_build_cycles = 0;
// the storage overhead of the RFC classifier
std::uint64_t rfc_storage_overhead = 0;
// the storage overhead of EqIDTables
std::uint64_t EqIDTable_storage_overhead = 0;

std::uint64_t EqID_GetStorageOverhead(const EqID* eqid){
    std::uint64_t overhead = 0;
    overhead = SIZE * sizeof(std::uint32_t) + sizeof(std::uint16_t) + sizeof(EqID*);
    return overhead;
}
 
// Function to calculate the storage overhead of an EqIDTable
std::uint64_t EqIDTable_GetStorageOverhead(const EqIDTable* eqidTable){
    std::uint64_t overhead = 0;
    overhead = sizeof(std::uint32_t) + sizeof(EqID*) * 2;
    EqID* current = eqidTable->head;
    while(current != nullptr){
        overhead += EqID_GetStorageOverhead(current);
        current = current->next;
    }

    return overhead;
}

// Function to calculate the storage overhead of the RFC classifier chunks
std::uint64_t Chunk_GetStorageOverhead(const RFC_Chunks& rfc_classifier){
    std::uint64_t overhead = 0;
    // phase0
    for(size_t i=0; i<6; ++i){
        overhead += 65536 * sizeof(std::uint16_t); // Each phase0 chunk has 65536 items of uint16_t
    }

    // phase1
    for(size_t i=0; i<2; ++i){
        overhead += rfc_classifier.phase1_chunks[i].item_num * sizeof(std::uint16_t); // Each phase1 chunk has item_num items of uint32_t
    }

    // final_phase
    overhead += rfc_classifier.final_chunks.item_num * sizeof(int); // Final chunk has item_num items of int
    
    return overhead;
}

// Function to set a bit in the CBM
void Set_cbm(std::uint32_t* tempcbm, unsigned int filter_index, bool value, unsigned int filter_num){
    unsigned int group,pos;
    unsigned int k =1;

    // Calculate the group and position in the CBM
    group = SIZE - 1 - (filter_num - 1 - filter_index) / 32;
    pos = (filter_num -1 - filter_index) % 32;
    k <<= pos;

    rfc_build_cycles += CYCLE_MEMORY_ACCESS; 

    if (value == true){
        tempcbm[group] |= k;
        rfc_build_cycles += CYCLE_ADD;
    }
    else{
        k = ~k;
        tempcbm[group] &= k;
        rfc_build_cycles += CYCLE_ADD;
    }
}

// Function to compare two CBMs
bool Compare_CBM(std::uint32_t* cbm_a, std::uint32_t* cbm_b) {
    
    if ((cbm_a == nullptr) || (cbm_b == nullptr)) {
        std::cerr << "cbm_a or cbm_b Null error" << std::endl;
        return false;
    }

    for (size_t i = 0; i < SIZE; ++i) {
        if ((*(cbm_a+i)) != (*(cbm_b+i))) {
            rfc_build_cycles += CYCLE_MEMORY_ACCESS + CYCLE_BRANCH;
            return false;
        }

        rfc_build_cycles += CYCLE_MEMORY_ACCESS;
    }

    return true;
}


// Function to search for a CBM in an EqIDTable
int Search_CBM(EqIDTable* temp_EqIDTable, std::uint32_t* tempcbm){
    EqID* tempEqID = temp_EqIDTable->head;

    if(tempEqID == nullptr){
        return -1;
    }
    else{
        while (tempEqID != nullptr){

            if (Compare_CBM(tempEqID->cbm, tempcbm)) {
                rfc_build_cycles += CYCLE_BRANCH;
                rfc_build_cycles += CYCLE_MEMORY_ACCESS;
                return tempEqID->eqid;
            }
            else{
                tempEqID = tempEqID->next;
                rfc_build_cycles += CYCLE_MEMORY_ACCESS;
            }
        }
        
        rfc_build_cycles += CYCLE_BRANCH;
        return -1;
    }
}


// Function to add a CBM to an EqIDTable
int Add_cbm_EqIDTable(EqIDTable* temp_EqIDTable, std::uint32_t* tempcbm){
    EqID* new_EqID = new EqID;

    for (size_t i=0; i<SIZE; i++){
        new_EqID->cbm[i] = tempcbm[i];
        rfc_build_cycles += CYCLE_MEMORY_ACCESS;
    }
    
    if (temp_EqIDTable->head == nullptr){
        new_EqID->eqid = 0;
        new_EqID->next = nullptr;
        temp_EqIDTable->head = new_EqID;
        temp_EqIDTable->tail = new_EqID;
        temp_EqIDTable->item_num ++;

        rfc_build_cycles += CYCLE_MEMORY_ACCESS;
    }
    else{
        new_EqID->eqid = temp_EqIDTable->item_num;
        new_EqID->next = nullptr;
        temp_EqIDTable->tail->next = new_EqID;
        temp_EqIDTable->tail = new_EqID;
        temp_EqIDTable->item_num ++;

        rfc_build_cycles += CYCLE_MEMORY_ACCESS;
    }

    return temp_EqIDTable->item_num -1;
}


// Function to find a rule in the CBM
int Find_rule(std::uint32_t* tempcbm, unsigned int filter_num){
    std::uint32_t flag;
    std::uint32_t result;

    for (size_t k=0; k<SIZE; k++){
        rfc_build_cycles += CYCLE_BRANCH;

        flag = 1 << 31;
        rfc_build_cycles += CYCLE_ADD;

        for (size_t pos=1; pos <= 32; pos++){
            rfc_build_cycles += CYCLE_BRANCH;

            result = tempcbm[k] & flag;
            rfc_build_cycles += CYCLE_MEMORY_ACCESS;
            rfc_build_cycles += CYCLE_ADD;

            if (result){
                rfc_build_cycles += CYCLE_BRANCH;
                rfc_build_cycles += CYCLE_MUL;
                return (32*k + pos - 1) - (32 - filter_num % 32);
            }
            else flag >>= 1;
        }
    }

    // rfc_build_cycles += CYCLE_BRANCH;
    return -1;
}


// building rfc.classifier.phase0
void Creat_Phase0(std::vector<FilterRule>& filter_rules, RFC_Chunks& rfc_classifier){
    for (size_t i = 0; i < 6; i++){
        
        // Initialize cbm
        std::uint32_t tempcbm[SIZE];
        for (size_t j = 0; j < SIZE; j++){
            tempcbm[j] = 0;
        }

        // Initialize EqIDTable
        EqIDTable* temp_EqIDTable = new EqIDTable;
        temp_EqIDTable->item_num = 0;
        temp_EqIDTable->head = nullptr;
        temp_EqIDTable->tail = nullptr;

        // Match EqIDTable with phase0_chunk
        rfc_classifier.phase0_chunks[i].matched_EqIDTable = temp_EqIDTable;

        for (unsigned int n = 0; n < 65536; n++){
            unsigned short tempstart, tempend;
            int tempEqID;

            for (unsigned int m=0; m < filter_rules.size(); m++){
                tempstart = filter_rules[m].dim[i][0];
                tempend = filter_rules[m].dim[i][1];
                rfc_build_cycles += CYCLE_MEMORY_ACCESS * 2;

                if (tempstart == n) {
                    rfc_build_cycles += CYCLE_BRANCH;
                    Set_cbm(tempcbm, m, true, filter_rules.size());
                }
                if ((tempend + 1) == n) {
                    rfc_build_cycles += CYCLE_BRANCH;
                    Set_cbm(tempcbm, m, false, filter_rules.size());
                }
            }

            tempEqID = Search_CBM(temp_EqIDTable, tempcbm);
            if (tempEqID == -1){
                tempEqID = Add_cbm_EqIDTable(temp_EqIDTable, tempcbm);
            }
            
            rfc_classifier.phase0_chunks[i].item[n] = tempEqID;
        }
    }
}

// building rfc.classifier.phase1
void Creat_Phase1(RFC_Chunks& rfc_classifier){
    phase0_ChunkTable* temp1;
    phase0_ChunkTable* temp2;
    phase0_ChunkTable* temp3;

    for (size_t i =0; i<2; ++i){

        switch (i)
        {
        case 0:
            temp1 = &rfc_classifier.phase0_chunks[0];
            temp2 = &rfc_classifier.phase0_chunks[1];
            temp3 = &rfc_classifier.phase0_chunks[5];
            rfc_build_cycles += CYCLE_MEMORY_ACCESS * 3;
            break;

        case 1:
            temp1 = &rfc_classifier.phase0_chunks[2];
            temp2 = &rfc_classifier.phase0_chunks[3];
            temp3 = &rfc_classifier.phase0_chunks[4];
            rfc_build_cycles += CYCLE_MEMORY_ACCESS * 3;
            break;
        }
        
        // Initialize rfc_classifier.phase1_chunks
        std::uint32_t tempnum;
        tempnum = temp1->matched_EqIDTable->item_num * temp2->matched_EqIDTable->item_num * temp3->matched_EqIDTable->item_num;
        rfc_build_cycles += CYCLE_MUL * 2;

        rfc_classifier.phase1_chunks[i].item_num = tempnum;
        // rfc_classifier.phase1_chunks[i].item = (std::uint16_t*) malloc (tempnum * sizeof(std::uint16_t));
        rfc_classifier.phase1_chunks[i].item.resize(tempnum);
        

        EqIDTable* temp_EqIDTable = new EqIDTable;
        temp_EqIDTable->item_num = 0;
        temp_EqIDTable->head = nullptr;
        temp_EqIDTable->tail = nullptr;
        rfc_classifier.phase1_chunks[i].matched_EqIDTable = temp_EqIDTable;
      

        // Initialize cbm
        std::uint32_t tempcbm[SIZE];
        for (size_t j = 0; j < SIZE; j++){
            tempcbm[j] = 0;
        }
        std::uint32_t index = 0;

        for (auto* eq1 = temp1->matched_EqIDTable->head; eq1 != nullptr; eq1 = eq1->next){
            // rfc_build_cycles += CYCLE_BRANCH;
            for (auto* eq2 = temp2->matched_EqIDTable->head; eq2 != nullptr; eq2 = eq2->next){
                // rfc_build_cycles += CYCLE_BRANCH;
                for (auto* eq3 = temp3->matched_EqIDTable->head; eq3 != nullptr; eq3 = eq3->next){
                    // rfc_build_cycles += CYCLE_BRANCH;

                    for (size_t j =0; j < SIZE; ++j){
                        tempcbm[j] = eq1->cbm[j] & eq2->cbm[j] & eq3->cbm[j];
                        rfc_build_cycles += CYCLE_MEMORY_ACCESS * 3 + CYCLE_ADD * 2;                    
                    }

                    int tempEqID = Search_CBM(rfc_classifier.phase1_chunks[i].matched_EqIDTable, tempcbm);
                    if (tempEqID == -1){     
                        rfc_build_cycles += CYCLE_BRANCH;                   
                        tempEqID = Add_cbm_EqIDTable(rfc_classifier.phase1_chunks[i].matched_EqIDTable, tempcbm);

                    }

                    rfc_classifier.phase1_chunks[i].item[index] = tempEqID;
                    // rfc_build_cycles += CYCLE_MEMORY_ACCESS;

                    index ++;
                }
            }
        }   
    }
}

// building rfc.classifier.finalphase
void Creat_phasefinal(std::vector<FilterRule>& filter_rules, RFC_Chunks& rfc_classifier){

    // Initialize rfc_classifier.phase1_chunks
    std::uint32_t tempnum;
    tempnum = rfc_classifier.phase1_chunks[0].matched_EqIDTable->item_num * rfc_classifier.phase1_chunks[1].matched_EqIDTable->item_num;
    rfc_build_cycles += CYCLE_MUL * 2;

    rfc_classifier.final_chunks.item_num = tempnum;
    // rfc_classifier.final_chunks.item = (int*) malloc (tempnum * sizeof(int));
    rfc_classifier.final_chunks.item.resize(tempnum);
    // rfc_build_cycles += CYCLE_MEMORY_ACCESS;
    
    // Initialize cbm
    std::uint32_t tempcbm[SIZE];
    for (size_t j = 0; j < SIZE; j++){
        tempcbm[j] = 0;
    }

    std::uint32_t index = 0;

    for (auto* eq1 = rfc_classifier.phase1_chunks[0].matched_EqIDTable->head; eq1 != nullptr; eq1 = eq1->next){
        // rfc_build_cycles += CYCLE_BRANCH;
        for(auto* eq2 = rfc_classifier.phase1_chunks[1].matched_EqIDTable->head; eq2 != nullptr; eq2 = eq2->next){
            // rfc_build_cycles += CYCLE_BRANCH;

            for (size_t j=0; j<SIZE; ++j){
                tempcbm[j] = eq1->cbm[j] & eq2->cbm[j];
                rfc_build_cycles += CYCLE_MEMORY_ACCESS * 2;
                // rfc_build_cycles += CYCLE_ADD;
            }

            int matched_rule = Find_rule(tempcbm, filter_rules.size());
            rfc_classifier.final_chunks.item[index] = matched_rule;
            // rfc_build_cycles += CYCLE_MEMORY_ACCESS;

            index ++;
          
        }
    }
}



// Classifer building
RFC_Chunks Building_Classfier(std::vector<FilterRule>& filter_rules){
    rfc_build_cycles = 0;
    rfc_storage_overhead = 0;
    EqIDTable_storage_overhead = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    RFC_Chunks rfc_classifier;

    // building phase0
    
    Creat_Phase0(filter_rules, rfc_classifier);

    // building phase1
    Creat_Phase1(rfc_classifier);

    // building finalphase
    Creat_phasefinal(filter_rules, rfc_classifier);

    auto end_time = std::chrono::high_resolution_clock::now(); 
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    //
    for (const auto& chunk:rfc_classifier.phase0_chunks){
        EqIDTable_storage_overhead += EqIDTable_GetStorageOverhead(chunk.matched_EqIDTable);
    }

    for (const auto& chunk:rfc_classifier.phase1_chunks){
        EqIDTable_storage_overhead += EqIDTable_GetStorageOverhead(chunk.matched_EqIDTable);
    }

    rfc_storage_overhead += Chunk_GetStorageOverhead(rfc_classifier);

    // 输出时间和空间开销
    std::cout << "RFC classifier build time: " << elapsed_time << " microseconds" << std::endl;
    std::cout << "RFC classifier build cycles: " << rfc_build_cycles << " cycles" << std::endl;
    std::cout << "RFC classifier storage overhead: " << rfc_storage_overhead / 1024.0 << " KB" << std::endl;
    std::cout << "RFC EqIDTable storage overhead: " << EqIDTable_storage_overhead / 1024.0 << " KB" << std::endl;
    std::cout << " " << std::endl;
    return rfc_classifier;
}
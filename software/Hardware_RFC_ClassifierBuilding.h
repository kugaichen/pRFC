#ifndef HARDWARE_RFC_CLASSIFIERBUILDING_H
#define HARDWARE_RFC_CLASSIFIERBUILDING_H

#include <cstdint>
#include <vector>
#include "Hardware_RFC_Structure.h"
#include "FilterReader.h"
#include "HardwareSimulator.h" 

void Set_cbm(std::uint32_t* tempcbm, unsigned int filter_index,
    bool value, unsigned int filter_num);

bool Compare_CBM(std::uint32_t* cbm_a, std::uint32_t* cbm_b);

int Search_CBM(EqIDTable* temp_EqIDTable, std::uint32_t* tempcbm);

int Add_cbm_EqIDTable(EqIDTable* temp_EqIDTable, std::uint32_t* tempcbm);

void Creat_Phase0(std::vector<FilterRule>& filter_rules, RFC_Chunks& rfc_classifier);

void Creat_Phase1(RFC_Chunks& rfc_classifier);

// ChunkTable Creat_Phase2(std::uint32_t SIZE, std::vector<ChunkTable> phase1_chunks);

void Creat_phasefinal(std::vector<FilterRule>& filter_rules, RFC_Chunks& rfc_classifier);

int Find_rule(std::uint32_t* tempcbm, unsigned int filter_num);

std::uint64_t EqID_GetStorageOverhead(const EqID* eqid);

std::uint64_t EqIDTable_GetStorageOverhead(const EqIDTable* eqidTable);

std::uint64_t Chunk_GetStorageOverhead(const RFC_Chunks& rfc_classifier);

RFC_Chunks Building_Classfier(std::vector<FilterRule>& filter_rules);


#endif 
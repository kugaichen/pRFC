#pragma once

#ifndef HARDWARE_IMPROVEDCLASSIFIERBUILDING_H
#define HARDWARE_IMPROVEDCLASSIFIERBUILDING_H

#include <cstdint>
#include <vector>
#include <thread>       // For std::thread::hardware_concurrency
#include <stdexcept>    // For std::invalid_argument
#include <atomic>       // For std::atomic if fine-grained counters were used (not in this final version for phase0/1 tasks)
#include <mutex>        // For std::mutex, std::lock_guard (used in SE_Search_CBM, SE_Add_cbm_EqIDTable)
// #include "Hardware_RFC_Structure.h"
// #include "FilterReader.h"
#include "HardwareSimulator.h" 
#include "Filter_Preprocess.h"

// struct se_EqID
// {
//     std::uint16_t eqid;
//     std::vector<std::uint32_t> cbm;
//     se_EqID* next;

//     se_EqID() : eqid(0), next(nullptr) {}
// };

// struct se_EqIDTable
// {
//     std::uint32_t item_num;
//     se_EqID* head;
//     se_EqID* tail;
// };
struct se_EqID {
    std::vector<std::uint32_t> cbm;
    std::uint32_t eqid;
    se_EqID* next;

    se_EqID() : eqid(0), next(nullptr) {} // Basic constructor
};

struct se_EqIDTable {
    se_EqID* head;
    se_EqID* tail;
    std::uint32_t item_num;
    std::mutex mtx; // Mutex for thread-safe access

    se_EqIDTable() : head(nullptr), tail(nullptr), item_num(0) {}

    ~se_EqIDTable() {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety during destruction if table could be accessed
        se_EqID* current = head;
        while (current != nullptr) {
            se_EqID* next_node = current->next;
            delete current;
            current = next_node;
        }
        head = nullptr;
        tail = nullptr;
        item_num = 0;
    }
};

struct se_phase1_ChunkTable
{
    std::uint64_t item_num;         // Number of items in the table
    // std::uint16_t* item;            // Pointer to an array of items
    std::vector<std::uint16_t> item;
    se_EqIDTable* matched_EqIDTable;   // Pointer to the matched EqIDTable

};

struct se_fianl_ChunkTable
{
    std::uint64_t item_num;         // Number of items in the table
    // int* item;                      // Pointer to an array of items
    std::vector<int> item;
};




struct SUB_GROUP
{
    // basic information
    std::uint16_t src_mask;
    std::uint16_t dst_mask;
    int src_mask_flag;
    int dst_mask_flag;
    std::vector<FilterRule> sub_filter_rules;

    // CBM information 
    static const std::uint16_t size_length = 32;
    std::uint16_t sub_size;

    // chunk information
    std::vector<int> selected_chunks;
};

struct se_phase0_ChunkTable
{   
    std::uint16_t factor;             // the interval of items
    std::uint32_t item_num;             // Number of items in the table
    std::vector<std::uint16_t> item;    // Pointer to an array of items
    // std::uint16_t item[65536];
    // std::uint16_t* item;
    se_EqIDTable* matched_EqIDTable;   // Pointer to the matched EqIDTable
};


struct RFC_SE_Chunks
{
    se_phase0_ChunkTable phase0_chunks[6];
    se_phase1_ChunkTable phase1_chunks[2];
    se_fianl_ChunkTable final_chunks;

};


struct RFC_SE_Classifier
{
    RFC_SE_Chunks rfc_classifier_H_H;
    RFC_SE_Chunks rfc_classifier_H_L;
    RFC_SE_Chunks rfc_classifier_L_L;
    RFC_SE_Chunks rfc_classifier_L_H;

};

void SE_Set_cbm(std::vector<std::uint32_t>& tempcbm, unsigned int filter_index, bool value, unsigned int filter_num, 
    std::uint16_t sub_size, std::uint16_t size_length);

int SE_Search_CBM(se_EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>& tempcbm,std::uint16_t sub_size);

bool SE_Compare_CBM(const std::vector<std::uint32_t>& cbm_a, const std::vector<std::uint32_t>& cbm_b, std::uint16_t sub_size);

int SE_Add_cbm_EqIDTable(se_EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>& tempcbm, std::uint16_t sub_size);

int SE_Find_rule(const std::vector<std::uint32_t>& tempcbm, unsigned int filter_num, std::uint16_t sub_size, std::uint16_t size_length);

// std::uint64_t SE_EqID_GetStorageOverhead(const se_EqID* eqid, SUB_GROUP sub_group);

// std::uint64_t SE_EqIDTable_GetStorageOverhead(const se_EqIDTable* eqidTable, SUB_GROUP sub_group);

// std::uint64_t SE_Chunk_GetStorageOverhead(const RFC_SE_Chunks& rfc_se_classifier,std::vector<std::uint64_t>& rfc_se_classifier_p0_storage_vector,std::vector<std::uint64_t>& rfc_se_classifier_p1_storage_vector,std::vector<std::uint64_t>& rfc_se_classifier_p2_storage_vector);

void SE_Creat_phase0(SUB_GROUP sub_group, RFC_SE_Chunks& rfc_se_classifier);

void SE_Creat_phase1(SUB_GROUP sub_group, RFC_SE_Chunks& rfc_se_classifier);

void SE_Creat_phasefinal(SUB_GROUP sub_group, RFC_SE_Chunks& rfc_se_classifier);

// RFC_SE_Chunks Improved_Building_Classifier(const Filter_Division filter_division, const int flag);
RFC_SE_Chunks Improved_Building_Classifier(const Filter_Division& filter_division, const int flag);

// FPGA_Resources Calculate_FPGA_Resources(RFC_SE_Classifier& classifier);

#endif
#ifndef HARDWARE_RFC_STRUCTURE_H
#define HARDWARE_RFC_STRUCTURE_H

#include <cstdint>
#include <vector>

#define SIZE  313                //Define the size of the CBM array -> SIZE = MAX_PARSED_PKT_Rule / BITS_PER_GROUP
#define MAX_FILTER_RULE 10011 
#define MAX_PARSED_PKT  1001100 //Define the maximum number of parsed packets
#define BITS_PER_GROUP 32           //Define the number of bits per group

struct EqID
{
    std::uint16_t eqid;
    std::uint32_t cbm[SIZE];
    EqID* next;                     // Pointer to the next EqID in the linked list

};

struct EqIDTable
{
    std::uint32_t item_num;         // Number of items in the table
    EqID* head;                     // Pointer to the first EqID in the linked list
    EqID* tail;                     // Pointer to the last EqID in the linked list

};

struct phase0_ChunkTable
{
    std::uint16_t item[65536];      // Array of items (65536 entries)
    EqIDTable* matched_EqIDTable;   // Pointer to the matched EqIDTable
};

struct phase1_ChunkTable
{
    std::uint32_t item_num;         // Number of items in the table
    // std::uint16_t* item;            // Pointer to an array of items
    std::vector<std::uint32_t> item;
    EqIDTable* matched_EqIDTable;   // Pointer to the matched EqIDTable

};

struct fianl_ChunkTable
{
    std::uint32_t item_num;         // Number of items in the table
    // int* item;                      // Pointer to an array of items
    std::vector<std::uint32_t> item;
};

struct RFC_Chunks
{
    phase0_ChunkTable phase0_chunks[6]; // Array of 6 Phase 0 Chunk Tables
    phase1_ChunkTable phase1_chunks[2]; // Array of 2 Phase 1 Chunk Tables
    fianl_ChunkTable final_chunks;      // Final Chunk Table
};

// class ChunkTable;

// class EqID{
// public:
//     std::uint16_t eqid;
//     std::vector<std::uint32_t> cbm;
//     EqID* next;

//     EqID(std::uint16_t eqid_param, std::uint32_t SIZE): 
//         eqid(eqid_param), cbm(SIZE,0), next(nullptr){}

//     std::uint64_t GetStorageOverhead() const{
//         return sizeof(eqid) + cbm.size() * sizeof(std::uint32_t) + sizeof(next);
//     }

// };

// class EqIDTable{
// public:
//     std::uint32_t item_num;
//     EqID* head;
//     EqID* tail;
//     ChunkTable* matched_ChunkTable;

//     EqIDTable(std::uint16_t item_num_param = 0):
//         item_num(item_num_param), head(nullptr), tail(nullptr), matched_ChunkTable(nullptr){}

//     void add_EqID(EqID* real_EqID){
//         item_num ++;
//         if (head == nullptr){
//             head = real_EqID;
//             tail = real_EqID;
//         }
//         else{
//             tail->next = real_EqID;
//             tail = real_EqID;
//         }
//     }

//     std::uint64_t GetStorageOverhead() const{
//         std::uint64_t size = sizeof(item_num) + sizeof(head) + sizeof(tail) + sizeof(matched_ChunkTable);
//         return size;
//     }

//     std::uint64_t GetAllStorageOverhead() const{
//         std::uint64_t all_size = 0;
//         EqID* current = head;
//         while (current != nullptr){
//             all_size += current->GetStorageOverhead();
//             current = current->next;
//         }
//         return all_size;
//     }

// };



// class ChunkTable{
// public:
//     std::uint32_t chunk_num;
//     std::vector<std::uint16_t> item;
//     EqIDTable* matched_EqIDTable;

//     ChunkTable(std::uint32_t chunk_num_param = 0):
//         chunk_num(chunk_num_param){
//             item.resize(chunk_num);
//         }

//     void match_EqIDTable(EqIDTable* real_EqIDTable){
//         real_EqIDTable->matched_ChunkTable = this;
//         matched_EqIDTable = real_EqIDTable;
//     }

//     std::uint64_t GetStorageOverhead() const{
//         return sizeof(chunk_num) + item.size() * sizeof(int) + sizeof(matched_EqIDTable);
//     }
// };


// struct RFC_Chunks
// {
//     ChunkTable phase0_chunks[6]; 
//     ChunkTable phase1_chunks[2]; 
//     // ChunkTable phase2_chunks; 
//     ChunkTable final_chunks;

//     RFC_Chunks() {};

//     void Init_phase0chunks(std::uint32_t phase0_size){
//         for (size_t i=0; i<6; i++){
//             phase0_chunks[i] = ChunkTable(phase0_size);
//         }
//     }

//     void Init_phase1chunks_0(std::uint32_t phase1_size){
//         phase1_chunks[0] = ChunkTable(phase1_size);
//     }

//     void Init_phase1chunks_1(std::uint32_t phase1_size){
//         phase1_chunks[1] = ChunkTable(phase1_size);
//     }

//     void Init_finalchunks(std::uint32_t final_size){
//         final_chunks = ChunkTable(final_size);
//     }
// };







#endif
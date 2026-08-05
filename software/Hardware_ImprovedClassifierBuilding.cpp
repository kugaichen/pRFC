
// #include "Hardware_ImprovedClassifierBuilding.h"
// #include <iostream>
// #include <chrono>
// #include <numeric>

// #include <future> // Required for std::async, std::future
// #include <vector> // Required for std::vector<std::future>
// #include <functional> // Required for std::ref, std::cref
// #include <thread> // 为了 std::thread::hardware_concurrency





// // define the ceiling division argument
// int CeilDivide(int a, int b){
//     if(b == 0){
//         throw std::invalid_argument("Division by zero is not allowed");
//     }

//     return (a + b -1) / b;
// }

// // Function to set a bit in the CBM
// void SE_Set_cbm(std::vector<std::uint32_t>& tempcbm, unsigned int filter_index, bool value, unsigned int filter_num, std::uint16_t sub_size, std::uint16_t size_length){
//     unsigned int group,pos;
//     unsigned int k =1;

//     // Calculate the group and position in the CBM
//     group =  sub_size - 1 - (filter_num - 1 - filter_index) / size_length;
//     pos = (filter_num -1 - filter_index) % size_length;
//     k <<= pos;
    

//     if (value == true){
//         tempcbm[group] |= k;
 
//     }
//     else{
//         k = ~k;
//         tempcbm[group] &= k;
  
//     }
// }




// // Function to search for a CBM in an EqIDTable
// int SE_Search_CBM(se_EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>& tempcbm,std::uint16_t sub_size){
//     se_EqID* tempEqID = temp_EqIDTable->head;

//     if(tempEqID == nullptr){
//         return -1;
//     }
//     else{
//         while (tempEqID != nullptr){
          
//             if (SE_Compare_CBM(tempEqID->cbm, tempcbm, sub_size)) {
           
//                 return tempEqID->eqid;
//             }
//             else{
       
//                 tempEqID = tempEqID->next;
                
//             }
//         }
        
//         return -1;
//     }
// }


// // Function to compare two CBMs
// bool SE_Compare_CBM(const std::vector<std::uint32_t>& cbm_a, const std::vector<std::uint32_t>& cbm_b, std::uint16_t sub_size){
    
//     if (cbm_a.size() != sub_size || cbm_b.size() != sub_size) {
//         std::cerr << "[ERROR] Vector size mismatch: cbm_a.size() = " << cbm_a.size() 
//                   << ", cbm_b.size() = " << cbm_b.size() 
//                   << ", expected sub_size = " << sub_size << std::endl;
//         return false;
//     }

//     auto it_a = cbm_a.begin();
//     auto it_b = cbm_b.begin();

//     for (size_t i = 0; i < sub_size; ++i, ++it_a, ++it_b) {
//         if (*it_a != *it_b) {
//             return false;
//         }
//     }

//     return true;
// }


// // Function to add a CBM to an EqIDTable
// int SE_Add_cbm_EqIDTable(se_EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>& tempcbm, std::uint16_t sub_size){
//     // se_EqID* new_EqID = new(std::nothrow) se_EqID;
//     // new_EqID->cbm.resize(sub_size);
//     // new_EqID->next = nullptr;
//     se_EqID* new_EqID = new (std::nothrow) se_EqID;

//     if (new_EqID == nullptr) {
//         std::cerr << "[ERROR] Memory allocation failed" << std::endl;
//         return -1;
//     }
//     // Initialize
//     new_EqID->cbm.resize(sub_size);
//     new_EqID->next = nullptr;
//     new_EqID->eqid = 0;


//     if (new_EqID->cbm.size() != sub_size || tempcbm.size() != sub_size) {
//         std::cerr << "[ERROR] Vector size mismatch: new_EqID—>cbm.size() = " << new_EqID->cbm.size() 
//                   << ", tempcbm.size() = " << tempcbm.size() 
//                   << ", expected sub_size = " << sub_size << std::endl;
//         delete new_EqID; // 释放内存
//         return -1;
//     }


//     for (size_t i=0; i<sub_size; i++){
//         new_EqID->cbm[i] = tempcbm[i];
//     }
    
//     if (temp_EqIDTable->head == nullptr){
//         new_EqID->eqid = 0;
//         new_EqID->next = nullptr;
//         temp_EqIDTable->head = new_EqID;
//         temp_EqIDTable->tail = new_EqID;
//         temp_EqIDTable->item_num ++;


//     }
//     else{
//         new_EqID->eqid = temp_EqIDTable->item_num;
//         new_EqID->next = nullptr;
//         temp_EqIDTable->tail->next = new_EqID;
//         temp_EqIDTable->tail = new_EqID;
//         temp_EqIDTable->item_num ++;

//     }

//     // delete new_EqID;

//     return temp_EqIDTable->item_num -1;
// }

// // Function to find a rule in the CBM
// int SE_Find_rule(const std::vector<std::uint32_t>& tempcbm, unsigned int filter_num, std::uint16_t sub_size, std::uint16_t size_length){
//     std::uint32_t flag;
//     std::uint32_t result;

//     for (size_t k=0; k<sub_size; k++){

//         flag = 1 << (size_length-1);

//         for (size_t pos=1; pos <= size_length; pos++){

//             result = tempcbm[k] & flag;

//             if (result){
  
//                 return (size_length*k + pos - 1) - (size_length - filter_num % size_length);
//             }
//             else{
//                 flag >>= 1;
//             }
//         }
//     }

//     return -1;
// }


// // original！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
// // // building rfc_se.classifier.phase0
// // void SE_Creat_phase0(SUB_GROUP sub_group, RFC_SE_Chunks& rfc_se_classifier){
// //     int selected_flag;
// //     if(!sub_group.selected_chunks.empty()){
// //         // std::cout<<"seleceted_chunks is not empty!"<<std::endl;
// //         for (size_t i = 0; i < 6; i++){

// //             //Initialize seleceted_flag;
// //             selected_flag = 0;

// //             //Initialize cbm
// //             std::vector<std::uint32_t> tempcbm(sub_group.sub_size);
// //             for (size_t j=0; j < sub_group.sub_size; j++){
// //                 tempcbm[j] = 0;
// //             }

// //             // Initialize EqIDTable
// //             se_EqIDTable* temp_EqIDTable = new se_EqIDTable;
// //             temp_EqIDTable->item_num = 0;
// //             temp_EqIDTable->head = nullptr;
// //             temp_EqIDTable->tail = nullptr;

// //             // Match EqIDTable with phase0_chunk
// //             rfc_se_classifier.phase0_chunks[i].matched_EqIDTable = temp_EqIDTable;

// //             for (const auto sub_item:sub_group.selected_chunks){
// //                 if(i == sub_item){
// //                     std::uint16_t factor;
// //                     std::uint16_t interval;
// //                     std::uint32_t count;
// //                     std::vector<std::uint16_t> seg_range;

// //                     // flag to make sure haaving stroage efficiency
// //                     selected_flag = 1;
// //                     switch (i)
// //                     {
// //                         case 0:
// //                             // src_mask < 16
// //                             factor = 16 - sub_group.src_mask;
// //                             interval = (1 << factor) - 1;
// //                             count = 1 << sub_group.src_mask;
// //                             break;

// //                         case 1:
// //                             if(sub_group.selected_chunks[0] != 0){
// //                                 // src_mask >= 16 
// //                                 factor = 32- sub_group.src_mask;
// //                                 interval = (1 << factor) - 1;
// //                                 count = 1 << (sub_group.src_mask - 16);
// //                             }

// //                             else{
// //                                 factor = 16;
// //                                 interval = (1 << 16) -1;
// //                                 count = 1;
// //                             }

// //                             break;

// //                         case 2:
// //                             // dst_mask < 16
// //                             factor = 16 - sub_group.dst_mask;
// //                             interval = (1 << factor) - 1;
// //                             count = 1 << sub_group.dst_mask;
// //                             break;

// //                         case 3:
// //                             if(sub_group.selected_chunks[sub_group.selected_chunks.size()-2] != 2){
// //                                 // dst_mask >= 16 
// //                                 factor = 32- sub_group.dst_mask;
// //                                 interval = (1 << factor) - 1;
// //                                 count = 1 << (sub_group.dst_mask - 16);
// //                             }

// //                             else{
// //                                 factor = 16;
// //                                 interval = (1 << 16) -1;
// //                                 count = 1;
// //                             }

// //                             break;
// //                     }


// //                     // Intialization segment range
// //                     if (!(interval >= 0)){
// //                         throw std::invalid_argument("intevel error!");
                        
// //                     }

// //                     rfc_se_classifier.phase0_chunks[i].item_num = count;
// //                     rfc_se_classifier.phase0_chunks[i].factor = factor;
// //                     rfc_se_classifier.phase0_chunks[i].item.resize(count);
                    
// //                     // building 
// //                     unsigned int real_n = 0;



// //                     std::uint64_t cbm_time = 0;
// //                     for(unsigned int n = 0; n < 65536; n = n + interval + 1){
// //                         unsigned short tempstart, tempend;
// //                         int tempEqID;

// //                         for (unsigned int m = 0; m < sub_group.sub_filter_rules.size(); m++){
// //                             tempstart = sub_group.sub_filter_rules[m].dim[i][0];
// //                             tempend = sub_group.sub_filter_rules[m].dim[i][1];

// //                             if (tempstart == n) {
// //                                 SE_Set_cbm(tempcbm, m, true, sub_group.sub_filter_rules.size(),sub_group.sub_size,sub_group.size_length);
// //                             }
// //                             if ((tempend + 1) == n) {
// //                                 SE_Set_cbm(tempcbm, m, false, sub_group.sub_filter_rules.size(),sub_group.sub_size,sub_group.size_length);
// //                             }
// //                         }

// //                         tempEqID = SE_Search_CBM(temp_EqIDTable,tempcbm,sub_group.sub_size);


// //                         if (tempEqID == -1){
// //                             tempEqID = SE_Add_cbm_EqIDTable(temp_EqIDTable, tempcbm, sub_group.sub_size);
// //                         }
            
// //                         rfc_se_classifier.phase0_chunks[i].item[real_n] = tempEqID;
// //                         real_n ++;
// //                     }
// //                 }

// //             }

// //             if(selected_flag == 1){
// //                 continue;
// //             }

// //             // normal
// //             rfc_se_classifier.phase0_chunks[i].item_num = 65536;
// //             rfc_se_classifier.phase0_chunks[i].factor= 0;
// //             rfc_se_classifier.phase0_chunks[i].item.resize(65536);

// //             std::uint64_t cbm_time = 0;

// //             for (unsigned int n = 0; n < 65536; n++){
// //                 unsigned short tempstart, tempend;
// //                 int tempEqID;
                

// //                 for (unsigned int m=0; m<sub_group.sub_filter_rules.size(); m++){
// //                     tempstart = sub_group.sub_filter_rules[m].dim[i][0];
// //                     tempend = sub_group.sub_filter_rules[m].dim[i][1];

// //                     if (tempstart == n){
// //                         SE_Set_cbm(tempcbm, m, true, sub_group.sub_filter_rules.size(),sub_group.sub_size,sub_group.size_length);
// //                     }
// //                     if ((tempend + 1) == n){
// //                         SE_Set_cbm(tempcbm, m, false, sub_group.sub_filter_rules.size(),sub_group.sub_size,sub_group.size_length);
// //                     }
// //                 }


// //                 tempEqID = SE_Search_CBM(temp_EqIDTable,tempcbm,sub_group.sub_size);         

// //                 if (tempEqID == -1){
// //                     tempEqID = SE_Add_cbm_EqIDTable(temp_EqIDTable, tempcbm, sub_group.sub_size);
// //                 }

// //                 rfc_se_classifier.phase0_chunks[i].item[n] = tempEqID;
        
// //             }
// //         }   

// //     }

// //     else{
// //         // std::cout<<"seleceted_chunks is empty!"<<std::endl;
// //         for (size_t i=0; i < 6; i++){

// //             //Initialize cbm
// //             std::vector<std::uint32_t> tempcbm(sub_group.sub_size);
// //             for (size_t j=0; j < sub_group.sub_size; j++){
// //                 tempcbm[j] = 0;
// //             }

// //             // Initialize EqIDTable
// //             se_EqIDTable* temp_EqIDTable = new se_EqIDTable;
// //             temp_EqIDTable->item_num = 0;
// //             temp_EqIDTable->head = nullptr;
// //             temp_EqIDTable->tail = nullptr;

// //             // Match EqIDTable with phase0_chunk
// //             rfc_se_classifier.phase0_chunks[i].matched_EqIDTable = temp_EqIDTable;

// //             // normal
// //             rfc_se_classifier.phase0_chunks[i].item_num = 65536;
// //             rfc_se_classifier.phase0_chunks[i].factor = 0;
// //             rfc_se_classifier.phase0_chunks[i].item.resize(65536);


// //             std::uint64_t cbm_time = 0;
// //             for (unsigned int n = 0; n < 65536; n++){
// //                 unsigned short tempstart, tempend;
// //                 int tempEqID;

// //                 for (unsigned int m=0; m<sub_group.sub_filter_rules.size(); m++){
// //                     tempstart = sub_group.sub_filter_rules[m].dim[i][0];
// //                     tempend = sub_group.sub_filter_rules[m].dim[i][1];


// //                     if (tempstart == n){     
// //                         SE_Set_cbm(tempcbm, m, true, sub_group.sub_filter_rules.size(),sub_group.sub_size, sub_group.size_length);
// //                     }
// //                     if ((tempend + 1) == n){      
// //                         SE_Set_cbm(tempcbm, m, false, sub_group.sub_filter_rules.size(),sub_group.sub_size, sub_group.size_length);
// //                     }
// //                 }


// //                 tempEqID = SE_Search_CBM(temp_EqIDTable,tempcbm,sub_group.sub_size);

// //                 if (tempEqID == -1){
// //                     tempEqID = SE_Add_cbm_EqIDTable(temp_EqIDTable, tempcbm, sub_group.sub_size);
// //                 }

// //                 rfc_se_classifier.phase0_chunks[i].item[n] = tempEqID;
        
// //             }
// //         }
        
// //     }
// // }

// // void SE_Creat_phase1(SUB_GROUP sub_group, RFC_SE_Chunks& rfc_se_classifier){
// //     se_phase0_ChunkTable* temp1;
// //     se_phase0_ChunkTable* temp2;
// //     se_phase0_ChunkTable* temp3;
// //     std::vector<std::uint64_t> phase1_time;
    
    
// //     for (size_t i =0; i<2; ++i){
// //         std::uint64_t phase1_time_single;

// //         switch (i)
// //         {
// //         case 0:
// //             temp1 = &rfc_se_classifier.phase0_chunks[0];
// //             temp2 = &rfc_se_classifier.phase0_chunks[1];
// //             temp3 = &rfc_se_classifier.phase0_chunks[5];
// //             break;

// //         case 1:
// //             temp1 = &rfc_se_classifier.phase0_chunks[2];
// //             temp2 = &rfc_se_classifier.phase0_chunks[3];
// //             temp3 = &rfc_se_classifier.phase0_chunks[4];
// //             break;

// //         }
   

// //         //Initialize rfc_se_classifier.phase1_chunks
// //         std::uint32_t tempnum;
// //         tempnum = temp1->matched_EqIDTable->item_num * temp2->matched_EqIDTable->item_num * temp3->matched_EqIDTable->item_num;
 

// //         rfc_se_classifier.phase1_chunks[i].item_num = tempnum;
// //         rfc_se_classifier.phase1_chunks[i].item.resize(tempnum);

        
// //         se_EqIDTable* temp_EqIDTable = new se_EqIDTable;
// //         temp_EqIDTable->item_num = 0;
// //         temp_EqIDTable->head = nullptr;
// //         temp_EqIDTable->tail = nullptr;
// //         rfc_se_classifier.phase1_chunks[i].matched_EqIDTable = temp_EqIDTable;
    
// //         // Initialize cbm
// //         std::vector<std::uint32_t> tempcbm(sub_group.sub_size);
// //         for (size_t j=0; j < sub_group.sub_size; j++){
// //             tempcbm[j] = 0;
// //         }
    
// //         std::uint32_t index = 0;


// //         std::uint64_t cbm_time = 0;
// //         for (se_EqID* eq1 = temp1->matched_EqIDTable->head; eq1 != nullptr; eq1 = eq1->next){
// //             for (se_EqID* eq2 = temp2->matched_EqIDTable->head; eq2 != nullptr; eq2 = eq2->next){
// //                 for (se_EqID* eq3 = temp3->matched_EqIDTable->head; eq3 != nullptr; eq3 = eq3->next){

// //                     for (size_t j =0; j < sub_group.sub_size; ++j){
// //                         tempcbm[j] = eq1->cbm[j] & eq2->cbm[j] & eq3->cbm[j];                   
// //                     }

// //                     int tempEqID = SE_Search_CBM(rfc_se_classifier.phase1_chunks[i].matched_EqIDTable, tempcbm, sub_group.sub_size);
// //                     if (tempEqID == -1){     
                
// //                         tempEqID = SE_Add_cbm_EqIDTable(rfc_se_classifier.phase1_chunks[i].matched_EqIDTable, tempcbm, sub_group.sub_size);

// //                     }

// //                     rfc_se_classifier.phase1_chunks[i].item[index] = tempEqID;
// //                     index ++;
// //                 }
// //             }
// //         } 

// //     }

// // }

// // Helper function to execute a single iteration of SE_Creat_phase0 logic
// void SE_Creat_phase0_task(const SUB_GROUP& sub_group, RFC_SE_Chunks& rfc_se_classifier, int chunk_index) {
//     // This function encapsulates the logic for processing a single phase0_chunk[chunk_index].
//     // It's crucial to correctly transcribe the original SE_Creat_phase0 loop's body here,
//     // specifically the part that processes rfc_se_classifier.phase0_chunks[chunk_index].

//     // Initialize cbm for this chunk_index
//     std::vector<std::uint32_t> tempcbm(sub_group.sub_size);
//     for (size_t j = 0; j < sub_group.sub_size; ++j) {
//         tempcbm[j] = 0;
//     }

//     // Initialize EqIDTable for this chunk_index
//     se_EqIDTable* temp_EqIDTable = new se_EqIDTable; // Ensure this is managed (e.g., by RFC_SE_Chunks destructor)
//     temp_EqIDTable->item_num = 0;
//     temp_EqIDTable->head = nullptr;
//     temp_EqIDTable->tail = nullptr;
//     rfc_se_classifier.phase0_chunks[chunk_index].matched_EqIDTable = temp_EqIDTable;

//     int selected_flag_for_chunk = 0; // Mimics the original selected_flag logic for a single chunk

//     // Determine if the optimized path or normal path should be taken for this chunk_index
//     if (!sub_group.selected_chunks.empty()) {
//         for (const auto sub_item : sub_group.selected_chunks) {
//             if (chunk_index == sub_item) { // If current chunk_index is in selected_chunks
//                 std::uint16_t factor = 0;
//                 std::uint16_t interval = 0;
//                 std::uint32_t count = 0;
//                 bool use_optimized_path = false;

//                 // Logic from the switch statement in the original SE_Creat_phase0
//                 // This needs to be carefully adapted for the given chunk_index
//                 switch (chunk_index) {
//                     case 0:
//                         factor = 16 - sub_group.src_mask;
//                         interval = (1 << factor) - 1;
//                         count = 1 << sub_group.src_mask;
//                         use_optimized_path = true;
//                         break;
//                     case 1:
//                         // Original logic for case 1:
//                         // if(sub_group.selected_chunks[0] != 0){ // src_mask >= 16
//                         // This check on sub_group.selected_chunks[0] is fragile.
//                         // A more robust way might be to check sub_group.src_mask directly here
//                         // or ensure selected_chunks is populated to make this check safe.
//                         // Assuming src_mask_flag indicates if first part (dim 0) was optimized:
//                         if (sub_group.src_mask_flag == 1 && sub_group.src_mask > 16) { // src_mask >= 16 (actually >16 for this path)
//                              factor = 32 - sub_group.src_mask;
//                              interval = (1 << factor) - 1;
//                              count = 1 << (sub_group.src_mask - 16);
//                         } else { // src_mask <= 16, this is the second part of src_ip
//                              factor = 16;
//                              interval = (1 << 16) - 1;
//                              count = 1;
//                         }
//                         use_optimized_path = true;
//                         break;
//                     case 2:
//                         factor = 16 - sub_group.dst_mask;
//                         interval = (1 << factor) - 1;
//                         count = 1 << sub_group.dst_mask;
//                         use_optimized_path = true;
//                         break;
//                     case 3:
//                         // Similar to case 1, depends on dst_mask and whether dim 2 was optimized
//                         if (sub_group.dst_mask_flag == 1 && sub_group.dst_mask > 16) { // dst_mask > 16
//                             factor = 32 - sub_group.dst_mask;
//                             interval = (1 << factor) - 1;
//                             count = 1 << (sub_group.dst_mask - 16);
//                         } else { // dst_mask <= 16
//                             factor = 16;
//                             interval = (1 << 16) - 1;
//                             count = 1;
//                         }
//                         use_optimized_path = true;
//                         break;
//                     // Dimensions 4 and 5 (protocol, flags) typically don't have factor optimization in this scheme
//                 }

//                 if (use_optimized_path) {
//                     selected_flag_for_chunk = 1;
//                     if (!(interval >= 0)) { throw std::invalid_argument("interval error!"); }

//                     rfc_se_classifier.phase0_chunks[chunk_index].item_num = count;
//                     rfc_se_classifier.phase0_chunks[chunk_index].factor = factor;
//                     rfc_se_classifier.phase0_chunks[chunk_index].item.resize(count);
                    
//                     unsigned int real_n = 0;
//                     for (unsigned int n = 0; n < 65536; n = n + interval + 1) {
//                         if (real_n >= count) break; // Safety break
//                         unsigned short tempstart, tempend;
//                         int tempEqID_val;
//                         for (unsigned int m = 0; m < sub_group.sub_filter_rules.size(); m++) {
//                             tempstart = sub_group.sub_filter_rules[m].dim[chunk_index][0];
//                             tempend = sub_group.sub_filter_rules[m].dim[chunk_index][1];
//                             if (tempstart == n) { SE_Set_cbm(tempcbm, m, true, sub_group.sub_filter_rules.size(), sub_group.sub_size, sub_group.size_length); }
//                             if ((tempend + 1) == n) { SE_Set_cbm(tempcbm, m, false, sub_group.sub_filter_rules.size(), sub_group.sub_size, sub_group.size_length); }
//                         }
//                         tempEqID_val = SE_Search_CBM(temp_EqIDTable, tempcbm, sub_group.sub_size);
//                         if (tempEqID_val == -1) { tempEqID_val = SE_Add_cbm_EqIDTable(temp_EqIDTable, tempcbm, sub_group.sub_size); }
//                         rfc_se_classifier.phase0_chunks[chunk_index].item[real_n] = tempEqID_val;
//                         real_n++;
//                     }
//                 }
//                 break; // Found chunk_index in selected_chunks
//             }
//         }
//     }

//     if (selected_flag_for_chunk == 0) { // Normal processing path
//         rfc_se_classifier.phase0_chunks[chunk_index].item_num = 65536;
//         rfc_se_classifier.phase0_chunks[chunk_index].factor = 0;
//         rfc_se_classifier.phase0_chunks[chunk_index].item.resize(65536);
//         for (unsigned int n = 0; n < 65536; n++) {
//             unsigned short tempstart, tempend;
//             int tempEqID_val;
//             for (unsigned int m = 0; m < sub_group.sub_filter_rules.size(); m++) {
//                 tempstart = sub_group.sub_filter_rules[m].dim[chunk_index][0];
//                 tempend = sub_group.sub_filter_rules[m].dim[chunk_index][1];
//                 if (tempstart == n) { SE_Set_cbm(tempcbm, m, true, sub_group.sub_filter_rules.size(), sub_group.sub_size, sub_group.size_length); }
//                 if ((tempend + 1) == n) { SE_Set_cbm(tempcbm, m, false, sub_group.sub_filter_rules.size(), sub_group.sub_size, sub_group.size_length); }
//             }
//             tempEqID_val = SE_Search_CBM(temp_EqIDTable, tempcbm, sub_group.sub_size);
//             if (tempEqID_val == -1) { tempEqID_val = SE_Add_cbm_EqIDTable(temp_EqIDTable, tempcbm, sub_group.sub_size); }
//             rfc_se_classifier.phase0_chunks[chunk_index].item[n] = tempEqID_val;
//         }
//     }
// }

// // Helper function to execute a single iteration of SE_Creat_phase1 logic
// void SE_Creat_phase1_task(const SUB_GROUP& sub_group, RFC_SE_Chunks& rfc_se_classifier, int chunk_index) {
//     // This function encapsulates the logic for processing a single phase1_chunk[chunk_index].
//     // Transcribe the original SE_Creat_phase1 loop's body here for the given chunk_index.
//     se_phase0_ChunkTable* temp1;
//     se_phase0_ChunkTable* temp2;
//     se_phase0_ChunkTable* temp3;

//     switch (chunk_index) {
//         case 0:
//             temp1 = &rfc_se_classifier.phase0_chunks[0]; // src_ip_first_half
//             temp2 = &rfc_se_classifier.phase0_chunks[1]; // src_ip_second_half
//             temp3 = &rfc_se_classifier.phase0_chunks[5]; // dst_port (original was 5, check FilterRule dim mapping)
//                                                          // FilterRule.dim[4] = src_port, dim[5] = dst_port
//             break;
//         case 1:
//             temp1 = &rfc_se_classifier.phase0_chunks[2]; // dst_ip_first_half
//             temp2 = &rfc_se_classifier.phase0_chunks[3]; // dst_ip_second_half
//             temp3 = &rfc_se_classifier.phase0_chunks[4]; // src_port
//             break;
//         default:
//             // Should not happen
//             return;
//     }

//     std::uint32_t tempnum = temp1->matched_EqIDTable->item_num * temp2->matched_EqIDTable->item_num * temp3->matched_EqIDTable->item_num;
//     rfc_se_classifier.phase1_chunks[chunk_index].item_num = tempnum;
//     if (tempnum > 0) { // Avoid resizing to 0 if item_num is 0, though original code would.
//         rfc_se_classifier.phase1_chunks[chunk_index].item.resize(tempnum);
//     } else {
//         rfc_se_classifier.phase1_chunks[chunk_index].item.clear();
//     }
    
//     se_EqIDTable* temp_EqIDTable = new se_EqIDTable; // Ensure this is managed
//     temp_EqIDTable->item_num = 0;
//     temp_EqIDTable->head = nullptr;
//     temp_EqIDTable->tail = nullptr;
//     rfc_se_classifier.phase1_chunks[chunk_index].matched_EqIDTable = temp_EqIDTable;

//     std::vector<std::uint32_t> tempcbm(sub_group.sub_size);
//     // tempcbm is already initialized to 0 by vector default constructor if elements are POD.
//     // If not, initialize: for (size_t j=0; j < sub_group.sub_size; j++) tempcbm[j] = 0;

//     std::uint32_t item_idx = 0; // Renamed from 'index' to avoid conflict
//     for (se_EqID* eq1 = temp1->matched_EqIDTable->head; eq1 != nullptr; eq1 = eq1->next) {
//         for (se_EqID* eq2 = temp2->matched_EqIDTable->head; eq2 != nullptr; eq2 = eq2->next) {
//             for (se_EqID* eq3 = temp3->matched_EqIDTable->head; eq3 != nullptr; eq3 = eq3->next) {
//                 if (item_idx >= tempnum && tempnum > 0) break; // Safety break if tempnum was calculated high

//                 for (size_t j = 0; j < sub_group.sub_size; ++j) {
//                     tempcbm[j] = eq1->cbm[j] & eq2->cbm[j] & eq3->cbm[j];                   
//                 }
//                 int tempEqID_val = SE_Search_CBM(rfc_se_classifier.phase1_chunks[chunk_index].matched_EqIDTable, tempcbm, sub_group.sub_size);
//                 if (tempEqID_val == -1) { tempEqID_val = SE_Add_cbm_EqIDTable(rfc_se_classifier.phase1_chunks[chunk_index].matched_EqIDTable, tempcbm, sub_group.sub_size); }
                
//                 if (tempnum > 0) { // Only assign if item vector is not empty
//                    rfc_se_classifier.phase1_chunks[chunk_index].item[item_idx] = tempEqID_val;
//                 }
//                 item_idx++;
//             }
//             if (item_idx >= tempnum && tempnum > 0) break;
//         }
//         if (item_idx >= tempnum && tempnum > 0) break;
//     }
// }


// // SE_Creat_phasefinal 的并行任务辅助函数
// void SE_Creat_phasefinal_task_worker(
//     const std::vector<std::vector<std::uint32_t>>& cbms_p1c0, // 来自 phase1_chunks[0] 的 CBM 列表
//     const std::vector<std::vector<std::uint32_t>>& cbms_p1c1, // 来自 phase1_chunks[1] 的 CBM 列表
//     const SUB_GROUP& sub_group,
//     RFC_SE_Chunks& rfc_se_classifier,
//     size_t start_idx_p1c0, // cbms_p1c0 的起始索引
//     size_t end_idx_p1c0    // cbms_p1c0 的结束索引 (不包含)
// ) {
//     std::vector<std::uint32_t> tempcbm(sub_group.sub_size);
//     size_t num_cbms_p1c1 = cbms_p1c1.size();

//     if (num_cbms_p1c1 == 0 && !cbms_p1c0.empty() && start_idx_p1c0 < end_idx_p1c0) {
//         // 如果 cbms_p1c1 为空，但 cbms_p1c0 不为空，则所有组合结果都应为-1 (或默认)
//         // 并且 final_chunks.item 应该为空 (item_num 为 0)。
//         // 这种情况应该由 SE_Creat_phasefinal 主函数处理 item_num 和 resize。
//         // 这里假设如果 tempnum > 0, 则 num_cbms_p1c1 > 0。
//         return;
//     }


//     for (size_t i0 = start_idx_p1c0; i0 < end_idx_p1c0; ++i0) {
//         const auto& cbm_eq1 = cbms_p1c0[i0];
//         for (size_t i1 = 0; i1 < num_cbms_p1c1; ++i1) {
//             const auto& cbm_eq2 = cbms_p1c1[i1];

//             for (size_t j = 0; j < sub_group.sub_size; ++j) {
//                 // 确保 cbm_eq1 和 cbm_eq2 的大小与 sub_group.sub_size 一致，或者 tempcbm 能正确处理
//                 if (j < cbm_eq1.size() && j < cbm_eq2.size()) {
//                      tempcbm[j] = cbm_eq1[j] & cbm_eq2[j];
//                 } else {
//                     // 处理大小不匹配的情况，例如置零或抛出错误
//                     // 如果 cbm_eq1/cbm_eq2 的大小保证与 sub_group.sub_size 相同，则不需要此检查
//                     tempcbm[j] = 0; // 或者根据逻辑设定
//                 }
//             }

//             std::int16_t matched_rule_index = SE_Find_rule(tempcbm, sub_group.sub_filter_rules.size(), sub_group.sub_size, sub_group.size_length);
            
//             size_t linear_index = i0 * num_cbms_p1c1 + i1;

//             if (linear_index < rfc_se_classifier.final_chunks.item.size()) {
//                 if (matched_rule_index != -1) {
//                     rfc_se_classifier.final_chunks.item[linear_index] = sub_group.sub_filter_rules[matched_rule_index].priority;
//                 } else {
//                     rfc_se_classifier.final_chunks.item[linear_index] = -1;
//                 }
//             } else {
//                 // 索引越界，这不应该发生如果 tempnum 和 resize 正确
//                 // std::cerr << "Error: linear_index out of bounds in SE_Creat_phasefinal_task_worker" << std::endl;
//             }
//         }
//     }
// }

// void SE_Creat_phasefinal(SUB_GROUP sub_group, RFC_SE_Chunks& rfc_se_classifier){
//     std::vector<std::vector<std::uint32_t>> cbms_phase1_chunk0;
//     if (rfc_se_classifier.phase1_chunks[0].matched_EqIDTable) {
//         for (auto* eq = rfc_se_classifier.phase1_chunks[0].matched_EqIDTable->head; eq != nullptr; eq = eq->next) {
//             cbms_phase1_chunk0.push_back(eq->cbm);
//         }
//     }

//     std::vector<std::vector<std::uint32_t>> cbms_phase1_chunk1;
//     if (rfc_se_classifier.phase1_chunks[1].matched_EqIDTable) {
//         for (auto* eq = rfc_se_classifier.phase1_chunks[1].matched_EqIDTable->head; eq != nullptr; eq = eq->next) {
//             cbms_phase1_chunk1.push_back(eq->cbm);
//         }
//     }
    
//     std::uint32_t num_eq_p1c0 = static_cast<std::uint32_t>(cbms_phase1_chunk0.size());
//     std::uint32_t num_eq_p1c1 = static_cast<std::uint32_t>(cbms_phase1_chunk1.size());
//     std::uint32_t tempnum = num_eq_p1c0 * num_eq_p1c1;

//     // std::cout << "tempnum: " << tempnum << std::endl; // 原始调试输出

//     rfc_se_classifier.final_chunks.item_num = tempnum;
//     if (tempnum > 0) {
//         rfc_se_classifier.final_chunks.item.resize(tempnum);
//     } else {
//         rfc_se_classifier.final_chunks.item.clear();
//         return; // 没有工作可做
//     }
    
//     // 如果任何一个 CBM 列表为空，则乘积为0，上面已处理
//     if (num_eq_p1c0 == 0 || num_eq_p1c1 == 0) {
//         return;
//     }

//     unsigned int num_threads_to_use = std::thread::hardware_concurrency();
//     if (num_threads_to_use == 0) num_threads_to_use = 1; // 备用
//     // 如果外层循环的迭代次数少于线程数，则减少线程数
//     if (num_eq_p1c0 < num_threads_to_use) {
//         num_threads_to_use = num_eq_p1c0;
//     }
//     if (num_threads_to_use == 0 && num_eq_p1c0 > 0) num_threads_to_use = 1; // 确保至少一个线程（如果num_eq_p1c0 > 0）


//     std::vector<std::future<void>> final_futures;
//     if (num_threads_to_use > 0) { // 仅当有线程可用时才启动
//         size_t chunk_size_outer = (num_eq_p1c0 + num_threads_to_use - 1) / num_threads_to_use;

//         for (unsigned int i = 0; i < num_threads_to_use; ++i) {
//             size_t start_idx = i * chunk_size_outer;
//             size_t end_idx = std::min(start_idx + chunk_size_outer, (size_t)num_eq_p1c0);

//             if (start_idx < end_idx) { // 确保此线程有工作
//                 final_futures.push_back(std::async(std::launch::async, SE_Creat_phasefinal_task_worker,
//                                                    std::cref(cbms_phase1_chunk0),
//                                                    std::cref(cbms_phase1_chunk1),
//                                                    std::cref(sub_group),
//                                                    std::ref(rfc_se_classifier),
//                                                    start_idx, end_idx));
//             }
//         }

//         for (auto& fut : final_futures) {
//             if (fut.valid()) { // 检查 future 是否有效
//                 fut.get();
//             }
//         }
//     } else if (num_eq_p1c0 > 0) { // 如果没有线程（理论上不应发生，除非 num_threads_to_use 计算为0），但有工作，则串行执行
//          SE_Creat_phasefinal_task_worker(cbms_phase1_chunk0, cbms_phase1_chunk1, sub_group, rfc_se_classifier, 0, num_eq_p1c0);
//     }
// }


// // RFC_SE_Chunks Improved_Building_Classifier(const Filter_Division filter_division, const int flag){
// //     SUB_GROUP sub_group;

// //     switch (flag)
// //     {
// //         case 0:
// //         // L_L
// //             sub_group.src_mask = filter_division.selected_src_mask;
// //             sub_group.dst_mask = filter_division.selected_lower_dst_mask;
// //             sub_group.src_mask_flag = 1;
// //             sub_group.dst_mask_flag = 1;
// //             sub_group.sub_filter_rules = filter_division.Sub_L_L;
// //             sub_group.sub_size = CeilDivide(filter_division.L_L_accumulation,sub_group.size_length);
// //             if (sub_group.src_mask <= 16){
// //                 sub_group.selected_chunks.push_back(0);
// //                 sub_group.selected_chunks.push_back(1);
// //             }
// //             else{
// //                 sub_group.selected_chunks.push_back(1);
// //             }

// //             if (sub_group.dst_mask <= 16){
// //                 sub_group.selected_chunks.push_back(2);
// //                 sub_group.selected_chunks.push_back(3);
// //             }
// //             else{
// //                 sub_group.selected_chunks.push_back(3);
// //             }    
        
// //             break;

// //         case 1:
// //             // L_H
// //             sub_group.src_mask = filter_division.selected_src_mask;
// //             sub_group.dst_mask = filter_division.selected_lower_dst_mask;
// //             sub_group.src_mask_flag = 1;
// //             sub_group.dst_mask_flag = 0;
// //             sub_group.sub_filter_rules = filter_division.Sub_L_H;
// //             sub_group.sub_size = CeilDivide(filter_division.L_H_accumulation,sub_group.size_length);
// //             if (sub_group.src_mask <= 16){
// //                 sub_group.selected_chunks.push_back(0);
// //                 sub_group.selected_chunks.push_back(1);
// //             }
// //             else{
// //                 sub_group.selected_chunks.push_back(1);
// //             }

// //             break;
    
// //         case 2:
// //             // H_L
// //             sub_group.src_mask = filter_division.selected_src_mask;
// //             sub_group.dst_mask = filter_division.selected_higher_dst_mask;
// //             sub_group.src_mask_flag = 0;
// //             sub_group.dst_mask_flag = 1;
// //             sub_group.sub_filter_rules = filter_division.Sub_H_L;
// //             sub_group.sub_size = CeilDivide(filter_division.H_L_accumulation,sub_group.size_length); 
// //             if (sub_group.dst_mask <= 16){
// //                 sub_group.selected_chunks.push_back(2);
// //                 sub_group.selected_chunks.push_back(3);
// //             }
// //             else{
// //                 sub_group.selected_chunks.push_back(3);
// //             }  
        
// //             break;

// //         case 3:
// //             // H_H
// //             sub_group.src_mask = filter_division.selected_src_mask;
// //             sub_group.dst_mask = filter_division.selected_higher_dst_mask;
// //             sub_group.src_mask_flag = 0;
// //             sub_group.dst_mask_flag = 1;
// //             sub_group.sub_filter_rules = filter_division.Sub_H_H;
// //             sub_group.sub_size = CeilDivide(filter_division.H_H_accumulation,sub_group.size_length);
// //             break;
// //     }


// //     auto start_time = std::chrono::high_resolution_clock::now();
// //     RFC_SE_Chunks rfc_se_classifier;

// //     //building rfc_se phase0
// //     SE_Creat_phase0(sub_group, rfc_se_classifier);

// //     //building rfc_se phase1
// //     SE_Creat_phase1(sub_group, rfc_se_classifier);

// //     //building rfc_se finalphase
// //     SE_Creat_phasefinal(sub_group, rfc_se_classifier);

// //     auto end_time = std::chrono::high_resolution_clock::now(); 
// //     auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    
// //     return rfc_se_classifier;

// RFC_SE_Chunks Improved_Building_Classifier(const Filter_Division& filter_division, const int flag) { // Pass filter_division by const reference
//     SUB_GROUP sub_group;
//     // Initialize sub_group based on flag and filter_division
//     // This logic needs to be exactly as it was.
//     // Example for case 0 (L_L):
//     switch (flag)
//     {
//         case 0: // L_L
//             sub_group.src_mask = filter_division.selected_src_mask;
//             sub_group.dst_mask = filter_division.selected_lower_dst_mask;
//             sub_group.src_mask_flag = 1; // Indicates src_mask related dimensions might be optimized
//             sub_group.dst_mask_flag = 1; // Indicates dst_mask related dimensions might be optimized
//             sub_group.sub_filter_rules = filter_division.Sub_L_L;
//             if (filter_division.L_L_accumulation > 0) { // Avoid division by zero if accumulation is 0
//                 sub_group.sub_size = CeilDivide(filter_division.L_L_accumulation, sub_group.size_length);
//             } else {
//                 sub_group.sub_size = 0;
//             }
//             // Populate selected_chunks for factor optimization in SE_Creat_phase0_task
//             // Dims 0,1 are for src_ip; Dims 2,3 are for dst_ip
//             if (sub_group.src_mask <= 16) {
//                 sub_group.selected_chunks.push_back(0); // First half of src_ip
//                 sub_group.selected_chunks.push_back(1); // Second half of src_ip
//             } else { // src_mask > 16
//                 sub_group.selected_chunks.push_back(1); // Only second half of src_ip needs specific factor
//             }
//             if (sub_group.dst_mask <= 16) {
//                 sub_group.selected_chunks.push_back(2); // First half of dst_ip
//                 sub_group.selected_chunks.push_back(3); // Second half of dst_ip
//             } else { // dst_mask > 16
//                 sub_group.selected_chunks.push_back(3); // Only second half of dst_ip needs specific factor
//             }
//             break;
//         case 1: // L_H
//             sub_group.src_mask = filter_division.selected_src_mask;
//             sub_group.dst_mask = filter_division.selected_lower_dst_mask; // Note: Original uses selected_lower_dst_mask for L_H
//             sub_group.src_mask_flag = 1;
//             sub_group.dst_mask_flag = 0; // dst_mask is "High" (long), so less optimization expected via selected_chunks for dst
//             sub_group.sub_filter_rules = filter_division.Sub_L_H;
//             if (filter_division.L_H_accumulation > 0) {
//                 sub_group.sub_size = CeilDivide(filter_division.L_H_accumulation, sub_group.size_length);
//             } else {
//                 sub_group.sub_size = 0;
//             }
//             if (sub_group.src_mask <= 16) {
//                 sub_group.selected_chunks.push_back(0);
//                 sub_group.selected_chunks.push_back(1);
//             } else {
//                 sub_group.selected_chunks.push_back(1);
//             }
//             // For L_H, dst_mask is > selected_lower_dst_mask.
//             // The original code for case 1 in Improved_Building_Classifier only adds selected_chunks for src_mask.
//             // This implies factor optimization is primarily for src_ip in L_H.
//             break;
//         case 2: // H_L
//             sub_group.src_mask = filter_division.selected_src_mask; // Note: Original uses selected_src_mask for H_L
//             sub_group.dst_mask = filter_division.selected_higher_dst_mask;
//             sub_group.src_mask_flag = 0; 
//             sub_group.dst_mask_flag = 1;
//             sub_group.sub_filter_rules = filter_division.Sub_H_L;
//             if (filter_division.H_L_accumulation > 0) {
//                 sub_group.sub_size = CeilDivide(filter_division.H_L_accumulation, sub_group.size_length);
//             } else {
//                 sub_group.sub_size = 0;
//             }
//             if (sub_group.dst_mask <= 16) {
//                 sub_group.selected_chunks.push_back(2);
//                 sub_group.selected_chunks.push_back(3);
//             } else {
//                 sub_group.selected_chunks.push_back(3);
//             }
//             break;
//         case 3: // H_H
//             sub_group.src_mask = filter_division.selected_src_mask;
//             sub_group.dst_mask = filter_division.selected_higher_dst_mask;
//             sub_group.src_mask_flag = 0;
//             sub_group.dst_mask_flag = 0; // Both are "High" (long masks)
//             sub_group.sub_filter_rules = filter_division.Sub_H_H;
//             if (filter_division.H_H_accumulation > 0) {
//                 sub_group.sub_size = CeilDivide(filter_division.H_H_accumulation, sub_group.size_length);
//             } else {
//                 sub_group.sub_size = 0;
//             }
//             // For H_H, typically no factor optimization via selected_chunks for IP parts.
//             break;
//     }

//     auto start_time = std::chrono::high_resolution_clock::now();
//     RFC_SE_Chunks rfc_se_classifier;

//     if (sub_group.sub_size > 0) { // Only proceed if there are rules to process
//         // Parallel building of rfc_se phase0
//         std::vector<std::future<void>> phase0_futures;
//         for (int i = 0; i < 6; ++i) { // For all 6 dimensions/chunks
//             phase0_futures.push_back(std::async(std::launch::async, SE_Creat_phase0_task, std::cref(sub_group), std::ref(rfc_se_classifier), i));
//         }
//         for (auto& fut : phase0_futures) {
//             if (fut.valid()) fut.get(); // Wait for completion and propagate exceptions
//         }

//         // Parallel building of rfc_se phase1
//         std::vector<std::future<void>> phase1_futures;
//         for (int i = 0; i < 2; ++i) { // For the 2 phase1 chunks
//             phase1_futures.push_back(std::async(std::launch::async, SE_Creat_phase1_task, std::cref(sub_group), std::ref(rfc_se_classifier), i));
//         }
//         for (auto& fut : phase1_futures) {
//             if (fut.valid()) fut.get();
//         }

//         // Sequential building of rfc_se finalphase (now internally parallelized)
//         SE_Creat_phasefinal(sub_group, rfc_se_classifier);
//     } else {
//         // Handle cases with no rules (e.g. L_L_accumulation was 0)
//         // Initialize rfc_se_classifier to a valid empty state if necessary,
//         // or ensure downstream code handles empty/default-constructed classifier chunks.
//         // For example, ensure item_num is 0 for all chunks.
//         for(int i=0; i<6; ++i) rfc_se_classifier.phase0_chunks[i].item_num = 0;
//         for(int i=0; i<2; ++i) rfc_se_classifier.phase1_chunks[i].item_num = 0;
//         rfc_se_classifier.final_chunks.item_num = 0;
//     }


//     auto end_time = std::chrono::high_resolution_clock::now(); 
//     auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
//     // std::cout << "Flag " << flag << " Building time: " << elapsed_time << " microseconds" << std::endl;
    
//     return rfc_se_classifier;
// }

// //--------------------------------
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include "Hardware_ImprovedClassifierBuilding.h" // Should contain definitions for RFC_SE_Chunks, SUB_GROUP, Filter_Division, se_EqIDTable, etc.
#include <iostream>
#include <chrono>
#include <numeric>      // For std::iota (if used), std::accumulate (if used)
#include <algorithm>    // For std::min, std::fill
#include <vector>       // Required for std::vector
#include <future>       // Required for std::async, std::future
#include <functional>   // Required for std::ref, std::cref
#include <thread>       // For std::thread::hardware_concurrency
#include <stdexcept>    // For std::invalid_argument
#include <atomic>       // For std::atomic if fine-grained counters were used (not in this final version for phase0/1 tasks)
#include <mutex>        // For std::mutex, std::lock_guard (used in SE_Search_CBM, SE_Add_cbm_EqIDTable)

// It's good practice to define structs like se_EqIDTable in the header.
// For completeness, if it were here, it would look like this (including mutex and destructor):
// struct se_EqID {
//     std::vector<std::uint32_t> cbm;
//     std::uint32_t eqid;
//     se_EqID* next;

//     se_EqID() : eqid(0), next(nullptr) {} // Basic constructor
// };

// struct se_EqIDTable {
//     se_EqID* head;
//     se_EqID* tail;
//     std::uint32_t item_num;
//     std::mutex mtx; // Mutex for thread-safe access

//     se_EqIDTable() : head(nullptr), tail(nullptr), item_num(0) {}

//     ~se_EqIDTable() {
//         std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety during destruction if table could be accessed
//         se_EqID* current = head;
//         while (current != nullptr) {
//             se_EqID* next_node = current->next;
//             delete current;
//             current = next_node;
//         }
//         head = nullptr;
//         tail = nullptr;
//         item_num = 0;
//     }
// };


// define the ceiling division argument
int CeilDivide(int a, int b){
    if(b == 0){
        throw std::invalid_argument("Division by zero is not allowed");
    }
    return (a + b -1) / b;
}

// Function to set a bit in the CBM
void SE_Set_cbm(std::vector<std::uint32_t>& tempcbm, unsigned int filter_index, bool value, unsigned int filter_num, std::uint16_t sub_size, std::uint16_t size_length){
    if (size_length == 0) {
        // Avoid division by zero if size_length is 0
        // This case should ideally be handled by input validation earlier.
        // Or throw an exception: throw std::invalid_argument("size_length cannot be zero in SE_Set_cbm");
        return; 
    }
    unsigned int group,pos;
    unsigned int k =1;

    // Calculate the group and position in the CBM
    // Ensure filter_num > 0 and filter_index < filter_num
    if (filter_num == 0) return; // Or throw
    group =  sub_size - 1 - (filter_num - 1 - filter_index) / size_length;
    pos = (filter_num -1 - filter_index) % size_length;
    
    if (group >= tempcbm.size()) {
        // Index out of bounds, this indicates an issue with sub_size or calculation
        // std::cerr << "Error: CBM group index out of bounds." << std::endl;
        return; // Or throw
    }

    k <<= pos;
    
    if (value == true){
        tempcbm[group] |= k;
    }
    else{
        k = ~k;
        tempcbm[group] &= k;
    }
}

// Function to compare two CBMs
bool SE_Compare_CBM(const std::vector<std::uint32_t>& cbm_a, const std::vector<std::uint32_t>& cbm_b, std::uint16_t sub_size){
    if (cbm_a.size() != sub_size || cbm_b.size() != sub_size) {
        // This check is important. If sizes can mismatch, log or handle.
        // std::cerr << "[DEBUG] SE_Compare_CBM size mismatch: a=" << cbm_a.size() << ", b=" << cbm_b.size() << ", sub_size=" << sub_size << std::endl;
        return false; // Or throw, depending on how critical this is.
                      // Assuming sub_size is the authoritative size.
    }
    if (sub_size == 0 && cbm_a.empty() && cbm_b.empty()) return true; // Two empty CBMs are equal
    if (sub_size == 0) return false; // If sub_size is 0 but CBMs are not empty (or vice-versa based on above check)

    for (size_t i = 0; i < sub_size; ++i) {
        if (cbm_a[i] != cbm_b[i]) {
            return false;
        }
    }
    return true;
}

// Function to search for a CBM in an EqIDTable (Thread-safe)
int SE_Search_CBM(se_EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>& tempcbm,std::uint16_t sub_size){
    if (!temp_EqIDTable) return -1;
    std::lock_guard<std::mutex> lock(temp_EqIDTable->mtx); // Acquire lock

    se_EqID* current_EqID = temp_EqIDTable->head;
    while (current_EqID != nullptr){
        // Ensure current_EqID->cbm is valid and has expected size before comparing
        if (SE_Compare_CBM(current_EqID->cbm, tempcbm, sub_size)) {
            return current_EqID->eqid;
        }
        current_EqID = current_EqID->next;
    }
    return -1;
}

// Function to add a CBM to an EqIDTable (Thread-safe)
int SE_Add_cbm_EqIDTable(se_EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>& tempcbm, std::uint16_t sub_size){
    if (!temp_EqIDTable) return -1;
    
    se_EqID* new_EqID = new (std::nothrow) se_EqID;
    if (new_EqID == nullptr) {
        std::cerr << "[ERROR] Memory allocation failed for new_EqID in SE_Add_cbm_EqIDTable" << std::endl;
        return -1; // Indicate failure
    }

    // It's crucial that new_EqID->cbm is correctly sized *before* copying.
    // And tempcbm should also have the correct size.
    if (tempcbm.size() != sub_size) {
        std::cerr << "[ERROR] tempcbm size mismatch in SE_Add_cbm_EqIDTable. Expected: " << sub_size << ", Got: " << tempcbm.size() << std::endl;
        delete new_EqID;
        return -1;
    }
    new_EqID->cbm.resize(sub_size); // Ensure new_EqID's cbm is the correct size
    for (size_t i = 0; i < sub_size; ++i) {
        new_EqID->cbm[i] = tempcbm[i];
    }
    new_EqID->next = nullptr;
    // eqid will be set under lock

    std::lock_guard<std::mutex> lock(temp_EqIDTable->mtx); // Acquire lock for modifying the list

    // Re-check if another thread added this CBM while we were preparing new_EqID (before acquiring lock)
    // This is a common pattern for "put-if-absent" but makes logic more complex.
    // For simplicity here, we assume the search was done recently enough or that duplicates are handled by search logic.
    // If strict "add only if not present" is needed atomically, search must also be under the same lock or a more complex scheme used.
    // The current SE_Search_CBM + SE_Add_cbm_EqIDTable sequence is not atomic.
    // However, if SE_Search_CBM is always called before, and it returns -1, then adding is fine.

    if (temp_EqIDTable->head == nullptr) {
        new_EqID->eqid = 0;
        temp_EqIDTable->head = new_EqID;
        temp_EqIDTable->tail = new_EqID;
        temp_EqIDTable->item_num = 1;
    } else {
        new_EqID->eqid = temp_EqIDTable->item_num; // eqid is the current count, making it 0-indexed for next item
        temp_EqIDTable->tail->next = new_EqID;
        temp_EqIDTable->tail = new_EqID;
        temp_EqIDTable->item_num++;
    }
    return new_EqID->eqid; // Return the eqid of the newly added (or found if re-checked) item
}

// Function to find a rule in the CBM
int SE_Find_rule(const std::vector<std::uint32_t>& tempcbm, unsigned int filter_num, std::uint16_t sub_size, std::uint16_t size_length){
    if (size_length == 0 || tempcbm.empty() || sub_size == 0) {
        return -1; // Invalid input or empty CBM
    }
    if (tempcbm.size() != sub_size) {
        // std::cerr << "Error: tempcbm size does not match sub_size in SE_Find_rule." << std::endl;
        return -1;
    }


    // The original logic for SE_Find_rule seems to find the *first* set bit and map it back to a rule index.
    // Let's re-verify the indexing logic.
    // (filter_num - 1 - filter_index) gives an effective index from right-to-left, 0-based.
    // group = sub_size - 1 - effective_index / size_length;
    // pos   = effective_index % size_length;
    // To reverse this: effective_index = (sub_size - 1 - group) * size_length + pos (if pos is from right)
    // Or, if pos is from left (as in k <<= pos): effective_index = (sub_size - 1 - group) * size_length + (size_length - 1 - pos_from_left)
    // filter_index = filter_num - 1 - effective_index;

    for (std::uint16_t k_group = 0; k_group < sub_size; ++k_group) { // Iterate through each uint32_t in CBM
        if (tempcbm[k_group] == 0) continue; // Skip if no bits are set in this group

        for (std::uint16_t pos_in_group = 0; pos_in_group < size_length; ++pos_in_group) { // Iterate through bits in the uint32_t
            if ((tempcbm[k_group] >> pos_in_group) & 1) {
                // Bit at pos_in_group (from right, 0-indexed) in group k_group is set.
                // Map this back to the original filter_index.
                // effective_filter_index_from_right = (sub_size - 1 - k_group) * size_length + pos_in_group;
                // This is the (filter_num - 1 - filter_index) value.
                // So, filter_index = filter_num - 1 - effective_filter_index_from_right.
                unsigned int effective_index_from_right = (sub_size - 1 - k_group) * size_length + pos_in_group;
                if (filter_num > 0 && effective_index_from_right < filter_num) {
                     return filter_num - 1 - effective_index_from_right;
                } else {
                    // This implies an invalid mapping, possibly due to filter_num being too small
                    // or CBM containing bits beyond the valid filter_num range.
                    // std::cerr << "Warning: SE_Find_rule calculated invalid filter_index." << std::endl;
                    // Continue searching, or return -1 if this is an error state.
                }
            }
        }
    }
    return -1; // No rule found (no bit set, or invalid mapping)
}


// Task for creating a single phase0 chunk (internal logic is serial)
void SE_Creat_phase0_task(const SUB_GROUP& sub_group, RFC_SE_Chunks& rfc_se_classifier, int chunk_index) {
    if (chunk_index < 0 || chunk_index >= 6) {
        throw std::out_of_range("chunk_index out of bounds in SE_Creat_phase0_task");
    }

    std::vector<std::uint32_t> tempcbm(sub_group.sub_size);
    if (sub_group.sub_size > 0) {
        std::fill(tempcbm.begin(), tempcbm.end(), 0);
    }

    // Each task creates its own EqIDTable
    se_EqIDTable* local_EqIDTable = new se_EqIDTable();
    rfc_se_classifier.phase0_chunks[chunk_index].matched_EqIDTable = local_EqIDTable; // Ownership transferred to RFC_SE_Chunks

    bool use_optimized_path = false;
    std::uint16_t factor_opt = 0;
    std::uint16_t interval_opt = 0; // For optimized path, this is (1 << factor_opt) - 1
    std::uint32_t count_opt = 0;    // Number of items in the chunk for optimized path

    // Determine if optimized path applies for this chunk_index
    bool chunk_is_selected_for_optimization = false;
    if (!sub_group.selected_chunks.empty()) {
        for (const auto sub_item_idx : sub_group.selected_chunks) {
            if (chunk_index == sub_item_idx) {
                chunk_is_selected_for_optimization = true;
                break;
            }
        }
    }

    if (chunk_is_selected_for_optimization) {
        use_optimized_path = true;
        switch (chunk_index) {
            case 0: // src_ip_first_half
                factor_opt = (sub_group.src_mask < 16) ? (16 - sub_group.src_mask) : 0; // if src_mask >=16, factor is 0 for this part
                count_opt = (sub_group.src_mask < 16) ? (1 << sub_group.src_mask) : ((sub_group.src_mask == 16) ? (1 << 16) : 1); // Special handling if src_mask is exactly 16 or > 16 for this chunk
                if (sub_group.src_mask > 16) factor_opt = 0; // No factor if mask covers more than this 16-bit chunk
                break;
            case 1: // src_ip_second_half
                if (sub_group.src_mask <= 16) { // Entire mask fits in first 16 bits or less
                    factor_opt = 16; count_opt = 1; // This chunk is effectively a single point
                } else { // src_mask > 16
                    factor_opt = 32 - sub_group.src_mask;
                    count_opt = 1 << (sub_group.src_mask - 16);
                }
                break;
            case 2: // dst_ip_first_half
                factor_opt = (sub_group.dst_mask < 16) ? (16 - sub_group.dst_mask) : 0;
                count_opt = (sub_group.dst_mask < 16) ? (1 << sub_group.dst_mask) : ((sub_group.dst_mask == 16) ? (1 << 16) : 1);
                if (sub_group.dst_mask > 16) factor_opt = 0;
                break;
            case 3: // dst_ip_second_half
                if (sub_group.dst_mask <= 16) {
                    factor_opt = 16; count_opt = 1;
                } else {
                    factor_opt = 32 - sub_group.dst_mask;
                    count_opt = 1 << (sub_group.dst_mask - 16);
                }
                break;
            case 4: // src_port
            case 5: // dst_port
                // Typically, ports are not factor-optimized in this manner, they cover full range or specific points.
                // Assuming no factor optimization for ports, they behave like normal path.
                use_optimized_path = false; // Fall through to normal path logic
                break;
        }
        if (use_optimized_path) {
            interval_opt = (factor_opt > 0 && factor_opt <=16) ? ((1 << factor_opt) - 1) : 0; // if factor is 0, interval is 0 (step 1)
             if (count_opt == 0 && factor_opt > 0) count_opt = 1; // if factor implies reduction but count is 0, it should be at least 1.
                                                              // This logic needs to be robust based on mask interpretation.
                                                              // If count_opt is 1, interval should effectively make it iterate once.
            if (count_opt == 1 && factor_opt > 0 && factor_opt < 16) interval_opt = 65535; // Iterate once for the whole range if count is 1 due to optimization

            rfc_se_classifier.phase0_chunks[chunk_index].item_num = count_opt;
            rfc_se_classifier.phase0_chunks[chunk_index].factor = factor_opt;
            if (count_opt > 0) {
                rfc_se_classifier.phase0_chunks[chunk_index].item.resize(count_opt);
            } else {
                 rfc_se_classifier.phase0_chunks[chunk_index].item.clear(); // Should not happen if count_opt is derived correctly
            }
        }
    }

    if (!use_optimized_path) { // Normal processing path (or fallback for ports/unselected chunks)
        rfc_se_classifier.phase0_chunks[chunk_index].item_num = 65536;
        rfc_se_classifier.phase0_chunks[chunk_index].factor = 0;
        rfc_se_classifier.phase0_chunks[chunk_index].item.resize(65536);
        count_opt = 65536; // Loop limit
        interval_opt = 0;  // Step is 1
    }

    if (sub_group.sub_filter_rules.empty() && count_opt > 0) { // No rules, fill items with -1 (or default EqID for empty CBM)
        int default_eqid_for_empty_cbm = -1;
        if (sub_group.sub_size > 0) { // Only try to get EqID if CBM can be formed
            std::vector<std::uint32_t> empty_cbm(sub_group.sub_size, 0);
            default_eqid_for_empty_cbm = SE_Search_CBM(local_EqIDTable, empty_cbm, sub_group.sub_size);
            if (default_eqid_for_empty_cbm == -1) {
                default_eqid_for_empty_cbm = SE_Add_cbm_EqIDTable(local_EqIDTable, empty_cbm, sub_group.sub_size);
            }
        }
        std::fill(rfc_se_classifier.phase0_chunks[chunk_index].item.begin(), rfc_se_classifier.phase0_chunks[chunk_index].item.end(), default_eqid_for_empty_cbm);
        return;
    }


    unsigned int current_item_array_idx = 0;
    // Loop from 0 to 65535, stepping by (interval_opt + 1)
    for (unsigned int n_value = 0; n_value < 65536; n_value += (interval_opt + 1)) {
        if (current_item_array_idx >= count_opt && use_optimized_path) break; // Filled all required items for optimized path
        if (n_value >= count_opt && !use_optimized_path) break; // Standard path check (count_opt is 65536 here)


        // CBM state is cumulative. For each n_value, update based on rules.
        for (unsigned int rule_idx = 0; rule_idx < sub_group.sub_filter_rules.size(); ++rule_idx) {
            // Ensure rule_idx is valid for sub_filter_rules
            // Ensure chunk_index is valid for dim
            if (chunk_index < 0 || chunk_index >= 6) continue;

            unsigned short tempstart = sub_group.sub_filter_rules[rule_idx].dim[chunk_index][0];
            unsigned short tempend = sub_group.sub_filter_rules[rule_idx].dim[chunk_index][1];

            if (tempstart == n_value) {
                SE_Set_cbm(tempcbm, rule_idx, true, sub_group.sub_filter_rules.size(), sub_group.sub_size, sub_group.size_length);
            }
            if ((tempend + 1) == n_value) { // Point after range end
                SE_Set_cbm(tempcbm, rule_idx, false, sub_group.sub_filter_rules.size(), sub_group.sub_size, sub_group.size_length);
            }
        }

        int tempEqID_val = SE_Search_CBM(local_EqIDTable, tempcbm, sub_group.sub_size);
        if (tempEqID_val == -1) {
            tempEqID_val = SE_Add_cbm_EqIDTable(local_EqIDTable, tempcbm, sub_group.sub_size);
        }
        
        if (use_optimized_path) {
            if (current_item_array_idx < rfc_se_classifier.phase0_chunks[chunk_index].item.size()) {
                 rfc_se_classifier.phase0_chunks[chunk_index].item[current_item_array_idx] = tempEqID_val;
            }
            current_item_array_idx++;
        } else { // Normal path, n_value is the direct index
            if (n_value < rfc_se_classifier.phase0_chunks[chunk_index].item.size()){
                rfc_se_classifier.phase0_chunks[chunk_index].item[n_value] = tempEqID_val;
            }
        }
         if (interval_opt == 0 && n_value == 65535) break; // Ensure loop terminates for step 1
    }
}

// Task for creating a single phase1 chunk (internal logic is serial)
void SE_Creat_phase1_task(const SUB_GROUP& sub_group, RFC_SE_Chunks& rfc_se_classifier, int chunk_index) {
    if (chunk_index < 0 || chunk_index >= 2) {
        throw std::out_of_range("chunk_index out of bounds in SE_Creat_phase1_task");
    }

    se_phase0_ChunkTable* p0_chunk1 = nullptr;
    se_phase0_ChunkTable* p0_chunk2 = nullptr;
    se_phase0_ChunkTable* p0_chunk3 = nullptr;

    switch (chunk_index) {
        case 0:
            p0_chunk1 = &rfc_se_classifier.phase0_chunks[0]; // src_ip_first_half
            p0_chunk2 = &rfc_se_classifier.phase0_chunks[1]; // src_ip_second_half
            p0_chunk3 = &rfc_se_classifier.phase0_chunks[5]; // dst_port (dim index 5)
            break;
        case 1:
            p0_chunk1 = &rfc_se_classifier.phase0_chunks[2]; // dst_ip_first_half
            p0_chunk2 = &rfc_se_classifier.phase0_chunks[3]; // dst_ip_second_half
            p0_chunk3 = &rfc_se_classifier.phase0_chunks[4]; // src_port (dim index 4)
            break;
    }

    // Ensure dependent EqIDTables are not null
    if (!p0_chunk1 || !p0_chunk1->matched_EqIDTable ||
        !p0_chunk2 || !p0_chunk2->matched_EqIDTable ||
        !p0_chunk3 || !p0_chunk3->matched_EqIDTable) {
        // std::cerr << "Warning: Missing dependent EqIDTable in SE_Creat_phase1_task for chunk " << chunk_index << std::endl;
        rfc_se_classifier.phase1_chunks[chunk_index].item_num = 0;
        rfc_se_classifier.phase1_chunks[chunk_index].item.clear();
        if (!rfc_se_classifier.phase1_chunks[chunk_index].matched_EqIDTable) {
             rfc_se_classifier.phase1_chunks[chunk_index].matched_EqIDTable = new se_EqIDTable(); // Ensure it exists
        }
        return;
    }
    
    std::uint32_t num_eq_c1 = p0_chunk1->matched_EqIDTable->item_num;
    std::uint32_t num_eq_c2 = p0_chunk2->matched_EqIDTable->item_num;
    std::uint32_t num_eq_c3 = p0_chunk3->matched_EqIDTable->item_num;
    std::uint32_t total_items = num_eq_c1 * num_eq_c2 * num_eq_c3;

    rfc_se_classifier.phase1_chunks[chunk_index].item_num = total_items;
    if (total_items > 0) {
        rfc_se_classifier.phase1_chunks[chunk_index].item.resize(total_items);
    } else {
        rfc_se_classifier.phase1_chunks[chunk_index].item.clear();
    }
    
    // Each task creates its own EqIDTable for this phase1 chunk
    se_EqIDTable* local_EqIDTable_p1 = new se_EqIDTable();
    rfc_se_classifier.phase1_chunks[chunk_index].matched_EqIDTable = local_EqIDTable_p1;

    if (total_items == 0 || sub_group.sub_size == 0) { // No items to compute or CBM is empty
        return;
    }

    std::vector<std::uint32_t> tempcbm(sub_group.sub_size, 0);
    std::uint32_t current_item_array_idx = 0;

    for (se_EqID* eq1 = p0_chunk1->matched_EqIDTable->head; eq1 != nullptr; eq1 = eq1->next) {
        if (eq1->cbm.size() != sub_group.sub_size) continue; // Skip if CBM size mismatch
        for (se_EqID* eq2 = p0_chunk2->matched_EqIDTable->head; eq2 != nullptr; eq2 = eq2->next) {
            if (eq2->cbm.size() != sub_group.sub_size) continue;
            for (se_EqID* eq3 = p0_chunk3->matched_EqIDTable->head; eq3 != nullptr; eq3 = eq3->next) {
                if (eq3->cbm.size() != sub_group.sub_size) continue;

                if (current_item_array_idx >= total_items) break; 

                for (size_t j = 0; j < sub_group.sub_size; ++j) {
                    tempcbm[j] = eq1->cbm[j] & eq2->cbm[j] & eq3->cbm[j];                   
                }

                int tempEqID_val = SE_Search_CBM(local_EqIDTable_p1, tempcbm, sub_group.sub_size);
                if (tempEqID_val == -1) { 
                    tempEqID_val = SE_Add_cbm_EqIDTable(local_EqIDTable_p1, tempcbm, sub_group.sub_size); 
                }
                
                if (current_item_array_idx < rfc_se_classifier.phase1_chunks[chunk_index].item.size()){
                   rfc_se_classifier.phase1_chunks[chunk_index].item[current_item_array_idx] = tempEqID_val;
                }
                current_item_array_idx++;
            }
            if (current_item_array_idx >= total_items) break;
        }
        if (current_item_array_idx >= total_items) break;
    }
}

// Worker for SE_Creat_phasefinal (internally parallelized)
void SE_Creat_phasefinal_task_worker(
    const std::vector<std::vector<std::uint32_t>>& cbms_p1c0,
    const std::vector<std::vector<std::uint32_t>>& cbms_p1c1,
    const SUB_GROUP& sub_group,
    RFC_SE_Chunks& rfc_se_classifier, // Allow modification
    size_t start_idx_p1c0,
    size_t end_idx_p1c0
) {
    if (sub_group.sub_size == 0) return; // No CBM to form

    std::vector<std::uint32_t> tempcbm(sub_group.sub_size);
    size_t num_cbms_p1c1 = cbms_p1c1.size();

    if (num_cbms_p1c1 == 0 && !cbms_p1c0.empty() && start_idx_p1c0 < end_idx_p1c0) {
        // If cbms_p1c1 is empty, all combinations result in an empty intersection for those rules.
        // The final_chunks.item should be filled with -1 or default.
        // This case is handled by the main SE_Creat_phasefinal if total items is 0.
        // If total items > 0 but one list is empty, it implies an issue.
        // For safety, if num_cbms_p1c1 is 0, this loop won't run.
        return;
    }

    for (size_t i0 = start_idx_p1c0; i0 < end_idx_p1c0; ++i0) {
        const auto& cbm_eq1 = cbms_p1c0[i0];
        if (cbm_eq1.size() != sub_group.sub_size) continue; // Skip if CBM size mismatch

        for (size_t i1 = 0; i1 < num_cbms_p1c1; ++i1) {
            const auto& cbm_eq2 = cbms_p1c1[i1];
            if (cbm_eq2.size() != sub_group.sub_size) continue;

            for (size_t j = 0; j < sub_group.sub_size; ++j) {
                tempcbm[j] = cbm_eq1[j] & cbm_eq2[j];
            }

            std::int16_t matched_rule_original_index = SE_Find_rule(tempcbm, sub_group.sub_filter_rules.size(), sub_group.sub_size, sub_group.size_length);
            
            size_t linear_index = i0 * num_cbms_p1c1 + i1;

            if (linear_index < rfc_se_classifier.final_chunks.item.size()) {
                if (matched_rule_original_index != -1 && static_cast<size_t>(matched_rule_original_index) < sub_group.sub_filter_rules.size()) {
                    rfc_se_classifier.final_chunks.item[linear_index] = sub_group.sub_filter_rules[matched_rule_original_index].priority;
                } else {
                    rfc_se_classifier.final_chunks.item[linear_index] = -1; // No rule or invalid index
                }
            }
        }
    }
}

// Creates the final chunk, internally parallelized
void SE_Creat_phasefinal(SUB_GROUP sub_group, RFC_SE_Chunks& rfc_se_classifier){
    // Extract CBMs from phase1_chunks' EqIDTables
    std::vector<std::vector<std::uint32_t>> cbms_list_p1c0;
    if (rfc_se_classifier.phase1_chunks[0].matched_EqIDTable) {
        std::lock_guard<std::mutex> lock(rfc_se_classifier.phase1_chunks[0].matched_EqIDTable->mtx); // Lock for reading list
        for (auto* eq = rfc_se_classifier.phase1_chunks[0].matched_EqIDTable->head; eq != nullptr; eq = eq->next) {
            cbms_list_p1c0.push_back(eq->cbm);
        }
    }

    std::vector<std::vector<std::uint32_t>> cbms_list_p1c1;
    if (rfc_se_classifier.phase1_chunks[1].matched_EqIDTable) {
        std::lock_guard<std::mutex> lock(rfc_se_classifier.phase1_chunks[1].matched_EqIDTable->mtx); // Lock for reading list
        for (auto* eq = rfc_se_classifier.phase1_chunks[1].matched_EqIDTable->head; eq != nullptr; eq = eq->next) {
            cbms_list_p1c1.push_back(eq->cbm);
        }
    }
    
    std::uint32_t num_eq_from_p1c0 = static_cast<std::uint32_t>(cbms_list_p1c0.size());
    std::uint32_t num_eq_from_p1c1 = static_cast<std::uint32_t>(cbms_list_p1c1.size());
    std::uint32_t total_final_items = num_eq_from_p1c0 * num_eq_from_p1c1;

    rfc_se_classifier.final_chunks.item_num = total_final_items;
    if (total_final_items > 0) {
        rfc_se_classifier.final_chunks.item.resize(total_final_items);
    } else {
        rfc_se_classifier.final_chunks.item.clear();
        return; 
    }
    
    if (num_eq_from_p1c0 == 0 || num_eq_from_p1c1 == 0) { // Should be caught by total_final_items == 0
        return;
    }

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 1; 
    if (num_eq_from_p1c0 < num_threads) { // Don't use more threads than outer loop items
        num_threads = num_eq_from_p1c0;
    }
    if (num_threads == 0 && num_eq_from_p1c0 > 0) num_threads = 1; // Ensure at least one thread if work exists


    std::vector<std::future<void>> final_phase_futures;
    if (num_threads > 0) {
        size_t items_per_thread_outer = (num_eq_from_p1c0 + num_threads - 1) / num_threads;
        for (unsigned int i = 0; i < num_threads; ++i) {
            size_t start_idx = i * items_per_thread_outer;
            size_t end_idx = std::min(start_idx + items_per_thread_outer, (size_t)num_eq_from_p1c0);

            if (start_idx < end_idx) {
                final_phase_futures.push_back(std::async(std::launch::async, SE_Creat_phasefinal_task_worker,
                                                   std::cref(cbms_list_p1c0),
                                                   std::cref(cbms_list_p1c1),
                                                   std::cref(sub_group), // sub_group is const ref
                                                   std::ref(rfc_se_classifier), // rfc_se_classifier is modified
                                                   start_idx, end_idx));
            }
        }
        for (auto& fut : final_phase_futures) {
            if (fut.valid()) fut.get();
        }
    } else if (num_eq_from_p1c0 > 0) { // Fallback to serial if somehow num_threads is 0 but work exists
         SE_Creat_phasefinal_task_worker(cbms_list_p1c0, cbms_list_p1c1, sub_group, rfc_se_classifier, 0, num_eq_from_p1c0);
    }
}

// Main classifier building function
RFC_SE_Chunks Improved_Building_Classifier(const Filter_Division& filter_division, const int flag) {
    SUB_GROUP sub_group;
    // sub_group.size_length should be initialized, e.g., sub_group.size_length = 32; (bits in uint32_t for CBM element)
    // This is a crucial parameter for SE_Set_cbm and SE_Find_rule. Assuming it's set in SUB_GROUP's definition or init.
    // If not, set a default:
    // if (sub_group.size_length == 0) sub_group.size_length = 32;


    switch (flag) {
        case 0: // L_L
            sub_group.src_mask = filter_division.selected_src_mask;
            sub_group.dst_mask = filter_division.selected_lower_dst_mask;
            sub_group.src_mask_flag = 1; 
            sub_group.dst_mask_flag = 1; 
            sub_group.sub_filter_rules = filter_division.Sub_L_L;
            if (filter_division.L_L_accumulation > 0) {
                sub_group.sub_size = CeilDivide(filter_division.L_L_accumulation, sub_group.size_length);
            } else {
                sub_group.sub_size = 0;
            }
            // Populate selected_chunks for factor optimization
            if (sub_group.src_mask <= 16) sub_group.selected_chunks.push_back(0); // src_ip_first_half
            sub_group.selected_chunks.push_back(1); // src_ip_second_half (always for src if src_mask > 0)
            if (sub_group.dst_mask <= 16) sub_group.selected_chunks.push_back(2); // dst_ip_first_half
            sub_group.selected_chunks.push_back(3); // dst_ip_second_half (always for dst if dst_mask > 0)
            break;
        case 1: // L_H
            sub_group.src_mask = filter_division.selected_src_mask;
            sub_group.dst_mask = filter_division.selected_higher_dst_mask; // L_H uses selected_higher_dst_mask
            sub_group.src_mask_flag = 1;
            sub_group.dst_mask_flag = 0; // dst_mask is "High"
            sub_group.sub_filter_rules = filter_division.Sub_L_H;
            if (filter_division.L_H_accumulation > 0) {
                sub_group.sub_size = CeilDivide(filter_division.L_H_accumulation, sub_group.size_length);
            } else {
                sub_group.sub_size = 0;
            }
            if (sub_group.src_mask <= 16) sub_group.selected_chunks.push_back(0);
            sub_group.selected_chunks.push_back(1);
            // No selected_chunks for dst_ip parts as dst_mask is "High" (handled by normal path in phase0)
            break;
        case 2: // H_L
            sub_group.src_mask = filter_division.selected_src_mask; // H_L uses selected_src_mask (which is "High")
            sub_group.dst_mask = filter_division.selected_lower_dst_mask;
            sub_group.src_mask_flag = 0; 
            sub_group.dst_mask_flag = 1;
            sub_group.sub_filter_rules = filter_division.Sub_H_L;
            if (filter_division.H_L_accumulation > 0) {
                sub_group.sub_size = CeilDivide(filter_division.H_L_accumulation, sub_group.size_length);
            } else {
                sub_group.sub_size = 0;
            }
            // No selected_chunks for src_ip parts
            if (sub_group.dst_mask <= 16) sub_group.selected_chunks.push_back(2);
            sub_group.selected_chunks.push_back(3);
            break;
        case 3: // H_H
            sub_group.src_mask = filter_division.selected_src_mask;
            sub_group.dst_mask = filter_division.selected_higher_dst_mask;
            sub_group.src_mask_flag = 0;
            sub_group.dst_mask_flag = 0; 
            sub_group.sub_filter_rules = filter_division.Sub_H_H;
            if (filter_division.H_H_accumulation > 0) {
                sub_group.sub_size = CeilDivide(filter_division.H_H_accumulation, sub_group.size_length);
            } else {
                sub_group.sub_size = 0;
            }
            // No selected_chunks for IP parts as both are "High"
            break;
        default:
            throw std::invalid_argument("Invalid flag for Improved_Building_Classifier");
    }

    // auto start_time = std::chrono::high_resolution_clock::now(); // For timing this specific call
    RFC_SE_Chunks rfc_se_classifier; // This will hold the results

    if (sub_group.sub_size == 0 || sub_group.sub_filter_rules.empty()) {
        // Handle cases with no rules to process for this sub_group
        for(int i=0; i<6; ++i) {
            rfc_se_classifier.phase0_chunks[i].item_num = (i<4 ? 65536 : 65536); // Default full size for IP/Port dimensions
            rfc_se_classifier.phase0_chunks[i].factor = 0;
            rfc_se_classifier.phase0_chunks[i].item.assign(rfc_se_classifier.phase0_chunks[i].item_num, -1); // Fill with -1
            if (!rfc_se_classifier.phase0_chunks[i].matched_EqIDTable) { // Ensure EqIDTable exists
                 rfc_se_classifier.phase0_chunks[i].matched_EqIDTable = new se_EqIDTable();
                 // Add a default entry for "no rules match" CBM if sub_size > 0 (but sub_filter_rules is empty)
                 // This case is tricky; if sub_size is 0, CBM is empty. If sub_size > 0 but no rules, CBM is all zeros.
                 if (sub_group.sub_size > 0) {
                    std::vector<std::uint32_t> empty_cbm(sub_group.sub_size, 0);
                    SE_Add_cbm_EqIDTable(rfc_se_classifier.phase0_chunks[i].matched_EqIDTable, empty_cbm, sub_group.sub_size);
                 }
            }
        }
        for(int i=0; i<2; ++i) {
            rfc_se_classifier.phase1_chunks[i].item_num = 0; // Will be product of phase0 items
            rfc_se_classifier.phase1_chunks[i].item.clear();
             if (!rfc_se_classifier.phase1_chunks[i].matched_EqIDTable) {
                 rfc_se_classifier.phase1_chunks[i].matched_EqIDTable = new se_EqIDTable();
             }
        }
        rfc_se_classifier.final_chunks.item_num = 0;
        rfc_se_classifier.final_chunks.item.clear();
    } else {
        // Parallel building of rfc_se phase0
        std::vector<std::future<void>> phase0_futures;
        for (int i = 0; i < 6; ++i) { // For all 6 dimensions/chunks
            phase0_futures.push_back(std::async(std::launch::async, SE_Creat_phase0_task, std::cref(sub_group), std::ref(rfc_se_classifier), i));
        }
        for (auto& fut : phase0_futures) {
            if (fut.valid()) fut.get(); 
        }

        // Parallel building of rfc_se phase1
        std::vector<std::future<void>> phase1_futures;
        for (int i = 0; i < 2; ++i) { // For the 2 phase1 chunks
            phase1_futures.push_back(std::async(std::launch::async, SE_Creat_phase1_task, std::cref(sub_group), std::ref(rfc_se_classifier), i));
        }
        for (auto& fut : phase1_futures) {
            if (fut.valid()) fut.get();
        }

        // Building of rfc_se finalphase (internally parallelized)
        SE_Creat_phasefinal(sub_group, rfc_se_classifier);
    }

    // auto end_time = std::chrono::high_resolution_clock::now(); 
    // auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    // std::cout << "Flag " << flag << " Building time: " << elapsed_time << " microseconds" << std::endl;
    
    return rfc_se_classifier;
}
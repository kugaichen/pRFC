#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <map>
#include <future> // Required for std::async, std::future
#include <thread>   // Required for std::thread::hardware_concurrency

#include "FilterReader.h"
#include "Filter_Preprocess.h"


double balance_weight = 0.3;     // weight for balance_score 
double combine_weight = 0.3;     // weight for combine_score
double storage_weight = 0.4;     // weight for storage_score

// double balance_weight = 1.0;     // weight for balance_score
// double combine_weight = 0.0;     // weight for combine_score
// double storage_weight = 0.0;     // weight for storage_score

/* Calculate_Storage_Optimiztion
    related parameters: 
    bool enableStorageOptimization -> Decide whether this parameter should be utilized to impact the objective function 
    core parameters: mask_length -> the shorter the optimization effect better
    accroding weight: 
*/

// the bigger return value , the better performance
double Calculate_Storage_Score(std::uint16_t mask_length){
    double original_item_num = std::pow(2.0, 16) * 2;

    double improved_item_num;
    double impact_factor;

    if (mask_length >= 16){
        improved_item_num = std::pow(2.0,16) / std::pow(2.0,32 - mask_length) + std::pow(2.0, 16);     // selected mask to compress data chunk item_num 
        impact_factor = (1.0 - improved_item_num / original_item_num) * 100;    // transforme to 100% percentage
    }

    else if (mask_length < 16)
    {
        improved_item_num = std::pow(2.0,16) / std::pow(2.0,16 - mask_length) + 1.0;
        impact_factor = (1.0 - improved_item_num / original_item_num) * 100;
    }

    // Normalization
    double max_improved_result = (1.0 - (1.0 + 1.0) / original_item_num) * 100;
    double min_improved_result = 0.0;

    double normalized_score = (impact_factor - min_improved_result) / (max_improved_result - min_improved_result);

    return normalized_score;
    
}


// Function to calculate the balance score based on the distribution of rules(only dst distribution)
// the bigger return value , the better performance
double Calculate_Balance_Score(std::uint32_t low_accumulation, std::uint32_t high_accumulation, std::uint32_t total_rules_num){

    double score = 0.0;
    double ideal_fraction = 0.25;    // Ideal distribution fraction 
    
    // Calculate the fraction of low_accumulation and high_accumulation relative to totalRules
    double low_fraction = static_cast<double>(low_accumulation) / total_rules_num;
    double high_fraction = static_cast<double>(high_accumulation) / total_rules_num;

    // Calculate the squared difference from the ideal fraction and add to total_diff
    // sequared difference
    score += std::pow(low_fraction - ideal_fraction, 2);
    score += std::pow(high_fraction - ideal_fraction, 2);

    // absolute difference
    // score += std::abs(low_fraction - ideal_fraction);
    // score += std::abs(high_fraction - ideal_fraction);

    // Normalize the score
    // sequared difference
    // The maximum possible difference is 2 * (1 - 0.25)^2 = 2 * 0.5625 = 1.125
    double max_diff = std::pow(1 - ideal_fraction, 2) + std::pow(0 - ideal_fraction, 2);

    // absolute difference
    // The maximum possible difference is 2 * (1 - 0.25)
    // double max_diff = std::abs(1 - ideal_fraction) + std::abs(0 - ideal_fraction);
    double normalized_score = 1.0 - (score / max_diff);

    return normalized_score;

}

        

// Function to calculate the combination score to ensure to mitigate combinatorial explosion
// the bigger return value , the better performance
double Calculate_Combine_Score(std::uint16_t src_mask_selected, std::uint16_t dst_mask_selected){
    double score = 0.0;
    double src_score = 0.0;
    double dst_score = 0.0;
    if (src_mask_selected <= 16){
        src_score = log(32.0 - src_mask_selected + 1.0) / log(32.0 + 1.0);   // src < 16, the closer the src_mask and 0 the better
        dst_score = 1 - log(32.0 - dst_mask_selected + 1.0) / log(32.0 + 1.0);  // src < 16, the closer the dst_mask and 32 the better 
    }
    else if(src_mask_selected >= 16) {
        src_score = 1 - log(32.0 - src_mask_selected + 1.0) / log(32.0 + 1.0);  // src > 16, the closer the src_mask and 32 the better
        dst_score = log(32.0 - dst_mask_selected + 1.0) / log(32.0 + 1.0);  // src > 16, the closer the src_mask and 0 the better
    }
    
    score = src_score * 0.5 + dst_score * 0.5;

    return score;
}


double Evaluation(double balance_score, double storage_score, double combine_score, bool enableStorageOpimization,
    double w1, double w2, double w3){
    double evaluation_result = (w1 * balance_score + w2 * combine_score + 
        (enableStorageOpimization ? w3 * storage_score : 0.0));
    
    return evaluation_result;

}

// Helper structure to store results from each parallel task
struct ParallelCategorizationResult {
    std::vector<FilterRule> sub_L_L;
    std::vector<FilterRule> sub_L_H;
    std::vector<FilterRule> sub_H_L;
    std::vector<FilterRule> sub_H_H;
    std::uint32_t l_l_accumulation = 0;
    std::uint32_t l_h_accumulation = 0;
    std::uint32_t h_l_accumulation = 0;
    std::uint32_t h_h_accumulation = 0;
};



// Purpose: Computes and returns a structure (Src_Mask_Bucket) containing source address mask buckets based on a given list of filter rules (FilterRule).
// Parameters: filter_rules - A vector containing filter rules.
// Returns: Src_Mask_Bucket structure, which includes mask buckets, reach buckets, and remaining buckets.
Src_Mask_Bucket Obtain_Src_mask_bucket(const std::vector<FilterRule>& filter_rules){

    // Creat dictionary to record {0-32} num
    std::map<std::uint16_t,std::uint32_t> src_count_map;
    // Initialize the source address mask dictionary
    for (size_t i=0; i<=32; i++){
        src_count_map[i] = 0;
    }


    // Sort filter rules based on the source address mask value
    std::vector<std::vector<FilterRule>> count_filter_rules(33);

    // Calculate the number of filter rules for each mask (0-32)
    std::uint32_t src_filter_num = 0; 
    for(const auto & filter : filter_rules){
        src_count_map[filter.src_ip_mask] ++;
        src_filter_num ++;
        count_filter_rules[filter.src_ip_mask].push_back(filter);   // Group rules by mask
    }

    // new filter_rule
    std::vector<FilterRule> copy_filter_rules(src_filter_num);

    // Use counting sort to sort the rules
    std::uint32_t index = 0;
    for (size_t i =0; i <= 32; ++i){
        for (const FilterRule& rule : count_filter_rules[i]){
            copy_filter_rules[index] = rule;
            index ++; 
        }
    }

    // Initialize the factors 
    double scale_factor_bottom = 0.4;
    double scale_factor_top = 0.6;
    int flag;
    std::uint32_t target_range_bottom;
    std::uint32_t target_range_top;
    std::uint32_t accumulate_sum;

    // Initialize the struct
    Src_Mask_Bucket src_mask_bucket;

    // record the sorted filter rules 
    src_mask_bucket.src_sorting_filters = copy_filter_rules;

    // Interverl Search
    while (scale_factor_bottom >= 0.1 && scale_factor_bottom < 0.5 && scale_factor_top > 0.5 && scale_factor_top <= 0.9){
        // Initialization
        src_mask_bucket.mask_bucket.clear();
        Src_Mask temp_src_mask_struct;
    
        flag = 5;
        target_range_bottom = scale_factor_bottom * src_filter_num;
        target_range_top = scale_factor_top * src_filter_num;
        accumulate_sum = 0;

        // core process -> find the mask 
        for (const auto& pair:src_count_map){
            accumulate_sum += pair.second;
            if (accumulate_sum >= target_range_bottom && accumulate_sum <= target_range_top){
                temp_src_mask_struct.selected_src_mask = pair.first;
                temp_src_mask_struct.reach_accumulation = accumulate_sum;
                temp_src_mask_struct.remain_accumulation = src_filter_num - accumulate_sum;
                src_mask_bucket.mask_bucket.push_back(temp_src_mask_struct);
                flag --;
            }
        }

        // Exit the loop if enough masks are found
        if (flag <= 0 && flag >= -1){
            src_mask_bucket.scale_factor_bottom = scale_factor_bottom;
            src_mask_bucket.scale_factor_top = scale_factor_top;
            break;
        }

        // else if(flag < -1){
        //     std::cout << flag << std::endl;
        //     scale_factor_bottom = scale_factor_bottom + 0.07;
        //     scale_factor_top = scale_factor_top - 0.07;
        // }

        else{
            // Otherwise, adjust the scaling factors to expand the search range
            scale_factor_bottom = scale_factor_bottom - 0.05;
            scale_factor_top = scale_factor_top + 0.05;
        }

    }
 
    return src_mask_bucket;
}


// Ensure the src_mask_selected, and selected the dst_mask_bucket
Dst_Mask_Bucket Obtain_Dst_mask_bucket(Src_Mask& src_mask_struct, std::vector<FilterRule>& src_sorting_filters, std::uint16_t src_scale_factor_bottom, std::uint16_t src_scale_factor_top){
    // Creat dictionary to record {0-32} num -> for dst_mask
    std::map<std::uint16_t,std::uint32_t> low_dst_count_map;
    std::map<std::uint16_t,std::uint32_t> high_dst_count_map;

    // Initialize the source address mask dictionary
    for (size_t i=0; i<=32; i++){
        low_dst_count_map[i] = 0;
        high_dst_count_map[i] = 0;
    }

    std::uint32_t low_dst_filter_num = 0;
    std::uint32_t high_dst_filter_num = 0;
    std::uint32_t counter = 0;

    for (const auto& rules:src_sorting_filters){
        if (counter < src_mask_struct.reach_accumulation){
            low_dst_count_map[rules.dst_ip_mask] ++;
            low_dst_filter_num ++;
            counter ++;
        }

        else{
            high_dst_count_map[rules.dst_ip_mask] ++;
            high_dst_filter_num ++;
            counter ++;
        }
            
    }

    if (low_dst_filter_num != src_mask_struct.reach_accumulation || high_dst_filter_num != src_mask_struct.remain_accumulation){
        std::cerr << "Dictionary record error: num does not match reach_accumlulation or remain_accumulation." << std::endl;
    }

    // Interverl Search -> for lower src mask 
    // src_mask = [0 ~ selected_src_mask]
    // dst_mask = [0 ~ 32]
    // Initialize the factors
    double scale_factor_bottom = 0.4;
    double scale_factor_top = 0.6;
    int flag;
    std::uint32_t target_range_bottom;
    std::uint32_t target_range_top;
    std::uint32_t accumulate_sum;

    // Initialize the struct
    Dst_Mask_Bucket dst_mask_bucket;
    dst_mask_bucket.selected_src_mask = src_mask_struct.selected_src_mask;
    dst_mask_bucket.total_nums = src_sorting_filters.size();

    // Interverl Search -> for lower src mask 
    // src_mask = [0 ~ selected_src_mask]
    // dst_mask = [0 ~ 32]
    while (scale_factor_bottom >= 0.1 && scale_factor_bottom < 0.5 && scale_factor_top > 0.5 && scale_factor_top <= 0.9){
        // Initialization
        dst_mask_bucket.lower_dst_mask_bucket.clear();
        Dst_Mask temp_dst_mask_struct;

        flag = 5;
        target_range_bottom = low_dst_filter_num * scale_factor_bottom;
        target_range_top = low_dst_filter_num * scale_factor_top;
        accumulate_sum = 0;

        // core process -> find the mask intervel
        for (const auto& pair:low_dst_count_map){
            accumulate_sum += pair.second;
            if (accumulate_sum >= target_range_bottom && accumulate_sum <= target_range_top){
                temp_dst_mask_struct.selected_dst_mask = pair.first;
                temp_dst_mask_struct.reach_accumulation = accumulate_sum;
                temp_dst_mask_struct.remain_accumulation = low_dst_filter_num - accumulate_sum;
                dst_mask_bucket.lower_dst_mask_bucket.push_back(temp_dst_mask_struct);
                flag --;                    
            }
        }

            // Exit the loop if enough masks are found
        if (flag <= 0 && flag >= -1){
            dst_mask_bucket.lower_min_intervel_factor = scale_factor_bottom * src_scale_factor_bottom;
            dst_mask_bucket.lower_max_intervel_factor = scale_factor_top * src_scale_factor_top;
            break;
        }

        // else if(flag < -1){
        //     scale_factor_bottom = scale_factor_bottom + 0.05;
        //     scale_factor_top = scale_factor_top + 0.05;
        // }

        else{
            // Otherwise, adjust the scaling factors to expand the search range
            scale_factor_bottom = scale_factor_bottom - 0.05;
            scale_factor_top = scale_factor_top + 0.05;
        }
    }


    // Interverl Search -> for higher src mask 
    // src_mask = [selected_src_mask ~ 32]
    // dst_mask = [0 ~ 32]
    // Initialize the factors
    scale_factor_bottom = 0.4;
    scale_factor_top = 0.6;

    while (scale_factor_bottom >= 0.1 && scale_factor_bottom < 0.5 && scale_factor_top > 0.5 && scale_factor_top <= 0.9){
        // Initialization
        dst_mask_bucket.higher_dst_mask_bucket.clear();
        Dst_Mask temp_dst_mask_struct;

        flag = 5;
        target_range_bottom = high_dst_filter_num * scale_factor_bottom;
        target_range_top = high_dst_filter_num * scale_factor_top;
        accumulate_sum = 0;

        for (const auto& pair:high_dst_count_map){
            accumulate_sum += pair.second;
            if (accumulate_sum >= target_range_bottom && accumulate_sum <= target_range_top){
                temp_dst_mask_struct.selected_dst_mask = pair.first;
                temp_dst_mask_struct.reach_accumulation = accumulate_sum;
                temp_dst_mask_struct.remain_accumulation = high_dst_filter_num - accumulate_sum;
                dst_mask_bucket.higher_dst_mask_bucket.push_back(temp_dst_mask_struct);
                flag --;                    
            }
        }
            
        // Exit the loop if enough masks are found
        if (flag <= 0 && flag >= -1){
            dst_mask_bucket.higher_min_intervel_factor = scale_factor_bottom * src_scale_factor_bottom;
            dst_mask_bucket.higher_max_intervel_factor = scale_factor_top * src_scale_factor_top;
            break;
        }

        else if(flag < -1){
            scale_factor_bottom = scale_factor_bottom + 0.05;
            scale_factor_top = scale_factor_top + 0.05;
        }

        else{
            // Otherwise, adjust the scaling factors to expand the search range
            scale_factor_bottom = scale_factor_bottom - 0.05;
            scale_factor_top = scale_factor_top + 0.05;
        }    
     
    }

    return dst_mask_bucket;

}


// Ensure the src_mask_selected, and select the dst_mask_selected with the best score
Mask_Checkpoint Decide_Dst_mask_bucket(Dst_Mask_Bucket& dst_mask_bucket){
    std::uint16_t temp_selected_src_mask = dst_mask_bucket.selected_src_mask;
    std::uint16_t temp_lower_selected_dst_mask;
    std::uint32_t temp_lower_low_accumulation; // lower lower
    std::uint32_t temp_lower_high_accumulation; // lower higher
    // double balance_weight = 0.5;     // weight for balance_score 
    // double combine_weight = 0.2;     // weight for combine_score
    // double storage_weight = 0.3;     // weight for storage_score


    double lower_best_score = 0.0;
    Mask_Checkpoint src_dst_checkpoint;
       
    // lower 
    for (auto& low_dst_mask_struct:dst_mask_bucket.lower_dst_mask_bucket){
        temp_lower_selected_dst_mask = low_dst_mask_struct.selected_dst_mask;
        temp_lower_low_accumulation = low_dst_mask_struct.reach_accumulation; // lower lower
        temp_lower_high_accumulation = low_dst_mask_struct.remain_accumulation; // lower higher

        double balance_score = Calculate_Balance_Score(temp_lower_low_accumulation,temp_lower_high_accumulation,dst_mask_bucket.total_nums);
        double storage_score = Calculate_Storage_Score(temp_lower_selected_dst_mask);
        double combine_score = Calculate_Combine_Score(temp_selected_src_mask,temp_lower_selected_dst_mask);
        double evaluation_result = Evaluation(balance_score, storage_score, combine_score, true, balance_weight, combine_weight, storage_weight);
        if (evaluation_result > lower_best_score){
            lower_best_score = evaluation_result;
            src_dst_checkpoint.selected_src_mask = temp_selected_src_mask;
            src_dst_checkpoint.selected_lower_dst_mask = temp_lower_selected_dst_mask;
            src_dst_checkpoint.L_L_accumulation = temp_lower_low_accumulation;
            src_dst_checkpoint.L_H_accumulation = temp_lower_high_accumulation;
            src_dst_checkpoint.lower_balance_score = balance_score;
            src_dst_checkpoint.lower_storage_score = storage_score;
            src_dst_checkpoint.lower_combine_score = combine_score;
        }

    }

    std::uint16_t temp_higher_selected_dst_mask;
    std::uint32_t temp_higher_low_accumulation; // lower lower
    std::uint32_t temp_higher_high_accumulation; // lower higher
    double higher_best_score = 0.0;

    //higher
    for (auto& high_dst_mask_struct:dst_mask_bucket.higher_dst_mask_bucket){
        temp_higher_selected_dst_mask = high_dst_mask_struct.selected_dst_mask;
        temp_higher_low_accumulation = high_dst_mask_struct.reach_accumulation;
        temp_higher_high_accumulation = high_dst_mask_struct.remain_accumulation;

        double balance_score = Calculate_Balance_Score(temp_higher_low_accumulation,temp_higher_high_accumulation,dst_mask_bucket.total_nums);
        double storage_score = Calculate_Storage_Score(temp_higher_selected_dst_mask);
        double combine_score = Calculate_Combine_Score(temp_selected_src_mask,temp_higher_selected_dst_mask);
        double evaluation_result = Evaluation(balance_score, storage_score, combine_score, true, balance_weight, combine_weight, storage_weight);
        if (evaluation_result > higher_best_score){
            higher_best_score = evaluation_result;
            src_dst_checkpoint.selected_higher_dst_mask = temp_higher_selected_dst_mask;
            src_dst_checkpoint.H_L_accumulation = temp_higher_low_accumulation;
            src_dst_checkpoint.H_H_accumulation = temp_higher_high_accumulation;
            src_dst_checkpoint.higher_balance_score = balance_score;
            src_dst_checkpoint.higher_storage_score = storage_score;
            src_dst_checkpoint.higher_combine_score = combine_score;
        }
    }

    return src_dst_checkpoint;
}



Mask_Checkpoint Decide_best_mask_bucket(const std::vector<FilterRule>& filter_rules){
    Src_Mask_Bucket src_mask_bucket = Obtain_Src_mask_bucket(filter_rules);
    
    double candidate_score = 0;
    Mask_Checkpoint final_selected;
    // double balance_weight = 0.4;     // weight for balance_score 
    // double combine_weight = 0.4;     // weight for combine_score
    // double storage_weight = 0.2;     // weight for storage_score
    for(auto& src_mask_struct : src_mask_bucket.mask_bucket){
        Dst_Mask_Bucket dst_mask_bucket = Obtain_Dst_mask_bucket(src_mask_struct,src_mask_bucket.src_sorting_filters, src_mask_bucket.scale_factor_bottom, src_mask_bucket.scale_factor_top);
        Mask_Checkpoint src_dst_checkpoint = Decide_Dst_mask_bucket(dst_mask_bucket);

        double src_storage_score = Calculate_Storage_Score(src_dst_checkpoint.selected_src_mask);
        double balance_score = (src_dst_checkpoint.lower_balance_score + src_dst_checkpoint.higher_balance_score) / 2.0;
        double dst_storage_score = (src_dst_checkpoint.lower_storage_score + src_dst_checkpoint.higher_storage_score) / 2.0; 
        double storage_score = (src_storage_score + dst_storage_score) / 2.0;
        double combine_score = (src_dst_checkpoint.lower_combine_score + src_dst_checkpoint.higher_combine_score) / 2.0;
        double evaluation_result = Evaluation(balance_score, storage_score, combine_score, true, balance_weight, combine_weight, storage_weight);

        if (evaluation_result > candidate_score){
            final_selected = src_dst_checkpoint;
        }
    }
 
    return final_selected;
}


// void Filter_Preprocess(Filter_Division& final_filter_division, const std::vector<FilterRule>& filter_rules){
//     Mask_Checkpoint final_selected = Decide_best_mask_bucket(filter_rules);
//     final_filter_division.selected_src_mask = final_selected.selected_src_mask;
//     final_filter_division.selected_lower_dst_mask = final_selected.selected_lower_dst_mask;
//     final_filter_division.selected_higher_dst_mask = final_selected.selected_higher_dst_mask;
//     final_filter_division.L_L_accumulation = final_selected.L_L_accumulation;
//     final_filter_division.L_L_accumulation = 0;
//     final_filter_division.L_H_accumulation = final_selected.L_H_accumulation;
//     final_filter_division.L_H_accumulation = 0;
//     final_filter_division.H_L_accumulation = final_selected.H_L_accumulation;
//     final_filter_division.H_L_accumulation = 0;
//     final_filter_division.H_H_accumulation = final_selected.H_H_accumulation;
//     final_filter_division.H_H_accumulation = 0;

//     for (const auto& filter: filter_rules){
//         if (filter.src_ip_mask <= final_filter_division.selected_src_mask && filter.dst_ip_mask <= final_filter_division.selected_lower_dst_mask){
//             final_filter_division.Sub_L_L.push_back(filter);
//             final_filter_division.L_L_accumulation ++;
//         }

//         else if(filter.src_ip_mask <= final_filter_division.selected_src_mask && filter.dst_ip_mask > final_filter_division.selected_lower_dst_mask){
//             final_filter_division.Sub_L_H.push_back(filter);
//             final_filter_division.L_H_accumulation ++;
//         }

//         else if(filter.src_ip_mask > final_filter_division.selected_src_mask && filter.dst_ip_mask <= final_filter_division.selected_higher_dst_mask){
//             final_filter_division.Sub_H_L.push_back(filter);
//             final_filter_division.H_L_accumulation ++;
//         }

//         else if(filter.src_ip_mask > final_filter_division.selected_src_mask && filter.dst_ip_mask > final_filter_division.selected_higher_dst_mask){
//             final_filter_division.Sub_H_H.push_back(filter);
//             final_filter_division.H_H_accumulation ++;
//         }
//     }

//     if (final_filter_division.L_L_accumulation != final_selected.L_L_accumulation or
//         final_filter_division.L_H_accumulation != final_selected.L_H_accumulation or
//         final_filter_division.H_H_accumulation != final_selected.H_H_accumulation or
//         final_filter_division.H_L_accumulation != final_selected.H_L_accumulation){
//             std::cerr << "filter rules preprocess error! amount of subset is not right" << std::endl;
//         }

// }


void Filter_Preprocess(Filter_Division& final_filter_division, const std::vector<FilterRule>& filter_rules){
    Mask_Checkpoint final_selected = Decide_best_mask_bucket(filter_rules);
    final_filter_division.selected_src_mask = final_selected.selected_src_mask;
    final_filter_division.selected_lower_dst_mask = final_selected.selected_lower_dst_mask;
    final_filter_division.selected_higher_dst_mask = final_selected.selected_higher_dst_mask;

    // Initialize accumulations to 0 before parallel processing
    final_filter_division.L_L_accumulation = 0;
    final_filter_division.L_H_accumulation = 0;
    final_filter_division.H_L_accumulation = 0;
    final_filter_division.H_H_accumulation = 0;

    // Clear existing sub-vectors if they might contain old data
    final_filter_division.Sub_L_L.clear();
    final_filter_division.Sub_L_H.clear();
    final_filter_division.Sub_H_L.clear();
    final_filter_division.Sub_H_H.clear();

    if (filter_rules.empty()) {
        // Handle empty input if necessary, or rely on the check at the end
        if (final_filter_division.L_L_accumulation != final_selected.L_L_accumulation ||
            final_filter_division.L_H_accumulation != final_selected.L_H_accumulation ||
            final_filter_division.H_H_accumulation != final_selected.H_H_accumulation ||
            final_filter_division.H_L_accumulation != final_selected.H_L_accumulation){
                std::cerr << "filter rules preprocess error! amount of subset is not right (empty input)" << std::endl;
            }
        return;
    }

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 1; // Fallback if hardware_concurrency is not informative
    if (filter_rules.size() < num_threads * 100) { // Heuristic: only use threads for larger datasets
        num_threads = 1;
    }


    std::vector<std::future<ParallelCategorizationResult>> futures;
    std::size_t rules_per_thread = (filter_rules.size() + num_threads - 1) / num_threads;

    // These selected masks are read-only within the lambda, safe to capture by value or const ref
    const auto selected_src_mask_val = final_filter_division.selected_src_mask;
    const auto selected_lower_dst_mask_val = final_filter_division.selected_lower_dst_mask;
    const auto selected_higher_dst_mask_val = final_filter_division.selected_higher_dst_mask;

    for (unsigned int i = 0; i < num_threads; ++i) {
        std::size_t start_index = i * rules_per_thread;
        std::size_t end_index = std::min(start_index + rules_per_thread, filter_rules.size());

        if (start_index >= end_index) continue; // No work for this thread

        futures.push_back(std::async(std::launch::async, 
            [&filter_rules, start_index, end_index, 
             selected_src_mask_val, selected_lower_dst_mask_val, selected_higher_dst_mask_val]() {
            ParallelCategorizationResult local_result;
            for (std::size_t j = start_index; j < end_index; ++j) {
                const auto& filter = filter_rules[j];
                if (filter.src_ip_mask <= selected_src_mask_val && filter.dst_ip_mask <= selected_lower_dst_mask_val){
                    local_result.sub_L_L.push_back(filter);
                    local_result.l_l_accumulation++;
                }
                else if(filter.src_ip_mask <= selected_src_mask_val && filter.dst_ip_mask > selected_lower_dst_mask_val){
                    local_result.sub_L_H.push_back(filter);
                    local_result.l_h_accumulation++;
                }
                else if(filter.src_ip_mask > selected_src_mask_val && filter.dst_ip_mask <= selected_higher_dst_mask_val){
                    local_result.sub_H_L.push_back(filter);
                    local_result.h_l_accumulation++;
                }
                else if(filter.src_ip_mask > selected_src_mask_val && filter.dst_ip_mask > selected_higher_dst_mask_val){
                    local_result.sub_H_H.push_back(filter);
                    local_result.h_h_accumulation++;
                }
            }
            return local_result;
        }));
    }

    // Reserve space in final vectors to reduce reallocations during merge
    // This is an estimate; actual sizes might differ if some rules don't fit any category
    final_filter_division.Sub_L_L.reserve(final_selected.L_L_accumulation);
    final_filter_division.Sub_L_H.reserve(final_selected.L_H_accumulation);
    final_filter_division.Sub_H_L.reserve(final_selected.H_L_accumulation);
    final_filter_division.Sub_H_H.reserve(final_selected.H_H_accumulation);


    // Merge results from all threads
    for (auto& fut : futures) {
        ParallelCategorizationResult local_result = fut.get();
        
        final_filter_division.Sub_L_L.insert(final_filter_division.Sub_L_L.end(), 
                                             std::make_move_iterator(local_result.sub_L_L.begin()), 
                                             std::make_move_iterator(local_result.sub_L_L.end()));
        final_filter_division.L_L_accumulation += local_result.l_l_accumulation;

        final_filter_division.Sub_L_H.insert(final_filter_division.Sub_L_H.end(), 
                                             std::make_move_iterator(local_result.sub_L_H.begin()), 
                                             std::make_move_iterator(local_result.sub_L_H.end()));
        final_filter_division.L_H_accumulation += local_result.l_h_accumulation;

        final_filter_division.Sub_H_L.insert(final_filter_division.Sub_H_L.end(), 
                                             std::make_move_iterator(local_result.sub_H_L.begin()), 
                                             std::make_move_iterator(local_result.sub_H_L.end()));
        final_filter_division.H_L_accumulation += local_result.h_l_accumulation;

        final_filter_division.Sub_H_H.insert(final_filter_division.Sub_H_H.end(), 
                                             std::make_move_iterator(local_result.sub_H_H.begin()), 
                                             std::make_move_iterator(local_result.sub_H_H.end()));
        final_filter_division.H_H_accumulation += local_result.h_h_accumulation;
    }


    if (final_filter_division.L_L_accumulation != final_selected.L_L_accumulation or
        final_filter_division.L_H_accumulation != final_selected.L_H_accumulation or
        final_filter_division.H_H_accumulation != final_selected.H_H_accumulation or
        final_filter_division.H_L_accumulation != final_selected.H_L_accumulation){
            std::cerr << "filter rules preprocess error! amount of subset is not right" << std::endl;
            // Optionally print the differing values for debugging:
            std::cerr << "Expected L_L: " << final_selected.L_L_accumulation << " Got: " << final_filter_division.L_L_accumulation << std::endl;
            std::cerr << "Expected L_H: " << final_selected.L_H_accumulation << " Got: " << final_filter_division.L_H_accumulation << std::endl;
            std::cerr << "Expected H_L: " << final_selected.H_L_accumulation << " Got: " << final_filter_division.H_L_accumulation << std::endl;
            std::cerr << "Expected H_H: " << final_selected.H_H_accumulation << " Got: " << final_filter_division.H_H_accumulation << std::endl;
        }

}

#pragma once

#ifndef FILTER_PREPROCESS_H
#define FILTER_PREPROCESS_H

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <map>
#include "FilterReader.h"

extern double balance_weight;     // weight for balance_score 
extern double combine_weight;     // weight for combine_score
extern double storage_weight;     // weight for storage_score

struct Src_Mask
{
    std::uint16_t selected_src_mask;
    std::uint32_t reach_accumulation;
    std::uint32_t remain_accumulation;
};

struct Src_Mask_Bucket{
    std::vector<Src_Mask> mask_bucket;
    std::vector<FilterRule> src_sorting_filters;
    double scale_factor_bottom;
    double scale_factor_top;
};

struct Dst_Mask
{
    std::uint16_t selected_dst_mask;
    std::uint32_t reach_accumulation;
    std::uint32_t remain_accumulation;
};

struct Dst_Mask_Bucket
{
    std::uint16_t selected_src_mask;
    std::uint32_t total_nums;
    std::vector<Dst_Mask> lower_dst_mask_bucket;
    std::vector<Dst_Mask> higher_dst_mask_bucket;
    std::vector<FilterRule> dst_sorting_filters;
    double lower_min_intervel_factor;
    double lower_max_intervel_factor;
    double higher_min_intervel_factor;
    double higher_max_intervel_factor;
};

struct Mask_Checkpoint
{
    std::uint16_t selected_src_mask;
    std::uint16_t selected_lower_dst_mask;
    std::uint16_t selected_higher_dst_mask;
    double lower_balance_score;
    double higher_balance_score;
    double lower_storage_score;
    double higher_storage_score;
    double lower_combine_score;
    double higher_combine_score;
    std::uint32_t L_L_accumulation;
    std::uint32_t L_H_accumulation;
    std::uint32_t H_L_accumulation;
    std::uint32_t H_H_accumulation;  
};



struct Filter_Division
{
    std::uint16_t selected_src_mask;
    std::uint16_t selected_lower_dst_mask;
    std::uint16_t selected_higher_dst_mask;
    std::uint32_t field_rules_num[4];   // 4 sub-rulesets
    std::uint32_t L_L_accumulation;
    std::uint32_t L_H_accumulation;
    std::uint32_t H_L_accumulation;
    std::uint32_t H_H_accumulation; 
    std::vector<FilterRule> Sub_L_L;    // sub-ruleset, src_mask < selected_src_mask; dst_mask < selected_dst_mask
    std::vector<FilterRule> Sub_L_H;    // sub-ruleset, src_mask < selected_src_mask; dst_mask >= selected_dst_mask
    std::vector<FilterRule> Sub_H_L;    // sub-ruleset, src_mask >= selected_src_mask; dst_mask < selected_dst_mask
    std::vector<FilterRule> Sub_H_H;    // sub-ruleset, src_mask >= selected_src_mask; dst_mask >= selected_dst_mask

};



/* 
Calculate_Storage_Optimiztion
Purpose: Calculate the storage optimization score based on the mask length -> the shorter the mask length, the better the optimization effect.
Parameters:
    mask_length -> The length of the mask used for optimization.
Returns: A normalized storage score, where a higher value indicates better performance.
*/
double Calculate_Storage_Score(std::uint16_t mask_length);

/* 
Calculate_Balance_Score
Purpose: calculate the balance score based on the distribution of rules -> the closer every parts to 0.25, the better
Parameters:
    low_accumulation -> Accumulated count of rules in the lower range.
    high_accumulation -> Accumulated count of rules in the higher range.
    total_rules_num -> Total number of rules.
Returns: A normalized balance score, where a higher value indicates better performance.
*/
double Calculate_Balance_Score(std::uint32_t low_accumulation, std::uint32_t high_accumulation, std::uint32_t total_rules_num);

/* 
Calculate_Combine_Score
Purpose: Calculate the combination score to test the effect of mitigating combinatorial explosion.
Parameters:
    src_mask_selected -> Selected source mask length.
    dst_mask_selected -> Selected destination mask length.
Returns: A normalized combination score, where a higher value indicates better performance.
*/
double Calculate_Combine_Score(std::uint16_t src_mask_selected, std::uint16_t dst_mask_selected);

/* 
Evaluation
Purpose: Evaluate the overall score based on balance, storage, and combination scores.
Parameters:
    balance_score -> Score representing the balance of rule distribution.
    storage_score -> Score representing the storage optimization.
    combine_score -> Score representing the effect of mitigating combinatorial explosion.
    enableStorageOptimization -> Flag to enable or disable storage optimization.
    w1 -> Weight for balance score.
    w2 -> Weight for combine score.
    w3 -> Weight for storage score.
Process: "evaluation_result = w1 * balance_score + w2 * combine_score + (enableStorageOpimization ? w3 * storage_score : 0.0)"
Returns: The final evaluation result, where a higher value indicates better performance.
*/
double Evaluation(double balance_score, double storage_score, double combine_score, bool enableStorageOpimization,
    double w1, double w2, double w3);



// Purpose: Computes and returns a structure (Src_Mask_Bucket) containing source address mask buckets based on a given list of filter rules (FilterRule).
// Parameters: filter_rules - A vector containing filter rules.
// Returns: Src_Mask_Bucket structure, which includes mask buckets, reach buckets, and remaining buckets.


/* 
Obtain_Src_mask_bucket
Purpose: Computes and returns a structure (Src_Mask_Bucket) containing selected source address mask buckets(Initial Selection Parameters: 0.4~0.6) of Filter_Rules.
Parameters:
    filter_rules -> A vector containing all filter rules.
Returns: Src_Mask_Bucket structure, which includes mask buckets, reach buckets, and remaining buckets.
*/
Src_Mask_Bucket Obtain_Src_mask_bucket(const std::vector<FilterRule>& filter_rules);

/* 
Obtain_Dst_mask_bucket
Purpose: based on the selected source mask to compute and obtain the destination mask bucket(Initial Selection Parameters: 0.4~0.6).
Parameters:
    src_mask_struct -> Structure containing the selected source mask and related information.
    src_sorting_filters -> Sorted filter rules based on source mask.
    src_scale_factor_bottom -> Lower scaling factor for source mask( the final bottom Selection Parameters ).
    src_scale_factor_top -> Upper scaling factor for source mask( the final top Selection Parameters )
Returns: Dst_Mask_Bucket structure, which includes destination mask buckets for lower and higher ranges.
*/
Dst_Mask_Bucket Obtain_Dst_mask_bucket(Src_Mask& src_mask_struct, std::vector<FilterRule>& src_sorting_filters, std::uint16_t src_scale_factor_bottom, std::uint16_t src_scale_factor_top);

/* 
Decide_Dst_mask_bucket
Purpose: based on the selected source mask and select the destination mask with the best score.
Parameters:
    dst_mask_bucket -> Structure containing destination mask buckets.
Returns: Mask_Checkpoint structure, which includes the selected source and destination masks with their scores.
*/
Mask_Checkpoint Decide_Dst_mask_bucket(Dst_Mask_Bucket& dst_mask_bucket);

/* 
Decide_best_mask_bucket
Purpose: Decide the best mask bucket based on the given filter rules( containing src_mask and dst_mask ).
Parameters:
    filter_rules -> A vector containing filter rules.
Returns: Mask_Checkpoint structure, which includes the best selected source and destination masks with their scores.
*/
Mask_Checkpoint Decide_best_mask_bucket(const std::vector<FilterRule>& filter_rules);

/* 
Filter_Preprocess
Purpose: Preprocess the filter rules to determine the final filter division.
Parameters:
    final_filter_division -> Structure to store the final filter division results.
    filter_rules -> A vector containing filter rules.
*/
void Filter_Preprocess(Filter_Division& final_filter_division, const std::vector<FilterRule>& filter_rules);

#endif
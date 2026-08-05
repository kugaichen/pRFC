
#include <array>
#include <exception>
#include <filesystem>
#include <future>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "Filter_Preprocess.h"
#include "Hardware_ImprovedClassifierBuilding.h"
#include "Hardware_Improved_statemachine.h"

namespace {

void PrintUsage(const char* program_name) {
    std::cout << "Usage: " << program_name
              << " <rules-file> [packet-trace-file]\n\n"
              << "Build the four pRFC classifiers. When a packet trace is "
                 "provided, also run the hardware lookup simulation.\n";
}

bool IsReadableFile(const std::string& path) {
    std::error_code error;
    return std::filesystem::is_regular_file(path, error);
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        PrintUsage(argv[0]);
        return 0;
    }
    if (argc < 2 || argc > 3) {
        PrintUsage(argv[0]);
        return 1;
    }

    const std::string filter_filepath = argv[1];
    if (!IsReadableFile(filter_filepath)) {
        std::cerr << "Error: rules file is not readable: " << filter_filepath << '\n';
        return 1;
    }

    const bool simulate_packets = argc == 3;
    const std::string packet_filepath = simulate_packets ? argv[2] : "";
    if (simulate_packets && !IsReadableFile(packet_filepath)) {
        std::cerr << "Error: packet trace file is not readable: "
                  << packet_filepath << '\n';
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    try {
        std::vector<FilterRule> filter_rules = readFilterRule(filter_filepath);
        if (filter_rules.empty()) {
            std::cerr << "Error: no valid rules were parsed.\n";
            return 1;
        }

        Filter_Division filter_division{};
        Filter_Preprocess(filter_division, filter_rules);

        std::cout << '\n'
                  << "selected_src_mask: " << filter_division.selected_src_mask << '\n'
                  << "selected_lower_dst_mask: "
                  << filter_division.selected_lower_dst_mask << '\n'
                  << "selected_higher_dst_mask: "
                  << filter_division.selected_higher_dst_mask << "\n\n";

        std::cout << "Building L_L, L_H, H_L, and H_H classifiers...\n";
        std::array<std::future<RFC_SE_Chunks>, 4> classifiers;
        for (int flag = 0; flag < 4; ++flag) {
            classifiers[flag] = std::async(
                std::launch::async,
                [&filter_division, flag]() {
                    return Improved_Building_Classifier(filter_division, flag);
                });
        }

        RFC_SE_Classifier classifier;
        classifier.rfc_classifier_L_L = classifiers[0].get();
        classifier.rfc_classifier_L_H = classifiers[1].get();
        classifier.rfc_classifier_H_L = classifiers[2].get();
        classifier.rfc_classifier_H_H = classifiers[3].get();

        const auto end_time = std::chrono::high_resolution_clock::now();
        std::cout << "Construction time: "
                  << std::chrono::duration_cast<std::chrono::microseconds>(
                         end_time - start_time)
                         .count()
                  << " microseconds\n";

        if (simulate_packets) {
            SE_Simulation_Hardware_Behavior(classifier, packet_filepath);
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}

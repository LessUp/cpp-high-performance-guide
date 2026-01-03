/**
 * @file benchmark_properties.cpp
 * @brief Property-based tests for benchmark utilities
 * 
 * Feature: hpc-optimization-guide
 * Property 13: Benchmark JSON Output Validity
 * Validates: Requirements 6.4
 */

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>

#include "../../benchmarks/common/benchmark_utils.hpp"

namespace {

// Simple JSON validation (checks basic structure)
bool is_valid_json_structure(const std::string& json) {
    // Check for opening and closing braces
    if (json.empty()) return false;
    
    int brace_count = 0;
    int bracket_count = 0;
    bool in_string = false;
    char prev_char = 0;
    
    for (char c : json) {
        if (c == '"' && prev_char != '\\') {
            in_string = !in_string;
        }
        
        if (!in_string) {
            if (c == '{') ++brace_count;
            else if (c == '}') --brace_count;
            else if (c == '[') ++bracket_count;
            else if (c == ']') --bracket_count;
            
            if (brace_count < 0 || bracket_count < 0) {
                return false;
            }
        }
        
        prev_char = c;
    }
    
    return brace_count == 0 && bracket_count == 0 && !in_string;
}

// Check if JSON contains required fields
bool has_required_fields(const std::string& json) {
    // Check for required benchmark fields
    return json.find("\"name\"") != std::string::npos &&
           json.find("\"iterations\"") != std::string::npos &&
           json.find("\"real_time\"") != std::string::npos &&
           json.find("\"cpu_time\"") != std::string::npos;
}

// Generate a valid benchmark name
std::string generate_benchmark_name() {
    static const char* prefixes[] = {"BM_", "Bench_", "Test_", ""};
    static const char* operations[] = {"Add", "Multiply", "Sort", "Search", "Hash", "Copy"};
    static const char* suffixes[] = {"_Scalar", "_SIMD", "_Parallel", "_Sequential", ""};
    
    std::string name;
    name += prefixes[rand() % 4];
    name += operations[rand() % 6];
    name += suffixes[rand() % 5];
    return name;
}

} // anonymous namespace

/**
 * Property 13: Benchmark JSON Output Validity
 * 
 * For any completed benchmark run, the JSON output SHALL be valid JSON
 * and contain required fields: name, iterations, real_time, cpu_time.
 * 
 * Validates: Requirements 6.4
 */
RC_GTEST_PROP(BenchmarkJSONProperties, ValidJSONStructure, ()) {
    // Generate random benchmark results
    const int num_results = *rc::gen::inRange(1, 20);
    
    std::vector<hpc::bench::BenchmarkResult> results;
    results.reserve(num_results);
    
    for (int i = 0; i < num_results; ++i) {
        hpc::bench::BenchmarkResult result;
        result.name = "BM_Test_" + std::to_string(i);
        result.iterations = *rc::gen::inRange<int64_t>(1, 1000000);
        result.real_time_ns = *rc::gen::inRange(1, 1000000) * 1.0;
        result.cpu_time_ns = *rc::gen::inRange(1, 1000000) * 1.0;
        result.bytes_per_second = *rc::gen::inRange(0, 1000000000) * 1.0;
        result.items_per_second = *rc::gen::inRange(0, 1000000000) * 1.0;
        results.push_back(result);
    }
    
    // Export to temporary file
    std::string temp_file = "/tmp/benchmark_test_" + std::to_string(rand()) + ".json";
    
    try {
        hpc::bench::export_to_json(temp_file, results);
        
        // Read back the file
        std::ifstream file(temp_file);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        
        // Validate JSON structure
        RC_ASSERT(is_valid_json_structure(json));
        
        // Validate required fields
        RC_ASSERT(has_required_fields(json));
        
        // Cleanup
        std::filesystem::remove(temp_file);
    } catch (const std::exception& e) {
        std::filesystem::remove(temp_file);
        RC_FAIL("Exception: " + std::string(e.what()));
    }
}

/**
 * Property 13: Benchmark JSON Output Validity - Contains All Results
 * 
 * The JSON output should contain all benchmark results.
 * 
 * Validates: Requirements 6.4
 */
RC_GTEST_PROP(BenchmarkJSONProperties, ContainsAllResults, ()) {
    const int num_results = *rc::gen::inRange(1, 10);
    
    std::vector<hpc::bench::BenchmarkResult> results;
    std::vector<std::string> names;
    
    for (int i = 0; i < num_results; ++i) {
        hpc::bench::BenchmarkResult result;
        result.name = "BM_Unique_" + std::to_string(i) + "_" + std::to_string(rand());
        result.iterations = 1000;
        result.real_time_ns = 1000.0;
        result.cpu_time_ns = 1000.0;
        results.push_back(result);
        names.push_back(result.name);
    }
    
    std::string temp_file = "/tmp/benchmark_test_" + std::to_string(rand()) + ".json";
    
    try {
        hpc::bench::export_to_json(temp_file, results);
        
        std::ifstream file(temp_file);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        
        // Check all names are present
        for (const auto& name : names) {
            RC_ASSERT(json.find(name) != std::string::npos);
        }
        
        std::filesystem::remove(temp_file);
    } catch (const std::exception& e) {
        std::filesystem::remove(temp_file);
        RC_FAIL("Exception: " + std::string(e.what()));
    }
}

/**
 * Property 13: Benchmark JSON Output Validity - Numeric Values
 * 
 * Numeric values should be properly formatted in JSON.
 * 
 * Validates: Requirements 6.4
 */
RC_GTEST_PROP(BenchmarkJSONProperties, NumericValuesValid, ()) {
    hpc::bench::BenchmarkResult result;
    result.name = "BM_NumericTest";
    result.iterations = *rc::gen::inRange<int64_t>(1, 1000000000);
    result.real_time_ns = *rc::gen::inRange(1, 1000000) * 0.123;
    result.cpu_time_ns = *rc::gen::inRange(1, 1000000) * 0.456;
    
    std::vector<hpc::bench::BenchmarkResult> results = {result};
    std::string temp_file = "/tmp/benchmark_test_" + std::to_string(rand()) + ".json";
    
    try {
        hpc::bench::export_to_json(temp_file, results);
        
        std::ifstream file(temp_file);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        
        // Check iterations is a valid integer
        std::regex iter_regex("\"iterations\":\\s*(\\d+)");
        std::smatch match;
        RC_ASSERT(std::regex_search(json, match, iter_regex));
        
        // Check times are valid numbers
        std::regex time_regex("\"real_time\":\\s*([\\d.]+)");
        RC_ASSERT(std::regex_search(json, match, time_regex));
        
        std::filesystem::remove(temp_file);
    } catch (const std::exception& e) {
        std::filesystem::remove(temp_file);
        RC_FAIL("Exception: " + std::string(e.what()));
    }
}

/**
 * Property 13: Benchmark JSON Output Validity - Result Validation
 * 
 * validate_result should correctly identify valid and invalid results.
 * 
 * Validates: Requirements 6.4
 */
RC_GTEST_PROP(BenchmarkJSONProperties, ResultValidation, ()) {
    // Valid result
    hpc::bench::BenchmarkResult valid_result;
    valid_result.name = "BM_Valid";
    valid_result.iterations = *rc::gen::inRange<int64_t>(1, 1000000);
    valid_result.real_time_ns = *rc::gen::inRange(1, 1000000) * 1.0;
    valid_result.cpu_time_ns = *rc::gen::inRange(1, 1000000) * 1.0;
    
    RC_ASSERT(hpc::bench::validate_result(valid_result));
    
    // Invalid: empty name
    hpc::bench::BenchmarkResult invalid_name;
    invalid_name.name = "";
    invalid_name.iterations = 1000;
    invalid_name.real_time_ns = 1000.0;
    invalid_name.cpu_time_ns = 1000.0;
    
    RC_ASSERT(!hpc::bench::validate_result(invalid_name));
    
    // Invalid: zero iterations
    hpc::bench::BenchmarkResult invalid_iter;
    invalid_iter.name = "BM_Test";
    invalid_iter.iterations = 0;
    invalid_iter.real_time_ns = 1000.0;
    invalid_iter.cpu_time_ns = 1000.0;
    
    RC_ASSERT(!hpc::bench::validate_result(invalid_iter));
}

// Standard GTest for edge cases
TEST(BenchmarkUtilsTests, EmptyResultsExport) {
    std::vector<hpc::bench::BenchmarkResult> empty_results;
    std::string temp_file = "/tmp/benchmark_empty_test.json";
    
    EXPECT_NO_THROW(hpc::bench::export_to_json(temp_file, empty_results));
    
    std::ifstream file(temp_file);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();
    
    EXPECT_TRUE(is_valid_json_structure(json));
    EXPECT_TRUE(json.find("\"benchmarks\": []") != std::string::npos ||
                json.find("\"benchmarks\":[]") != std::string::npos);
    
    std::filesystem::remove(temp_file);
}

TEST(BenchmarkUtilsTests, SpeedupCalculation) {
    EXPECT_DOUBLE_EQ(hpc::bench::calculate_speedup(100.0, 50.0), 2.0);
    EXPECT_DOUBLE_EQ(hpc::bench::calculate_speedup(100.0, 100.0), 1.0);
    EXPECT_DOUBLE_EQ(hpc::bench::calculate_speedup(100.0, 200.0), 0.5);
    EXPECT_DOUBLE_EQ(hpc::bench::calculate_speedup(100.0, 0.0), 0.0);
}

TEST(BenchmarkUtilsTests, ThroughputFormatting) {
    EXPECT_EQ(hpc::bench::format_throughput(500), "500.00 B/s");
    EXPECT_EQ(hpc::bench::format_throughput(1024), "1.00 KB/s");
    EXPECT_EQ(hpc::bench::format_throughput(1024 * 1024), "1.00 MB/s");
    EXPECT_EQ(hpc::bench::format_throughput(1024 * 1024 * 1024), "1.00 GB/s");
}

TEST(BenchmarkUtilsTests, TimeFormatting) {
    EXPECT_EQ(hpc::bench::format_time(500), "500.00 ns");
    EXPECT_EQ(hpc::bench::format_time(5000), "5.00 us");
    EXPECT_EQ(hpc::bench::format_time(5000000), "5.00 ms");
    EXPECT_EQ(hpc::bench::format_time(5000000000), "5.00 s");
}

TEST(BenchmarkUtilsTests, TimerBasicUsage) {
    hpc::bench::Timer timer;
    
    timer.start();
    // Do some work
    volatile int sum = 0;
    for (int i = 0; i < 10000; ++i) {
        sum += i;
    }
    timer.stop();
    
    EXPECT_GT(timer.elapsed_ns(), 0);
    EXPECT_GT(timer.elapsed_us(), 0);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>
#include <tuple>
#include <time.h>

#include "eckit/testing/Test.h"

#include "odc/api/Odb.h"

using namespace eckit::testing;

// ------------------------------------------------------------------------------------------------------

std::tuple<std::string, int> test_run_command(std::string command) {
    FILE * fp_command;
    const int max_buffer = 256;
    char buffer[max_buffer];
    std::string command_output;
    int command_exit_status;
    int command_exit_code;

    // Redirect stderr to stdout
    command.append(" 2>&1");

    fp_command = popen(command.c_str(), "r");

    ASSERT(fp_command != NULL);

    if (fp_command) {
        while (!feof(fp_command)) {
            if (fgets(buffer, max_buffer, fp_command) != NULL) {
                command_output.append(buffer);
            }
        }

        command_exit_status = pclose(fp_command);
        ASSERT(WIFEXITED(command_exit_status));
        command_exit_code = WEXITSTATUS(command_exit_status);
    }

    return std::make_tuple(command_output, command_exit_code);
}

std::string test_read_text_file(const std::string &fileName) {
    std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return std::string(bytes.data(), fileSize);
}

bool test_check_file_exists(std::string file_path) {
    std::ifstream file_stream(file_path);
    return file_stream.good();
}

void test_replace_text(std::string& haystack, const std::string& needle, const std::string& replacement) {
    if (needle.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = haystack.find(needle, start_pos)) != std::string::npos) {
        haystack.replace(start_pos, needle.length(), replacement);
        start_pos += replacement.length();
    }
}

// ------------------------------------------------------------------------------------------------------

CASE("Test C encode row-major example") {
    std::string command_output;
    int command_exit_code;

    // Run command without any arguments
    tie(command_output, command_exit_code) = test_run_command("./odc-c-encode-row-major");

    // Check exit code (error)
    EXPECT_EQUAL(command_exit_code, 1);

    std::string command_expected = test_read_text_file("./odc-c-encode-row-major.1");

    // Check command output
    EXPECT_EQUAL(command_output, command_expected);

    // Run command with a single argument
    tie(command_output, command_exit_code) = test_run_command("./odc-c-encode-row-major test-1.odb");

    // Check exit code (success)
    EXPECT_EQUAL(command_exit_code, 0);

    command_expected = test_read_text_file("./odc-c-encode-row-major.2");

    // Check command output
    EXPECT_EQUAL(command_output, command_expected);
}

CASE("Test C encode custom example") {
    std::string command_output;
    int command_exit_code;

    // Run command without any arguments
    tie(command_output, command_exit_code) = test_run_command("./odc-c-encode-custom");

    // Check exit code (error)
    EXPECT_EQUAL(command_exit_code, 1);

    std::string command_expected = test_read_text_file("./odc-c-encode-custom.1");

    // Check command output
    EXPECT_EQUAL(command_output, command_expected);

    // Run command with a single argument
    tie(command_output, command_exit_code) = test_run_command("./odc-c-encode-custom test-2.odb");

    // Check exit code (success)
    EXPECT_EQUAL(command_exit_code, 0);

    command_expected = test_read_text_file("./odc-c-encode-custom.2");

    // Check command output
    EXPECT_EQUAL(command_output, command_expected);
}

CASE("Test C list program example") {
    std::string command_output;
    int command_exit_code;

    // Run command without any arguments
    tie(command_output, command_exit_code) = test_run_command("./odc-c-ls");

    // Check exit code (error)
    EXPECT_EQUAL(command_exit_code, 1);

    std::string command_expected = test_read_text_file("./odc-c-ls.1");

    // Check command output
    EXPECT_EQUAL(command_output, command_expected);

    ASSERT(test_check_file_exists("test-1.odb"));

    // Run command with a single argument
    tie(command_output, command_exit_code) = test_run_command("./odc-c-ls test-1.odb");

    // Check exit code (success)
    EXPECT_EQUAL(command_exit_code, 0);

    command_expected = test_read_text_file("./odc-c-ls.2");

    // Prepare the current date as a string
    time_t rawtime;
    time(&rawtime);
    struct tm * timeinfo;
    timeinfo = localtime(&rawtime);
    std::string date = std::to_string(10000 * (timeinfo->tm_year + 1900)
                                      + 100 * (timeinfo->tm_mon + 1)
                                      + timeinfo->tm_mday);

    // Replace placeholder with current date
    test_replace_text(command_expected, "%current_date%", date);

    // Check command output
    EXPECT_EQUAL(command_output, command_expected);
}

CASE("Test C header stats example") {
    std::string command_output;
    int command_exit_code;

    // Run command without any arguments
    tie(command_output, command_exit_code) = test_run_command("./odc-c-header");

    // Check exit code (error)
    EXPECT_EQUAL(command_exit_code, 1);

    std::string command_expected = test_read_text_file("./odc-c-header.1");

    // Check command output
    EXPECT_EQUAL(command_output, command_expected);

    ASSERT(test_check_file_exists("test-1.odb"));
    ASSERT(test_check_file_exists("test-2.odb"));

    // Run command with a single argument
    tie(command_output, command_exit_code) = test_run_command("./odc-c-header test-1.odb test-2.odb");

    // Check exit code (success)
    EXPECT_EQUAL(command_exit_code, 0);

    command_expected = test_read_text_file("./odc-c-header.2");

    // Replace placeholder with current version number
    test_replace_text(command_expected, "%odc_version_placeholder%", odc::api::Settings::version());

    // Check command output
    EXPECT_EQUAL(command_output, command_expected);
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

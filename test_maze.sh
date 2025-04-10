#!/bin/bash
set -eo pipefail  # Strict error checking and pipeline failure handling

# ==============================================================================
# Global Configuration
# ==============================================================================
readonly TEST_DIR="test_data/mazes"  # Maze test files directory
readonly EXECUTABLE="./maze"         # Program executable path
readonly TIMEOUT_DURATION=5          # Test timeout in seconds
declare -i PASS=0                    # Pass counter
declare -i FAIL=0                    # Fail counter

# ==============================================================================
# Test Case Definitions (12 test cases)
# ==============================================================================

# Test Case 01: Valid 5x5 maze with normal operations
declare -A test1=(
    [name]="valid_maze_operations"
    [maze]="valid_5x5.txt"
    [inputs]=$'d\ns\na\nm\nq'        # Valid moves + map display
    [expected_output]=""             # No error messages expected
    [exit_code]=0
)

# Test Case 02: Under-sized maze (4x4)
declare -A test2=(
    [name]="undersized_maze"
    [maze]="invalid_4x4.txt"
    [inputs]=""
    [expected_output]="Invalid maze dimensions"
    [exit_code]=1
)

# Test Case 03: Missing start point (S)
declare -A test3=(
    [name]="missing_start_point"
    [maze]="no_start.txt"
    [inputs]=""
    [expected_output]="Missing start point"
    [exit_code]=1
)

# Test Case 04: Wall collision detection
declare -A test4=(
    [name]="wall_collision_test"
    [maze]="wall_maze.txt"
    [inputs]=$'w\nw\nq'              # Attempt to move into wall
    [expected_output]="Wall collision detected!"
    [exit_code]=0
)

# Test Case 05: Case-insensitive input handling
declare -A test5=(
    [name]="case_insensitive_input"
    [maze]="valid_5x5.txt"
    [inputs]=$'W\nA\nS\nD\nQ'        # Mixed case inputs
    [expected_output]=""
    [exit_code]=0
)

# Test Case 06: Invalid input characters
declare -A test6=(
    [name]="invalid_input_chars"
    [maze]="valid_5x5.txt"
    [inputs]=$'x\nz\n1\nq'           # Non-WASD inputs
    [expected_output]="Invalid input"
    [exit_code]=0
)

# Test Case 07: Invalid maze symbol (@)
declare -A test7=(
    [name]="invalid_maze_symbol"
    [maze]="invalid_char.txt"
    [inputs]=""
    [expected_output]="Invalid maze character"
    [exit_code]=1
)

# Test Case 08: Successful maze completion
declare -A test8=(
    [name]="maze_completion"
    [maze]="simple_path.txt"
    [inputs]=$'d\nd\nd\ns\n'         # Path to exit
    [expected_output]="Congratulations!"
    [exit_code]=0
)

# Test Case 09: Non-existent maze file
declare -A test9=(
    [name]="file_not_found"
    [maze]="ghost_file.txt"
    [inputs]=""
    [expected_output]="Error opening file"
    [exit_code]=1
)

# Test Case 10: Non-rectangular maze
declare -A test10=(
    [name]="non_rectangular_maze"
    [maze]="uneven_rows.txt"
    [inputs]=""
    [expected_output]="Not rectangular"
    [exit_code]=1
)

# Test Case 11: Missing command line arguments
declare -A test11=(
    [name]="missing_arguments"
    [maze]=""                        # No filename provided
    [inputs]=""
    [expected_output]="Usage:.*maze" # Regex pattern for usage message
    [exit_code]=1
)

# Test Case 12: Extra command line arguments
declare -A test12=(
    [name]="extra_arguments"
    [maze]="valid_5x5.txt dummy"     # Extra argument
    [inputs]=""
    [expected_output]="Usage:.*maze" # Regex pattern for usage message
    [exit_code]=1
)

# ==============================================================================
# Helper Functions
# ==============================================================================

# Validate test environment setup
validate_environment() {
    # Check test data directory
    if [ ! -d "$TEST_DIR" ]; then
        echo "ERROR: Test data directory missing: $TEST_DIR" >&2
        exit 1
    fi
    
    # Check executable existence and permissions
    if [ ! -x "$EXECUTABLE" ]; then
        echo "ERROR: Executable not found or not runnable: $EXECUTABLE" >&2
        exit 1
    fi
}

# Execute single test case with proper error handling
execute_test() {
    local -n test_case=$1            # Reference to test case array
    local test_name=${test_case[name]}
    local maze_args=(${test_case[maze]})
    local inputs=${test_case[inputs]}
    local expected_output=${test_case[expected_output]}
    local expected_exit=${test_case[exit_code]}

    echo "=== RUNNING TEST: $test_name ==="

    # Build command with arguments
    local command="$EXECUTABLE"
    [ -n "${maze_args[0]}" ] && command+=" ${TEST_DIR}/${maze_args[0]}"
    [ -n "${maze_args[1]}" ] && command+=" ${maze_args[1]}"

    # Execute with timeout and capture output
    local output
    output=$(timeout $TIMEOUT_DURATION bash -c "echo -e \"$inputs\" | $command 2>&1")
    local actual_exit=$?

    # Handle timeout
    if [ $actual_exit -eq 124 ]; then
        echo "[FAIL] Test timed out after ${TIMEOUT_DURATION}s"
        ((FAIL++))
        return
    fi

    # Verify results using regex matching
    if [[ "$output" =~ $expected_output ]] && [ $actual_exit -eq $expected_exit ]; then
        echo "[PASS]"
        ((PASS++))
    else
        echo "[FAIL] Expected output pattern: '$expected_output'"
        echo "       Actual output: '$output'"
        echo "       Expected exit code: $expected_exit, Actual: $actual_exit"
        ((FAIL++))
    fi
}

# ==============================================================================
# Main Execution Flow
# ==============================================================================

# Initial environment validation
validate_environment

# List of all test cases to execute
declare -a test_cases=(
    test1 test2 test3 test4 test5 test6
    test7 test8 test9 test10 test11 test12
)

# Execute all test cases
for test in "${test_cases[@]}"; do
    execute_test "$test"
    echo "----------------------------------------"
done

# Print final test summary
echo "TEST SUMMARY"
echo "============"
echo "Total tests executed: ${#test_cases[@]}"
echo "Passed: $PASS"
echo "Failed: $FAIL"

# Set exit code based on test results
[ $FAIL -eq 0 ] && exit 0 || exit 1
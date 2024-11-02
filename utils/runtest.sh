#!/bin/bash

# Expected output (can be set here or read from a file)
EXPECTED_OUTPUT='Import(Id(moduleA)), Import(Id(moduleB)), Let(n=Id(x), t=Id(Int64)), Let(n=Id(y), t=Id(Int64), i=Int(10, 5)), Let(n=Id(z), i=OP_MINUS(l=OP_PLUS(l=Int(10, 3), r=OP_MULT(l=Int(10, 4), r=Int(10, 2))), r=Int(10, 1))), Func(n=Id(multiply), a=FuncArgs([FArg(n=Id(a), t=Id(Int64))], [FArg(n=Id(b), t=Id(Int64))]), r=Id(Int64), s=[Let(n=Id(result), t=Id(Int64), i=Int(10, 32)), Id(result)]), If(?=Id(x), then=[Let(n=Id(msg), t=Id(String), i=Str("Hello World!"))], else=[If(?=Id(y), then=[If(?=Id(z), then=[Let(n=Id(nested), t=Id(String), i=Str("Nested If"))], else=[Let(n=Id(alt), t=Id(Int64), i=OP_DIVF(l=Int(10, 10), r=Int(10, 2)))])], else=[Let(n=Id(fallback), t=Id(String), i=Str("Hello"))])]), Func(n=Id(emptyFunc), a=FuncArgs([FArg(n=Id(a), t=Id(Int64))]), r=Id(Void), s=[]), If(?=Id(y), then=[Let(n=Id(conditionMet), t=Id(Int64), i=Int(10, 1))], else=[Let(n=Id(alt1), t=Id(Int64), i=Int(10, 20)), If(?=Id(z), then=[Let(n=Id(subCondition), t=Id(String), i=Str("Sub"))], else=[Let(n=Id(subAlt), t=Id(Int64), i=Int(10, 30))]), Let(n=Id(endCondition), t=Id(Int64), i=Int(10, 40))]), Let(n=Id(complexExpr), i=OP_DIVF(l=OP_MULT(l=OP_PLUS(l=Int(10, 3), r=Int(10, 2)), r=OP_MINUS(l=Int(10, 4), r=OP_MULT(l=OP_PLUS(l=Int(10, 2), r=Int(10, 1)), r=Int(10, 5)))), r=OP_PLUS(l=Int(10, 2), r=Int(10, 3)))), Assign(l=Id(x), r=OP_PLUS(l=Int(10, 1), r=OP_MULT(l=Int(10, 5), r=Int(10, 3)))), Class(n=Id(Test), s=CStmtList([Func(n=Id(testFunc), a=FuncArgs([FArg(n=Id(a), t=Id(Int64))], [FArg(n=Id(b), t=Id(String))], [FArg(n=Id(c), t=Id(Boolean))]), r=Id(Void), s=[Let(n=Id(localVar), t=Id(Int64), i=Int(10, 10)), Let(n=Id(localStr), t=Id(Str), i=Str("test")), If(?=Id(c), then=[Assign(l=Id(localVar), r=OP_MULT(l=Int(10, 4), r=Int(10, 2)))], else=[Assign(l=Id(localVar), r=OP_PLUS(l=Int(10, 1), r=Int(10, 5)))])])]))'

# Get current working directory
current_dir=$(pwd)

# Log file
LOG_FILE="kirazc_test.log"

# Temporary files for comparison
EXPECTED_FILE=$(mktemp)
OUTPUT_FILE=$(mktemp)

# Save expected output to a temp file
echo "$EXPECTED_OUTPUT" > "$EXPECTED_FILE"

# Run the command and capture output
cd "../build"
OUTPUT=$(./kirazc -f "${current_dir}/test.ki" 2>&1)
cd "$current_dir"

# Save actual output to a temp file
echo "$OUTPUT" > "$OUTPUT_FILE"

# Get the current timestamp
TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")

# Compare the output to the expected value and capture only differences
DIFF_OUTPUT=$(diff "$EXPECTED_FILE" "$OUTPUT_FILE")

# Determine success or failure based on the diff output
if [[ -z "$DIFF_OUTPUT" ]]; then
    STATUS="SUCCESS"
    MESSAGE="Output matches expected value."
else
    STATUS="FAILURE"
    MESSAGE="Output does not match expected value."
fi

# Log the results
{
    echo "[$TIMESTAMP] Status: $STATUS"
    echo "Command: ./kirazc -f 'test.ki'"
    echo "Expected Output: $EXPECTED_OUTPUT"
    echo "Actual Output: $OUTPUT"
    echo "Differences:"
    echo "$DIFF_OUTPUT"
    echo "Message: $MESSAGE"
    echo "------------------------------------"
} >> "$LOG_FILE"

# Print the status to the terminal
echo "Test $STATUS. Check $LOG_FILE for details."

# Clean up temporary files
rm "$EXPECTED_FILE" "$OUTPUT_FILE"


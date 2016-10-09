#!/bin/bash

if [[ "$1" == "-d" ]]
then
    display_diffs=1
    program_name=$2
    test_files_dir=$3
else
    display_diffs=0
    program_name=$1
    test_files_dir=$2
fi

echo "Testing: '$program_name' with files in: '$test_files_dir/'"

query_failures=0
insertions_failures=0

for input in ${test_files_dir}/*.in; do
    echo ""
    echo "Testing: ${input%.*}..."
    ./${program_name%.*} < ${input} >temp_stdout.out 2>temp_stderr.out

    if [[ ${display_diffs} == 0 ]]
    then
    diff -q "${input%.*}.err" temp_stderr.out 1>/dev/null
    else
    diff "${input%.*}.err" temp_stderr.out
    fi

    if [[ $? == "0" ]]
    then
        echo "insertions: success"
    else
        echo "insertions: failure"
        ((insertions_failures++))
    fi

    if [[ ${display_diffs} == 0 ]]
    then
    diff -q "${input%.*}.out" temp_stdout.out 1>/dev/null
    else
    diff "${input%.*}.out" temp_stdout.out
    fi

    if [[ $? == "0" ]]
    then
        echo "queries: success"
    else
        echo "queries: failure"
        ((query_failures++))
    fi
done

rm temp_stdout.out
rm temp_stderr.out

echo "TESTS FAILED: ${query_failures}"
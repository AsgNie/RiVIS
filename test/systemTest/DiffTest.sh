#!/usr/bin/bash
# Expects arg1 to be path to RiVIS, and arg2 to be folder name

binFiles=$(ls $2/*.bin)


# Create test case by removing .bin file-extension from binFiles with parameter substitution
for line in $binFiles; do
    tests+="${line%.*} "
done

sum=0
for i in $tests; do
    $1 -i $i.bin -o $i.riv  # Call RiVIS
    (diff $i.res $i.riv)    # diff will print info to stderr if files do not match
    diffVal=$?              # Capture exit value of diff (0:success, 1:diff, 2:error)
    sum=$(($sum + $diffVal))
done

# Clean-up
rm $2/*.riv

if [ -z "$tests" ]; then    # Check for no test cases
    exit 1
elif [ $sum != 0 ]; then    # Check for diffs
    exit 1
else
    exit 0                  # Exit SUCCESS
fi

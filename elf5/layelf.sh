#!/bin/bash

# Check for the executable file argument
if [ -z "$1" ]; then
    echo "Usage: $0 <elf-file>"
    exit 1
fi

EXECUTABLE=$1

# AWK script to parse segments and sections, now with more robust parsing
awk '
# BEGIN block runs once at the start to print the table header
BEGIN {
    printf "%-12s %-7s %-20s %-18s %-12s %s\n", "SEGMENT", "[Nr]", "NAME", "VMA", "SIZE", "FLAGS";
    print "----------------------------------------------------------------------------------------";
}

# FNR==NR is true only for the first input (segment info from readelf -l)
FNR==NR {
    if ($1 == "LOAD") {
        vma_start = strtonum("0x" $3)
        memsz = strtonum("0x" $5)
        
        # We only care about segments that will actually occupy memory
        if (vma_start > 0 && memsz > 0) {
            load_segments[++count] = vma_start
            load_segments_end[count] = vma_start + memsz
        }
    }
    next # Move to the next line of the first input
}

# This block runs for the second input (section info from readelf -S -W)
{
    # *** THIS IS THE KEY FIX ***
    # A real section line starts with a bracketed number like "[10]".
    # We check if the first field matches this pattern. This filters out
    # the malformed header lines from your readelf output.
    if ($1 !~ /^\[[[:space:]]*[0-9]+\]$/) {
        next # If it is not a valid section line, skip it.
    }

    # Extract the required fields from the line
    sec_idx = $1
    sec_name = $2
    sec_vma_hex = $4
    sec_size_hex = $6
    sec_flags = $8

    sec_vma = strtonum("0x" sec_vma_hex)
    sec_size = strtonum("0x" sec_size_hex)
    sec_end = sec_vma + sec_size

    segment_info = "-" # Default for sections not loaded into memory
    
    # Check if the section falls within any of the PT_LOAD segments
    if (sec_vma > 0) {
        for (i = 1; i <= count; i++) {
            if (sec_vma >= load_segments[i] && sec_end <= load_segments_end[i]) {
                # If it is, mark it with its segment number and color it green
                segment_info = sprintf("\033[0;32mLOAD #%d\033[0m", i-1)
                break
            }
        }
    }
    
    # Print the clean, formatted output for this section
    printf "%-12s %-7s %-20s %-18s %-12s %s\n", segment_info, sec_idx, sec_name, sec_vma_hex, sec_size_hex, sec_flags
}
' <(readelf -l "$EXECUTABLE") <(readelf -S -W "$EXECUTABLE")

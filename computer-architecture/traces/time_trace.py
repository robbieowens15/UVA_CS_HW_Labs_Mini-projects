#!/usr/bin/python3
import argparse
import csv

"""
Simulate based on the instruction trace read from 'fh'.

This is meant to represent a simple five-stage pipeline with:
* load/use hazards
* branch delay penalty for non-taken branches

The stalls introduced by each of these are assumed to be independent. (For example,
if a load is a cache miss followed by a load/use hazard, this assumes that both the
full cache miss penalty plus the full load/use hazard penalty must be taken,
even though maybe a processor could perhaps use clever forwarding to avoid the extra cycle
of load/use hazard penalty.)

Takes arguments:
    args: contains the command-line arguments passed to this program. If you want to
          support additional command line arguments, add new `add_argument` calls
          in main()
    fh: the file handle for the trace CSV file

Returns a dictionary containing:
    branch_delay: estimated delay from branch mispredictions
    load_use_delay: estimated delay from load_use hazards
    delay: total delay of all types
    cycles: total simualted cycles
    instructions: total instructions processed
"""
def count_time_in(args, fh):
    csv_reader = csv.DictReader(fh)

    # last instruction, for detecting load/use hazards
    last_instruction = None

    # number instructions processed
    num_instructions = 0

    # total cycles accounted to load/use hazards
    load_use_delay = 0

    # total cycles account to branch misprediction penalties
    branch_delay = 0

    #Remember decision from last branching instruction
    conditional_branch_dict ={}

    for instruction in csv_reader:
        if last_instruction != None:
            forward_from_last = (
                last_instruction['dst'] != '' and
                last_instruction['dst'] in (instruction['srcA'], instruction['srcB'])
            )
            if args.load_use_hazard and forward_from_last and (args.split_execute or last_instruction['is_memory_read'] == 'Y'):
                load_use_delay += 1
        if  (not args.smart_branch_prediction and args.branch_prediction and instruction['is_conditional_branch'] == 'Y' and instruction['branch_taken'] == 'N') or (args.smart_branch_prediction and (instruction['orig_pc'] in conditional_branch_dict and conditional_branch_dict[instruction['orig_pc']] != instruction['branch_taken']) or (instruction['orig_pc'] not in conditional_branch_dict and instruction['branch_taken'] == "N")):
            branch_delay += args.branch_delay
        last_instruction = instruction
        num_instructions += 1

        if instruction['is_conditional_branch'] == 'Y':
            conditional_branch_dict[instruction['orig_pc']] = instruction['branch_taken']

    print(branch_delay)
    
    return {
        'load_use_delay': load_use_delay,
        'branch_delay': branch_delay,
        'delay': load_use_delay + branch_delay,
        'cycles': num_instructions + load_use_delay + branch_delay,
        'instructions': num_instructions,
    }

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('input', type=argparse.FileType('r'), metavar='INPUT-FILE')
    parser.add_argument('--enable-load-use-hazard', action='store_true', dest='load_use_hazard',
        help='enable simulated load/use hazard (default)',
        default=True,
    )
    parser.add_argument('--disable-load-use-hazard', action='store_false', dest='load_use_hazard',
        help='disable simulated load/use hazard'
    )
    parser.add_argument('--branch-delay', default=2, type=int, help='branch delay in cycles')

    parser.add_argument('--enable-branch-prediction', action='store_true',
        dest='branch_prediction', help='disable simulated branch prediction', default=True)
    parser.add_argument('--disable-branch-prediction', action='store_false',
        dest='branch_prediction', help='disable simulated branch prediction')

    parser.add_argument('--enable-split-execute', action='store_true',
        dest='split_execute', help='enable simulated split execute into 2 stages')
    parser.add_argument('--disable-split-execute', action='store_false',
        dest='split_execute', help='disable simulated split execute into 2 stages', default=True)

    parser.add_argument('--enable-smart-branch-prediction', action='store_true',
        dest='smart_branch_prediction', help='enable simulated smart branch prediction (based on last time instruction was seen)')
    parser.add_argument('--disable-smart-branch-prediction', action='store_false',
        dest='smart_branch_prediction', help='disable simulated smart branch prediction (based on last time instruction was seen)', default=True)

    args = parser.parse_args()
    result = count_time_in(args, args.input)
    print("Total cycles", result['cycles'])
    print("Total instructions", result['instructions'])
    print("Total branch delay", result['branch_delay'])
    print("Total load/use hazard delay", result['load_use_delay'])
    print("Total delay", result['delay'])

if __name__ == '__main__':
    main()

# Created by Dylan Johnson and Cody Ohlsen
# Modified by Kevin Bi
# Modified by John Feltrup
# Modified by Arrvindh Shriraman
import json
import re
import subprocess
import sys

# First category of binary operation restrictions
ff_bad_binop = re.compile(".*Binop: ((" + \
                          ")|(" + re.escape("%") + \
                          ")|(" + re.escape("%=") + \
                          ")|(" + re.escape("/") + \
                          ")|(" + re.escape("/=") + \
                          ")|(" + re.escape("|") + \
                          ")|(" + re.escape("|=") + \
                          ")|(" + re.escape("&") + \
                          ")|(" + re.escape("&=") + \
                          ")|(" + re.escape("^") + \
                          ")|(" + re.escape("^=") + \
                          ")|(" + re.escape("&&") + \
                          ")|(" + re.escape("||") + \
                          ")|(" + re.escape(">>") + \
                          ")|(" + re.escape("<<") + \
                          ")|(" + re.escape(">>=") + \
                          ")|(" + re.escape("<<=") + \
                          ")|(" + re.escape("==") + \
                          ")|(" + re.escape("!=") + \
                          ")|(" + re.escape(">") + \
                          ")|(" + re.escape("<") + \
                          ")|(" + re.escape(">=") + \
                          ")|(" + re.escape("<=") + "))\s*$")
# First category of unary operation restrictions
ff_bad_unary = re.compile(".*Unary: ((" + re.escape("~") + \
                          ")|(" + re.escape("-") + "))\s*$")
# Second category of binary operation restrictions
sf_bad_binop = re.compile(".*Binop: ((" + \
                          ")|(" + re.escape("%") + \
                          ")|(" + re.escape("%=") + \
                          ")|(" + re.escape("/") + \
                          ")|(" + re.escape("/=") + \
                          ")|(" + re.escape("|") + \
                          ")|(" + re.escape("|=") + \
                          ")|(" + re.escape("&") + \
                          ")|(" + re.escape("&=") + \
                          ")|(" + re.escape("&&") + \
                          ")|(" + re.escape("||") + \
                          ")|(" + re.escape("!=") + \
                          ")|(" + re.escape(">") + \
                          ")|(" + re.escape("<") + \
                          ")|(" + re.escape(">=") + \
                          ")|(" + re.escape("<=") + "))\s*$")
# Third category of binary operation restrictions
tf_bad_binop = re.compile(".*Binop: ((" + \
                          ")|(" + re.escape("%") + \
                          ")|(" + re.escape("%=") + \
                          ")|(" + re.escape("/") + \
                          ")|(" + re.escape("/=") + \
                          ")|(" + re.escape("|") + \
                          ")|(" + re.escape("|=") + \
                          ")|(" + re.escape("&") + \
                          ")|(" + re.escape("&=") + \
                          ")|(" + re.escape("^") + \
                          ")|(" + re.escape("^=") + \
                          ")|(" + re.escape("&&") + \
                          ")|(" + re.escape("||") + \
                          ")|(" + re.escape(">>") + \
                          ")|(" + re.escape("<<") + \
                          ")|(" + re.escape(">>=") + \
                          ")|(" + re.escape("<<=") + "))\s*$")
# Second category of unary operation restrictions
sf_bad_unary = re.compile(".*Unary: ((" + re.escape("-") + "))\s*$")
# Constant restrictions
af_bad_constant = re.compile(".*Const:.*(25[6-9]|2[6-9][0-9]|[3-9][0-9][0-9]|\d{4,})U?\s*$")
# Array indexing restrictions
af_bad_indexing = re.compile(".*Array:\s*$")

restrict = {
    'intSize': (ff_bad_binop, ff_bad_unary, af_bad_constant, af_bad_indexing),
    'doubleSize': (ff_bad_binop, ff_bad_unary, af_bad_constant, af_bad_indexing),
    'pointerSize': (ff_bad_binop, ff_bad_unary, af_bad_constant, af_bad_indexing),
    'changeValue': (sf_bad_binop, sf_bad_unary, af_bad_constant, af_bad_indexing),
    'withinSameBlock': (sf_bad_binop, sf_bad_unary, af_bad_constant, af_bad_indexing),
    'withinArray': (sf_bad_binop, sf_bad_unary, af_bad_constant, af_bad_indexing),
    'swapInts': (ff_bad_binop, ff_bad_unary, af_bad_constant, af_bad_indexing),
    'stringLength': (tf_bad_binop, ff_bad_unary, af_bad_constant, af_bad_indexing),
    'endianExperiment': (ff_bad_binop, ff_bad_unary, af_bad_constant, af_bad_indexing),
    'selectionSort': (tf_bad_binop, ff_bad_unary, af_bad_constant, af_bad_indexing),
    'smallest_idx': (ff_bad_unary, af_bad_constant, af_bad_indexing)
}

grading = len(sys.argv) == 2 and sys.argv[1] == '-g'

if grading:
    grades = [int(x) for x in sys.stdin.readline().split('\t')]
    func_points = {
        'intSize': grades[0],
        'doubleSize': grades[1],
        'pointerSize': grades[2],
        'changeValue': grades[3],
        'withinSameBlock': grades[4],
        'withinArray': grades[5],
        'swapInts': grades[6],
        'stringLength': grades[7],
        'endianExperiment': grades[8],
        'selectionSort': grades[9],
        'smallest_idx': grades[10]
    }
else:
    func_points = {
        'intSize': 1,
        'doubleSize': 1,
        'pointerSize': 1,
        'changeValue': 1,
        'withinSameBlock': 3,
        'withinArray': 3,
        'swapInts': 1,
        'stringLength': 1,
        'endianExperiment': 3,
        'selectionSort': 3,
        'smallest_idx': 1,
    }
    grades = [1, 1, 1, 1, 3, 3, 1, 1, 3, 3, 1]

try:
    dlc_output = subprocess.check_output(['./dlc', '-ast', './pointer.c'], stderr=subprocess.STDOUT)
except subprocess.CalledProcessError as e:
    dlc_output = e.output

pointer_funcs = dlc_output.split("Proc:\n  Decl: ")

if len(pointer_funcs) != 12:
    print
    "dlc.py detects more functions then specified"
#  sys.exit(0)

if "undeclared!" in dlc_output:
    print
    "dlc.py failed due to out of order declaration, check pointer.c manually"
    sys.exit(0)

seen_bad_ops = {
    'intSize': [],
    'doubleSize': [],
    'pointerSize': [],
    'swapInts': [],
    'changeValue': [],
    'withinSameBlock': [],
    'withinArray': [],
    'stringLength': [],
    'endianExperiment': [],
    'selectionSort': [],
    'smallest_idx': []
}
final_grade = []
skip_next_line = False
seen_given_bad_op = False
last_line_return = False
for func, grade in zip(pointer_funcs[1:], grades):
    lines = func.split("\n");
    name = lines[0].split(" ")[0];
    for line in func.split("\n"):
        # skip over any constant values that preceed a "Value:"
        # ast node, these are computed constants, not ones that the
        # student has written themselves.
        if skip_next_line:
            skip_next_line = False
            continue
        if "Value:" in line:
            skip_next_line = True

        for i in range(len(restrict[name])):
            res = restrict[name][i].match(line)
            if res:
                bad_op = res.group(0).lstrip(" ")
                # changeValue is given an index element access, only record > 1 of these
                # in this function
                if not seen_given_bad_op and "Array:" in bad_op and "changeValue" in name:
                    seen_given_bad_op = True
                    continue
                # change array bad ops to convey more information
                if "Array:" in bad_op:
                    bad_op = "Array Indexing []"
                # add this bad_op to the list of currently known bad ops and dock any points
                if 'Const' in bad_op and 'Const' in str(seen_bad_ops[name]):
                    # if we have seen a bad constant before, don't dock more points
                    # but still add it to the list
                    seen_bad_ops[name].append(bad_op)
                elif bad_op not in seen_bad_ops[name]:
                    func_points[name] -= 1
                    if grade:
                        grade -= 1
                    seen_bad_ops[name].append(bad_op)

        # Check to see if the code returns a constant value.
        # In this case, we check to see if the line after "Return:"
        # contains "Const:"
        if last_line_return:
            last_line_return = False
            if 'Const' in line:
                bad_op = "Returning Constant"
                if bad_op not in seen_bad_ops[name]:
                    func_points[name] -= 1
                    if grade:
                        grade -= 1
                    seen_bad_ops[name].append(bad_op)
        if "Return:" in line:
            last_line_return = True

    final_grade.append(grade)

output = ""
if grading:
    json_output = {}
    for name, points in func_points.items():
        json_output[name] = {
            "score": points,
            "bad_ops": seen_bad_ops[name]
        }

    print
    json.dumps(json_output)

else:
    output += str([k + ': ' + str(v) for k, v in seen_bad_ops.iteritems() if len(seen_bad_ops[k])])
    print
    output.rstrip("\t")

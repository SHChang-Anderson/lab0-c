import subprocess
import re
import matplotlib.pyplot as plt

cmd_template = """option fail 0
option malloc 0
new
ih RAND {}
time
sort
time
free
"""
x_values = []
y1_values = []
y2_values = []
for i in range(1,100000,1000):
    cmd_content = cmd_template.format(i)  
    file_name = f"./traces/sort_test.cmd"  
    with open(file_name, "w") as cmd_file:
        cmd_file.write(cmd_content)

    command = "perf stat -e cycles ./qtest -f ./traces/sort_test.cmd"


    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)


    if result.stderr:
        cycles_match = re.search(r'\b(\d{1,3}(?:,\d{3})*\b)', result.stderr)
        
        if cycles_match:
            cycles_number = cycles_match.group(1)
            cycles_number = int(cycles_number.replace(",", ""))
            x_values.append(i)
            y1_values.append(cycles_number)
        else:
            continue

cmd_template = """option fail 0
option malloc 0
new
ih RAND {}
time
lsort
time
free
"""

for i in range(1,100000,1000):
    cmd_content = cmd_template.format(i)  
    file_name = f"./traces/sort_test.cmd"  
    with open(file_name, "w") as cmd_file:
        cmd_file.write(cmd_content)

    command = "perf stat -e cycles ./qtest -f ./traces/sort_test.cmd"


    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)


    if result.stderr:
        cycles_match = re.search(r'\b(\d{1,3}(?:,\d{3})*\b)', result.stderr)
        
        if cycles_match:
            cycles_number = cycles_match.group(1)
            cycles_number = int(cycles_number.replace(",", ""))
            y2_values.append(cycles_number)
        else:
            continue

plt.plot(x_values, y1_values, label='my_sort', marker='x')
plt.plot(x_values, y2_values, label='list_sort', marker='x')
plt.title('Sort Performance')
plt.xlabel('Size of the Linked List')
plt.ylabel('Number of Cycles')
plt.legend()
plt.show()
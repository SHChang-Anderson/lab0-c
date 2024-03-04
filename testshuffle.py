import subprocess
import re
import matplotlib.pyplot as plt



command = "./qtest -f ./traces/shuffle_test.cmd"
reslist = []
for i in range(400000):
    print(i)    
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    lines = result.stdout.split('\n')

    # Check if there are at least six lines
    if len(lines) >= 5:
        line = lines[4].strip()  # Get the sixth line and remove leading/trailing spaces
        match = re.search(r'\[([a-z\s]+)\]', line)
        if match:
            desired_list = match.group(1).replace(' ', '')  # Extract content inside square brackets and remove spaces
            reslist.append(desired_list)
        else:
            print("No matching list found in the sixth line.")
    else:
        print("Output does not have at least six lines.")

# Count occurrences of each unique string
count_dict = {}
for item in reslist:
    if item in count_dict:
        count_dict[item] += 1
    else:
        count_dict[item] = 1

# Print the results
keys = list(count_dict.keys())
values = list(count_dict.values())

# Create a bar chart
plt.bar(keys, values)

# Add labels and title
plt.xlabel('Unique Strings')
plt.ylabel('Occurrences')
plt.title('Occurrences of Unique Strings')

# Rotate x-axis labels for better readability (optional)
plt.xticks(rotation=45)

# Show the plot
plt.show()

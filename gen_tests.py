import random

with open('gen_data.txt', 'w') as f:

    for count in range(2000000):
        num = random.randint(1, 1000)
        f.write(str(num))
        f.write("\n")

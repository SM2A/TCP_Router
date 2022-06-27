import random

size = 1024 * 1024 * 10
name = 'Data.txt'

file = open(name, 'w')

for i in range(0, size):
    file.write(str(random.randint(0, 9)))

file.close()

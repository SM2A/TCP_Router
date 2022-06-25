import random

size = 1024
name = 'Data.txt'

file = open(name, 'w')

for i in range(0, 1024):
    file.write(str(random.randint(0, 9)))

file.close()

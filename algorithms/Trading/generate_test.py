from random import *

def randomCord():
    return (randint(-500000000,500000000)/1000)

print(str(1432))
for i in range(0, 1432):
   print(str(randomCord()) + ' ' + str(randomCord()) + ' ' + str(randomCord()))
   
print(str(0))

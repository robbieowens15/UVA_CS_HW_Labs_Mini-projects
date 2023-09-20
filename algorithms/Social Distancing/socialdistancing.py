"""
While I wrote all this code myself
I did talk through & diagram how to 
systematically solve this problem with 
two classmates: Christopher Obsbone
and Mac McLean 
"""

# Global vars
park = []
num_people = 2
num_table = 0

def read_input():
    global num_people
    global num_table
    global park

    temp = input()
    num_table, num_people = temp.split()
    num_table = int(num_table)
    num_people = int(num_people)

    for i in range(num_table):
        park.append(int(input()))

def walk(step):
    global park
    global num_table

    counter = 1
    last_table = park[0]
    for table_number in park:
        if table_number - last_table >= step:
            counter += 1
            last_table = table_number

    return counter >= num_people
        

def find_smallest_spacing():
    global park
    global num_people

    lower_bound = 1 #inclusive
    upper_bound = (park[-1])//(num_people-1) #exclusive
    
    while upper_bound - lower_bound > 1:
        avg = (lower_bound + upper_bound) // 2
        if walk(avg):
            lower_bound = avg
        else:
            upper_bound = avg

    return lower_bound

def main():
    read_input()
    print(str(find_smallest_spacing()))

if __name__ == "__main__": 
    main()

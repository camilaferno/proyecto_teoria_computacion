import random
import itertools
import sys

r = random.SystemRandom()

def generator():
    #n = r.choice(list(range(4, 100)))
    n = 5
    n_final_states = r.choice(list(range(1, int(n/5+1))))

    states = list(range(0, n))

    final_states = r.sample(states, n_final_states)
    init_state = r.choice([s for s in states if s not in final_states])

    filename = "generated/aut_" + str(i+1) + ".txt"
    file = open(filename, "w+")
    line = str(n) + " " + str(init_state) + " " + str(n_final_states) + " "
    for fs in final_states:
        line += str(fs)
        if fs != final_states[len(final_states) - 1]:
            line += " "
    file.write(line + "\n")
    for s in states:
        tr = str(s) + " 0 " + str(r.choice(states))
        file.write(tr + "\n")
        tr = str(s) + " 1 " + str(r.choice(states))
        file.write(tr + "\n")
    file.close()


for i in range(0, int(sys.argv[1])):
    generator()
    
import random
import numpy as np
import sys
usepytwo=False
if sys.version_info[0] < 3:
    usepytwo=True


#these are the probability that a car is behind door 1, 2 or 3
#for the symmetric case discussed in Question 2(b)
#door_probs=[0.3333,0.3333,0.3334]
#for the skewed case in Question 2(c)
door_probs=[0.8,0.1,0.1]

#choose an initial door for the car
actual_cd=1+list(np.random.multinomial(1,door_probs)).index(1)

## the simulator function.
## inputs (tuple s and integer a)
##      * state s is a tuple giving the previous door picked by the player (pd) and the hosts choice (hc). Each can take on values 0-3, where 0 denotes no choice has been made (start of a game) and 1-3 denote which door has been chosen.
##      * action a is the action by the player - this must be 1,2, or 3 and denotes which door is chosen
## output (tuple with first element being a tuple sp and second being a reward value)
##      * state sp is a tuple (pdp,hcp) giving the updated state after the action a is taken in state (pd,hc)
##      * reward gathered r by taking a in state (pd,hc) and ending up in (pdp,hcp)
def simulator(s,a):
    # use the global car door variable (this is hidden to the player who sees the simulator as a black box)
    global actual_cd
    pd=s[0]
    hc=s[1]

    #new value of picked door based on action
    if pd==0:
        pdp=a
    else:      #resets every second round
        pdp=0


    #choose a door for the host - based on the actual car door in this game
    if hc==0:
        # the possible choices are a door in 1,2 or 3
        rem_cardoors=[1,2,3]
        # with the actual car door removed,
        rem_cardoors.remove(actual_cd)
        # and the choice of the player removed.
        if a in rem_cardoors:
            rem_cardoors.remove(a)
        # a random choice in the remaining set (which may be a single door)
        hcp=rem_cardoors[random.randint(0,len(rem_cardoors)-1)]
    else:     #resets every second round
        hcp=0

    #reward gathered after the final choice (when hc is non-zero)
    if hc==0 or not a==actual_cd:
        r=0
    else:
        r=1

    #reset actual car door in the second round in preparation for the next game
    if not hc==0:
        actual_cd=1+list(np.random.multinomial(1,door_probs)).index(1)
    #return new state and reward
    return ((pdp,hcp),r)

def get_best_action(s,Q):
    Q_s = [Q[s,1], Q[s,2], Q[s,3]]
    if Q[s,1] == max(Q_s):
        return 1
    elif Q[s,2] == max(Q_s):
        return 2
    elif Q[s,3] == max(Q_s):
        return 3
    else :
        return np.random.choice([1, 2, 3], 1)[0]

def print_q(Q):
    for r in Q:
        r = str(r)
    print("pd      hc      1                2                3")
    print("0       0       " + str(Q[(0,0),1]) + "   " + str(Q[(0,0),2]) + "   " + str(Q[(0,0),3]))
    print("1       2       " + str(Q[(1,2),1]) + "   " + str(Q[(1,2),2]) + "   " + str(Q[(1,2),3]))
    print("1       3       " + str(Q[(1,3),1]) + "   " + str(Q[(1,3),2]) + "   " + str(Q[(1,3),3]))
    print("2       1       " + str(Q[(2,1),1]) + "   " + str(Q[(2,1),2]) + "   " + str(Q[(2,1),3]))
    print("2       3       " + str(Q[(2,3),1]) + "   " + str(Q[(2,3),2]) + "   " + str(Q[(2,3),3]))
    print("3       1       " + str(Q[(3,1),1]) + "   " + str(Q[(3,1),2]) + "   " + str(Q[(3,1),3]))
    print("3       2       " + str(Q[(3,2),1]) + "   " + str(Q[(3,2),2]) + "   " + str(Q[(3,2),3]))


## a simple test program to show how this works
if __name__ == "__main__":

    #default initial state- must be (0,0)
    s=(0,0)
    learning_rate = 0.1;
    discount_factor = 0.75;
    game = 1000

    Q = {}
    for pd in range(4):
        for hc in range(4):
            for a in range(1, 4):
                if pd == 0 and hc == 0:
                    Q[(pd,hc), a] = 0.0
                elif pd != hc and pd > 0 and hc > 0:
                    Q[(pd,hc), a] = 0.0

    avg_err = 1
    total_batches = game*2
    while avg_err > 0.1:
        err = 0
        batch = 0
        reward = 0
        while batch < total_batches:
            a = get_best_action(s,Q);
            probs = [0.9, 0.0333, 0.0333, 0.0334]
            a = np.random.choice([a,1,2,3], p = probs);

            (sp,r) = simulator(s,a)
            Qr = Q[sp, get_best_action(sp, Q)]
            Q[s,a] = Q[s,a] + learning_rate * (r + discount_factor * Qr - Q[s,a])

            err = err + abs(r + discount_factor * Qr - Q[s,a])

            s = sp
            batch = batch+1
            reward = reward+r
        avg_err = err/total_batches
        print("Reward: " + str(reward))

    print_q(Q)

// ### attempt to acquire forks ###
void take_forks(int i) { 	// philosopher no. i (from 0 -> N-1)
	down(&mutex)         	// enter Critical Region
	state[i] = HUNGRY    	// record that philosopher is hungry
	test(i); 			 	// -- try to acquire 2 forks --
	up(&mutex);          	// exit Critical Region
	down(&s[i]);         	// block if forks were not acquired
}

// ### attempt to put down forks ###
void put_forks(i) { 		// philosopher no. i (from 0 -> N-1)
	down(&mutex);           // enter Critical Region
	state[i] = THINKING;    // philosopher has finished eating
	test(LEFT); 		    // see if left enough can now eat
	test(RIGHT); 			// see if right neighbour can now eat
	up(&mutex); 		    // exit critical region
}

// ### test acquisition of forks ###
void test_acquire(i) {
	// attempting to take forks + LHS no fork + RHS no fork = YOU CAN EAT!!!
	if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
		state[i] = EATING;
		up(&s[i]);
	}
}

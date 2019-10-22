// ### Lock operations ###
struct lock *lock_create(const char *name);
void         lock_destroy(struct lock *);

void  	     lock_acquire(struct lock *);
void         lock_release(struct lock *);

// ### Semaphore operations ###
struct semaphore *sem_create(const char *name, int initial_count);
void              sem_destroy(struct semaphore *);

void              P(struct semaphore *); // decrement
void              V(struct semaphore *); // increment

// ### Conditional Variable operations ###
struct cv   *cv_create(const char *name);
void	     cv_destroy(struct cv *);

void         cv_wait(struct cv *cv, struct lock *lock);      // releases Lock and Blocks
													       // if condition is met, re-acquires Lock

void	     cv_signal(struct cv *cv, struct lock *lock);    // wake one thread
void	     cv_broadcast(struct cv *cv, struct lock *lock); // wake all threads



#include<types.h>
#include<lib.h>
#include<synch.h>

/* number of seconds of delay used to simulate eating */
#define CATEATINGTIME 1
#define MOUSEEATINGTIME 3
/* number of seconds of delay used to simulate sleeping */
#define CATSLEEPINGTIME 5
#define MOUSESLEEPINGTIME 3

static volatile char *bowls;

static int num_bowls;

static int volatile eating_cats_count;

static int volatile eating_mice_count;

static struct semaphore *mutex;


int
initialize_bowls(unsigned int bowlcount)
{
  unsigned int i;

  if (bowlcount == 0) {
    kprintf("initialize_bowls: invalid bowl count %d\n",bowlcount);
    return 1;
  }

  bowls = kmalloc(bowlcount*sizeof(char));
  if (bowls == NULL) {
    panic("initialize_bowls: unable to allocate space for %d bowls\n",bowlcount);
  }
  /* initialize bowls */
  for(i=0;i<bowlcount;i++) {
    bowls[i] = '-';
  }
  eating_cats_count = eating_mice_count = 0;
  num_bowls = bowlcount;

  /* intialize mutex semaphore */
  mutex = sem_create("bowl mutex",1);
  if (mutex == NULL) {
    panic("initialize_bowls: sem_create failed\n");
  }
  
  return 0;
}


void
cleanup_bowls()
{
  if (mutex != NULL) {
    sem_destroy( mutex );
    mutex = NULL;
  }
  if (bowls != NULL) {
    kfree( (void *) bowls );
    bowls = NULL;
  }
}



void
cat_eat(unsigned int bowlnumber) {

  /* check the argument */
  if ((bowlnumber == 0) || ((int)bowlnumber > num_bowls)) {
    panic("cat_eat: invalid bowl number %d\n",bowlnumber);
  }

  /* check and update the simulation state to indicate that
   * the cat is now eating at the specified bowl */
  P(mutex);   // start critical section

  /* first check whether allowing this cat to eat will
   * violate any simulation requirements */
  if (bowls[bowlnumber-1] == 'c') {
    /* there is already a cat eating at the specified bowl */
    panic("cat_eat: attempt to make two cats eat from bowl %d!\n",bowlnumber);
  }
  if (eating_mice_count > 0) {
    /* there is already a mouse eating at some bowl */
    panic("cat_eat: attempt to make a cat eat while mice are eating!\n");
  }
  assert(bowls[bowlnumber-1]=='-');
  assert(eating_mice_count == 0);

  /* now update the state to indicate that the cat is eating */
  eating_cats_count += 1;
  bowls[bowlnumber-1] = 'c';

  V(mutex);  // end critical section

  /* simulate eating by introducing a delay
   * note that eating is not part of the critical section */
  clocksleep(CATEATINGTIME);

  /* update the simulation state to indicate that
   * the cat is finished eating */
  P(mutex);  // start critical section

  assert(eating_cats_count > 0);
  assert(bowls[bowlnumber-1]=='c');
  eating_cats_count -= 1;
  bowls[bowlnumber-1]='-';

  V(mutex);  // end critical section

  return;
}


void
cat_sleep() {
  /* simulate sleeping by introducing a delay */
  clocksleep(CATSLEEPINGTIME);
  return;
}





void
mouse_eat(unsigned int bowlnumber) {

  /* check the argument */
  if ((bowlnumber == 0) || ((int)bowlnumber > num_bowls)) {
    panic("mouse_eat: invalid bowl number %d\n",bowlnumber);
  }

  /* check and updated the simulation state to indicate that
   * the mouse is now eating at the specified bowl. */
  P(mutex);  // start critical section

  /* first check whether allowing this mouse to eat will
   * violate any simulation requirements */
  if (bowls[bowlnumber-1] == 'm') {
    /* there is already a mouse eating at the specified bowl */
    panic("mouse_eat: attempt to make two mice eat from bowl %d!\n",bowlnumber);
  }
  if (eating_cats_count > 0) {
    /* there is already a cat eating at some bowl */
    panic("mouse_eat: attempt to make a mouse eat while cats are eating!\n");
  }
  assert(bowls[bowlnumber-1]=='-');
  assert(eating_cats_count == 0);

  /* now update the state to indicate that the mouse is eating */
  eating_mice_count += 1;
  bowls[bowlnumber-1] = 'm';


  V(mutex);  // end critical section

  /* simulate eating by introducing a delay
   * note that eating is not part of the critical section */
  clocksleep(MOUSEEATINGTIME);

  /* update the simulation state to indicate that
   * the mouse is finished eating */
  P(mutex); // start critical section

  assert(eating_mice_count > 0);
  eating_mice_count -= 1;
  assert(bowls[bowlnumber-1]=='m');
  bowls[bowlnumber-1]='-';

  V(mutex);  // end critical section
  return;
}


void
mouse_sleep() {
  /* simulate sleeping by introducing a delay */
  clocksleep(MOUSESLEEPINGTIME);
  return;
}


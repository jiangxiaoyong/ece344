/*
 * catsem.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use SEMAPHORES to solve the cat syncronization problem in 
 * this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>
#include <curthread.h>


/*
 * 
 * Constants
 *
 */

/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 * Number of cats.
 */

#define NCATS 6

/*
 * Number of mice.
 */

#define NMICE 2


/*
 *Number of loop
 */
#define NLOOPS 4


struct semaphore *CatMouseWait;


int initialize_bowls(unsigned int bowlcount);
void cleanup_bowls( void );

static volatile char *bowls;

static int num_bowls;

static int volatile eating_cats_count;

static int volatile eating_mice_count;

static struct semaphore *mutex;

static int cat_num_count;
static int mouse_num_count;

/*
 *
 *initialization
 *
 */
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
  cat_num_count = 0;
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



/*
 * 
 * Function Definitions
 * 
 */

/* who should be "cat" or "mouse" */
static void
sem_eat(const char *who, int num, int bowl, int iteration)
{
	P(mutex);
        kprintf("%s: %d starts eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
	V(mutex);

        clocksleep(1);

	P(mutex);
        kprintf("%s: %d ends eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
	V(mutex);

	kprintf("\n");
}

/*
 * catsem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
catsem(void * unusedpointer, 
       unsigned long catnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) unusedpointer;
        (void) catnumber;

	int i; 
	int cat_num = 0;
	unsigned int bowl;

	/*
	 *get to know which number of current cat
	 */
	P(mutex);
	cat_num = cat_num_count++;
	V(mutex);





	  for(i=0;i<NLOOPS;i++) {


		  bowl = ((unsigned int)random() % NFOODBOWLS ) + 1;
		  /* check the argument */
		  if ((bowl== 0) || ((int)bowl> NFOODBOWLS )) {
		    panic("cat_eat: invalid bowl number %d\n",bowl );
		  }


		  back:
		  while(bowls[bowl - 1] == 'c' || eating_mice_count > 0)
		  {
		  
		  //	kprintf("cat is waiting\n");
		  }

		  P(mutex);   // start critical section

		  if (bowls[bowl -1] == 'c') {
 		    kprintf("cat_eat(%d): attempt to make two cats eat from bowl %d!\n",cat_num,bowl);
		    V(mutex);
		    goto back; //while this cat see a vacancy, try to eat that bowl, however, occupy by anothre fast cat
		  }
		  if (eating_mice_count > 0) {
		    /* there is already a mouse eating at some bowl */
		  //  panic("cat_eat(%d): attempt to make a cat eat while mice are eating!\n",cat_num );
		    kprintf("cat_eat(%d): attempt to make a cat eat while mice are eating!\n",cat_num);
	            V(mutex);
		    goto back;
		  }
		  assert(bowls[bowl-1]=='-');
		  assert(eating_mice_count == 0);

		  /* now update the state to indicate that the cat is eating */
		  eating_cats_count += 1;
		  bowls[bowl-1] = 'c';
		  V(mutex);

		  /* simulate eating by introducing a delay
		   * note that eating is not part of the critical section */
		  sem_eat("cat", cat_num, (int)bowl, i);

		/* update the simulation state to indicate that
		   * the cat is finished eating */

		  P(mutex);
		  assert(eating_cats_count > 0);
		  assert(bowls[bowl-1]=='c');
		  eating_cats_count -= 1;
		  bowls[bowl-1]='-';

		  V(mutex);  // end critical section



	  }
	  P(mutex);
	  
	  kprintf("cat(%d)finishing eating all round\n", cat_num );

	  V(mutex);
	  /* indicate that this cat simulation is finished */
	  V(CatMouseWait); 

}
        

/*
 * mousesem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long mousenumber: holds the mouse identifier from 0 to 
 *              NMICE - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
mousesem(void * unusedpointer, 
         unsigned long mousenumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) unusedpointer;
        (void) mousenumber;

	int i; 
	int mouse_num = 0;
	unsigned int bowl;

	/*
	 *get to know which number of current cat
	 */
	P(mutex);
	mouse_num = mouse_num_count++;
	V(mutex);


	  for(i=0;i<NLOOPS;i++) {


		  bowl = ((unsigned int)random() % NFOODBOWLS ) + 1;
		  /* check the argument */
		  if ((bowl== 0) || ((int)bowl> NFOODBOWLS )) {
		    panic("mouse_eat: invalid bowl number %d\n",bowl );
		  }


		  back:
		  while(bowls[bowl - 1] == 'm' || eating_cats_count > 0)
		  {
		  
		  }

		  P(mutex);   // start critical section

		  if (bowls[bowl -1] == 'm') {
 		    kprintf("mouse_eat(%d): attempt to make two mice eat from bowl %d!\n",mouse_num ,bowl);
		    V(mutex);
		    goto back; //while this mouse see a vacancy, try to eat that bowl, however, occupy by anothre fast mouse
		  }
		  if (eating_cats_count > 0) {
		    /* there is already a mouse eating at some bowl */
		   // panic("mouse_eat(%d): attempt to make a mouse eat while cat are eating!\n",mouse_num );
		    kprintf("mouse_eat(%d): attempt to make a mouse eat while cat are eating!\n",mouse_num );
		    V(mutex);
		    goto back;
		  }
		  assert(bowls[bowl-1]=='-');
		  assert(eating_cats_count == 0);

		  /* now update the state to indicate that the cat is eating */
		  eating_mice_count += 1;
		  bowls[bowl-1] = 'm';
		  V(mutex);

		  /* simulate eating by introducing a delay
		   * note that eating is not part of the critical section */
		  sem_eat("mouse", mouse_num , (int)bowl, i);

		/* update the simulation state to indicate that
		   * the cat is finished eating */

		  P(mutex);
		  assert(eating_mice_count > 0);
		  assert(bowls[bowl-1]=='m');
		  eating_mice_count -= 1;
		  bowls[bowl-1]='-';

		  V(mutex);  // end critical section



	  }

	  /* indicate that this cat simulation is finished */

	  P(mutex);
	  
	  kprintf("mouse(%d)finishing eating all round\n", mouse_num );

	  V(mutex);
	  V(CatMouseWait); 

}


/*
 * catmousesem()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catsem() and mousesem() threads.  Change this 
 *      code as necessary for your solution.
 */

int
catmousesem(int nargs,
            char ** args)
{
        int index, error;
 	int i;  
        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;
   
	//wait all cat mouse finish
	CatMouseWait = sem_create("CatMouseWait",0);
        if (CatMouseWait == NULL)
       	{
   		 panic("catmouse: could not create semaphore\n");
        }

	  /* 
	   * initialize the bowls
	   */
	  if (initialize_bowls(NFOODBOWLS))
	  {
	    panic("catmouse: error initializing bowls.\n");
	  }


        /*
         * Start NCATS catsem() threads.
         */
	kprintf("Using %d bowls, %d cats, and %d mice. \n",
        	 NFOODBOWLS,NCATS ,NMICE );
        for (index = 0; index < NCATS; index++) {
           
                error = thread_fork("catsem Thread", 
                                    NULL, 
                                    index, 
                                    catsem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catsem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }
        
        /*
         * Start NMICE mousesem() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mousesem Thread", 
                                    NULL, 
                                    index, 
                                    mousesem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mousesem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }



	  /* wait for all of the cats and mice to finish before
	     terminating */  
	  for(i=0;i<( NCATS + NMICE );i++) {
	    P(CatMouseWait);
	  }


	  /* clean up the semaphore that we created */
	  sem_destroy(CatMouseWait);

	  /* clean up resources used for tracking bowl use */
	  cleanup_bowls();

       	return 0;
}


/*
 * End of catsem.c
 */

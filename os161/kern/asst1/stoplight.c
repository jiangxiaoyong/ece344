/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use any synchronization primitives available to solve
 * the stoplight problem in this file.
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


/*
 *
 * Constants
 *
 */

/*
 * Number of cars created.
 */

#define NCARS 20

struct semaphore * CarsWait; 

struct cv *condition_lock; 
struct lock *lock_NW;
struct lock *lock_NE;
struct lock *lock_SW;
struct lock *lock_SE;

struct lock * approaching_N;
struct lock * approaching_S;
struct lock * approaching_W;
struct lock * approaching_E;

struct lock * mutex_N;
struct lock * mutex_S;
struct lock * mutex_W;
struct lock * mutex_E;
/*
 *
 * Function Definitions
 *
 */

static const char *directions[] = { "N", "E", "S", "W" };
static const char *destination[] = { "N", "E", "S", "W" };
typedef struct intersection{
	int NW;
	int NE;
	int SW;
	int SE;

}intersection;

intersection region;

static const char *msgs[] = {
        "approaching:",
        "region1:    ",
        "region2:    ",
        "region3:    ",
        "leaving:    "
};

/* use these constants for the first parameter of message */
enum { APPROACHING, REGION1, REGION2, REGION3, LEAVING };

static void
message(int msg_nr, int carnumber, int cardirection, int destdirection)
{
        kprintf("%s car = %2d, direction = %s, destination = %s\n",
                msgs[msg_nr], carnumber,
                directions[cardirection], directions[destdirection]);
}
 
/*
 * gostraight()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement passing straight through the
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
gostraight(int cardirection,
           int carnumber,
	   int cardestination )
{
        /*
         * Avoid unused variable warnings.
         */
        
        (void) cardirection;
        (void) carnumber;
//	lock_acquire(lk);
//	kprintf("gostraight\n");

	if(strcmp(directions[cardirection], "N") == 0)
	{


//		assert(region.NW == 0);
	/*	
       	 *	before enter region 1
	 */
		lock_acquire(mutex_N );
		back_gostraight_N:

		while(region.SW == 1)
		{
		}


		/*
		 *similating enter region 1
		 */
		lock_acquire(lock_NW);

		if(region.NW == 1)
		{
			panic("gostraight N region 1 occupied by other thread\n");
		}

		/*
		 *the region occupied by anothre thread just before entering
		 */

		if(region.SW == 1)
		{
			lock_release(lock_NW);
			goto back_gostraight_N;
		}


		region.NW = 1;
		message(1, carnumber, cardirection, cardestination);
		lock_release(mutex_N );

		clocksleep(1);




		/*
		 *simulating enter regin 2
		 */
		lock_acquire(lock_SW);

		if(region.SW == 1)
		{
			panic("gostraight N region 2  occupied by other thread\n");
		}
		assert(region.SW == 0);
		region.SW = 1;
		message(2, carnumber, cardirection, cardestination);
		/*
		 *leave region 1
		 */
		region.NW = 0;
		lock_release(lock_NW);

		clocksleep(1);

		/*
		 *leave region 2
		 */
		region.SW = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_SW);
	
	
	}
	else if(strcmp(directions[cardirection], "S") == 0)
	{
//		assert(region.SE == 0);

	/*	
       	 *	before enter region 1
	 */
		lock_acquire(mutex_S );
		back_gostraight_S:
		while(region.NE == 1)
		{
		}


		/*
		 *similating enter region 1
		 */
		lock_acquire(lock_SE );

		if(region.SE == 1 )
		{
			panic("gostraight S region 1  occupied by other thread\n");
		}
		if(region.NE == 1)
		{
			lock_release(lock_SE);
			goto back_gostraight_S ;
		}

		region.SE = 1;
		message(1, carnumber, cardirection, cardestination);
		lock_release(mutex_S );
		clocksleep(1);




		/*
		 *simulating enter regin 2
		 */
		lock_acquire(lock_NE );

		if(region.NE == 1)
		{
			panic("gostraight S region 2 occupied by other thread\n");
		}
		assert(region.NE == 0);
		region.NE = 1;
		message(2, carnumber, cardirection, cardestination);
		/*
		 *leave region 1
		 */
		region.SE = 0;
		lock_release(lock_SE);

		clocksleep(1);

		/*
		 *leave region 2
		 */
		region.NE = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_NE);
	
	
	}
	else if(strcmp(directions[cardirection], "W") == 0)
	{


//		assert(region.SW == 0);

	/*	
       	 *	before enter region 1
	 */
		lock_acquire(mutex_W );
		back_gostraight_W:
		while(region.SE  == 1)
		{
		}


		/*
		 *similating enter region 1
		 */
		lock_acquire(lock_SW );

		if(region.SW == 1 )
		{
			panic("gostraight W region 1 occupied by other thread\n");
		}
		if(region.SE == 1)
		{
			lock_release(lock_SW);
			goto back_gostraight_W ;
		}


		region.SW = 1;
		message(1, carnumber, cardirection, cardestination);
		lock_release(mutex_W );
		clocksleep(1);




		/*
		 *simulating enter regin 2
		 */

		lock_acquire(lock_SE );
		if(region.SE == 1)
		{
			panic("gostraight W region 2 occupied by other thread\n");
		}
		assert(region.SE == 0);
		region.SE = 1;
		message(2, carnumber, cardirection, cardestination);
		/*
		 *leave region 1
		 */
		region.SW = 0;
		lock_release(lock_SW);

		clocksleep(1);

		/*
		 *leave region 2
		 */
		region.SE = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_SE);
	
	
	}
	else if(strcmp(directions[cardirection], "E") == 0)
	{
//		assert(region.NW == 0);
	/*	
       	 *	before enter region 1
	 */
		lock_acquire(mutex_E );
		back_gostraight_E:
		while(region.NW  == 1)
		{
		}


		/*
		 *similating enter region 1
		 */
		lock_acquire(lock_NE );

		if(region.NE == 1)
		{
			panic("gostraight E region 1 occupied by other thread\n");
		}

		if(region.NW == 1)
		{
			lock_release(lock_NE);
			goto back_gostraight_E ;
		}



		region.NE = 1;
		message(1, carnumber, cardirection, cardestination);
		lock_release(mutex_E );
		clocksleep(1);




		/*
		 *simulating enter regin 2
		 */
		lock_acquire(lock_NW );

		if(region.NW == 1)
		{
			panic("gostraight E region 2 occupied by other thread\n");
		}
		assert(region.NW == 0);
		region.NW = 1;
		message(2, carnumber, cardirection, cardestination);
		/*
		 *leave region 1
		 */
		region.NE = 0;
		lock_release(lock_NE);

		clocksleep(1);

		/*
		 *leave region 2
		 */

		region.NW = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_NW);
	
	
	}

	//lock_release(lk);
}


/*
 * turnleft()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnleft(int cardirection,
         int carnumber,
	 int cardestination )
{
        /*
         * Avoid unused variable warnings.
         */

        (void) cardirection;
        (void) carnumber;
	kprintf("left\n");
	
//	lock_acquire(lk);

	if(strcmp(directions[cardirection], "N") == 0)
	{


//		assert(region.NW == 0);
	/*	
       	 *	before enter region 1
	 */
		lock_acquire(mutex_N );
		back_left_N:
		while(region.SE  == 1)
		{
		}


		/*
		 *similating enter region 1
		 */
		lock_acquire(lock_NW);


		if(region.NW == 1 )
		{
			panic("left N region 1 occupied by other thread\n");
		}
		if(region.SE == 1)
		{
			lock_release(lock_NW);
			goto back_left_N ;
		}



		region.NW = 1;
		message(1, carnumber, cardirection, cardestination);
		lock_release(mutex_N );
		clocksleep(1);




		/*
		 *simulating enter regin 2
		 */
		lock_acquire(lock_SW);
		if(region.SW == 1)
		{
			panic("left N region 2  occupied by other thread\n");
		}
		assert(region.SW == 0);
		region.SW = 1;
		message(2, carnumber, cardirection, cardestination);
		/*
		 *leave region 1
		 */
		region.NW = 0;
		lock_release(lock_NW);

		clocksleep(1);




		/*
		 *simulation enter region 3
		 */
		lock_acquire(lock_SE);

		if(region.SE == 1)
		{
			panic("left N region 3  occupied by other thread\n");	
		}
		region.SE = 1;
		message(3, carnumber, cardirection, cardestination);
		/*
		 *leave region 2
		 */
		region.SW = 0;
		lock_release(lock_SW);
		clocksleep(1);

		/*
		 *leave region 3
		 */
		region.SE = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_SE);
	}
	else if(strcmp(directions[cardirection], "S") == 0)
	{
//		assert(region.SE == 0);
	/*	
       	 *	before enter region 1
	 */
		lock_acquire(mutex_S );
		back_left_S:
		while(region.NW  == 1)
		{
		}


		/*
		 *similating enter region 1
		 */


		lock_acquire(lock_SE);


		if( region.SE ==1)
		{
			panic("left S region 1 occupied by other thread\n");
		}
		if( region.NW == 1 )
		{
			lock_release(lock_SE);
			goto back_left_S ;
		}

		region.SE = 1;
		message(1, carnumber, cardirection, cardestination);
		lock_release(mutex_S );
		clocksleep(1);




		/*
		 *simulating enter regin 2
		 */

		lock_acquire(lock_NE);
		if(region.NE == 1)
		{
			panic("left S region 2  occupied by other thread\n");
		}
		assert(region.NE == 0);
		region.NE = 1;
		message(2, carnumber, cardirection, cardestination);
		/*
		 *leave region 1
		 */
		region.SE = 0;
		lock_release(lock_SE);
		clocksleep(1);




		/*
		 *simulation enter region 3
		 */

		lock_acquire(lock_NW);
		if(region.NW == 1)
		{
			panic("left S region 3  occupied by other thread\n");	
		}
		region.NW = 1;
		message(3, carnumber, cardirection, cardestination);
		/*
		 *leave region 2
		 */
		region.NE = 0;
		lock_release(lock_NE);
		clocksleep(1);

		/*
		 *leave region 3
		 */
		region.NW = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_NW);
	}
	else if(strcmp(directions[cardirection], "W") == 0)
	{
//		assert(region.SW == 0);
	/*	
       	 *	before enter region 1
	 */
		lock_acquire(mutex_W );
		back_left_W:
		while(region.NE  == 1)
		{
		}


		/*
		 *similating enter region 1
		 */
		lock_acquire(lock_SW);


		if(region.SW == 1 )
		{
			panic("left W region 1 occupied by other thread\n");
		}
		if( region.NE == 1 )
		{
			lock_release(lock_SW);
			goto back_left_W ;
		}



		region.SW = 1;
		message(1, carnumber, cardirection, cardestination);
		lock_release(mutex_W );
		clocksleep(1);




		/*
		 *simulating enter regin 2
		 */

		lock_acquire(lock_SE);
		if(region.SE == 1)
		{
			panic("left W region 2  occupied by other thread\n");
		}
		assert(region.SE == 0);
		region.SE = 1;
		message(2, carnumber, cardirection, cardestination);
		/*
		 *leave region 1
		 */
		region.SW = 0;
		lock_release(lock_SW);
		clocksleep(1);



		/*
		 *simulation enter region 3
		 */
		lock_acquire(lock_NE);

		if(region.NE == 1)
		{
			panic("left W region 3  occupied by other thread\n");	
		}
		region.NE = 1;
		message(3, carnumber, cardirection, cardestination);

		/*
		 *leave region 2
		 */
		region.SE = 0;
		lock_release(lock_SE);
		clocksleep(1);

		/*
		 *leave region 3
		 */
		region.NE = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_NE);
	}
	else if(strcmp(directions[cardirection], "E") == 0)
	{
//		assert(region.NE == 0);
	/*	
       	 *	before enter region 1
	 */
		lock_acquire(mutex_E );
		back_left_E:
		while(region.SW  == 1)
		{
		}


		/*
		 *similating enter region 1
		 */

		lock_acquire(lock_NE);


		if( region.NE == 1 )
		{
			panic("left E region 1 occupied by other thread\n");
		}
		if( region.SW == 1 )
		{
			lock_release(lock_NE);
			goto back_left_E ;
		}



		region.NE = 1;
		message(1, carnumber, cardirection, cardestination);
		lock_release(mutex_E );
		clocksleep(1);




		/*
		 *simulating enter regin 2
		 */
		lock_acquire(lock_NW);

		if(region.NW == 1)
		{
			panic("left E region 2  occupied by other thread\n");
		}
		assert(region.NW == 0);
		region.NW = 1;
		message(2, carnumber, cardirection, cardestination);
		/*
		 *leave region 1
		 */
		region.NE = 0;
		lock_release(lock_NE);
		clocksleep(1);




		/*
		 *simulation enter region 3
		 */

		lock_acquire(lock_SW);
		if(region.SW == 1)
		{
			panic("left E region 3  occupied by other thread\n");	
		}
		region.SW = 1;
		message(3, carnumber, cardirection, cardestination);
		/*
		 *leave region 2
		 */
		region.NW = 0;
		lock_release(lock_NW);
		clocksleep(1);

		/*
		 *leave region 3
		 */
		region.SW = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_SW);
	}


//	lock_release(lk);
}


/*
 * turnright()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnright(int cardirection,
          int carnumber,
	  int cardestination )
{
        /*
         * Avoid unused variable warnings.
         */

        (void) cardirection;
        (void) carnumber;

//	lock_acquire(lk);
//	kprintf("right\n");

	if(strcmp(directions[cardirection],"N") == 0)
	{
		/*
		 *check desired region
		 */


		/*
		 *simulated car go through the desired region
		 */
		lock_acquire(mutex_N );
		lock_release(mutex_N );

		lock_acquire(lock_NW);
		if(region.NW ==1 )
		{
			panic("right N region 1 occupied by anothre car\n");
		}
		message(1, carnumber, cardirection, cardestination);
		region.NW = 1;
		clocksleep(1);

		/*
		 *leave region
		 */
		region.NW = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_NW);

	}
	else if(strcmp(directions[cardirection],"S") == 0)
	{
		/*
		 *check desired region
		 */
		lock_acquire(mutex_S );
		lock_release(mutex_S );
		/*
		 *simulated car go through the desired region
		 */
		lock_acquire(lock_SE);
		if(region.SE ==1 )
		{
			panic("right S  region occupied by anothre car\n");
		}
		message(1, carnumber, cardirection, cardestination);
		region.SE = 1;
		clocksleep(1);
		/*
		 *leave region
		 */

		region.SE = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_SE);
	}
	else if(strcmp(directions[cardirection],"W") == 0)
	{
		/*
		 *check desired region
		 */
		lock_acquire(mutex_W );
		lock_release(mutex_W );
		/*
		 *simulated car go through the desired region
		 */
		lock_acquire(lock_SW);
		if(region.SW ==1 )
		{
			panic("right region W  occupied by anothre car\n");
		}
		message(1, carnumber, cardirection, cardestination);
		region.SW = 1;
		clocksleep(1);
		/*
		 *leave region
		 */

		region.SW = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_SW);

	}
	else if(strcmp(directions[cardirection],"E") == 0)
	{
		/*
		 *check desired region
		 */
		lock_acquire(mutex_E );
		lock_release(mutex_E );
		/*
		 *simulated car go through the desired region
		 */
		lock_acquire(lock_NE);
		if(region.NE ==1 )
		{
			panic("right region E occupied by anothre car\n");
		}
		message(1, carnumber, cardirection, cardestination);
		region.NE = 1;
		clocksleep(1);
		/*
		 *leave region
		 */
		region.NE = 0;
	message(4, carnumber, cardirection, cardestination);
		lock_release(lock_NE);
	}


//	lock_release(lk);



}

/*
 *
 *turn direction decision making
 *
 */

char * 
turnDirectionMaking(int cardirection,
		    int cardestination)
{

       /* decision of turn direction*/
	char *turndirection;
	if(strcmp(directions[cardirection], "N") == 0)
	{
		if(strcmp(destination[cardestination], "W") ==0 )
			turndirection = "right";
		if(strcmp(destination[cardestination], "S") == 0)
			turndirection = "straight";
		if(strcmp(destination[cardestination], "E") ==0)
			turndirection = "left";
	}

	if(strcmp(directions[cardirection], "S") == 0)
	{
		if(strcmp(destination[cardestination], "E") ==0 )
			turndirection = "right";
		if(strcmp(destination[cardestination], "N") == 0)
			turndirection = "straight";
		if(strcmp(destination[cardestination], "W") ==0)
			turndirection = "left";
	}

	if(strcmp(directions[cardirection], "W") == 0)
	{
		if(strcmp(destination[cardestination], "S") ==0 )
			turndirection = "right";
		if(strcmp(destination[cardestination], "E") == 0)
			turndirection = "straight";
		if(strcmp(destination[cardestination], "N") ==0)
			turndirection = "left";
	}

	if(strcmp(directions[cardirection], "E") == 0)
	{
		if(strcmp(destination[cardestination], "N") ==0 )
			turndirection = "right";
		if(strcmp(destination[cardestination], "W") == 0)
			turndirection = "straight";
		if(strcmp(destination[cardestination], "S") ==0)
			turndirection = "left";
	}

	return turndirection;

}



/*
 * approachintersection()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long carnumber: holds car id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by createcars().  Each one must choose a direction
 *      randomly, approach the intersection, choose a turn randomly, and then
 *      complete that turn.  The code to choose a direction randomly is
 *      provided, the rest is left to you to implement.  Making a turn
 *      or going straight should be done by calling one of the functions
 *      above.
 */
 
static
void
approachintersection(void * unusedpointer,
                     unsigned long carnumber)
{
        int cardirection;
	int cardestination;
	char *turndirection;

        /*
         * Avoid unused variable and function warnings.
         */

        (void) unusedpointer;
        (void) carnumber;
	(void) gostraight;
	(void) turnleft;
	(void) turnright;

	/*
	 *initialize intersecion value
	 */
//	region.NW = 0;
//	region.NE = 0;
//	region.SW = 0;
//	region.SE = 0;

        /*
         * cardirection is set randomly.
         */


        cardirection = random() % 4;

	/*
	 *set turn direction
	 */

	cardestination = random() % 4;
	while(cardestination == cardirection )
	{
		cardestination = random() % 4;
	}

	turndirection = turnDirectionMaking(cardirection, cardestination );	
//	kprintf("carnumber %d directions %s destination %s turndirection %s\n", (int)carnumber, directions[cardirection ], destination[cardestination ], turndirection );

	if(strcmp(turndirection,"right") == 0)
	{
		if(strcmp(directions[cardirection], "N" )== 0)
		{
			lock_acquire(approaching_N );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_N );
			turnright(cardirection,carnumber, cardestination );
	
		}
		else if(strcmp(directions[cardirection], "S" )== 0)
		{
			lock_acquire(approaching_S );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_S );
			turnright(cardirection,carnumber, cardestination );
	
		}
		else if(strcmp(directions[cardirection], "W" )== 0)
		{
			lock_acquire(approaching_W );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_W );
			turnright(cardirection,carnumber, cardestination );
	
		}
		else if(strcmp(directions[cardirection], "E" )== 0)
		{
			lock_acquire(approaching_E );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_E );
			turnright(cardirection,carnumber, cardestination );
	
		}
	}
	else if(strcmp(turndirection,"left") ==0 )
	{

		if(strcmp(directions[cardirection], "N" )== 0)
		{
			lock_acquire(approaching_N );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_N );
			turnleft(cardirection, carnumber, cardestination );
		}
		else if(strcmp(directions[cardirection], "S" )== 0)
		{
			lock_acquire(approaching_S );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_S );
			turnleft(cardirection, carnumber, cardestination );
		}
		else if(strcmp(directions[cardirection], "W" )== 0)
		{
			lock_acquire(approaching_W );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_W );
			turnleft(cardirection, carnumber, cardestination );
		}
		else if(strcmp(directions[cardirection], "E" )== 0)
		{
			lock_acquire(approaching_E );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_E );
			turnleft(cardirection, carnumber, cardestination );
		}

	}
	else if(strcmp(turndirection,"straight") == 0)
	{
		if(strcmp(directions[cardirection], "N" )== 0)
		{
			lock_acquire(approaching_N );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_N );
			gostraight(cardirection,carnumber,cardestination );
		}
		if(strcmp(directions[cardirection], "S" )== 0)
		{
			lock_acquire(approaching_S );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_S );
			gostraight(cardirection,carnumber,cardestination );
		}
		if(strcmp(directions[cardirection], "W" )== 0)
		{
			lock_acquire(approaching_W );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_W );
			gostraight(cardirection,carnumber,cardestination );
		}
		if(strcmp(directions[cardirection], "E" )== 0)
		{
			lock_acquire(approaching_E );
			message(0, (int)carnumber, cardirection, cardestination );
			lock_release(approaching_E );
			gostraight(cardirection,carnumber,cardestination );
		}

	}
	kprintf("\n");

	/*
	 *indicating one car finishing crossing intersection
	 */
	V(CarsWait);
}


/*
 * createcars()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modiy this code as necessary for your solution.
 */

int
createcars(int nargs,
           char ** args)
{
        int index, error,i;

        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;

        /*
         * Start NCARS approachintersection() threads.
         */

	//start create semaphore to wait all cars complete
	CarsWait = sem_create("CarsWait",0);
        if (CarsWait == NULL)
       	{
   		 panic("CarsWait : could not create semaphore\n");
        }
       /* initializa condition_lock*/
	condition_lock = cv_create("condition_lock");
        if (condition_lock == NULL)
       	{
   		 panic("condition_lock : could not create condition_lock \n");
        }
	//create mutex
	mutex_N = lock_create("mutex_N ");
	mutex_S = lock_create("mutex_S ");
	mutex_W = lock_create("mutex_W ");
	mutex_E = lock_create("mutex_E ");
        if (mutex_N == NULL) {
	    panic("approaching_N : lock_create failed\n");

	 }
        if (mutex_S == NULL) {
	    panic("approaching_N : lock_create failed\n");

	 }
        if (mutex_W == NULL) {
	    panic("approaching_N : lock_create failed\n");

	 }
        if (mutex_E == NULL) {
	    panic("approaching_N : lock_create failed\n");

	 }
        /* intialize approaching */
	approaching_N = lock_create("approaching_N ");
        if (approaching_N  == NULL) {
	    panic("approaching_N : lock_create failed\n");

	 }

	approaching_S = lock_create("approaching_S ");
        if (approaching_S  == NULL) {
	    panic("approaching_S : lock_create failed\n");

	 }

	approaching_W = lock_create("approaching_W ");
        if (approaching_W == NULL) {
	    panic("approaching_W : lock_create failed\n");

	 }

	approaching_E = lock_create("approaching_E ");
        if (approaching_E == NULL) {
	    panic("approaching_E : lock_create failed\n");

	 }
	/*
	 *initialize lock of four regions
	 */
	lock_NW = lock_create("lock_NW");
        if (lock_NW == NULL) {
	    panic("lock_NW : lock_create failed\n");

	 }
	 
	lock_NE = lock_create("lock_NE");
        if (lock_NE == NULL) {
	    panic("lock_NE : lock_create failed\n");

	 }
	lock_SW = lock_create("lock_SW");
        if (lock_SW == NULL) {
	    panic("lock_SW : lock_create failed\n");

	 }
	lock_SE = lock_create("lock_SE");
        if (lock_SE == NULL) {
	    panic("lock_SE : lock_create failed\n");

	 }

	kprintf("starting simulating cars\n");

        for (index = 0; index < NCARS; index++) {

                error = thread_fork("approachintersection thread",
                                    NULL,
                                    index,
                                    approachintersection,
                                    NULL
                                    );

                /*
                 * panic() on error.
                 */

                if (error) {
                        
                        panic("approachintersection: thread_fork failed: %s\n",
                              strerror(error)
                              );
                }
        }

	/*
	 *wait all cars complete
	 */
        for(i=0;i<NCARS ;i++) 
	{
	    P(CarsWait );
	}

	sem_destroy(CarsWait );

        return 0;
}

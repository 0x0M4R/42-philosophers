/*
arguments
number_of_philosophers: is the number of philosophers and also the number
of forkss.
◦ time_to_die: is in milliseconds, if a philosopher doesn’t start eating ’time_to_die’
milliseconds after starting their last meal or the beginning of the simulation,
it dies.
◦ time_to_eat: is in milliseconds and is the time it takes for a philosopher to
eat. During that time they will need to keep the two forkss.
◦ time_to_sleep: is in milliseconds and is the time the philosopher will spend
sleeping.
◦ number_of_times_each_philosopher_must_eat: argument is optional, if all
philosophers eat at least ’number_of_times_each_philosopher_must_eat’ the
simulation will stop. If not specified, the simulation will stop only at the death
of a philosopher.

-Each philosopher should be given a number from 1 to ’number_of_philosophers’.
• Philosopher number 1 is next to philosopher number ’number_of_philosophers’.
Any other philosopher with the number N is seated between philosopher N - 1 and
philosopher N + 1.
About the logs of your program:
• Any change of status of a philosopher must be written as follows (with X replaced
with the philosopher number and timestamp_in_ms the current timestamp in milliseconds):
◦ timestamp_in_ms X has taken a forks
◦ timestamp_in_ms X is eating
◦ timestamp_in_ms X is sleeping
◦ timestamp_in_ms X is thinking
◦ timestamp_in_ms X died
• The status printed should not be scrambled or intertwined with another philosopher’s status.
• You can’t have more than 10 ms between the death of a philosopher and when it
will print its death.
• Again, philosophers should avoid dying!
memset, printf, malloc, free, write,
usleep, gettimeofday, pthread_create,
pthread_detach, pthread_join, pthread_mutex_init,
pthread_mutex_destroy, pthread_mutex_lock,
pthread_mutex_unlock
The specific rules for the mandatory part are:
• Each philosopher should be a thread.
• One forks between each philosopher, therefore if they are multiple philosophers, there
will be a forks at the right and the left of each philosopher.
• To avoid philosophers duplicating forkss, you should protect the forkss state with a
mutex for each of them.
*/
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <limits.h>
#define FOOD 50

struct philo
{
    pthread_t *philosopher;
    pthread_t *death;
    pthread_mutex_t *fork_lock;
    pthread_mutex_t food_lock;
    pthread_mutex_t plate_lock;
    pthread_mutex_t time_lock;
    pthread_mutex_t dead_lock;
    struct timeval t1;
    int *num_meals;
    int dead;
    int id;
    int *last_meal;
    int num_phil;
    int num_allowed;
    int time_start;
    int time_die;
    int time_eat;
    int time_sleep;
    int meals;
};
struct philo philos;
int check_all_alive();
void *philosopher(void *num);
void print_status(char *msg, int id);
void grab_chopstick(struct philo *philos, int i, int id);
void down_chopsticks(struct philo *philos,int c1,int c2);
int food_on_table(int i);
void get_token(struct philo *philos);
void return_token(struct philo *philos);

int current_time()
{
    struct timeval curr;
    if(gettimeofday(&curr, NULL)<0)
        exit(1);
    return (curr.tv_sec*1000+curr.tv_usec/1000);
}
void better_usleep(int t)
{
    int start=current_time();
    while(current_time()-start<t)
        usleep(100);
}
void *check_dead(void *num)
{
    while((philos.num_meals[(int)num]< philos.meals))
    {
        if ((current_time()- philos.last_meal[(int)num]) >= philos.time_die)
		{
			//write_mutex(&data->write, FALSE);
            pthread_mutex_lock(&philos.dead_lock);
            print_status("died", (int)num);
            
            //pthread_mutex_lock(&philos.dead_lock);
            philos.dead=1;
            pthread_mutex_unlock(&philos.dead_lock);
			//write_mutex(&data->death, TRUE);
            exit(0);
            //return NULL;
		}
    }
}
void *philosopher(void *num)
{
    int id;
    int i, left_chopstick, right_chopstick, f;

    id = (int)num;
    right_chopstick = id;
    left_chopstick = id + 1;

    /* Wrap around the chopsticks. */
    if (left_chopstick == philos.num_phil)
        left_chopstick = 0;
    if (id % 2==1)
        {
            better_usleep(philos.time_eat/50);
        }
    while ((food_on_table(id)<=philos.meals))
    {
        //gettimeofday(&philos.t2, NULL);
        //printf("%f %d is thinking\n",(philos.t2.tv_usec-philos.time_start)/1000,id); //timestamp_in_ms X is thinking
        //get_token(&philos);
        /*
        if (id % 2==0)
        {
        grab_chopstick(&philos,right_chopstick,id);
        grab_chopstick(&philos,left_chopstick,id);
        }
        else
        {*/
            grab_chopstick(&philos,left_chopstick,id);
            grab_chopstick(&philos,right_chopstick,id);
        //}
        //pthread_mutex_lock(&philos.time_lock);
        philos.last_meal[id]=current_time();
        //pthread_mutex_unlock(&philos.time_lock);
        print_status("is eating",id);
        //printf("%d is eating.\n", id); //timestamp_in_ms X is eating
        better_usleep(philos.time_eat);//for time to eat
        down_chopsticks(&philos,left_chopstick, right_chopstick);
        //return_token(&philos);
        //pthread_mutex_lock(&philos.time_lock);
        //pthread_mutex_unlock(&philos.time_lock);
        print_status("is sleeping",id);
        better_usleep(philos.time_sleep);
        print_status("is thinking",id);
    }
    return(NULL);
    //printf("%d is sleeping.\n", id); //timestamp_in_ms X is sleeping
    //return (NULL);
}
void print_status(char *msg, int id)
{
    if(!philos.dead)
    {
    pthread_mutex_lock(&philos.time_lock);
    //int curr=current_time();
    printf("%d %d %s\n",(current_time()-philos.time_start),id,msg);
    pthread_mutex_unlock(&philos.time_lock);
    }
}
int food_on_table(int i)
{
    int myfood;

    pthread_mutex_lock(&philos.food_lock);
    (philos.num_meals)[i]++;
    pthread_mutex_unlock(&philos.food_lock);
    return philos.num_meals[i];
}

void grab_chopstick(struct philo *philos, int i, int id)
{
    pthread_mutex_lock(&philos->fork_lock[i]);
    print_status("has taken a fork",id); //timestamp_in_ms X has taken a forks
}

void down_chopsticks(struct philo *philos,int c1,
                     int c2)
{
    pthread_mutex_unlock(&philos->fork_lock[c1]);
    pthread_mutex_unlock(&philos->fork_lock[c2]);
}

void get_token(struct philo *philos)
{
    int successful = 0;

    while (!successful)
    {
        pthread_mutex_lock(&philos->plate_lock);
        if (philos->num_allowed > 0)
        {
            philos->num_allowed--;
            successful = 1;
        }
        else
        {
            successful = 0;
        }
        pthread_mutex_unlock(&philos->plate_lock);
    }
}

void return_token(struct philo *philos)
{
    pthread_mutex_lock(&philos->plate_lock);
    philos->num_allowed++;
    pthread_mutex_unlock(&philos->plate_lock);
}
int main(int ac, char **av)
{
    //write(1,"hi",2);
    if (ac==5)
    philos.meals=INT_MAX;
    else
    philos.meals=atoi(av[5]);

    philos.num_phil=atoi(av[1]);
    philos.num_allowed= philos.num_phil-1;
    philos.time_die=atoi(av[2]);
    philos.time_eat=atoi(av[3]);
    philos.time_sleep=atoi(av[4]);
    //philos.meals=atoi(av[5]);
    philos.time_start=current_time();
    philos.dead=0;
    //write(1,"hi",2);
    philos.philosopher= (pthread_t*)malloc(sizeof(pthread_t)*philos.num_phil); 
    philos.death= (pthread_t*)malloc(sizeof(pthread_t)*philos.num_phil); 
    philos.fork_lock=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*philos.num_phil);
    philos.num_meals=malloc(sizeof(int)*philos.num_phil);
    //philos.dead=malloc(sizeof(int)*philos.num_phil);
    philos.last_meal=malloc(sizeof(int)*philos.num_phil);;
    //philos.status_lock=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*philos.num_phil);
     //initialize mutex for each forks
     int i=0;
     while(i<philos.num_phil)
     {
         philos.num_meals[i]=0;
         //philos.dead[i]=0;
         philos.last_meal[i]=philos.time_start;
         i++;
     }
    i=0;
     int k;
    pthread_mutex_init(&philos.food_lock, NULL);
    pthread_mutex_init(&philos.plate_lock, NULL);
     pthread_mutex_init(&philos.dead_lock, NULL);
    while(i<philos.num_phil)
    {
        k = pthread_mutex_init(&philos.fork_lock[i], NULL);
        if (k == -1)
        {
            printf("\n Mutex initialization failed");
            exit(1);
        }
        i++;
    }
    //printf("\n Mutex initialization done");
    i=0;
    while(i<philos.num_phil)
    {
        struct philo *temp = malloc(sizeof(struct philo));
        *temp = philos;
        (*temp).id= i;
        k = pthread_create(&philos.philosopher[i], NULL, (void *)philosopher, (void *)i);
        if (k == -1)
        {
            printf("\n Thread creation error \n");
            exit(1);
        }
        k = pthread_create(&philos.death[i], NULL, (void *)check_dead, (void *)i);
        if (k == -1)
        {
            printf("\n Thread creation error \n");
            exit(1);
        }
        i++;
    }
    i=0;
    for (i = 0; i < philos.num_phil; i++)
    {
       pthread_join(philos.philosopher[i], NULL);
       pthread_join(philos.death[i], NULL);
    }
    i=0;
    while(i<philos.num_phil)
    {
    printf("%d ",philos.num_meals[i]-1);
    i++;
    }
    for (i = 0; i < philos.num_phil; i++)
    {
        k = pthread_mutex_destroy(&philos.fork_lock[i]);
        if (k != 0)
        {
            printf("\n Mutex Destroyed \n");
            exit(1);
        }
    }
    return 0;
}
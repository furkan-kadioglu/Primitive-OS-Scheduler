#include <iostream>
#include <vector>
#include <fstream>
#include <pthread.h>
#include <unistd.h>



using namespace std;

int random_generator(double low, double high);
void *client(void *param);
void *server(void *param);



//keeps available seats' id 
vector<int> available_seat;


//keeps permission in real time 
vector<bool> flags;

//keeps first id for client threads 
int id_client = -1;


//are mutex locks' initial value
pthread_mutex_t m_id = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_client = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_server = PTHREAD_MUTEX_INITIALIZER;


//output file 
ofstream outFile("output.txt");


int main(int argc, char const *argv[])
{

    /*
        PART I
        Checks input is valid or not
    */
    //checks input variable and its interval 
    if (argc == 2)
    { 
        if(atoi(argv[1]) < 50)
        {
            cout << "Number of seats should be between 50 and 10" << endl;
            return 1;
        }
             
        if(atoi(argv[1]) > 100)
        {
            cout << "Number of seats should be between 50 and 100" << endl;
            return 1;
        }
            
    }
    else 
    {
        cout << "Run the code with the following command: ./a.out NUMBER_OF_SEATS" << endl;
        return 1;
    }

    


    /*
        PART II
        Initiliazes neccessary data structure for thread 
    */  
    
    //keeps number of seats
    int num_of_seats = atoi(argv[1]);

    //parameter for client thread creation s
    vector<pthread_t> clients(num_of_seats);
    

    //fill available seats to available_seat 
    for(int i=0; i< num_of_seats;i ++)
        available_seat.push_back(i);


    srandom((unsigned)time(NULL));
    
    //provides that no server go further without clients' permissions 
    flags.assign(num_of_seats,1);

    //is written by main thread 
    outFile << "Number of total seats: "<< num_of_seats <<endl;



    /*
        PART III
        Creates client threads and waits for them 
    */


    //creates clients' threads
    for (int i = 0; i < num_of_seats; i++)
		pthread_create(&clients[i], 0, client, NULL);
    
    //waits for clients' threads to terminate
    for(int i=0;i < num_of_seats; i++)
        pthread_join(clients[i],NULL);




    /*
        PART IV
        Checks all seats are reserved or not 
    */
    //prints all seat and client matches
    if(available_seat.size() == 0)
        outFile << "All seats are reserved." <<endl;
    
    return 0;
}


//generates random number at the interval which is wanted 
int random_generator(double low, double high)
{   
    return (random() / ((double)RAND_MAX + 1)) * (high-low + 0.9999) + low;
}


//is client threads' codes
void *client(void *param)
{   
    //gets id 
    pthread_mutex_lock(&m_id);
    int id = ++id_client;
    pthread_mutex_unlock(&m_id);
    
    //keeps seat for selection 
    int _seat;
    

    //sleeps ZZZ
    int time = random_generator(50,200);
    usleep(time*1000);    
    
    //creates server thread 
    pthread_t _server_;
    pair<int,int> sid= make_pair(_seat,id);
    pthread_create(&_server_,0,server,&sid);
    
    
    
    //lock before selection for available seat 
    pthread_mutex_lock(&m_client); 

    //selects random seat from available seats
    _seat = random_generator(0,available_seat.size()-1);

    //sends selected seat to it's server 
    sid.first = available_seat[_seat];

    //removes selected seats
    available_seat.erase(available_seat.begin()+ _seat);

    //gives permission server 
    flags[id] = false;

    pthread_mutex_unlock(&m_client);
    //lock after selection for available seat 
    
    
    
    
    
    pthread_join(_server_,NULL);
    pthread_exit(0);
    
}


//is server threads' codes
void *server(void *param)
{   
    //keeps server thread as busy till it's client chooses
    while(flags[((pair<int,int> *) param)->second]);

    //locks file before writing operations  
    pthread_mutex_lock(&m_server);

    //makes reservation process done 
    outFile << "Client" << ((pair<int,int> *) param)->second + 1 << " reserves Seat" <<((pair<int,int> *) param)->first + 1 <<endl;

    pthread_mutex_unlock(&m_server);
    //locks file after writing operations 
    
    
    pthread_exit(0);
}

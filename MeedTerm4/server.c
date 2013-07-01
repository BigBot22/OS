//Server program

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <map>
#include <assert.h>


#define MAX_CLIENTS 10
#define MAX_GAMES 10

const size_t BUF_SIZE = 32;

int sfd;
struct addrinfo hints;
struct addrinfo *result;
struct addrinfo *rp;

ssize_t nread;
//char buf_in[BUF_SIZE];
int weAreServer;

void AddrInit(void);
int SetConnection(char *);
void write_to_client(int, const char *);
int Solve(char, char);
void HandleComandFromSoket(char *);

char * my_malloc(size_t size) {
    char * res = (char *) malloc(size);
    if (res == NULL) {
        _exit(EXIT_FAILURE);
    }
    return res;
}




struct Game {
    
    int state;// 0: wait for mes fom first player;  1: have mes
    int game_state;
    char mes;
    size_t len_in;
    char buf_in[BUF_SIZE];
    size_t len_out;
    char buf_out[BUF_SIZE];
    
    Game()
    {
        state = 0;// 0: wait for mes fom first player;  1: have mes
        game_state = 0; // 2 players  : 1
        mes = ' ';
        len_in = 0;
        len_out = 0;
    }
    
    
    
    void handleIncomingData()
    {
        assert(game_state == 1);
        assert(state == 0);
        int i;
        for(i = 0; (buf_in[i] != '\n') && (i < len_in); ++i)
        {
            mes = buf_in[i];
        }
        len_in = 0;
        state = 1;
    }
    
    int setMesToBuf(const char * mes)
    {
        int i;
        for(i = 0; mes[i] != '\n'; ++i);
        ++i;
        if (this->len_out + i < BUF_SIZE)
        {
            memcpy(this->buf_out + this->len_out, mes, i);
            this->len_out += i;
            printf("game[x].buf_out:%s, %zu\n", buf_out, len_out);
            return 0;
        }
        return -1;
    }
    
    
    ~Game()
    {
        //free(buf_in);
    }
};

std::map<int, Game> games;

//Game games[MAX_GAMES + 1];
int gamesCount;

#define MAX 10
int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pid = fork();

    if(pid)
    {
        waitpid(pid, NULL, 0);
    }
    else
    {
        pid_t sid = setsid();
        if (sid == (pid_t) -1)
        {
            perror("setsid");
            _exit(EXIT_FAILURE);
        }

            AddrInit();
            SetConnection(argv[1]);

        struct pollfd ufds[MAX_CLIENTS + 1];
        int ufdsCount = 1;

        ufds[0].fd = sfd;
        ufds[0].events = POLLIN;

        gamesCount = 0;
        int curGame = 0;

        int rv;
        for (;;) {

            rv = poll(ufds, ufdsCount + 1, -1);

            if (rv == -1)
            {
                printf("poll Error");
                perror("poll"); // error occurred in poll()
            }
            else
            {
                if (rv > 0)
                {

                    // check for events on soket ( adding new clients)
                    if (ufds[0].revents & POLLIN)
                    {

                        int new_cfd; // client fd
                        struct sockaddr_storage peer_addr;
                        socklen_t addr_size = sizeof(peer_addr);

                        if ( (new_cfd = accept(sfd, (struct sockaddr *) &peer_addr, &addr_size)) == -1)
                        {
                            if(close(sfd) == -1) { _exit(EXIT_FAILURE);}
                            perror("accept fail");
                            return 6;
                        }

                        if (new_cfd < MAX_CLIENTS )
                        {
                            ufds[new_cfd].fd = new_cfd;
                            ufds[new_cfd].events = POLLIN | POLLRDHUP; // check for just normal data
                            ufdsCount = new_cfd;
                            printf("creating new games[%d]\n", new_cfd);
                            games[new_cfd].game_state = 0;

                        }
                        else
                        {
                            printf("New client dismist\n");
                            if(close(new_cfd) == -1) { _exit(EXIT_FAILURE);}
                            continue;
                        }

                        //write_to_client(new_cfd, "cenected to server.\n");
                        printf("New client %d\n", new_cfd);
                        
                        int pair_player;
                        if(new_cfd % 2 == 0)
                        {
                            pair_player = new_cfd + 1;
                            games[new_cfd].setMesToBuf("Please wait for second player.\n"); 
                            ufds[new_cfd].events |= POLLOUT;
                           
                            
                            continue;
                        }
                        else
                        {
                            pair_player = new_cfd - 1;
                        }
                        
                        games[new_cfd].game_state = 1;
                        games[pair_player].game_state = 1;
                        games[new_cfd].setMesToBuf("Ready to play.\nYour tearn.\n");
                        games[pair_player].setMesToBuf("Ready to play.\nYour tearn.\n");
                        ufds[new_cfd].events |= POLLOUT;
                        ufds[pair_player].events |= POLLOUT;
                        continue;


                    }
                    
                    int i;
                    char mes = ' ';
                    for (i = 1; i <= ufdsCount; ++i)
                    {
                        
                        if (ufds[i].revents & POLLRDHUP)
                        {
                            printf("POLLRDHUP ivent\n");
 
                            int pair_player;
                            if(i % 2 == 0)
                            {
                                pair_player = i + 1;
                            }
                            else
                            {
                                pair_player = i - 1;
                            }

                            if(close(i) == -1) { _exit(EXIT_FAILURE);}
                            if(close(pair_player) == -1) { _exit(EXIT_FAILURE);}

                            //games[pair_player].setMesToBuf("You partner is off, game over.\n");
                           
                            ufds[i].fd = -1;
                            ufds[i].events = 0;
                            ufds[pair_player].fd = -1;
                            ufds[pair_player].events = 0;
                            
                          

                            games.erase(i);
                            games.erase(pair_player);
                            printf("Game %d end %d is over. One of the players is off.\n", i, pair_player);
                            continue;

                        }

                        if (ufds[i].revents & POLLOUT)
                        {
                            //printf("POLLOUT ivent\n");
                            if (games[i].len_out != 0)
                            {
                                int writen = write(ufds[i].fd, games[i].buf_out, games[i].len_out);
                                //int writen = write(ufds[i].fd, "meage\n", 6);
                                if (writen == -1)
                                {
                                    printf("Write error\n");
                                    //handle_error("write", ufds, inputs, i, &nfds);
                                }
                                else
                                {
                                    games[i].len_out -= writen;
                                    printf("write game[%d].buf_out:%s, %zu\n", i, games[i].buf_out, games[i].len_out);
                                    if (games[i].len_out == 0) {
                                        ufds[i].events = POLLIN | POLLRDHUP;
                                    }
                                }
                            }
                        }
                        
                        if (ufds[i].revents & POLLIN)
                        {
                            printf("POLLIN ivent\n");
                            nread = read(ufds[i].fd, games[i].buf_in + games[i].len_in, BUF_SIZE - games[i].len_in);


                            if (nread == -1)
                            {
                                printf("reading tearn error\n");
                                continue;               /* Ignore failed request */
                            }
                            
                            if (games[i].len_in + nread < BUF_SIZE)
                            {
                                games[i].len_in += nread;
                            }
                            else
                            {
                                printf("reading from client error, to long mes\n");
                                continue;
                            }
                                

                            printf("Received %zu bytes from soket %d :%s\n", nread, ufds[i].fd, games[i].buf_in);

                            
                            
                            if(games[i].game_state == 1)
                            {
                                if (games[i].state == 0)
                                {
                                    printf("Player%d did tearn: %c\n", i, games[i].mes);
                                    games[i].handleIncomingData(); 
                                    games[i].setMesToBuf("Your turn ecepted.\n");
                                    ufds[i].events |= POLLOUT;
                                    
                                }
                                else
                                {
                                    printf("Player%d did extra tearn: %c\n", i, games[i].mes);
                                    games[i].len_in = 0;
                                    games[i].setMesToBuf("Wait till your tern.\n");
                                    ufds[i].events |= POLLOUT;
                                    continue;
                                }
                            }
                            else
                            {
                                printf("Player %d has no partner.\n", i);
                                games[i].setMesToBuf("Wait before second plyer conected.\n");
                                ufds[i].events |= POLLOUT;
                                games[i].len_in = 0;
                                continue;
                            }
                            
                            int pair_player;
                            if(i % 2 == 0)
                            {
                                pair_player = i + 1;
                            }
                            else
                            {
                                pair_player = i - 1;
                            }

                            //assert( games[i].state && games[pair_player].state);
                            
                            if(games[i].state == 1 && games[pair_player].state == 1)
                            {
                                printf("Got bouth tearns.\n", i);
                                int res = Solve(games[i].mes, games[pair_player].mes);
                                if(res == 0)
                                {
                                    games[i].setMesToBuf("Nobody win!\n");
                                    games[pair_player].setMesToBuf("Nobody win!\n");
                                    ufds[i].events |= POLLOUT;
                                    ufds[pair_player].events |= POLLOUT;
                                }
                                if(res == 1)
                                {
                                    games[i].setMesToBuf("You win!\n");
                                    games[pair_player].setMesToBuf("You lose!\n");
                                    ufds[i].events |= POLLOUT;
                                    ufds[pair_player].events |= POLLOUT;
                                }
                                if(res == 2)
                                {
                                    games[i].setMesToBuf("You lose!\n");
                                    games[pair_player].setMesToBuf("You win!\n");
                                    ufds[i].events |= POLLOUT;
                                    ufds[pair_player].events |= POLLOUT;
                                }
                                
                                if(res == -1)
                                {
                                    games[i].setMesToBuf("Wrong licsikon!\n");
                                    games[pair_player].setMesToBuf("Wrong licsikon!\n");
                                    ufds[i].events |= POLLOUT;
                                    ufds[pair_player].events |= POLLOUT;
                                }

                                games[i].state = 0;
                                games[pair_player].state = 0;
                                
                                games[i].setMesToBuf("Next game\nYour tearn!\n");
                                games[pair_player].setMesToBuf("Next game\nYour tearn!\n");
                                ufds[i].events |= POLLOUT;
                                ufds[pair_player].events |= POLLOUT;
                                

                                printf("Game res:%d\n", res);
                            }
                            else
                            {
                                printf("Not bouth stre = 1.\n");
                            }
                        }
                    }
                }
            }
        }
        
        if(close(sfd) == -1) { _exit(EXIT_FAILURE);}
    }  
    _exit(EXIT_SUCCESS);
}

int Solve(char m1, char m2)
{
    if(m1 == 'r')
    {
        if(m2 == 'r')
        {
            return 0;
        }
        if(m2 == 's')
        {
            return 1;
        }
        if(m2 == 'p')
        {
            return 2;
        }
    }

    if(m1 == 's')
    {
        if(m2 == 'r')
        {
            return 2;
        }
        if(m2 == 's')
        {
            return 0;
        }
        if(m2 == 'p')
        {
            return 1;
        }
    }

    if(m1 == 'p')
    {
        if(m2 == 'r')
        {
            return 1;
        }
        if(m2 == 's')
        {
            return 2;
        }
        if(m2 == 'p')
        {
            return 0;
        }
    }
    
    return -1;
}



void AddrInit(void)
{
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;

    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
}


int SetConnection(char *argv1)
{


    int s = getaddrinfo(NULL, argv1, &hints, &result);
    if (s) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        _exit(EXIT_FAILURE);
    }

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        perror("socket");
        _exit(EXIT_FAILURE);
    }
    int yes = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
        perror("setsockopt");
        _exit(EXIT_FAILURE);
    }
    if (bind(sfd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("bind");
        _exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    if (listen(sfd, 32)) {
        perror("listen");
        _exit(EXIT_FAILURE);
    }
    
    
    return 0;
}

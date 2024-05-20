#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

void displayTable(char *board[][10], int p1Pos, int p2Pos, char p1, char p2) {
    // display the game board with borders
    printf("+---------------------------------------------------------------------+\n");
    for (int i = 9; i >= 0; i--) {
        printf("| ");

        if (i % 2 == 1) {
            for (int j = 9; j >= 0; j--) {
                if (i * 10 + j + 1 == p1Pos && i * 10 + j + 1 == p2Pos) {
                    printf("  *  | "); // both players are on the same cell
                } else if (i * 10 + j + 1 == p1Pos) {
                    printf(" %-3c| ", p1); // player 1's position
                } else if (i * 10 + j + 1 == p2Pos) {
                    printf(" %-3c| ", p2); // player 2's position
                } else {
                    printf(" %-4s| ", board[i][j]); // empty cell
                }
            }
        } else if (i % 2 == 0) {
            for (int j = 0; j < 10; j++) {
                if (i * 10 + j + 1 == p1Pos && i * 10 + j + 1 == p2Pos) {
                    printf("  *  | "); // both players are on the same cell
                } else if (i * 10 + j + 1 == p1Pos) {
                    printf(" %-3c| ", p1); // player 1's position
                } else if (i * 10 + j + 1 == p2Pos) {
                    printf(" %-3c| ", p2); // player 2's position
                } else {
                    printf(" %-4s| ", board[i][j]); // empty cell
                }
            }
        }
        printf("\n+---------------------------------------------------------------------+\n");
    }
}

bool powerUp(int roll, bool powerBoard[10][10], int *p1Pos, int *p2Pos) {
    int p2Row = (*p2Pos - 1) / 10;
    int p2Col = (*p2Pos - 1) % 10;

    if (powerBoard[p2Row][p2Col]) {
        char choice;
        printf("Power-up tile encountered! Do you want to utilize the power-up? (y/n): ");
        scanf(" %c", &choice);
        getchar(); 

        if (choice == 'y' || choice == 'Y') {
            int index = rand() % 6 + 1;
            switch (index) {
                case 1:
                    (*p2Pos)++;
                    printf("Added 1 to your roll!\n");
                    break;
                case 2:
                    (*p2Pos) += 5;
                    printf("Added 5 to your roll!\n");
                    break;
                case 3:
                    (*p2Pos) += roll;
                    printf("Multiplied your roll by 2!\n");
                    break;
                case 4:
                    *p2Pos = *p1Pos;
                    printf("Teleported you to your opponent!\n");
                    break;
                case 5:
                    (*p1Pos) += 3;
                    printf("Bad luck! Your opponent moved 3 bonus tiles!\n");
                    break;
                case 6:
                    (*p1Pos) += 5;
                    printf("Bad luck! Your opponent moved 5 bonus tiles!\n");
                    break;
            }

            powerBoard[p2Row][p2Col] = false; 
            return true; 
        } else {
            printf("The power-up tile was not utilized!\n");
            return false;
        }
    }
    return false;
}


void die_with_error(const char *error_msg) {
    perror(error_msg);
    exit(1);
}

int main(int argc, char *argv[]){
    int client_sock,  port_no,  n;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[101];

    if (argc < 3) {
        printf("Usage: %s port_no\n", argv[0]);
        exit(1);
    }

    printf("Client starting ...\n");
    
    client_sock = socket(AF_INET,  SOCK_STREAM,  0);
    if (client_sock < 0) 
        die_with_error("Error: socket() Failed.");

    printf("Looking for host '%s'...\n", argv[1]);
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        die_with_error("Error: No such host.");
    }
    printf("Host found!\n");

    port_no = atoi(argv[2]);
    bzero((char *) &server_addr,  sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,  
         (char *)&server_addr.sin_addr.s_addr, 
         server->h_length);
         
    server_addr.sin_port = htons(port_no);

    printf("Connecting to server at port %d...\n", port_no);
    if (connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        die_with_error("Error: connect() Failed.\n");
    }

    printf("Connection successful!\n");

    system("clear"); // windows implementation

    char *gameBoard[10][10] = {
        {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"},
        {"11", "12", "13", "14", "15", "16", "17", "18", "19", "20"},
        {"21", "22", "23", "24", "25", "26", "27", "28", "29", "30"},
        {"31", "32", "33", "34", "35", "36", "37", "38", "39", "40"},
        {"41", "42", "43", "44", "45", "46", "47", "48", "49", "50"},
        {"51", "52", "53", "54", "55", "56", "57", "58", "59", "60"},
        {"61", "62", "63", "64", "65", "66", "67", "68", "69", "70"},
        {"71", "72", "73", "74", "75", "76", "77", "78", "79", "80"},
        {"81", "82", "83", "84", "85", "86", "87", "88", "89", "90"},
        {"91", "92", "93", "94", "95", "96", "97", "98", "99", "100"}
    };
    
    char p1, p2;
    int p1Pos = 1, p2Pos = 1;
    bool powerBoard[10][10];

    for (int i = 0; i < 10; i++) {
        int col = (rand() % 10);
        int row = (rand() % 10) + 1;
        powerBoard[row][col] = true;
    }
    
    printf("Player 2 please input your desired character!\n");
    scanf(" %c", &p2);
    getchar();
    system("clear"); 

    n = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
	if (n < 0) {
	    die_with_error("Error: recv() Failed.\n");
	}
    p1 = buffer[0];

    n = send(client_sock, &p2, sizeof(p2), 0);
    if (n < 0) {
	    die_with_error("Error: send() Failed.\n");
    }


    srand(time(0) + getpid()); // RANDOMIZER SEEDING

    while(1) {
         
        while (p1Pos != 100 || p2Pos != 100) {
            if(p1Pos == 100){
                displayTable(gameBoard, p1Pos, p2Pos, p1, p2);
                printf("PLAYER 1 IS THE WINNER\n");
                printf("THE GAME IS OVER\n");
                return 0;
            }
            else if(p2Pos == 100) {
                displayTable(gameBoard, p1Pos, p2Pos, p1, p2);
                printf("PLAYER 2 IS THE WINNER\n");
                printf("THE GAME IS OVER\n");
                return 0;
            }
            // Receive the updated position of player 1
            n = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
	        if (n < 0) {
	            die_with_error("Error: recv() Failed.\n");
	        }
	    // Convert string to integer
            p1Pos = *(int *)buffer;
             

            // Receive the updated position of player 2
            n = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
	        if (n < 0) {
	            die_with_error("Error: recv() Failed.\n");
	        }
	    // Convert string to integer
            p2Pos = *(int *)buffer;
             
            system("clear");
            displayTable(gameBoard, p1Pos, p2Pos, p1, p2);
            printf("Player 2 please roll the dice.\n(Press Enter.) >");

            while ((getchar()) != '\n');
            system("clear");


            int diceRoll = (rand() % 6) + 1;
            p2Pos += diceRoll;
            bool usedPowerUp = powerUp(diceRoll, powerBoard, &p1Pos, &p2Pos);

            if (p2Pos > 100) {
                int excess = p2Pos - 100;
                p2Pos = 100;
                p2Pos -= excess;
                printf("Oh no! you rolled too much!\n");
            }
            if (usedPowerUp) {
                printf("Player 2 utilized a power-up!\n");
            }

            displayTable(gameBoard, p1Pos, p2Pos, p1, p2);
            printf("Player 2 rolled %d.\n", diceRoll);
            // sleep(1);

            // Send the position of player 2 
            n = send(client_sock, &p2Pos, sizeof(p2Pos), 0);
            if (n < 0) {
                die_with_error("Error: send() Failed.\n");
            }
	    // Send the position of player 1
            n = send(client_sock, &p1Pos, sizeof(p1Pos), 0);
            if (n < 0) {
                die_with_error("Error: send() Failed.\n");
            }
            
        }   
       
    }
        
    close(client_sock);
    
    return 0;
}

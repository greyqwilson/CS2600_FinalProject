#include <stdio.h>
#include <stdlib.h>
#include <termios.h> //Thank you joerg bayer from stackoverflow
#include <unistd.h>

#define CMD_PUB "mosquitto_pub"
#define MQTT_IP "192.168.1.128"
#define MQTT_PORT 1833
#define TOPIC "Led_Signal"

void PrintMenu();

int main()
{
    struct termios old_tio, new_tio;
    unsigned char c;

    int choice = -1;
    char command[] = CMD_PUB;
    //Build command
    
    //Get terminal settings for stdin
    tcgetattr(STDIN_FILENO, &old_tio);

    //old settings will not be modified
    new_tio = old_tio;

    //turn off buffered i/o and echo
    new_tio.c_lflag &=(~ICANON & ~ECHO);

    PrintMenu();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    do {
        c = getchar();
        if (c == '1')
        {
            system("mosquitto_pub -h 192.168.1.128 -p 1883 -t \'Led_Signal\' -m \"1\"");
            c = '9';
            system("clear");
            PrintMenu();
        }
        else if (c == '2')
        {
            system("mosquitto_pub -h 192.168.1.128 -p 1883 -t \'Led_Signal\' -m \"2\"");
            c = '9';
            system("clear");
            PrintMenu();
        }
    } while (c != '0');

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    //scanf("%d", &choice);

    //If choice is 0, quit
    // while (choice != 0)
    // {
    //     if (choice == 1)
    //     {
    //         system("mosquitto_pub -h 192.168.1.128 -p 1883 -t \'Led_Signal\' -m \"1\"");
    //         choice = -1;
    //     }
    //     else if (choice == 2)
    //     {
    //         system("mosquitto_pub -h 192.168.1.128 -p 1883 -t \'Led_Signal\' -m \"2\"");
    //         choice = -1;
    //     }
    //     scanf("%d", &choice);
    // }
    
    return 0;
}

void PrintMenu(){
    printf("MQTT Publisher");
    printf("\n1 - Turn ON led");
    printf("\n2 - Turn OFF led");
    printf("\n0 - Quit program");
    printf("\n");
}
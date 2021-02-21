#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <gcrypt.h>

#include "../include/rtt.h"
#include "../include/config.h"

char server_reply[2000];
std::string message = "GET / HTTP/1.1\r\n\r\n";
uint64_t bitmask;
std::vector<std::string> ips;
std::map<int, struct sockaddr_in> sockaddrs;

char *cmd;
char **cmdargv;

int32_t
main(int32_t argc, char **argv)
{
    cmd = argv[0];
    cmdargv = argv;

    int32_t opt = -1;
    while ((opt = getopt(argc, argv, ":n:bh")) != -1) {
        switch(opt) {
            case 'n':
                nbits = atoi(optarg);
                break;
            case 'b':
                print_binary = true;
                break;
            case 'h':
                printHelp();
                return(0);
                break;
        }
    }

    for(auto it: urls)
        ips.push_back(hostname_to_ip(it));

    int sock_desc;
    struct sockaddr_in server;

    for(auto it: ips) {
        sock_desc = socket(AF_INET, SOCK_STREAM, 0);
        if(sock_desc == -1) {
            execvp(argv[0], argv);
        }

        server.sin_addr.s_addr = inet_addr(it.c_str());
        server.sin_family = AF_INET;
        server.sin_port = htons(80);

        sockaddrs[sock_desc] = server;

        if(connect(sock_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
            execvp(argv[0], argv);
        }
    }

    while (true) {

        uint32_t bits = 8;
        uint64_t entropy = nextValue();
        while (bits < nbits) {
            entropy = entropy << 8;
            entropy |= nextValue();
            bits += 8;
        }

        uint64_t *drbg = (uint64_t *)gcry_random_bytes(nbits/8, GCRY_STRONG_RANDOM);
        entropy ^= (*drbg & bitmask);

        if (print_binary) {
            std::cout << std::bitset<64>(entropy) << std::endl;
        } else {
            std::cout << entropy << std::endl;
        }
    }
}

std::string
hostname_to_ip(std::string &ip_str) {
    struct hostent *he;
    struct in_addr **addr_list;

    if((he = gethostbyname(ip_str.c_str())) == nullptr) {
        std::cerr << "getbyhostname error\n";
        exit(1);
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    char *c_ip = inet_ntoa(*addr_list[0]);
    std::string ip(c_ip);

    return ip;
}

uint8_t nextValue(void)
{
    auto start = std::chrono::high_resolution_clock::now();

    for(auto it: sockaddrs) {


        if(send(it.first, message.c_str(), message.size(), 0) < 0) {
            execvp(cmd, cmdargv);
        }

        if(recv(it.first, server_reply, 2000, 0) < 0) {
            execvp(cmd, cmdargv);
        }

    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>( stop-start );
    return duration.count() & 0b11111111;
}

void printHelp(void)
{
    std::cout << "Usage: ./rtt [-b] [-n <int>]\n"
        << "-b\t\tprint output in binary\n"
        << "-n <int>\tset number of bits to get (will get rounded to next multiple of 8)"
        << std::endl;
}

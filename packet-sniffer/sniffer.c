#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <pcap.h>



// To hold the error message if any of the pcap function
// calls fail. 
char errbuf[PCAP_ERRBUF_SIZE];  
// Packet capture session handler. 
pcap_t *handle;
// Berekly packet filter. To filter out traffic if needed. 
struct bpf_program fp;        
// The current network's mask. 
bpf_u_int32 maskp;            
// The current network's ipaddress. 
bpf_u_int32 netp;  
// Device to sniff on. 
char *device = NULL;
char *filter_protocol = NULL;


/*
 *  args: the arguments we passed to the callback, here it's NULL for now. 
 *  pkthdr: Is pcap header, which containsi inforamtion about the packet sniffed:
 *      struct pcap_pkthdr {
 *          struct timeval ts; 
 *          bpf_u_int32 caplen;
 *          bpf_u_int32 len;
 *      }
 *  packet: Points to the first byte of a chunk of data containing the entire packet. 
 *          To extract the data, we'll have to accordingly typecast it certain packet headers
 *          structures. 
 */
void callback(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    static unsigned long long count=0;

    //printf("Packet Count:%lld\n", ++count);    /* Number of Packets */
    //printf("Recieved Packet Size: %d\n", pkthdr->len);    /* Length of header */
    //printf("Time secs: %ld\n", pkthdr->ts.tv_sec);
    //printf("Time u_secs: %ld\n", pkthdr->ts.tv_usec);
    //printf("%ld\n", (pkthdr->ts.tv_sec)*1000000L + pkthdr->ts.tv_usec);
    printf("%ld\n", pkthdr->ts.tv_usec);
    /*
     *printf("Payload:\n");                     [> And now the data <]
     *for(int i=0; i<pkthdr->len; i++) {
     *    if(isprint(packet[i]))                
     *        printf("%c ",packet[i]);         
     *    else
     *        printf(" . ");          [> If not print a . <]
     *    if((i%16==0 && i!=0) || i==pkthdr->len-1)
     *        printf("\n");
     *}
     */
}

int parseInput(int argc , char **argv) {
    // If more that 5 arguments, it's invalid. 
    if (argc > 5 || argc == 4 || argc == 2)
        return 0;
    
    if (strcmp(argv[1], "-p") == 0 )
        filter_protocol = strdup(argv[2]);
    else if (strcmp(argv[1], "-d") == 0 )
        device = strdup(argv[2]);
    else 
        // the second parameter can either be "-p" or "-d".
        // and thing else is invalid. 
        return 0;
    
    if (argc == 5) {
        if (strcmp(argv[3], "-p") == 0 )
            filter_protocol = strdup(argv[4]);
        else if (strcmp(argv[3], "-d") == 0 )
            device = strdup(argv[4]);
        else 
            return 0;
    }
    else {
        // This means either "-p" or "-d" is default. 
       
        // "-p" is set but device is default
        if (strcmp(argv[1], "-p") == 0)
            device = strdup("any");
        else
            // "-d" is set but "-p" is none.
            filter_protocol = NULL;
    }

    return 1;
}

int main(int argc,char **argv)
{
    int ret;

    if(argc > 1){
        if(!parseInput(argc, argv)) {
            fprintf(stdout, "Usage: %s \" [-p Protocol]  [-d device]\"\n" ,argv[0]);
            return 0;
        }
    } 
    else {
        // default setup, i.e. no filter and sniff on device "any".
        device = strdup("any");
        filter_protocol = NULL;
    }

/*
 *     //Now get a device 
 *     devce = pcap_lookupdev(errbuf); 
 *
 *    if(device == NULL) {
 *        fprintf(stderr, "%s\n", errbuf);
 *        exit(1);
 *    }
 */
    // Get the network address and mask of the device.  
    // Returns -1 on failure. 
    ret = pcap_lookupnet(device, &netp, &maskp, errbuf);
    if (ret) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", device, errbuf);
        netp = 0;
        maskp = 0;
    }

    // Open device for reading in promiscuous mode 
    // Returns a pointer to a session handler of type pcap_t*.
    handle = pcap_open_live(device, BUFSIZ, 1,-1, errbuf);
    if(handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n",device, errbuf);
        return 2;
    }

    //printf("Sniffing On Device: %s", device);
    //printf("Filtered Expression: %s", filter_protocol);
    //filter_protocol = strdup("dst host 192.168.1.8"); 
    // Applying the filter if required else sniffing all the packets. 
    if (filter_protocol != NULL) {
        // Compile the filter expression
         printf("In here\n");
        if(pcap_compile(handle, &fp, filter_protocol, 0, netp) == -1) {
            fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_protocol, pcap_geterr(handle));
            return 2;
        }

        /* set the filter */
        if(pcap_setfilter(handle, &fp) == -1) {
            fprintf(stderr, "Error setting filter %s: %s\n", filter_protocol, pcap_geterr(handle));
            return 2;
        }
         printf("Outta here\n");
    }

    // Setting timestamp precision to nanoseconds. 
    /*
     *ret = pcap_set_tstamp_precision(handle, PCAP_TSTAMP_PRECISION_NANO);
     *if (ret) {
     *    if (ret == PCAP_ERROR_TSTAMP_PRECISION_NOTSUP)
     *        fprintf(stderr, "Nanoseconds not supported");
     *    else
     *        fprintf(stderr, "Capture handle activated already");
     *} 
     */

    // Processes packets from live capture. 
    pcap_loop(handle, -1, callback, NULL);
    return 0;
}

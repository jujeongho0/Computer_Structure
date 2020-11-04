#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> // for getopt()

#define BYTES_PER_WORD 4

int time = 0;

typedef struct
{
    int age;
    int valid;
    int modified;
    uint32_t tag;
} cline;

typedef struct
{
    cline *lines;
} cset;

typedef struct
{
    int s; // index bits
    int E; // way
    int b; // block offset bits
    cset *sets;
} cache;

int index_bit(int n){
    int cnt = 0;

    while(n) {
        cnt++;
        n = n >> 1;
    }

    return cnt-1;
}

cache build_cache(int S, int E, int b)//16 8 8
{  
    cache simCache;
    simCache.sets = malloc(sizeof(cset)*S);
    for(int i=0;i<S;i++)
    {
        simCache.sets[i].lines = malloc(sizeof(cline)*E);
    }
    
    for (int i = 0; i < S; i++)
    {
        for (int j = 0; j < E; j++)
        {
            simCache.sets[i].lines[j].age = 0;
            simCache.sets[i].lines[j].valid = 0;
            simCache.sets[i].lines[j].modified = 0;
        }
    }
   
    simCache.s = index_bit(S);//4 index bit
    simCache.E = E;//8 way
    simCache.b = index_bit(E);//3 offset bit
   

    return simCache;
}

void access_cache(cache* L,char* op,uint32_t addr,int *read,int *write,int* writeback,int* readhit,int* writehit,int* readmiss,int* writemiss)
{
    int tag,index,check=0;       

    index = (addr >> L->b) & 0xf;
    tag = (addr >> (L->s + L->b));   

   

    for (int i = 0; i < 1<<L->s; i++)
    {
        for (int j = 0; j < L->E; j++)
        {
            if(L->sets[i].lines[j].valid == 1)
                L->sets[i].lines[j].age++;           
        }
    }   
    
    if(strcmp(op,"R") == 0)
    {        
        *read = *read + 1;
        
        for(int i=0;i<L->E;i++)
        {    
            if(L->sets[index].lines[i].tag == tag && L->sets[index].lines[i].valid == 1)
            {                
                *readhit = *readhit + 1;
                L->sets[index].lines[i].age = 0;                
                check = 1;             
                break;
            }
        }    

        if(!check)
        {
            *readmiss = *readmiss + 1;
            
            for(int i=0;i<L->E;i++)
            {
                if(! L->sets[index].lines[i].valid)
                {                    
                    L->sets[index].lines[i].tag = tag;
                    L->sets[index].lines[i].valid = 1;                                          
                    check = 1;                   
                    break;
                }
            }            
        }
        
        if(!check)
        {         
           int LRU=0,LRU_way;
           for(int i=0;i<L->E;i++)
            {
                if(L->sets[index].lines[i].age > LRU)
                {
                    LRU = L->sets[index].lines[i].age;
                    LRU_way = i;                   
                }
            }    

            if(L->sets[index].lines[LRU_way].modified)
            {
                *writeback = *writeback + 1; 
                L->sets[index].lines[LRU_way].modified = 0;
                L->sets[index].lines[LRU_way].tag = tag;            
                L->sets[index].lines[LRU_way].age = 0;    
            }
            
            else
            {
                L->sets[index].lines[LRU_way].tag = tag;            
                L->sets[index].lines[LRU_way].age = 0;    
            }                             
        }
    }
    else 
    {
        *write = *write + 1;
        for(int i=0;i<L->E;i++)
        {
            if(L->sets[index].lines[i].tag == tag && L->sets[index].lines[i].valid == 1)
            {
                *writehit = *writehit + 1;
                check = 1;
                L->sets[index].lines[i].age = 0; 
                L->sets[index].lines[i].modified = 1;               
                            
                break;
            }
        }

        if(!check)
        {
            *writemiss = *writemiss + 1;
            for(int i=0;i<L->E;i++)
            {
                if(! L->sets[index].lines[i].valid)
                {
                    L->sets[index].lines[i].tag = tag;
                    L->sets[index].lines[i].valid = 1;   
                    L->sets[index].lines[i].modified = 1;
                                                                                        
                    check = 1;
                    break;
                }
            }     
        }

        if(!check)
        {
           int LRU=0,LRU_way;
           for(int i=0;i<L->E;i++)
            {
                if(L->sets[index].lines[i].age > LRU)
                {
                    LRU = L->sets[index].lines[i].age;
                    LRU_way = i;                   
                }
            }     

            if(L->sets[index].lines[LRU_way].modified)
            {
                *writeback = *writeback + 1;                     
                L->sets[index].lines[LRU_way].tag = tag;       
                L->sets[index].lines[LRU_way].age = 0; 
            }
            else
            {
                L->sets[index].lines[LRU_way].modified = 1;                    
                L->sets[index].lines[LRU_way].tag = tag;       
                L->sets[index].lines[LRU_way].age = 0; 
            }           
        }
    }   
}

void cdump(int capacity, int assoc, int blocksize)
{

    printf("Cache Configuration:\n");
    printf("-------------------------------------\n");
    printf("Capacity: %dB\n", capacity);
    printf("Associativity: %dway\n", assoc);
    printf("Block Size: %dB\n", blocksize);
    printf("\n");
}

void sdump(int total_reads, int total_writes, int write_backs,
           int reads_hits, int write_hits, int reads_misses, int write_misses)
{
    printf("Cache Stat:\n");
    printf("-------------------------------------\n");
    printf("Total reads: %d\n", total_reads);
    printf("Total writes: %d\n", total_writes);
    printf("Write-backs: %d\n", write_backs);
    printf("Read hits: %d\n", reads_hits);
    printf("Write hits: %d\n", write_hits);
    printf("Read misses: %d\n", reads_misses);
    printf("Write misses: %d\n", write_misses);    
    printf("\n");
}

void xdump(cache* L)
{
    int i, j, k = 0;
    int b = L->b, s = L->s; // index bit, offset bit
    int way = L->E, set = 1 << s; // way

    uint32_t line;

    printf("Cache Content:\n");
    printf("-------------------------------------\n");

    for(i = 0; i < way; i++) {
        if(i == 0) {
            printf("    ");
        }
        printf("      WAY[%d]", i);
    }
    printf("\n");

    for(i = 0; i < set; i++) {
        printf("SET[%d]:   ", i);

        for(j = 0; j < way; j++) {
            if(k != 0 && j == 0) {
                printf("          ");
            }
            if(L->sets[i].lines[j].valid) {
                line = L->sets[i].lines[j].tag << (s + b);
                line = line | (i << b);
            }
            else {
                line = 0;
            }
            printf("0x%08x  ", line);
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char *argv[])
{
    int capacity=1024;
    int way=8;
    int blocksize=8;
    int set;
    //Tag : 25Bit
    //Index : 4Bit 
    //Offset : 3Bit

    // Cache
    cache simCache;

    // Counts
    int read=0, write=0, writeback=0;
    int readhit=0, writehit=0;
    int readmiss=0, writemiss = 0;

    // Input option
    int opt = 0;
    char* token;
    int xflag = 0;

    // Parse file
    char *trace_name = (char*)malloc(32);
    FILE *fp;
    char line[16];
    char *op;
    uint32_t addr;

    trace_name = argv[argc-1];
    if (argc < 3) {
        printf("Usage: %s -c cap:assoc:block_size [-x] input_trace \n", argv[0]);
        exit(1);
    }

    while((opt = getopt(argc, argv, "c:x")) != -1) {
        switch(opt) {
            case 'c':
                token = strtok(optarg, ":");
                capacity = atoi(token);
                token = strtok(NULL, ":");
                way = atoi(token);
                token = strtok(NULL, ":");
                blocksize  = atoi(token);
                break;

            case 'x':
                xflag = 1;
                break;

            default:
                printf("Usage: %s -c cap:assoc:block_size [-x] input_trace \n", argv[0]);
                exit(1);

        }
    }
    
    set = capacity / way / blocksize; 
    simCache = build_cache(set,way,blocksize);//16 8 8    

    fp = fopen(trace_name, "r"); // read trace file
    if(fp == NULL) {
        printf("\nInvalid trace file: %s\n", trace_name);
        return 1;
    }

    cdump(capacity, way, blocksize);  

    while (fgets(line, sizeof(line), fp) != NULL) {
        op = strtok(line, " ");
        addr = strtoull(strtok(NULL, ","), NULL, 16);
        
        
        access_cache(&simCache,op,addr,&read,&write,&writeback, &readhit, &writehit, &readmiss, &writemiss);
    }

    sdump(read, write, writeback, readhit, writehit, readmiss, writemiss);
    if (xflag) {
        xdump(&simCache);
    }

    return 0;
}
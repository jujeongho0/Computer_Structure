#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/******************************************************
 * Structure Declaration
 *******************************************************/

struct Inst {
    char name[100];
    char op[100];
    char type;
    char funct[100];
};

struct Text {
    int idx;
    char d[100];
    char s[100];
    char t[100];
    unsigned int address;
    struct Text* next;
};

struct Sym {
    char name[100];
    int size;
    int value;
    unsigned int address;
    struct Sym* daum;
    struct Sym* next;    
};


struct Inst inst[20] = {
    {"addiu",   "001001", 'I', ""}, //$ $ 0x1
    {"addu",    "000000", 'R', "100001"},//$ $ $
    {"and",     "000000", 'R', "100100"},//$ $ $
    {"andi",    "001100", 'I', ""},//$ $ 100
    {"beq",     "000100", 'I', ""},//$ $ lab
    {"bne",     "000101", 'I', ""},//$ $ lab
    {"j",       "000010", 'J', ""},//lab
    {"jal",     "000011", 'J', ""},//lab
    {"jr",      "000000", 'J', "001000"},//$
    {"lui",     "001111", 'I', ""},//$ data
    {"lw",      "100011", 'I', ""},//$ -4($)
    {"nor",     "000000", 'R', "100111"},//$ $ $
    {"or",      "000000", 'R', "100101"},//$ $ $
    {"ori",     "001101", 'I', ""},//$ $ 255
    {"sltiu",   "001011", 'I', ""},//$ $ 100
    {"sltu",    "000000", 'R', "101011"},//$ $ $
    {"sll",     "000000", 'S', "000000"},//$ $ 1
    {"srl",     "000000", 'S', "000010"},//$ $ 1
    {"sw",      "101011", 'I', ""},//$ 16($)
    {"subu",    "000000", 'R', "100011"}//$ $ $
};
struct Sym* Symbols;
struct Text* Texts;
int datasize, textsize;

char* change_file_ext(char* str);
char* num_to_bits(unsigned int num, int len);

void read_asm();
void subst_asm_to_num();
void print_bits();


int main(int argc, char* argv[]) {
    
    FILE* input, * output;
    char* filename;


    if (argc != 2) {
        fprintf(stderr, "Usage: %s <*.s>\n", argv[0]);
        fprintf(stderr, "Example: %s sample_input/example?.s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // reading the input file
    input = freopen(argv[1], "r", stdin);
    if (input == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    // creating the output file (*.o)
    filename = strdup(argv[1]); // strdup() is not a standard C library but fairy used a lot.
    if (change_file_ext(filename) == NULL) {
        fprintf(stderr, "'%s' file is not an assembly file.\n", filename);
        exit(EXIT_FAILURE);
    }

    output = freopen(filename, "w", stdout);
    if (output == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    Symbols = (struct Sym*)malloc(sizeof(struct Sym));  
    Texts = (struct Text*)malloc(sizeof(struct Text));

    Symbols->next = NULL;
    Texts->next = NULL;
    Symbols->daum = NULL;
    
    read_asm();
    subst_asm_to_num();
    print_bits();

    // freeing the memory
    free(filename);
    free(Symbols);
    free(Texts);

    fclose(input);
    fclose(output);
    exit(EXIT_SUCCESS);
}

char* change_file_ext(char* str) {
    char* dot = strrchr(str, '.');

    if (!dot || dot == str || (strcmp(dot, ".s") != 0))
        return NULL;

    str[strlen(str) - 1] = 'o';
    return "";
}

char* num_to_bits(unsigned int num, int len) {
    char* bits = (char*)malloc(len + 1);
    int idx = len - 1, i;

    while (num > 0 && idx >= 0) {
        if (num % 2 == 1) {
            bits[idx--] = '1';
        }
        else {
            bits[idx--] = '0';
        }
        num /= 2;
    }

    for (i = idx; i >= 0; i--) {
        bits[i] = '0';
    }

    return bits;
}


void read_asm() {
    int i;
    unsigned int address;
    char temp[0x1000] = { 0 };
    char temp1[0x1000] = { 0 };          
    struct Sym* curSym;        
    struct Sym* curData;    
    struct Text* curText;
    
    

    //Read .data region
    address = 0x10000000;
    for (i = 0; scanf("%s", temp) == 1;) {
        
        if (strcmp(temp, ".text") == 0) {
            
            break;

        }
        else if (temp[strlen(temp) - 1] == ':') {             
         
            struct Sym* temp_sym = malloc(sizeof(struct Sym));            
            temp[strlen(temp) - 1] = '\0';
            strcpy(temp_sym->name ,temp);
            temp_sym->address = address;
            temp_sym->size = 4;
            curSym = Symbols;
            while (curSym->next != NULL)
            {                
                curSym = curSym->next;
            }
            temp_sym->next = curSym->next;
            curSym->next = temp_sym;      
               

        }
        else if (strcmp(temp, ".word") == 0) {

            struct Sym* temp_data = malloc(sizeof(struct Sym));          
            address += 4;
            scanf("%s", temp);
            if (temp[1] == 'x')
            {
                temp_data->value = strtol(temp, NULL, 16);
            }
            else
                temp_data->value = atoi(temp);           

            curData = Symbols;             
            while (curData->daum != NULL)
            {
                curData = curData->daum;
            }              
            temp_data->daum = curData->daum;
            curData->daum = temp_data;                

        }
    }    

    datasize = address - 0x10000000;
 
    
    //Read .text region
    address = 0x400000;
    for (i = 0; scanf("%s", temp) == 1;) {
        
        if (temp[strlen(temp) - 1] == ':') {

            struct Sym* temp_sym = malloc(sizeof(struct Sym));            
            temp[strlen(temp) - 1] = '\0';
            strcpy(temp_sym->name ,temp);
            temp_sym->address = address;
            temp_sym->size = 4;
            curSym = Symbols;
            while (curSym->next != NULL)
            {                
                curSym = curSym->next;
            }
            temp_sym->next = curSym->next;
            curSym->next = temp_sym;   
      

        }
        else {

            struct Text* temp_text = malloc(sizeof(struct Text));;
                   
            if (strcmp(temp, "la") == 0)
            {
                scanf("%s", temp);
                scanf("%s", temp1);
                struct Sym* cur = Symbols->next;
                unsigned int add;
                while (cur != NULL) {
                    if (strcmp(temp1, cur->name) == 0)
                    {
                        add = cur->address;
                        break;
                    }
                    cur = cur->next;
                }
                if (add == 0x10000000)
                {
                    temp_text->idx = 9;
                    temp_text->address = address;
                    address += 4;
                    temp[0] = '0';
                    temp[strlen(temp) - 1] = '\0';
                    strcpy(temp_text->s , "0");
                    strcpy(temp_text->t , temp);
                    strcpy(temp_text->d , "4096");
                    
                    curText = Texts;
                    while (curText->next != NULL)
                    {
                        curText = curText->next;
                    }
                    temp_text->next = curText->next;
                    curText->next = temp_text;

                    continue;
                }
                else {
                    temp_text->idx = 9;
                    temp_text->address = address;
                    address += 4;
                    temp[0] = '0';
                    temp[strlen(temp) - 1] = '\0';
                    strcpy(temp_text->s , "0");
                    strcpy(temp_text->t , temp);
                    strcpy(temp_text->d , "4096");

                    curText = Texts;
                    while (curText->next != NULL)
                    {
                        curText = curText->next;
                    }
                    temp_text->next = curText->next;
                    curText->next = temp_text;


                    struct Text* temp_text = malloc(sizeof(struct Text));;
                    temp_text->idx = 13;
                    temp_text->address = address;
                    address += 4;
                    strcpy(temp_text->s, temp);
                    strcpy(temp_text->t , temp);
                    sprintf(temp_text->d, "%d", (add - 0x10000000));
                    curText = Texts;
                    while (curText->next != NULL)
                    {
                        curText = curText->next;
                    }
                    temp_text->next = curText->next;
                    curText->next = temp_text;
                    continue;
                }
            }
            for (i = 0; i <= 20; i++)
            {
                if (strcmp(temp,inst[i].name)==0)
                {
                    break;
                }
            }
            

            temp_text->idx = i;
            temp_text->address = address;
            address += 4;
            if (i == 1 || i == 2 || i == 11 || i == 12 || i == 15 || i == 19)//R포맷 d s t
            {
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->d ,temp);
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->s,temp);
                scanf("%s", temp);
                temp[0] = '0';
                strcpy(temp_text->t ,temp);
                curText = Texts;
                while (curText->next != NULL)
                {
                    curText = curText->next;
                }
                temp_text->next = curText->next;
                curText->next = temp_text;

                
            }
            else if ( i == 4 || i == 5 )
            {
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->s , temp);
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->t , temp);
                scanf("%s", temp);
                if (temp[1] == 'x')
                {
                    char a[100];
                    sprintf(a, "%ld", strtol(temp, NULL, 16));
                    strcpy(temp_text->d ,a);
                }
                else
                    strcpy(temp_text->d , temp);
                curText = Texts;
                while (curText->next != NULL)
                {
                    curText = curText->next;
                }
                temp_text->next = curText->next;
                curText->next = temp_text;
            }
            else if (i == 0 || i == 3 ||  i == 13 || i == 14)//I포맷 t s d
            {
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->t , temp);
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->s , temp);
                scanf("%s", temp);
                if (temp[1] == 'x')
                {
                    char a[100];
                    sprintf(a, "%ld", strtol(temp, NULL, 16));
                    strcpy(temp_text->d ,a);
                }
                else
                    strcpy(temp_text->d , temp);
                curText = Texts;
                while (curText->next != NULL)
                {
                    curText = curText->next;
                }
                temp_text->next = curText->next;
                curText->next = temp_text;
            }
            else if (i == 9)//lui
            {
                strcpy(temp_text->s , "0");
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->t , temp);
                scanf("%s", temp);
                if (temp[1] == 'x')
                {
                    char a[100];
                    sprintf(a, "%ld", strtol(temp, NULL, 16));
                    strcpy(temp_text->d, a);
                }
                else
                    strcpy(temp_text->d , temp);
                curText = Texts;
                while (curText->next != NULL)
                {
                    curText = curText->next;
                }
                temp_text->next = curText->next;
                curText->next = temp_text;
            }
            else if (i == 10 || i == 18)//I포맷 lw sw
            {
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->t ,temp);
                scanf("%s", temp);
                char* p;
                p = strtok(temp, "(,$,)");
                strcpy(temp_text->d , p);
                p = strtok(NULL, "(,$,)");
                strcpy(temp_text->s , p);
                curText = Texts;
                while (curText->next != NULL)
                {
                    curText = curText->next;
                }
                temp_text->next = curText->next;
                curText->next = temp_text;
            }
            else if (i == 16 || i == 17)//S포맷 t s d
            {
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->t , temp);
                scanf("%s", temp);
                temp[0] = '0';
                temp[strlen(temp) - 1] = '\0';
                strcpy(temp_text->s , temp);
                scanf("%s", temp);
                strcpy(temp_text->d , temp);
                curText = Texts;
                while (curText->next != NULL)
                {
                    curText = curText->next;
                }
                temp_text->next = curText->next;
                curText->next = temp_text;
            }
            else if (i == 6 || i == 7)//J포맷 s   j jal 
            {
                scanf("%s", temp);
                strcpy(temp_text->s, temp);
                curText = Texts;
                while (curText->next != NULL)
                {
                    curText = curText->next;
                }
                temp_text->next = curText->next;
                curText->next = temp_text;
            }
            else//J포맷 jr
            {
                scanf("%s", temp);
                temp[0] = '0';
                strcpy(temp_text->s , temp);
                curText = Texts;
                while (curText->next != NULL)
                {
                    curText = curText->next;
                }
                temp_text->next = curText->next;
                curText->next = temp_text;
            }
        }
    }
   
    textsize = address - 0x400000;
   
}

void subst_asm_to_num() {
    struct Text* text;

    for (text = Texts->next; text != NULL; text = text->next) {
        if (text->idx == 4 || text->idx == 5)
        {
            struct Sym* cur = Symbols->next;
            unsigned int add;
            while (cur != NULL) {
                if (strcmp(text->d, cur->name) == 0)
                {
                    add = cur->address;
                    break;
                }
                cur = cur->next;
            }
            int numm = add - text->address - 4;
            if(numm<0)
            {
                numm = numm * (-1);
                numm = numm / 4;
                numm = numm * (-1);
            }
            else{
                numm = numm / 4;
            }
            
            sprintf(text->d, "%d", numm);
            
        }
        else if (text->idx == 6 || text->idx == 7)
        {
            struct Sym* cur = Symbols->next;
            int add;
            while (cur != NULL) {
                if (strcmp(text->s, cur->name) == 0)
                {
                    
                    add = cur->address/4;
                    break;
                }
                cur = cur->next;
            }
            strcpy(text->s,num_to_bits(add,26));
            
        }
    }
}

void print_bits() {
    struct Text* text;
    struct Sym* sym;
    struct Data* data;

   
     

    // print the header
    printf("%s", num_to_bits(textsize, 32));
    printf("%s", num_to_bits(datasize, 32));

    // print the body
    for (text = Texts->next; text != NULL; text = text->next) {
       printf("%s", inst[text->idx].op);

        if (inst[text->idx].type == 'R') {
            printf("%s%s%s00000%s", num_to_bits(atoi(text->s), 5), num_to_bits(atoi(text->t), 5),
                num_to_bits(atoi(text->d), 5), inst[text->idx].funct);

        }
        else if (inst[text->idx].type == 'I') {
            printf("%s%s%s", num_to_bits(atoi(text->s), 5), num_to_bits(atoi(text->t), 5),
                num_to_bits(atoi(text->d), 16));

        }
        else if (inst[text->idx].type == 'S') {
            printf("00000%s%s%s%s", num_to_bits(atoi(text->s), 5), num_to_bits(atoi(text->t), 5),
                num_to_bits(atoi(text->d), 5), inst[text->idx].funct);

        }
        else {
            if (text->idx == 8)
            {
                printf("%s000000000000000%s", num_to_bits(atoi(text->s), 5), inst[text->idx].funct);
            }
            else
            {
                printf("%s", text->s);
            }


        }
    }

    for (sym = Symbols->daum; sym != NULL; sym = sym->daum) {   

            printf("%s", num_to_bits(sym->value, 32));
            
        }
    printf("\n"); // To exclude "No newline at end of file"
}

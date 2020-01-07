#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct {
                    unsigned int count;
                    int child_0;
                    int child_1;
               }NODE;

typedef struct {
                    unsigned int code;
                    unsigned int code_bits;
               }CODE;
unsigned long count_frequency(FILE *,unsigned long *);
void scale_frequency(unsigned long *,NODE *);
void store_frequency(FILE *,NODE *);
int build_huffman_tree(NODE *);
void convert_tree_to_table(NODE *,CODE *,unsigned int,unsigned int,unsigned int);
unsigned long bitwise_write(FILE *,FILE *,CODE *);

int main()
{
    char s_file[100],t_file[100],f_storage[100];
    FILE *sfp,*tfp,*temp;
    printf("Enter the name of file to be compressed or its path: ");
    gets(s_file);
    if(s_file[strlen(s_file)-4]!='.')
        strcpy(s_file+strlen(s_file),".txt");
    sfp=fopen(s_file,"r");
    if(sfp==NULL)
    {
        printf("\aERROR : %s Can't be opened.\n",s_file);
        exit(1);
    }
    strcpy(t_file,s_file);
    strcpy(t_file+strlen(t_file)-3,"mkt");
    tfp=fopen(t_file,"w");
    if(tfp==NULL)
    {
        printf("\aERROR : Compressed file can't be created.\n");
        exit(2);
    }
    strcpy(f_storage,s_file);
    strcpy(f_storage+strlen(f_storage)-4,"(Counts).txt");
    temp=fopen(f_storage,"w");
    if(temp==NULL)
    {
        printf("\aERROR : A subsidiary file can't be created.\n");
        exit(3);
    }
    unsigned long *counts=(unsigned long *)calloc(256,sizeof(unsigned long));
    NODE *nodes=(NODE *)calloc(514,sizeof(NODE));
    CODE *codes=(CODE *)calloc(257,sizeof(CODE));
    int root_node,i,j;
    unsigned long f_size,c_size;
    f_size=count_frequency(sfp,counts);
    rewind(sfp);
    scale_frequency(counts,nodes);
    store_frequency(temp,nodes);
    root_node=build_huffman_tree(nodes);
    convert_tree_to_table(nodes,codes,0,0,root_node);
    c_size=bitwise_write(tfp,sfp,codes);
    printf("\n\nSource File Size = %lu KB\nCompressed File Size = %lu KB\n\nCompression Percentage = %u\n",f_size/1024,c_size/1024,c_size*100/f_size);
    printf("\n***Please store & carry the files -\n\n\t\t%s\n\t\t%s\n\nfor decompression.***\n\n\n",t_file,f_storage);
    fclose(sfp);
    fclose(tfp);
    fclose(temp);
    return 0;
}

/*Counts the frequency of all the ASCII charecters in the input file.*/

unsigned long count_frequency(FILE *fp,unsigned long *counts)
{
    unsigned char ch;
    unsigned long size=0;
    while((ch=getc(fp))!=255)
    {
        counts[ch]++;
        size++;
    }
    return ++size;
}

/*Scales down the frequencies to the range 0 to 256 for increasing the speed of program.*/

void scale_frequency(unsigned long *counts,NODE *nodes)
{
    unsigned long max_count=0;
    register int i;
    for(i=0;i<256;i++)
    {
        if(counts[i]>max_count)
            max_count=counts[i];
    }
    max_count=max_count/255+1;
    for(i=0;i<256;i++)
    {
        nodes[i].count=(unsigned int)(counts[i]/max_count);
        //printf("%d\n",nodes[i].count);
        if(!nodes[i].count && counts[i])
            nodes[i].count=1;
    }
    nodes[256].count=1;
}

/*Stores te frequencies of charecters for decompression purpose.*/

void store_frequency(FILE *fp,NODE *nodes)
{
    register int i;
    for(i=0;i<=256;i++)
        fprintf(fp,"%u ",nodes[i].count);
    fclose(fp);
}

/*Builds the huffman tree.*/

int build_huffman_tree(NODE *nodes)
{
    register int next_free_node,i,min1,min2;
    nodes[513].count=65535;
    for(next_free_node=257;;next_free_node++)
    {
        min1=min2=513;
        for(i=0;i<next_free_node;i++)
        {
            if(nodes[i].count)
            {
                if(nodes[i].count<nodes[min1].count)
                {
                    min2=min1;
                    min1=i;
                }
                else if(nodes[i].count<nodes[min2].count)
                    min2=i;
            }
        }
        if(min2==513)
            break;
        nodes[next_free_node].count=nodes[min1].count+nodes[min2].count;
        nodes[next_free_node].child_0=min1;
        nodes[next_free_node].child_1=min2;
        nodes[min1].count=nodes[min2].count=0;
    }
    return (next_free_node-1);
}

/*Creates a table for the codes of each corresponding charecter.*/

void convert_tree_to_table(NODE *nodes,CODE *codes,unsigned int node_path,unsigned int bits,unsigned int node)
{
    if(node<=256)
    {
        codes[node].code=node_path;
        codes[node].code_bits=bits;
        return;
    }
    node_path<<=1;
    bits++;
    convert_tree_to_table(nodes,codes,node_path,bits,nodes[node].child_0);
    convert_tree_to_table(nodes,codes,node_path|1,bits,nodes[node].child_1);
}

/*Writes the compressed file.*/

unsigned long bitwise_write(FILE *tfp,FILE *sfp,CODE *codes)
{
    register int c=128,i,index;
    unsigned long size=0;
    unsigned char ch,byte=0;
    do
    {
        ch=getc(sfp);
        index=(ch!=255?ch:256);
        for(i=1<<(codes[index].code_bits-1);i;i>>=1)
        {
            byte+=((codes[index].code&i)?1:0)*c;
            c/=2;
            if(!c)
            {
                fputc(byte,tfp);
                byte=0;
                c=128;
                size++;
            }
        }
    }while(ch!=255);
    if(c!=128)
        fputc(byte,tfp);
    return size;
}

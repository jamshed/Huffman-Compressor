#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

typedef struct {
                    unsigned int count;
                    int child_0;
                    int child_1;
               }NODE;
void recover_frequency(FILE *,NODE *);
int build_huffman_tree(NODE *);
void decompress(FILE *,FILE *,NODE *);

int main()
{
    char s_file[100],t_file[100],f_storage[100];
    FILE *sfp,*tfp,*temp;
    int i;
    unsigned char ch;
    NODE *nodes=(NODE *)calloc(514,sizeof(NODE));
    printf("Enter the file name to be decompressed : ");
    gets(s_file);
    if(s_file[strlen(s_file)-4]!='.')
        strcpy(s_file+strlen(s_file),".mkt");
    sfp=fopen(s_file,"rb");
    if(sfp==NULL)
    {
        printf("\aERROR : %s Can't be opened.\n",s_file);
        exit(1);
    }
    strcpy(t_file,s_file);
    strcpy(t_file+strlen(t_file)-4,"(decompressed).txt");
    tfp=fopen(t_file,"w");
    if(tfp==NULL)
    {
        printf("\aERROR : Compressed file can't be decompressed.\n");
        exit(2);
    }
    strcpy(f_storage,s_file);
    strcpy(f_storage+strlen(f_storage)-4,"(Counts).txt");
    temp=fopen(f_storage,"r");
    recover_frequency(temp,nodes);
    decompress(sfp,tfp,nodes);
    fclose(sfp);
    fclose(tfp);
    fclose(temp);
    printf("\a\tFile Decompressed!.\n");
    return 0;
}

/*Recovers the frequencies of the charecters.*/

int extra[300];

void recover_frequency(FILE *temp,NODE *nodes)
{
    register int i;
    for(i=0;i<=256;i++)
        {
            fscanf(temp,"%u",&nodes[i].count);
            extra[i] = nodes[i].count;
        }
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

/*Decompresses the compressed file.*/

void decompress(FILE *sfp,FILE *tfp,NODE *nodes)
{
    int root_node;
    register int i,node;
    unsigned char ch,n;
    node=root_node=build_huffman_tree(nodes);
    while(1)
    {
        fread(&ch,1,1,sfp);
        for(i=1<<7;i;i>>=1)
        {
            node=((i&ch)!=0?nodes[node].child_1:nodes[node].child_0);
            if(node<256)
            {
                //assert( extra[node] > 0);
                //printf("%c",node);
                fputc(node,tfp);
                node=root_node;
            }
            else if(node==256)
            {
                //printf(">>>\n");
                break;
            }
        }
        if(node==256)
            break;
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char cityNames[500][50];
int numberOfCities =0;


typedef struct Node
{
    int cityIndex;
    int distance;
    struct Node* next;
} Node;
typedef struct Node* node;


node adjCities[500];

typedef struct edge
{
    int city1;
    int city2;
    float weight;
} edge;
typedef struct edge* Edge;


typedef struct MSTNode
{
    float weight;//as in the distance
    int parent;// as where does it belong to
    int visited; // binary flag
} MSTNode;
typedef struct MSTNode* MSTnode;



int loadCities()
{
    int edges=0;
    FILE *file = fopen("cities.txt","r");
    char line[100];
    while (fgets(line,sizeof(line),file))
    {
        char city1[50],city2[50];
        int distance;
        sscanf(line, "%[^#]#%[^#]#%d", city1, city2, &distance);
        int one = findCity(city1);
        int two = findCity(city2);
        addEdge(one,two,distance);
        edges++;
    }
    fclose(file);
    printf("\n load is done! \n");

    for (int i=0; i<numberOfCities; i++)
    {
        printf("%s : ",cityNames[i]);
        node current = adjCities[i];
        while (current)
        {
            int inx = current->cityIndex;
            int dis = current -> distance;
            printf ("-> %s (%dkm)", cityNames[inx], dis);
            current = current->next;
        }
        printf("\n");
    }
    return edges;
}

int findCity(char* name)  // here i return the index of the city
{
    for (int i=0; i<numberOfCities; i++)
    {
        if (strcmp(cityNames[i],name)==0)
        {
            // printf("found city!\n");
            return i;
        }
    }
    strcpy(cityNames[numberOfCities],name);
    numberOfCities++;
    return numberOfCities-1;
}
int findCityIndex(char* name)
{
    for (int i = 0; i < numberOfCities; i++)
    {
        if (strcmp(cityNames[i], name) == 0)
        {
            return i;
        }
    }
    return -1;
}


void addEdge(int city1, int city2, int weight)
{
    node city1to2 = (node)malloc(sizeof(struct Node));
    city1to2 -> cityIndex = city2;
    city1to2 -> distance = weight;
    city1to2 -> next = adjCities[city1];
    adjCities[city1] = city1to2; // this holds the whole linkedlist


    node city2to1 = (node)malloc(sizeof(struct Node));
    city2to1 -> cityIndex = city1;
    city2to1 -> distance = weight;
    city2to1 -> next = adjCities[city2];
    adjCities[city2] = city2to1;


}

//END OF LOAD

typedef struct primHeap
{
    int cityIndex;
    float distance;
} primHeap;
typedef struct primHeap* Least;

Least minDistance[500];
int heapSize = 0;
int position[500];

void swap(int more, int less)
{
    Least temp = minDistance[more];
    minDistance[more] = minDistance[less];
    minDistance[less] = temp;

    position[minDistance[more]->cityIndex] = more;
    position[minDistance[less]->cityIndex] = less;
}

void heapifyPrim(int i)
{
    int least = i;
    int left = 2 * i;
    int right = 2 * i + 1;

    if (left <= heapSize && minDistance[left]->distance < minDistance[least]->distance)
        least = left;
    if (right <= heapSize && minDistance[right]->distance < minDistance[least]->distance)
        least = right;

    if (least != i)
    {
        swap(i, least);
        heapifyPrim(least);
    }
}


float primAlgo(int sourceIndex)
{
    MSTnode MST[numberOfCities]; // all cities are in this array of structs

    for (int i=0; i< numberOfCities; i++)
    {
        MST[i] = (MSTnode)malloc(sizeof(struct MSTNode));
        MST[i]->weight = 100000; //set all nodes to infinity
        MST[i]->parent = -1;// each node is a tree itself
        MST[i]->visited = 0;// so far we didnt visit any city
    }
    heapSize=numberOfCities;
    for (int i =1; i<=numberOfCities; i++)
    {
        minDistance[i] = (Least)malloc(sizeof(primHeap));
        minDistance[i]->cityIndex = i -1;
        minDistance[i]->distance = 100000;
        position[i-1]=i;
    }

    MST[sourceIndex]->weight = 0; // start from my source city
    minDistance[sourceIndex+1]->distance = 0;

    for (int i = heapSize / 2; i >= 1; i--)
    {
        heapifyPrim(i);
    }

    while (heapSize > 0)
    {

        Least current = minDistance[1];
        int minCityIndex = current->cityIndex;

        minDistance[1] = minDistance[heapSize];//next node
        position[minDistance[1]->cityIndex] = 1;
        heapSize=heapSize-1; // remove from heap

        heapifyPrim(1);

        MST[minCityIndex]->visited=1;

        node currentNeighbour = adjCities[minCityIndex];
        while (currentNeighbour != NULL)
        {
            int thisNeighbourIndex = currentNeighbour->cityIndex;
            float thisNeighbourdistance = currentNeighbour->distance;

            if (MST[thisNeighbourIndex]->visited == 0 && thisNeighbourdistance < MST[thisNeighbourIndex]->weight)
            {
                MST[thisNeighbourIndex]->weight = thisNeighbourdistance;
                MST[thisNeighbourIndex]->parent = minCityIndex;

                minDistance[position[thisNeighbourIndex]]->distance = thisNeighbourdistance;
                int pos = position[thisNeighbourIndex];
                while (pos > 1 && minDistance[pos/2]->distance > minDistance[pos]->distance)
                {
                    swap(pos, pos/2);
                    pos = pos/2;
                }
            }
            currentNeighbour = currentNeighbour->next;
        }

    }
    float cost = 0;
    for (int i = 0; i < numberOfCities; i++)
    {
        if (MST[i]->parent != -1)    // Skip the root node
        {
            cost += MST[i]->weight;
        }
    }

    printf("Minimum Spanning Tree:\n Starting from : %s \n", cityNames[sourceIndex]);

    for (int i = 0; i < numberOfCities; i++)
    {
        if (MST[i]->parent != -1)
        {

            printf("\nFrom %s to %s : %.2fkm\n",
                   cityNames[MST[i]->parent],
                   cityNames[i],
                   MST[i]->weight);
        }
    }

    for (int i = 0; i < numberOfCities; i++)
    {
        free(MST[i]);
    }
    for (int i = 1; i <= heapSize; i++)
    {
        free(minDistance[i]);
    }
    return cost;
}

// END OF PRIM


void buildMinHeap(Edge edgeHeap[], int eCount)
{
    for (int i = eCount/2; i>=1; i--)
    {
        Heapify(edgeHeap,eCount,i);
    }
}
void Heapify(Edge edgeHeap[], int eCount, int index)
{
    int min = index;
    if (2*index <= eCount)
    {
        if (edgeHeap[2*index]->weight<edgeHeap[min]->weight)
        {
            min = 2*index;
        }
    }
    if (2*index+1 <=eCount)
    {
        if (edgeHeap[2*index+1]->weight<edgeHeap[min]->weight)
        {
            min = 2*index+1;
        }
    }

    if (min != index)
    {
        Edge temp = edgeHeap[index];
        edgeHeap[index]=edgeHeap[min];
        edgeHeap[min]=temp;
        Heapify(edgeHeap,eCount,min);
    }
}
int findParent(int parents[],int city)
{
    if (parents[city]!=city)
    {
        parents[city]=findParent(parents,parents[city]);
    }
    return parents[city];

}
float kAlgo()
{

    Edge edgeHeap[500];
    int edgeCount = 1;

    for (int i = 0; i < numberOfCities; i++)
    {
        node current = adjCities[i];
        while (current != NULL)
        {

            edgeHeap[edgeCount] = (Edge)malloc(sizeof(struct edge));
            edgeHeap[edgeCount]->city1 = i;
            edgeHeap[edgeCount]->city2 = current->cityIndex;
            edgeHeap[edgeCount]->weight = current->distance;
            edgeCount++;

            current = current->next;
        }
    }
    buildMinHeap(edgeHeap, edgeCount-1);

    Edge MST[numberOfCities - 1];
    int parents[numberOfCities];
    int edges = 0;
    int heapSize = edgeCount - 1;

    for (int i = 0; i < numberOfCities; i++)
    {
        parents[i] = i;
    }

    printf("Kruskal's MST Edges:\n");

    while (heapSize > 0 && edges < numberOfCities - 1)
    {

        Edge chosen = edgeHeap[1];

        edgeHeap[1] = edgeHeap[heapSize];
        heapSize--;

        if (heapSize > 0)
        {
            Heapify(edgeHeap, heapSize, 1);
        }

        int from = chosen->city1;
        int dest = chosen->city2;

        int parentFrom = findParent(parents, from);
        int parentDest = findParent(parents, dest);

        if (parentFrom != parentDest)
        {
            MST[edges] = (Edge)malloc(sizeof(struct edge));
            MST[edges]->city1 = chosen->city1;
            MST[edges]->city2 = chosen->city2;
            MST[edges]->weight = chosen->weight;
            parents[parentDest] = parentFrom;
            edges++;
        }

        free(chosen);
    }

    printf("\nKruskal's MST Edges:\n");
    for (int i = 0; i < edges; i++)
    {
        printf("From %s to %s : %.2f km\n",
               cityNames[MST[i]->city1],
               cityNames[MST[i]->city2],
               MST[i]->weight);
    }

    float cost = 0;
    for (int i = 0; i < edges; i++)
    {
        cost += MST[i]->weight;
    }


    for (int i = 0; i < edges; i++)
    {
        free(MST[i]);
    }

    for (int i = 1; i <= heapSize; i++)
    {
        free(edgeHeap[i]);
    }

    return cost;
}




int main()
{
    for (int i = 0; i < 100; i++)
    {
        adjCities[i] = NULL;
    }
    int sourceIndex=-1;
    clock_t t,t2;
    float cost1,cost2;
    int edges;
    int choice =10;
    while (choice != 0)
    {
        printf("enter ur choice : ");
        scanf("%d",&choice);

        switch (choice)
        {
        case 1:
            edges = loadCities();
            printf("number of cities (order of the graph) is: %d \n",numberOfCities);
            printf("number of ways (size of the graph) is: %d \n",edges);
            break;
        case 2:
            printf("minimized map using prim's algp : \n");
            char source[50];
            printf("what is ur source city? :");
            scanf("%s",source);
            sourceIndex = findCityIndex(source);
            if (sourceIndex == -1)
            {
                printf("City not found!\n");
                return 2;
            }
            t= clock();
            cost1 = primAlgo(sourceIndex);
            t = clock()-t;
            printf("execution time is : %f seconds !\n",((double)t)/CLOCKS_PER_SEC);
            printf("total cost is: %0.2f km\n",cost1);
            break;
        case 3:
            printf("minimized map using kur's algp : \n");
            t2= clock();
            cost2=kAlgo();
            t2= clock() -t2;
            printf("execution time is : %f seconds !\n",((double)t2)/CLOCKS_PER_SEC);
            printf("total cost is: %0.2f km\n",cost2);
            break;
        case 4:
            if (numberOfCities == 0)
            {
                printf("Please load cities data first\n");
                break;

            }



            if (sourceIndex == -1 )
            {
                char source[50];
                printf("what is ur source city? :");
                scanf("%s",source);
                sourceIndex = findCityIndex(source);
                if (sourceIndex == -1)
                {
                    printf("City not found!\n");
                    break;
                }
            }
            t= clock();

            cost1 = primAlgo(sourceIndex);
            t = clock()-t;
            t2= clock();
            cost2=kAlgo();
            t2= clock() -t2;
            printf(" EXECUTION TIME COMPARISON:\n ");
            printf("execution time for prim is: %f seconds !\n",((double)t)/CLOCKS_PER_SEC);

            printf("execution time for kuruskal's is : %f seconds !\n",((double)t2)/CLOCKS_PER_SEC);

            if (t < t2)
                printf("Prim's algo is faster\n");
            else if (t2 < t)
                printf("Kruskal's algo is faster\n");
            else
                printf("Both have the same execution time\n");

            printf(" TOTAL COST COMPARISON:\n ");
            printf("\prim's algo: %f km \n",cost1);
            printf("\kurkal's algo: %f km \n",cost2);
            if (cost1==cost2)
                printf("both algorithim's give same cost! CORRECT!\n");


            break;
        default:
            printf("INVALID CHOICE ! \n");
            break;

        }
    }
    return 0;


}


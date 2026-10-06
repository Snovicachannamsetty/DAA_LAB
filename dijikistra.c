#include <stdio.h>

#define MAX 20
#define INF 1000000000


/* Find the unvisited vertex with the smallest distance */
int findMinDistance(int dist[], int visited[], int n)
{
    int min = INF;
    int minIndex = -1;

    for (int i = 0; i < n; i++)
    {
        if (visited[i] == 0 && dist[i] < min)
        {
            min = dist[i];
            minIndex = i;
        }
    }

    return minIndex;
}


/* Print the shortest path */
void printPath(int parent[], int destination)
{
    int path[MAX];
    int length = 0;
    int current = destination;

    /* Store the path backwards */
    while (current != -1)
    {
        path[length] = current;
        length++;

        current = parent[current];
    }

    /* Print the path in correct order */
    for (int i = length - 1; i >= 0; i--)
    {
        printf("%c", 'A' + path[i]);

        if (i != 0)
        {
            printf(" -> ");
        }
    }
}


/* Dijkstra's Algorithm */
void dijkstra(int graph[MAX][MAX], int n, int source)
{
    int dist[MAX];
    int visited[MAX];
    int parent[MAX];


    /* Initialize arrays */
    for (int i = 0; i < n; i++)
    {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }


    /* Distance from source to itself is 0 */
    dist[source] = 0;


    /* Main Dijkstra loop */
    for (int count = 0; count < n - 1; count++)
    {
        int u = findMinDistance(dist, visited, n);


        /* No more reachable vertices */
        if (u == -1)
        {
            break;
        }


        /* Mark current vertex as visited */
        visited[u] = 1;


        /* Relax all neighbours */
        for (int v = 0; v < n; v++)
        {
            if (visited[v] == 0 &&
                graph[u][v] > 0 &&
                dist[u] != INF &&
                dist[u] + graph[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph[u][v];

                parent[v] = u;
            }
        }
    }


    /* Print results */
    printf("\nShortest routes from vertex %c:\n",
           'A' + source);

    printf("--------------------------------------------------\n");

    printf("Destination\tDistance\tRoute\n");

    printf("--------------------------------------------------\n");


    for (int i = 0; i < n; i++)
    {
        printf("%c\t\t", 'A' + i);


        if (dist[i] == INF)
        {
            printf("INF\t\tNo route\n");
        }
        else
        {
            printf("%d\t\t", dist[i]);

            printPath(parent, i);

            printf("\n");
        }
    }
}


/* Main function */
int main(void)
{
    int graph[MAX][MAX];

    int n;
    int source;


    printf("SHORTEST ROUTE USING DIJKSTRA'S ALGORITHM\n");

    printf("=========================================\n");


    /* Read number of vertices */
    printf("Enter number of vertices (maximum %d): ", MAX);

    scanf("%d", &n);


    /* Validate number of vertices */
    if (n <= 0 || n > MAX)
    {
        printf("Invalid number of vertices.\n");

        return 1;
    }


    printf("\nVertices are named A, B, C, ...\n");

    printf("Enter the adjacency matrix.\n");

    printf("Use 0 when there is no direct edge.\n\n");


    /* Read adjacency matrix */
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &graph[i][j]);


            /* Negative weights are not allowed */
            if (graph[i][j] < 0)
            {
                printf(
                    "Negative edge weights are not allowed "
                    "in Dijkstra's algorithm.\n"
                );

                return 1;
            }
        }
    }


    /* Read source vertex */
    printf(
        "\nEnter source vertex number "
        "(A=0, B=1, C=2, ...): "
    );

    scanf("%d", &source);


    /* Validate source */
    if (source < 0 || source >= n)
    {
        printf("Invalid source vertex.\n");

        return 1;
    }


    /* Run Dijkstra */
    dijkstra(graph, n, source);


    return 0;
}

#include <stdio.h> 
#include <limits.h> 
 
#define MAX 10 
 
int n; 
int cost[MAX][MAX]; 
int visited[MAX]; 
int path[MAX + 1]; 
int bestPath[MAX + 1]; 
int minCost = INT_MAX; 
 
void tsp(int currentCity, int count, int currentCost) 
{ 
    int nextCity, i; 
 
    /* If all cities are visited, return to city 0 */ 
    if (count == n) 
    { 
        if (cost[currentCity][0] != 0) 
        { 
            int totalCost = currentCost + cost[currentCity][0]; 
 
            if (totalCost < minCost) 
            { 
                minCost = totalCost; 
 
                for (i = 0; i < n; i++) 
                    bestPath[i] = path[i]; 
 
                bestPath[n] = 0; 
            } 
        } 
        return; 
    } 
 
    /* Try every unvisited city as the next city */ 
    for (nextCity = 0; nextCity < n; nextCity++) 
    { 
        if (visited[nextCity] == 0 && 
            cost[currentCity][nextCity] != 0) 
        { 
            /* Choose */ 
            visited[nextCity] = 1; 
            path[count] = nextCity; 
 
            /* Explore */ 
            tsp(nextCity, 
                count + 1, 
                currentCost + cost[currentCity][nextCity]); 
 
            /* Undo the choice: backtracking */ 
            visited[nextCity] = 0; 
        } 
    } 
} 
 
int main() 
{ 
    int i, j; 
     printf("Enter number of cities: "); 
    scanf("%d", &n); 
 
    if (n < 2 || n > MAX) 
    { 
        printf("Number of cities must be between 2 and %d.\n", MAX); 
        return 0; 
    } 
 
    printf("Enter the cost matrix:\n"); 
 
    for (i = 0; i < n; i++) 
    { 
        for (j = 0; j < n; j++) 
        { 
            scanf("%d", &cost[i][j]); 
        } 
    } 
 
    /* Start from city 0 */ 
    visited[0] = 1; 
    path[0] = 0; 
 
    tsp(0, 1, 0); 
 
    if (minCost == INT_MAX) 
    { 
        printf("No complete tour is possible.\n"); 
    } 
    else 
    { 
        printf("\nMinimum travelling cost = %d\n", minCost); 
        printf("Best route: "); 
 
        for (i = 0; i <= n; i++) 
        { 
            printf("%d", bestPath[i]); 
 
            if (i < n) 
                printf(" -> "); 
        } 
 
        printf("\n"); 
    } 
    return 0;
}

#include <stdio.h>
#include <stdlib.h>

/* =========================================================
   1. FRACTIONAL KNAPSACK (GREEDY)
   ========================================================= */

typedef struct {
    int weight;
    int value;
    double ratio;
    int index;
} Item;

int compareItems(const void *a, const void *b)
{
    Item *x = (Item *)a;
    Item *y = (Item *)b;

    if (x->ratio < y->ratio)
        return 1;
    else if (x->ratio > y->ratio)
        return -1;
    return 0;
}

void fractionalKnapsack(int w[], int v[], int n, int W)
{
    Item items[n];

    for (int i = 0; i < n; i++) {
        items[i].weight = w[i];
        items[i].value = v[i];
        items[i].ratio = (double)v[i] / w[i];
        items[i].index = i + 1;
    }

    qsort(items, n, sizeof(Item), compareItems);

    int remaining = W;
    double total = 0.0;

    printf("\n--- Fractional Knapsack ---\n");

    for (int i = 0; i < n; i++) {

        if (remaining <= 0)
            break;

        if (items[i].weight <= remaining) {

            total += items[i].value;
            remaining -= items[i].weight;

            printf("Item %d: 100%% taken\n", items[i].index);
        }
        else {

            double fraction =
                (double)remaining / items[i].weight;

            total += fraction * items[i].value;

            printf("Item %d: %.2f%% taken\n",
                   items[i].index, fraction * 100);

            remaining = 0;
        }
    }

    printf("Maximum value = %.2f\n", total);
}


/* =========================================================
   2. 0/1 KNAPSACK - FULL TABLE + ITEM RECOVERY
   ========================================================= */

int knapsackTable(int w[], int v[], int n, int W)
{
    int K[n + 1][W + 1];

    for (int i = 0; i <= n; i++)
        K[i][0] = 0;

    for (int c = 0; c <= W; c++)
        K[0][c] = 0;

    for (int i = 1; i <= n; i++) {

        for (int c = 1; c <= W; c++) {

            K[i][c] = K[i - 1][c];

            if (w[i - 1] <= c) {

                int take =
                    v[i - 1] +
                    K[i - 1][c - w[i - 1]];

                if (take > K[i][c])
                    K[i][c] = take;
            }
        }
    }

    printf("\n--- 0/1 Knapsack Full Table ---\n");

    for (int i = 0; i <= n; i++) {
        for (int c = 0; c <= W; c++)
            printf("%4d", K[i][c]);

        printf("\n");
    }

    /* Recover selected items */

    int c = W;

    printf("Selected items: ");

    for (int i = n; i >= 1; i--) {

        if (K[i][c] != K[i - 1][c]) {

            printf("%d ", i);

            c = c - w[i - 1];
        }
    }

    printf("\n");

    return K[n][W];
}


/* =========================================================
   3. 0/1 KNAPSACK - O(W) SPACE
   ========================================================= */

int knapsackFast(int w[], int v[], int n, int W)
{
    int dp[W + 1];

    for (int c = 0; c <= W; c++)
        dp[c] = 0;

    for (int i = 0; i < n; i++) {

        /*
           DECREASING capacity:
           Each item can be used only once.
        */

        for (int c = W; c >= w[i]; c--) {

            int value =
                dp[c - w[i]] + v[i];

            if (value > dp[c])
                dp[c] = value;
        }
    }

    return dp[W];
}


/* =========================================================
   4. UNBOUNDED KNAPSACK
   ========================================================= */

int knapsackUnbounded(int w[], int v[], int n, int W)
{
    int dp[W + 1];

    for (int c = 0; c <= W; c++)
        dp[c] = 0;

    /*
       INCREASING capacity:
       Items can be used multiple times.
    */

    for (int c = 1; c <= W; c++) {

        for (int i = 0; i < n; i++) {

            if (w[i] <= c) {

                int value =
                    dp[c - w[i]] + v[i];

                if (value > dp[c])
                    dp[c] = value;
            }
        }
    }

    return dp[W];
}


/* =========================================================
   5. BOUNDED KNAPSACK
      Binary Splitting
   ========================================================= */

int knapsackBounded(int w[], int v[], int count[],
                    int n, int W)
{
    int dp[W + 1];

    for (int c = 0; c <= W; c++)
        dp[c] = 0;

    /*
       Convert each item count into bundles:
       1, 2, 4, 8, ...
    */

    for (int i = 0; i < n; i++) {

        int remaining = count[i];
        int k = 1;

        while (k <= remaining) {

            int bundleWeight = k * w[i];
            int bundleValue = k * v[i];

            /*
               0/1 update because each bundle
               can be selected only once.
            */

            for (int c = W; c >= bundleWeight; c--) {

                int value =
                    dp[c - bundleWeight]
                    + bundleValue;

                if (value > dp[c])
                    dp[c] = value;
            }

            remaining -= k;
            k *= 2;
        }

        /*
           Handle remaining items.
        */

        if (remaining > 0) {

            int bundleWeight = remaining * w[i];
            int bundleValue = remaining * v[i];

            for (int c = W; c >= bundleWeight; c--) {

                int value =
                    dp[c - bundleWeight]
                    + bundleValue;

                if (value > dp[c])
                    dp[c] = value;
            }
        }
    }

    return dp[W];
}


/* =========================================================
   6. SUBSET SUM
   ========================================================= */

int subsetSum(int nums[], int n, int target)
{
    int reach[target + 1];

    for (int i = 0; i <= target; i++)
        reach[i] = 0;

    reach[0] = 1;

    for (int i = 0; i < n; i++) {

        /*
           Go backwards so each number
           is used only once.
        */

        for (int t = target;
             t >= nums[i];
             t--) {

            if (reach[t - nums[i]])
                reach[t] = 1;
        }
    }

    return reach[target];
}


/* =========================================================
   7. COUNT SUBSETS
   ========================================================= */

int countSubsets(int nums[], int n, int target)
{
    int ways[target + 1];

    for (int i = 0; i <= target; i++)
        ways[i] = 0;

    ways[0] = 1;

    for (int i = 0; i < n; i++) {

        /*
           Backward loop = each item used once.
        */

        for (int t = target;
             t >= nums[i];
             t--) {

            ways[t] =
                ways[t] + ways[t - nums[i]];
        }
    }

    return ways[target];
}


/* =========================================================
   MAIN FUNCTION
   ========================================================= */

int main()
{
    /*
       Example 1 / 1b
       weights = {10, 20, 30}
       values  = {60, 100, 120}
       capacity = 50
    */

    int A_w[] = {10, 20, 30};
    int A_v[] = {60, 100, 120};

    int nA = 3;
    int WA = 50;

    fractionalKnapsack(A_w, A_v, nA, WA);

    int result1 =
        knapsackTable(A_w, A_v, nA, WA);

    printf("0/1 Knapsack maximum value = %d\n",
           result1);


    /*
       Example 2
       weights = {2, 3, 4, 5}
       values  = {3, 4, 5, 6}
       capacity = 8
    */

    int B_w[] = {2, 3, 4, 5};
    int B_v[] = {3, 4, 5, 6};

    int nB = 4;
    int WB = 8;

    printf("\n--- Example 2 ---\n");

    printf("0/1 Knapsack O(W) = %d\n",
           knapsackFast(B_w, B_v, nB, WB));


    /*
       Example 3
       weights = {5, 4, 6, 3, 2}
       values  = {60, 40, 65, 50, 15}
       capacity = 8
    */

    int C_w[] = {5, 4, 6, 3, 2};
    int C_v[] = {60, 40, 65, 50, 15};

    int nC = 5;
    int WC = 8;

    printf("\n--- Example 3 ---\n");

    printf("0/1 Knapsack = %d\n",
           knapsackTable(C_w, C_v, nC, WC));


    /*
       Example 4
       Unbounded Knapsack
    */

    int D_w[] = {2, 3, 4};
    int D_v[] = {3, 4, 5};

    int nD = 3;
    int WD = 8;

    printf("\n--- Example 4 ---\n");

    printf("Unbounded Knapsack = %d\n",
           knapsackUnbounded(D_w, D_v, nD, WD));


    /*
       Example 5
       Bounded Knapsack

       weights = {2, 3, 5, 8}
       values  = {11, 15, 21, 30}
       counts  = {3, 2, 2, 1}
    */

    int E_w[] = {2, 3, 5, 8};
    int E_v[] = {11, 15, 21, 30};
    int E_count[] = {3, 2, 2, 1};

    int nE = 4;
    int WE = 10;

    printf("\n--- Example 5 ---\n");

    printf("Bounded Knapsack = %d\n",
           knapsackBounded(E_w, E_v,
                           E_count, nE, WE));


    /*
       Example 6
       Subset Sum and Counting

       nums = {1, 2, 3, 4, 5}
       target = 8
    */

    int nums[] = {1, 2, 3, 4, 5};
    int nNums = 5;
    int target = 8;

    printf("\n--- Example 6 ---\n");

    if (subsetSum(nums, nNums, target))
        printf("Subset Sum: YES\n");
    else
        printf("Subset Sum: NO\n");

    printf("Number of subsets = %d\n",
           countSubsets(nums, nNums, target));


    return 0;
}

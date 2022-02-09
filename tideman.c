#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool is_cycle(int, int);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        //printf("\n");
    }
    add_pairs();
    sort_pairs();
    lock_pairs();
    printf("\n");
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = 0; j < pair_count; j++)
        {
            if (locked[i][j] == true)
            {
                printf("%i ", 1);
            }
            else
            {
                printf("%i ", 0);
            }
        }
        printf("\n");
    }
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // add preference [i][j] by checking the ranking of candidate
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // if candidate i has more preference than j, add pair (i, j)
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            // if candidate j has more preference than i, add pair (j, i)
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
            // if both candidates have equal preference, skip
            else
            {
                continue;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int swap;
    do
    {
        swap = 0;
        // loop over pairs, comparing two subsequent pairs at a time
        for (int i = 0, j = i + 1; (i < pair_count) && (j < pair_count); i++, j++)
        {
            // if the preceeding pair has a winner with less preference, swap
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[j].winner][pairs[j].loser])
            {
                pair temp = pairs[i];
                pairs[i] = pairs[j];
                pairs[j] = temp;
                swap++;
            }
        }
    }
    while (swap != 0);
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        // if pair doesn't generate a cycle, lock pair
        //if (is_cycle(pairs[i].winner, pairs[i].loser) == false)
        if (!is_cycle(pairs[i].loser, pairs[i].winner))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    bool win;
    // Check if a candidate is locked over
    for (int i = 0; i < candidate_count; i++)
    {
        win = true;
        for (int j = 0; j < candidate_count; j++)
        {
            // If candidate is locked over, candidate can't be the winner
            if (locked[j][i] == true)
            {
                win = false;
            }
        }
        // If candidate is not locked over, candidate is the winner
        if (win == true)
        {
            printf("%s\n", candidates[i]);
        }
    }


    // printf("%s\n", winner);
    return;
}

bool is_cycle(int current, int target)
{
    if (current == target)
    {
        return true;
    }
    for (int i = 0; i < pair_count; i++)
    {
        // if current candidate is locked against another candidate
        if (locked[current][pairs[i].loser])
        {
            // if "another" candidate and target forms a cycle
            if (is_cycle(pairs[i].loser, target))
            {
                return true;
            }
        }
    }
    return false;
}

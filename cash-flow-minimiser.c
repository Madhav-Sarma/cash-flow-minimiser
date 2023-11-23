#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_BANKS 100

// Node structure for linked list
typedef struct Node {
    int creditorIndex;
    int amount;
    int typeIndex;
    struct Node* next;
} Node;

// Bank structure
typedef struct Bank {
    char name[MAX_NAME_LENGTH];
    int numTypes;
    char types[MAX_BANKS][MAX_NAME_LENGTH];
    Node* head;
} Bank;

// Function to create a new node
Node* createNode(int creditorIndex, int amount, int typeIndex) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->creditorIndex = creditorIndex;
    newNode->amount = amount;
    newNode->typeIndex = typeIndex;
    newNode->next = NULL;
    return newNode;
}

// Function to insert a node at the end of the linked list
void insertNode(Node** head, int creditorIndex, int amount, int typeIndex) {
    Node* newNode = createNode(creditorIndex, amount, typeIndex);
    if (*head == NULL) {
        *head = newNode;
    } else {
        Node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Function to free the memory allocated for the linked list
void freeLinkedList(Node* head) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

// Function to minimize cash flow using linked list representation of the graph
void minimizeCashFlow(int numBanks, Bank inputList[], int index_of[], int numTransactions, int graph[][MAX_BANKS][2], int maxNumTypes) {
    int i, j, k;

    // Create an array of net amounts for each bank
    int netAmounts[MAX_BANKS] = {0};

    // Calculate net amounts for each bank
    for (i = 0; i < numBanks; i++) {
        for (j = 0; j < numBanks; j++) {
            netAmounts[i] += graph[j][i][0];  // Incoming amount
            netAmounts[i] -= graph[i][j][0];  // Outgoing amount
        }
    }

    // Find the bank with the maximum net amount
    int maxIndex = -1;
    int maxAmount = 0;
    for (i = 0; i < numBanks; i++) {
        if (netAmounts[i] > maxAmount) {
            maxIndex = i;
            maxAmount = netAmounts[i];
        }
    }

    // Check if all banks have zero net amount
    int allZero = 1;
    for (i = 0; i < numBanks; i++) {
        if (netAmounts[i] != 0) {
            allZero = 0;
            break;
        }
    }

    // If all banks have zero net amount, no transactions are needed
    if (allZero) {
        printf("No transactions needed.\n");
        return;
    }

    // Create a linked list for each bank to store transactions
    Node* transactionList[MAX_BANKS] = {NULL};

    // Iterate until the maximum net amount bank is settled
    while (maxIndex != -1) {
        // Find the bank with the minimum net amount
        int minIndex = -1;
        int minAmount = maxAmount;
        for (i = 0; i < numBanks; i++) {
            if (netAmounts[i] < minAmount) {
                minIndex = i;
                minAmount = netAmounts[i];
            }
        }

        // Find the minimum of the absolute values of the net amounts
        int minAbsoluteAmount = (minAmount < 0) ? -minAmount : minAmount;

        // Determine the transaction amount
        int amount = (minAbsoluteAmount < maxAmount) ? minAbsoluteAmount : maxAmount;

        // Add the transaction to the linked list of each bank involved
        insertNode(&transactionList[minIndex], maxIndex, amount, inputList[maxIndex].numTypes);

        // Update the net amounts
        netAmounts[minIndex] += amount;
        netAmounts[maxIndex] -= amount;

        // Update the maximum net amount bank
        if (netAmounts[maxIndex] == 0) {
            maxIndex = -1;
            maxAmount = 0;
            for (i = 0; i < numBanks; i++) {
                if (netAmounts[i] > maxAmount) {
                    maxIndex = i;
                    maxAmount = netAmounts[i];
                }
            }
        }
    }

    // Print the transactions
    for (i = 0; i < numBanks; i++) {
        Node* temp = transactionList[i];
        while (temp != NULL) {
            printf("%s pays %s $%d\n", inputList[i].name, inputList[temp->creditorIndex].name, temp->amount);
            temp = temp->next;
        }
    }

    // Free the memory allocated for the linked lists
    for (i = 0; i < numBanks; i++) {
        freeLinkedList(transactionList[i]);
    }
}

int main() {
    int numBanks, numTransactions, i, j, k;

    // Read the number of banks
    printf("Enter the number of banks: ");
    scanf("%d", &numBanks);

    // Read the bank details
    Bank inputList[MAX_BANKS];
    for (i = 0; i < numBanks; i++) {
        printf("Enter the name of bank %d: ", i + 1);
        scanf("%s", inputList[i].name);
        printf("Enter the number of payment types for bank %d: ", i + 1);
        scanf("%d", &inputList[i].numTypes);
        printf("Enter the payment types for bank %d:\n", i + 1);
        for (j = 0; j < inputList[i].numTypes; j++) {
            printf("Payment type %d: ", j + 1);
            scanf("%s", inputList[i].types[j]);
        }
    }

    // Read the number of transactions
    printf("Enter the number of transactions: ");
    scanf("%d", &numTransactions);

    // Initialize the graph
    int graph[MAX_BANKS][MAX_BANKS][2] = {0};
    int index_of[MAX_BANKS] = {0};
    int maxNumTypes = 0;

    // Read the transactions and build the graph
    printf("Enter the transactions:\n");
    for (i = 0; i < numTransactions; i++) {
        char debtorName[MAX_NAME_LENGTH];
        char creditorName[MAX_NAME_LENGTH];
        int amount, debtorIndex, creditorIndex, typeIndex;

        // Read the transaction details
        printf("Transaction %d:\n", i + 1);
        printf("Debtor name: ");
        scanf("%s", debtorName);
        printf("Creditor name: ");
        scanf("%s", creditorName);
        printf("Amount: ");
        scanf("%d", &amount);

        // Find the indices of the debtor and creditor banks
        for (j = 0; j < numBanks; j++) {
            if (strcmp(debtorName, inputList[j].name) == 0) {
                debtorIndex = j;
            }
            if (strcmp(creditorName, inputList[j].name) == 0) {
                creditorIndex = j;
            }
        }

        // Find the index of the payment type
        for (j = 0; j < inputList[debtorIndex].numTypes; j++) {
            if (strcmp(inputList[debtorIndex].types[j], inputList[creditorIndex].types[j]) == 0) {
                typeIndex = j;
            }
        }

        // Update the graph with the transaction
        graph[debtorIndex][creditorIndex][0] += amount;
        graph[debtorIndex][creditorIndex][1] = typeIndex;

        // Update the index of the debtor bank
        index_of[debtorIndex] = 1;

        // Update the maximum number of types
        if (inputList[debtorIndex].numTypes > maxNumTypes) {
            maxNumTypes = inputList[debtorIndex].numTypes;
        }
    }

    // Perform the cash flow minimization
    minimizeCashFlow(numBanks, inputList, index_of, numTransactions, graph, maxNumTypes);

    return 0;
}
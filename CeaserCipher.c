#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define SHIFT 7 // shift number for encryption
#define PERPROCESS 16

// PROTOTYPES
char *createCharArray(int size);
char *caesar_cipher(char *message, int size, int shift);

main(int argc, char **argv)
{
    int rank, size;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("\n--------------------%d/%d--------------------", rank, size);

    char *message = NULL;
    char *local_message = NULL;
    char *encrypted_message = NULL;

    // master
    if (rank == 0)
    {
        // create message in master node
        message = "Roses are red, "
                  "Violets are blue, "
                  "MPI is beutiful, "
                  "How about you?";
        printf("\n\nmessage: |%s|", message);
        printf("\n\nmessage length : %d", PERPROCESS * size);

        // allocate encrypted message for master node
        encrypted_message = (char *)malloc(size * PERPROCESS * sizeof(char));
    }

    // allocate local_message for all nodes
    local_message = createCharArray(PERPROCESS);

    // Scatter data to all nodes from master node
    MPI_Scatter(message, PERPROCESS, MPI_CHAR, local_message, PERPROCESS, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Add null after the end of local message for well printing '\0'
    local_message[PERPROCESS] = NULL;

    printf("\n\nReceived local message: |%s|", local_message);

    // Encrypt local message
    local_message = caesar_cipher(local_message, PERPROCESS, SHIFT);

    printf("\n\nEncrypted local message: |%s|", local_message);

    // Gather all encrypted messages from all nodes to master node
    MPI_Gather(local_message, PERPROCESS, MPI_CHAR,
               encrypted_message, PERPROCESS, MPI_CHAR,
               0, MPI_COMM_WORLD);

    // master
    if (rank == 0)
    {
        // Add null after the end of encrypted message for well printing '\0'
        encrypted_message[PERPROCESS * size] = NULL;
        printf("\n\nEncrypted message: |%s|", encrypted_message);

        // free memory
        free(encrypted_message);
    }

    // free memory
    free(local_message);
    printf("\n-------------------------------------------");
    MPI_Finalize();
}

// Creates array dynamically
char *createCharArray(int size)
{
    char *tmpArray = (char *)malloc(size * sizeof(char));

    if (!tmpArray)
    {
        printf("\nError : Cannot allocate memory\n");
        exit(1);
    }

    return tmpArray;
}

// Caesar cipher encryption
char *caesar_cipher(char *message, int size, int shift)
{
    int i;                    // index
    char ch;                  // temp char
    int diff = 'z' - 'a' + 1; // or 'Z' - 'A' + 1, num of letters

    for (i = 0; i < size; i++)
    {
        ch = message[i];

        if (ch >= 'a' && ch <= 'z')
        {
            message[i] = (ch + shift - 'a') % diff + 'a';
        }
        else if (ch >= 'A' && ch <= 'Z')
        {
            message[i] = (ch + shift - 'A') % diff + 'A';
        }
    }

    return message;
}
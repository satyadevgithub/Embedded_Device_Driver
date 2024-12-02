#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "alpha_ioctls.h"
#define DEVICE_PATH "/dev/cdac_dev"


int main()
{
    int fd;
    char ch;
    int ret;

    // Open the device file
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Failed to open the device");
        return -1;
    }

    // Write one character to the device at a time
    while (1) {
        printf("Enter a character (or q to quit): ");
        scanf(" %c", &ch);  // Read one character from user

        if (ch == 'q') {
            break;  // Quit the loop if 'q' is entered
        }

        // Write the character to the device
        ret = write(fd, &ch, sizeof(ch));
        if (ret == -1) {
            perror("Failed to write to the device");
            close(fd);
            return -1;
        }

        printf("Character '%c' written to the device\n", ch);
    }

    // Optionally, show the current lists
    printf("Displaying the alphabet list:\n");
    ret = ioctl(fd, SHOW_LIST_ALPHA, NULL);
    if (ret == -1) {
        perror("Failed to show alphabet list");
    }

    printf("Displaying the vowel list:\n");
    ret = ioctl(fd, SHOW_LIST_VOWEL, NULL);
    if (ret == -1) {
        perror("Failed to show vowel list");
    }

    printf("Displaying the consonant list:\n");
    ret = ioctl(fd, SHOW_LIST_CONSO, NULL);
    if (ret == -1) {
        perror("Failed to show consonant list");
    }

    // Close the device
    close(fd);
    return 0;
}


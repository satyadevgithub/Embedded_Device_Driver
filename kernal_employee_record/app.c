#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include "alpha_ioctls.h" 

#define DEVICE_FILE "/dev/cdac_dev"

struct my_node {
    char name[10];
    unsigned long id;
    int sex;    // 1 = Male, 0 = Female
    int status; // 1 = Permanent, 0 = Contractor
    char padding[2];
};

void display_menu() {
    printf("===== Employee Management =====\n");
    printf("1. Add Employee\n");
    printf("2. Display Employee List\n");
    printf("3. Delete All Employees\n");
    printf("4. Exit\n");
    printf("================================\n");
    printf("Choose an option: ");
}

void add_employee(int fd) {
    srand((unsigned int)time(NULL));
    int num_records;

    printf("Enter the number of employee records to generate: ");
    scanf("%d", &num_records);

    for (int i = 0; i < num_records; i++) {
        struct my_node new_employee;

        // Generate random name
        int name_length = rand() % 7 + 3;
        for (int j = 0; j < name_length - 1; j++) {
            new_employee.name[j] = 'a' + rand() % 26;
        }
        new_employee.name[name_length - 1] = '\0';

        new_employee.id = rand() % 1000 + 1;
        new_employee.sex = rand() % 2;    
        new_employee.status = rand() % 2; 

        if (write(fd, &new_employee, sizeof(new_employee)) < 0) {
            perror("Failed to write employee data");
        } else {
            printf("Employee ID %lu added successfully.\n", new_employee.id);
        }
    }
}


    


void display_employees(int fd) {
    if (ioctl(fd, SHOW_LIST, 0) < 0) {
        perror("Failed to display employee list");
    } else {
        printf("Employee list displayed in kernel log.\n");
    }
}

void delete_all_employees(int fd) {
    if (ioctl(fd, DEL_LIST, 0) < 0) {
        perror("Failed to delete employee list");
    } else {
        printf("Employee list deleted successfully.\n");
    }
}

int main() {
    int fd;
    int choice;

    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device file");
        return EXIT_FAILURE;
    }

    while (1) {
        display_menu();
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            add_employee(fd);
            break;
        case 2:
            display_employees(fd);
            break;
        case 3:
            delete_all_employees(fd);
            break;
        case 4:
            close(fd);
            printf("Exiting application.\n");
            return EXIT_SUCCESS;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }

    close(fd);
    return EXIT_SUCCESS;
}


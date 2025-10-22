#include <stdio.h>

int main() {
    int number;

    // Ask the user to enter a number
    printf("Enter a number: ");
    scanf("%d", &number);

    // Check if the number is even or odd
    if (number % 2 == 0) {
        printf("%d is Even\n", number);
    } else {
        printf("%d is Odd\n", number);
    }

    return 0;
}

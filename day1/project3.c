#include <stdio.h>

int main() {
    float num1, num2;
    float sum, difference, product, quotient;

    // Ask user for input
    printf("Enter first number: ");
    scanf("%f", &num1);
    printf("Enter second number: ");
    scanf("%f", &num2);

    // Calculate sum, difference, product, and quotient
    sum = num1 + num2;
    difference = num1 - num2;
    product = num1 * num2;

    // Check for division by zero
    if (num2 != 0) {
        quotient = num1 / num2;
        printf("Sum = %.2f\n", sum);
        printf("Difference = %.2f\n", difference);
        printf("Product = %.2f\n", product);
        printf("Quotient = %.2f\n", quotient);
    } else {
        printf("Sum = %.2f\n", sum);
        printf("Difference = %.2f\n", difference);
        printf("Product = %.2f\n", product);
        printf("Error! Division by zero is not allowed.\n");
    }

    return 0;
}

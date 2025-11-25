#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_NAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 20
#define MAX_DESCRIPTION_LENGTH 100
#define ACCOUNTS_DB "accounts.db"
#define TRANSACTIONS_DB "transactions.db"

// Structure to hold account information
typedef struct {
    char fullName[MAX_NAME_LENGTH];
    int accountNumber;
    char password[MAX_PASSWORD_LENGTH];
    float balance;
} BankAccount;

// Structure to hold transaction information
typedef struct {
    int transactionId;
    int accountNumber;
    char type[20];
    float amount;
    float balanceAfter;
    char timestamp[20];
    char description[MAX_DESCRIPTION_LENGTH];
} Transaction;

// Global variables for current session
BankAccount currentUser;
int isLoggedIn = 0;

// Database function prototypes
int initializeDatabase();
int createAccount(const BankAccount *account);
int findAccountByNumber(int accountNumber, BankAccount *result);
int updateAccountBalance(int accountNumber, float newBalance);
int updateAccountPassword(int accountNumber, const char *newPassword);
int recordTransaction(const Transaction *transaction);
int getAccountTransactions(int accountNumber, Transaction **transactions, int *count);
void freeTransactions(Transaction *transactions);
int validatePassword(const char *password);
void clearInputBuffer();
void printHeader(const char *title);
void getCurrentTimestamp(char* buffer);

// Business logic function prototypes
void mainMenu();
void userMenu();
void registerAccount();
int login();
void depositFunds();
void withdrawFunds();
void transferFunds();
void changePassword();
void displayAccountDetails();
void viewTransactionHistory();

int main() {
    if (!initializeDatabase()) {
        printf("❌ Failed to initialize database system!\n");
        return 1;
    }
    
    printf("============================================\n");
    printf("      WELCOME TO ONLINE BANKING SYSTEM\n");
    printf("        Student: 2025013535\n"); 
    printf("          Currency: Zambian Kwacha\n");
    printf("============================================\n");
    
    mainMenu();
    return 0;
}

int initializeDatabase() {
    FILE *file;
    
    // Initialize accounts database
    file = fopen(ACCOUNTS_DB, "ab");
    if (file == NULL) return 0;
    fclose(file);
    
    // Initialize transactions database
    file = fopen(TRANSACTIONS_DB, "ab");
    if (file == NULL) return 0;
    fclose(file);
    
    return 1;
}

// Database Functions
int createAccount(const BankAccount *account) {
    FILE *file = fopen(ACCOUNTS_DB, "ab");
    if (file == NULL) return 0;
    
    int result = fwrite(account, sizeof(BankAccount), 1, file);
    fclose(file);
    return result == 1;
}

int findAccountByNumber(int accountNumber, BankAccount *result) {
    FILE *file = fopen(ACCOUNTS_DB, "rb");
    if (file == NULL) return 0;
    
    BankAccount account;
    int found = 0;
    
    while (fread(&account, sizeof(BankAccount), 1, file)) {
        if (account.accountNumber == accountNumber) {
            *result = account;
            found = 1;
            break;
        }
    }
    
    fclose(file);
    return found;
}

int updateAccountBalance(int accountNumber, float newBalance) {
    FILE *file = fopen(ACCOUNTS_DB, "rb+");
    if (file == NULL) return 0;
    
    BankAccount account;
    long position;
    int found = 0;
    
    while ((position = ftell(file)) != -1 && 
           fread(&account, sizeof(BankAccount), 1, file)) {
        if (account.accountNumber == accountNumber) {
            account.balance = newBalance;
            fseek(file, position, SEEK_SET);
            fwrite(&account, sizeof(BankAccount), 1, file);
            found = 1;
            break;
        }
    }
    
    fclose(file);
    return found;
}

int updateAccountPassword(int accountNumber, const char *newPassword) {
    FILE *file = fopen(ACCOUNTS_DB, "rb+");
    if (file == NULL) return 0;
    
    BankAccount account;
    long position;
    int found = 0;
    
    while ((position = ftell(file)) != -1 && 
           fread(&account, sizeof(BankAccount), 1, file)) {
        if (account.accountNumber == accountNumber) {
            strcpy(account.password, newPassword);
            fseek(file, position, SEEK_SET);
            fwrite(&account, sizeof(BankAccount), 1, file);
            found = 1;
            break;
        }
    }
    
    fclose(file);
    return found;
}

int recordTransaction(const Transaction *transaction) {
    FILE *file = fopen(TRANSACTIONS_DB, "ab");
    if (file == NULL) return 0;
    
    int result = fwrite(transaction, sizeof(Transaction), 1, file);
    fclose(file);
    return result == 1;
}

int getAccountTransactions(int accountNumber, Transaction **transactions, int *count) {
    FILE *file = fopen(TRANSACTIONS_DB, "rb");
    if (file == NULL) return 0;
    
    Transaction transaction;
    int capacity = 10;
    int size = 0;
    
    // CORRECTED: Properly allocate memory for Transaction array
    *transactions = (Transaction *)malloc(capacity * sizeof(Transaction));
    if (*transactions == NULL) {
        fclose(file);
        return 0;
    }
    
    while (fread(&transaction, sizeof(Transaction), 1, file)) {
        if (transaction.accountNumber == accountNumber) {
            if (size >= capacity) {
                capacity *= 2;
                // CORRECTED: Proper reallocation with correct type
                Transaction *temp = (Transaction *)realloc(*transactions, capacity * sizeof(Transaction));
                if (temp == NULL) {
                    free(*transactions);
                    fclose(file);
                    return 0;
                }
                *transactions = temp;
            }
            // CORRECTED: Direct assignment to the array
            (*transactions)[size] = transaction;
            size++;
        }
    }
    
    fclose(file);
    *count = size;
    return 1;
}

void freeTransactions(Transaction *transactions) {
    free(transactions);
}

// Business Logic Functions
void mainMenu() {
    int choice;
    
    do {
        printHeader("MAIN MENU");
        printf("1. Register New Account\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("============================================\n");
        printf("Enter your choice (1-3): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        
        switch(choice) {
            case 1:
                registerAccount();
                break;
            case 2:
                if (login()) {
                    userMenu();
                }
                break;
            case 3:
                printf("Thank you for using Online Banking System!\n");
                printf("Goodbye! 👋\n");
                break;
            default:
                printf("Invalid choice! Please select 1-3.\n");
        }
    } while (choice != 3);
}

void userMenu() {
    int choice;
    
    do {
        printHeader("USER DASHBOARD");
        printf("Welcome, %s!\n", currentUser.fullName);
        printf("============================================\n");
        printf("1. Deposit Funds\n");
        printf("2. Withdraw Funds\n");
        printf("3. Transfer Funds\n");
        printf("4. Change Password\n");
        printf("5. View Account Details\n");
        printf("6. View Transaction History\n");
        printf("7. Logout\n");
        printf("============================================\n");
        printf("Enter your choice (1-7): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        
        switch(choice) {
            case 1:
                depositFunds();
                break;
            case 2:
                withdrawFunds();
                break;
            case 3:
                transferFunds();
                break;
            case 4:
                changePassword();
                break;
            case 5:
                displayAccountDetails();
                break;
            case 6:
                viewTransactionHistory();
                break;
            case 7:
                isLoggedIn = 0;
                printf("Logged out successfully!\n");
                break;
            default:
                printf("Invalid choice! Please select 1-7.\n");
        }
    } while (choice != 7 && isLoggedIn);
}

void registerAccount() {
    printHeader("ACCOUNT REGISTRATION");
    
    BankAccount newAccount;
    char confirmPassword[MAX_PASSWORD_LENGTH];
    
    // Get full name
    clearInputBuffer();
    printf("Enter your full name: ");
    fgets(newAccount.fullName, MAX_NAME_LENGTH, stdin);
    newAccount.fullName[strcspn(newAccount.fullName, "\n")] = 0;
    
    // Validate name
    if (strlen(newAccount.fullName) < 2) {
        printf("Name must be at least 2 characters long.\n");
        return;
    }
    
    // Get account number from user
    printf("Create your account number (must be 5-10 digits): ");
    if (scanf("%d", &newAccount.accountNumber) != 1) {
        printf("❌ Invalid account number! Please enter numbers only.\n");
        clearInputBuffer();
        return;
    }
    
    // Validate account number length and uniqueness
    if (newAccount.accountNumber < 10000 || newAccount.accountNumber > 9999999999) {
        printf("❌ Account number must be between 5 and 10 digits!\n");
        return;
    }
    
    BankAccount existingAccount;
    if (findAccountByNumber(newAccount.accountNumber, &existingAccount)) {
        printf("❌ Account number already exists! Please choose a different number.\n");
        return;
    }
    
    // Get and validate password with confirmation
    printf("Enter password (min 6 characters): ");
    scanf("%s", newAccount.password);
    
    if (!validatePassword(newAccount.password)) {
        printf("Password must be at least 6 characters long.\n");
        return;
    }
    
    // Confirm password
    printf("Confirm your password: ");
    scanf("%s", confirmPassword);
    
    if (strcmp(newAccount.password, confirmPassword) != 0) {
        printf("❌ Passwords do not match! Registration failed.\n");
        return;
    }
    
    // Get initial deposit
    printf("Enter initial deposit amount (K): ");
    float initialDeposit;
    if (scanf("%f", &initialDeposit) != 1 || initialDeposit < 0) {
        printf("Invalid deposit amount!\n");
        clearInputBuffer();
        return;
    }
    
    newAccount.balance = initialDeposit;
    
    // Save to database
    if (!createAccount(&newAccount)) {
        printf("❌ Failed to create account! Please try again.\n");
        return;
    }
    
    // Record account creation transaction
    Transaction transaction;
    transaction.transactionId = 0;
    transaction.accountNumber = newAccount.accountNumber;
    strcpy(transaction.type, "ACCOUNT_CREATION");
    transaction.amount = initialDeposit;
    transaction.balanceAfter = initialDeposit;
    getCurrentTimestamp(transaction.timestamp);
    strcpy(transaction.description, "Account created with initial deposit");
    
    if (!recordTransaction(&transaction)) {
        printf("⚠  Account created but failed to record transaction.\n");
    }
    
    // Enhanced success message with congratulations
    printf("\n🎉 CONGRATULATIONS! 🎉\n");
    printf("============================================\n");
    printf("✅ ACCOUNT SUCCESSFULLY REGISTERED!\n");
    printf("============================================\n");
    printf("Account Holder: %s\n", newAccount.fullName);
    printf("Account Number: %d\n", newAccount.accountNumber);
    printf("Initial Balance: K%.2f\n", newAccount.balance);
    printf("============================================\n");
    printf("Thank you for choosing Online Banking System!\n");
    printf("Your account is now active and ready to use.\n");
    printf("You can login to access all banking services.\n");
    printf("============================================\n\n");
}

int login() {
    printHeader("ACCOUNT LOGIN");
    
    int accountNumber;
    char password[MAX_PASSWORD_LENGTH];
    
    printf("Enter your account number: ");
    if (scanf("%d", &accountNumber) != 1) {
        printf("Invalid account number!\n");
        clearInputBuffer();
        return 0;
    }
    
    printf("Enter your password: ");
    scanf("%s", password);
    
    // Find account in database
    BankAccount account;
    if (findAccountByNumber(accountNumber, &account)) {
        if (strcmp(account.password, password) == 0) {
            currentUser = account;
            isLoggedIn = 1;
            printf("✅ Login successful! Welcome back, %s!\n", account.fullName);
            return 1;
        }
    }
    
    printf("❌ Login failed! Invalid account number or password.\n");
    return 0;
}

void depositFunds() {
    printHeader("DEPOSIT FUNDS");
    
    float amount;
    printf("Enter amount to deposit (K): ");
    
    if (scanf("%f", &amount) != 1 || amount <= 0) {
        printf("❌ Invalid amount! Please enter a positive number.\n");
        clearInputBuffer();
        return;
    }
    
    float newBalance = currentUser.balance + amount;
    
    // Update account in database
    if (!updateAccountBalance(currentUser.accountNumber, newBalance)) {
        printf("❌ Failed to process deposit! Please try again.\n");
        return;
    }
    
    // Record deposit transaction
    Transaction transaction;
    transaction.transactionId = 0;
    transaction.accountNumber = currentUser.accountNumber;
    strcpy(transaction.type, "DEPOSIT");
    transaction.amount = amount;
    transaction.balanceAfter = newBalance;
    getCurrentTimestamp(transaction.timestamp);
    strcpy(transaction.description, "Cash deposit");
    
    if (!recordTransaction(&transaction)) {
        printf("⚠  Deposit processed but failed to record transaction.\n");
    }
    
    // Update current session
    currentUser.balance = newBalance;
    
    printf("✅ Deposit successful!\n");
    printf("New balance: K%.2f\n", currentUser.balance);
}

void withdrawFunds() {
    printHeader("WITHDRAW FUNDS");
    
    float amount;
    printf("Current balance: K%.2f\n", currentUser.balance);
    printf("Enter amount to withdraw (K): ");
    
    if (scanf("%f", &amount) != 1 || amount <= 0) {
        printf("❌ Invalid amount! Please enter a positive number.\n");
        clearInputBuffer();
        return;
    }
    
    if (amount > currentUser.balance) {
        printf("❌ Insufficient funds! Your balance is K%.2f\n", currentUser.balance);
        return;
    }
    
    float newBalance = currentUser.balance - amount;
    
    // Update account in database
    if (!updateAccountBalance(currentUser.accountNumber, newBalance)) {
        printf("❌ Failed to process withdrawal! Please try again.\n");
        return;
    }
    
    // Record withdrawal transaction
    Transaction transaction;
    transaction.transactionId = 0;
    transaction.accountNumber = currentUser.accountNumber;
    strcpy(transaction.type, "WITHDRAWAL");
    transaction.amount = amount;
    transaction.balanceAfter = newBalance;
    getCurrentTimestamp(transaction.timestamp);
    strcpy(transaction.description, "Cash withdrawal");
    
    if (!recordTransaction(&transaction)) {
        printf("⚠  Withdrawal processed but failed to record transaction.\n");
    }
    
    // Update current session
    currentUser.balance = newBalance;
    
    printf("✅ Withdrawal successful!\n");
    printf("New balance: K%.2f\n", currentUser.balance);
}

void transferFunds() {
    printHeader("FUND TRANSFER");
    
    int targetAccountNumber;
    float amount;
    
    printf("Current balance: K%.2f\n", currentUser.balance);
    printf("Enter recipient's account number: ");
    
    if (scanf("%d", &targetAccountNumber) != 1) {
        printf("❌ Invalid account number!\n");
        clearInputBuffer();
        return;
    }
    
    // Check if transferring to own account
    if (targetAccountNumber == currentUser.accountNumber) {
        printf("❌ Cannot transfer to your own account!\n");
        return;
    }
    
    // Find target account in database
    BankAccount targetAccount;
    if (!findAccountByNumber(targetAccountNumber, &targetAccount)) {
        printf("❌ Recipient account not found!\n");
        return;
    }
    
    printf("Recipient: %s\n", targetAccount.fullName);
    printf("Enter amount to transfer (K): ");
    
    if (scanf("%f", &amount) != 1 || amount <= 0) {
        printf("❌ Invalid amount!\n");
        clearInputBuffer();
        return;
    }
    
    if (amount > currentUser.balance) {
        printf("❌ Insufficient funds! Your balance is K%.2f\n", currentUser.balance);
        return;
    }
    
    // Calculate new balances
    float senderNewBalance = currentUser.balance - amount;
    float receiverNewBalance = targetAccount.balance + amount;
    
    // Update both accounts in database
    if (!updateAccountBalance(currentUser.accountNumber, senderNewBalance) ||
        !updateAccountBalance(targetAccountNumber, receiverNewBalance)) {
        printf("❌ Failed to process transfer! Please try again.\n");
        return;
    }
    
    // Record transactions for both accounts
    Transaction senderTransaction, receiverTransaction;
    
    // Sender transaction
    senderTransaction.transactionId = 0;
    senderTransaction.accountNumber = currentUser.accountNumber;
    strcpy(senderTransaction.type, "TRANSFER_SENT");
    senderTransaction.amount = amount;
    senderTransaction.balanceAfter = senderNewBalance;
    getCurrentTimestamp(senderTransaction.timestamp);
    snprintf(senderTransaction.description, MAX_DESCRIPTION_LENGTH, 
             "Transfer to account %d (%s)", targetAccountNumber, targetAccount.fullName);
    
    // Receiver transaction
    receiverTransaction.transactionId = 0;
    receiverTransaction.accountNumber = targetAccountNumber;
    strcpy(receiverTransaction.type, "TRANSFER_RECEIVED");
    receiverTransaction.amount = amount;
    receiverTransaction.balanceAfter = receiverNewBalance;
    getCurrentTimestamp(receiverTransaction.timestamp);
    snprintf(receiverTransaction.description, MAX_DESCRIPTION_LENGTH, 
             "Transfer from account %d (%s)", currentUser.accountNumber, currentUser.fullName);
    
    if (!recordTransaction(&senderTransaction) || !recordTransaction(&receiverTransaction)) {
        printf("⚠  Transfer processed but failed to record some transactions.\n");
    }
    
    // Update current session
    currentUser.balance = senderNewBalance;
    
    printf("✅ Transfer successful!\n");
    printf("Transferred: K%.2f to %s\n", amount, targetAccount.fullName);
    printf("Your new balance: K%.2f\n", currentUser.balance);
}

void changePassword() {
    printHeader("CHANGE PASSWORD");
    
    char currentPassword[MAX_PASSWORD_LENGTH];
    char newPassword[MAX_PASSWORD_LENGTH];
    char confirmPassword[MAX_PASSWORD_LENGTH];
    
    printf("Enter current password: ");
    scanf("%s", currentPassword);
    
    // Verify current password
    if (strcmp(currentUser.password, currentPassword) != 0) {
        printf("❌ Current password is incorrect!\n");
        return;
    }
    
    printf("Enter new password (min 6 characters): ");
    scanf("%s", newPassword);
    
    if (!validatePassword(newPassword)) {
        printf("❌ Password must be at least 6 characters long.\n");
        return;
    }
    
    printf("Confirm new password: ");
    scanf("%s", confirmPassword);
    
    if (strcmp(newPassword, confirmPassword) != 0) {
        printf("❌ Passwords do not match!\n");
        return;
    }
    
    // Update password in database
    if (!updateAccountPassword(currentUser.accountNumber, newPassword)) {
        printf("❌ Failed to change password! Please try again.\n");
        return;
    }
    
    // Record password change transaction
    Transaction transaction;
    transaction.transactionId = 0;
    transaction.accountNumber = currentUser.accountNumber;
    strcpy(transaction.type, "PASSWORD_CHANGE");
    transaction.amount = 0;
    transaction.balanceAfter = currentUser.balance;
    getCurrentTimestamp(transaction.timestamp);
    strcpy(transaction.description, "Password changed successfully");
    
    if (!recordTransaction(&transaction)) {
        printf("⚠  Password changed but failed to record transaction.\n");
    }
    
    // Update current session
    strcpy(currentUser.password, newPassword);
    
    printf("✅ Password changed successfully!\n");
}

void displayAccountDetails() {
    printHeader("ACCOUNT DETAILS");
    
    printf("Account Holder: %s\n", currentUser.fullName);
    printf("Account Number: %d\n", currentUser.accountNumber);
    printf("Current Balance: K%.2f\n", currentUser.balance);
    printf("Password: ********\n");
    printf("============================================\n");
}

void viewTransactionHistory() {
    printHeader("TRANSACTION HISTORY");
    
    Transaction *transactions = NULL;
    int count = 0;
    
    if (!getAccountTransactions(currentUser.accountNumber, &transactions, &count)) {
        printf("❌ Failed to load transaction history!\n");
        return;
    }
    
    printf("Account: %s (%d)\n", currentUser.fullName, currentUser.accountNumber);
    printf("Current Balance: K%.2f\n", currentUser.balance);
    printf("============================================\n");
    
    if (count == 0) {
        printf("No transactions found for this account.\n");
    } else {
        printf("Date       | Type            | Amount    | Balance   | Description\n");
        printf("-----------+-----------------+-----------+-----------+----------------\n");
        
        for (int i = 0; i < count; i++) {
            printf("%s | %-15s | K%8.2f | K%8.2f | %s\n",
                   transactions[i].timestamp,
                   transactions[i].type,
                   transactions[i].amount,
                   transactions[i].balanceAfter,
                   transactions[i].description);
        }
    }
    
    printf("============================================\n");
    
    if (transactions != NULL) {
        freeTransactions(transactions);
    }
}

// Utility functions
int validatePassword(const char *password) {
    return strlen(password) >= 6;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printHeader(const char *title) {
    printf("\n============================================\n");
    printf("          %s\n", title);
    printf("============================================\n");
}

void getCurrentTimestamp(char* buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 20, "%Y-%m-%d %H:%M", t);
}
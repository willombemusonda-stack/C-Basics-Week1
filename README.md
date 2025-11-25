# Online Banking (C) student number: 2025013535

Console-based online banking system written in C. It provides local account management and transaction recording using simple binary file storage.

## Features
- Register account with name, account number, and password
- Login and access a user dashboard
- Deposit, withdraw, and transfer funds between accounts
- Change password with confirmation
- View account details and transaction history
- Persistent storage in `accounts.db` and `transactions.db`

## Project Layout
- `online_banking.c`: Single translation unit containing all logic (menus, storage, utilities)
- `accounts.db`: Binary file storing `BankAccount` records
- `transactions.db`: Binary file storing `Transaction` records

Key entry points:
- `main()` initializes storage and shows the main menu
- `mainMenu()` handles registration and login
- `userMenu()` provides account operations after login

## Build

Windows (MinGW-w64 GCC):

```powershell
gcc -O2 -std=c11 -Wall -Wextra -o online_banking.exe online_banking.c
```

Windows (Clang):

```powershell
clang -O2 -std=c11 -Wall -Wextra -o online_banking.exe online_banking.c
```

Windows (MSVC `cl`):

```powershell
cl /Fe:online_banking.exe /std:c11 /W4 online_banking.c
```

No external dependencies are required beyond a C compiler and the C standard library.

## Run

```powershell
./online_banking.exe
```

On first run, the program creates `accounts.db` and `transactions.db` in the working directory.

Main flow:
1. Use "Register New Account" to create an account
2. Login with your account number and password
3. Use the dashboard to deposit, withdraw, transfer, and view history
4. Logout when finished

## Data & Persistence
- Files are created next to the executable and persist across runs
- Delete `accounts.db` and `transactions.db` to reset all data
- Records are appended in binary form; transaction history is filtered by account at read time

## Notes & Limitations
- Account number input is numeric-only and expected to be 5–10 digits
- Passwords are stored in plaintext in the local file (for learning only)
- Transaction IDs are not unique; history relies on append order and timestamps
- Currency shown is Zambian Kwacha (K) as displayed in the UI

## Troubleshooting
- If build fails, ensure a C toolchain is installed (MinGW-w64, Clang, or MSVC)
- If run fails to create files, check write permissions to the working directory
- To recover from corrupt data, remove `accounts.db` and `transactions.db` and re-register accounts

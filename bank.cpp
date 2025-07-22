#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <sstream>
#include<bits/stdc++.h>
using namespace std;

// Forward declarations
class Transaction;
class Account;
class Customer;
class Bank;

// Enum for transaction types
enum class TransactionType {
    DEPOSIT,
    WITHDRAWAL,
    TRANSFER,
    LOAN_PAYMENT,
    INTEREST_CREDIT
};

// Enum for account types
enum class AccountType {
    SAVINGS,
    CHECKING,
    LOAN,
    FIXED_DEPOSIT
};

// Base Exception class for custom exceptions
class BankException : public exception {
private:
    string message;
public:
    BankException(const string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Custom exceptions
class InsufficientFundsException : public BankException {
public:
    InsufficientFundsException() : BankException("Insufficient funds in account") {}
};

class AccountNotFoundException : public BankException {
public:
    AccountNotFoundException() : BankException("Account not found") {}
};

class InvalidAmountException : public BankException {
public:
    InvalidAmountException() : BankException("Invalid amount specified") {}
};

// Transaction class to record all transactions
class Transaction {
private:
    static int nextTransactionId;
    int transactionId;
    string fromAccountId;
    string toAccountId;
    double amount;
    TransactionType type;
    string timestamp;
    string description;

public:
    Transaction(const string& from, const string& to, double amt, 
                TransactionType t, const string& desc = "")
        : transactionId(++nextTransactionId), fromAccountId(from), 
          toAccountId(to), amount(amt), type(t), description(desc) {
        
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        timestamp = ss.str();
    }

    // Getters
    int getTransactionId() const { return transactionId; }
    string getFromAccountId() const { return fromAccountId; }
    string getToAccountId() const { return toAccountId; }
    double getAmount() const { return amount; }
    TransactionType getType() const { return type; }
    string getTimestamp() const { return timestamp; }
    string getDescription() const { return description; }

    string getTypeString() const {
        switch(type) {
            case TransactionType::DEPOSIT: return "DEPOSIT";
            case TransactionType::WITHDRAWAL: return "WITHDRAWAL";
            case TransactionType::TRANSFER: return "TRANSFER";
            case TransactionType::LOAN_PAYMENT: return "LOAN_PAYMENT";
            case TransactionType::INTEREST_CREDIT: return "INTEREST_CREDIT";
            default: return "UNKNOWN";
        }
    }

    void display() const {
        cout << "ID: " << transactionId 
             << " | Type: " << getTypeString()
             << " | Amount: $" << fixed << setprecision(2) << amount
             << " | Time: " << timestamp
             << " | From: " << fromAccountId
             << " | To: " << toAccountId
             << " | Desc: " << description << endl;
    }
};

// Initialize static member
int Transaction::nextTransactionId = 0;

// Abstract base class for all accounts
class Account {
protected:
    string accountId;
    string customerId;
    double balance;
    AccountType accountType;
    string creationDate;
    bool isActive;
    vector<shared_ptr<Transaction>> transactionHistory;

public:
    Account(const string& accId, const string& custId, double initialBalance, AccountType type)
        : accountId(accId), customerId(custId), balance(initialBalance), 
          accountType(type), isActive(true) {
        
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&time_t), "%Y-%m-%d");
        creationDate = ss.str();
    }

    virtual ~Account() = default;

    // Pure virtual functions
    virtual void deposit(double amount) = 0;
    virtual bool withdraw(double amount) = 0;
    virtual void calculateInterest() = 0;
    virtual string getAccountTypeString() const = 0;
    virtual void displayAccountInfo() const = 0;

    // Getters
    string getAccountId() const { return accountId; }
    string getCustomerId() const { return customerId; }
    double getBalance() const { return balance; }
    AccountType getAccountType() const { return accountType; }
    string getCreationDate() const { return creationDate; }
    bool getIsActive() const { return isActive; }

    // Common methods
    void addTransaction(shared_ptr<Transaction> transaction) {
        transactionHistory.push_back(transaction);
    }

    void displayTransactionHistory() const {
        cout << "\n=== Transaction History for Account: " << accountId << " ===" << endl;
        if (transactionHistory.empty()) {
            cout << "No transactions found." << endl;
            return;
        }
        
        for (const auto& transaction : transactionHistory) {
            transaction->display();
        }
    }

    void closeAccount() {
        isActive = false;
        cout << "Account " << accountId << " has been closed." << endl;
    }

protected:
    void setBalance(double newBalance) { balance = newBalance; }
};

// Savings Account class
class SavingsAccount : public Account {
private:
    double interestRate;
    double minimumBalance;

public:
    SavingsAccount(const string& accId, const string& custId, double initialBalance)
        : Account(accId, custId, initialBalance, AccountType::SAVINGS),
          interestRate(0.035), minimumBalance(100.0) {}

    void deposit(double amount) override {
        if (amount <= 0) {
            throw InvalidAmountException();
        }
        balance += amount;
        cout << "Deposited $" << fixed << setprecision(2) << amount 
             << " to Savings Account. New balance: $" << balance << endl;
    }

    bool withdraw(double amount) override {
        if (amount <= 0) {
            throw InvalidAmountException();
        }
        if (balance - amount < minimumBalance) {
            throw InsufficientFundsException();
        }
        balance -= amount;
        cout << "Withdrew $" << fixed << setprecision(2) << amount 
             << " from Savings Account. New balance: $" << balance << endl;
        return true;
    }

    void calculateInterest() override {
        double interest = balance * interestRate / 12; // Monthly interest
        balance += interest;
        cout << "Interest of $" << fixed << setprecision(2) << interest 
             << " credited to Savings Account. New balance: $" << balance << endl;
    }

    string getAccountTypeString() const override {
        return "SAVINGS";
    }

    void displayAccountInfo() const override {
        cout << "\n=== Savings Account Information ===" << endl;
        cout << "Account ID: " << accountId << endl;
        cout << "Customer ID: " << customerId << endl;
        cout << "Balance: $" << fixed << setprecision(2) << balance << endl;
        cout << "Interest Rate: " << fixed << setprecision(1) << interestRate * 100 << "%" << endl;
        cout << "Minimum Balance: $" << fixed << setprecision(2) << minimumBalance << endl;
        cout << "Creation Date: " << creationDate << endl;
        cout << "Status: " << (isActive ? "Active" : "Closed") << endl;
    }
};

// Checking Account class
class CheckingAccount : public Account {
private:
    double overdraftLimit;
    double overdraftFee;

public:
    CheckingAccount(const string& accId, const string& custId, double initialBalance)
        : Account(accId, custId, initialBalance, AccountType::CHECKING),
          overdraftLimit(500.0), overdraftFee(35.0) {}

    void deposit(double amount) override {
        if (amount <= 0) {
            throw InvalidAmountException();
        }
        balance += amount;
        cout << "Deposited $" << fixed << setprecision(2) << amount 
             << " to Checking Account. New balance: $" << balance << endl;
    }

    bool withdraw(double amount) override {
        if (amount <= 0) {
            throw InvalidAmountException();
        }
        if (balance - amount < -overdraftLimit) {
            throw InsufficientFundsException();
        }
        
        balance -= amount;
        if (balance < 0) {
            balance -= overdraftFee;
            cout << "Overdraft fee of $" << fixed << setprecision(2) << overdraftFee << " applied." << endl;
        }
        
        cout << "Withdrew $" << fixed << setprecision(2) << amount 
             << " from Checking Account. New balance: $" << balance << endl;
        return true;
    }

    void calculateInterest() override {
        // Checking accounts typically don't earn interest
        // But we can implement a small interest rate for positive balances
        if (balance > 0) {
            double interest = balance * 0.001 / 12; // 0.1% annual rate
            balance += interest;
            cout << "Interest of $" << fixed << setprecision(2) << interest 
                 << " credited to Checking Account. New balance: $" << balance << endl;
        }
    }

    string getAccountTypeString() const override {
        return "CHECKING";
    }

    void displayAccountInfo() const override {
        cout << "\n=== Checking Account Information ===" << endl;
        cout << "Account ID: " << accountId << endl;
        cout << "Customer ID: " << customerId << endl;
        cout << "Balance: $" << fixed << setprecision(2) << balance << endl;
        cout << "Overdraft Limit: $" << fixed << setprecision(2) << overdraftLimit << endl;
        cout << "Overdraft Fee: $" << fixed << setprecision(2) << overdraftFee << endl;
        cout << "Creation Date: " << creationDate << endl;
        cout << "Status: " << (isActive ? "Active" : "Closed") << endl;
    }
};

// Loan Account class
class LoanAccount : public Account {
private:
    double loanAmount;
    double interestRate;
    int termMonths;
    double monthlyPayment;

public:
    LoanAccount(const string& accId, const string& custId, double loanAmt, int term)
        : Account(accId, custId, -loanAmt, AccountType::LOAN),
          loanAmount(loanAmt), interestRate(0.065), termMonths(term) {
        
        // Calculate monthly payment using loan formula
        double monthlyRate = interestRate / 12;
        monthlyPayment = (loanAmt * monthlyRate * pow(1 + monthlyRate, termMonths)) /
                        (pow(1 + monthlyRate, termMonths) - 1);
    }

    void deposit(double amount) override {
        if (amount <= 0) {
            throw InvalidAmountException();
        }
        balance += amount; // Balance is negative for loans, so this reduces debt
        cout << "Payment of $" << fixed << setprecision(2) << amount 
             << " applied to Loan Account. Remaining balance: $" << abs(balance) << endl;
    }

    bool withdraw(double amount) override {
        cout << "Withdrawals not allowed on loan accounts." << endl;
        return false;
    }

    void calculateInterest() override {
        double interest = abs(balance) * interestRate / 12; // Monthly interest on remaining balance
        balance -= interest; // Increases debt
        cout << "Interest of $" << fixed << setprecision(2) << interest 
             << " applied to Loan Account. Remaining balance: $" << abs(balance) << endl;
    }

    string getAccountTypeString() const override {
        return "LOAN";
    }

    double getMonthlyPayment() const { return monthlyPayment; }

    void displayAccountInfo() const override {
        cout << "\n=== Loan Account Information ===" << endl;
        cout << "Account ID: " << accountId << endl;
        cout << "Customer ID: " << customerId << endl;
        cout << "Original Loan Amount: $" << fixed << setprecision(2) << loanAmount << endl;
        cout << "Remaining Balance: $" << fixed << setprecision(2) << abs(balance) << endl;
        cout << "Interest Rate: " << fixed << setprecision(1) << interestRate * 100 << "%" << endl;
        cout << "Term: " << termMonths << " months" << endl;
        cout << "Monthly Payment: $" << fixed << setprecision(2) << monthlyPayment << endl;
        cout << "Creation Date: " << creationDate << endl;
        cout << "Status: " << (isActive ? "Active" : "Closed") << endl;
    }
};

// Customer class
class Customer {
private:
    string customerId;
    string firstName;
    string lastName;
    string email;
    string phone;
    string address;
    vector<string> accountIds;

public:
    Customer(const string& id, const string& fname, const string& lname,
             const string& email, const string& phone, const string& addr)
        : customerId(id), firstName(fname), lastName(lname),
          email(email), phone(phone), address(addr) {}

    // Getters
    string getCustomerId() const { return customerId; }
    string getFirstName() const { return firstName; }
    string getLastName() const { return lastName; }
    string getFullName() const { return firstName + " " + lastName; }
    string getEmail() const { return email; }
    string getPhone() const { return phone; }
    string getAddress() const { return address; }
    vector<string> getAccountIds() const { return accountIds; }

    // Methods
    void addAccount(const string& accountId) {
        accountIds.push_back(accountId);
    }

    void removeAccount(const string& accountId) {
        accountIds.erase(remove(accountIds.begin(), accountIds.end(), accountId), accountIds.end());
    }

    void displayCustomerInfo() const {
        cout << "\n=== Customer Information ===" << endl;
        cout << "Customer ID: " << customerId << endl;
        cout << "Name: " << getFullName() << endl;
        cout << "Email: " << email << endl;
        cout << "Phone: " << phone << endl;
        cout << "Address: " << address << endl;
        cout << "Number of Accounts: " << accountIds.size() << endl;
        
        if (!accountIds.empty()) {
            cout << "Account IDs: ";
            for (size_t i = 0; i < accountIds.size(); ++i) {
                cout << accountIds[i];
                if (i < accountIds.size() - 1) cout << ", ";
            }
            cout << endl;
        }
    }

    // Setters for updating customer information
    void setEmail(const string& newEmail) { email = newEmail; }
    void setPhone(const string& newPhone) { phone = newPhone; }
    void setAddress(const string& newAddress) { address = newAddress; }
};

// Bank class - Main management class
class Bank {
private:
    string bankName;
    map<string, shared_ptr<Customer>> customers;
    map<string, shared_ptr<Account>> accounts;
    vector<shared_ptr<Transaction>> allTransactions;
    int nextCustomerId;
    int nextAccountId;

public:
    Bank(const string& name) : bankName(name), nextCustomerId(1000), nextAccountId(10000) {}

    // Customer management
    string createCustomer(const string& firstName, const string& lastName,
                         const string& email, const string& phone, const string& address) {
        string customerId = "CUST" + to_string(++nextCustomerId);
        auto customer = make_shared<Customer>(customerId, firstName, lastName, email, phone, address);
        customers[customerId] = customer;
        
        cout << "Customer created successfully with ID: " << customerId << endl;
        return customerId;
    }

    shared_ptr<Customer> findCustomer(const string& customerId) {
        auto it = customers.find(customerId);
        if (it != customers.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Account management
    string createSavingsAccount(const string& customerId, double initialDeposit) {
        auto customer = findCustomer(customerId);
        if (!customer) {
            throw AccountNotFoundException();
        }

        string accountId = "SAV" + to_string(++nextAccountId);
        auto account = make_shared<SavingsAccount>(accountId, customerId, initialDeposit);
        accounts[accountId] = account;
        customer->addAccount(accountId);

        // Create initial deposit transaction
        auto transaction = make_shared<Transaction>("BANK", accountId, initialDeposit, 
                                                   TransactionType::DEPOSIT, "Initial deposit");
        account->addTransaction(transaction);
        allTransactions.push_back(transaction);

        cout << "Savings account created successfully with ID: " << accountId << endl;
        return accountId;
    }

    string createCheckingAccount(const string& customerId, double initialDeposit) {
        auto customer = findCustomer(customerId);
        if (!customer) {
            throw AccountNotFoundException();
        }

        string accountId = "CHK" + to_string(++nextAccountId);
        auto account = make_shared<CheckingAccount>(accountId, customerId, initialDeposit);
        accounts[accountId] = account;
        customer->addAccount(accountId);

        // Create initial deposit transaction
        auto transaction = make_shared<Transaction>("BANK", accountId, initialDeposit, 
                                                   TransactionType::DEPOSIT, "Initial deposit");
        account->addTransaction(transaction);
        allTransactions.push_back(transaction);

        cout << "Checking account created successfully with ID: " << accountId << endl;
        return accountId;
    }

    string createLoanAccount(const string& customerId, double loanAmount, int termMonths) {
        auto customer = findCustomer(customerId);
        if (!customer) {
            throw AccountNotFoundException();
        }

        string accountId = "LOAN" + to_string(++nextAccountId);
        auto account = make_shared<LoanAccount>(accountId, customerId, loanAmount, termMonths);
        accounts[accountId] = account;
        customer->addAccount(accountId);

        // Create initial loan transaction
        auto transaction = make_shared<Transaction>("BANK", accountId, loanAmount, 
                                                   TransactionType::DEPOSIT, "Loan disbursement");
        account->addTransaction(transaction);
        allTransactions.push_back(transaction);

        cout << "Loan account created successfully with ID: " << accountId << endl;
        return accountId;
    }

    shared_ptr<Account> findAccount(const string& accountId) {
        auto it = accounts.find(accountId);
        if (it != accounts.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Transaction operations
    void deposit(const string& accountId, double amount) {
        auto account = findAccount(accountId);
        if (!account) {
            throw AccountNotFoundException();
        }

        account->deposit(amount);

        // Record transaction
        auto transaction = make_shared<Transaction>("EXTERNAL", accountId, amount, 
                                                   TransactionType::DEPOSIT);
        account->addTransaction(transaction);
        allTransactions.push_back(transaction);
    }

    void withdraw(const string& accountId, double amount) {
        auto account = findAccount(accountId);
        if (!account) {
            throw AccountNotFoundException();
        }

        if (account->withdraw(amount)) {
            // Record transaction
            auto transaction = make_shared<Transaction>(accountId, "EXTERNAL", amount, 
                                                       TransactionType::WITHDRAWAL);
            account->addTransaction(transaction);
            allTransactions.push_back(transaction);
        }
    }

    void transfer(const string& fromAccountId, const string& toAccountId, double amount) {
        auto fromAccount = findAccount(fromAccountId);
        auto toAccount = findAccount(toAccountId);

        if (!fromAccount || !toAccount) {
            throw AccountNotFoundException();
        }

        if (fromAccount->withdraw(amount)) {
            toAccount->deposit(amount);

            // Record transaction for both accounts
            auto transaction = make_shared<Transaction>(fromAccountId, toAccountId, amount, 
                                                       TransactionType::TRANSFER);
            fromAccount->addTransaction(transaction);
            toAccount->addTransaction(transaction);
            allTransactions.push_back(transaction);

            cout << "Transfer of $" << fixed << setprecision(2) << amount 
                 << " completed from " << fromAccountId << " to " << toAccountId << endl;
        }
    }

    // Reporting and display methods
    void displayAllCustomers() const {
        cout << "\n=== All Customers ===" << endl;
        if (customers.empty()) {
            cout << "No customers found." << endl;
            return;
        }

        for (const auto& pair : customers) {
            pair.second->displayCustomerInfo();
            cout << "------------------------" << endl;
        }
    }

    void displayAllAccounts() const {
        cout << "\n=== All Accounts ===" << endl;
        if (accounts.empty()) {
            cout << "No accounts found." << endl;
            return;
        }

        for (const auto& pair : accounts) {
            pair.second->displayAccountInfo();
            cout << "------------------------" << endl;
        }
    }

    void displayCustomerAccounts(const string& customerId) const {
        auto customer = customers.find(customerId);
        if (customer == customers.end()) {
            cout << "Customer not found." << endl;
            return;
        }

        cout << "\n=== Accounts for Customer: " << customer->second->getFullName() << " ===" << endl;
        auto accountIds = customer->second->getAccountIds();
        
        if (accountIds.empty()) {
            cout << "No accounts found for this customer." << endl;
            return;
        }

        for (const string& accId : accountIds) {
            auto account = accounts.find(accId);
            if (account != accounts.end()) {
                account->second->displayAccountInfo();
                cout << "------------------------" << endl;
            }
        }
    }

    void generateBankReport() const {
        cout << "\n========== BANK REPORT ==========" << endl;
        cout << "Bank Name: " << bankName << endl;
        cout << "Total Customers: " << customers.size() << endl;
        cout << "Total Accounts: " << accounts.size() << endl;
        cout << "Total Transactions: " << allTransactions.size() << endl;

        double totalDeposits = 0;
        int savingsCount = 0, checkingCount = 0, loanCount = 0;

        for (const auto& pair : accounts) {
            auto account = pair.second;
            if (account->getAccountType() == AccountType::SAVINGS) {
                savingsCount++;
                totalDeposits += account->getBalance();
            } else if (account->getAccountType() == AccountType::CHECKING) {
                checkingCount++;
                totalDeposits += account->getBalance();
            } else if (account->getAccountType() == AccountType::LOAN) {
                loanCount++;
            }
        }

        cout << "Savings Accounts: " << savingsCount << endl;
        cout << "Checking Accounts: " << checkingCount << endl;
        cout << "Loan Accounts: " << loanCount << endl;
        cout << "Total Deposits: $" << fixed << setprecision(2) << totalDeposits << endl;
        cout << "=================================" << endl;
    }

    // Monthly operations
    void processMonthlyInterest() {
        cout << "\n=== Processing Monthly Interest ===" << endl;
        for (const auto& pair : accounts) {
            auto account = pair.second;
            if (account->getIsActive()) {
                cout << "Processing account: " << account->getAccountId() << endl;
                account->calculateInterest();
                
                // Record interest transaction
                auto transaction = make_shared<Transaction>("BANK", account->getAccountId(), 0, 
                                                           TransactionType::INTEREST_CREDIT, "Monthly interest");
                account->addTransaction(transaction);
                allTransactions.push_back(transaction);
                cout << "------------------------" << endl;
            }
        }
    }

    // Save and load functionality
    void saveToFile(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error opening file for writing." << endl;
            return;
        }

        file << "=== BANK DATA EXPORT ===" << endl;
        file << "Bank Name: " << bankName << endl;
        file << "Export Date: ";
        
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);
        file << put_time(localtime(&time_t), "%Y-%m-%d %H:%M:%S") << endl;
        
        file << "\n=== CUSTOMERS ===" << endl;
        for (const auto& pair : customers) {
            auto customer = pair.second;
            file << customer->getCustomerId() << "|" 
                 << customer->getFirstName() << "|"
                 << customer->getLastName() << "|"
                 << customer->getEmail() << "|"
                 << customer->getPhone() << "|"
                 << customer->getAddress() << endl;
        }

        file << "\n=== ACCOUNTS ===" << endl;
        for (const auto& pair : accounts) {
            auto account = pair.second;
            file << account->getAccountId() << "|"
                 << account->getCustomerId() << "|"
                 << account->getAccountTypeString() << "|"
                 << account->getBalance() << "|"
                 << account->getCreationDate() << "|"
                 << (account->getIsActive() ? "ACTIVE" : "CLOSED") << endl;
        }

        file.close();
        cout << "Bank data saved to " << filename << endl;
    }
};

// Utility functions for the menu system
void displayMainMenu() {
    cout << "\n========== BANK MANAGEMENT SYSTEM ==========" << endl;
    cout << "1.  Create Customer" << endl;
    cout << "2.  Create Savings Account" << endl;
    cout << "3.  Create Checking Account" << endl;
    cout << "4.  Create Loan Account" << endl;
    cout << "5.  Deposit Money" << endl;
    cout << "6.  Withdraw Money" << endl;
    cout << "7.  Transfer Money" << endl;
    cout << "8.  View Customer Information" << endl;
    cout << "9.  View Account Information" << endl;
    cout << "10. View Customer Accounts" << endl;
    cout << "11. View Transaction History" << endl;
    cout << "12. View All Customers" << endl;
    cout << "13. View All Accounts" << endl;
    cout << "14. Process Monthly Interest" << endl;
    cout << "15. Generate Bank Report" << endl;
    cout << "16. Save Data to File" << endl;
    cout << "0.  Exit" << endl;
    cout << "=============================================" << endl;
    cout << "Choose an option: ";
}

// Main function with menu-driven interface
int main() {
    Bank bank("First National Bank");
    int choice;
    string customerId, accountId, fromAccount, toAccount;
    double amount;
    
    cout << "Welcome to the Bank Management System!" << endl;

    while (true) {
        try {
            displayMainMenu();
            cin >> choice;

            switch (choice) {
                case 1: {
                    string firstName, lastName, email, phone, address;
                    cout << "Enter first name: ";
                    cin >> firstName;
                    cout << "Enter last name: ";
                    cin >> lastName;
                    cout << "Enter email: ";
                    cin >> email;
                    cout << "Enter phone: ";
                    cin >> phone;
                    cout << "Enter address: ";
                    cin.ignore();
                    getline(cin, address);
                    
                    customerId = bank.createCustomer(firstName, lastName, email, phone, address);
                    break;
                }
                case 2: {
                    cout << "Enter customer ID: ";
                    cin >> customerId;
                    cout << "Enter initial deposit: $";
                    cin >> amount;
                    
                    bank.createSavingsAccount(customerId, amount);
                    break;
                }
                case 3: {
                    cout << "Enter customer ID: ";
                    cin >> customerId;
                    cout << "Enter initial deposit: $";
                    cin >> amount;
                    
                    bank.createCheckingAccount(customerId, amount);
                    break;
                }
                case 4: {
                    cout << "Enter customer ID: ";
                    cin >> customerId;
                    cout << "Enter loan amount: $";
                    cin >> amount;
                    int term;
                    cout << "Enter loan term (months): ";
                    cin >> term;
                    
                    bank.createLoanAccount(customerId, amount, term);
                    break;
                }
                case 5: {
                    cout << "Enter account ID: ";
                    cin >> accountId;
                    cout << "Enter deposit amount: $";
                    cin >> amount;
                    
                    bank.deposit(accountId, amount);
                    break;
                }
                case 6: {
                    cout << "Enter account ID: ";
                    cin >> accountId;
                    cout << "Enter withdrawal amount: $";
                    cin >> amount;
                    
                    bank.withdraw(accountId, amount);
                    break;
                }
                case 7: {
                    cout << "Enter from account ID: ";
                    cin >> fromAccount;
                    cout << "Enter to account ID: ";
                    cin >> toAccount;
                    cout << "Enter transfer amount: $";
                    cin >> amount;
                    
                    bank.transfer(fromAccount, toAccount, amount);
                    break;
                }
                case 8: {
                    cout << "Enter customer ID: ";
                    cin >> customerId;
                    
                    auto customer = bank.findCustomer(customerId);
                    if (customer) {
                        customer->displayCustomerInfo();
                    } else {
                        cout << "Customer not found." << endl;
                    }
                    break;
                }
                case 9: {
                    cout << "Enter account ID: ";
                    cin >> accountId;
                    
                    auto account = bank.findAccount(accountId);
                    if (account) {
                        account->displayAccountInfo();
                    } else {
                        cout << "Account not found." << endl;
                    }
                    break;
                }
                case 10: {
                    cout << "Enter customer ID: ";
                    cin >> customerId;
                    
                    bank.displayCustomerAccounts(customerId);
                    break;
                }
                case 11: {
                    cout << "Enter account ID: ";
                    cin >> accountId;
                    
                    auto account = bank.findAccount(accountId);
                    if (account) {
                        account->displayTransactionHistory();
                    } else {
                        cout << "Account not found." << endl;
                    }
                    break;
                }
                case 12: {
                    bank.displayAllCustomers();
                    break;
                }
                case 13: {
                    bank.displayAllAccounts();
                    break;
                }
                case 14: {
                    bank.processMonthlyInterest();
                    break;
                }
                case 15: {
                    bank.generateBankReport();
                    break;
                }
                case 16: {
                    string filename;
                    cout << "Enter filename: ";
                    cin >> filename;
                    bank.saveToFile(filename);
                    break;
                }
                case 0: {
                    cout << "Thank you for using Bank Management System!" << endl;
                    cout << "Goodbye!" << endl;
                    return 0;
                }
                default: {
                    cout << "Invalid choice. Please try again." << endl;
                    break;
                }
            }
        }
        catch (const BankException& e) {
            cout << "Bank Error: " << e.what() << endl;
        }
        catch (const exception& e) {
            cout << "System Error: " << e.what() << endl;
        }
        catch (...) {
            cout << "Unknown error occurred. Please try again." << endl;
        }

        // Pause before showing menu again
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }

    return 0;
}
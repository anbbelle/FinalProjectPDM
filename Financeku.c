#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_TRANSACTIONS 1000
#define MAX_USERS 100

typedef struct{
	char username[50];
	char password[50];
} user;

typedef struct{
	int id;
	char username[50];
	int day;
	int month;
	int year;
	char type[20];
	char category[50]; // income atau expense
	int amount;
	char notes[100];
} transaction;

transaction transactions[MAX_TRANSACTIONS];
int totalTransactions = 0;
char currentUser[50];
int i;

void clearScreen();
void pressAnyKey();
void showMainMenu();
void showLoginMenu();
int loginUser();
void registerUser();
void addTransaction();
void viewDashboard();
void viewHistory();
void viewReport();
void searchTransaction();
void exportData();
void showIncomeCategories();
void showExpenseCategories();
int isValidDate(int day, int month, int year);
int isLeapYear(int year);
void loadTransactions();
void saveTransaction(transaction t);
int getTotalIncome();
int getTotalExpense();
void toLowerCase(char *str);

int main(){
	int choice;
	
	clearScreen();
	printf("=========================================\n");
	printf("	  Welcome to Financeku!			\n");
	printf("=========================================\n");
	pressAnyKey();
	
	showLoginMenu();
	
	while(1){
		clearScreen();
		showMainMenu();
		
		printf("Select menu >> ");
		scanf("%d", &choice); getchar();
		
		switch(choice){
			case 1:
				clearScreen();
				viewDashboard();
				pressAnyKey();
				break;
			case 2:
				clearScreen();
				addTransaction();
				pressAnyKey();
				break;
			case 3:
				clearScreen();
				viewHistory();
				pressAnyKey();
				break;
			case 4:
				clearScreen();
				viewReport();
				pressAnyKey();
				break;
			case 5:
				clearScreen();
				searchTransaction();
				pressAnyKey();
				break;
			case 6:
				printf("\nThank you for using Financeku!\n");
				printf("Stay financially wise!\n");
				exit(0);
			default:
				printf("\n");
				printf("Invalid choice! Please try again.\n");
				pressAnyKey();
		}
	}
	
	return 0;
}

void clearScreen(){
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
}

void pressAnyKey(){
	printf("\n");
	printf("Press Enter to continue..."); getchar();
}

void showMainMenu(){
	printf("=========================================\n");
	printf("	   Financeku - Main Menu		\n");
	printf("=========================================\n");
	printf(" User: %s\n", currentUser);
	printf("=========================================\n");
	printf(" 1. Dashboard\n");
	printf(" 2. Add Transaction\n");
	printf(" 3. View History\n");
	printf(" 4. Monthly Report\n");
	printf(" 5. Search Transaction\n");
	printf(" 6. Exit\n");
	printf("=========================================\n");
}

void showLoginMenu(){
	int choice;
    
	while(1){
		clearScreen();
		printf("=========================================\n");
		printf("            Financeku - Login        \n");
		printf("=========================================\n");
		printf(" 1. Login\n");
		printf(" 2. Register\n");
		printf(" 3. Exit\n");
		printf("=========================================\n");
		printf("Select menu >> ");
		scanf("%d", &choice); getchar();
		
		switch(choice){
			case 1:
				if(loginUser()){
					return;
				}
				break;
			case 2:
				registerUser();
				break;
			case 3:
				printf("\n");
				printf("Goodbye!\n");
				exit(0);
			default:
				printf("\n");
				printf("Invalid choice!\n");
				pressAnyKey();
		}
	}
}

int loginUser(){
	clearScreen();
	FILE *file = fopen("users.txt", "r");
	if(!file){
		printf("No users registered yet!\n");
		pressAnyKey();
		return 0;
	}
	
	user user;
	char username[50], password[50];
	
	printf("=========================================\n");
	printf("                 Login                   \n");
	printf("=========================================\n");
	printf("Username: ");
	scanf("%s", username); getchar();
	printf("Password: ");
	scanf("%s", password); getchar();
	
	while(fscanf(file, "%s %s", user.username, user.password) == 2){
		if(strcmp(username, user.username) == 0 && strcmp(password, user.password) == 0){
			strcpy(currentUser, username);
			fclose(file);
			
			loadTransactions();
			
			printf("\n");
			printf("Login successful! Welcome, %s!\n", currentUser);
			pressAnyKey();
			return 1;
		}
	}
	
	fclose(file);
	printf("Invalid username or password!\n");
	pressAnyKey();
	return 0;
}

void registerUser(){
	clearScreen();
	FILE *file = fopen("users.txt", "a+");
	if(!file){
		printf("Error opening file!\n");
		return;
	}
	
	user newUser;
	user existingUser;
	int exists = 0;
	
	printf("=========================================\n");
	printf("              Registration               \n");
	printf("=========================================\n");
	
	do{
		exists = 0;
		printf("Enter username (3-20 characters): ");
		scanf("%s", newUser.username); getchar();
		
		if(strlen(newUser.username) < 3 || strlen(newUser.username) > 20){
			printf("Username must be 3-20 characters!\n");
			continue;
		}
	
		//check if username exists
		rewind(file);
		while(fscanf(file, "%s %s", existingUser.username, existingUser.password) == 2){
			if(strcmp(existingUser.username, newUser.username) == 0){
				exists = 1;
				printf("Username already exists! Try another.\n");
				break;
			}
		}
	} while(exists || strlen(newUser.username) < 3 || strlen(newUser.username) > 20);
	 
	printf("Enter password (3-20 characters): ");
	scanf("%s", newUser.password); getchar();
	
	fprintf(file, "%s %s\n", newUser.username, newUser.password);
	fclose(file);
	
	printf("\n");
	printf("Registration successful!\n");
	printf("You can now login with your credentials.\n");
	pressAnyKey();
}

void addTransaction(){
	transaction t;
	int typeChoice, categoryChoice;
	
	printf("=========================================\n");
	printf("           Add New Transaction           \n");
	printf("=========================================\n");
	
	//auto-generate ID
	t.id = totalTransactions + 1;
	strcpy(t.username, currentUser);
	
    //current date atau custom date
	printf("\n");
	printf("Use current date? (1 = Yes, 2 = No): ");
	int useCurrentDate;
	scanf("%d", &useCurrentDate); getchar();
	
	if(useCurrentDate == 1){
		time_t now = time(NULL);
		struct tm *tm = localtime(&now);
		t.day = tm->tm_mday;
		t.month = tm->tm_mon + 1;
        t.year = tm->tm_year + 1900;
	} else{
		do{
			printf("Enter date (DD MM YYYY): ");
			scanf("%d %d %d", &t.day, &t.month, &t.year); getchar();
			if(!isValidDate(t.day, t.month, t.year)){
			printf("Invalid date! Try again.\n");
			}
		} while(!isValidDate(t.day, t.month, t.year));
	}
    
    // tipe transaksi
	printf("\n");
	printf("Transaction Type: \n");
	printf("1. Income\n");
	printf("2. Expense\n");
    do{
		printf("Select type >> ");
		scanf("%d", &typeChoice); getchar();
		if(typeChoice < 1 || typeChoice > 2){
			printf("Invalid choice!\n");
		}
	} while(typeChoice < 1 || typeChoice > 2);
	
	if(typeChoice == 1){
		strcpy(t.type, "Income");
		showIncomeCategories();
		const char *incomeCategories[] = {"Salary/Allowance", "Bonus", "Freelance", 
											"Investment", "Gift", "Others"};
		do{
			printf("Select category >> ");
			scanf("%d", &categoryChoice); getchar();
			if(categoryChoice < 1 || categoryChoice > 6){
				printf("Invalid category!\n");
			}
		} while(categoryChoice < 1 || categoryChoice > 6);
		strcpy(t.category, incomeCategories[categoryChoice - 1]);
	} else{
		strcpy(t.type, "Expense");
		showExpenseCategories();
		const char *expenseCategories[] = {"Food & Drinks", "Transportation", "Entertainment",
											"Education", "Shopping", "Bills", "Health", "Others"};
		do{
			printf("Select category >> ");
			scanf("%d", &categoryChoice); getchar();
			if(categoryChoice < 1 || categoryChoice > 8){
				printf("Invalid category!\n");
			}
		} while(categoryChoice < 1 || categoryChoice > 8);
		strcpy(t.category, expenseCategories[categoryChoice - 1]);
	}
    
    //amount
	do{
		printf("\n");
		printf("Amount: Rp ");
		scanf("%d", &t.amount); getchar();
		if(t.amount <= 0){
			printf("Amount must be positive!\n");
		}
	} while(t.amount <= 0);
	
    //notes
	printf("Notes (optional, max 100 chars): ");
	fgets(t.notes, sizeof(t.notes), stdin);
	t.notes[strcspn(t.notes, "\n")] = 0;
	
    //save
	saveTransaction(t);
	transactions[totalTransactions++] = t;
	
	printf("\n");
	printf("Transaction added successfully!\n");
}

void viewDashboard(){
	int totalIncome = getTotalIncome();
	int totalExpense = getTotalExpense();
	int balance = totalIncome - totalExpense;
	
	printf("=========================================\n");
	printf("              Dashboard                  \n");
	printf("=========================================\n");
	printf("\n");
	printf("Current Balance: Rp %d\n", balance);
	printf("=========================================\n");
	printf("Total Income  : Rp %d\n", totalIncome);
	printf("Total Expense : Rp %d\n", totalExpense);
	printf("=========================================\n");
	
    // transaksi terbaru
    printf("\n");
	printf("Recent Transactions (Last 5):\n");
	printf("-----------------------------------------\n");
	
	int start = totalTransactions - 5;
	if(start < 0){
		start = 0;	
	}
	
	for(i = totalTransactions - 1; i >= start; i--){
		char sign = (strcmp(transactions[i].type, "Income") == 0) ? '+' : '-';
		printf(" %02d-%02d-%04d | %s | %c Rp %d\n",
				transactions[i].day, transactions[i].month, transactions[i].year,
				transactions[i].category, sign, transactions[i].amount);
	}
}

void viewHistory(){
	printf("============================================================================\n");
	printf("                           Transaction History                              \n");
	printf("============================================================================\n");
	printf("No | Date       | Type    | Category         | Amount      | Notes\n");
	printf("----------------------------------------------------------------------------\n");
	
	if(totalTransactions == 0){
		printf("                         No transactions yet.\n");
	}
	
	for(i = 0; i < totalTransactions; i++){
		printf("%-2d | %02d-%02d-%04d | %-7s | %-16s | %c%-10d | %s\n",
				i + 1,
				transactions[i].day, transactions[i].month, transactions[i].year,
				transactions[i].type,
				transactions[i].category,
				(strcmp(transactions[i].type, "Income") == 0) ? '+' : '-',
				transactions[i].amount,
				transactions[i].notes);
	}
	printf("============================================================================\n");
}

void viewReport(){
	int month, year;
	
	printf("=========================================\n");
	printf("            Monthly Report               \n");
	printf("=========================================\n");
	
	printf("Enter month (1-12): ");
	scanf("%d", &month);
	printf("Enter year: ");
	scanf("%d", &year); getchar();
	
	int monthlyIncome = 0, monthlyExpense = 0;
	
	printf("\n");
	printf("============================================================================\n");
	printf("                    Report for %02d-%04d\n", month, year);
	printf("============================================================================\n");
	printf("No | Date       | Type    | Category         | Amount      | Notes\n");
	printf("----------------------------------------------------------------------------\n");
	
	int found = 0;
	for(i = 0; i < totalTransactions; i++){
		if (transactions[i].month == month && transactions[i].year == year) {
			found = 1;
			printf("%-2d | %02d-%02d-%04d | %-7s | %-16s | %c%-10d | %s\n",
					i + 1,
					transactions[i].day, transactions[i].month, transactions[i].year,
					transactions[i].type,
					transactions[i].category,
					(strcmp(transactions[i].type, "Income") == 0) ? '+' : '-',
					transactions[i].amount,
					transactions[i].notes);
					
            if(strcmp(transactions[i].type, "Income") == 0){
            	monthlyIncome += transactions[i].amount;	
			} else{
				monthlyExpense += transactions[i].amount;	
			}
		}
	}
	
	if(!found){
		printf("                    No transactions for this month.\n");
	}
	
	printf("============================================================================\n");
	printf("Total Income : Rp %d\n", monthlyIncome);
	printf("Total Expense: Rp %d\n", monthlyExpense);
	printf("Net          : Rp %d\n", monthlyIncome - monthlyExpense);
	printf("============================================================================\n");
}

void searchTransaction(){
	printf("=========================================\n");
	printf("          Search Transaction             \n");
	printf("=========================================\n");
	printf("1. Search by Category\n");
	printf("2. Search by Type (Income/Expense)\n");
	printf("3. Search by Amount Range\n");
	printf("=========================================\n");
	
	int choice;
	printf("Select >> ");
	scanf("%d", &choice); getchar();
	
	char keyword[50];
	int minAmount, maxAmount;
	
	switch(choice){
		case 1:
			printf("Enter category name: ");
			fgets(keyword, sizeof(keyword), stdin);
			keyword[strcspn(keyword, "\n")] = 0;
			toLowerCase(keyword);
			break;
		case 2:
			printf("Enter type (income/expense): ");
			fgets(keyword, sizeof(keyword), stdin);
			keyword[strcspn(keyword, "\n")] = 0;
			toLowerCase(keyword);
			break;
		case 3:
			printf("Enter minimum amount: ");
			scanf("%d", &minAmount);
			printf("Enter maximum amount: ");
			scanf("%d", &maxAmount); getchar();
			break;
		default:
			printf("Invalid choice!\n");
			return;
	}
	
	printf("\n");
	printf("============================================================================\n");
	printf("                           Search Results                                   \n");
	printf("============================================================================\n");
	printf("No | Date       | Type    | Category         | Amount      | Notes\n");
	printf("----------------------------------------------------------------------------\n");
	
	int found = 0;
	for(i = 0; i < totalTransactions; i++){
		int match = 0;
		
		if(choice == 1){
			char tempCategory[50];
			strcpy(tempCategory, transactions[i].category);
			toLowerCase(tempCategory);
			if(strstr(tempCategory, keyword) != NULL){
			match = 1;	
			}
		} else if(choice == 2){
			char tempType[20];
			strcpy(tempType, transactions[i].type);
			toLowerCase(tempType);
			if(strcmp(tempType, keyword) == 0){
				match = 1;
			}
		} else if(choice == 3){
			if(transactions[i].amount >= minAmount && transactions[i].amount <= maxAmount){
				match = 1;
			}
		}
		
		if(match){
			found = 1;
			printf("%-2d | %02d-%02d-%04d | %-7s | %-16s | %c%-10d | %s\n",
					i + 1,
					transactions[i].day, transactions[i].month, transactions[i].year,
					transactions[i].type,
					transactions[i].category,
					(strcmp(transactions[i].type, "Income") == 0) ? '+' : '-',
					transactions[i].amount,
					transactions[i].notes);
		}
	}
	
	if(!found){
		printf("                         No matching transactions found.\n");
	}
	printf("============================================================================\n");
}

void showIncomeCategories(){
	printf("\n");
	printf("Income Categories:\n");
	printf("1. Salary/Allowance\n");
	printf("2. Bonus\n");
	printf("3. Freelance\n");
	printf("4. Investment\n");
	printf("5. Gift\n");
	printf("6. Others\n");
}

void showExpenseCategories(){
    printf("\n");
	printf("Expense Categories:\n");
	printf("1. Food & Drinks\n");
	printf("2. Transportation\n");
	printf("3. Entertainment\n");
	printf("4. Education\n");
	printf("5. Shopping\n");
	printf("6. Bills\n");
	printf("7. Health\n");
	printf("8. Others\n");
}

int isValidDate(int day, int month, int year){
	if(year < 2000 || year > 2100 || month < 1 || month > 12 || day < 1){
    	return 0;
	}
	
	int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	
	if(isLeapYear(year)){
    	daysInMonth[2] = 29;
	}
	
	return day <= daysInMonth[month];
}

int isLeapYear(int year){
	return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

void loadTransactions(){
	FILE *file = fopen("transactions.txt", "r");
	if(!file){
		return;
	}
	
	totalTransactions = 0;
	while(fscanf(file, "%d|%[^|]|%d-%d-%d|%[^|]|%[^|]|%d|%[^\n]\n",
				&transactions[totalTransactions].id,
				transactions[totalTransactions].username,
				&transactions[totalTransactions].day,
				&transactions[totalTransactions].month,
				&transactions[totalTransactions].year,
				transactions[totalTransactions].type,
				transactions[totalTransactions].category,
				&transactions[totalTransactions].amount,
				transactions[totalTransactions].notes) == 9){
				
		if(strcmp(transactions[totalTransactions].username, currentUser) == 0){
			totalTransactions++;
		}
	}
	fclose(file);
}

void saveTransaction(transaction t){
	FILE *file = fopen("transactions.txt", "a");
	if(!file){
		printf("Error saving transaction!\n");
		return;
	}
	
	fprintf(file, "%d|%s|%02d-%02d-%04d|%s|%s|%d|%s\n",
			t.id, t.username, t.day, t.month, t.year,
			t.type, t.category, t.amount, t.notes);
	fclose(file);
}

int getTotalIncome(){
	int total = 0;
	for(i = 0; i < totalTransactions; i++){
		if(strcmp(transactions[i].type, "Income") == 0){
			total += transactions[i].amount;
		}
	}
	return total;
}

int getTotalExpense(){
	int total = 0;
	for(i = 0; i < totalTransactions; i++){
		if(strcmp(transactions[i].type, "Expense") == 0){
			total += transactions[i].amount;
		}	
	}
	return total;
}

void toLowerCase(char *str){
	int j; //ganti variable i jadi j
	for(j = 0; str[j]; j++){ //ganti variable i jadi j
		str[j] = tolower(str[j]); //ganti variable i jadi j
	}
}

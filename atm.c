#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

struct Account{
    int accNo;
    char name[30];
    int pin;
    float balance;
};

// ---------------- PIN INPUT ----------------
int inputPIN(){
    char ch;
    int pin = 0, count = 0;
    printf("Enter 4-digit PIN: ");
    while((ch = getch()) != '\r'){
        if(ch >= '0' && ch <= '9' && count < 4){
            pin = pin * 10 + (ch - '0');
            count++;
            printf("*");
        }
        else if(ch == 8 && count > 0){
            pin = pin / 10;
            count--;
            printf("\b \b");}
    }
    printf("\n");
    if(count != 4){
        printf("PIN must be exactly 4 digits!\n");
        return -1;}
    return pin;
}

// ---------------- SAVE TRANSACTION ----------------
void saveTransaction(int accNo, char type[], float amount, float balance){
    FILE *fp = fopen("history.txt","a");
    if(fp == NULL){
        printf("Transaction file error!\n");
        return;}
    fprintf(fp,
            "Acc:%d | %s | Amount: %.2f | Balance: %.2f\n",
            accNo,type,amount,balance);
    fclose(fp);
}

// ---------------- UPDATE ACCOUNT ----------------
void updateAccount(struct Account user){
    FILE *fp = fopen("accounts.txt","rb");
    FILE *temp = fopen("temp.txt","wb");
    struct Account data;
    while(fread(&data,sizeof(data),1,fp)){
        if(data.accNo == user.accNo)
            fwrite(&user,sizeof(user),1,temp);
        else
            fwrite(&data,sizeof(data),1,temp);
    }
    fclose(fp);
    fclose(temp);

    remove("accounts.txt");
    rename("temp.txt","accounts.txt");
}

// ---------------- CREATE ACCOUNT ----------------
void createAccount(){
    struct Account user,temp;
    printf("Enter Account Number: ");
    scanf("%d",&user.accNo);
    FILE *fp = fopen("accounts.txt","rb");
    if(fp != NULL){
        while(fread(&temp,sizeof(temp),1,fp)){
            if(temp.accNo == user.accNo){
                printf("Account already exists!\n");
                fclose(fp);
                return;
            }
        }
        fclose(fp);
    }
    printf("Enter Name: ");
    scanf("%s",user.name);

    user.pin = inputPIN();
    if(user.pin == -1)
        return;
    printf("Enter Initial Balance: ");
    scanf("%f",&user.balance);
    fp = fopen("accounts.txt","ab");
    if(fp == NULL){
        printf("File error!\n");
        return;
    }
    fwrite(&user,sizeof(user),1,fp);
    fclose(fp);
    printf("Account Created Successfully!\n");
}

// ---------------- LOGIN ----------------
int login(struct Account *user){
    int acc,pin;
    printf("Enter Account Number: ");
    scanf("%d",&acc);
    pin = inputPIN();
    if(pin == -1)
        return 0;
    FILE *fp = fopen("accounts.txt","rb");
    if(fp == NULL){
        printf("No accounts found!\n");
        return 0;
    }
    while(fread(user,sizeof(struct Account),1,fp)){
        if(user->accNo == acc && user->pin == pin){
            fclose(fp);
            return 1;}
    }
    fclose(fp);
    return 0;
}

// ---------------- CHECK BALANCE ----------------
void checkBalance(struct Account user){
    printf("Current Balance: %.2f\n",user.balance);}

// ---------------- DEPOSIT ----------------
void deposit(struct Account *user){
    float amt;
    printf("Enter amount to deposit: ");
    scanf("%f",&amt);
    if(amt <= 0){
        printf("Invalid amount!\n");
        return;}
    user->balance += amt;
    updateAccount(*user);
    saveTransaction(user->accNo,
                    "DEPOSIT",
                    amt,
                    user->balance);
    printf("Deposit Successful!\n");
}

// ---------------- WITHDRAW ----------------
void withdraw(struct Account *user){
    float amt;
    printf("Enter amount to withdraw: ");
    scanf("%f",&amt);
    if(amt <= 0){
        printf("Invalid amount!\n");
        return;}
    if(amt > user->balance){
        printf("Insufficient Balance!\n");
        return;}
    user->balance -= amt;
    updateAccount(*user);
    saveTransaction(user->accNo,
                    "WITHDRAW",
                    amt,
                    user->balance);
    printf("Withdrawal Successful!\n");
    }

// ---------------- MINI STATEMENT ----------------
void miniStatement(int accNo){
    FILE *fp = fopen("history.txt","r");
    if(fp == NULL){
        printf("No transaction history found!\n");
        return;}
    char line[200];
    char store[3][200];
    int count = 0;
    char prefix[20];
    sprintf(prefix,"Acc:%d |",accNo);
    while(fgets(line,sizeof(line),fp)){
        if(strstr(line,prefix)){
            strcpy(store[count % 3],line);
            count++;}
    }
    fclose(fp);
    printf("\n===== LAST 3 TRANSACTIONS =====\n");
    if(count == 0){
        printf("No transactions yet.\n");
        return;}
    int start = (count > 3) ? count - 3 : 0;
    for(int i = start; i < count; i++){
        printf("%s",store[i % 3]);}}

// ---------------- MENU ----------------
void menu(struct Account *user){
    int choice;
    do{
        printf("\n====== ATM MENU ======\n");

        printf("1. Balance Enquiry\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Mini Statement\n");
        printf("5. Exit\n");

        printf("Enter choice: ");
        scanf("%d",&choice);

        switch(choice){
            case 1: checkBalance(*user);
                break;
            case 2:deposit(user);
                break;
            case 3:withdraw(user);
                break;
            case 4:miniStatement(user->accNo);
                break;
            case 5:printf("Thank You!\n");
                break;
            default:
                printf("Invalid Choice!\n");
        }
    }while(choice != 5);
}

// ---------------- MAIN ----------------
int main(){
    int choice;
    struct Account user;
    do{
        printf("\n======= ATM SYSTEM =======\n");

        printf("1. Create Account\n");
        printf("2. Login\n");
        printf("3. Exit\n");

        printf("Enter choice: ");
        scanf("%d",&choice);

        switch(choice){
            case 1:createAccount();
                break;
            case 2:
                if(login(&user)){
                    printf("Login Successful!\n");
                    menu(&user);}
                else{
                    printf("Invalid Account Number or PIN!\n");
                }
                break;
            case 3:printf("Exiting Program...\n");
                break;
            default:
                printf("Invalid Choice!\n");
        }
    }while(choice != 3);
    return 0;
}
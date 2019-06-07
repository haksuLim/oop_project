#include<iostream>
#include<cstring>
#include<cstdlib>
#include<cstdio>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define MAX_CUSTOMER 100

using namespace std;

class Customer
{	
protected:
  char name[10];
  int money;
  int account;
  
public:
  Customer()
  {
	  
  }
  
  Customer(char* name, int money, int account)
  {
    strcpy(this->name, name);
    this->money = money;
    this->account = account;
  }
  
  Customer(Customer& cus)
  {
    strcpy(this->name, cus.GetName());
    this->money = cus.GetMoney();
    this->account = cus.GetAccount();
  }
  
  char* GetName()
  {
    return name;
  }
  
  void SetName(char* name)
  {
    strcpy(this->name, name);
  }
  
  int GetMoney() const
  {
    return money;
  }
  
  void SetMoney(int money)
  {
    this->money = money;
  }
  
  int GetAccount() const
  {
    return account;
  }
  
  void SetAccount(int account)
  {
    this->account = account;
  }
  
  void Deposit(int money)
  {
    this->money += money;
  }

  void Withdraw(int money)
  {
    this->money -= money;
  }
};

class NormalCustomer : public Customer
{
protected:
  int normalRate;

public:
  NormalCustomer(char* name, int money, int account, int rate)
  : Customer(name, money, account), normalRate(rate)
  {
  }
  
  NormalCustomer(NormalCustomer& cus)
  {
    strcpy(this->name, cus.GetName());
    this->money = cus.GetMoney();
    this->account = cus.GetAccount();
    this->normalRate = cus.GetRate();
  }

  virtual void Deposit(int money) 
  {
    Customer::Deposit(money*(1+(normalRate/100.0)));
  }
  int GetRate() const
  {
    return normalRate;
  }
};

class HighCreditCustomer : public NormalCustomer
{
protected:
  int specialRate;

public:
  HighCreditCustomer(char* name, int money, int account, int rate, int specialRate)
  : NormalCustomer(name, money, account, rate), specialRate(specialRate)
  {
  }

  virtual void Deposite(int money)
  {
    Customer::Deposit(money*(1+((GetRate()+specialRate)/100.0)));
  }
};

class CustomerHandler
{
private:
  int customerNumber;
  Customer* cus[MAX_CUSTOMER];
public:
  CustomerHandler() : customerNumber(0)
  {
  }
  
  int GetCustomerNumber() const
  {
    return customerNumber;
  }
  
  Customer** GetCus() //const
  {
    return cus;
  }
  
  Customer* CreateCustomer(char* name, int money, int account) //const
  {
    cus[customerNumber] = new Customer(name, money, account);
    return cus[customerNumber++];
  }

  Customer* CreateNormalCustomer(char* name, int money, int account, int rate) //const
  {
    cus[customerNumber] = new NormalCustomer(name, money, account, rate);
    return cus[customerNumber++];
  }
  
  Customer* CreateHighCreditCustomer(char* name, int money, int account, int rate, int specialRate) //const
  {
    cus[customerNumber] = new HighCreditCustomer(name, money, account, rate, specialRate);
    return cus[customerNumber++];
  }

  Customer* FindCustomer(int account) const
  {
    for(int i=0; i<customerNumber; i++)
    {
      if(account == cus[i]->GetAccount()){
        return cus[i];
      }
				
    }
    
    return NULL;
  }
};

CustomerHandler ch;

class ServSocketManager
{
private:
  int serv_sock;
  int clnt_sock;
  socklen_t clnt_addr_size;

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;

public:

  int makeServSocket(int domain, int type, int protocol)
  {
    serv_sock = socket(domain, type, protocol);
    return serv_sock;
  }

  int doListen(int backlog)
  {
    return listen(serv_sock, backlog);
  }

  int doBind()
  {
    return bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  }

  int doAccept()
  {
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    return clnt_sock;
  }

  void setServAddr(int family, int addr, int port)
  {
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = family;
    serv_addr.sin_addr.s_addr = addr;
    serv_addr.sin_port = port;
  }

  int getServSockFD()
  {
    return serv_sock;
  }
  
  int getClntSockFD()
  {
    return clnt_sock;
  }

  int readMSG(char* msg, int msg_size)
  {
    return read(clnt_sock, msg, msg_size);
  }

  int writeMSG(char* msg, int msg_size)
  {
    return write(clnt_sock, msg, msg_size);
  }

  void closeSocket()
  {
    close(serv_sock);
    close(clnt_sock);
  }
};

class ErrorHandler
{
public:
  static void error_handling(char* message)
  {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
  }
};

int main(int argc, char* argv[])
{
  int str_len = 1;
  int cmd;
  int cmd2;
  char message[30];
  ServSocketManager ssm;

  int account, money, normalRate, rank;
  char name[10];  

  if(argc!=2)
  {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }
  
  if(ssm.makeServSocket(PF_INET, SOCK_STREAM, 0) == -1)
    ErrorHandler::error_handling("socket() error");
    
  ssm.setServAddr(AF_INET, htonl(INADDR_ANY), htons(atoi(argv[1])));
  
  if(ssm.doBind() == -1)
    ErrorHandler::error_handling("bind() error");
  
  if(ssm.doListen(5) == -1)
    ErrorHandler::error_handling("listen() error");
  
  if(ssm.doAccept() == -1)
    ErrorHandler::error_handling("accept() error");
  
  while(str_len != -1)
  {
    str_len = ssm.readMSG(message, sizeof(message)-1);
    if(str_len != 0)
    {
      sscanf(message, "%d", &cmd);
      //cmd = atoi(message);
      
      switch(cmd)
      {
      case 1:
        ssm.readMSG(message, sizeof(message)-1);
	sscanf(message, "%d", &cmd2);

        ssm.readMSG(message, sizeof(message)-1);
	if(cmd2 == 1)
	{
	  sscanf(message, "%d %s %d %d", &account, name, &money, &normalRate);
	  ch.CreateNormalCustomer(name, money, account, normalRate);
	}
	else if(cmd2 == 2)
	{
	  sscanf(message, "%d %s %d %d %d", &account, name, &money, &normalRate, &rank);
	  if(rank == 1)
	    ch.CreateHighCreditCustomer(name, money, account, normalRate, 7); 
	  else if(rank == 2)
	    ch.CreateHighCreditCustomer(name, money, account, normalRate, 4); 
	  else if(rank == 3)
	    ch.CreateHighCreditCustomer(name, money, account, normalRate, 1); 
	}
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      default:
        break;
      }
      
      printf("MSG : %s\n", message);
    }
  }
  
  ssm.writeMSG(message, sizeof(message));
  
  ssm.closeSocket();
  
  return 0;
}



#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

class ClntSocketManager
{
private:
  int serv_sock;
  struct sockaddr_in serv_addr;
  socklen_t clnt_addr_size;

public:
  int makeServSocket(int domain, int type, int protocol)
  {
    serv_sock = socket(domain, type, protocol);
  }

  void setServAddr(int family, int addr, int port)
  {
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = family;
    serv_addr.sin_addr.s_addr = addr;
    serv_addr.sin_port = port;
  }

  int doConnect()
  {
    return connect(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  }

  int readMSG(char* msg, int msg_size)
  {
    return read(serv_sock, msg, msg_size);
  }

  int writeMSG(char* msg, int msg_size)
  {
    return write(serv_sock, msg, msg_size);
  }

  int getServSockFD()
  {
    return serv_sock;
  }

  void closeSocket()
  {
    close(serv_sock);
  }
};

class UIManager
{
public:
  void createAccount()
  {
     
  }

  void createNormalAccount()
  {

  }

  void createSpecialAccount()
  {

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
  ClntSocketManager csm;
  char message[30];
  char cmd[10];
  char nextCmd[10];
  int sel;
  int str_len;
  int acc_type;

  int account;
  char name[10];
  int money;
  int normalRate;
  int specialRate;


  if(argc!=3)
  {
    printf("Usage : %s <IP> <port>\n", argv[0]);
    exit(1);
  }
  
  if(csm.makeServSocket(PF_INET, SOCK_STREAM, 0) == -1)
    ErrorHandler::error_handling("socket() error");

  csm.setServAddr(AF_INET, inet_addr(argv[1]), htons(atoi(argv[2])));

  if(csm.doConnect() == -1)
    ErrorHandler::error_handling("connect() error");

  while(1)
  {
    scanf("%s", cmd);
    sel = atoi(cmd);
    str_len = csm.writeMSG(cmd, sizeof(cmd)-1);

    if(str_len == -1)
      ErrorHandler::error_handling("write() error");

    switch(sel)
    {
    case 1:		// CreateAccount();
      system("clear");
      printf("\n[계좌종류선택]\n");
      printf("1.Normal 2.HighCredit\n");
      scanf("%d", &acc_type);
      sprintf(message, "%d", acc_type);
      csm.writeMSG(message, sizeof(message)-1);
      
      printf("\n[계좌개설]\n");
      printf("계좌ID: ");

      scanf("%d", &account);
      printf("이 름: ");
      scanf("%s", name);
      printf("입금액: ");
      scanf("%d", &money);
      printf("이 자: ");
      scanf("%d", &normalRate);
      if(acc_type == 1)
      {	
        sprintf(message, "%d %s %d %d", account, name, money, normalRate);
      }
      else if(acc_type == 2)
      {
        printf("신용등급(1, 2, 3)\n");
	scanf("%d", &specialRate);
        sprintf(message, "%d %s %d %d %d", account, name, money, normalRate, specialRate);
      }
      csm.writeMSG(message, sizeof(message)-1);
      break;

    case 2:		// Deposit();
      break;

    case 3:		// Withdraw();
      break;

    case 4:		// PrintAllAccountInfo();
      break;

    case 5:
      exit(0);
      break;

    default:
      break;

    }

  }

  printf("Message from server : %s \n", message);
  csm.closeSocket();

  return 0;
}

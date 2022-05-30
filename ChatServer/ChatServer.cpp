#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)
SOCKET clients[64];
int numClients = 0;
void RemoveClient(SOCKET client)
{
    // Tim vi tri cua client trong mang
    int i = 0;
    for (; i < numClients; i++)
        if (clients[i] == client) break;
    // Xoa client khoi mang
    if (i < numClients - 1)
        clients[i] = clients[numClients - 1];
    numClients--;
}
DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET client = *(SOCKET*)lpParam;
    int ret;
    char buf[256];
    char cmd[32], id[32], tmp[32];
    const char* fault = "Sai cu phap lenh! Vui long nhap lai\n";
    const char* success = "Dung cu phap lenh! Thuc hien lenh\n";
    // Xu ly dang nhap
    while (1)
    {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            return 0;

        buf[ret] = 0;
        printf("Du lieu nhan duoc: %s\n", buf);
        ret = sscanf(buf, "%s %s %s", cmd, id, tmp);
        if (ret != 2)
        {
            const char* msg = "Sai cu phap. Hay nhap lai.\n";
            send(client, msg, strlen(msg), 0);
        }
        else
        {
            if (strcmp(cmd, "client_id:") != 0)
            {
                const char* msg = "Sai cu phap. Hay nhap lai.\n";
                send(client, msg, strlen(msg), 0);
            }
            else
            {
                const char* msg = "Dung cu phap. Nhap tin nhan de chuyen tiep.\n";
                send(client, msg, strlen(msg), 0);
                // Them vao mang
                clients[numClients] = client;
                numClients++;

                //gui thong bao khi co client dang nhap thanh cong
                const char* msg = "co client moi dang nhap vao";
                for (int i = 0; i < numClients; i++) {
                    send(clients[i], msg, strlen(msg), 0);
                }
                break;
            }
        }
    }
    // Xu li lenh
    char list[64];

    while (1)
    {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            RemoveClient(client);
            return 0;
        }
        buf[ret] = 0;
        printf("Du lieu nhan duoc: %s\n", buf);
        
        //xu li xem danh sach clients
        ret = sscanf(buf, "%s %s %s", cmd, list, tmp);
        if (ret != 2)
        {
            send(client, fault, strlen(fault), 0);
        }
        else
        {
            if (strcmp(cmd, "request:") != 0)
                send(client, fault, strlen(fault), 0);
            else
            {
                if (strcmp(list, "list") != 0)
                    send(client, fault, strlen(fault), 0);
                else
                {
                    send(client, success, strlen(success), 0);
                    send(client, "Danh sach clients dang ket noi: \n", strlen("Danh sach clients dang ket noi: \n"), 0);
                    for (int i = 0; i < numClients; i++) {
                        
                    }
                }
            }
        }  
    }
    closesocket(client);
}
int main()
{
    // Khoi tao thu vien
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    // Tao socket
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Khai bao dia chi server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);
    // Gan cau truc dia chi voi socket
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    // Chuyen sang trang thai cho ket noi
    listen(listener, 5);
    while (1)
    {
        SOCKET client = accept(listener, NULL, NULL);
        printf("Client moi ket noi: %d\n", client);
        CreateThread(0, 0, ClientThread, &client, 0, 0);
    }
}
#include "Renderer.h"

enum
{
    PACKET_NAME
};

typedef struct Packet
{
    byte type;
    void* memoryLocation;
    uint memorySize;
} Packet;

typedef struct
{
    SOCKET socket;
    char* username;
} Player;

Player* players;
uint playerCount;

bool QueryIsHost()
{
    bool isHost;

    printf("Type \"H\" for host or \"C\" for client:\n");
    char response[5];
    GetStrInput(response, 4);

    while (strcmp(response, "h") != 0 && strcmp(response, "H") != 0 && strcmp(response, "c") != 0 && strcmp(response, "C") != 0)
    {
        printf("That was invalid! You must type \"H\" or \"C\"\n");
        GetStrInput(response, 4);
    }

    isHost = strcmp(response, "h") == 0 || strcmp(response, "H") == 0;

    if (isHost)
        printf("You are now the host.\n");
    else
        printf("You are now a client.\n");

    return isHost;
}

bool WinsockInit()
{
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult == 0) return true;
    printf("WSAStartup failed: %d\n", iResult);
    return false;
}
// "10.0.0.55"
struct addrinfo* SetupClient()
{
    struct addrinfo* result = NULL,
        hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    printf("What IP address would you like to connect to?\nIf you wish to exit type \"exit\"\n");
    char ipAddress[MAX_IP_ADDRESS_LENGTH + 1];
    GetStrInput(ipAddress, MAX_IP_ADDRESS_LENGTH);
    if (strcmp(ipAddress, "exit") == 0) return NULL;

    int iResult = getaddrinfo(ipAddress, DEFAULT_PORT, &hints, &result);
    while (iResult != 0)
    {
        printf("Invalid IP address! Please enter again.\nIf you wish to exit type \"exit\"\n");
        char ipAddress[MAX_IP_ADDRESS_LENGTH + 1];
        GetStrInput(ipAddress, MAX_IP_ADDRESS_LENGTH);
        if (strcmp(ipAddress, "exit") == 0) return NULL;
        iResult = getaddrinfo(ipAddress, DEFAULT_PORT, &hints, &result);
    }
    return result;
}

int PrintLocalIPStuff()
{
    char ac[80];
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
        printf("Error %d when getting local host name.\n", WSAGetLastError());
        return 1;
    }
    printf("Host name is %s\n", ac);

    struct hostent* phe = gethostbyname(ac);
    if (phe == 0) {
        printf("Bad host lookup.");
        return 1;
    }

    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        printf("Address %i: %s\n", i, inet_ntoa(addr));
    }

    return 0;
}

struct addrinfo* SetupHost()
{
    struct addrinfo* result = NULL,
        hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        return NULL;
    }
    printf("Your information for pairing is:\n");
    PrintLocalIPStuff();

    return result;
}

bool SetupHostListenSocket(SOCKET* ListenSocket, struct addrinfo* result)
{
    // Create a SOCKET for the server to listen for client connections
    *ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (*ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        return false;
    }

    // Setup the TCP listening socket
    int iResult = bind(*ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(*ListenSocket);
        return false;
    }

    freeaddrinfo(result);

    return true;
}
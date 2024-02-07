#include "Multiplayer.h"

int main(int argc, char** argv) {
    printf("Greetings universe!\n");
    if (!InitRenderer())
        goto BadReturn;

    printf("Input your username (max of %i):\n", MAX_USERNAME_LENGTH);
    char username[MAX_USERNAME_LENGTH + 1];
    GetStrInput(username, MAX_USERNAME_LENGTH);
    printf("Your name is now %s\n", username);

    bool isHost = QueryIsHost();

    if (!WinsockInit())
        return 1;

    if (isHost)
    {
        SOCKET ListenSocket = INVALID_SOCKET;

        SetupHostListenSocket(&ListenSocket, SetupHost());

        if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
            printf("Listen failed with error: %ld\n", WSAGetLastError());
            closesocket(ListenSocket);
            goto BadReturn;
        }

        SOCKET ClientSocket;
        ClientSocket = INVALID_SOCKET;

        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            goto BadReturn;
        }

        char recvbuf[DEFAULT_BUFLEN];
        int iSendResult;
        int recvbuflen = DEFAULT_BUFLEN;
        Packet receivedPacket;
        
        int iResult;
        // Receive until the peer shuts down the connection
        do
        {
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult > 1)
            {
                receivedPacket = (Packet){ (byte)recvbuf[0], recvbuf + 1, iResult - 1};

                switch (receivedPacket.type)
                {
                case PACKET_NAME:
                    printf("Username of client is: %s\n", (char*)receivedPacket.memoryLocation);
                    break;
                default:
                    printf("Packet type was invalid\n");
                }

                int usernameLen = (int)(strlen(username) + 1);

                if (!usernameLen)
                {
                    printf("send failed: Invalid send packet\n");
                    closesocket(ClientSocket);
                    goto BadReturn;
                }

                byte* sentPacket = (byte*)malloc((size_t)usernameLen + 1);
                if (sentPacket == NULL)
                {
                    printf("send failed: Invalid send packet malloc\n");
                    closesocket(ClientSocket);
                    goto BadReturn;
                }

                sentPacket[0] = PACKET_NAME;
                memcpy(&sentPacket[1], username, usernameLen);

                iSendResult = send(ClientSocket, sentPacket, usernameLen + 1, 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    goto BadReturn;
                }
            }
            else if (iResult == 0)
                printf("Connection closing...\n");
            else if (iResult == 1)
            {
                printf("recv failed: Undersized packet.");
                closesocket(ClientSocket);
                goto BadReturn;
            }
            else {
                printf("recv failed: %d\n", WSAGetLastError());
            }

        } while (iResult > 0);

        // shutdown the send half of the connection since no more data will be sent
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            goto BadReturn;
        }

        // shutdown the send half of the connection since no more data will be sent
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            goto BadReturn;
        }

        // cleanup
        closesocket(ClientSocket);

        printf("Host stuff was successful!\n");
    }
    else // Is Client
    {
        struct addrinfo* result = SetupClient(),
            * ptr = NULL;
        if (result == NULL)
            goto BadReturn;

        SOCKET ConnectSocket = INVALID_SOCKET;

        // Attempt to connect to the first address returned by
        // the call to getaddrinfo
        ptr = result;

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET) {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            goto BadReturn;
        }

        // Connect to server.
        int iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
        }

        // Should really try the next address returned by getaddrinfo
        // if the connect call failed
        // But for this simple example we just free the resources
        // returned by getaddrinfo and print an error message

        freeaddrinfo(result);

        if (ConnectSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            goto BadReturn;
        }

        int recvbuflen = DEFAULT_BUFLEN;
        char recvbuf[DEFAULT_BUFLEN];

        int usernameLen = (int)(strlen(username) + 1);

        if (!usernameLen)
        {
            printf("send failed: Invalid send packet\n");
            closesocket(ConnectSocket);
            goto BadReturn;
        }

        byte* sentPacket = (byte*)malloc((size_t)usernameLen + 1);
        if (sentPacket == NULL)
        {
            printf("send failed: Invalid send packet malloc\n");
            closesocket(ConnectSocket);
            goto BadReturn;
        }

        sentPacket[0] = PACKET_NAME;
        memcpy(&sentPacket[1], username, usernameLen);

        // Send an initial buffer
        iResult = send(ConnectSocket, sentPacket, usernameLen + 1, 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            goto BadReturn;
        }

        // shutdown the connection for sending since no more data will be sent
        // the client can still use the ConnectSocket for receiving data
        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            goto BadReturn;
        }

        // Receive data until the server closes the connection
        do {
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            if (iResult > 1)
            {
                Packet receivedPacket = (Packet){ (byte)recvbuf[0], recvbuf + 1, iResult - 1 };

                switch (receivedPacket.type)
                {
                case PACKET_NAME:
                    printf("Username of host is: %s\n", (char*)receivedPacket.memoryLocation);
                    break;
                default:
                    printf("Packet type was invalid\n");
                }
            }
            else if (iResult == 0)
                printf("Connection closed\n");
            else if (iResult == 1)
                printf("recv failed: Undersized packet.");
            else
                printf("recv failed: %d\n", WSAGetLastError());
        } while (iResult > 0);

        // shutdown the send half of the connection since no more data will be sent
        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            goto BadReturn;
        }

        // cleanup
        closesocket(ConnectSocket);

        printf("Client stuff was successful!\n");
    }

    while (!glfwWindowShouldClose(renderer.window))
        UpdateRenderer();

    WSACleanup();
    DestroyRenderer();
    printf("Farewell universe!\n\n");
    return 0;

BadReturn:
    WSACleanup();
    DestroyRenderer();
    return 1;
}
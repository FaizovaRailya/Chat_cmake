#include "socket.h"

Socket_server::~Socket_server(){
#if defined(__linux__)
    close(socket_id);
#elif (defined(_WIN32) || defined(_WIN64))
    closesocket(socket_id);
    WSACleanup();
#endif
}

#if defined(__linux__)
Socket_server::Socket_server(){
    tcpserveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    tcpserveraddress.sin_port = htons(PORT);
    tcpserveraddress.sin_family = AF_INET;

    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        std::cout << "Socket creation failed.!" << std::endl;
        exit(1);
    }
    
    bind_status = bind(socket_id, (struct sockaddr*)&tcpserveraddress, sizeof(tcpserveraddress));
    if (bind_status == -1) {
        std::cout << "Socket binding failed!" << std::endl;
        exit(1);
    }
    listen_socket();
}
#elif (defined(_WIN32) || defined(_WIN64))
Socket_server::Socket_server()
{
    tcpserveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    tcpserveraddress.sin_port = htons(PORT);
    tcpserveraddress.sin_family = AF_INET;

    WSADATA wsaData;
    int result_code = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result_code != NO_ERROR){
        std::cout << "Winsock Initialize failed with error: " << result_code << std::endl;
        return;
    }
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == INVALID_SOCKET){
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }
    result_code = bind(socket_id, (SOCKADDR*)&tcpserveraddress, sizeof(tcpserveraddress));
    if (result_code != NO_ERROR){
        std::cout << "Binding error: " << result_code << std::endl;
        exit(1);
    }
    listen_socket();
}
#endif 

#if defined(__linux__)
bool Socket_server::listen_socket(){
    connection_status = listen(socket_id, 5);
    if (connection_status == -1) {
        std::cout << "Socket is unable to listen for new connections!" << std::endl;
        exit(1);
    }
    else{
        std::cout << "Server is listening for new connection: " << std::endl;
    }
    length = sizeof(tcpclient);
    accept_socket = accept(socket_id, (struct sockaddr*)&tcpclient, &length);
    if (accept_socket == -1) {
        std::cout << "Server is unable to accept the data from client.!" << std::endl;
        close(socket_id);
        return false;
    }
    std::cout << "Client connected." << std::endl;
    return true;
}
#elif (defined(_WIN32) || defined(_WIN64))
bool Socket_server::listen_socket(){
    int result_code = listen(socket_id, 5);
    if (result_code != NO_ERROR){
        std::cout << "Listening error: " << result_code;
        exit(1);
    }
    else {
        std::cout << "Server is listening for new connection..." << std::endl;
    }
    socklen_t length = sizeof(tcpclient);
    accept_socket = accept(socket_id, (struct sockaddr*)&tcpclient, &length);
    if (accept_socket == INVALID_SOCKET) {
        std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(socket_id);
        WSACleanup();
        return false;
    }
    std::cout << "Client connected.\n";
    return true;
}
#endif

#if defined(__linux__)
char* Socket_server::receive_data(){
    bzero(message, MESSAGE_LENGTH);
    int result = read(accept_socket, message, MESSAGE_LENGTH);
    if (result > 0)    {
        message[result] = '\0';
    }
    else if (result == 0){
        std::cout << "Connection closed" << std::endl;
        return (char*)"end_chat";
    }
    else{
        std::cout << "recv failed with error" << std::endl;
        return (char*)"end_chat";
    }
    if (!strncmp("end_chat", message, 8)) {
        std::cout << "Client Exited." << std::endl;
        std::cout << "Server is Exiting..!" << std::endl;
    }
    return message;
    std::cout << "Data received from client: " << message << std::endl;
}
#elif (defined(_WIN32) || defined(_WIN64))
char* Socket_server::receive_data(){
    int result = recv(accept_socket, message, MESSAGE_LENGTH, 0);
    if (result > 0){
        message[result] = '\0';
    }
    else if (result == 0){
        std::cout << "Connection closed\n";
        return "end_chat";
    }
    else{
        std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
        return "end_chat";
    }
    if (!strncmp("end_chat", message, 8)) {
        std::cout << "Client Exited." << std::endl;
        std::cout << "Server is Exiting..!" << std::endl;
        return "end_chat";
    }
    return message;
}
#endif

#if defined(__linux__)
int Socket_server::send_data(char* msg){
    bzero(message, MESSAGE_LENGTH);
    strcpy(message, msg);
    ssize_t bytes = write(accept_socket, message, (int)strlen(message));
    if (bytes < 0) {
        std::cout << "send failed with error" << std::endl;
        close(accept_socket);
        return bytes;
    }
    if (strncmp("end_receive", message, 11)&&strncmp("end_chat", message, 8)) {
        std::cout << ">>> " << message << std::endl;
    }
    return bytes;
}
#elif (defined(_WIN32) || defined(_WIN64))
int Socket_server::send_data(char* msg){
    memset((message), '\0', (MESSAGE_LENGTH)), (void)0; //bzero(message2, MESSAGE_LENGTH);1111111111111111111111111111111
    strcpy_s(message, msg);
    int result = send(accept_socket, message, (int)strlen(message), 0);
    //cout << "sent: " << result << " bytes: (" << message << ") - " << (int)strlen(message) << endl;1111111111111111111111
    if (result == SOCKET_ERROR) {
        std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(accept_socket);
        WSACleanup();
        return result;
    }
    if (strncmp("end_receive", message, 11)&&strncmp("end_chat", message, 8)) {
        std::cout << ">>> " << message << std::endl;
    }
    return result;
}
#endif

#if defined(__linux__)
Socket_client::Socket_client(){
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        std::cout << "Creation of Socket failed!" << std::endl;
        exit(1);
    }

    tcpserveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    tcpserveraddress.sin_port = htons(PORT);
    tcpserveraddress.sin_family = AF_INET;

    connection = connect(socket_id, (struct sockaddr*)&tcpserveraddress, sizeof(tcpserveraddress));
    if (connection == -1) {
        std::cout << "Connection with the server failed.!" << std::endl;
        exit(1);
    }
}
#elif (defined(_WIN32) || defined(_WIN64))
Socket_client::Socket_client(){
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR){
        std::cout << "Winsock Initialize failed with error : " <<  result << std::endl;
        return;
    }
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == INVALID_SOCKET)    {
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }
    //tcpserveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");11111111111111111111111111111111111111111111111111111111111111111111111
    inet_pton(AF_INET, "127.0.0.1", &tcpserveraddress.sin_addr.s_addr);
    tcpserveraddress.sin_port = htons(PORT);
    tcpserveraddress.sin_family = AF_INET;
    
    result = connect(socket_id, (struct sockaddr*)&tcpserveraddress, sizeof(tcpserveraddress));
    if (result != NO_ERROR) {
        std::cout << "Connection error: " << result;
        exit(1);
    }
}
#endif

Socket_client::~Socket_client(){
#if defined(__linux__)
    close(socket_id);
#elif (defined(_WIN32) || defined(_WIN64))
    closesocket(socket_id);
    WSACleanup();
#endif
}

#if defined(__linux__)
char* Socket_client::receive_data(){
    bzero(message, sizeof(message));
    int result = read(socket_id, message, MESSAGE_LENGTH);
    if (result > 0){
        message[result] = '\0';
    }
    else if (result == 0){
        std::cout << "Connection closed" << std::endl;
        return (char*)"end_chat";
    }
    else{
        std::cout << "recv failed with error" << std::endl;
        return (char*)"end_chat";
    }

    if (!strncmp("end_chat", message, 8)) {
        std::cout << "Client Exited." << std::endl;
        std::cout << "Server is Exiting..!" << std::endl;
    }
    return message;
}
#elif (defined(_WIN32) || defined(_WIN64))
char* Socket_client::receive_data(){
    int result = recv(socket_id, message, MESSAGE_LENGTH, 0);
    if (result > 0){
        message[result] = '\0';
    }
    else if (result == 0){
        std::cout << "Connection closed\n";
        return "end_chat";
    }
    else{
        std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
        return "end_chat";
    }

    if (!strncmp("end_chat", message, 8)) {
        std::cout << "Client Exited." << std::endl;
        std::cout << "Server is Exiting..!" << std::endl;
    }
    return message;
}
#endif

#if defined(__linux__)
int Socket_client::send_data(char* msg){
    bzero(message, sizeof(message));
    strcpy(message, msg);
    ssize_t bytes = write(socket_id, message, (int)strlen(message));
    if (bytes < 0) {
        std::cout << "send failed with error" << std::endl;
        close(socket_id);
    }
    return bytes;
}
#elif (defined(_WIN32) || defined(_WIN64))
int Socket_client::send_data(char* msg){
    memset((message), '\0', (MESSAGE_LENGTH)), (void)0; //bzero(message2, MESSAGE_LENGTH);
    strcpy_s(message, msg);
    int result = send(socket_id, message, (int)strlen(message), 0);
    if (result == SOCKET_ERROR) {
        std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(socket_id);
        WSACleanup();
    }
    return result;
}
#endif
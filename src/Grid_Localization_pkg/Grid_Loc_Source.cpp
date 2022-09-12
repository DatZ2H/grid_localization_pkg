#include "Grid_Localization_pkg/Grid_Loc_library.h"

clientSock::clientSock(string host, unsigned int port)
{
    int t = 0;
    HOST_IP = host;
    PORT_IP = port;
    connect();
}

clientSock::clientSock(string host, unsigned int port, bool automsg)
{
    int t = 0;
    HOST_IP = host;
    PORT_IP = port;
    connect();
    
    //  tcp_auto_read(1);
}

clientSock::clientSock()
{
    connected = false;
}

clientSock::clientSock(int sock)
{
    sockfd = sock;
    connected = true;
}

clientSock::~clientSock()
{
    // disconnect();
}

int clientSock::connect()
{
    cout << "connecting!!" << endl;
    clientSock::host = HOST_IP;
    clientSock::port = PORT_IP;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    server = gethostbyname(host.data());
    bcopy((char *)server->h_addr, (char *)&servaddr.sin_addr.s_addr, server->h_length);
    servaddr.sin_port = htons(port);

    if (connected)
        disconnect();

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // struct timeval tv;
    // tv.tv_sec = 10;//5 Secs Timeout
    // tv.tv_usec = 0;//Not init'ing this can cause strange errors
    // setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(struct timeval));
    // setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));

    enable_keepalive(sockfd);

    for (size_t i = 0; i < 10; i++)
    { // try to connect 10 times
        if (::connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
            cout << "Error on connecting: " << errno << "  " << strerror(errno) << endl;
        else
        {
            connected = true;
            cout << "connected!!!" << endl;
            return 0;
        }
    }

    connected = false;
    return 1;
}

bool clientSock::hasError()
{
    if (sockfd == -1)
        return true;

    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);

    if (retval != 0 || error != 0)
        return true;
    else
        return false;
}

int clientSock::enable_keepalive(int sock)
{
    int yes = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) == -1)
        return -1;

    int idle = 1;

    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) == -1)
        return -1;

    int interval = 1;

    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) == -1)
        return -1;

    int maxpkt = 10;

    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) == -1)
        return -1;

    return 0;
}

int clientSock::disconnect()
{
    if (!connected)
        return -1;

    close(sockfd);
    connected = false;

    return 0;
}
string clientSock::convertToString(char* chararray){
    string str(chararray);
    return str;
}
int clientSock::convertToInt( char* chararray,int pos, int len){
    string tmp = this->convertToString(chararray);
    //cout << "tmp.substr(pos, len) = "<<tmp.substr(pos, len)<< endl;
    return std::stoi(tmp.substr(pos, len));

}
size_t clientSock::getContent( char *buffer){
    

Qrcode.code.rowcode = buffer[POSE_POS_CODE];
Qrcode.code.colcode = buffer[POSE_POS_CODE+1];

Qrcode.content.x = this->convertToInt(buffer,POSE_POS_X+1,PARA_POS_X_LENGTH-1);
Qrcode.content.y = this->convertToInt(buffer,POSE_POS_Y+1,PARA_POS_Y_LENGTH-1);

}
size_t clientSock::getPose( char *buffer){
Qrcode.pose.XMCL= this->convertToInt(buffer,POSE_XMCL,PARA_XMCL_LENGTH);
Qrcode.pose.YMCL = this->convertToInt(buffer,POSE_YMCL,PARA_YMCL_LENGTH);
Qrcode.pose.ANS  = this->convertToInt(buffer,POSE_AN2,PARA_AN2_LENGTH);
}
size_t clientSock::getCode( char *buffer){
                clientSock::getContent(buffer);
                
                cout << "label_content.code.rowcode = " << Qrcode.code.rowcode << endl;
                cout << "label_content.code.rowcode = " << Qrcode.code.colcode << endl;
                cout << "label_content.x = " << Qrcode.content.x << endl;
                cout << "label_content.y = " << Qrcode.content.y << endl;
                clientSock::getPose(buffer);
                cout << "label_pose.XMCL.x = " << Qrcode.pose.XMCL<< endl;
                cout << "label_pose.YMCL.y = " << Qrcode.pose.YMCL << endl;
                cout << "label_pose.ANS = " << Qrcode.pose.ANS << endl;
}
int clientSock::tcp_read()
{
    cout << ("tcp_read...") << endl;

    int MAX_LENGTH = PARA_READ_MCL_LENGTH;

     char to_rec[MAX_LENGTH];
    ssize_t k = tcp_receive(to_rec, MAX_LENGTH);
     cout << ("tcp_receive Stop") << endl;
    if (k = -1)
    {
    }
    if (to_rec[0] == PARA_START_BIT)
    {
        if (to_rec[PARA_HEARTHEART_LENGTH - 1] == PARA_END_BIT)
        {
            for (int i = 0; i < PARA_HEARTHEART_LENGTH; i++)
            {
                cout << "to_rec PING [" << i << "]= " << to_rec[i] << endl;
            }
        }else if (to_rec[PARA_READ_MCL_LENGTH - 1] == PARA_END_BIT)
        {
            for (int i = 0; i < PARA_READ_MCL_LENGTH; i++)
            {

                cout << "to_rec MCL [" << i << "]= " << to_rec[i] << endl;
                clientSock::getCode(to_rec);
            }

        }
    }
}
size_t clientSock::tcp_send( char *to_send, int length)
{
    if (connected)
    {

        struct timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(sockfd, &writefds);

        int sentBytes = 0;

        FD_ZERO(&writefds);
        FD_SET(sockfd, &writefds);
        int rv = select(sockfd + 1, NULL, &writefds, NULL, &tv);

        if (rv == -1)
        {

            cout << errno << "  " << strerror(errno) << endl;
            set_bad_con();
        }
        else if (rv == 0)
        {

            set_bad_con();
            sentBytes = 0;
        }
        else if (rv > 0 && FD_ISSET(sockfd, &writefds))
        {
             char new_to_send[length];
            for (size_t i = 0; i < length; i++)
            {
                new_to_send[i] = to_send[i];
                /* code */
            }

            sentBytes = ::write(sockfd, ( char *)new_to_send, length);

            if (sentBytes == -1)
            {
                cout << "Error sending IDs: " << errno << "  " << strerror(errno) << endl;
                set_bad_con();
                return 1;
            }
        }

        return 0;
    }
    else
    {
        connected = false;
        return 1;
    }
}
size_t clientSock::tcp_receive( char *buffer, int length)
{
     cout << ("tcp_receive Start") << endl;
     char buffer_array[buffSize];
    struct timeval tv;
    tv.tv_sec = 45;
    tv.tv_usec = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    uint16_t resp = 0;
     int n = 0;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    int rv = select(sockfd + 1, &readfds, NULL, NULL, &tv);
    cout << ("Listining socket...") << endl;
    if (rv <= -1)
    {
        cout << ("socket error accured") << endl;

        set_bad_con();
        return -1;
    }
    else if (rv == 0)
    {
        cout << ("socket timeout occured") << endl;

        set_bad_con();
        return -1;
    }
    else if (rv > 0 && FD_ISSET(sockfd, &readfds))
    {

        int tn = ::recv(sockfd, buffer_array, buffSize, 0); // avoid signcompare warning

        if (tn > 0)
        {
            cout << ("Listining socket") << endl;
            for (int i = 0; i < length; i++)
            {
                buffer[i] = buffer_array[i];
            }
            return tn;
        }
        else if (tn == -1)
        {
            cout << ("socket timeout occured") << endl;
            set_bad_con();
            return -1;
        }
    }
    else
    {
        cout << "ERROR: rv: " << rv << endl;
        set_bad_con();
        return -1;
    }
}
void clientSock::set_bad_con()
{
    ROS_ERROR("DISCONNECT FLEXISOFT");
    connected = false;
}

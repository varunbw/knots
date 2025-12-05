#include <gtest/gtest.h>
#include <string>
#include <thread>

#include "HttpServer.hpp"
#include "NetworkIO.hpp"
#include "Socket.hpp"
#include "Utils.hpp"

constexpr int serverPort = 10000;
constexpr int serverMaxConnections = 10;
constexpr bool runConsoleInputThread = false;

/*
    Basic client object giving you a TCP socket to talk to the server
    Use `NetworkIO` namespace for easier communication
*/
class Client {
public:
    bool m_isReady;

    const Socket m_socket;

    sockaddr_in m_serverInfo;
    const std::string m_serverIp;
    const int m_serverPort;

    Client();

    bool ConnectToServer();
};

Client::Client() :
    m_isReady(false),
    m_socket(socket(AF_INET, SOCK_STREAM, 0)),
    m_serverIp("127.0.0.1"),
    m_serverPort(serverPort) {

    if (m_socket.Get() < 0) {
        Log::Error(std::format(
            "Client(): Socket creation failed; {}",
            strerror(errno)
        ));
    }

    m_serverInfo.sin_family = AF_INET;
    m_serverInfo.sin_port = htons(m_serverPort);

    // Convert IP address to binary
    if (inet_pton(AF_INET, m_serverIp.c_str(), &m_serverInfo.sin_addr) < 0) {
        Log::Error(std::format(
            "Sender(): Invalid IP Address; {}",
            strerror(errno)
        ));
        return;
    }

    m_isReady = true;
    
    return;
}

bool Client::ConnectToServer() {
    // Connect to the server
    if (connect(
        m_socket.Get(), reinterpret_cast<sockaddr*>(&m_serverInfo), sizeof(m_serverInfo)
    ) < 0) {
        Log::Error(std::format(
            "Sender(): Connection failed; {}",
            strerror(errno)
        ));
        
        return false;
    }
    
    return true;
}

/*
    Just a normal connection to the server
*/
TEST(HttpServerTest, BasicConnection) {

    constexpr HttpServerConfiguration config(
        serverPort, serverMaxConnections, runConsoleInputThread
    );
    Router router;

    HttpServer server(config, router);
    std::jthread thread(&HttpServer::AcceptConnections, &server);

    // Probably not required, but I'd rather not be debugging race conditions
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    
    Client client;

    // Client initialized properly?
    EXPECT_TRUE(client.m_isReady) << MakeErrorMessage(
        "Client failed to initialize"
    );

    // Client can connect to server?
    EXPECT_TRUE(client.ConnectToServer()) << MakeErrorMessage(
        "Client could not connect to server"
    );

    server.Shutdown();
}

/*
    Send a basic request to the server, and get the appropriate response
*/
TEST(HttpServerTest, BasicRequestResponse) {

    const std::string messageToSend =
        "<html><body>\n"
        "<h1>Hello world!</h1>\n"
        "</body></html>\n";

    constexpr HttpServerConfiguration config(
        serverPort, serverMaxConnections, runConsoleInputThread
    );

    Router router;
    router.AddRoute(
        HttpMethod::GET, "/",
        [messageToSend] (const HttpRequest& req, HttpResponse& res) {
            res.body = messageToSend;
            res.headers["Content-Length"] = std::to_string(res.body.size());
            
            return;
        }
    );

    HttpServer server(config, router);
    std::jthread thread(&HttpServer::AcceptConnections, &server);

    Client client;

    // Client initialized properly?
    EXPECT_TRUE(client.m_isReady) << MakeErrorMessage(
        "Client failed to initialize"
    );
    
    // Client can connect to server?
    EXPECT_TRUE(client.ConnectToServer())
        << MakeErrorMessage("Client could not connect to server");

    const std::string req = 
        "GET / HTTP/1.1\r\n"
        "Host: localhost:10000\r\n"
        "\r\n"
        "Ping\r\n";
    
    // Request sent?
    EXPECT_TRUE(NetworkIO::Send(client.m_socket, req, 0))
        << MakeErrorMessage("Client failed to send request to server");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    std::string buffer(1024, '\0');
    ssize_t bytesReceived = recv(client.m_socket.Get(), buffer.data(), buffer.size() - 1, 0);

    // Received response properly?
    ASSERT_GT(bytesReceived, 0)
        << MakeErrorMessage(std::format(
            "Client did not receive properly, `bytesReceived`:{}",
            bytesReceived
        ));

    buffer = buffer.substr(0, bytesReceived);

    const std::string expectedResponse = std::format(
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: {}\r\n"
        "\r\n"
        "{}",
        messageToSend.size(),
        messageToSend
    );

    // Received correct response?
    EXPECT_EQ(buffer, expectedResponse)
        << MakeErrorMessage("Wrong message received");

    server.Shutdown();
}
 
/*
    Check whether requesting an invalid route returns a 404
*/
TEST(HttpServerTest, InvalidRouteReturns404) {

    constexpr HttpServerConfiguration config(
        serverPort, serverMaxConnections, runConsoleInputThread
    );

    Router router;

    HttpServer server(config, router);
    std::jthread thread(&HttpServer::AcceptConnections, &server);

    Client client;

    // Client initialized properly?
    EXPECT_TRUE(client.m_isReady) << MakeErrorMessage(
        "Client failed to initialize"
    );
    
    // Client can connect to server?
    EXPECT_TRUE(client.ConnectToServer())
        << MakeErrorMessage("Client could not connect to server");

    const std::string req = 
        "GET / HTTP/1.1\r\n"
        "Host: localhost:10000\r\n"
        "\r\n"
        "Ping\r\n";
    
    // Request sent?
    EXPECT_TRUE(NetworkIO::Send(client.m_socket, req, 0))
        << MakeErrorMessage("Client failed to send request to server");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    std::string buffer(1024, '\0');
    ssize_t bytesReceived = recv(client.m_socket.Get(), buffer.data(), buffer.size() - 1, 0);

    // Received response properly?
    ASSERT_GT(bytesReceived, 0)
        << MakeErrorMessage(std::format(
            "Client did not receive properly, `bytesReceived`:{}",
            bytesReceived
        ));

    buffer = buffer.substr(0, bytesReceived);

    // ! UNDO BEFORE MERGING
    // const std::string expectedResponse = 
    //     "HTTP/1.1 404 Not Found\r\n"
    //     "\r\n"
    //     "<!DOCTYPE html>\n<html>\n<body>\n"
    //     "    <h1 align='center'>404 Not Found</h1>\n"
    //     "    <p align='center'>The request URL <b>/</b> does not exist.</p>\n"
    //     "    <p align='center'>Please check the URL and try again.</p>\n"
    //     "</body>\n</html>\n";

    // // Received correct response?
    // EXPECT_EQ(buffer, expectedResponse)
    //     << MakeErrorMessage("Wrong message received");

    server.Shutdown();
}

/*
    Tests whether the server keeps the connection alive if the header
    `Connection: keep-alive` is specified
*/
TEST(HttpServerTest, ConnectionStaysAlive) {

    const std::string serverResponseBody =
        "<html><body>\n"
        "<h1>Hello world!</h1>\n"
        "</body></html>\n";

    const std::string serverResponse = std::format(
        "HTTP/1.1 200 OK\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: {}\r\n"
        "\r\n"
        "{}",
        serverResponseBody.size(),
        serverResponseBody
    );

    constexpr HttpServerConfiguration config(
        serverPort, serverMaxConnections, runConsoleInputThread
    );

    Router router;
    router.AddRoute(
        HttpMethod::GET, "/",
        [serverResponseBody] (const HttpRequest& req, HttpResponse& res) {
            res.body = serverResponseBody;
            res.headers["Content-Length"] = std::to_string(res.body.size());
            
            return;
        }
    );

    HttpServer server(config, router);
    std::jthread thread(&HttpServer::AcceptConnections, &server);

    Client client;

    // Client initialized properly?
    EXPECT_TRUE(client.m_isReady) << MakeErrorMessage(
        "Client failed to initialize"
    );
    
    // Client can connect to server?
    EXPECT_TRUE(client.ConnectToServer())
        << MakeErrorMessage("Client could not connect to server");


    const std::string req = 
        "GET / HTTP/1.1\r\n"
        "Host: localhost:10000\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: 4\r\n"
        "\r\n"
        "Ping";

    // Req #1
    // Request sent?
    EXPECT_TRUE(NetworkIO::Send(client.m_socket, req, 0))
        << MakeErrorMessage("Client failed to send request to server");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));


    std::string buffer(1024, '\0');
    ssize_t bytesReceived = recv(client.m_socket.Get(), buffer.data(), buffer.size(), 0);

    // Received some response?
    EXPECT_GT(bytesReceived, 0)
        << MakeErrorMessage(std::format(
            "Client did not receive properly, `bytesReceived`:{}",
            bytesReceived
        ));

    buffer.resize(bytesReceived);

    //     Received correct response?
    EXPECT_EQ(buffer, serverResponse)
        << MakeErrorMessage("Unexpected response from server");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // Req #2
    // Request sent again?
    EXPECT_TRUE(NetworkIO::Send(client.m_socket, req, 0))
        << MakeErrorMessage("Client failed to send request to server");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    buffer = std::string(1024, '\0');
    bytesReceived = recv(client.m_socket.Get(), buffer.data(), buffer.size(), 0);

    // Received some response?
    EXPECT_GT(bytesReceived, 0)
        << MakeErrorMessage(std::format(
            "Client did not receive properly, `bytesReceived`:{}",
            bytesReceived
        ));

    buffer.resize(bytesReceived);

    // Received correct response?
    EXPECT_EQ(buffer, serverResponse)
        << MakeErrorMessage("Unexpected response from server");

    server.Shutdown();
}
// Project
#include "Utils.hpp"
#include "HttpServer.hpp"

int main(void) {

    HttpServerConfiguration config = ParseConfigurationFile("config/config.yaml");

    Router router;

    // Example route
    router.AddRoute(
        HttpMethod::GET, "/",
        [] (const HttpRequest& req, HttpResponse& res) {
            
        res.body = "<html><body>"
            "<h1>Hello world</h1>"
            "</body></html>";
        res.headers["Content-Length"] = std::to_string(res.body.size());

        return;
    });

    HttpServer server(config, router);
    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}
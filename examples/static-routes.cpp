#include "knots/HttpServer.hpp"
#include "knots/StaticRoutes.hpp"
#include "knots/utils/Config.hpp"
#include "knots/utils/Log.hpp"

int main(void) {

    constexpr int port = 8600;
    constexpr int maxConnections = 125;
    constexpr int inputPollingIntevalMs = 100;
    constexpr RequestLoggingVerbosity verbosity = RequestLoggingVerbosity::FULL;

    constexpr HttpServerConfiguration config (
        port, maxConnections, inputPollingIntevalMs, verbosity
    );

    Router router;

    StaticRoutes::AddStaticDirectory("../knots", router);

    HttpServer server(config, router);

    server.AddErrorRoute(404, [] (const HttpRequest& req, HttpResponse& res) {
        res.SetBody(
            "<html>\n"
                "<body>\n"
                    "<h1 align=\"center\">404 Not Found\n</h1>"
                "<html>\n"
            "<body>\n"
        );
        res.SetStatus(404);
        res.SetHeader("Content-Type", "text/html");
        return;
    });

    server.AddErrorRoute(405, [] (const HttpRequest& req, HttpResponse& res) {
        res.SetBody(
            "<html>\n"
                "<body>\n"
                    "<h1 align=\"center\">405 Method Not Allowed\n</h1>"
                "<html>\n"
            "<body>\n"
        );
        res.SetStatus(405);
        res.SetHeader("Content-Type", "text/html");
        return;
    });

    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}
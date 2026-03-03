// Project
#include "knots/FileHandler.hpp"
#include "knots/HttpServer.hpp"
#include "knots/Utils.hpp"

int main(void) {

    HttpServerConfiguration config = ParseConfigurationFile("config/config.yaml");
    Router router;

    std::vector<std::string> urlsToAdd = {
        "/users",
        "/users/{userId}",
        "/users/{userId}/profile",
        "/users/{userId}/settings",
        "/users/{userId}/orders",
        "/users/{userId}/orders/{orderId}",
        "/users/details",
        "/users/details/{detailId}",
        "/users/search",
        "/users/search/{query}",
        "/login",
        "/logout",
        "/auth/{provider}",
        "/auth/{provider}/callback",
        "/session/{sessionId}",
        "/products",
        "/products/{productId}",
        "/products/{productId}/reviews",
        "/products/{productId}/reviews/{reviewId}",
        "/products/category/{categoryName}",
        "/products/category/{categoryName}/page/{page}",
        "/blog",
        "/blog/{slug}",
        "/blog/{slug}/comments",
        "/blog/{slug}/comments/{commentId}",
        "/blog/archive/{year}/{month}",
        "/blog/tags/{tag}",
        "/admin",
        "/admin/users",
        "/admin/users/{userId}",
        "/admin/settings",
        "/admin/logs/{date}",
        "/files/{fileId}",
        "/files/{fileId}/download",
        "/media/images",
        "/media/images/{imageId}",
        "/cart",
        "/cart/items",
        "/cart/items/{itemId}",
        "/checkout",
        "/notifications",
        "/notifications/{notificationId}",
        "/search/{query}/page/{page}",
        "/status/{code}",
        "/settings",
        "/settings/{section}",
        "/cities/{cityName}/weather",
        "/v1/{resource}/{id}",
        "/api/{version}/users/{userId}",
        "/healthcheck",
    };

    for (const std::string& url : urlsToAdd) {
        router.Get(url,
            [] (const HttpRequest& req, HttpResponse& res) {
                res.SetBody(
                    "<html>\n"
                        "<body>\n"
                            "<h1 align=\"center\">Hello world\n</h1>"
                        "<html>\n"
                    "<body>\n"
                );
                res.SetHeader("Content-Type", "text/html");
                return;
            }
        );
    }
    
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
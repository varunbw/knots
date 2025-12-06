// Project
#include "FileHandler.hpp"
#include "HttpServer.hpp"
#include "Utils.hpp"

const std::vector<std::string> urls = {
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

int main(void) {

    HttpServerConfiguration config = ParseConfigurationFile("config/config.yaml");

    Router router;

    // Example route
    router.Get("/",
        [] (const HttpRequest& req, HttpResponse& res) {
            FileHandler::ReadFileIntoBody("static/index.html", res);
            res.headers["Content-Length"] = std::to_string(res.body.size());

            return;
        }
    );

    HandlerFunction function = 
        [] (const HttpRequest& req, HttpResponse& res) {

            std::string finalString{};
            for (const auto& routeParam : req.routeParams) {
                finalString += std::format(
                    "<div style='color: white;'>{} : {}</div>",
                    routeParam.first, routeParam.second
                );
            }

            res.body = std::format(
                "<!DOCTYPE html>"
                "<html lang='en'>"
                    "<head>"
                        "<meta charset='UTF-8' />"
                        "<meta name='viewport' content='width=device-width, initial-scale=1.0' />"
                        "<title>Index Page</title>"
                    "</head>"
                    "<body style='background-color: black'>"
                        "<h1 align='center'>"
                            "<div style='color: white;'>Hello World</div>"
                            "{}"
                        "</h1>"
                    "</body>"
                "</html>",
                finalString
            );

            res.headers["Content-Length"] = std::to_string(res.body.size());
            res.headers["Content-Type"] = "text/html";
            return;
        };

    for (const std::string& url : urls) {
        router.Post(url, function);
        router.Get(url, function);
        router.Head(url, function);
        router.Put(url, function);
        router.Delete(url, function);
        router.Connect(url, function);
        router.Options(url, function);
        router.Trace(url, function);
        router.Patch(url, function);
    }

    HttpServer server(config, router);

    server.AddErrorRoute(404, [] (const HttpRequest& req, HttpResponse& res) {
        FileHandler::ReadFileIntoBody("static/not-found.html", res);
        res.SetStatus(404);
        return;
    });

    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}
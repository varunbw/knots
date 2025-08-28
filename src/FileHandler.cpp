#include <fstream>

#include "FileHandler.hpp"
#include "Utils.hpp"

/*
    @brief Load file `fileName` from disk and build an HTTP response with the file contents as body
    @param statusCode The HTTP response's status code
    @param fileName The file to load

    @return A `HttpResponse` object with the file contents as the body
*/
HttpResponse FileHandler::MakeHttpResponseFromFile(const int statusCode, const std::string& fileName) {

    std::ifstream infile(fileName, std::ios::binary | std::ios::ate);

    if (infile.is_open() == false) {
        Log::Error(std::format(
            "MakeHttpResponseFromFile(): Could not open file {}",
            fileName
        ));
        return {};
    }

    const auto fileSize = infile.tellg();
    infile.seekg(0);

    std::string responseBody(fileSize, 0);
    infile.read(responseBody.data(), fileSize);

    HttpResponse res;
    res.SetStatus(200);
    res.body = std::move(responseBody);

    return res;
}
#include <fstream>

#include "FileHandler.hpp"
#include "Utils.hpp"

/*
    ? deprecate?
    @brief Load file `fileName` from disk and build an HTTP response with the file contents as body
    @param statusCode The HTTP response's status code
    @param fileName The file to load

    @return A `HttpResponse` object with the file contents as the body
*/
HttpResponse FileHandler::MakeHttpResponseFromFile(const std::string& fileName) {

    std::ifstream infile(fileName, std::ios::binary | std::ios::ate);

    if (infile.is_open() == false) {
        Log::Error(std::format(
            "MakeHttpResponseFromFile(): Could not open file {}",
            fileName
        ));

        HttpResponse res;
        res.SetStatus(404);
        return res;
    }

    const auto fileSize = infile.tellg();
    infile.seekg(0);

    std::string responseBody(fileSize, 0);
    infile.read(responseBody.data(), fileSize);

    HttpResponse res;
    // todo Remove
    res.SetStatus(200);
    res.body = std::move(responseBody);

    return res;
}

/*
    @brief Read contents of file with path `filePath` into `res.body`
    @param filePath File path to load
    @param res `HttpResponse` object to load file into
*/
void FileHandler::ReadFileIntoBody(const std::string& filePath, HttpResponse& res) {

    std::ifstream infile(filePath, std::ios::binary | std::ios::ate);

    if (infile.is_open() == false) {
        Log::Error(std::format(
            "ReadFileIntoBody(): Could not open file {}",
            filePath
        ));

        res.SetStatus(404);
        return;
    }
    
    const auto fileSize = infile.tellg();
    infile.seekg(0);

    std::string responseBody(fileSize, 0);
    infile.read(responseBody.data(), fileSize);

    // todo Remove
    res.SetStatus(200);
    res.body = std::move(responseBody);
    res.headers["Content-Length"] = std::to_string(res.body.size());
    return;
}
// #define WEBGET_DEBUG

#include "socket.hh"
#include "util.hh"

#include <cstdlib>
#include <iostream>

using namespace std;

void get_URL(const string &host, const string &path) {
    // Your code here.

    // You will need to connect to the "http" service on
    // the computer whose name is in the "host" string,
    // then request the URL path given in the "path" string.

    // Then you'll need to print out everything the server sends back,
    // (not just one call to read() -- everything) until you reach
    // the "eof" (end of file).

    // cerr << "Function called: get_URL(" << host << ", " << path << ").\n";
    // cerr << "Warning: get_URL() has not been implemented yet.\n";

    // Trying to reimply the example
    auto addr = Address(host, "http");
#ifdef WEBGET_DEBUG
    cout << "ip: " << addr.ip() << endl;
#endif
    TCPSocket sock2;
    sock2.connect(addr);

    // Packet HTTP datagram
    std::string http_get_req = std::string("GET ") + path + std::string(" HTTP/1.1\r\n");
    sock2.write(http_get_req);

    std::string http_req = std::string("Host: ") + host + std::string("\r\n");
    sock2.write(http_req);
    sock2.write("Connection: close\r\n");
    sock2.write("\r\n");
    std::string resp;
    while (!sock2.eof()) {
        resp = sock2.read();
        cout << resp;
    }
    sock2.shutdown(SHUT_RDWR);
}

int main(int argc, char *argv[]) {
    try {
        if (argc <= 0) {
            abort();  // For sticklers: don't try to access argv[0] if argc <= 0.
        }

        // The program takes two command-line arguments: the hostname and "path" part of the URL.
        // Print the usage message unless there are these two arguments (plus the program name
        // itself, so arg count = 3 in total).
        if (argc != 3) {
            cerr << "Usage: " << argv[0] << " HOST PATH\n";
            cerr << "\tExample: " << argv[0] << " stanford.edu /class/cs144\n";
            return EXIT_FAILURE;
        }

        // Get the command-line arguments.
        const string host = argv[1];
        const string path = argv[2];

#ifdef WEBGET_DEBUG
    cout << "host: " << host << endl;
    cout << "path: " << path << endl;
#endif
        // Call the student-written function.
        get_URL(host, path);
    } catch (const exception &e) {
        cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

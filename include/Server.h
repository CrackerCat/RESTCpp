#pragma once

#ifdef _WIN32
    #include <WinSock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/ip.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <cerrno>
    #include <poll.h>
#endif

#include <functional>
#include <thread>
#include <ctime>
#include <fstream>
#include <string>
#include <tuple>
#include <cstring>

#include "Common.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "ThreadPool.h"

namespace restcpp
{
    using std::function;
    using std::unique_ptr;
    using std::thread;
    using std::time;
    using std::ctime;
    using std::ifstream;
    using std::stringstream;
    using std::memcpy;
    using std::memset;

    class Server
    {
        public:
            Server(uint16_t port = 8080) : mPort(port) { init(); };
            void fRun();
            void fStop();
            void fSetLogging(bool value) { mLog = value; };
            class Router
            {
                public:
                    void fAddRoute(const string& URLPath, const METHOD& method, const function<void(const HTTPRequest&,HTTPResponse&)>& callBack) { mRoutes.push_back(std::make_tuple(URLPath, method, callBack)); };
                    void fAddStaticRoute(string URLPath, string folderPath) { mStaticRoutes[URLPath] = folderPath; };
                    const unordered_map<string,string>& fGetStaticRoutes() const { return mStaticRoutes; };
                    const vector<std::tuple<string,METHOD,function<void(const HTTPRequest&,HTTPResponse&)>>>& fGetDefinedRoutes() const { return mRoutes; }; 
                    friend class HTTPServer;
                private:
                    unordered_map<string,string> mStaticRoutes;
                    vector<std::tuple<string,METHOD,function<void(const HTTPRequest&,HTTPResponse&)>>> mRoutes;
            };
            void fConnectRouter(Router* router) { mRouter = router; };
        private:
            uint64_t mSock,mAcceptSocket;
            struct sockaddr_in mServerAddr,mClientAddr;
            bool mLog; 
            uint16_t mPort;
            Router* mRouter;
            vector<thread> mClientThreads;
            void init();
            void fOnRequest(uint64_t socket);
            const string fRecieveNext(uint64_t socket);
            std::shared_ptr<HTTPResponse> fProcessRequest(const string& rawData);
            void fSendResponse(std::shared_ptr<HTTPResponse>& response,const uint64_t& sock);
    };

}
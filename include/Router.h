#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <regex>

#include "Common.h"




namespace restcpp
{

    class Server;
    class HTTPRequest;
    class HTTPResponse;

    class Router
    {
        public:
            struct pathParam
            {
                string m_path;
                string m_name;
                char m_endChar = 0;
            };
            struct route
            {
                route(const string& URLPath, const METHOD& method, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack)
                {
                    m_URLPath = URLPath;
                    m_method = method;
                    m_callBack = callBack;
                    parsePathParams(URLPath,m_pathParams);
                }
                string m_URLPath;
                METHOD m_method;
                std::function<void(const HTTPRequest&,HTTPResponse&)> m_callBack; 
                std::vector<pathParam> m_pathParams;
            };

            void addRoute(const string& URLPath, const METHOD& method, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack) { m_routes.push_back(route(URLPath,method,callBack)); };
            void addStaticRoute(string URLPath, string folderPath) { m_staticRoutes[URLPath] = folderPath; };
            const std::unordered_map<string, string>& getStaticRoutes() const { return m_staticRoutes; };
            const std::vector<route>& getDefinedRoutes() const { return m_routes; }; 
            friend class Server;
            friend class HTTPRequest;
        private:
            static inline void parsePathParams(string path, std::vector<pathParam>& params,size_t lastPos = 0)     
            {
                if(!std::regex_search(path,std::regex("\\{([^}]+)\\}")))
                    return;
                auto pos = path.find_first_of('{',lastPos);
                auto endPos = path.find_first_of('}',pos);
                if(endPos < pos)
                    return;
                auto name = path.substr(pos + 1,endPos - pos - 1);
                auto URLPath = path.substr(0, pos);
                if(path.length() <= endPos + 1)
                    params.push_back({URLPath, name, 0});
                else
                    params.push_back({URLPath, name, path[endPos + 1]});
                path.erase(pos + 1,endPos - pos - 1);
                parsePathParams(path, params,endPos - name.length());
            };
            std::unordered_map<string, string> m_staticRoutes;
            std::vector<route> m_routes;
    };

}


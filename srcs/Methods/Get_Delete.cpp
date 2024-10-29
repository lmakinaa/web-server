
#include "Methods.hpp"

std::string sessionIdGen(Server &Serv)
{
    std::string session_id;
    std::string elems = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    srand(time(NULL));
    while (true)
    {
        session_id = "";

        for (int i = 0; i < 20; i++)
        {
            session_id += elems[rand() % 62];
        }
        if (Serv.session_ids.find(session_id) == Serv.session_ids.end())
        {
            Serv.session_ids[session_id] = time(0);
            break ;
        }
    }

    return (session_id);
    
}


int isDirectory(const std::string& path) {
    struct stat info;
    
    // Check if the path exists and get its information
    if (stat(path.c_str(), &info) != 0) {
        // Error accessing the path
        return -1; // Path does not exist
    } else {
        // Check if the path is a directory
        return (info.st_mode & S_IFDIR) != 0; // Return true if it's a directory
    }
}


bool    fileExist(std::string &path)
{
    if (access(path.c_str(), F_OK) == 0)
        return (1);
    return (0);
}


void    listAllfiles(std::string path)
{
    std::string file = "<!DOCTYPE html>\n"
                       "<html lang=\"en\">\n"
                       "<head>\n"
                       "    <meta charset=\"UTF-8\">\n"
                       "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                       "    <title>Directory Listing</title>\n"
                       "    <style>\n"
                       "        body {\n"
                       "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
                       "            background-color: #f0f0f0;\n"
                       "            margin: 0;\n"
                       "            padding: 0;\n"
                       "        }\n"
                       "        header {\n"
                       "            background-color: #4CAF50;\n"
                       "            color: white;\n"
                       "            padding: 20px;\n"
                       "            text-align: center;\n"
                       "            font-size: 24px;\n"
                       "        }\n"
                       "        .container {\n"
                       "            max-width: 800px;\n"
                       "            margin: 30px auto;\n"
                       "            padding: 20px;\n"
                       "            background-color: #fff;\n"
                       "            border-radius: 10px;\n"
                       "            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);\n"
                       "        }\n"
                       "        ul {\n"
                       "            list-style-type: none;\n"
                       "            padding: 0;\n"
                       "            margin: 0;\n"
                       "        }\n"
                       "        li {\n"
                       "            padding: 12px;\n"
                       "            border-bottom: 1px solid #eee;\n"
                       "            display: flex;\n"
                       "            justify-content: space-between;\n"
                       "            align-items: center;\n"
                       "        }\n"
                       "        li:last-child {\n"
                       "            border-bottom: none;\n"
                       "        }\n"
                       "        a {\n"
                       "            color: #4CAF50;\n"
                       "            text-decoration: none;\n"
                       "            font-weight: bold;\n"
                       "        }\n"
                       "        a:hover {\n"
                       "            text-decoration: underline;\n"
                       "        }\n"
                       "        .file-info {\n"
                       "            color: #555;\n"
                       "            font-size: 14px;\n"
                       "        }\n"
                       "        .icon {\n"
                       "            font-size: 20px;\n"
                       "            margin-right: 10px;\n"
                       "        }\n"
                       "        .file {\n"
                       "            display: flex;\n"
                       "            align-items: center;\n"
                       "        }\n"
                       "    </style>\n"
                       "</head>\n"
                       "<body>\n"
                       "    <header>\n"
                       "        Directory Listing\n"
                       "    </header>\n"
                       "    <div class=\"container\">\n"
                       "        <ul>\n";


    DIR* dir;
    struct dirent* entry;

    // Open the current directory
    dir = opendir(path.c_str());
    if (dir == NULL) {
        std::cerr << "Error: Could not open current directory" << std::endl;
        return ;
    }


    while ((entry = readdir(dir)) != NULL) {

            if (entry->d_type == DT_DIR) {
                file += "<li>"
                    "<div class=\"file\">"
                    "<span class=\"icon\">📁</span>";

                file += "<a href=\"" + path + "/" + entry->d_name + "\">" + entry->d_name + "</a>";

                file += "</div>"
                "</li>";
            } else {
                file += "<li>"
                    "<div class=\"file\">"
                    "<span class=\"icon\">📄</span>";

                file += "<a href=\"" + path + "/" + entry->d_name + "\">" + entry->d_name + "</a>";

                file += "</div>"
                "</li>";
            }
    }

    closedir(dir);


    file += "</ul>"
        "</div>"
        "</body>"
        "</html>";

    std::ofstream ofs("test.html");

    ofs << file;
    ofs.close();

}

std::string getFileFullPath(Server &serv, std::map<std::string, Location>::iterator &it, std::string &requestPath)
{
    std::string root = "";
    std::string path = "";
    std::string _Method = "GET";
    std::string sec_path = "";

    /* ===== Location doesn't have a root directive ====*/
    if (it->second.directives.find("root") == it->second.directives.end())
    {
        root = serv.directives["root"].values[0];

        path = requestPath;

        path.replace(0, it->first.size(), root);

        int val = isDirectory(path);
        if (val == -1)
        {
            // throw 404NotFoundClass;
            std::cout << "0\n";
            return "";
        }
        else if (val == 1)
        {

            if (_Method == "DELETE")
            {
                // throw 403 Forbidden
                std::cout << "403 Forbidden" << std::endl;
                return "";
            }
            sec_path = path;
            for (size_t i = 0; i < serv.directives["index"].values.size(); i++)
            {
                path +=  "/" + serv.directives["index"].values[i];
                if (fileExist(path))
                    return (path);
            }
            if (serv.directives.find("return") != serv.directives.end())
            {
                // should redirect to ...
                std::cout << "redirect to " << serv.directives["return"].values[0] << std::endl;
            }
            else if (serv.directives.find("autoindex") != serv.directives.end()
                        &&  serv.directives["autoindex"].values[0] == "on")
            {
                // should list all files
                std::cout << "list all files\n";
                listAllfiles(sec_path);
                return "";
            }
            else
            {
                // throw 403ForbiddenClass;
                std::cout << "1\n";
                return "";
            }
        }
        else if (val == 0)
        {
            if (fileExist(path))
                return (path);
            else
            {
                // throw 404NotFoundClass;
                std::cout << "2\n";
                return "";
            }
        }

    }
    else
    {
        root = it->second.directives["root"].values[0];

        path = requestPath;

        path.replace(0, it->first.size(), root);

        int val = isDirectory(path);
        if (val == -1)
        {
            // throw 404NotFoundClass;
            std::cout << "3\n";
            return "";
        }
        else if (val == 1)
        {

            if (_Method == "DELETE")
            {
                // throw 403 Forbidden
                std::cout << "403 Forbidden" << std::endl;
                return "";
            }
            sec_path = path;
            for (size_t i = 0; i < it->second.directives["index"].values.size(); i++)
            {
                path += "/" +  it->second.directives["index"].values[i];
                if (fileExist(path))
                    return (path);
            }
            if (it->second.directives.find("return") != it->second.directives.end())
            {
                // should redirect to ...
                std::cout << "redirect to " << it->second.directives["return"].values[0] << std::endl;
            }
            else if (it->second.directives.find("autoindex") != it->second.directives.end()
                        &&  it->second.directives["autoindex"].values[0] == "on")
            {
                // should list all files
                std::cout << "list all files\n";
                listAllfiles(sec_path);
                return "";
            }
            else
            {
                // throw 403ForbiddenClass;
                std::cout << "4\n";
                return "";
            }
        }
        else if (val == 0)
        {
            if (fileExist(path))
                return (path);
            else
            {
                // throw 404NotFoundClass;
                std::cout << "5\n";
                return "";
            }
        }

    }

    return (path);
}


bool    stringMaching(std::string locat , std::string &requestPath)
{

    if (requestPath.find(locat) == 0)
        return (1);
    return (0);
}

std::string    _GET_DELETE(WebServ &main)
{
    Server serv = main.servers[0];
    std::string requestPath = "/srcs/hello/main.hpp";
    std::string resquestedFile = "";
    std::string line;
    std::string response = "";
    std::string _Method = "GET";

    std::map<std::string, Location>::iterator it2 = serv.locations.begin();
    std::map<std::string, Location>::iterator it = serv.locations.end();
    

    /* ======= Looking for the request path in server locations ======= */
    for ( ; it2 != serv.locations.end(); it2++)
    {
        if (stringMaching((*it2).first , requestPath))
        {
            if (it == serv.locations.end() || it2->first.size() >= it->first.size())
                it = it2;
            
        }
    }


    /* ======= Found the matching path ======= */
    if (it != serv.locations.end())
    {
        resquestedFile = getFileFullPath(serv, it, requestPath);
        if (resquestedFile == "")
        {
            // throw 404NotFoundClass;
            std::cout << "404 NOt found" << std::endl;
            return "";
        }

        /* ===== Check Read Permession ===== */
        if (access(resquestedFile.c_str(), R_OK) != 0)
        {
            // Throw 403 Forbidden
            std::cout << "403 Forbidden" << std::endl;
            return "";
        }
    }
    else
    {
        // throw 404NotFoundClass;
        std::cout << "404 NOt found" << std::endl;
    }

    if (_Method == "DELETE")
    {
        // unlink(resquestedFile.c_str());
        // throw 04 No Content
    }

    // Send the file to the Client.
    std::cout << "Result : " << resquestedFile << std::endl;
    return (resquestedFile);
}
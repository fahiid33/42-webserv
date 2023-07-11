#include "../includes/cgi.hpp"
#include "../includes/server.hpp"

Cgi::Cgi() // should be Cgi(server)
{
}


Cgi::Cgi(std::string path, std::string ext)
{
    this->_cgi = std::make_pair(path, ext);
}

Cgi::~Cgi()
{
}

void Cgi::initEnv(Request const & req, std::string const & server_name, std::string const & root)
{

    if (req.getHeaders().find("Content-Length") != req.getHeaders().end()) {
        this->env["CONTENT_LENGTH"] = req.getHeaders().find("Content-Length")->second;
    } else {
        this->env["CONTENT_LENGTH"] = std::to_string(req.getBody().size());
    }
    if (req.getHeaders().find("User-Agent") != req.getHeaders().end())
        this->env["HTTP_USER_AGENT"] = req.getHeaders().find("User-Agent")->second;
    if (req.getHeaders().find("Content-Type") != req.getHeaders().end())
        this->env["CONTENT_TYPE"] = req.getHeaders().find("Content-Type")->second;; // application/x-www-form-urlencoded or multipart/form-data, etc.
    this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
    if (req.getHeaders().find("Cookie") != req.getHeaders().end())
        this->env["HTTP_COOKIE"] = req.getHeaders().find("Cookie")->second;
    this->env["REQUEST_METHOD"] = req.getMethod(); // GET, POST, HEAD
    this->env["SERVER_NAME"] = server_name; // kayna fl config, normally server object
    this->env["SCRIPT_NAME"] = req.getPath() + req.getFile(); // The name of the CGI script.
    this->env["PATH_INFO"] = req.getPath() + req.getFile(); // The name of the CGI script.
    this->env["REQUEST_URI"] = req.getPath() + req.getFile(); // The name of the CGI script.
    this->env["DOCUMENT_URI"] = req.getPath() + req.getFile(); // The name of the CGI script.
    this->env["DOCUMENT_ROOT"] = root; // The name of the CGI script.
    this->env["QUERY_STRING"] = req.getQuery(); // url mor ? 
    this->env["SCRIPT_FILENAME"] = root + req.getPath() + req.getFile(); // The full path to the CGI script.
    this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->env["SERVER_SOFTWARE"] = "webserv";
}

void Cgi::print_cgi()
{
    std::cout << "CGI: " << this->_cgi.first << " " << this->_cgi.second << std::endl;
}

std::pair<std::string, std::string> const & Cgi::get_Cgi() const
{
    return this->_cgi;
}

char **Cgi::getEnv()
{
    int i = 0;
    this->_env = new char*[this->env.size() + 1];
    for (std::map<std::string, std::string>::iterator it = this->env.begin(); it != this->env.end(); it++)
    {
        this->_env[i] = new char[it->first.length() + it->second.length() + 2];
        strcpy(this->_env[i], (it->first + "=" + it->second).c_str());
        i++;
    }
    this->_env[i] = NULL;
    return this->_env;
}

void Cgi::set_Cgi(std::pair<std::string, std::string> const &cgi)
{
    this->_cgi = cgi;
}

void Cgi::clear()
{
    this->_cgi.first.clear();
    this->_cgi.second.clear();
}

void Cgi::runCgi(Request &req, Location &loc, Response &_resp)
{
    
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    std::stringstream output;
    ssize_t bytesRead;
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
    int fdin[2];
    int fdout[2];
    pid_t child_pid;
    int status;
    int fdtmp = dup(0);
    // execute cgi
    this->initEnv(req, "localhost", loc.getRoot());
    std::cout << "Init Env" << std::endl;
    pipe(fdin);
    pipe(fdout);
    child_pid = fork();
    if (child_pid < 0)
        exit(1);
    if (child_pid == 0) {
        dup2(fdout[1], STDOUT_FILENO);
        dup2(fdin[0], STDIN_FILENO);
        close(fdout[0]);
        close(fdin[1]);
        char *av[3];
        av[0] = strdup(this->get_Cgi().first.c_str());
        av[1] = strdup(request_resource.c_str());
        av[2] = NULL;
        if (execve(av[0], av, this->getEnv()) == -1)
        {
            
            exit(1);
        }
    }
    close(fdout[1]);
    close(fdin[0]);
    size_t rc = write(fdin[1], req.getBody().data(), req.getBody().size());
    if (rc != req.getBody().size()) {
        perror("error write: ");
        exit(1);
    }
    close(fdin[1]);
    while ((bytesRead = read(fdout[0], buffer, BUFFER_SIZE)) > 0)
        output.write(buffer, bytesRead);
    close (fdout[0]);
    wait (&status);
    dup2(fdtmp, 0);
    close(fdtmp);
    if (status != 0)
    {
        _resp.generateErrorPage(500, loc);
        return ;
    }
    _resp.setHeader("Status", "200 OK");
    _resp.setHeader("Content-Type", "text/html; charset=UTF-8");
    _resp.setHeader("Content-Length", std::to_string(output.str().length()));
    _resp.setResp(std::make_pair(output.str(), output.str().length()));
}
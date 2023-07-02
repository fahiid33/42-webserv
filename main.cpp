#include "./includes/webserv.hpp"
#include "./includes/parse_conf.hpp"


int main(int ac, char **av)
{
    (void)av;
    MultiPlexing mp;
    signal(SIGPIPE, SIG_IGN);
    if (ac != 2 && ac != 1)
    {
        std::cout << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }
    else if (ac == 2)
    {
        Config conf(av[1]);
        conf.parse_config();
        // mp.setup_server(conf.getServers());
        // create_socket();
    }
    else if (ac == 1)
    {
        Config conf;
        conf.parse_config();
        for(auto it = conf.getServers().begin(); it != conf.getServers().end(); it++)
        {
            std::cout << "port " << it->first << std::endl;
            for (size_t i = 0; i < it->second.size(); i++)
            {
                std::cout << "server " << i << std::endl;
                it->second[i].print_server();
            }
        }
        // mp.setup_server(conf.getServers());
        // cgi cc;
        // mp.setup_server(conf.getServers());
        
    }
    return 0;
}
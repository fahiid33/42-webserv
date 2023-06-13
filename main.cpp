// #include "parse_conf.hpp"
#include "server.hpp"


int main(int ac, char **av)
{
    (void)av;
   
    signal(SIGPIPE, SIG_IGN);
    if (ac != 2 && ac != 1)
    {
        std::cout << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }
    else if (ac == 2)
    {
        // Config conf(av[1]);
        // conf.parse_config();
        // for (size_t i = 0; i < conf.getServers().size(); i++)
        // {
        //     std::cout << "++++++++++++++++++++++++++" "server " << i << " : ++++++++++++++++++++++++++"  << std::endl;
        //     conf.getServers()[i].print_server();
        // }
        create_socket();
    }
    else if (ac == 1)
    {
        // Config conf("conf/default.conf");
        // conf.parse_config();
        // for (size_t i = 0; i < conf.getServers().size(); i++)
        // {
        //     std::cout << "++++++++++++++++++++++++++" "server " << i << " : ++++++++++++++++++++++++++"  << std::endl;
        //     conf.getServers()[i].print_server();
        // }
        create_socket();
    }
    return 0;
}

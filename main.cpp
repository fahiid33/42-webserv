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
        mp.setup_server();
        // Config conf(av[1]);
        // conf.parse_config();
        // create_socket();
    }
    else if (ac == 1)
    {
        mp.setup_server();
        // Config conf;
        // conf.parse_config();
        // for(int i =0; i < conf.getServers().size(); i++)
        // {
        //     conf.getServers()[i].print_server();
        // }
        // create_socket();
    }
    return 0;
}
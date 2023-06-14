#include "webserv.hpp"

int main(int ac, char **av)
{
    (void)av;
    MultiPlexing mp;
    // Config conf;
    signal(SIGPIPE, SIG_IGN);
    if (ac != 2 && ac != 1)
    {
        std::cout << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }
    else if (ac == 2)
    {
        mp.setup_server();
        // conf.Config(av[1]);
        // conf.parse_config();
        // create_socket();
    }
    else if (ac == 1)
    {
        mp.setup_server();
        // conf.Config();
        // create_socket();
    }
    return 0;
}
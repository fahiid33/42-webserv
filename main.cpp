#include "parse_conf.hpp"
#include "server.hpp"

int main(int ac, char **av)
{
    (void)av;
    // Config conf;
    if (ac != 2 && ac != 1)
    {
        std::cout << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }
    else if (ac == 2)
    {
        // conf.Config(av[1]);
        // conf.parse_config();
        create_socket();
    }
    else if (ac == 1)
    {
        // conf.Config();
        create_socket();
    }
    return 0;
}
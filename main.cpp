#include "./includes/webserv.hpp"
#include "./includes/parse_conf.hpp"


int main(int ac, char **av)
{
    (void)av;
    MultiPlexing mp;
    signal(SIGPIPE, SIG_IGN);
    Config conf;
    if (ac != 2 && ac != 1)
    {
        std::cout << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }
    else if (ac == 2) {
        Config conf(av[1]);
    }
    try {
        conf.parse_config();
        mp.setup_server(conf.getServers());
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
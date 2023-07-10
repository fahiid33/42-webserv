#!/usr/bin/perl
# FILEPATH: /Users/hlachkar/webserv/cgi-bin/cgi_get.pl
use strict;
use warnings;
use CGI;
use CGI::Carp qw(fatalsToBrowser);
use CGI::Cookie;


use CGI;
use CGI::Cookie;

my $cgi = CGI->new;
my $cookie_string = $ENV{HTTP_COOKIE};
my $first_name = $cgi->param('first_name');
my $last_name = $cgi->param('last_name');

print $cgi->header(-type=>'text/html', -charset=>'utf-8');
print "<html>";
print "<head>";
print "<title>Hello - Second CGI Program</title>";
print "</head>";
print "<body>";
print "<h2>Hello $first_name $last_name</h2>";
if ($cookie_string) {
    my $cookie = CGI::Cookie->parse($cookie_string);
    if (exists $cookie->{'lala'}) {
        my $lala_value = $cookie->{'lala'}->value;
        print "Value of cookie 'lala': $lala_value";
    } else {
        print "Cookie 'lala' does not exist";
    }
}
print "</body>";
print "</html>";
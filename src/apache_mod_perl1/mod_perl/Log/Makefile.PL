use ExtUtils::MakeMaker;

use lib qw(../lib);
use Apache::src ();
my $src = Apache::src->new;

WriteMakefile(
    NAME	=> "Apache::Log",
    VERSION_FROM => "Log.pm",
    'INC'	=> $src->inc,
    'TYPEMAPS'  => $src->typemaps,
);


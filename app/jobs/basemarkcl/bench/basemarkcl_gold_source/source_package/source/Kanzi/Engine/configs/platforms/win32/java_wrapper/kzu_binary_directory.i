%{
#include <user/binary/kzu_binary_directory.h>
%}

%nodefaultctor KzuBinaryDirectory;
%nodefaultdtor KzuBinaryDirectory;

struct KzuBinaryDirectory {};

%include <user/binary/kzu_binary_base.h>
%include <user/binary/kzu_binary_directory.h>

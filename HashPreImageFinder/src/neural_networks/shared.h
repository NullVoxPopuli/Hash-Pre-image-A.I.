#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>


#include "fann.h"
#include "floatfann.h"


#include "../Config.h"

void print_config();
void save_to_folder(struct fann * network, const char * save_name);

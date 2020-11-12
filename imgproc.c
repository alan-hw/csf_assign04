#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include "pnglite.h"
#include "image.h"

struct Plugin {
    void *handle;
    const char *(*get_plugin_name)(void);
    const char *(*get_plugin_desc)(void);
    void *(*parse_arguments)(int num_args, char *args[]);
    struct Image *(*transform_image)(struct Image *source, void *arg_data);
};



void printUsageInfo(){
    printf("Usage: imgproc <command> [<command args...>]\nCommands are:\n  list\n  exec <plugin> <input img> <output img> [<plugin args...>]\n");
}

char** get_new_argv(char **argv){
    int length_of_argv = sizeof(argv) / sizeof(argv[0]);
    if(length_of_argv == 5) {
        return NULL;
    }
    char** new_argv = malloc(sizeof(argv[0]) * (length_of_argv - 5)); // remember to free!!
    for (int i = 5; i < (int) (sizeof(argv) / sizeof(argv[0])); i++){
        new_argv[i - 5] = argv[i];
    }
    return new_argv;
}

int main(int argc, char* argv[]){
    const char* plugin_dir_name;
    struct dirent *entry;
    if ((plugin_dir_name = getenv("PLUGIN_DIR")) == NULL) {
        plugin_dir_name = "./plugins";
    }
    
    
    DIR *plugin_dir = opendir(plugin_dir_name);
    if(plugin_dir == NULL) {
        printf("Error: Unable to read directory\n");
        return 1;
    }
    int counter = 0;
    char *name;
    char plugin_location[100];
    strcat(plugin_location, "./plugins/");
    struct Plugin plugin;
    struct Plugin* plugins = calloc(1, 4 * sizeof(struct Plugin));
    
    while((entry=readdir(plugin_dir)) != NULL) {

        if ( !strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") )
        {
            // do nothing (straight logic)
        } else {
	  name = entry->d_name;
	  if (name[strlen(name)-3] == '.' && name[strlen(name)-2] == 's' && name[strlen(name)-1] == 'o'){
	    char temp_path[100];
	    strcpy(temp_path,plugin_location);
            strcat(temp_path, name);
            //printf("the string is %s\n", plugin_location);
            plugin.handle = dlopen(temp_path, RTLD_LAZY);
            if (!plugin.handle) {
                printf("an image plugin can't be loaded\n");
                return 1;
            }
            *(void **)(&plugin.get_plugin_name) = dlsym(plugin.handle, "get_plugin_name");
            *(void **)(&plugin.get_plugin_desc) = dlsym(plugin.handle, "get_plugin_desc");
            *(void **)(&plugin.parse_arguments) = dlsym(plugin.handle, "parse_arguments");
            *(void **)(&plugin.transform_image) = dlsym(plugin.handle, "transform_image");
            plugins[counter] = plugin;
            counter++;
	  }
	}
    }
    if(plugin_dir!=NULL){ 
      closedir(plugin_dir);
    }
    if (argc == 2 && !strcmp(argv[1], "list")) {
        printf("Loaded %d plugin(s)\n", counter);
        for (int i = 0; i < counter; i++) {
	  //printf("%s: %s\n", (plugins[i].get_plugin_name()), (plugins[i].get_plugin_desc()));
            printf("%s: %s\n", plugins[0].get_plugin_name(), plugins[0].get_plugin_desc());

        }
    } else if (argc >= 5 && !strcmp(argv[1], "exec")) {
        char** new_argv = get_new_argv(argv);
        for (int i = 0; i < counter; i++) {
            if(!strcmp(argv[2], plugins[i].get_plugin_name())) {
                
                struct Image* input_image;
                struct Image* output_image;
                if((input_image = img_read_png(argv[3])) == NULL) {
                    printf("Error: Invalid name or format of input image.\n");
                    printUsageInfo(); // distinguish between
                    return 1;
                }
                void* arguments = plugins[i].parse_arguments(argc - 5, new_argv);
                if ((output_image = plugins[i].transform_image(input_image, arguments)) == NULL){
                    printf("Error: Invalid format of input image.\n");
                    return 1;
                }
                if (img_write_png(output_image, argv[4]) == 0) {
                    printf("Error: Write Failure.\n");
                    return 1;
                };
                free(new_argv);
                return 0;
            } else{
                    printf("Error: Invalid name of plugin.\n");
                    printUsageInfo();
                    return 1;
                }
            }
        } else{
        printf("Error: Invalid format of command line arguments.\n");
        printUsageInfo();
        return 1;
    }
    //dlclose(plugins[0].handle);
}



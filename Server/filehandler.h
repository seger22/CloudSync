#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include<stdio.h>
#include<boost/asio.hpp>
#include<boost/filesystem.hpp>
#include<iostream>
#include<fstream>

using namespace std;

class filehandler
{
public:
    filehandler();
    std::ofstream get_output_stream(string path,string name);
    void create_directory(string path, string dir_name);
    void delete_directory(string path, string dir_name);
    void move_directory(string old_path, string new_path);
    void rename_file(string filepath, string old_name, string new_name);
    void delete_file(string path, string file_name);
    void move_file(string old_path,string new_path, string file_name);
    void rename_directory(string path,string old_name, string new_name);

private:
    string watch_dir;
};

#endif // FILEHANDLER_H

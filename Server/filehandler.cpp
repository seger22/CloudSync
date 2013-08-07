#include "filehandler.h"

filehandler::filehandler()
{
    watch_dir="/home/satthy/Server";
}

void filehandler::create_directory(string path, string dir_name)
{
    std::string dir_pos= watch_dir+path+dir_name;
    boost::filesystem3::path dir(dir_pos);
    if(boost::filesystem3::create_directory(dir))
        cout<<"Created directory successfully"<<endl;
    else
        cout<<"Error creating directory"<<endl;
}

void filehandler::delete_directory(string path, string dir_name)
{
    std::string dir_pos=  watch_dir+path+dir_name;
    boost::filesystem3::path dir(dir_pos);
    boost::filesystem3::remove_all(dir);
}

void filehandler::move_directory(string old_path, string new_path)
{
    std::string old_dir= watch_dir+old_path;
    std::string new_dir=watch_dir+new_path;
    if(rename(old_dir.c_str(),new_dir.c_str())==0){
        cout<<"Directory moving success!!"<<endl;
    }
    else
       ;// cout<<"Directory moving failed"<<endl;
}

void filehandler::rename_directory(string path, string old_name, string new_name){
    string olddir=path+old_name;
    string newdir=path+new_name;
    this->move_directory(olddir,newdir);
}

void filehandler::delete_file(string path, string file_name)
{
    std::cout<<"File deleting "<<std::endl;
    std::string file_del=watch_dir+path+file_name;
    std::cout<<file_del<<std::endl;
    if( remove(file_del.c_str()) != 0 )
        std::cout<< "Error deleting file"<<std::endl;
    else
      std::cout<<"File successfully deleted"<<std::endl;
}

void filehandler::rename_file(string filepath, string old_name, string new_name){
    std::string old_file_path= watch_dir+filepath+old_name;
    std::string new_file_path=watch_dir+filepath+new_name;
    int result = rename(old_file_path.c_str(),new_file_path.c_str());
    if(result==0)
        std::cout<<"Rename Success";
    else
        std::cout<<"Rename failed";
}

void filehandler::move_file(string old_path, string new_path, string file_name){
    std::string old_file= watch_dir+old_path+"/"+file_name;
    std::string new_file=watch_dir+new_path+"/"+file_name;
    std::cout<<new_file<<std::endl;
    if(rename(old_file.c_str(),new_file.c_str())==0)
        std::cout<<"File Successfully Moved";
    else
        std::cout<<"Move failed";
}



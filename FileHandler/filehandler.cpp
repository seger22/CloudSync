#include "filehandler.h"

DatabaseManager dbmanager;

filehandler::filehandler()
{
    //watch_dir="/home/mayuresan/Project/SyncDest";
    watch_dir=ConfigurationManager::getLocation();
}

void filehandler::create_directory(string path, string dir_name)
{
    string dir_pos= watch_dir+path+dir_name;
    try{
        boost::filesystem::path dir(dir_pos);
        if(boost::filesystem::create_directory(dir))
            cout<<"Directory created successfully"<<endl;
        else
            cout<<"Error creating directory"<<endl;
    }catch(boost::filesystem::filesystem_error &e){
        cout<<"Create File error: "<<e.what()<<endl;
    }
}

void filehandler::delete_directory(string path, string dir_name)
{
    string dir_pos= watch_dir+path+dir_name;
    try{
        boost::filesystem::path dir(dir_pos);
        boost::filesystem::remove_all(dir);
    }catch(boost::filesystem::filesystem_error &e){
        cout<<"Delete directory error: "<<e.what()<<endl;
    }
}

void filehandler::move_directory(string old_path, string new_path)
{
    string old_dir= watch_dir+old_path;
    string new_dir=watch_dir+new_path;
    if(rename(old_dir.c_str(),new_dir.c_str())==0){
        cout<<"Directory moved/renamed successfully!!"<<endl;
        dbmanager.moveDirectory((old_dir+"/").c_str(), (new_dir+"/").c_str());
    }
    else
        cout<<"Directory moving/renamed failed"<<endl;
}

void filehandler::rename_directory(string path, string old_name, string new_name){
    string olddir=path+old_name;
    string newdir=path+new_name;
    this->move_directory(olddir,newdir);        
}

void filehandler::delete_file(string path, string file_name)
{    
    string file_del=watch_dir+path+file_name;
    if( remove(file_del.c_str()) != 0 )
        std::cout<< "Error deleting file"<<std::endl;
    else{
        cout<<"File successfully deleted!!"<<endl;
        dbmanager.deleteFile(file_del.c_str());
    }
}

void filehandler::rename_file(string filepath, string old_name, string new_name){
    std::string old_file_path= watch_dir+filepath+old_name;
    std::string new_file_path=watch_dir+filepath+new_name;
    int result = rename(old_file_path.c_str(),new_file_path.c_str());
    if(result==0){
        std::cout<<"File successfully renamed!!\n";
        dbmanager.moveFile(old_file_path.c_str(), new_file_path.c_str());
    }
    else
        cout<<"File Rename failed\n";


}

void filehandler::move_file(string old_path, string new_path, string file_name){
    std::string old_file= watch_dir+old_path+file_name;
    std::string new_file=watch_dir+new_path+file_name;
    //std::cout<<new_file<<std::endl;
    int result=rename(old_file.c_str(),new_file.c_str());
    if(result==0){
        std::cout<<"File Successfully Moved\n";
         dbmanager.moveFile(old_file.c_str(), new_file.c_str());
    }
    else
        std::cout<<"Move failed\n";
}

bool filehandler::file_exists(string fullpath){
    struct stat buffer;
      return (stat ((fullpath).c_str(), &buffer) == 0);
}


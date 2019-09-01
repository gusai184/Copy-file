#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>  
#include<string.h>
#include<unistd.h>
#include<fstream>
#include<dirent.h>
#include<iostream>
#include <unistd.h>
#include <time.h>

using namespace std;
string findfileExetenstion(string str) 
{ 
    // Traverse from right 
    for (int i = str.length() - 1; i >= 0; i--) 
        if (str[i] == '.') 
            return str.substr(i+1,str.length()); 
  
    return str; 
} 
void copyfile(string file_path, string destination)
{
	cout<<"Source is "<< file_path<<endl;
	cout<<"Destination is "<<destination<<endl;

	int fd_source = open(file_path.c_str(),O_RDONLY);
	
	if(fd_source==-1 )
	{
		cout<<"Error while opeing file"<<endl;
		cout<<endl;	
		return;
	}

	static int fd_desti = open(destination.c_str(),O_RDWR | O_CREAT ,0777);
//BIG CHAGNE -------------------------------------------------------------
	char *buffer = (char *) malloc(2048);
		
	if(fd_desti==-1)
	{
		cout<<"Error while opeing file"<<endl;
		cout<<endl;	
		return;
	}
	
	int x = read(fd_source, buffer, 2048);
	while(x!=0)
	{
		write(fd_desti,buffer,x);	
		x = read(fd_source, buffer, 2048);
	}
	close(fd_source);
//	close(fd_desti);	
}

bool dontsplit = false;

void splitcopyfile(string file_path, string destination)
{
	cout<<"Source is "<< file_path<<endl;
	cout<<"Destination is "<<destination<<endl;
	cout<<endl;

	int fd_source = open(file_path.c_str(),O_RDONLY);
	
	if(fd_source==-1 )
	{
		cout<<"Error while opeing file"<<endl;
		return;
	}

	int fd_desti = open(destination.c_str(),O_CREAT | O_RDWR,0777);
	char *buffer = (char *) malloc(2048);
	string extension = findfileExetenstion(file_path);
	
	if(fd_desti==-1)
	{
		cout<<"Error while opeing file"<<endl;
		return;
	}

	long long int x = read(fd_source, buffer, 2048),written_bytes=0,no=0;
	while(x>0)
	{
		
//		cout<<"writtern bytes : "<<written_bytes<<endl;
		if(dontsplit == false && written_bytes>=200*1024*1024)
		{
			close(fd_desti);

			string newfile = destination+"_" + to_string(no++) +"."+ extension;
			fd_desti = open(newfile.c_str(),O_RDWR | O_CREAT ,0777);
			written_bytes = 0;					
			
		}
		
		write(fd_desti,buffer,x);
		written_bytes += x;	
					
		x = read(fd_source, buffer, 2048);
		//cout<<"read " <<x<<" "<<buffer<<endl;

	}
	close(fd_source);
	close(fd_desti);	
}

void copydirectory(DIR *source_dir_ptr,string source, string destination)
{
	
	DIR * dest_dir_ptr = opendir(destination.c_str());
	struct dirent * dirent_ptr;
	if(dest_dir_ptr == NULL)
	{
		cout<<"Destination dir does not exist"<<endl;
		  // Creating a directory 
	    if (mkdir(destination.c_str(), 0777) == -1) 
		cerr << "Error :  " << strerror(errno) << endl; 
	  
	    else
		cout << "Directory created"<<endl; 

//		return;
		//logic to creat dir
	}

	closedir(dest_dir_ptr);
	dirent_ptr = readdir(source_dir_ptr);
	while(dirent_ptr != NULL)
	{	string file = dirent_ptr->d_name;	
		if(string(file)!="." && string(file)!="..")
		{
			splitcopyfile(source+"/"+file,destination+"/"+file);
		}
		dirent_ptr = readdir(source_dir_ptr);
	}
	
			
}

string findfilename(string str) 
{ 
    // Traverse from right 
    for (int i = str.length() - 1; i >= 0; i--) 
        if (str[i] == '/') 
            return str.substr(i+1,str.length()); 
  
    return str; 
} 


int main(int argc, char **argv)
{
	cout<<"Do you want to use split feature ?\n (When file size exceed 4 GB it automatically splits file into 4 GB size file)\n Y or N"<<endl;
	char y =getchar();

	if(y == 'N')
		dontsplit = true;
		
	DIR * dir_ptr,* dir_ptr1;
	clock_t start,end;
	start = clock();
	string s1(argv[1]);
	string s2(argv[2]);
	
	if(s1=="-m")
	{	
		if(argc < 5)
		{
			cout<<"invalid command plz refer -m format";
			return 1;
		}
		else
		{
			string d = argv[argc-1];
			
			for(int i=2;i<argc-1;i++)
			{
				copyfile(argv[i],d);
			}			

		}		

	}
	else if (argc > 3)
	{
		string d = argv[argc-1];
		dir_ptr1 = opendir(argv[argc-1]);
		
		if(dir_ptr1==NULL)
		{
			cout<<"Destination dir does not exist"<<endl;
				  // Creating a directory 
			    if (mkdir(d.c_str(), 0777) == -1) 
				cerr << "Error : While creating destination dire  " << strerror(errno) << endl; 
			  
			    else
				
				cout << "Directory created"<<endl; 
		}
		closedir(dir_ptr1);
		dir_ptr1 = opendir(argv[argc-1]);
		if(dir_ptr1!=NULL )
		{
			for(int i=1;i<argc-1;i++)
			{
				string s(argv[i]);
				string file = findfilename(s);
				if(s[s.length()-1] != '/')
					splitcopyfile(s,d+"/"+file);		
				else
					splitcopyfile(s,d+file);
				
			}	
		}
		closedir(dir_ptr1);
		end = clock();
		double time = ((double)(end-start))/CLOCKS_PER_SEC;
		cout << "Time taken for copy : "<<time<<" seconds";
		return 1;
	}
	else
	{
		
		dir_ptr = opendir(argv[1]);	
		if(dir_ptr!=NULL)
		{
			cout<<"Directory "<<endl;

			
			copydirectory(dir_ptr,s1,s2);
			closedir(dir_ptr);
			end = clock();
			double time = ((double)(end-start))/CLOCKS_PER_SEC;
			cout << "Time taken for copy : "<<time<<" seconds";
			return 0;
		}
		closedir(dir_ptr);

		dir_ptr1 = opendir(argv[2]);
		if(dir_ptr1!=NULL )
		{//dest is directory
			string file = findfilename(s1);
			cout<<" path " <<file;
			if(s2[s2.length()-1] != '/')
				splitcopyfile(s1,s2+"/"+file);		
			else
				splitcopyfile(s1,s2+file);
		}
		else
		{
			
			splitcopyfile(s1,s2);
			//copyfile(s1,s2);
		}	
	}
	end = clock();
	double time = ((double)(end-start))/CLOCKS_PER_SEC;
	cout << "Time taken for copy : "<<time<<" seconds";
	return 1;
}

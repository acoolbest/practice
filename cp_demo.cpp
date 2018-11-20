#include <fstream>
#include <string>
using namespace std;

bool cp_file(const char* src_filename, const char* dst_filename)
{
	bool ret = false;
	string strline = "";
	ifstream ifs(src_filename);
	ofstream ofs(dst_filename);
    if (ifs.is_open() && ofs.is_open())
    { 
    	while (ifs.good()){
			getline(ifs,strline);
			ofs << strline;
			if(!ifs.eof()) ofs << endl;
	  	}
		ret = true;
    }
	if(ifs.is_open()) ifs.close();
	if(ofs.is_open()) ofs.close();
	return ret;
}

int main()
{
	cp_file("a.out", "a.out_cp");
}
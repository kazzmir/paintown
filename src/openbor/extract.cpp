
void mkdirs(string path){
    string dir(path);
    int start = 0;
    int slash = dir.find('/', start);
    // cout << "Dir is " << dir << endl;
    string sofar = ".";
    while (slash != string::npos){
        string part = dir.substr(start, slash - start);
        // cout << "Create dir '" << (sofar + "/" + part) << "'" << endl;
        mkdir((sofar + "/" + part).c_str(), 0775);
        sofar += "/" + part;
        start = slash + 1;
        slash = dir.find('/', start);
    }
    // string part = dir.substr(start);
    // mkdir((sofar + "/" + part).c_str(), 0775);
}

string replaceSlashes(string in){
    int place = in.find('\\');
    while (place != string::npos){
        in[place] = '/';
        place = in.find('\\');
    }
    return in;
}

string lowercase(string in){
    for (unsigned int i = 0; i < in.length(); i++){
        if (in[i] >= 'A' && in[i] <= 'Z'){
            in[i] = in[i] - 'A' + 'a';
        }
    }
    return in;
}

void extract(string where){
    mkdir(where.c_str(), 0775);
    ifstream stream;
    stream.open(filename.c_str(), std::ios::in | std::ios::binary);
    for (std::map<std::string, File>::iterator it = files.begin(); it != files.end(); it++){
        string path = lowercase(replaceSlashes((*it).first));
        // cout << "Path is '" << path << "'" << endl;
        File offset = (*it).second;
        stream.seekg(offset.start);
        char * data = new char[offset.length];
        stream.read(data, offset.length);
        string final = where + "/" + path;
        // cout << "Final is '" << final << "'" << endl;
        mkdirs(final);
        ofstream out(final.c_str(), std::ios::out | std::ios::binary);
        cout << "Writing " << final << endl;
        out.write(data, offset.length);
    }
}

int main(int argc, char ** argv){
    string file;
    bool extract = false;
    for (int i = 1; i < argc; i++){
        if (string(argv[i]) == "extract"){
            extract = true;
        } else {
            file = string(argv[i]);
        }
    }
    Bor::PackReader reader(file);
    if (extract){
        reader.extract(file + "-out");
    }
}

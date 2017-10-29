int main(){
std::string x;
std::string b(\"abc\");
const static std::string y = x + b;
std::string str1, str2, str3 = \"abc\";
std::string str4(b, 5), str6(str4);
std::map<int, int>* mp1 = new std::map<int, int>();
std::string& str5 = str4;
}

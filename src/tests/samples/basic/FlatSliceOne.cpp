void fun(int z){
	z++;
	return z;
}
void foo(int &x, int *y){
fun(x);
y++;
}
int main(){
int sum = 0;
int i = 1;
while (i<=10){
foo(sum, i);
}
std::cout<<"i: "<<i<<"sum: "<<sum<<std::endl;
std::cout<<fun(i);
sum=sum+i;
};

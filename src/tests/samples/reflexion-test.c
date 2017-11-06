void func1();
void func2();
void func3();
void func4();
void func5();
int func6(int);

int g1;
int g2;
int g3;
int g4;
int g5;
int g6;
int g7;
int g8;
int g9;
int g10;

int main(int argc, char **argv)
{
	g1 = 0;
	func1();
	g3 = g2;

	func2();
	func3();
	func4();

	printf("%d", g1);
	printf("%d", g2);
	printf("%d", g3);
	printf("%d", g4);
	printf("%d", g5);
	printf("%d", g8);

	func5();
	printf("%d", g6);
	printf("%d", g7);

	g9 = 1;
	g10 = func6(argc);
	printf("%d", g10);
}

void func1()
{
	g2 = g1;
}

void func2()
{
	int l1;
	if (g1 == 0)
	{
		l1 = 1;
	}
	else
	{
		l1 = 2;
	}
	g4 = l1;
}

void func3()
{
	g5 = g4;
}

void func4()
{
	int l2 = 0;
	if (g4 == 1)
	{
		l2 = g2;
	}
	g8 = l2;
}

void func5()
{
	if (g1 == 1)
		exit(1);
	g6 = g1;
	g7 = g2;
}

int func6(int param)
{
	return param + g9;
}

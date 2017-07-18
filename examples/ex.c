int foo(int a)
{
	int b, c = 1;

	b = c * a;
	for (int n = 0; n < c + 5; n++)
		b = b + (n * a);

	return b;
}

int
main(int argc, char *argv[])
{
	int v1 = foo(1);
	int v2 = foo(2);
	int v3 = foo(4);

	return (v1 + v2 + v3);
}


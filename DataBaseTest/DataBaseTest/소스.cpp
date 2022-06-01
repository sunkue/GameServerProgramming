#include <windows.h>  
#include <sqlext.h> 
#include <iostream>
#include <any>
using namespace std;

int main()
{
	any* a = new any;
	*a = make_any<SQLINTEGER*>(new SQLINTEGER(3));

	try {
		if (a->type().hash_code() == typeid(SQLWCHAR*).hash_code())
		{
			cout << " !!" << endl;
		}
		else if (a->type().hash_code() == typeid(SQLINTEGER*).hash_code())
		{
			cout << " ???>>" << endl;
		}
	}
	catch (...)
	{
		cout << "errr" << endl;
	}

	
	cout << "end" << endl;
}
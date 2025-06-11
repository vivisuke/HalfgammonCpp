#include <iostream>
#include "Board.h"

using namespace std;

int main()
{
	test_Board();
	//
	Board bd;
	bd.print();
	bd.gen_moves_black_2(1, 2);
	bd.print_vmoves();
	bd.move_black(bd.m_vmoves[0]);
	bd.print();

    std::cout << "\nOK.\n";
}

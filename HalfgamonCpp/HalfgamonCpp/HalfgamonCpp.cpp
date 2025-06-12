#include <iostream>
#include "Board.h"

using namespace std;

int main()
{
	test_Board();
	//
	Board bd;
	if (false) {
		bd.print();
		bd.gen_moves_black_2(1, 2);
		bd.print_vmoves();
		bd.move_black(bd.m_vmoves[0]);
		bd.print();
	}
	if (false) {
		auto r = bd.playout(1000, BLACK);
		cout << "rate = " << r << endl;
	}
	auto mvs = bd.sel_move(AI_TYPE_PMC, BLACK, 2, 3);
	cout << "mvs = " << to_str(mvs) << endl;
	bd.do_move(BLACK, mvs);
	bd.print();

    std::cout << "\nOK.\n";
}

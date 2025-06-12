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
	if( false ) {
		auto mvs = bd.sel_move(AI_TYPE_PMC, BLACK, 2, 3);
		cout << "mvs = " << to_str(mvs) << endl;
		bd.do_move(BLACK, mvs);
		bd.print();
	}
	if( true ) {
		char next = BLACK;
		uchar black_AI = AI_TYPE_PMC;
		uchar white_AI = AI_TYPE_RANDOM;
		for(int i = 0;;++i) {
			int d1, d2;
			roll_dice(d1, d2);
			cout << "#" << (i+1) << ": dice = (" << d1 << ", " << d2 << ")" << endl;
			if( next == BLACK ) {
				auto mvs = bd.sel_move(black_AI, BLACK, d1, d2);
				cout << "mvs = " << to_str(mvs) << endl;
				bd.do_move(next, mvs);
			} else {
				auto mvs = bd.sel_move(black_AI, WHITE, d1, d2);
				cout << "mvs = " << to_str(mvs) << endl;
				bd.do_move(next, mvs);
			}
			bd.print();
			if( bd.m_pip_black == 0 || bd.m_pip_white == 0 )
				break;
			next = (BLACK+WHITE) - next;
			//getchar();
		}
	}

    std::cout << "\nOK.\n";
}

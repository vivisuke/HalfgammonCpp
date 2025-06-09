#include <iostream>
#include <sstream>
#include <assert.h>
#include "Board.h"

using namespace std;

const char init_data[] = {0, 0, -2, 0, 3, 0, 0, -3, 3, 0, 0, -3, 0, 2, 0, 0};

//--------------------------------------------------------------------------------
void Move::print(bool nl) const {
	cout << (int)m_src << " to " << (int)m_dst
			<< ", dice = " << (int)m_dice << (m_hit ? " hit" : " not hit");
	if( nl ) cout << endl;
	else cout << ", ";
}
string Move::to_str() const {
	std::ostringstream oss;
	oss << (int)m_src << "/" << (int)m_dst;
	if( m_hit ) oss << "*";
	return oss.str();
}

//--------------------------------------------------------------------------------
void Board::init() {
	init(init_data);
}
void Board::init(const char* data) {
	for(int i = 0; i != N_CELLS+2; ++i)		//	+2 for ゴール
		m_cell[i] = data[i+1];
	m_bar_black = data[0];
	m_bar_white = data[N_CELLS+3];
	//m_pip_black = m_pip_white = 0;
	update_pip_count();
}
void Board::update_pip_count() {
	m_pip_black = -m_bar_black * (N_CELLS + 1);
	m_pip_white = m_bar_white * (N_CELLS + 1);
	for (int i = 1; i < N_CELLS + 1; ++i) {
		if (m_cell[i] >= WHITE)			// 人間駒
			m_pip_white += m_cell[i] * i;
		else if (m_cell[i] <= BLACK)	// AI 駒
			m_pip_black -= m_cell[i] * (B_GOAL - i);
	}
}
void Board::print() const {
	cout << "◯ white pip: " << m_pip_white << ", ● black pip: " << m_pip_black << endl;
	for(int y = N_CHECKERS+1; --y > 0;) {
		if( y <= -m_bar_black ) cout << "●";
		else cout << "・";
		for(int x = B_START; x <= B_GOAL;  ++x) {
			if( m_cell[x] >= WHITE ) {
				cout << (y <= m_cell[x] ? "◯" : "・");
			} else if( m_cell[x] <= BLACK ) {
				cout << (y <= -m_cell[x] ? "●" : "・");
				//if( y <= -m_cell[x] )
				//	cout << "＞";
				//else
				//	cout << "・";
			} else
				cout << "・";
		}
		if( y <= m_bar_white ) cout << "◯";
		else cout << "・";
		cout << endl;
	}
	cout << "ＳＧ１２３４５６７８９101112ＧＳ" << endl << endl;
}
//--------------------------------------------------------------------------------
void test_Board()
{
	cout << "testing..." << endl;
}

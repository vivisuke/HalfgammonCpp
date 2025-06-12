#include <iostream>
#include <sstream>
#include <random>
#include <assert.h>
#include "Board.h"

using namespace std;

std::mt19937 rgen(std::random_device{}()); // ランダムシード設定
//std::mt19937 rgen(0); // シードを設定

const char init_data[] = {0, 0, -2, 0, 3, 0, 0, -3, 3, 0, 0, -3, 0, 2, 0, 0};

void roll_dice(int& d1, int& d2) {
	d1 = rgen() % 3 + 1;
	d2 = rgen() % 3 + 1;
}

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
Board::Board(const Board& other)
	: m_bar_black(other.m_bar_black)
	, m_bar_white(other.m_bar_white)
	, m_vmoves(other.m_vmoves)		// std::vectorのコピーコンストラクタが呼ばれる
	, m_moves_1(other.m_moves_1)	// Movesクラスのコピーコンストラクタが呼ばれる
	, m_moves(other.m_moves)		// Movesクラスのコピーコンストラクタが呼ばれる
	, m_n_d1_move(other.m_n_d1_move)
	, m_pip_black(other.m_pip_black)
	, m_pip_white(other.m_pip_white)
{
	std::copy(std::begin(other.m_cell), std::end(other.m_cell), std::begin(m_cell));
}
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
	cout << "ＢＧ１２３４５６７８９101112ＧＢ" << endl << endl;
}
void Board::print_moves() const {
	for(int i = 0; i != m_moves.size(); ++i) {
		cout << (i+1) << ": ";
		m_moves[i].print();
	}
	cout << endl;
}
string to_str(const Moves& mvs) {
	string txt;
	for(int k = 0; k != mvs.size(); ++k) {
		if( mvs[k].m_src == 0 && mvs[k].m_dst == 0 ) break;
		txt += mvs[k].to_str() + " ";
	}
	return txt;
}
void Board::print_vmoves() const {
	for(int i = 0; i != m_vmoves.size(); ++i) {
		cout << (i+1) << ": " << to_str(m_vmoves[i]) << endl;
	}
	cout << endl;
}
int Board::n_black() const {
	int n = m_bar_black;
	for(int i = 1; i <= B_GOAL; ++i) {
		if( m_cell[i] <= BLACK )
			n += m_cell[i];
	}
	return -n;
}
int Board::n_white() const {
	int n = m_bar_white;
	for(int i = 0; i <= N_CELLS; ++i) {
		if( m_cell[i] >= WHITE )
			n += m_cell[i];
	}
	return n;
}
bool Board::is_last_black(int src) const {		//	ベアオフ可能な場合のみコール可
	for(int i = 10; i < src; ++i) {
		if( m_cell[i] <= BLACK ) return false;
	}
	return true;
}
bool Board::is_last_white(int src) const {		//	ベアオフ可能な場合のみコール可
	for(int i = src+1; i < 4; ++i) {
		if( m_cell[i] >= WHITE ) return false;
	}
	return true;
}
bool Board::can_bear_off_black() const {
	int n = -m_cell[B_GOAL];
	for (int i = 10; i != 13; ++i)
		if (m_cell[i] < 0) n -= m_cell[i];
	return n == N_CHECKERS;
}
bool Board::can_bear_off_white() const {
	int n = m_cell[W_GOAL];
	for (int i = 1; i != 4; ++i)
		if (m_cell[i] > 0) n += m_cell[i];
	return n == N_CHECKERS;
}
//	複数着手生成 → m_vmoves[]
void Board::gen_moves_2(char next, int d1, int d2) {
	if( next == BLACK )
		gen_moves_black_2(d1, d2);
	else
		gen_moves_white_2(d1, d2);
}
//	複数着手生成 → m_vmoves[]
void Board::gen_moves_black_2(int d1, int d2) {
	m_vmoves.clear();
	if( d1 != d2 ) {	//	非ゾロ目の場合
		Moves vm1, vm2;
		gen_moves_black_1(vm1, d1);
		gen_moves_black_1(vm2, d2);
		if( !vm1.is_empty() ) {
			Moves mvs(2);
			for(int i = 0; i != vm1.size(); ++i) {
				mvs[0] = vm1[i];
				move_black(vm1[i]);
				gen_moves_black_1(d2, vm1[i].m_src);
				if( m_moves_1.is_empty() ) {
					if( vm2.is_empty() || d1 > d2 && vm2.size() == 1 && vm1[0].m_src == vm2[0].m_src ) {
						mvs[1] = Move();
						m_vmoves.push_back(mvs);
					}
				} else {
					for(int k = 0; k != m_moves_1.size(); ++k) {
						mvs[1] = m_moves_1[k];
						if( mvs[1].m_src > mvs[0].m_src || mvs[1].m_src == mvs[0].m_src && mvs[1].m_dst > mvs[0].m_dst || mvs[1].m_dst == B_GOAL )
							m_vmoves.push_back(mvs);
					}
				}
				unmove_black(vm1[i]);
			}
		}
		if( !vm2.is_empty() ) {
			Moves mvs(2);
			for(int i = 0; i != vm2.size(); ++i) {
				mvs[0] = vm2[i];
				move_black(vm2[i]);
				gen_moves_black_1(d1, vm2[i].m_src);
				if( m_moves_1.is_empty() ) {
					if( vm1.is_empty() || d1 < d2 && vm1.size() == 1 && vm1[0].m_src == vm2[0].m_src ) {
						mvs[1] = Move();
						m_vmoves.push_back(mvs);
					}
				} else {
					for(int k = 0; k != m_moves_1.size(); ++k) {
						mvs[1] = m_moves_1[k];
						if( mvs[1].m_src > mvs[0].m_src || mvs[1].m_src == mvs[0].m_src && mvs[1].m_dst > mvs[0].m_dst || mvs[1].m_dst == B_GOAL )
							m_vmoves.push_back(mvs);
					}
				}
				unmove_black(vm2[i]);
			}
		}
	} else {		//	ゾロ目の場合
		int max_du = 0;		//	使用ダイス目
		gen_moves_black_1(d1);
		if( m_moves_1.is_empty() ) return;
		Moves mvs(4);
		Moves vm1;
		vm1.swap(m_moves_1);
		int min_src = vm1[0].m_src;		//	src：昇順と仮定
		for(int i = 0; i != vm1.size(); ++i) {
			mvs[0] = vm1[i];
			move_black(vm1[i]);
			gen_moves_black_1(d1, vm1[i].m_src);
			if( m_moves_1.is_empty() ) {
				if( max_du == 0 && mvs[0].m_src == min_src ) {
					mvs[1] = mvs[2] = mvs[3] = Move();
					m_vmoves.push_back(mvs);
					max_du = 1;
				}
			} else {
				Moves vm2;
				vm2.swap(m_moves_1);
				for(int i2 = 0; i2 != vm2.size(); ++i2) {
					mvs[1] = vm2[i2];
					move_black(vm2[i2]);
					gen_moves_black_1(d1, vm2[i2].m_src);
					if( m_moves_1.is_empty() ) {
						if( max_du < 2 && mvs[0].m_src == min_src ) {
							mvs[2] = mvs[3] = Move();
							m_vmoves.push_back(mvs);
							max_du = 2;
						}
					} else {
						Moves vm3;
						vm3.swap(m_moves_1);
						for(int i3 = 0; i3 != vm3.size(); ++i3) {
							mvs[2] = vm3[i3];
							move_black(vm3[i3]);
							gen_moves_black_1(d1, vm3[i3].m_src);
							if( m_moves_1.is_empty() ) {
								if( max_du <= 3 && mvs[0].m_src == min_src ) {
									mvs[3] = Move();
									m_vmoves.push_back(mvs);
									max_du = 3;
								}
							} else {
								for(int i4 = 0; i4 != m_moves_1.size(); ++i4) {
									mvs[3] = m_moves_1[i4];
									m_vmoves.push_back(mvs);
									max_du = 4;
								}
							}
							unmove_black(vm3[i3]);
						}
					}
					unmove_black(vm2[i2]);
				}
			}
			unmove_black(vm1[i]);
		}
	}
}
bool Board::can_bear_off_all_white(int d1, int d2) const {
	if( m_pip_white != d1 + d2 ) return false;
	return false;
}
void Board::gen_moves_white_2(int d1, int d2) {
	m_vmoves.clear();
	int max_du = 0;		//	使用ダイス目
	if( d1 != d2 ) {	//	非ゾロ目の場合
		Moves vm1, vm2;
		gen_moves_white_1(vm1, d1);
		gen_moves_white_1(vm2, d2);
		if( can_bear_off_all_white(d1, d2) ) {
			Moves mvs(2);
			mvs[0] = vm1[0];
			mvs[1] = vm2[0];
			m_vmoves.push_back(mvs);
			return;
		}
		if( !vm1.is_empty() ) {
			Moves mvs(2);
			for(int i = 0; i != vm1.size(); ++i) {
				mvs[0] = vm1[i];
				move_white(vm1[i]);
				gen_moves_white_1(d2, vm1[i].m_src);
				if( m_moves_1.is_empty() ) {
					if( vm2.is_empty() || d1 > d2 && vm2.size() == 1 && vm1[0].m_src == vm2[0].m_src ) {
						mvs[1] = Move();
						m_vmoves.push_back(mvs);
					}
				} else {
					for(int k = 0; k != m_moves_1.size(); ++k) {
						mvs[1] = m_moves_1[k];
						if( mvs[1].m_src < mvs[0].m_src || mvs[1].m_src == mvs[0].m_src && mvs[1].m_dst < mvs[0].m_dst || mvs[1].m_dst == W_GOAL )
							m_vmoves.push_back(mvs);
					}
				}
				unmove_white(vm1[i]);
			}
		}
		if( !vm2.is_empty() ) {
			Moves mvs(2);
			for(int i = 0; i != vm2.size(); ++i) {
				mvs[0] = vm2[i];
				move_white(vm2[i]);
				gen_moves_white_1(d1, vm2[i].m_src);
				if( m_moves_1.is_empty() ) {
					if( vm1.is_empty() || d1 < d2 && vm1.size() == 1 && vm1[0].m_src == vm2[0].m_src ) {
						mvs[1] = Move();
						m_vmoves.push_back(mvs);
					}
				} else {
					for(int k = 0; k != m_moves_1.size(); ++k) {
						mvs[1] = m_moves_1[k];
						if( mvs[1].m_src < mvs[0].m_src || mvs[1].m_src == mvs[0].m_src && mvs[1].m_dst < mvs[0].m_dst || mvs[1].m_dst == W_GOAL )
							m_vmoves.push_back(mvs);
					}
				}
				unmove_white(vm2[i]);
			}
		}
	} else {		//	ゾロ目の場合
		gen_moves_white_1(d1);
		if( m_moves_1.is_empty() ) return;
		Moves mvs(4);
		Moves vm1;
		vm1.swap(m_moves_1);
		int min_src = vm1[0].m_src;		//	src：昇順と仮定
		for(int i = 0; i != vm1.size(); ++i) {
			mvs[0] = vm1[i];
			move_white(vm1[i]);
			gen_moves_white_1(d1, vm1[i].m_src);
			if( m_moves_1.is_empty() ) {
				if( max_du == 0 && mvs[0].m_src == min_src ) {
					mvs[1] = mvs[2] = mvs[3] = Move();
					m_vmoves.push_back(mvs);
					max_du = 1;
				}
			} else {
				Moves vm2;
				vm2.swap(m_moves_1);
				for(int i2 = 0; i2 != vm2.size(); ++i2) {
					mvs[1] = vm2[i2];
					move_white(vm2[i2]);
					gen_moves_white_1(d1, vm2[i2].m_src);
					if( m_moves_1.is_empty() ) {
						if( max_du < 2 && mvs[0].m_src == min_src ) {
							mvs[2] = mvs[3] = Move();
							m_vmoves.push_back(mvs);
							max_du = 2;
						}
					} else {
						Moves vm3;
						vm3.swap(m_moves_1);
						for(int i3 = 0; i3 != vm3.size(); ++i3) {
							mvs[2] = vm3[i3];
							move_white(vm3[i3]);
							gen_moves_white_1(d1, vm3[i3].m_src);
							if( m_moves_1.is_empty() ) {
								if( max_du <= 3 && mvs[0].m_src == min_src ) {
									mvs[3] = Move();
									m_vmoves.push_back(mvs);
									max_du = 3;
								}
							} else {
								for(int i4 = 0; i4 != m_moves_1.size(); ++i4) {
									mvs[3] = m_moves_1[i4];
									m_vmoves.push_back(mvs);
									max_du = 4;
								}
							}
							unmove_white(vm3[i3]);
						}
					}
					unmove_white(vm2[i2]);
				}
			}
			unmove_white(vm1[i]);
		}
	}
}
void Board::gen_moves_black_1(int d1, int start) {
	gen_moves_black_1(m_moves_1, d1, start);
}
void Board::gen_moves_white_1(int d1, int start) {
	gen_moves_white_1(m_moves_1, d1, start);
}
void Board::gen_moves_black_1(Moves& mvs, int d1, int start) {
	mvs.clear();
	if( !can_bear_off_black() ) {		// ベアオフ不可の場合
		if( m_bar_black != 0 ) {		// バーに駒がある場合
			if( m_cell[d1] <= WHITE )		// 白1 or 空 or 黒
				mvs.push_back(Move(B_START, B_START+d1, d1, m_cell[d1]==WHITE));		// 0 to d1
		} else {
			for(int src = start; src < N_CELLS+1; ++src) {
				if( m_cell[src] <= BLACK ) {		// if BLACK
					int dst = src + d1;
					if( dst < B_GOAL && m_cell[dst] <= WHITE )		// 白1 or 空 or 黒
						mvs.push_back(Move(src, dst, d1, m_cell[dst] == WHITE));
				}
			}
		}
	} else {	// ベイオフ可能な場合
		for(int src = start; src < B_GOAL; ++src) {
			if( m_cell[src] <= BLACK ) {		// if 黒
				int dst = src + d1;
				if( dst == B_GOAL ) {
					mvs.push_back(Move(src, B_GOAL, d1));
				} else if( dst > B_GOAL ) {
					if( is_last_black(src) )
						mvs.push_back(Move(src, B_GOAL, d1));
				} else if( m_cell[dst] <= WHITE )		// 白1 or 空 or 黒
					mvs.push_back(Move(src, dst, d1));
			}
		}
	}
}
void Board::gen_moves_white_1(Moves& mvs, int d1, int start) {
	mvs.clear();
	if( !can_bear_off_white() ) {		// ベアオフ不可の場合
		if( m_bar_white != 0 ) {		// バーに駒がある場合
			int dst = W_START-d1;
			if( m_cell[dst] >= BLACK )		// 黒1 or 空 or 白
				mvs.push_back(Move(W_START, dst, d1, m_cell[dst]==BLACK));		// 0 to d1
		} else {
			for(int src = start; src != 1; --src) {
				if( m_cell[src] >= WHITE ) {		// 白
					int dst = src - d1;
					if( dst > 0 && m_cell[dst] >= BLACK )		// 黒1 or 空 or 白
						mvs.push_back(Move(src, dst, d1, m_cell[dst] == BLACK));
				}
			}
		}
	} else {	// ベイオフ可能な場合
		for(int src = start; src != 0; --src) {
			if( m_cell[src] >= WHITE ) {		// 白
				int dst = src - d1;
				if( dst == W_GOAL ) {
					mvs.push_back(Move(src, W_GOAL, d1));
				} else if( dst < W_GOAL ) {
					if( is_last_white(src) )
						mvs.push_back(Move(src, W_GOAL, d1));
				} else if( m_cell[dst] >= BLACK )		// 黒1 or 空 or 白
					mvs.push_back(Move(src, dst, d1));
			}
		}
	}
}
void Board::gen_moves_black(int d1, int d2) {
	if( d1 == 0 ) swap(d1, d2);
	gen_moves_black_1(d1);
	m_n_d1_move = (int)m_moves_1.size();
	m_moves.swap(m_moves_1);
	if( d2 != 0 && d2 != d1 ) {
		gen_moves_black_1(d2);
		m_moves.insert(m_moves.end(), m_moves_1.begin(), m_moves_1.end());
	}
}
void Board::gen_moves_white(int d1, int d2) {
	if( d1 == 0 ) swap(d1, d2);
	gen_moves_white_1(d1);
	m_n_d1_move = (int)m_moves_1.size();
	m_moves.swap(m_moves_1);
	if( d2 != 0 && d2 != d1 ) {
		gen_moves_white_1(d2);
		m_moves.insert(m_moves.end(), m_moves_1.begin(), m_moves_1.end());
	}
}
void Board::move_black(int src, int dst) {
	if( src == dst ) return;
	if( src == B_START ) m_bar_black += 1;		// バーからの移動
	else m_cell[src] += 1;
	if( m_cell[dst] == WHITE ) {			// 白１個ある場合→ヒット
		m_cell[dst] = BLACK;
		m_bar_white += 1;
		m_pip_white += W_START - dst;
	}
	else
		m_cell[dst] -= 1;
	m_pip_black -= dst - src;
}
void Board::move_white(int src, int dst) {
	if( src == dst ) return;
	if( src == W_START ) m_bar_white -= 1;		// バーからの移動
	else m_cell[src] -= 1;
	if( m_cell[dst] == BLACK ) {			// 白１個ある場合→ヒット
		m_cell[dst] = WHITE;
		m_bar_black -= 1;
		m_pip_black += dst - B_START;
	}
	else
		m_cell[dst] += 1;
	m_pip_white -= src - dst;
}
void Board::do_move(char next, const Moves& mvs) {
	if( next == BLACK )
		move_black(mvs);
	else
		move_white(mvs);
}
void Board::move_black(const Moves& mvs) {
	for(int i = 0; i != mvs.size(); ++i)
		move_black(mvs[i]);
}
void Board::move_white(const Moves& mvs) {
	for(int i = 0; i != mvs.size(); ++i)
		move_white(mvs[i]);
}
void Board::unmove_black(int src, int dst, bool hit) {
	if( src == B_START ) m_bar_black -= 1;		// バーからの移動だった場合
	else m_cell[src] -= 1;
	if( hit ) {
		m_cell[dst] = WHITE;
		m_bar_white -= 1;
		m_pip_white -= W_START - dst;
	} else
		m_cell[dst] += 1;
	m_pip_black += dst - src;
}
void Board::unmove_white(int src, int dst, bool hit) {
	if( src == W_START ) m_bar_white += 1;		// バーからの移動だった場合
	else m_cell[src] += 1;
	if( hit ) {
		m_cell[dst] = BLACK;
		m_bar_black += 1;
		m_pip_black -= dst - B_START;
	} else
		m_cell[dst] -= 1;
	m_pip_white += src - dst;
}
double  Board::playout(int N_LOOP, char next) const {
	int sum = 0;
	for(int i = 0; i < N_LOOP; ++i) {
		Board bd(*this);
		sum += bd.playout(next);
	}
	return (double)sum / N_LOOP;
}
int  Board::playout(char next) {
	for (int i = 0;;++i) {
		//int d1 = rgen() % 3 + 1;
		//int d2 = rgen() % 3 + 1;
		int d1, d2;
		roll_dice(d1, d2);
		gen_moves_2(next, d1, d2);
		if( !m_vmoves.is_empty() ) {
			int r = rgen() % m_vmoves.size();
			//cout << (i+1) << ": " << to_str(m_vmoves[r]) << endl;
			do_move(next, m_vmoves[r]);
			//print();
			if( m_pip_black == 0 ) {
				return BLACK;
			} else if( m_pip_white == 0 ) {
				return WHITE;
			}
		}
		next = (BLACK + WHITE) - next;
	}
	return 0;
}
static Moves g_mvs;
const Moves& Board::sel_move(uchar ai_type, char next, int d1, int d2) {
	switch( ai_type ) {
	case AI_TYPE_RANDOM:	return sel_move_random(next, d1, d2);
	case AI_TYPE_PMC:		return sel_move_PMC(next, d1, d2);
	}
	return g_mvs;
}
const Moves& Board::sel_move_random(char next, int d1, int d2) {
	gen_moves_2(next, d1, d2);
	if( m_vmoves.is_empty() ) {
		return g_mvs;
	}
	int r = rgen() % m_vmoves.size();
	return m_vmoves[r];
}
const Moves& Board::sel_move_PMC(char next, int d1, int d2) {
	gen_moves_2(next, d1, d2);
	int bestix = -1;
	double maxev = -9999;
	for(int ix = 0; ix < m_vmoves.size(); ++ix) {
		Board bd(*this);
		bd.do_move(next, m_vmoves[ix]);
		auto ev = bd.playout(100, (BLACK+WHITE)-next) * next;
		if( ev > maxev ) {
			maxev = ev;
			bestix = ix;
		}
	}
	if (bestix >= 0)
		return m_vmoves[bestix];
	return g_mvs;
}
//--------------------------------------------------------------------------------
#define		TEST_EQU(v, exp)	test_equ(__FILE__, __LINE__, v, exp)
int n_tested = 0;
int n_failed = 0;
bool test_equ(const string& fileName, int linenum, int v, int exp) {
	++n_tested;
	if( v != exp ) {
		++n_failed;
		cout << "TEST FAILED. '" << fileName << "': " << linenum << endl;
		cout << "expected: " << exp << ", actual value: " << v << endl << endl;
		return false;
	} else
		return true;
}
void test_finished() {
	cout << n_failed << " failed / " << n_tested << " tested." << endl << endl;
}

const char data_000[] = {0, 0, 0, -2, 3, 0, 0, -3, 3, 0, 0, -3, 0, 2, 0, 0};
const char data_002[] = {0, 0, 3, 3, 2, 0, 0, 0, 0, 0, 0, -2, -3, -3, 0, 0};
const char data_003[] = {0, 0, -8, 2, 2, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0};
const char data_004[] = {0, 0, -8, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0};
void test_Board()
{
	cout << "testing..." << endl;
	Board bd;
	//bd.print();
	TEST_EQU(bd.m_pip_black, 54);
	TEST_EQU(bd.m_pip_white, 54);
	TEST_EQU(bd.n_black(), 8);
	TEST_EQU(bd.n_white(), 8);
	//
	cout << "testing gen_moves_black_2() ..." << endl;
	//	初期状態
	bd.init();
    bd.gen_moves_black_2(1, 2);
    if( !TEST_EQU( (int)bd.m_vmoves.size(), 4 ) ) {
		bd.print();
	    cout << "dice = 1, 2" << endl;
	    bd.print_vmoves();
    }
    bd.gen_moves_black_2(2, 1);
    if( !TEST_EQU( (int)bd.m_vmoves.size(), 4 ) ) {
		bd.print();
	    cout << "dice = 2, 1" << endl;
	    bd.print_vmoves();
    }

}

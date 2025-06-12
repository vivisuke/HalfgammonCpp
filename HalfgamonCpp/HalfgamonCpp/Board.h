#pragma once

#include <vector>
#include <string>

#define	is_empty()	empty()
using uchar = unsigned char;

const int N_CELLS = 12;
const int BLACK = -1;
const int WHITE = 1;
const int B_START = 0;
const int B_GOAL = N_CELLS + 1;
const int W_START = N_CELLS + 1;
const int W_GOAL = 0;
const int N_CHECKERS = 8;			//	片方の駒数

void	test_Board();


struct Move {
public:
	Move(char src = 0, char dst = 0, char dice = 0, bool hit=false)
		: m_src(src), m_dst(dst), m_dice(dice), m_hit(hit)
	{
	}
	void print(bool=true) const;
	std::string to_str() const;
public:
	char	m_src;
	char	m_dst;
	char	m_dice;			//	使用ダイス目
	bool	m_hit;
};

typedef std::vector<Move> Moves;
std::string to_str(const Moves&);

class Board
{
public:
	Board() {
		init();
	}
	Board(const Board& other);		//	コピーコンストラクタ
public:
	void	init();
	void	init(const char*);
	void	print() const;
	void	print_moves() const;
	void	print_vmoves() const;
	int		n_black() const;
	int		n_white() const;
	bool	is_last_black(int) const;
	bool	is_last_white(int) const;
	bool	can_bear_off_black() const;
	bool	can_bear_off_white() const;
	bool	can_bear_off_all_black(int, int) const;
	bool	can_bear_off_all_white(int, int) const;
	void	update_pip_count();
	void	gen_moves_2(char next, int d1, int d2);		//	先後手指定、複数着手生成 → m_vmoves[]
	void	gen_moves_black_2(int d1, int d2);		//	複数着手生成 → m_vmoves[]
	void	gen_moves_white_2(int d1, int d2);
	void	gen_moves_black_1(Moves&, int d1, int start = 1);
	void	gen_moves_white_1(Moves&, int d1, int start = N_CELLS);
	void	gen_moves_black_1(int d1, int start = 1);
	void	gen_moves_white_1(int d1, int start = N_CELLS);
	void	gen_moves_black(int d1, int d2=0);
	void	gen_moves_white(int d1, int d2=0);
	void	move_black(int src, int dst);
	void	move_white(int src, int dst);
	void	move_black(const Move& mv) { move_black(mv.m_src, mv.m_dst); }
	void	move_white(const Move& mv) { move_white(mv.m_src, mv.m_dst); }
	void	unmove_black(int src, int dst, bool hit=false);
	void	do_move(char next, const Moves& mvs);
	void	move_black(const Moves& mvs);
	void	move_white(const Moves& mvs);
	void	unmove_white(int src, int dst, bool hit=false);
	void	unmove_black(const Move& mv) { unmove_black(mv.m_src, mv.m_dst, mv.m_hit); }
	void	unmove_white(const Move& mv) { unmove_white(mv.m_src, mv.m_dst, mv.m_hit); }

	int		playout(char next);
	double	playout(int N_LOOP, char next) const;

public:
	char 	m_cell[N_CELLS+2];
	char	m_bar_black;		//	0 - 駒数 at バー、駒がある場合はマイナス値
	char	m_bar_white;		//	駒数 at バー
	std::vector<Moves>	m_vmoves;	//	
	Moves	m_moves_1;
	Moves	m_moves;
	int		m_n_d1_move;		//	d1 による可能着手数
	int		m_pip_black;
	int		m_pip_white;
};


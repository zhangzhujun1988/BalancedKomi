
//	2021.7.22 
//	平衡贴点值测试 


#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>

using namespace std;


//=============================================================================搜索配置 开始 
//设置受限的矩形棋盘大小，ROW 为行数（高度），COL 为列数（宽度） 
const int ROW = 3;
const int COL = 6;

//设置棋盘类型：1 表示右侧受限；2 表示左侧和右侧受限；3 表示右侧和下侧受限 
const int BOARD_TYPE = 1;			

//设置搜索时对局手数的最大值 
const int MAX_PLAY_NUMBER = 19;		

//设置白方被提子的数量的最大值 
const int CAPTURE_WHITE_LIMIT = 2;	

//当 CAPTURE_PLAY_MIN 手内有白子被提，放弃该分支 
const int CAPTURE_PLAY_MIN = 0; 

//当白子被提的数量与黑子被提数量的差超过 CAPTURE_DIFFERENCE_MAX，放弃该分支 
const int CAPTURE_DIFFERENCE_MAX = 1;

//设置初始着法 
const int INIT_PLAY_NUMBER = 3;		//初始着法数量 
const int INIT_PLAY[INIT_PLAY_NUMBER + 1][2] = {
	{1,1},		//白方第 1 手 
	{-1,-1},		//黑方第 2 手 
	{1,0},		//白方第 3 手 
	
	{-1,-1}
};

const int TABLE_MAX_HEIGHT = 27;

//设置打印节点的高度的最大值 
const int PRINT_TREE_HEIGHT = 7; 
//=============================================================================搜索配置 结束 


//=============================================================================常量定义 结束 
//定义颜色，用于表示棋盘交叉点的颜色和行动方 
const int EMPTY	= 0;
const int BLACK = 1;
const int WHITE = 2;
 
//定义着手的类型 
const int PASS    = 101;	//表示虚着，当着子点超出棋盘范围时默认为虚着 
const int SUPERKO = 102;	//表示形成同形再现 
const int CAPTURE = 103;	//表示不造成同形的提取对方棋子的着法
const int SUICIDE = 104;	//表示不造成同形的自尽的着法，如果是单颗棋子自尽，将自动转换为虚着 
const int NORMAL  = 105;	//表示不造成同形的没有提子的合法着法 
const int ILLEGAL = 106;	//表示不合法的着法（着子点已有棋子） 

//定义死活的类型 
const int NOTALIVE       = -1;		//表示白方不能做活 
const int NORMALALIVE    = 1;		//表示白方可以做活 
const int MONOTONICALIVE = 2;		//表示白方可以做活，且做活过程中没有被提子 

//棋盘的中心位置 
const int CENTERX = (ROW - 1) / 2;
const int CENTERY = (COL - 1) / 2;
//=============================================================================常量定义 结束 


//=============================================================================全局变量 开始 
//表示每个交叉点是否受限的数组
//0 表示双方均可落子；1 表示仅黑方可落子，且在该点的黑子始终被认为是有气 
//在 init() 函数中根据棋盘类型进行初始化 
int boardLimit[ROW][COL] = {0};		

//死活表，保存之前计算的局面下白方能否做活，用于查询 
map<long long, int> deadAliveTable;

//全局数组，用于减少数组初始化消耗的时间 
int liberty[ROW][COL] = {0};
//=============================================================================全局变量 结束 


//=============================================================================计数器 结束 
//节点计数器 
long long countNode                            = 0;		//总节点量计数器 
long long countCaptureBlack[MAX_PLAY_NUMBER]   = {0};	//黑子被提节点计数器 
long long countCaptureWhite[MAX_PLAY_NUMBER]   = {0};	//白子被提节点计数器 
long long countTreeHeight[MAX_PLAY_NUMBER + 2] = {0};	//节点深度（手数）计数器 

//函数调用次数计数器 
long long countWhiteAlive    = 0;	//判断白方能否做活函数计数器 
long long countPlay          = 0;	//落子函数计数器 
long long countSuperko       = 0;	//判断全同函数计数器 
long long countUnconditional = 0;	//判断无条件活函数计数器 

//死活表相关计数器 
long long countNotAlive       = 0;
long long countNormalAlive    = 0;
long long countMonotonicAlive = 0;
//=============================================================================计数器 结束 


//============================================================================= Node 类声明 开始 
class Node {
public:
	
	//基本值成员 
	int board[ROW][COL];	//表示棋盘 
	int turn;				//表示轮到哪方行动 
	
	//辅助值成员 
	Node* father;			//表示父节点 
	long long hashValue;	//表示当前局面的 hash 值，用于判断局面是否同形 
	int captureBlackNumber;	//表示目前黑子被提掉的数量 
	int captureWhiteNumber;	//表示目前白子被提掉的数量 
	int playNumber;			//表示当前已经下的手数 
	
	
	//构造函数 
	Node();
	
	//复制函数
	void Copy(Node* node);
	 
	//打印函数，用于查看值成员 
	void Print();
	
	//随机生成一个局面函数 
	void Random();
	
	//计算 hash 值函数 
	void ComputeHashValue();
	
	//落子函数 
	int Play(int x, int y);
	
	//计算每个点的气的函数 
	//void ComputeLiberty(int liberty[ROW][COL]);
	void ComputeLiberty();
	
	//提掉颜色为 color 的无气棋子，返回提掉的棋子数量 
	int CaptureStones(int liberty[ROW][COL], int color);
	
	//判断局面是否形成全同 
	int IsSuperko(); 
	
	//判断白方是否有无条件活的子 
	int WhiteUnconditionalAlive();
	
	//判断某个点是否被白子包围 
	int IsSurroundedByWhite(int x, int y);
	
	//计算某个空交叉点支持的白子（该点是白子的气） 
	void ComputeSupport(int x, int y, int support[ROW][COL]);
	
	//判断白方是否能做活 
	int WhiteAlive();
	
	//依据 key 数组，对数组 array 排序 
	void Sort(Node* array[ROW * COL], int key[ROW * COL], int length);
	
	//改进黑方的着法顺序 
	void ImproveBlackOrder(Node* array[ROW * COL], int length);
	
	//改进白方的着法顺序 
	int ImproveWhiteOrder(Node* array[ROW * COL], int length);
}; 

//全局变量搜索节点指针，在 init() 函数中进行初设化 
Node* searchNode = NULL; 
//============================================================================= Node 类声明 开始 


//============================================================================= Node 类函数定义 开始 
//构造函数 
Node::Node() {
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			board[i][j] = EMPTY;
		}
	}
	turn = WHITE;	//空盘情况下，白方先行 
	
	father = NULL; 
	hashValue = 0;
	captureBlackNumber = 0; 
	captureWhiteNumber = 0; 
	playNumber = 0;
	
	++countNode; 
	
	///*调试用 
	//每创建一百万个节点后打印一次，用于反映程序仍在运行 
	if (countNode % 1000000 == 0) {
		cout << "node number : " << countNode / 1000000 << " * 10^6" << endl;
	}
	//*/
}


//复制函数 
void Node::Copy(Node* node) {
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			board[i][j] = node->board[i][j];
		}
	}
	turn = node->turn;
	
	father = node->father;
	hashValue = node->hashValue;
	captureBlackNumber = node->captureBlackNumber;
	captureWhiteNumber = node->captureWhiteNumber;
	playNumber = node->playNumber;
}


//打印函数，用于查看值成员 
void Node::Print() {
	cout << "Node : " << this << endl;
	cout << "\t" << "board :" << endl;
	cout << "\t" << "\t";
	for (int j = 0; j < COL; ++j) {
		cout << j;
		if (j <= 9) {
			cout << " ";
		}
	}
	cout << endl;
	for (int i = 0; i < ROW; ++i) {
		cout << "\t" << "\t";
		for (int j = 0; j < COL; ++j) {
			if (board[i][j] == EMPTY) {
				cout << "  ";
			} else if (board[i][j] == BLACK) {
				cout << "O ";
			} else if (board[i][j] == WHITE) {
				cout << "# ";
			} else {
				cout << "ERROR : Node::Print() board value";
			}
			
		}
		cout << "  " << i;
		if (i == 0) {
			cout << "    O = BLACK";
		} else if (i == 1) {
			cout << "    # = WHITE";
		}
		cout << endl;
	}
	
	if (turn == BLACK) {
		cout << "\t" << "turn       : BLACK" << endl;
	} else if (turn == WHITE) {
		cout << "\t" << "turn       : WHITE" << endl;
	} else {
		cout << "ERROR : Node::Print() turn value";
	}
	
	cout << "\t" << "father     : " << father << endl;
	cout << "\t" << "hash value : " << hashValue << endl;
	cout << "\t" << "black capture number : " << captureBlackNumber << endl;
	cout << "\t" << "white capture number : " << captureWhiteNumber << endl;
}


//计算 hash 值函数 
void Node::ComputeHashValue() {
	long long base = 1;
	hashValue = 0; 
	
	//将棋盘表示为 3 进制数，作为 hash 值 
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			if (board[i][j] == BLACK) {
				hashValue += base;
			} else if (board[i][j] == WHITE) {
				hashValue += 2 * base;
			}
			base *= 3;
		}
	}
} 


//随机生成一个局面函数，仅用于测试 
void Node::Random() {
	int a = 0;
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			a = rand() % 3;
			if (a == 1) {
				board[i][j] = BLACK;
			} else if (a == 2) {
				board[i][j] = WHITE;
			} else {
				board[i][j] = EMPTY;
			}
		}
	}
	
	ComputeHashValue();
	
	a = rand() % 2;
	if (a == 0) {
		turn = BLACK;
	} else {
		turn = WHITE;
	}
} 


//落子函数 
int Node::Play(int x, int y) {
	
	++countPlay;
	 
	//己方和对方的颜色 
	int player = turn;
	int opponent = (turn == BLACK ? WHITE : BLACK);
	
	//若 x 或 y 的范围超出棋盘，默认为 pass 
	if (x < 0 || y < 0 || x >= ROW || y >= COL) {
		turn = opponent;
		++playNumber;
		return PASS;
	}
	
	//若位置 (x,y) 已有棋子，则为非法着法，不改变 turn，直接返回 
	if (board[x][y] != EMPTY) {
		return ILLEGAL;
	}
	
	//若白方落子在受限点上，则为非法着法，不改变 turn，直接返回 
	if (turn == WHITE && boardLimit[x][y] == 1) {
		return ILLEGAL;
	}
	
	//落子 
	board[x][y] = player;
	
	//定义气的数组 
	//int liberty[ROW][COL] = {0};
	
	//计算每个交叉点是否有气 
	//ComputeLiberty(liberty); 
	ComputeLiberty();
	
	//提子数量 
	int count = 0;
	
	//落子后需要改变行动方 
	turn = opponent;
	
	//尝试提取对手的棋子 
	count = CaptureStones(liberty, opponent);
	
	//如果提取了对方的棋子，则检查是否同形后返回 
	if (count > 0) {
		if (IsSuperko() == 1) {
			++playNumber;
			return SUPERKO;
		} else {
			++playNumber;
			return CAPTURE;
		}
	}
	
	//尝试提取己方的棋子 
	count = CaptureStones(liberty, player);
	
	if (count == 1) {
		//单颗棋子自尽，自动转为 pass 
		++playNumber;
		return PASS;
	} else {
		if (IsSuperko() == 1) {
			++playNumber;
			return SUPERKO;
		} else {
			if (count > 1) {
				//多颗棋子自尽 
				++playNumber;
				return SUICIDE;
			} else {
				//无提子着法 
				++playNumber;
				return NORMAL;
			}
		}
	} 
	
	cout << "ERROR : Node::Play()" << endl;
	return 0;
}


//计算每个点的气的函数 
//void Node::ComputeLiberty(int liberty[ROW][COL]) {
void Node::ComputeLiberty() {
	
	//空的交叉点和受限的点被认为是有气的 
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			if (board[i][j] == EMPTY || boardLimit[i][j] == 1) {
				liberty[i][j] = 1;
			} else {
				liberty[i][j] = 0;
			}
		}
	}
	
	//用迭代的方式计算每个交叉点是否有气，每次循环是否有新的点被设置为有气的，如果有则继续循环，如果没有则终止循环 
	int flag = 0;
	do {
		flag = 0;
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j) {
				
				//如果(i,j)已经有气，则直接跳过，检查下一个交叉点
				if (liberty[i][j] == 1) {
					continue; 
				}
				
				//检查点(i,j)能否从四个方向上获得气 
				//上方 
				if (i - 1 >= 0 && (board[i - 1][j] == EMPTY || (board[i - 1][j] == board[i][j] && liberty[i - 1][j] == 1))) {
					liberty[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//下方 
				if (i + 1 < ROW && (board[i + 1][j] == EMPTY || (board[i + 1][j] == board[i][j] && liberty[i + 1][j] == 1))) {
					liberty[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//左侧 
				if (j - 1 >= 0 && (board[i][j - 1] == EMPTY || (board[i][j - 1] == board[i][j] && liberty[i][j - 1] == 1))) {
					liberty[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//右侧 
				if (j + 1 < COL && (board[i][j + 1] == EMPTY || (board[i][j + 1] == board[i][j] && liberty[i][j + 1] == 1))) {
					liberty[i][j] = 1;
					flag = 1;
					continue; 
				} 
			}
		}
	} while (flag == 1);
}


//提掉颜色为 color 的无气棋子，返回提掉的棋子数量 
int Node::CaptureStones(int liberty[ROW][COL], int color) {
	
	//提子数量 
	int count = 0;
	
	//根据 liberty 数组，提掉颜色为 color 的无气棋子 
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			if (board[i][j] == color && liberty[i][j] == 0) {
				board[i][j] = EMPTY;
				++count; 
			}
		}
	}
	
	//更新 captureBlackNumber 和 captureWhiteNumber 
	if (color == WHITE) {
		captureWhiteNumber += count;
	} else if (color == BLACK) {
		captureBlackNumber += count;
	}
	
	//更新 hash 值 
	ComputeHashValue();
	
	return count;
}


//判断局面是否形成全同 
int Node::IsSuperko() {
	
	++countSuperko;
	
	Node* p = NULL;
	
	p = father;
	while (p != NULL) {
		if (hashValue == p->hashValue) {
			return 1;
		}
		p = p->father;
	}
	
	return 0;
}


//判断白方是否有无条件活的子 
//这里并没有完全实现寻找无条件活的串，只是在寻找最左侧的两个（假）眼是否足够形成无条件活的串 
//在允许自尽的规则下，如果能得到无条件活的串，能么再下几手就能必定得到两个（或以上）单眼的无条件活串 
int Node::WhiteUnconditionalAlive() {
	
	++countUnconditional;
	
	//两个假眼的坐标 
	int x1 = -1;
	int y1 = -1;
	int x2 = -1;
	int y2 = -1;
	
	//寻找位于棋盘最左侧的两个被白子包围的交叉点（即假眼） 
	for (int j = 0; j < COL; ++j) {
		for (int i = 0; i < ROW; ++i) {
			if (boardLimit[i][j] == 0 && board[i][j] == EMPTY && IsSurroundedByWhite(i, j) == 1) {
				if (x1 < 0 || y1 < 0) {
					x1 = i;
					y1 = j;
				} else {	
					x2 = i;
					y2 = j;
					goto finishSearch;
				}
			}
		}
	}
	
	finishSearch:
	
	//如果没有找到两个假眼，那么直接返回 
	if (x2 < 0 || y2 < 0 || x1 < 0 || y1 < 0) {
		return 0;
	} 
	
	int support1[ROW][COL] = {0};
	int support2[ROW][COL] = {0};
	
	//计算两个眼支持的白子 
	ComputeSupport(x1, y1, support1);
	ComputeSupport(x2, y2, support2);
	
	//判断每个眼周围的白子是否由另一个眼支持 
	if (x1 - 1 >= 0 && support2[x1 - 1][y1] == 0) {
		return 0;
	}
	if (y1 - 1 >= 0 && support2[x1][y1 - 1] == 0) {
		return 0;
	}
	if (x1 + 1 < ROW && support2[x1 + 1][y1] == 0) {
		return 0;
	}
	if (y1 + 1 < COL && support2[x1][y1 + 1] == 0) {
		return 0;
	}
	if (x2 - 1 >= 0 && support1[x2 - 1][y2] == 0) {
		return 0;
	}
	if (y2 - 1 >= 0 && support1[x2][y2 - 1] == 0) {
		return 0;
	}
	if (x2 + 1 < ROW && support1[x2 + 1][y2] == 0) {
		return 0;
	}
	if (y2 + 1 < COL && support1[x2][y2 + 1] == 0) {
		return 0;
	}
	
	return 1; 
} 


//判断某个点是否被白子包围 
int Node::IsSurroundedByWhite(int x, int y) {
	
	//判断四个方向上是否是白子 
	if (x - 1 >= 0 && board[x - 1][y] != WHITE) {
		return 0;
	}
	
	if (y - 1 >= 0 && board[x][y - 1] != WHITE) {
		return 0;
	}
	
	if (x + 1 < ROW && board[x + 1][y] != WHITE) {
		return 0;
	}
	
	if (y + 1 < COL && board[x][y + 1] != WHITE) {
		return 0;
	}
	
	return 1;
}


//计算某个空交叉点支持的白子（该点是白子的气） 
void Node::ComputeSupport(int x, int y, int support[ROW][COL]) {
	
	support[x][y] = 1;
	
	//用迭代的方式计算每个白子是否由点（x,y）提供气 
	int flag = 0;
	do {
		flag = 0;
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j) {
				
				//该点不是白子或者已经有气，那么直接跳过，检查下一个交叉点 
				if (board[i][j] != WHITE || support[i][j] == 1) {
					continue; 
				}
				
				//检查能否从四个方向上获得气 
				//上方 
				if (i - 1 >= 0 && support[i - 1][j] == 1) {
					support[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//下方 
				if (i + 1 < ROW && support[i + 1][j] == 1) {
					support[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//左侧 
				if (j - 1 >= 0 && support[i][j - 1] == 1) {
					support[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//右侧 
				if (j + 1 < COL && support[i][j + 1] == 1) {
					support[i][j] = 1;
					flag = 1;
					continue; 
				} 
			}
		}
	} while (flag == 1);
} 


//判断白方是否能做活 
int Node::WhiteAlive() {
	
	countCaptureBlack[captureBlackNumber] += 1;
	countCaptureWhite[captureWhiteNumber] += 1;
	++countWhiteAlive;
	
	//计算当前节点的手数（高度） 
	/*
	Node* node = NULL;
	node = this->father;
	int height= 0;
	while (node != NULL) {
		node = node->father;
		++height;
	}*/
	
	countTreeHeight[playNumber] += 1;
	
	//检查手数是否已经超过头文件定义的最大手数，如果已经超过则直接放弃该分支 
	if (playNumber + 1 > MAX_PLAY_NUMBER) {
		return NOTALIVE;
	} 
	//检查 CAPTURE_PLAY_MIN 手内是否有白子被提，如果被提放弃该分支 
	if (playNumber <= CAPTURE_PLAY_MIN && captureWhiteNumber > 0) {
		return NOTALIVE;
	}
	//检查白子被提的数量与黑子被提数量的差是否超过 CAPTURE_DIFFERENCE_MAX，如果超过放弃该分支 
	if (captureWhiteNumber - captureBlackNumber > CAPTURE_DIFFERENCE_MAX) {
		return NOTALIVE;
	}
	
	//用于调试 
	if (playNumber <= PRINT_TREE_HEIGHT) {
		cout << "===================================================height : " << playNumber << endl;
		Print();
		cout << "alive table size : " << deadAliveTable.size() << endl;
	}
	if (countNode % 1000000 == 0) {
		Print();
	}
	
	//在死活表中查找当前局面 
	long long key = 0; 
	key = hashValue * (turn == BLACK ? 1 : -1);
	map<long long, int>::iterator iter;
	iter = deadAliveTable.find(key);
	
	//如果白方有单调递增做活策略，且满足一定条件，那么当前局面下白方也能单调递增做活 
	if (captureWhiteNumber == 0 && iter != deadAliveTable.end() && iter->second == MONOTONICALIVE) {
		if (BOARD_TYPE == 2 || turn == WHITE) {
			//cout << "MONOTONICALIVE" << endl;
			return MONOTONICALIVE;
		}
	}
	
	//临时节点指针 
	Node* next = NULL;
	
	//着法的类型 
	int play = 0;
	
	//用于保存当前节点之后的各种着法形成的新节点 
	Node* array[4][ROW * COL] = {0};	//array[0] 是提子的着法，array[1] 是形成同形的着法，array[2] 是普通着法，array[3] 是自尽的着法 
	int length[4] = {0};
	
	//用于保存结果 
	int result = NOTALIVE;
	
	next = new Node;
	
	if (turn == WHITE) {
		//轮到白方行动 
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j) {
				
				//落子在点(i,j) 
				next->Copy(this);
				next->father = this; 
				play = next->Play(i, j);
				
				//白方落子后，已经有无条件活的白子 
				if ((play == NORMAL || play == CAPTURE) && next->WhiteUnconditionalAlive() == 1) {
					result = MONOTONICALIVE;
					goto returnResult;
				}
				
				//将提子着法和普通着法存入数组 array，形成同形和自尽的着法直接放弃 
				if (play == CAPTURE) {
					array[0][length[0]] = next;
					++length[0];
					next = new Node;
				} else if (play == NORMAL) {
					array[2][length[2]] = next;
					++length[2];
					next = new Node;
				}
			}
		} 
		
		int notAlivePlay[4] = {0};
		
		//改进白方的着法顺序 
		notAlivePlay[0] = ImproveWhiteOrder(array[0], length[0]);
		notAlivePlay[2] = ImproveWhiteOrder(array[2], length[2]);
		
		//检查 array 数组中保存的着法是否能做活 
		int whiteAliveResult = 0; 
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < length[i] - notAlivePlay[i]; ++j) {
				whiteAliveResult = array[i][j]->WhiteAlive();
				if (whiteAliveResult != NOTALIVE) {
					//白方有一个着法可以做活 
					result = whiteAliveResult;
					goto returnResult;
				}
			}
		}	
		
		//白方没有着法可以做活 
		result = NOTALIVE;
		goto returnResult;
		
	} else {
		//轮到黑方行动 
		result = MONOTONICALIVE; 
		
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j) {
				
				//落子在点(i,j)
				next->Copy(this);
				next->father = this; 
				play = next->Play(i, j);
				
				//如果白子已经被提大于文件头部设置的 CAPTURE_WHITE_LIMIT，那么默认白方无法做活放弃这个分支 
				if (next->captureWhiteNumber > CAPTURE_WHITE_LIMIT) {
					//if (play == CAPTURE || (play == SUPERKO && BLACKSIMPLEKO == 0)) {
					if (play == CAPTURE || (play == SUPERKO && BOARD_TYPE == 2)) {
						result = NOTALIVE;
						goto returnResult;
					}
				}
				
				//将着法存入数组 array 
				if (play == CAPTURE) {
					
					array[0][length[0]] = next;
					++length[0];
					next = new Node;
					result = NORMALALIVE;
				} else if (play == SUPERKO) {
					//如果该黑方的着法形成同形 
					result = NORMALALIVE;
					//在 BLACKSIMPLEKO = 1 配置下，不检查单劫立即提回的着法（即循环长度为 2 的着法） 
					if (BOARD_TYPE != 2 && next->hashValue == father->hashValue) {
						;
					} else {
						array[1][length[1]] = next;
						++length[1];
						next = new Node;
					}
				} else if (play == NORMAL) {
					array[2][length[2]] = next;
					++length[2];
					next = new Node;
				} else if (play == SUICIDE) {
					array[3][length[3]] = next;
					++length[3];
					next = new Node;
				}
			}
		}
		
		//改进黑方的着法顺序 
		ImproveBlackOrder(array[0], length[0]);
		ImproveBlackOrder(array[1], length[1]);
		ImproveBlackOrder(array[2], length[2]);
		ImproveBlackOrder(array[3], length[3]);
		
		int whiteAliveResult = 0; 
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < length[i]; ++j) {
				whiteAliveResult = array[i][j]->WhiteAlive();
				if (whiteAliveResult == NOTALIVE) {
					//黑方有一个着法可以阻止白方做活 
					result = NOTALIVE;
					goto returnResult;
				} else if (whiteAliveResult == NORMALALIVE) {
					//黑方有一个着法使得白方不能单调递增做活 
					result = NORMALALIVE;
				}
			}
		}
		
		//检查黑方的 pass 着法是否能阻止白方做活 
		//虽然通常 pass 着法是不利的，但是为了完整性还是进行搜索 
		next->Copy(this);
		next->father = this; 
		play = next->Play(-1, -1);
		whiteAliveResult = next->WhiteAlive();
		if (whiteAliveResult == NOTALIVE) {
			result = NOTALIVE;
			goto returnResult;
		} else if (whiteAliveResult == NORMALALIVE) {
			result = NORMALALIVE;
		}
		
		//黑方没有着法可以阻止白方做活 
		goto returnResult;
	}
	
	returnResult: 
	
	//将搜索结果存入表 deadAliveTable 
	if (iter == deadAliveTable.end() && playNumber <= TABLE_MAX_HEIGHT) {
		
		//当死活表规模过大时，清空死活表避免容器崩溃 
		if (deadAliveTable.size() > 10000000) {
			deadAliveTable.clear();
		}
		
		deadAliveTable.insert(pair<long long, int>(key, result));
		
		if (result == NOTALIVE) {
			++countNotAlive;
		} else if (result == NORMALALIVE) {
			++countNormalAlive;
		} else if (result == MONOTONICALIVE) {
			++countMonotonicAlive;
		} 
	}
	
	//删除对象，释放内存 
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < length[i]; ++j) {
			delete array[i][j];
		} 
	}
	if (next != NULL) {
		delete next;
	}
	
	return result;
}


//依据 key 数组，对数组 array 排序 
void Node::Sort(Node* array[ROW * COL], int key[ROW * COL], int length) {
	
	//选择排序 
	int max = 0;
	int temp = 0;
	Node* node = NULL;
	
	for (int i = 0; i < length - 1; ++i) {
		
		//查找最大值 
		max = 0;
		for (int j = 0; j < length - i; ++j) {
			if (key[j] > key[max]) {
				max = j;
			}
		}
		
		//交换下标为 max 和 length - 1 -i 的元素 
		temp = key[max];
		key[max] = key[length - 1 - i];
		key[length - 1 - i] = temp;
		
		node = array[max];
		array[max] = array[length - 1 - i];
		array[length - 1 - i] = node;
	}
}


//改进黑方的着法顺序 
void Node::ImproveBlackOrder(Node* array[ROW * COL], int length) {
	int result[ROW * COL] = {0};
	long long key = 0;
	
	map<long long, int>::iterator iter;
	for (int i = 0; i < length; ++i) {
		key = array[i]->hashValue * (array[i]->turn == BLACK ? 1 : -1);
		iter = deadAliveTable.find(key);
		if (iter != deadAliveTable.end()) {
			result[i] = iter->second;
		}
	}
	
	Sort(array, result, length);
}


//改进白方的着法顺序 
int Node::ImproveWhiteOrder(Node* array[ROW * COL], int length) {
	int result[ROW * COL] = {0};
	long long key = 0;
	int notAlive = 0;
	
	map<long long, int>::iterator iter;
	for (int i = 0; i < length; ++i) {
		key = array[i]->hashValue * (array[i]->turn == BLACK ? 1 : -1);
		iter = deadAliveTable.find(key);
		if (iter != deadAliveTable.end()) {
			result[i] = iter->second * -1;
			
			
			if (iter->second == NOTALIVE) {
				++notAlive;
			}
		}
	}
	
	Sort(array, result, length);
	
	return notAlive;
}
//============================================================================= Node 类函数定义 开始 


//=============================================================================主程序 开始 
//测试 Node 类的基本功能 
void elementaryTest() {
	cout << "elementary test :" << endl;
	
	Node* r = new Node;
	r->Print();
	
	r->board[0][3] = BLACK;
	r->board[1][4] = WHITE;
	r->turn = WHITE;
	r->father = r;
	r->captureBlackNumber = 4;
	r->captureWhiteNumber = 2;
	
	r->ComputeHashValue();
	
	Node* s = new Node;
	s->Copy(r);
	s->Print();
	
	s->Random();
	s->Print();
	
	int key[20] = {0};
	Node* array[20] = {0};
	for (int i = 0; i < 20; ++i) {
		key[i] = rand();
		array[i] = (Node*)(rand());
	}
	
	for (int i = 0; i < 20; ++i) {
		cout << i << " " << key[i] << " " << array[i] << endl;
	}
	
	s->Sort(array, key, 20);
	
	cout << "sort() :" << endl;
	for (int i = 0; i < 20; ++i) {
		cout << i << " " << key[i] << " " << array[i] << endl;
	}
	
	delete r;
	delete s;
} 


//测试 Node 类的落子函数及相关函数 
void playTest() {
	cout << "play test :" << endl;
	
	Node* r = new Node;
	
	int x = 0;
	int y = 0;
	int play = 0;
	
	for (int i = 0; i < 30; ++i) {
		x = rand() % ROW;
		y = rand() % COL;
		cout << "step : " << i << endl;
		cout << "x y : " << x << " " << y << endl;
		cout << "play : " << r->Play(x, y) << endl;
		r->Print();
	}
	
	Node* s = new Node;
	Node* t = new Node;
	
	t->father = s;
	s->Play(0, 1);
	
	cout << t->Play(0, 1) << endl;
	
	s->board[0][1] = WHITE;
	s->board[1][0] = WHITE;
	s->board[1][1] = EMPTY;
	s->turn = BLACK;
	s->Print();
	cout << s->Play(0, 0);
	s->Print();
	
	delete s;
	delete t;
	delete r;
}


//测试 Node 类的判断白方是否有无条件活子函数及相关函数 
void unconditionalTest() {
	cout << "unconditional alive test :" << endl;
	
	Node* r = new Node;
	r->Random();
	r->Print();
	
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			if (r->IsSurroundedByWhite(i, j)) {
				cout << "1 ";
			} else {
				cout << "0 ";
			}
		}
		cout << endl;
	}
	
	r->Random();
	r->Print();
	int support[ROW][COL] = {0};
	r->ComputeSupport(1, 3, support);
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			cout << support[i][j] << " ";
		}
		cout << endl;
	}
	
	r->Random();
	r->Print();
	cout << r->WhiteUnconditionalAlive() << endl;
	
	do {
		r->Random();
	} while (r->WhiteUnconditionalAlive() == 0);
	
	r->Print();
	
	delete r;
}


void whiteAliveTest() {
	int result = 0;
	result = searchNode->WhiteAlive();
	
	cout << endl;
	cout << "black capture number : " << endl;
	for (int i = 0; i <= MAX_PLAY_NUMBER / 2; ++i) {
		cout << i << "\t" << countCaptureBlack[i] << endl;
	}
	
	cout << endl;
	cout << "white capture number : " << endl;
	for (int i = 0; i <= CAPTURE_WHITE_LIMIT; ++i) {
		cout << i << "\t" << countCaptureWhite[i] << endl;
	}
	
	cout << endl;
	cout << "tree height number : " << endl;
	for (int i = 0; i < 2 * ROW * COL + 100; ++i) {
		if (i > 10 && countTreeHeight[i] == 0) {
			break;
		}
		cout << i << "\t" << countTreeHeight[i] << endl;
	}
	
	cout << endl;
	cout << "Node number            : " << countNode / 10000 << "\t * 10^4" << endl;
	cout << "WhiteAlive() number    : " << countWhiteAlive / 10000 << "\t * 10^4" << endl;
	cout << "Play() number          : " << countPlay / 10000 << "\t * 10^4" << endl;
	cout << "IsSuperko() number     : " << countSuperko / 10000 << "\t * 10^4" << endl;
	cout << "Unconditional() number : " << countUnconditional / 10000 << "\t * 10^4" << endl;
	
	cout << endl;
	cout << "dead alive table size  : " << deadAliveTable.size() << endl;
	cout << "not alive number       : " << countNotAlive << endl;
	cout << "normal alive number    : " << countNormalAlive << endl;
	cout << "monotonic alive number : " << countMonotonicAlive << endl;
	
	cout << endl << "RESULT : " << endl;
	
	//打印一些配置 
	cout << "white capture limit    : " << CAPTURE_WHITE_LIMIT << endl;
	//cout << "black simple ko     : " << (BLACKSIMPLEKO == 1 ? "Yes" : "No") << endl;
	cout << "max play number        : " << MAX_PLAY_NUMBER << endl;
	cout << "capture play min       : " << CAPTURE_PLAY_MIN << endl;
	cout << "capture difference max : " << CAPTURE_DIFFERENCE_MAX << endl;
	
	searchNode->Print();
	//cout << node->hashValue << endl;
	cout << endl << "white alive : ";
	if (result == NOTALIVE) {
		cout << "NOT ALIVE !" << endl;
	} else if (result == NORMALALIVE) {
		cout << "NORMAL ALIVE !" << endl;
	} else if (result == MONOTONICALIVE) {
		cout << "MONOTONIC ALIVE !" << endl;
	} else {
		cout << "ERROR !" << endl;
	}
}


//初始化 
void init() {
	
	cout << ">>>> init begin" << endl; 
	
	//打印棋盘大小 
	cout << "board size : " << ROW << " " << COL << endl;
	
	//初始化受限棋盘数组
	if (BOARD_TYPE == 1) {
		//棋盘右侧受限 
		for (int i = 0; i < ROW; ++i) {
			boardLimit[i][COL - 1] = 1;
		}
	} else if (BOARD_TYPE == 2) {
		//棋盘左侧和右侧受限
		for (int i = 0; i < ROW; ++i) {
			boardLimit[i][0] = 1;
			boardLimit[i][COL - 1] = 1;
		}
	} else if (BOARD_TYPE == 3) {
		//棋盘右侧和下侧受限
		for (int i = 0; i < ROW; ++i) {
			boardLimit[i][COL - 1] = 1;
		}
		for (int j = 0; j < COL; ++j) {
			boardLimit[ROW - 1][j] = 1;
		}
	}
	
	//打印受限棋盘 
	cout << "board limit :" << endl;
	for (int i = 0; i < ROW; ++i) {
		cout << "\t"; 
		for (int j = 0; j < COL; ++j) {
			cout << boardLimit[i][j] << " ";
		}
		cout << endl;
	}
	
	//打印一些配置 
	cout << "white capture limit    : " << CAPTURE_WHITE_LIMIT << endl;
	//cout << "black simple ko     : " << (BLACKSIMPLEKO == 1 ? "Yes" : "No") << endl;
	cout << "max play number        : " << MAX_PLAY_NUMBER << endl;
	cout << "capture play min       : " << CAPTURE_PLAY_MIN << endl;
	cout << "capture difference max : " << CAPTURE_DIFFERENCE_MAX << endl;
	
	//打印初始着法形成的局面 
	cout << "init play : " << endl;
	Node* p = new Node;
	p->Print();
	Node* q = NULL;
	for (int i = 0; i < INIT_PLAY_NUMBER; ++i) {
		q = new Node;
		q->Copy(p);
		q->father = p;
		q->Play(INIT_PLAY[i][0], INIT_PLAY[i][1]);
		p = q;
		q->Print();
	}
	
	searchNode = q;
	
	cout << ">>>> init end" << endl; 
}


//用于测试各模块功能 
void test() {
	
	//elementaryTest();
	
	//playTest(); 
	
	//unconditionalTest();
	
	whiteAliveTest();
}


int main() {
	
	srand(time(NULL));
	
	int startTime = time(NULL);
	
	init(); 
	 
	test();
	
	int finishTime = time(NULL);
	
	//cout << endl;
	
	cout << "total time  : " << finishTime - startTime << " s" << endl;
	
	//char c = 0;
	//cin >> c;
	
	return 0;
} 
//=============================================================================主程序 结束 


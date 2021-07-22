
//	2021.7.22 
//	ƽ������ֵ���� 


#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>

using namespace std;


//=============================================================================�������� ��ʼ 
//�������޵ľ������̴�С��ROW Ϊ�������߶ȣ���COL Ϊ��������ȣ� 
const int ROW = 3;
const int COL = 6;

//�����������ͣ�1 ��ʾ�Ҳ����ޣ�2 ��ʾ�����Ҳ����ޣ�3 ��ʾ�Ҳ���²����� 
const int BOARD_TYPE = 1;			

//��������ʱ�Ծ����������ֵ 
const int MAX_PLAY_NUMBER = 19;		

//���ð׷������ӵ����������ֵ 
const int CAPTURE_WHITE_LIMIT = 2;	

//�� CAPTURE_PLAY_MIN �����а��ӱ��ᣬ�����÷�֧ 
const int CAPTURE_PLAY_MIN = 0; 

//�����ӱ������������ӱ��������Ĳ�� CAPTURE_DIFFERENCE_MAX�������÷�֧ 
const int CAPTURE_DIFFERENCE_MAX = 1;

//���ó�ʼ�ŷ� 
const int INIT_PLAY_NUMBER = 3;		//��ʼ�ŷ����� 
const int INIT_PLAY[INIT_PLAY_NUMBER + 1][2] = {
	{1,1},		//�׷��� 1 �� 
	{-1,-1},		//�ڷ��� 2 �� 
	{1,0},		//�׷��� 3 �� 
	
	{-1,-1}
};

const int TABLE_MAX_HEIGHT = 27;

//���ô�ӡ�ڵ�ĸ߶ȵ����ֵ 
const int PRINT_TREE_HEIGHT = 7; 
//=============================================================================�������� ���� 


//=============================================================================�������� ���� 
//������ɫ�����ڱ�ʾ���̽�������ɫ���ж��� 
const int EMPTY	= 0;
const int BLACK = 1;
const int WHITE = 2;
 
//�������ֵ����� 
const int PASS    = 101;	//��ʾ���ţ������ӵ㳬�����̷�ΧʱĬ��Ϊ���� 
const int SUPERKO = 102;	//��ʾ�γ�ͬ������ 
const int CAPTURE = 103;	//��ʾ�����ͬ�ε���ȡ�Է����ӵ��ŷ�
const int SUICIDE = 104;	//��ʾ�����ͬ�ε��Ծ����ŷ�������ǵ��������Ծ������Զ�ת��Ϊ���� 
const int NORMAL  = 105;	//��ʾ�����ͬ�ε�û�����ӵĺϷ��ŷ� 
const int ILLEGAL = 106;	//��ʾ���Ϸ����ŷ������ӵ��������ӣ� 

//������������� 
const int NOTALIVE       = -1;		//��ʾ�׷��������� 
const int NORMALALIVE    = 1;		//��ʾ�׷��������� 
const int MONOTONICALIVE = 2;		//��ʾ�׷�������������������û�б����� 

//���̵�����λ�� 
const int CENTERX = (ROW - 1) / 2;
const int CENTERY = (COL - 1) / 2;
//=============================================================================�������� ���� 


//=============================================================================ȫ�ֱ��� ��ʼ 
//��ʾÿ��������Ƿ����޵�����
//0 ��ʾ˫���������ӣ�1 ��ʾ���ڷ������ӣ����ڸõ�ĺ���ʼ�ձ���Ϊ������ 
//�� init() �����и����������ͽ��г�ʼ�� 
int boardLimit[ROW][COL] = {0};		

//���������֮ǰ����ľ����°׷��ܷ�������ڲ�ѯ 
map<long long, int> deadAliveTable;

//ȫ�����飬���ڼ��������ʼ�����ĵ�ʱ�� 
int liberty[ROW][COL] = {0};
//=============================================================================ȫ�ֱ��� ���� 


//=============================================================================������ ���� 
//�ڵ������ 
long long countNode                            = 0;		//�ܽڵ��������� 
long long countCaptureBlack[MAX_PLAY_NUMBER]   = {0};	//���ӱ���ڵ������ 
long long countCaptureWhite[MAX_PLAY_NUMBER]   = {0};	//���ӱ���ڵ������ 
long long countTreeHeight[MAX_PLAY_NUMBER + 2] = {0};	//�ڵ���ȣ������������� 

//�������ô��������� 
long long countWhiteAlive    = 0;	//�жϰ׷��ܷ������������ 
long long countPlay          = 0;	//���Ӻ��������� 
long long countSuperko       = 0;	//�ж�ȫͬ���������� 
long long countUnconditional = 0;	//�ж���������������� 

//�������ؼ����� 
long long countNotAlive       = 0;
long long countNormalAlive    = 0;
long long countMonotonicAlive = 0;
//=============================================================================������ ���� 


//============================================================================= Node ������ ��ʼ 
class Node {
public:
	
	//����ֵ��Ա 
	int board[ROW][COL];	//��ʾ���� 
	int turn;				//��ʾ�ֵ��ķ��ж� 
	
	//����ֵ��Ա 
	Node* father;			//��ʾ���ڵ� 
	long long hashValue;	//��ʾ��ǰ����� hash ֵ�������жϾ����Ƿ�ͬ�� 
	int captureBlackNumber;	//��ʾĿǰ���ӱ���������� 
	int captureWhiteNumber;	//��ʾĿǰ���ӱ���������� 
	int playNumber;			//��ʾ��ǰ�Ѿ��µ����� 
	
	
	//���캯�� 
	Node();
	
	//���ƺ���
	void Copy(Node* node);
	 
	//��ӡ���������ڲ鿴ֵ��Ա 
	void Print();
	
	//�������һ�����溯�� 
	void Random();
	
	//���� hash ֵ���� 
	void ComputeHashValue();
	
	//���Ӻ��� 
	int Play(int x, int y);
	
	//����ÿ��������ĺ��� 
	//void ComputeLiberty(int liberty[ROW][COL]);
	void ComputeLiberty();
	
	//�����ɫΪ color ���������ӣ������������������ 
	int CaptureStones(int liberty[ROW][COL], int color);
	
	//�жϾ����Ƿ��γ�ȫͬ 
	int IsSuperko(); 
	
	//�жϰ׷��Ƿ�������������� 
	int WhiteUnconditionalAlive();
	
	//�ж�ĳ�����Ƿ񱻰��Ӱ�Χ 
	int IsSurroundedByWhite(int x, int y);
	
	//����ĳ���ս����֧�ֵİ��ӣ��õ��ǰ��ӵ����� 
	void ComputeSupport(int x, int y, int support[ROW][COL]);
	
	//�жϰ׷��Ƿ������� 
	int WhiteAlive();
	
	//���� key ���飬������ array ���� 
	void Sort(Node* array[ROW * COL], int key[ROW * COL], int length);
	
	//�Ľ��ڷ����ŷ�˳�� 
	void ImproveBlackOrder(Node* array[ROW * COL], int length);
	
	//�Ľ��׷����ŷ�˳�� 
	int ImproveWhiteOrder(Node* array[ROW * COL], int length);
}; 

//ȫ�ֱ��������ڵ�ָ�룬�� init() �����н��г��軯 
Node* searchNode = NULL; 
//============================================================================= Node ������ ��ʼ 


//============================================================================= Node �ຯ������ ��ʼ 
//���캯�� 
Node::Node() {
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			board[i][j] = EMPTY;
		}
	}
	turn = WHITE;	//��������£��׷����� 
	
	father = NULL; 
	hashValue = 0;
	captureBlackNumber = 0; 
	captureWhiteNumber = 0; 
	playNumber = 0;
	
	++countNode; 
	
	///*������ 
	//ÿ����һ������ڵ���ӡһ�Σ����ڷ�ӳ������������ 
	if (countNode % 1000000 == 0) {
		cout << "node number : " << countNode / 1000000 << " * 10^6" << endl;
	}
	//*/
}


//���ƺ��� 
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


//��ӡ���������ڲ鿴ֵ��Ա 
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


//���� hash ֵ���� 
void Node::ComputeHashValue() {
	long long base = 1;
	hashValue = 0; 
	
	//�����̱�ʾΪ 3 ����������Ϊ hash ֵ 
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


//�������һ�����溯���������ڲ��� 
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


//���Ӻ��� 
int Node::Play(int x, int y) {
	
	++countPlay;
	 
	//�����ͶԷ�����ɫ 
	int player = turn;
	int opponent = (turn == BLACK ? WHITE : BLACK);
	
	//�� x �� y �ķ�Χ�������̣�Ĭ��Ϊ pass 
	if (x < 0 || y < 0 || x >= ROW || y >= COL) {
		turn = opponent;
		++playNumber;
		return PASS;
	}
	
	//��λ�� (x,y) �������ӣ���Ϊ�Ƿ��ŷ������ı� turn��ֱ�ӷ��� 
	if (board[x][y] != EMPTY) {
		return ILLEGAL;
	}
	
	//���׷����������޵��ϣ���Ϊ�Ƿ��ŷ������ı� turn��ֱ�ӷ��� 
	if (turn == WHITE && boardLimit[x][y] == 1) {
		return ILLEGAL;
	}
	
	//���� 
	board[x][y] = player;
	
	//������������ 
	//int liberty[ROW][COL] = {0};
	
	//����ÿ��������Ƿ����� 
	//ComputeLiberty(liberty); 
	ComputeLiberty();
	
	//�������� 
	int count = 0;
	
	//���Ӻ���Ҫ�ı��ж��� 
	turn = opponent;
	
	//������ȡ���ֵ����� 
	count = CaptureStones(liberty, opponent);
	
	//�����ȡ�˶Է������ӣ������Ƿ�ͬ�κ󷵻� 
	if (count > 0) {
		if (IsSuperko() == 1) {
			++playNumber;
			return SUPERKO;
		} else {
			++playNumber;
			return CAPTURE;
		}
	}
	
	//������ȡ���������� 
	count = CaptureStones(liberty, player);
	
	if (count == 1) {
		//���������Ծ����Զ�תΪ pass 
		++playNumber;
		return PASS;
	} else {
		if (IsSuperko() == 1) {
			++playNumber;
			return SUPERKO;
		} else {
			if (count > 1) {
				//��������Ծ� 
				++playNumber;
				return SUICIDE;
			} else {
				//�������ŷ� 
				++playNumber;
				return NORMAL;
			}
		}
	} 
	
	cout << "ERROR : Node::Play()" << endl;
	return 0;
}


//����ÿ��������ĺ��� 
//void Node::ComputeLiberty(int liberty[ROW][COL]) {
void Node::ComputeLiberty() {
	
	//�յĽ��������޵ĵ㱻��Ϊ�������� 
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			if (board[i][j] == EMPTY || boardLimit[i][j] == 1) {
				liberty[i][j] = 1;
			} else {
				liberty[i][j] = 0;
			}
		}
	}
	
	//�õ����ķ�ʽ����ÿ��������Ƿ�������ÿ��ѭ���Ƿ����µĵ㱻����Ϊ�����ģ�����������ѭ�������û������ֹѭ�� 
	int flag = 0;
	do {
		flag = 0;
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j) {
				
				//���(i,j)�Ѿ���������ֱ�������������һ�������
				if (liberty[i][j] == 1) {
					continue; 
				}
				
				//����(i,j)�ܷ���ĸ������ϻ���� 
				//�Ϸ� 
				if (i - 1 >= 0 && (board[i - 1][j] == EMPTY || (board[i - 1][j] == board[i][j] && liberty[i - 1][j] == 1))) {
					liberty[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//�·� 
				if (i + 1 < ROW && (board[i + 1][j] == EMPTY || (board[i + 1][j] == board[i][j] && liberty[i + 1][j] == 1))) {
					liberty[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//��� 
				if (j - 1 >= 0 && (board[i][j - 1] == EMPTY || (board[i][j - 1] == board[i][j] && liberty[i][j - 1] == 1))) {
					liberty[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//�Ҳ� 
				if (j + 1 < COL && (board[i][j + 1] == EMPTY || (board[i][j + 1] == board[i][j] && liberty[i][j + 1] == 1))) {
					liberty[i][j] = 1;
					flag = 1;
					continue; 
				} 
			}
		}
	} while (flag == 1);
}


//�����ɫΪ color ���������ӣ������������������ 
int Node::CaptureStones(int liberty[ROW][COL], int color) {
	
	//�������� 
	int count = 0;
	
	//���� liberty ���飬�����ɫΪ color ���������� 
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			if (board[i][j] == color && liberty[i][j] == 0) {
				board[i][j] = EMPTY;
				++count; 
			}
		}
	}
	
	//���� captureBlackNumber �� captureWhiteNumber 
	if (color == WHITE) {
		captureWhiteNumber += count;
	} else if (color == BLACK) {
		captureBlackNumber += count;
	}
	
	//���� hash ֵ 
	ComputeHashValue();
	
	return count;
}


//�жϾ����Ƿ��γ�ȫͬ 
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


//�жϰ׷��Ƿ�������������� 
//���ﲢû����ȫʵ��Ѱ����������Ĵ���ֻ����Ѱ���������������٣����Ƿ��㹻�γ���������Ĵ� 
//�������Ծ��Ĺ����£�����ܵõ���������Ĵ�����ô���¼��־��ܱض��õ������������ϣ����۵�������� 
int Node::WhiteUnconditionalAlive() {
	
	++countUnconditional;
	
	//�������۵����� 
	int x1 = -1;
	int y1 = -1;
	int x2 = -1;
	int y2 = -1;
	
	//Ѱ��λ���������������������Ӱ�Χ�Ľ���㣨�����ۣ� 
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
	
	//���û���ҵ��������ۣ���ôֱ�ӷ��� 
	if (x2 < 0 || y2 < 0 || x1 < 0 || y1 < 0) {
		return 0;
	} 
	
	int support1[ROW][COL] = {0};
	int support2[ROW][COL] = {0};
	
	//����������֧�ֵİ��� 
	ComputeSupport(x1, y1, support1);
	ComputeSupport(x2, y2, support2);
	
	//�ж�ÿ������Χ�İ����Ƿ�����һ����֧�� 
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


//�ж�ĳ�����Ƿ񱻰��Ӱ�Χ 
int Node::IsSurroundedByWhite(int x, int y) {
	
	//�ж��ĸ��������Ƿ��ǰ��� 
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


//����ĳ���ս����֧�ֵİ��ӣ��õ��ǰ��ӵ����� 
void Node::ComputeSupport(int x, int y, int support[ROW][COL]) {
	
	support[x][y] = 1;
	
	//�õ����ķ�ʽ����ÿ�������Ƿ��ɵ㣨x,y���ṩ�� 
	int flag = 0;
	do {
		flag = 0;
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j) {
				
				//�õ㲻�ǰ��ӻ����Ѿ���������ôֱ�������������һ������� 
				if (board[i][j] != WHITE || support[i][j] == 1) {
					continue; 
				}
				
				//����ܷ���ĸ������ϻ���� 
				//�Ϸ� 
				if (i - 1 >= 0 && support[i - 1][j] == 1) {
					support[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//�·� 
				if (i + 1 < ROW && support[i + 1][j] == 1) {
					support[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//��� 
				if (j - 1 >= 0 && support[i][j - 1] == 1) {
					support[i][j] = 1;
					flag = 1;
					continue; 
				} 
				
				//�Ҳ� 
				if (j + 1 < COL && support[i][j + 1] == 1) {
					support[i][j] = 1;
					flag = 1;
					continue; 
				} 
			}
		}
	} while (flag == 1);
} 


//�жϰ׷��Ƿ������� 
int Node::WhiteAlive() {
	
	countCaptureBlack[captureBlackNumber] += 1;
	countCaptureWhite[captureWhiteNumber] += 1;
	++countWhiteAlive;
	
	//���㵱ǰ�ڵ���������߶ȣ� 
	/*
	Node* node = NULL;
	node = this->father;
	int height= 0;
	while (node != NULL) {
		node = node->father;
		++height;
	}*/
	
	countTreeHeight[playNumber] += 1;
	
	//��������Ƿ��Ѿ�����ͷ�ļ�������������������Ѿ�������ֱ�ӷ����÷�֧ 
	if (playNumber + 1 > MAX_PLAY_NUMBER) {
		return NOTALIVE;
	} 
	//��� CAPTURE_PLAY_MIN �����Ƿ��а��ӱ��ᣬ�����������÷�֧ 
	if (playNumber <= CAPTURE_PLAY_MIN && captureWhiteNumber > 0) {
		return NOTALIVE;
	}
	//�����ӱ������������ӱ��������Ĳ��Ƿ񳬹� CAPTURE_DIFFERENCE_MAX��������������÷�֧ 
	if (captureWhiteNumber - captureBlackNumber > CAPTURE_DIFFERENCE_MAX) {
		return NOTALIVE;
	}
	
	//���ڵ��� 
	if (playNumber <= PRINT_TREE_HEIGHT) {
		cout << "===================================================height : " << playNumber << endl;
		Print();
		cout << "alive table size : " << deadAliveTable.size() << endl;
	}
	if (countNode % 1000000 == 0) {
		Print();
	}
	
	//��������в��ҵ�ǰ���� 
	long long key = 0; 
	key = hashValue * (turn == BLACK ? 1 : -1);
	map<long long, int>::iterator iter;
	iter = deadAliveTable.find(key);
	
	//����׷��е�������������ԣ�������һ����������ô��ǰ�����°׷�Ҳ�ܵ����������� 
	if (captureWhiteNumber == 0 && iter != deadAliveTable.end() && iter->second == MONOTONICALIVE) {
		if (BOARD_TYPE == 2 || turn == WHITE) {
			//cout << "MONOTONICALIVE" << endl;
			return MONOTONICALIVE;
		}
	}
	
	//��ʱ�ڵ�ָ�� 
	Node* next = NULL;
	
	//�ŷ������� 
	int play = 0;
	
	//���ڱ��浱ǰ�ڵ�֮��ĸ����ŷ��γɵ��½ڵ� 
	Node* array[4][ROW * COL] = {0};	//array[0] �����ӵ��ŷ���array[1] ���γ�ͬ�ε��ŷ���array[2] ����ͨ�ŷ���array[3] ���Ծ����ŷ� 
	int length[4] = {0};
	
	//���ڱ����� 
	int result = NOTALIVE;
	
	next = new Node;
	
	if (turn == WHITE) {
		//�ֵ��׷��ж� 
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j) {
				
				//�����ڵ�(i,j) 
				next->Copy(this);
				next->father = this; 
				play = next->Play(i, j);
				
				//�׷����Ӻ��Ѿ�����������İ��� 
				if ((play == NORMAL || play == CAPTURE) && next->WhiteUnconditionalAlive() == 1) {
					result = MONOTONICALIVE;
					goto returnResult;
				}
				
				//�������ŷ�����ͨ�ŷ��������� array���γ�ͬ�κ��Ծ����ŷ�ֱ�ӷ��� 
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
		
		//�Ľ��׷����ŷ�˳�� 
		notAlivePlay[0] = ImproveWhiteOrder(array[0], length[0]);
		notAlivePlay[2] = ImproveWhiteOrder(array[2], length[2]);
		
		//��� array �����б�����ŷ��Ƿ������� 
		int whiteAliveResult = 0; 
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < length[i] - notAlivePlay[i]; ++j) {
				whiteAliveResult = array[i][j]->WhiteAlive();
				if (whiteAliveResult != NOTALIVE) {
					//�׷���һ���ŷ��������� 
					result = whiteAliveResult;
					goto returnResult;
				}
			}
		}	
		
		//�׷�û���ŷ��������� 
		result = NOTALIVE;
		goto returnResult;
		
	} else {
		//�ֵ��ڷ��ж� 
		result = MONOTONICALIVE; 
		
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j) {
				
				//�����ڵ�(i,j)
				next->Copy(this);
				next->father = this; 
				play = next->Play(i, j);
				
				//��������Ѿ���������ļ�ͷ�����õ� CAPTURE_WHITE_LIMIT����ôĬ�ϰ׷��޷�������������֧ 
				if (next->captureWhiteNumber > CAPTURE_WHITE_LIMIT) {
					//if (play == CAPTURE || (play == SUPERKO && BLACKSIMPLEKO == 0)) {
					if (play == CAPTURE || (play == SUPERKO && BOARD_TYPE == 2)) {
						result = NOTALIVE;
						goto returnResult;
					}
				}
				
				//���ŷ��������� array 
				if (play == CAPTURE) {
					
					array[0][length[0]] = next;
					++length[0];
					next = new Node;
					result = NORMALALIVE;
				} else if (play == SUPERKO) {
					//����úڷ����ŷ��γ�ͬ�� 
					result = NORMALALIVE;
					//�� BLACKSIMPLEKO = 1 �����£�����鵥��������ص��ŷ�����ѭ������Ϊ 2 ���ŷ��� 
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
		
		//�Ľ��ڷ����ŷ�˳�� 
		ImproveBlackOrder(array[0], length[0]);
		ImproveBlackOrder(array[1], length[1]);
		ImproveBlackOrder(array[2], length[2]);
		ImproveBlackOrder(array[3], length[3]);
		
		int whiteAliveResult = 0; 
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < length[i]; ++j) {
				whiteAliveResult = array[i][j]->WhiteAlive();
				if (whiteAliveResult == NOTALIVE) {
					//�ڷ���һ���ŷ�������ֹ�׷����� 
					result = NOTALIVE;
					goto returnResult;
				} else if (whiteAliveResult == NORMALALIVE) {
					//�ڷ���һ���ŷ�ʹ�ð׷����ܵ����������� 
					result = NORMALALIVE;
				}
			}
		}
		
		//���ڷ��� pass �ŷ��Ƿ�����ֹ�׷����� 
		//��Ȼͨ�� pass �ŷ��ǲ����ģ�����Ϊ�������Ի��ǽ������� 
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
		
		//�ڷ�û���ŷ�������ֹ�׷����� 
		goto returnResult;
	}
	
	returnResult: 
	
	//�������������� deadAliveTable 
	if (iter == deadAliveTable.end() && playNumber <= TABLE_MAX_HEIGHT) {
		
		//��������ģ����ʱ��������������������� 
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
	
	//ɾ�������ͷ��ڴ� 
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


//���� key ���飬������ array ���� 
void Node::Sort(Node* array[ROW * COL], int key[ROW * COL], int length) {
	
	//ѡ������ 
	int max = 0;
	int temp = 0;
	Node* node = NULL;
	
	for (int i = 0; i < length - 1; ++i) {
		
		//�������ֵ 
		max = 0;
		for (int j = 0; j < length - i; ++j) {
			if (key[j] > key[max]) {
				max = j;
			}
		}
		
		//�����±�Ϊ max �� length - 1 -i ��Ԫ�� 
		temp = key[max];
		key[max] = key[length - 1 - i];
		key[length - 1 - i] = temp;
		
		node = array[max];
		array[max] = array[length - 1 - i];
		array[length - 1 - i] = node;
	}
}


//�Ľ��ڷ����ŷ�˳�� 
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


//�Ľ��׷����ŷ�˳�� 
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
//============================================================================= Node �ຯ������ ��ʼ 


//=============================================================================������ ��ʼ 
//���� Node ��Ļ������� 
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


//���� Node ������Ӻ�������غ��� 
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


//���� Node ����жϰ׷��Ƿ������������Ӻ�������غ��� 
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
	
	//��ӡһЩ���� 
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


//��ʼ�� 
void init() {
	
	cout << ">>>> init begin" << endl; 
	
	//��ӡ���̴�С 
	cout << "board size : " << ROW << " " << COL << endl;
	
	//��ʼ��������������
	if (BOARD_TYPE == 1) {
		//�����Ҳ����� 
		for (int i = 0; i < ROW; ++i) {
			boardLimit[i][COL - 1] = 1;
		}
	} else if (BOARD_TYPE == 2) {
		//���������Ҳ�����
		for (int i = 0; i < ROW; ++i) {
			boardLimit[i][0] = 1;
			boardLimit[i][COL - 1] = 1;
		}
	} else if (BOARD_TYPE == 3) {
		//�����Ҳ���²�����
		for (int i = 0; i < ROW; ++i) {
			boardLimit[i][COL - 1] = 1;
		}
		for (int j = 0; j < COL; ++j) {
			boardLimit[ROW - 1][j] = 1;
		}
	}
	
	//��ӡ�������� 
	cout << "board limit :" << endl;
	for (int i = 0; i < ROW; ++i) {
		cout << "\t"; 
		for (int j = 0; j < COL; ++j) {
			cout << boardLimit[i][j] << " ";
		}
		cout << endl;
	}
	
	//��ӡһЩ���� 
	cout << "white capture limit    : " << CAPTURE_WHITE_LIMIT << endl;
	//cout << "black simple ko     : " << (BLACKSIMPLEKO == 1 ? "Yes" : "No") << endl;
	cout << "max play number        : " << MAX_PLAY_NUMBER << endl;
	cout << "capture play min       : " << CAPTURE_PLAY_MIN << endl;
	cout << "capture difference max : " << CAPTURE_DIFFERENCE_MAX << endl;
	
	//��ӡ��ʼ�ŷ��γɵľ��� 
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


//���ڲ��Ը�ģ�鹦�� 
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
//=============================================================================������ ���� 


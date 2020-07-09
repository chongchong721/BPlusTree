#include <iostream>
#include <string>
#include <cstring>
#include <queue>
#include <cmath>

using namespace std;
//实际上key作为索引（用int？） value存在叶子节点中

class Node
{
    public:
        int KeyNum;
        bool isLeaf;
        int *key;
        int *value;
        Node *parent;
        Node **child;

        Node(int , bool);
        ~Node();
};


class BPTree
{
    int n;//阶数
    int inNodeMinChild;
    int inNodeMinKey;
    int LeafNodeMinChild;
    int LeafNodeMinKey;

    Node *root;

    public:
        BPTree(int);
        BPTree();
        ~BPTree();

            void FreeNode(Node *p);
        //找到p为该节点并返回是第几个，找不到指向插入节点返回0
            int Search(Node *&, int );

            int FindPosition(Node *, int);
        //insert

            int add(int, int);

            void Insert_Leaf(Node *, int, int);
            void Insert_Leaf_NotFull(Node *, int, int);

            void Insert_Inner(Node *, Node *, int);
            void Insert_Inner_NotFull(Node *, Node *, int);

        //split
            Node *Split_Leaf(Node *, int, int);
            Node *Split_InnerNode(Node *, int, Node *);

        //delete
            bool del (int);

            bool del_leaf_byPosition(Node *, int);
            bool del_inner_byPosition(Node *, int position);
            bool del_inner_byKey(Node *, int key);

            int Position_inParent(Node *);
            void replace(Node *, int oldkey, int newkey); 

        //other
            Node *get_FirstNode();

        //print
            void PrintNode(Node *);
            void Print(); //层次遍历
            void Print_leaf();

            //serialize&deserialize
            void deserialize(string);
            void serialize(string);

            void preorder(Node *);
            void PrintNode_no_bracket(Node *);
            void transfer(int *arr, string buf);
            void deserialize_recursive(Node *my_child, Node *my_parent, bool root_mode,queue<Node *> *);
            void link_leaf(queue <Node *> *);
            void link_leaf_alt(queue<Node *> *);
};


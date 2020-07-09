#include "Bplus.h"
#include <random>
#include <vector>
#include <time.h>
#include <windows.h>

using namespace std;

Node::Node(int n,bool isLeaf)
{
    this->KeyNum = 0;
    this->isLeaf = isLeaf;
    this->parent = nullptr;
    this->key = new int[n + 1];

    this->child = new Node *[n + 1];
    for (int i = 0; i < n + 1;i++)
    {
        this->child[i] = nullptr;
    }

    if(isLeaf)
    {
        value = new int[n + 1];
    }

}

Node::~Node()
{
    this->KeyNum = 0;
    delete[] this->key;
    delete[] this->child;
    this->parent = nullptr;
    this->key = nullptr;
    this->child = nullptr;

    if(isLeaf)
    {
        delete[] value;
    }
}


BPTree::BPTree(int n)
{
    this->n = n;
    this->inNodeMinChild = ceil((n + 1.0) / 2.0);
    this->inNodeMinKey = this->inNodeMinChild - 1;
    this->LeafNodeMinChild = this->LeafNodeMinKey = floor((n + 1.0) / 2.0);
    this->root = new Node(n, true);
}

BPTree::BPTree()
{
    this->n = 0;
    this->inNodeMinChild = 0;
    this->inNodeMinKey = 0;
    this->LeafNodeMinChild = this->LeafNodeMinKey = 0;
    root = nullptr;
}

BPTree::~BPTree()
{
    this->n = 0;
    this->inNodeMinChild = 0;
    this->inNodeMinKey = 0;
    this->LeafNodeMinChild = this->LeafNodeMinKey = 0;
    //FreeNode(root);
    delete root;
}

void BPTree::FreeNode(Node *p)
{
    if(p==nullptr)
        return;
    if(p->isLeaf)
    {
        delete p;
        return;
    }
    for (int i = 0; i <= p->KeyNum;i++)
    {
        if(p->child[i]!=nullptr)
        FreeNode((Node *)(p->child[i]));
    }

    delete p;
    return;
}

//return the key's position in the p node
int BPTree::Search(Node *& p, int my_key)
{
    p = this->root;
    while(p!=nullptr)
    {
        int i = p->KeyNum;
        while(i>0 && my_key<p->key[i])
        {
            i--;
        }
        if(p->isLeaf)
        {
            if(i>0 && my_key==p->key[i])
            {
                return i;
            }
            return 0;//not found
        }
        else
        {
            p = (Node *)(p->child[i]);
        }
    }
    return -1;
}

int BPTree::FindPosition(Node *p,int my_key)
{
    int i = p->KeyNum;
    while(i>0 && my_key!=p->key[i])
    {
        i--;
    }
    if(i==0)
    {
        return -1;//not found
    }
    else
    {
        return i;
    }
}

int BPTree::add(int my_key,int my_value)
{
    Node *p = nullptr;
    int temp = this->Search(p, my_key);
    if(temp==-1||p==nullptr)
    {
        return -1;
    }
    if(temp!=0)
    {
        return 0; // already exist
    }
    Insert_Leaf(p, my_key, my_value);
    return 1;//ok
    
}
Node * BPTree::Split_Leaf(Node *p, int my_key, int my_value)
{
    Node *p_new = new Node(n, true);
    p_new->isLeaf = p->isLeaf;
    p_new->parent = p->parent;

    if(my_key<p->key[LeafNodeMinKey]) //insert my_key into left side node
    {
        int j = 1;
        for (int i = LeafNodeMinKey; i <= p->KeyNum;i++,j++)
        {
            p_new->key[j] = p->key[i];
            p_new->value[j] = p->value[i];
        }
        int i;
        for (i = LeafNodeMinKey-1; i > 0;i--)
        {
            if(my_key<p->key[i])
            {
                p->key[i + 1] = p->key[i];
                p->value[i + 1] = p->value[i];
            }
            else
            {
                break;
            }
        }
        p->key[i + 1] = my_key;
        p->value[i + 1] = my_value;
        p->KeyNum = LeafNodeMinKey;
        p_new->KeyNum = n + 1 - p->KeyNum;
    }
    else
    {
        int j = 1;
        bool flag = false;
        for (int i = LeafNodeMinKey + 1; i <= p->KeyNum;i++)
        {
            if(p->key[i]>my_key && !flag)
            {
                p_new->key[j] = my_key;
                p_new->value[j] = my_value;
                flag = true;
                j++;
            }
            p_new->key[j] = p->key[i];
            p_new->value[j] = p->value[i];
            j++;
        }
        if(!flag)
        {
            p_new->key[j] = my_key;
            p_new->value[j] = my_value;
        }

        p->KeyNum = LeafNodeMinKey;
        p_new->KeyNum = n + 1 - p->KeyNum;
    }

    if (p->isLeaf)
    {
        p_new->child[0] = p->child[0];
        p->child[0] = p_new;
    }

    return p_new;
}

Node * BPTree::Split_InnerNode(Node *p,int my_key, Node * my_ptr)
{
    Node *p_new = new Node(n, true);
    p_new->isLeaf = p->isLeaf;
    p_new->parent = p->parent;

    if(my_key<p->key[inNodeMinKey])
    {
        int j = 0;
        for (int i = inNodeMinKey; i <= p->KeyNum;i++,j++)
        {
            p_new->key[j] = p->key[i];
            p_new->child[j] = p->child[i];
            ((Node *)(p_new->child[j]))->parent = p_new;
            p->child[i] = nullptr;
        }
        int i;
        for (i = inNodeMinKey-1; i > 0;i--)
        {
            if(my_key<p->key[i])
            {
                p->key[i + 1] = p->key[i];
                p->child[i + 1] = p->child[i];
            }
            else
            {
                break;
            }
        }
        p->key[i + 1] = my_key;
        p->child[i + 1] = my_ptr;
        my_ptr->parent = p;
        p->KeyNum = inNodeMinKey;
        p_new->KeyNum = n + 1 - p->KeyNum - 1;
    }
    else
    {
        int j = 0;
        bool flag = false;
        for (int i = inNodeMinKey+1; i <= p->KeyNum;i++)
        {
            if(p->key[i]>my_key&&!flag)        //~!!!!!!!!!!!!!!!!!!!!!!!!
            {
                p_new->key[j] = my_key;
                p_new->child[j] = my_ptr;
                my_ptr->parent = p_new;
                j++;
                flag = true;
            }
            p_new->key[j] = p->key[i];
            p_new->child[j] = p->child[i];
            ((Node *)(p_new->child[j]))->parent = p_new;
            p->child[i] = nullptr;
            j++;
        }
        if(!flag)
        {
            p_new->key[j] = my_key;
            p_new->child[j] = my_ptr;
            my_ptr->parent = p_new;
        }
        p->KeyNum = inNodeMinKey;
        p_new->KeyNum = n + 1 - p->KeyNum - 1;
    }
    return p_new;
}


void BPTree::Insert_Leaf(Node *p,int my_key,int my_value)
{
    if(p->KeyNum<n)
    {
        Insert_Leaf_NotFull(p, my_key, my_value);
    }
    else
    {
        Node *p_new = Split_Leaf(p, my_key, my_value);
        if(p->parent!=nullptr)
        {
            Insert_Inner(p->parent, p_new, p_new->key[1]);
        }
        else
        {
            Node *root_new = new Node(n, false);
            root_new->KeyNum = 1;
            root_new->key[1] = p_new->key[1];
            root_new->child[0] = p;
            root_new->child[1] = p_new;
            p->parent = root_new;
            p_new->parent = root_new;
            this->root = root_new;
        }
    }
}


void BPTree::Insert_Leaf_NotFull(Node *p,int my_key,int my_value)
{
    int i;
    for (i = p->KeyNum; i > 0;i--)
    {
        if(my_key<p->key[i])
        {
            p->key[i + 1] = p->key[i];
            p->value[i + 1] = p->value[i];
        }
        else
        {
            break;
        }
    }
    p->key[i + 1] = my_key;
    p->value[i + 1] = my_value;
    p->KeyNum++;
    return;
}

void BPTree::Insert_Inner(Node *p,Node *my_ptr,int my_key)
{
    if(p->KeyNum<n)
    {
        Insert_Inner_NotFull(p, my_ptr, my_key);
    }
    else
    {
        Node *p_new = Split_InnerNode(p, my_key, my_ptr);
        if(p->parent!=nullptr)
        {
            Insert_Inner(p->parent, p_new, p_new->key[0]);
        }
        else
        {
            Node *root_new = new Node(n, false);
            root_new->KeyNum = 1;
            root_new->key[1] = p_new->key[0];
            root_new->child[0] = p;
            root_new->child[1] = p_new;
            p->parent = root_new;
            p_new->parent = root_new;
            this->root = root_new;
        }
    }
}

void BPTree::Insert_Inner_NotFull(Node *p,Node *my_ptr,int my_key)
{
    int i;
    for (i = p->KeyNum; i > 0;i--)
    {
        if(my_key<p->key[i])
        {
            p->key[i + 1] = p->key[i];
            p->child[i + 1] = p->child[i];
        }
        else
        {
            break;
        }
    }
    p->key[i + 1] = my_key;
    p->child[i + 1] = my_ptr;
    p->KeyNum++;
}


bool BPTree::del(int my_key)
{
    Node *p = nullptr;
    int temp = Search(p, my_key);
    if(temp==-1||p==nullptr)
    {
        return false;
    }
    else if(temp==0)
    {
        return false;
    }

    if(p->KeyNum>LeafNodeMinKey||p->parent==nullptr)
    {
        return del_leaf_byPosition(p, temp);
    }
    else if(p->KeyNum==LeafNodeMinKey)
    {
        Node *pp = p->parent;
        Node *left = nullptr;
        Node *right = nullptr;

        right = p->child[0];

        if(right!=nullptr && right->parent==pp)
        {
            if(right->KeyNum>LeafNodeMinKey)
            {
                int pos = FindPosition(pp, right->key[1]);
                del_leaf_byPosition(p, temp);
                p->KeyNum++;
                p->key[p->KeyNum] = right->key[1];
                p->value[p->KeyNum] = right->value[1];

                del_leaf_byPosition(right, 1); //该函数已经会替换key[1]了
                return true;
            }
        }
        else
        {
            right = nullptr;
        }

        int flag = FindPosition(pp, p->key[1]);
        if(flag!=-1) //有左边兄弟
        {
            left = pp->child[flag - 1];
            if(left->KeyNum>LeafNodeMinKey)
            {
                del_leaf_byPosition(p, temp);
                replace(pp, p->key[1], left->key[left->KeyNum]);
                Insert_Leaf_NotFull(p, left->key[left->KeyNum], left->value[left->KeyNum]);
                left->KeyNum--;
                return true;
            }
        }
        else
        {
            left = nullptr;
        }

        if(left==nullptr && right==nullptr)
        {
            cout << "sth wrong: no left or right sibling" << endl;
            return false;
        }

        if(left!=nullptr)
        {
            del_leaf_byPosition(p, temp);
            int j = left->KeyNum + 1;
            for (int i = 1; i <= p->KeyNum;i++,j++)
            {
                left->key[j] = p->key[i];
                left->value[j] = p->value[i];
            }
            left->KeyNum = left->KeyNum + p->KeyNum;
            left->child[0] = p->child[0];
            del_inner_byKey(pp, p->key[1]);
            delete p;
            return true;
        }
        else
        {
            del_leaf_byPosition(p, temp);
            int j = p->KeyNum + 1;
            for (int i = 1; i <= right->KeyNum;i++,j++)
            {
                p->key[j] = right->key[i];
                p->value[j] = right->value[i];
            }
            p->KeyNum = p->KeyNum + right->KeyNum;
            p->child[0] = right->child[0];
            del_inner_byKey(pp,right->key[1]);
            delete right;
            return true;
        }
    }
    else
    {
        cout << "sth wrong" << endl;
        return false;
    }

}

bool BPTree::del_inner_byKey(Node *p,int my_key) //对已经合并过后的进行操作，所以说child个数跟key个数是一样的
{
    int temp = FindPosition(p, my_key);
    if(temp==-1)
    {
        return false;
    }
    else
    {
        /************************************************/
        if(p->KeyNum>inNodeMinKey)
        {
            del_inner_byPosition(p,temp);
            return true;
        }
        /************************************************/


        /************************************************/
        if(p->parent==nullptr)
        {
            if(p->KeyNum>1)
            {
                del_inner_byPosition(p, temp);
                return true;
            }
            else
            {
                Node *root_new = (Node *)(p->child[0]);
                root_new->parent = nullptr;
                delete root;
                root = root_new;
                return true;
            }
        }
        /************************************************/

        /************************************************/
        Node *pp = p->parent;
        Node *left = nullptr;
        Node *right = nullptr;
        int pos = Position_inParent(p);

        /************************************************/

        if(pos>0)//不是第一个节点，可以与左边的兄弟操作
        {
            left = (Node *)(pp->child[pos - 1]);
            if(left->KeyNum>inNodeMinKey)
            {
                del_inner_byPosition(p, temp);

                int i = p->KeyNum;
                while(i>=0)
                {
                    p->key[i + 1] = p->key[i];
                    p->child[i+1]=p->child[i];
                    i--;
                }

                p->key[1] = pp->key[pos];
                p->KeyNum++;

                pp->key[pos] = left->key[left->KeyNum];

                p->child[0] = left->child[left->KeyNum];
                p->child[0]->parent = p; //!!!

                left->KeyNum--;

                return true;
            }
            
        }
        else
        {
            left = nullptr; //接下来 判断右边是否有兄弟
        }
        
        /************************************************/


        /************************************************/
        if(pos<pp->KeyNum)
        {
            right = (Node *)(pp->child[pos + 1]);
            if(right->KeyNum>inNodeMinKey)
            {
                del_inner_byPosition(p, temp);
                p->KeyNum++;
                p->key[p->KeyNum] = pp->key[pos + 1];
                pp->key[pos + 1] = right->key[1];
                p->child[p->KeyNum] = right->child[0];
                p->child[p->KeyNum]->parent = p;
                for (int i = 1; i <= right->KeyNum;i++)
                {
                    right->key[i - 1] = right->key[i];
                    right->child[i - 1] = right->child[i];
                }

                right->KeyNum--;

                return true;
            }
        }
        else
        {
            right = nullptr;
        }

            
        /************************************************/
        //剩下的情况需要合并


        /************************************************/
        if(left!=nullptr)
        {
            del_inner_byPosition(p, temp);

            p->key[0] = pp->key[pos];
            int j = left->KeyNum + 1;
            for (int i = 0; i <= p->KeyNum;i++,j++)
            {
                left->key[j] = p->key[i];
                left->child[j] = p->child[i];
                p->child[i]->parent = left;
            }
            left->KeyNum = left->KeyNum + p->KeyNum + 1;
            del_inner_byKey(pp, pp->key[pos]); //用position行吗？

            delete p;
            return true;
        }
        else
        {
            del_inner_byPosition(p, temp);

            right->key[0] = pp->key[pos + 1];
            int j = p->KeyNum + 1;
            for (int i = 0; i <= right->KeyNum;i++,j++)
            {
                p->key[j] = right->key[i];
                p->child[j] = right->child[i];
                p->child[j]->parent = p;
            }

            p->KeyNum = p->KeyNum + right->KeyNum + 1;
            del_inner_byKey(pp, pp->key[pos + 1]);
            delete right;
            return true;
        }
        
        /************************************************/
    }
}

int BPTree::Position_inParent(Node *p)
{
    if(p->parent==nullptr)
    {
        return -1;
    }

    Node *pp = p->parent;

    int i = pp->KeyNum;
    while(i>0 &&p->key[1]<pp->key[i])
    {
        i--;
    }
    return i;
}

bool BPTree::del_leaf_byPosition(Node * p,int position)
{
    int Key2del = p->key[position];

    for (int i = position + 1; i <= p->KeyNum;i++)
    {
        p->key[i - 1] = p->key[i];
        p->value[i - 1] = p->value[i];
    }
    p->KeyNum--;

    if(p->isLeaf && position==1)
    {
        replace(p->parent, Key2del, p->key[1]);
    }
    return true;
}

bool BPTree::del_inner_byPosition(Node *p,int position)
{
    int key2del = p->key[position];

    for (int i = position + 1; i <= p->KeyNum;i++)
    {
        p->key[i - 1] = p->key[i];
        p->child[i - 1] = p->child[i];
    }
    p->KeyNum--;
    return true;
}

void BPTree::replace(Node *p,int oldkey,int newkey)
{
    bool if_find = false;
    while(!if_find&&p!=nullptr)
    {
        for (int i = 1; i <= p->KeyNum;i++)
        {
            if(p->key[i]==oldkey)
            {
                p->key[i] = newkey;
                if_find = true;
                break;
            }
        }
        p = p->parent;
    }
    return;
}


void BPTree::PrintNode(Node *p)
{
    cout << "(";
    bool if_first = true;
    for (int i = 1; i <= p->KeyNum;i++)
    {
        if(if_first)
        {
            cout << p->key[i];
            if_first = false;
            continue;
        }
        cout << "," << p->key[i];
    }
    cout << ")";
    return;
}

void BPTree::Print()
{
    if(root==nullptr)
    {
        cout << "empty BPTree" << endl;
        return;
    }

    queue<Node *> Node_queue;
    queue<int> KeyNum_queue;
    Node_queue.push(root);

    int NodeNum = 0;
    int NodeNum_sum = 1;
    int nextNodeNum = 0;

    while(!Node_queue.empty())
    {
        Node *tempNode = Node_queue.front();
        Node_queue.pop();
        if(tempNode)
        {
            PrintNode(tempNode);
            if(!tempNode->isLeaf)
            {
                for (int i = 0; i <= tempNode->KeyNum;i++)
                {
                    if(tempNode->child[i]!=nullptr)
                    {
                        Node_queue.push(tempNode->child[i]);
                    }
                    nextNodeNum++;
                }
            }
            NodeNum++;
            if(NodeNum==NodeNum_sum)
            {
                cout << endl;
                KeyNum_queue.push(nextNodeNum);
                nextNodeNum = 0;
                if(!KeyNum_queue.empty())
                {
                    NodeNum = 0;
                    NodeNum_sum = KeyNum_queue.front();
                    KeyNum_queue.pop();
                }
            }
        }
    }
    cout << "BPTree output over" << endl;
}

void BPTree::PrintNode_no_bracket(Node *p)
{
    for (int i = 1; i <= p->KeyNum;i++)
    {
        cout << p->key[i] << " ";
    }
    return;
}

void BPTree::preorder(Node *p)
{
    if(p==nullptr)
    {
        return;
    }
    if(!p->isLeaf)
    {
        cout << "1 " << p->KeyNum << " ";
        PrintNode_no_bracket(p);
        cout << endl;
        for (int i = 0; i <= p->KeyNum;i++)
        {
            preorder(p->child[i]);
        }
    }
    else
    {
        cout << "0 " << p->KeyNum << " ";
        PrintNode_no_bracket(p);
        for (int i = 1; i <= p->KeyNum;i++)
        {
            cout << p->value[i] << " ";
        }
        cout << endl;
        return;
    }
}

void BPTree::serialize(string str)
{
    char *str1 = new char[1000];
    strcpy(str1, str.c_str());
    freopen(str1, "w", stdout);
    cout << n << " " << inNodeMinKey << " " << inNodeMinChild << " " << LeafNodeMinKey << " " << LeafNodeMinChild << endl;
    preorder(root);
    freopen("CON", "w", stdout);
}

void BPTree::deserialize_recursive(Node * my_child,Node *my_parent,bool root_mode,queue<Node *> *leaf_queue)
{
    int line[100];
	string buf;

	if(!getline(cin, buf))
		return;

	transfer(line, buf);
	if(root_mode)
	{
		if(line[0]==0)
		{
			root = new Node(this->n,true);
            root->isLeaf = true;
			root->parent = my_parent;
			root->KeyNum = line[1];
			int i = 2;
			for (int j = 1; j <= root->KeyNum; j++, i++)
			{
				root->key[j] = line[i];
				root->value[j] = line[i + line[1]];
			}
            leaf_queue->push(root);
            return;
		}
		else
		{
			this->root = new Node(this->n, false);
            root->isLeaf = false;
            this->root->parent = my_parent;
			this->root->KeyNum = line[1];
			int i = 2;
			for (int j = 1; j <= root->KeyNum; j++, i++)
			{
				root->key[j] = line[i];
			}
			for (int i = 0; i <= root->KeyNum; i++)
			{
				Node *temp = new Node(this->n, true);
				root->child[i] = temp;
				deserialize_recursive(temp, this->root, false,leaf_queue);
			}
				

		}
	
	}
	else
	{
		if(line[0]==0)
		{
			Node *this_node = my_child;
            this_node->isLeaf = true;
            this_node->parent = my_parent;
			this_node->KeyNum = line[1];
			int i = 2;
			for (int j = 1; j <= this_node->KeyNum; j++, i++)
			{
				this_node->key[j] = line[i];
				this_node->value[j] = line[i + line[1]];
			}
            leaf_queue->push(this_node);
            link_leaf_alt(leaf_queue);
            return;
		}
		else
		{
			Node *this_node = my_child;
            this_node->isLeaf = false;
            this_node->parent = my_parent;
			this_node->KeyNum = line[1];
			int i = 2;
			for (int j = 1; j <= this_node->KeyNum;j++,i++)
			{
				this_node->key[j] = line[i];
			}
			for (int i = 0; i <= this_node->KeyNum;i++)
			{
				Node *temp = new Node(this->n, true);
				this_node->child[i] = temp;
				deserialize_recursive(temp, this_node, false,leaf_queue);
			}
		}
		return;
	}
}

void BPTree::transfer(int *arr,string buf)
{
    int length = buf.size();
    buf.insert(length, " ");
    length++;
    int temp = 0;
	int tmplen;
	string tmpstr;
	int j = 0;
	for (int i = 0;i<length;i++)
	{
		if(buf[i]==' ')
		{
			tmplen = i - temp;
			tmpstr = buf.substr(temp, tmplen);
            temp = i + 1;
			char *c=new char[100];
			strcpy(c, tmpstr.c_str());
			arr[j] = atoi(c);
			j++;
		}
		
	}
}

void BPTree::deserialize(string str)
{
    char *str1 = new char[1000];
	strcpy(str1, str.c_str());
    cin.clear();
    freopen(str1, "r", stdin);
    int line[100];
	string buf;
	getline(cin, buf);
	transfer(line, buf);

	this->n = line[0];
	this->inNodeMinKey = line[1];
	this->inNodeMinChild = line[2];
	this->LeafNodeMinKey = line[3];
	this->LeafNodeMinChild = line[4];

    queue<Node *> *leaf_queue=new queue<Node *>;

    deserialize_recursive(nullptr,nullptr, true,leaf_queue);

  //  link_leaf(leaf_queue);

    freopen("CON", "r", stdin);
}

void BPTree::Print_leaf()
{
    Node *p = root;
    while(!p->isLeaf && p!=nullptr)
    {
        p = p->child[0];
    }

    while(p!=nullptr)
    {
        bool if_first = true;
        cout << "(";
        for (int i = 1; i <= p->KeyNum;i++)
        {
            if(if_first)
            {
                cout << p->key[i];
                if_first = false;
                continue;
            }
            cout << "," << p->key[i];
        }
        cout << ") ";
        p = p->child[0];
    }
    cout << endl;
    return;
}


void BPTree::link_leaf_alt(queue<Node *> * leaf_queue)
{
    if(leaf_queue->size()==2)
    {
        Node *former = leaf_queue->front();
        leaf_queue->pop();
        Node *latter = leaf_queue->front();
        former->child[0] = latter;
    }
    return;
}

void BPTree::link_leaf(queue <Node *> * leaf_queue)
{
    while(true)
    {
        Node *latter;
        Node *former;
        former = leaf_queue->front();
        leaf_queue->pop();
        if(leaf_queue->empty())
        {
            break;
        }
        latter = leaf_queue->front();
        former->child[0] = latter;
    }
}


void t_btree4() {
	BPTree btree(4);
	for (int i = 1; i <= 13; i++) {
		btree.add(i, i);
	}
	btree.Print();
}

void t_1_to_10() {
	BPTree btree(3);
	for (int i = 1; i <= 10; i++) {
		btree.add(i, i);
	}
	btree.Print();
}

void t_add() {
	BPTree btree(3);
	btree.Print();

	btree.add(4, 4);
	btree.Print();

	btree.add(5, 5);
	btree.Print();

	btree.add(8, 8);
	btree.Print();

	btree.add(7, 7);
	btree.Print();

	btree.add(2, 2);
	btree.Print();

	btree.add(12, 12);
	btree.Print();

	btree.add(10, 10);
	btree.Print();

	btree.add(6, 6);
	btree.Print();

	btree.add(3, 3);
	btree.Print();

	btree.add(9, 9);
	btree.Print();

	btree.add(20, 20);
	btree.Print();

	btree.add(30, 30);
	btree.Print();

	btree.add(1, 1);
	btree.Print();

	btree.add(0, 0);
	btree.Print();

	btree.add(-1, -1);
	btree.Print();

}

BPTree *my_tree = new BPTree(3);
default_random_engine e;
vector<int> delete_vec;

void TimeProc(int nTimerid)
{
    int tmp = e();
    int length = delete_vec.size();
    int tmpLength;
    switch(nTimerid)
    {
        
        case 1:
            
            if(my_tree->add(tmp, tmp)==1)
            {
                cout << "insert " << tmp  << endl;
            }
            else
            {
                cout << "insert failed" << endl;
            }
            delete_vec.push_back(tmp);
            Sleep(1000);
            break;
        case 2:
            
            if(length!=0)
            {
                tmpLength = length / 2;
                vector<int>::iterator it = delete_vec.begin() + tmpLength;
                if(my_tree->del(*it))
                {
                    cout << "delete " << *it << endl;
                }
                else
                {
                    cout << "delete failed" << endl;
                }
                delete_vec.erase(it);
            }
            Sleep(3000);
            break;
    }
}

 int main()
 {
     time_t start_time = GetTickCount();
     time_t now_time = start_time;
     while((now_time-start_time)<18000000)
     {
         int ran = rand() % 100 + 1;
         if(ran<=75)
         {
             TimeProc(1);
         }
         else
         {
             TimeProc(2);
         }
         now_time = GetTickCount();
     }
     return 0;
}


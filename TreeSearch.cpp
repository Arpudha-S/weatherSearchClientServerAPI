#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <iomanip>

using namespace std;

struct Node
{
    string location;
    int temperature;
    Node* left;
    Node* right;
};

Node* newNode(const string& location,int temperature)
{
    Node* newNode=new Node();
    newNode->location=location;
    newNode->temperature=temperature;
    newNode->left=newNode->right=NULL;
    return newNode;
}

Node* rightRotate(Node* x)
{
    Node* y=x->left;
    x->left=y->right;
    y->right=x;
    return y;
}

Node* leftRotate(Node* x)
{
    Node* y=x->right;
    x->right=y->left;
    y->left=x;
    return y;
}

Node* splay(Node* root,const string& location)
{
    if (root==NULL||root->location==location)
        return root;

    if (root->location>location)
    {
        if (root->left==NULL)
            return root;

        if (root->left->location>location)
        {
            root->left->left=splay(root->left->left,location);
            root=rightRotate(root);
        }

        else if (root->left->location<location)
        {
            root->left->right=splay(root->left->right,location);
            if (root->left->right!=NULL)
                root->left=leftRotate(root->left);
        }
        return (root->left==NULL)?root:rightRotate(root);
    }
    else
    {
        if (root->right==NULL)
            return root;

        if (root->right->location>location)
        {
            root->right->left=splay(root->right->left,location);
            if (root->right->left!=NULL)
                root->right=rightRotate(root);
        }

        else if (root->right->location<location)
        {
            root->right->right=splay(root->right->right,location);
            root=leftRotate(root);
        }
        return (root->right==NULL)?root:leftRotate(root);
    }
}

Node* insert(Node* root,const string& location,int temperature)
{
    if (root==NULL)
        return newNode(location,temperature);

    root=splay(root,location);

    if (root->location==location)
        return root;

    Node* newNode=new Node();
    newNode->location=location;
    newNode->temperature=temperature;
    newNode->left=newNode->right=NULL;

    if (root->location >location)
    {
        newNode->right=root;
        newNode->left=root->left;
        root->left=NULL;
    } 
    else
    {
        newNode->left=root;
        newNode->right=root->right;
        root->right=NULL;
    } 
    return newNode;
}

void printTree(Node* root,int space=0,int height=10)
{
    if (root==NULL)
        return;

    space+=height;

    printTree(root->right,space);
    cout<<endl;
    for (int i=height;i<space;i++)
        cout<<" ";
    cout<<root->location<<" ("<<root->temperature<< "\u00B0C)"<<endl;
    printTree(root->left,space);
}

void search(Node* &root,const string& location)
{
    root=splay(root,location);
    if (root && root->location==location)
    {
        cout<<"City: "<<root->location<<", Temperature: "<<root->temperature<<"\u00B0C"<<endl;

        if (root->temperature >= 35)
        {
            cout<<"Warning: Heat wave conditions."<<endl;
        }
        else if (root->temperature >= 30)
        {
            cout<<"Possibility of high heat wave conditions."<<endl;
        }
        else if (root->temperature >= 20)
        {
            cout<<"Possibility of rain."<<endl;
        }
        else
        {
            cout<<"Conditions of heavy rain likely."<<endl;
        }
    }
    else
    {
        cout<<"City not found!"<<endl;
    }
}

int main()
{
    Node* root=NULL;

    ifstream file("city.txt");
    if (!file)
    {
        cerr<<"Error in opening file."<<endl;
        return 1;
    }

    string line;
    while(getline(file,line))
    {
        istringstream iss(line);
        string location;
        int temperature;

        if (!(iss>>location>>temperature))
        {
            cerr<<"Error in reading line:"<<line<<endl;
            continue;
        }
        root=insert(root,location,temperature);
    }
    file.close();
    cout<<"Tree structure:"<<endl;
    printTree(root);

    string searchLoc;
    cout<<"Enter a city to search: "<<endl;
    cin>>searchLoc;
    search(root, searchLoc);
    
    return 0;
}
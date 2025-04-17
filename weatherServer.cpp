#include <iostream>
#include <winsock2.h>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <iomanip>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

#define BUFFER_SIZE 1024
const int PORT = 8081;

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


void returnAllData(string &str,Node* root,int space=0,int height=10)
{
    if (root==NULL)
        return;

    space+=height;

    returnAllData(str,root->right,space);
    str += (root->location+":"+to_string(root->temperature)+ "  ");
    returnAllData(str,root->left,space);
}

void searchTemperature(Node* &root,int temperature,string &strOut,char oper)
{
    if (root==NULL)
        return;
    searchTemperature(root->right,temperature,strOut,oper);
   if (oper == '=' && temperature == root->temperature)
   {
    strOut += (root->location+" ");
   }
   else if (oper == '>' && temperature < root->temperature)
   {
    strOut += (root->location+" ");
   }
   else if (oper == '<' && temperature > root->temperature)
   {
    strOut += (root->location+" ");
   }
    searchTemperature(root->left,temperature,strOut,oper);
}

string searchCity(Node* &root,const string& location)
{
    root=splay(root,location);
    if (root && root->location==location)
    {
        return to_string(root->temperature);        
    }
    else
    {
        return "City temperature data not found!";
    }
}

int main()
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE] = { 0 };
    int addrLen = sizeof(struct sockaddr_in);
    cout << "Application started\n";
// Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" <<endl;
        return 1;
    }
    cout << "WinSock start up completed.\n";
    // Create socket,  IPv4 family
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        cerr << "Socket creation failed" <<endl;
        WSACleanup();
        return 1;
    }

    // Prepare the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; //inet_addr("227.157.1.100");
    serverAddr.sin_port = htons(PORT);//network byte order (port numbers)

// Bind
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed" <<endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    cout << "WinSock bind completed.\n";
    // Listen
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed" <<endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    Node* root = NULL;
    cout<<"Server ready...\n";
    while (true) {
        // Accept incoming connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen)) == INVALID_SOCKET) {
            cerr << "Accept failed" <<endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }
        cout << "WinSock accept completed.\n";
        cout << "Connection accepted" <<endl;

        // Receive message from client
        int valread = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        cout << "Received message from client: " << buffer <<endl;

        string strClientResponse="Unknown Format";
        
        string str = buffer;
        memset(buffer,'\0',100);
        string strArr[100];
        int index = 0;
        stringstream  data(str);
        string token;
        while (getline(data, token, '_')) {
            strArr[index++] = token;
        }
        if (index > 1)
        {
            if (strArr[0]=="set" && index > 2)
            {
                string location = strArr[1];
                int temperature= atoi(strArr[2].c_str());
                
                root=insert(root,location,temperature);  
                strClientResponse="Temperature update, Thanks for your input.";   
            }
            else if (strArr[0]=="get" && index > 1)
            {
                string type = strArr[1];
                if (type == "all")
                {
                    strClientResponse="";
                    returnAllData(strClientResponse,root);
                    cout<<strClientResponse<<endl;
                }
                else if (type == "city" && index > 2)
                {
                    string searchLoc = strArr[2];
                    strClientResponse= searchCity(root, searchLoc);
                }
                else if (type == "equal" && index > 2)
                {
                    strClientResponse="";
                    int temperature = atoi(strArr[2].c_str());
                    searchTemperature(root, temperature,strClientResponse,'=');
                }
                else if (type == "greater" && index > 2)
                {
                    strClientResponse="";
                    int temperature = atoi(strArr[2].c_str());
                    searchTemperature(root, temperature,strClientResponse,'>');
                }
                else if (type == "less" && index > 2)
                {
                    strClientResponse="";
                    int temperature = atoi(strArr[2].c_str());
                    searchTemperature(root, temperature,strClientResponse,'<');
                }
            }
            else if (strArr[0]=="delete")
            {
                string location = strArr[1];
                strClientResponse="under construction";
                //delete data
            }            
        }
        send(clientSocket, strClientResponse.c_str(), strClientResponse.length(), 0);
        cout << "Message sent to client" <<endl;
    }
    closesocket(serverSocket);
    WSACleanup();
}
 
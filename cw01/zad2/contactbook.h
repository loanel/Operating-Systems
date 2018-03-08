#ifndef contactbook_h
#define contactbook_h

// thanks to typedef we can just write for example TreeNode *head instead of struct TreeNode *head;
typedef struct TreeNode TreeNode;
typedef struct Tree Tree;
typedef struct Contact Contact;
typedef struct ListNode ListNode;
typedef struct List List;

struct Contact{
	char *name;
	char *surname;
	char *adress;
	char *birthDate;
	char *email;
	char *phoneNumber;
};

struct TreeNode{
	Contact *data;
	TreeNode *left;
	TreeNode *right;
	TreeNode *parent;
};

struct Tree{
	TreeNode *head;
	int currentOrder;
};

struct ListNode{
	Contact *data;
	ListNode *next;
	ListNode *previous;
};

struct List{
	ListNode *first;
	ListNode *last;
	int length;
};

/*
List associated functions
*/
List *makeList();
void deleteList(List *list);
void addToList(List *list, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber);
void removeFromList(List *list, char *name, char *surname, char *email);
ListNode *findContact(List *list, char *name, char *surname, char *email); // each contact diffrentiates with a different name, surname and email
void sortList(List *list, int option);
ListNode *mergeSort(ListNode *head, int option);
ListNode *merge(ListNode *list1, ListNode *list2, int option);// PO JAKIM ARGUMENCIE SORTUJE?
void printlist(List *list);
/*
Tree associated functions
*/
Tree *makeTree();
void deleteTree(Tree *tree);
void deleteTreeNode(TreeNode *treeNode);/// USED FOR deleteTree, not removing one contact from the tree
void addToTree(Tree *tree, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber);
void addToSubtree(TreeNode *newNode, TreeNode *currentNode, int order);
void removeFromTree(Tree *tree, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber);
TreeNode *findTreeContact(Tree *tree, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber);
Tree *sortTree(Tree *tree, int argNumber);
void printTree(Tree *tree);
void printNode(TreeNode *treeNode, char *direction);
/// used in both

int compareContacts(Contact *contact1, Contact *contact2, int option);

#endif

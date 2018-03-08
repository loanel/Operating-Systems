#include "contactbook.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/// list functions
List *makeList(){
    // two directional list with guardians
	List *newList = malloc(sizeof(List));
	ListNode *head = malloc(sizeof(ListNode));
	ListNode *end = malloc(sizeof(ListNode));
	// setting guardian pointers
	head->next = end;
	head->previous = NULL;
	end->previous = head;
	end->next = NULL;
	newList->first = head;
	newList->last = end;
	newList->length = 0;
	return newList;
}

void deleteList(List *list){
	ListNode *element1 = list->first->next;
	ListNode *element2;
	while(element1 != NULL && element1 != list->last){
		element2=element1;
		element1=element1->next;
		free(element2->data);
		free(element2);
	}
	free(list->first);
	free(list->last);
	free(list);
}

void printList(List *list){
    printf("%d",list->length);
    ListNode *a = list->first->next;
    while(a->next!=NULL){
        printf(a->data->name);
				printf("\n");
        a = a->next;
        }
    printf("END\n");
}

void addToList(List *list, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber){
	/// allocating new contact
	Contact *newContact = malloc(sizeof(Contact));
	newContact->name = name;
	newContact->surname = surname;
	newContact->adress = adress;
	newContact->birthDate = birthDate;
	newContact->email = email;
	newContact->phoneNumber = phoneNumber;

	ListNode *newNode = malloc(sizeof(ListNode));
	newNode->data = newContact;

	newNode->next = list->last; //newNode to last
	newNode->previous = list->last->previous; // newNode to second do last
	list->last->previous->next = newNode; // second to last to newNode
	list->last->previous = newNode;	// last to newNode
	list->length+=1;// new element in list, incrementing
}

void removeFromList(List *list,  char *name, char *surname, char *email){
	ListNode *toRemove = findContact(list, name, surname, email);
	toRemove->previous->next = toRemove->next;
	free(toRemove->data);
	free(toRemove);
	list->length-=1;
}

ListNode *findContact(List *list, char *name, char *surname, char *email){
	ListNode *current = list->first->next;/// finding contact based on name, surname and email
	while(name!=current->data->name && surname!=current->data->surname && email!=current->data->email && current->next!=list->last && current!=NULL){
		current = current->next;
	}
	return current;
}


void sortList(List *list, int option){
    /// unpin guards, sort the list without them

	ListNode * newHead = list->first->next;
	list->last->previous->next = NULL;
	newHead->previous = NULL;
	list->first->next = NULL;
	list->last->previous = NULL;

	newHead = mergeSort(newHead, option);
	/// pin the guards to the newly sorted list
	newHead->previous = list->first;
	list->first->next = newHead;
	while(newHead->next!=NULL){
		newHead = newHead->next;
	}
	newHead->next = list->last;
	list->last->previous = newHead;
}

ListNode *mergeSort(ListNode *head, int option){ // PO JAKIM ARGUMENCIE SORTUJE?
	if(head->next == NULL) {
            return head;
	}
	ListNode *frontReference = head;
	ListNode *middleReference;
	ListNode *slow = head;
	ListNode *fast = head;


	/// traversing list with 2 pointers, one twice as fast as the other, when the faster one reaches NULL the slower one is in the middle
	while(fast->next != NULL){
		fast = fast->next;
		if(fast->next != NULL) {
                slow = slow->next;
                fast = fast->next;
		}
	}
	/// fast is at the end, slow is at the middle
	middleReference = slow -> next;
	slow->next = NULL;
	ListNode *n1 = mergeSort(frontReference, option);
	ListNode *n2 = mergeSort(middleReference, option);
	return merge(n1, n2, option);
}

ListNode *merge(ListNode *list1, ListNode *list2, int option){
	ListNode *head;
	ListNode *pom;
	if(compareContacts(list1->data, list2->data, option)<=0){
		head = list1;
		list1 = list1->next;
	}
	else{
		head = list2;
		list2 = list2->next;
	}
	pom = head;///pointer
	while(list1 != NULL && list2 != NULL){
        if(compareContacts(list1->data, list2->data, option)<=0){
            pom->next = list1;
            list1->previous = pom;
            list1 = list1->next;
        }
        else{
            pom->next = list2;
            list2->previous = pom;
            list2 = list2->next;
        }
        pom = pom->next;
	}
	if(list1!=NULL){
		pom->next = list1;
		list1->previous = pom;
	}
	if(list2!=NULL){
		pom->next = list2;
		list2->previous = pom;
	}
	return head;
}

/// tree functions

Tree *makeTree(){
    Tree *newTree = malloc(sizeof(Tree));
    newTree->currentOrder = 5;// default tree order = phonenubmers
    newTree->head = NULL;
    return newTree;
}

void deleteTree(Tree *tree){
    if(tree->head != NULL) deleteTreeNode(tree->head);
    free(tree);
}

void deleteTreeNode(TreeNode *treeNode){
    TreeNode *leftLeaf = treeNode->left;
    TreeNode *rightLeaf = treeNode->right;
    free(treeNode->data);
    free(treeNode);
    if(leftLeaf != NULL)deleteTreeNode(leftLeaf);
    if(rightLeaf != NULL)deleteTreeNode(rightLeaf);
}

void printTree(Tree *tree){
    printNode(tree->head, "DIR");
}
void printNode(TreeNode *treeNode, char *direction){
    printf(direction);
    if(treeNode != NULL){
        printf(treeNode->data->name);
        if(treeNode->left!=NULL) printNode(treeNode->left, "LEFT");
        if(treeNode->right!=NULL) printNode(treeNode->right, "RIGHT");
    }
    else{
        printf("Tree is empty !");
    }
}

void addToTree(Tree *tree, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber){
    Contact *newContact = malloc(sizeof(Contact));
	newContact->name = name;
	newContact->surname = surname;
	newContact->adress = adress;
	newContact->birthDate = birthDate;
	newContact->email = email;
	newContact->phoneNumber = phoneNumber;

	TreeNode *newNode = malloc(sizeof(TreeNode));
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->parent = NULL;
	newNode->data = newContact;

    if(tree->head==NULL) tree->head=newNode;
			else addToSubtree(newNode, tree->head, tree->currentOrder);
}

void addToSubtree(TreeNode *newNode, TreeNode *currentNode, int order){
    if(compareContacts(newNode->data, currentNode->data, order)>=0){
        if(currentNode->right==NULL){
            newNode->parent = currentNode;
            currentNode->right = newNode;
        }
        else addToSubtree(newNode, currentNode->right, order);
    }
    else{
        if(currentNode->left==NULL) {
            newNode->parent = currentNode;
            currentNode->left = newNode;
        }
        else addToSubtree(newNode, currentNode->left, order);
    }
}

void removeFromTree(Tree *tree, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber){
    Contact *newContact = malloc(sizeof(Contact));
	newContact->name = name;
	newContact->surname = surname;
	newContact->adress = adress;
	newContact->birthDate = birthDate;
	newContact->email = email;
	newContact->phoneNumber = phoneNumber;

	/// find the contact
	/// tree->head special cases:
	TreeNode *current = tree->head;
	if(compareContacts(newContact, current->data, tree->currentOrder)==0){
        if(current->left==NULL || current->right==NULL){
            if(current->left==NULL && current->right==NULL){/// head is alone
                tree->head = NULL;
                free(current->data);
                free(current);
                return;
            }
            else{
                if(current->left!=NULL){/// head has left child
                current->left->parent = NULL;
                tree->head = current->left;
                free(current->data);
                free(current);
                return;
                }
                else{///head has right child
                current->right->parent = NULL;
                tree->head = current->right;
                free(current->data);
                free(current);
                return;
                }
            }
        }
	}

    /// traverse the tree
    while(compareContacts(newContact, current->data, tree->currentOrder)!=0){
        if(compareContacts(newContact, current->data, tree->currentOrder)>0){
            current = current->right;
        }
        else {
            current = current->left;
        }
    }
    /// 3 options
    if(current->left==NULL && current->right==NULL){
        if(current->parent->left==current){
            current->parent->left = NULL;
        }
        else{
            current->parent->right = NULL;
        }
        free(current->data);
        free(current);
    }
    else{
        if(current->left!=NULL && current->right!=NULL){
            TreeNode *toRemove = current;
            current=current->right;
            while(1){
                while(current->left!=NULL)current = current->left;
                if(current->right!=NULL)current = current->right;
                else break;
            }
            /// we have the node that will be replacing the deleted one
            /// unpin the replacement
            if(current->parent->left==current){
                current->parent->left = NULL;
            }
            else{
                current->parent->right = NULL;
            }
            /// change replacement parent, check for head
            if(toRemove==tree->head){
                current->parent = NULL;
                tree->head = current;
            }
            else{
                current->parent = toRemove->parent;
                /// change toRemove parent to have current as child on the right pin
                if(toRemove->parent->left==toRemove){
                    toRemove->parent->left = current;
                }
                else{
                    toRemove->parent->right = current;
                }
            }
            if(toRemove->left != current) current->left = toRemove->left;
            else current->left = NULL;
            if(toRemove->right != current) current->right = toRemove->right;
            else current->right = NULL;

            free(toRemove->data);
            free(toRemove);
        }
        else{
            if(current->left!=NULL){
                if(current->parent->left==current){
                    current->parent->left=current->left;
                }
                else{
                    current->parent->right = current->left;
                }
            }
            else{
                if(current->parent->left==current){
                    current->parent->left = current->right;
                }
                else{
                    current->parent->right = current->right;
                }
            }
            free(current->data);
            free(current);
        }
    }
}

TreeNode *findTreeContact(Tree *tree, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber){

    TreeNode *current = tree->head;

    Contact *newContact = malloc(sizeof(Contact));
	newContact->name = name;
	newContact->surname = surname;
	newContact->adress = adress;
	newContact->birthDate = birthDate;
	newContact->email = email;
	newContact->phoneNumber = phoneNumber;

    while(compareContacts(newContact, current->data, tree->currentOrder)!=0){
        if(compareContacts(newContact, current->data, tree->currentOrder)>0){
            if(current->right==NULL) printf("node doesn't exist in the tree");
            else{
                current=current->right;
            }
        }
        else{
            if(current->left==NULL) printf("node doesn't exist in the tree");
            else{
                current=current->left;
            }
        }
    }
    /// we found the contact
    free(newContact);
    return current;
}

Tree *sortTree(Tree *tree, int option){
    Tree *sortedTree = malloc(sizeof(Tree));
    sortedTree->currentOrder = option;
    sortedTree->head = NULL;
    while(tree->head!=NULL){
        char *name = tree->head->data->name;
        char *surname = tree->head->data->surname;
        char *adress = tree->head->data->adress;
        char *birthDate = tree->head->data->birthDate;
        char *email = tree->head->data->email;
        char *phoneNumber = tree->head->data->phoneNumber;
     /*   Contact *newContact = malloc(sizeof(Contact));
        newContact->name = tree->head->data->name;
        newContact->surname = tree->head->data->surname;
        newContact->adress = tree->head->data->adress;
        newContact->birthDate = tree->head->data->birthDate;
        newContact->email = tree->head->data->email;
        newContact->phoneNumber = tree->head->data->phoneNumber; */
        addToTree(sortedTree, name, surname, adress, birthDate, email, phoneNumber);
        removeFromTree(tree, name, surname, adress, birthDate, email, phoneNumber);
    }
    deleteTree(tree);
    return sortedTree;
}


/// comparator functions
int compareContacts(Contact *contact1, Contact *contact2, int option){
    switch(option){
    case 0:
        return strcmp(contact1->name, contact2->name);
        break;
    case 1:
        return strcmp(contact1->surname, contact2->surname);
        break;
    case 2:
        return strcmp(contact1->adress, contact2->adress);
        break;
    case 3:
        return strcmp(contact1->birthDate, contact2->birthDate);
        break;
    case 4:
        return strcmp(contact1->email, contact2->email);
        break;
    case 5:
        return strcmp(contact1->phoneNumber, contact2->phoneNumber);
        break;
    default : return 2;
    }
}

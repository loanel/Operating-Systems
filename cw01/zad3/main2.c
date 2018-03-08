#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <dlfcn.h>
#include "contactbook.h"

/// time functions
/// global variables required to efficiently measure time
/// based on https://www.gnu.org/software/libc/manual/html_node/Processor-Time.html#Processor-Time and associated articles
clock_t time_of_start;
clock_t time_since_last;
clock_t current; // required for operations later on

static struct tms tos_tms;
static struct tms tsl_tms;
static struct tms current_tms; //up

void generateTimeData(char *type){
  printf(type);
  printf("\n");
  times(&current_tms); /// updates helper with current tick dataczas
  current = clock();
  /// real time
  printf("'Real' measurement : difference since last = %.12lf, time elapsed since start = %.12lf\n",
          (double)(current - time_since_last)/ CLOCKS_PER_SEC,
          (double)(current - time_of_start) / CLOCKS_PER_SEC); // .[numbers] - precision, apparently you should do lf for doubles (SO)
  //  user time, tms_utime
  printf("'User' measurement : difference since last = %.12f, time elapsed since start = %.12f\n",
          (double)(current_tms.tms_utime - tsl_tms.tms_utime) / CLOCKS_PER_SEC,
          (double)(current_tms.tms_utime - tos_tms.tms_utime) / CLOCKS_PER_SEC);
  // system time, tms_stime
  printf("'System' measurement : difference since last = %.12f, time elapsed since start = %.12f\n",
          (double)(current_tms.tms_stime - tsl_tms.tms_stime) / CLOCKS_PER_SEC,
          (double)(current_tms.tms_stime - tos_tms.tms_stime) / CLOCKS_PER_SEC);
  time_since_last = current;
  tsl_tms = current_tms;
  printf("--------\n");
}

void initialize(){
  time_of_start = time_since_last = clock(); /// has to be like this to ensure they are the same;
  times(&tos_tms); /// initialize start_tms structure with various clock tick data
  tsl_tms = tos_tms; /// intialize tsl to start the count
}


/// data generators

char *generateName(){
    char *newName = malloc(sizeof(char)*10);
    for(int i = 0; i < 9; i++){
        newName[i] = rand()%('z'-'a')+'a';  // returns a char from a to b, based on ascii numbers
    }
    return newName;
}

char *generateSurname(){
    char *newSurname = malloc(sizeof(char)*15);
    for(int i = 0; i < 15; i++){
        newSurname[i] = rand()%('z'-'a')+'a';
    }
    return newSurname;
}


char *generateAdress(){
    char *newAdress = malloc(sizeof(char)*25);
    for(int i = 0; i < 25; i++){
        newAdress[i] = rand()%('z'-'a')+'a';
    }
    return newAdress;
}
char *generateBirthdate(){
    char *newBirthDate = malloc(sizeof(char)*10);
    for(int i = 0; i < 10; i++){
        newBirthDate[i] = rand()%('9'-'0')+'0';
    }
    newBirthDate[4] = '-';
    newBirthDate[7] = '-';
    return newBirthDate;
 }
char *generateEmail(){
    char *newEmail = malloc(sizeof(char)*15);
    for(int i = 0; i < 10; i++){
        newEmail[i] = rand()%('z'-'a')+'a';
    }
    return newEmail;
}

char *generatePhoneNumber(){
    char *newPhoneNumber = malloc(sizeof(char)*9);
    for(int i = 0; i < 9; i++){
        newPhoneNumber[i] = rand()%('9'-'0')+'0';
    }
    return newPhoneNumber;

}

int main()
{
    void *library = dlopen("./libcontactbook.so", RTLD_LAZY);

    List *(*makeList)();
    makeList = dlsym(library, "makeList");

    void (*deleteList)(List *list);
    deleteList = dlsym(library, "deleteList");

    void (*addToList)(List *list, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber);
    addToList = dlsym(library, "addToList");

    void (*removeFromList)(List *list, char *name, char *surname, char *email);
    removeFromList = dlsym(library, "removeFromList");

    ListNode *(*findContact)(List *list, char *name, char *surname, char *email); // each contact diffrentiates with a different name, surname and email
    findContact = dlsym(library, "findContact");

    void (*sortList)(List *list, int option);
    sortList = dlsym(library, "sortList");

    ListNode *(*mergeSort)(ListNode *head, int option);
    mergeSort = dlsym(library, "mergeSort");

    ListNode *(*merge)(ListNode *list1, ListNode *list2, int option);
    merge = dlsym(library, "merge");

    void (*printlist)(List *list);
    printlist = dlsym(library, "printlist");
    /*
    Tree associated functions
    */
    Tree *(*makeTree)();
    makeTree = dlsym(library, "makeTree");

    void (*deleteTree)(Tree *tree);
    deleteTree = dlsym(library, "deleteTree");

    void (*deleteTreeNode)(TreeNode *treeNode);/// USED FOR deleteTree, not removing one contact from the tree
    deleteTreeNode = dlsym(library, "deleteTreeNode");

    void (*addToTree)(Tree *tree, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber);
    addToTree = dlsym(library, "addToTree");

    void (*addToSubtree)(TreeNode *newNode, TreeNode *currentNode, int order);
    addToSubtree = dlsym(library, "addToSubtree");

    void (*removeFromTree)(Tree *tree, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber);
    removeFromTree = dlsym(library, "removeFromTree");

    TreeNode *(*findTreeContact)(Tree *tree, char *name, char *surname, char *adress, char *birthDate, char *email, char *phoneNumber);
    findTreeContact = dlsym(library, "findTreeContact");

    Tree *(*sortTree)(Tree *tree, int argNumber);
    sortTree = dlsym(library, "sortTree");

    void (*printTree)(Tree *tree);
    printTree = dlsym(library, "printTree");

    void (*printNode)(TreeNode *treeNode, char *direction);
    printNode = dlsym(library, "printNode");
    /// used in both

    int (*compareContacts)(Contact *contact1, Contact *contact2, int option);
    compareContacts = dlsym(library, "compareContacts");




    srand(time(NULL));
    printf("Creation :\n");
    initialize();
    List *testList = makeList();
    addToList(testList, "mateusz", "machowski", "zawale", "19963010", "machowski.mat@gmail.com", "533863324");
    for(int i = 0; i<1000; i++){
      addToList(testList, generateName(), generateSurname(), generateAdress(), generateBirthdate(), generateEmail(), generatePhoneNumber());
    }
    addToList(testList, "testcase", "testcase", "testcase", "testcase", "testcase@gmail.com", "999999999");
        generateTimeData("List");

    Tree *testTree = makeTree();
    addToTree(testTree, "mateusz", "machowski", "zawale", "19963010", "machowski.mat@gmail.com", "533863324");
    for(int i = 0; i<1000; i++){
      addToTree(testTree, generateName(), generateSurname(), generateAdress(), generateBirthdate(), generateEmail(), generatePhoneNumber());
    }
    addToTree(testTree, "testcase", "testcase", "testcase", "testcase", "testcase@gmail.com", "999999999");
        generateTimeData("Tree");

    printf("Finding :\nOptimistic:\n");/// TIMECOUNT for find optimistic
    ListNode *testNode = findContact(testList, "mateusz", "machowski", "machowski.mat@gmail.com");
        generateTimeData("List");
    TreeNode *testTreeNode = findTreeContact(testTree, "mateusz", "machowski", "zawale", "19963010", "machowski.mat@gmail.com", "533863324");
        generateTimeData("Tree");

    printf("Pessimistic\n");/// TIMECOUNT for pessimistic
    ListNode *testNode2 = findContact(testList, "testcase", "testcase", "testcase");
        generateTimeData("List");
    TreeNode *testTreeNode2 = findTreeContact(testTree, "testcase", "testcase", "testcase", "testcase", "testcase@gmail.com", "999999999");
        generateTimeData("Tree");

    printf("Removing :\n Optimistic:\n");
    removeFromList(testList, "mateusz", "machowski", "machowski.mat@gmail.com");
        generateTimeData("List");
    removeFromTree(testTree, "mateusz", "machowski", "zawale", "19963010", "machowski.mat@gmail.com", "533863324");
        generateTimeData("Tree");

    printf("Pessimistic\n");
    removeFromList(testList, "testcase", "testcase", "testcase@gmail.com");
        generateTimeData("List");
    removeFromTree(testTree, "testcase", "testcase", "testcase", "testcase", "testcase@gmail.com", "999999999");
        generateTimeData("Tree");

    printf("Sorting :\n");
    sortList(testList, 0);
        generateTimeData("List");
    testTree = sortTree(testTree, 5);
        generateTimeData("Tree");

    printf("Destroying :\n");
    deleteList(testList);
        generateTimeData("List");
    deleteTree(testTree);
        generateTimeData("Tree");
    dlclose(library);
    return 0;
}

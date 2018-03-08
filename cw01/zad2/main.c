#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
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
          (double)(current_tms.tms_utime - tsl_tms.tms_utime) / 10,
          (double)(current_tms.tms_utime - tos_tms.tms_utime) / 10);
  // system time, tms_stime
  printf("'System' measurement : difference since last = %.12f, time elapsed since start = %.12f\n",
          (double)(current_tms.tms_stime - tsl_tms.tms_stime) / 10,
          (double)(current_tms.tms_stime - tos_tms.tms_stime) / 10);
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
void addcontact_list(List *list){
    addToList(list, generateName(), generateSurname(), generateAdress(), generateBirthdate(), generateEmail(), generatePhoneNumber());
}

void addcontact_tree(Tree *tree){
    addToTree(tree, generateName(), generateSurname(), generateAdress(), generateBirthdate(), generateEmail(), generatePhoneNumber());
}

int main()
{
    srand(time(NULL));
    printf("Creation :\n");

    initialize();

    List *testList = makeList();
    addToList(testList, "mateusz", "machowski", "zawale", "19963010", "machowski.mat@gmail.com", "533863324");
    for(int i = 0; i<1000; i++){
      addcontact_list(testList);
    }
    addToList(testList, "testcase", "testcase", "testcase", "testcase", "testcase@gmail.com", "999999999");
        generateTimeData("List");

    Tree *testTree = makeTree();
    addToTree(testTree, "mateusz", "machowski", "zawale", "19963010", "machowski.mat@gmail.com", "533863324");
    for(int i = 0; i<1000; i++){
      addcontact_tree(testTree);
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
    return 0;
}

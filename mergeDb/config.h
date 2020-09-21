//
// Created by ccddr on 29/03/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql.h"

#ifndef MERGE2DB_CONFIG_H
#define MERGE2DB_CONFIG_H

#endif //MERGE2DB_CONFIG_H

//Struct allowing to recover the columns name of a database
struct ColumnsInfo
{
    char* name;
    struct ColumnsInfo* next;
    int ifAutoIncrement;
}; typedef struct ColumnsInfo ColumnsInfo;

//Struct allowing to recover the tables name of a database
struct TableInfo
{
    char* name;
    struct ColumnsInfo* firstColumn;
    int columnSum;
    struct TableInfo* next;
}; typedef struct TableInfo TableInfo;

//the stuc allowing initiate the list of tables name
struct ListTable
{
    TableInfo *first;
    int sum;
}; typedef struct ListTable ListTable;


//info to the database source
char* sqlHostSource;
char* sqlDbSource;
char* sqlUserSource;
char* sqlPswdSource;

//Info of the database who receive data
char* sqlHostTarget;
char* sqlDbTarget;
char* sqlUserTarget;
char* sqlPswdTarget;

void insertBeginListTable(ListTable* listTable,  char tableName[]);
ListTable* recoverInfoTableColumn();
void recoverTableName(ListTable* listTable, MYSQL* mysql);
void recoverColumnsName(ListTable* listTable, MYSQL* mysql);
void insertBeginListColumns(TableInfo* tableInfo, char columnName[], const char* autoIncrement);
void showListTexture(ListTable* listTable);
void destroyListTable(ListTable* listTable);
void recoverDataSource(ListTable* infoTableColumn);
char* getColumnName(TableInfo* tableInfo, unsigned long* lenght, int* nbcolumns);
void addDataInDbTarget(ListTable* listTable);
MYSQL* initMysql(char* host, char* db, char* user, char* pswd);
MYSQL_RES* sqlQuery(const char query[], MYSQL* mysql);
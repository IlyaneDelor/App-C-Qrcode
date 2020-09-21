#include "config.h"




int main(int argc, char** argv) {

    //Affect has variables the parameters to connect of the database
    if(argc > 8){
        sqlHostSource = argv[1];
        sqlDbSource = argv[2];
        sqlUserSource = argv[3];
        sqlPswdSource= argv[4];

        sqlHostTarget = argv[5];
        sqlDbTarget = argv[6];
        sqlUserTarget = argv[7];
        sqlPswdTarget= argv[8];
//        for (int i = 0; i < argc; ++i) {
//            printf("%d.%s\n", i, argv[i]);
//        }

        ListTable* infoTableColumn;
        infoTableColumn =  recoverInfoTableColumn();
        recoverDataSource(infoTableColumn);
        addDataInDbTarget(infoTableColumn);
        destroyListTable(infoTableColumn);

    } else{
        printf("Argument issue");
        return 0;
    }
}

//Connection of the database
MYSQL* initMysql(char* host, char* db, char* user, char* pswd)
{

    MYSQL *mysql = NULL;
    mysql = mysql_init(NULL);
    if(mysql_real_connect(mysql,host, user, pswd, db,0,NULL,0))
    {
        return mysql;
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n",mysql_error(mysql));
        return NULL;
    }
}

//lets send a request of the database
MYSQL_RES* sqlQuery(const char query[], MYSQL* mysql)
{
    MYSQL_RES* res = NULL;
    char* sql_cmd = malloc(sizeof(char) * strlen(query));


    sprintf(sql_cmd, query);
    if(mysql_query(mysql, sql_cmd)){
        fprintf(stderr, "%s\n", mysql_error(mysql));

    }
    else
    {
        res = mysql_store_result(mysql);
    }

    free(sql_cmd);
    return res;
}

ListTable* initListTable(){
    ListTable* listTable = NULL;
    listTable = malloc(sizeof(ListTable));
    //TextureInfo *TextureInfo = malloc(sizeof(*TextureInfo));

    if (listTable== NULL )
    {
        exit(EXIT_FAILURE);
    }

    listTable->first = NULL;
    listTable->sum = 0;

    return listTable;
}

void insertBeginListTable(ListTable* listTable,  char tableName[]){

    TableInfo* nouveau = malloc(sizeof(TableInfo));
    if (listTable == NULL || nouveau == NULL)
    {
        exit(EXIT_FAILURE);
    }
    nouveau->name = tableName;
    nouveau->firstColumn = NULL;
    nouveau->columnSum = 0;

    nouveau->next = listTable->first;
    listTable->first = nouveau;
    listTable->sum++;
}

void showListTexture(ListTable* listTable){
    if (listTable == NULL)
    {
        exit(EXIT_FAILURE);
    }

    TableInfo* actuel = listTable->first;
    ColumnsInfo* actualColumn = NULL;

    while (actuel != NULL)
    {
        actualColumn = actuel->firstColumn;
        while (actualColumn != NULL){
            actualColumn = actualColumn->next;
        }
        actuel = actuel->next;
    }
}

void insertBeginListColumns(TableInfo* tableInfo, char columnName[], const char* autoIncrement){

    ColumnsInfo* nouveau = malloc(sizeof(ColumnsInfo));
    if (tableInfo == NULL || nouveau == NULL)
    {
        exit(EXIT_FAILURE);
    }

    nouveau->ifAutoIncrement=(strcmp("auto_increment", autoIncrement) == 0)?1:0;
    nouveau->name = columnName;

    nouveau->next = tableInfo->firstColumn;
    tableInfo->firstColumn = nouveau;
    tableInfo->columnSum++;
}

void destroyListTable(ListTable* listTable){
    if(listTable == NULL){
        exit(EXIT_FAILURE);
    }

    TableInfo* delElementTable = NULL;
    TableInfo* currentElementTable = listTable->first;
    ColumnsInfo* delElementColumn = NULL;
    ColumnsInfo* currentElementColumn = NULL;

    while (currentElementTable != NULL){
        currentElementColumn = currentElementTable->firstColumn;
        while (currentElementColumn != NULL){

            delElementColumn = currentElementColumn;
            currentElementColumn = currentElementColumn->next;
            free(delElementColumn);
        }

        delElementTable = currentElementTable;
        currentElementTable = currentElementTable->next;
        free(delElementTable);
    }
}



ListTable* recoverInfoTableColumn(){
    MYSQL* mysql = initMysql(sqlHostSource, sqlDbSource, sqlUserSource, sqlPswdSource);
    ListTable* listTableSource = initListTable();

    recoverTableName(listTableSource, mysql);
    recoverColumnsName(listTableSource, mysql);


    //showListTexture(listTableSource);
    return listTableSource;
}

void recoverTableName(ListTable* listTable, MYSQL* mysql){
    MYSQL_ROW row;
    MYSQL_RES* namesTables = sqlQuery("SHOW TABLES", mysql);


    while ((row = mysql_fetch_row(namesTables)) != NULL){

        insertBeginListTable(listTable, row[0]);
    }
}

void recoverColumnsName(ListTable* listTable, MYSQL* mysql){
    MYSQL_ROW row;
    MYSQL_ROW rowColumn;
    MYSQL_RES* columnsName;
    MYSQL_RES* infoColumns;

    if (listTable == NULL)
    {
        exit(EXIT_FAILURE);
    }

    TableInfo *actuel = listTable->first;

    while (actuel != NULL)
    {
        char* query = malloc(sizeof(char) * strlen(actuel->name) + 20);
        sprintf(query, "SHOW COLUMNS FROM %s;", actuel->name);
        columnsName = sqlQuery(query, mysql);

        char* queryInfoColumn = malloc(sizeof(char) * strlen("DESC ") + strlen(actuel->name) + 1);
        sprintf(queryInfoColumn,"DESC %s", actuel->name);
        //printf("%s", queryInfoColumn);
        infoColumns = sqlQuery(queryInfoColumn, mysql);

        while((row = mysql_fetch_row(columnsName)) != NULL && (rowColumn = mysql_fetch_row(infoColumns)) != NULL){
            insertBeginListColumns(actuel, row[0], rowColumn[5]);
        }

        free(query);
        actuel = actuel->next;

    }
}

void recoverDataSource(ListTable* infoTableColumn){
    FILE* actualFile = NULL;
    MYSQL* mysql = initMysql(sqlHostSource, sqlDbSource, sqlUserSource, sqlPswdSource);
    MYSQL_RES* data;
    MYSQL_ROW row;
    TableInfo* actualTable = infoTableColumn->first;
    //size_t lenght;
    int nbColumns;
    unsigned long lenghtColumn = 0;
    unsigned long nbChar;

    while (actualTable != NULL){
        nbChar = 0;
        actualFile = fopen(actualTable->name, "w");
        if (actualFile != NULL) {

            char* columnsName = getColumnName(actualTable, &lenghtColumn, &nbColumns);
            nbChar += strlen(actualTable->name);
            nbChar += lenghtColumn;
            char *query = malloc(sizeof(char) * (nbChar + strlen("SELECT FROM ") + strlen(actualTable->name)));
            sprintf(query, "SELECT %s FROM %s", columnsName, actualTable->name);
            free(columnsName);

            data = sqlQuery(query, mysql);

            while ((row = mysql_fetch_row(data)) != NULL) {
                for (int i = 0; i < nbColumns; ++i) {
                    if(nbColumns - 1 == i){
                        fprintf(actualFile,"\'%s\';", row[i]);
                        //printf("%s;", row[i]);
                    } else{
                        fprintf(actualFile, "\'%s\',", row[i]);
                        //printf("%s,", row[i]);
                    }
                }
            }
            free(query);
            //printf("%s", actualTable->name);
            actualTable = actualTable->next;
            fclose(actualFile);
        }
        else{
            actualTable = actualTable->next;
        }
    }
}

char* getColumnName(TableInfo* tableInfo, unsigned long* lenght,int* nbColumns){
    ColumnsInfo* actualColumn = NULL;
    actualColumn = tableInfo->firstColumn;
    *lenght = 0;
    *nbColumns = 0;
    //Count the character number of columns name
    while (actualColumn != NULL) {
        if (actualColumn->ifAutoIncrement == 0) {
            *lenght += strlen(actualColumn->name);
            *nbColumns += 1;
        }
        actualColumn = actualColumn->next;
    }

    char* columnsName = malloc(sizeof(char) * *lenght);
    columnsName[0] = '\0';
    actualColumn = tableInfo->firstColumn;

    //Affect at columnsName all column : column1,column2,...,columnN
    while (actualColumn != NULL) {
        if (actualColumn->ifAutoIncrement == 0) {
            strcat(columnsName, actualColumn->name);
            strcat(columnsName, ",");
        }
        actualColumn = actualColumn->next;
    }
    columnsName[strlen(columnsName) - 1] = '\0';

    return columnsName;
}

void addDataInDbTarget(ListTable* listTable){
    MYSQL* mysql = initMysql(sqlHostTarget, sqlDbTarget, sqlUserTarget, sqlPswdTarget);
    FILE* actualFile;
    TableInfo* actualTable = listTable->first;

    int nbColumns;
    unsigned long lenghtColumn;
    unsigned long nbChar;
    int c;
    int nbLigne;
    char values[10000];


    while (actualTable != NULL){
        printf("%s", actualTable->name);
        lenghtColumn = 0;
        nbColumns = 0;
        nbLigne = 0;
        nbChar = 0;
        int j;
        actualFile = fopen(actualTable->name, "r");
        if (actualFile != NULL) {
////
            char* columnsName = getColumnName(actualTable, &lenghtColumn, &nbColumns);
            nbChar += strlen(actualTable->name);
            nbChar += lenghtColumn;
////
            while ((c = fgetc(actualFile)) != EOF) {
                if (c == ';'){
                    nbLigne += 1;
                }
            }
////
            fseek(actualFile, 0, SEEK_SET);
            values[0] = '\0';
            for (int i = 0; i < nbLigne; ++i) {
                for (j = 0;  (c = fgetc(actualFile)) != ';'; ++j) {
                    values[j] = (char)c;
                }
                values[j] = '\0';
                printf("%s", values);
////
////
////
                char *query = malloc(sizeof(char) * (nbChar + strlen("INSERT INTO () VALUES () ") + strlen(actualTable->name) + strlen(values)));
                sprintf(query, "INSERT INTO %s (%s) VALUES (%s);", actualTable->name, columnsName, values);
                printf("\nquery : %s\n\n", query);
                sqlQuery(query, mysql);
                free(query);
            }
////
////
            free(columnsName);
            remove(actualTable->name);
            fclose(actualFile);
        }



        actualTable = actualTable->next;
    }
}
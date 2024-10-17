#include <iostream>
#include <cjson/cJSON.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include "HashTable.h"
#include "List.h"
using namespace std;

void lockTable(const string& pathToDir)
{
    ofstream lockFile("Схема 1/" + pathToDir + "/" + pathToDir + "_lock");
    lockFile << 1;
    lockFile.close();
}

void unlockTable(const string& pathToDir)
{
    ofstream lockFile("Схема 1/" + pathToDir + "/" + pathToDir + "_lock");
    if (lockFile.bad()) cerr << "error opening lock";
    lockFile << 0;
    lockFile.close();
}

string readJSON(const string& fileName)
{
    fstream file(fileName);
    if (!file.is_open())
    {
        cerr << "Error reading " << fileName << ".json file!" << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

bool createDir(const string& dirName)
{
    if (filesystem::create_directory(dirName)) return true;
    else return false;
}

void createFilesInSubfolder(const cJSON* table, const cJSON* structure, const string& subName)
{
    HASHtable<string> columnNames; // Sozdanie i zagruzka soderjimogo v .csv
    cJSON* array = cJSON_GetObjectItem(structure, table->string);
    int arraySize = cJSON_GetArraySize(array);

    for (int i = 0; i < arraySize; i++)
    {
        cJSON* arrayItem = cJSON_GetArrayItem(array, i);
        columnNames.HSET(to_string(i), arrayItem->valuestring);
    }
    
    ofstream CSV(subName + "/1.csv"); // zapolnenie .csv
    for (int i = 0; i < columnNames.size(); i++)
    {   
        if(i == columnNames.size()- 1)
        {
            CSV << columnNames.HGET(to_string(i)) << "";
            break;
        } 
        CSV << columnNames.HGET(to_string(i)) << ",";
    }
    CSV << endl;
    CSV.close();
    
    
    
    ofstream PKSEQ(subName + "/" + table->string +"_pk_sequence"); // cozdanie schetchika
    PKSEQ << "1";
    PKSEQ.close();

    ofstream PKLOCK(subName + "/" + table->string +"_lock"); // cozdanie mutexa
    PKLOCK << "0";
    PKLOCK.close();
}

bool isFileEmpty(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::ate);  // ios::ate открывает файл в конце
    return file.tellg() == 0;  // Если позиция указателя 0, значит файл пустой
}

void createDB()
{
    ofstream DBFlag("DBflag"); //flag nalichiya db
    DBFlag.close();

    string jsonContent = readJSON("schema.json"); // otkritie .json

    if (jsonContent.empty())
    {
        cerr << "Error reading schema.json file!" << endl;
        exit(-1);
    }

    cJSON* json = cJSON_Parse(jsonContent.c_str()); // parsing .json

    if (json == nullptr)
    {
        cerr << "Error parsing schema.json file!" << endl;
        exit(-1);
    }

    cJSON* tuple = cJSON_GetObjectItem(json, "tuples_limit");
    int tuplesLimit = tuple->valueint;
    cJSON* schemaName = cJSON_GetObjectItem(json, "name");
    string DataBaseName = schemaName->valuestring;
    createDir(DataBaseName); //papka DB

    cJSON* structure = cJSON_GetObjectItem(json, "structure"); //parsing structuri

    for (cJSON* table = structure->child; table != nullptr;table = table->next) // sozdanie papok tablic
    {
        string subName = DataBaseName+"/"+table->string;
        createDir(subName);
        createFilesInSubfolder(table, structure, subName);
    }

    cJSON_Delete(json);
}

void decreaseSequence(const string& pathToDir)
{
    string currentNum;

    ifstream pk_seqRead("Схема 1/" + pathToDir + "/" + pathToDir + "_pk_sequence");
    if (pk_seqRead.bad()) cerr << "Error with pk_seq";
    getline(pk_seqRead,currentNum,'\n');
    pk_seqRead.close();

    ofstream pk_seqWrite("Схема 1/" + pathToDir + "/" + pathToDir + "_pk_sequence");
    if (pk_seqWrite.bad()) cerr << "Error with pk_seq";
    pk_seqWrite << stoi(currentNum)-1;
    pk_seqWrite.close();
    
}

void increaseSequence(const string& pathToDir)
{
    string currentNum;

    ifstream pk_seqRead("Схема 1/" + pathToDir + "/" + pathToDir + "_pk_sequence");
    if (pk_seqRead.bad()) cerr << "Error with pk_seq";
    getline(pk_seqRead,currentNum,'\n');
    pk_seqRead.close();

    ofstream pk_seqWrite("Схема 1/" + pathToDir + "/" + pathToDir + "_pk_sequence");
    if (pk_seqWrite.bad()) cerr << "Error with pk_seq";
    pk_seqWrite << stoi(currentNum)+1;
    pk_seqWrite.close();
    
}

string subString(string oldLine, int startIndex, int endIndex)
{
    string newLine = "";
    for (int i = startIndex; i < endIndex; i++)
    {
        newLine += oldLine[i];
    }
    return newLine;
}

List<string> splitLineIntoColumns(string& rawLine)
{
    string columnLetters;
    List<string> columnName{"Empty"};
    for (int i = 0; i < rawLine.size(); i++)
    {
        if (rawLine[i] == ',' || rawLine[i] == ' ' || rawLine[i] == '\n')
        {
            columnName.LPUSH(columnLetters);
            columnLetters = "";
            continue;
        }
        else columnLetters += rawLine[i];
    }
    if(columnLetters.size() != 0) columnName.LPUSH(columnLetters);
    columnName.LDEL(0);
    return columnName;
}

List<HASHtable<string>> readTableContent(string tableName)
{
    HASHtable<string> dummy;
    List<HASHtable<string>> fullTable{dummy};
    string currentLine;

    // Открытие CSV-файла
    fstream tableFile("Схема 1/" + tableName + "/1.csv");
    if (!tableFile.is_open()) {
        cerr << "Wrong tablename!" << endl;
        exit(-1);
    }

    // Чтение заголовков таблицы
    getline(tableFile, currentLine);
    List<string> TABLECOLUMNS = splitLineIntoColumns(currentLine);
    int tableWidth = TABLECOLUMNS.GetSize();

    // Открытие sequence-файла
    fstream pkSeq("Схема 1/" + tableName+ "/" + tableName + "_pk_sequence");
    if (!pkSeq.is_open()) {
        cerr << "Error: Could not open sequence file." << endl;
        exit(-1);
    }

    // Чтение количества строк в таблице
    string pkSeqContent;
    getline(pkSeq, pkSeqContent);
    pkSeq.close();
    
    int amountOfLinesInTable = stoi(pkSeqContent);

    // Если в таблице только заголовок, возвращаем её
    if (amountOfLinesInTable == 1) {
        return fullTable;
    }

    // Чтение строк данных
    for (int i = 1; i < amountOfLinesInTable; i++) {
        HASHtable<string> row(tableWidth);
        getline(tableFile, currentLine);

        // Разбиваем строку на значения столбцов
        List<string> columnValues = splitLineIntoColumns(currentLine);

        // Сопоставляем значения с названиями столбцов
        for (int j = 0; j < columnValues.GetSize(); j++) {
            row.HSET(TABLECOLUMNS.getElement(j), columnValues.getElement(j));
        }

        fullTable.LPUSH(row);  
    }
    fullTable.LDEL(0);
    tableFile.close();  // Закрываем файл
    return fullTable;  // Возвращаем заполненную таблицу
}


void writeOutTableFile(List<HASHtable<string>>& table, const string& pathToDir)
{
    // Пути к файлам
    
    std::string sequencePath = "Схема 1/" + pathToDir + "/" + pathToDir + "_pk_sequence";
    std::string tablePath = "Схема 1/" + pathToDir + "/1.csv";
    
    cout << pathToDir << endl;
    cout << sequencePath << endl << tablePath << endl;

    // Чтение количества строк из sequence-файла
    std::string pk_seqInput;
    std::ifstream pk_seq(sequencePath);
    if (!pk_seq.is_open()) {
        std::cerr << "Error: Could not open sequence file." << std::endl;
        return;
    }
    std::getline(pk_seq, pk_seqInput);
    int amountOfLines = std::stoi(pk_seqInput);
    pk_seq.close();

    string firstLine;

    fstream tableFile("Схема 1/" + pathToDir + "/1.csv");
    if(tableFile.bad())
    {
        cerr << "Wrong tablename!" << endl;
        return;
    }

    getline(tableFile, firstLine);
    List<string> TABLECOLUMNS = splitLineIntoColumns(firstLine);
    tableFile.close();

    // Открываем CSV файл в режиме добавления (или создаем новый с очисткой)
    ofstream newTableFile(tablePath, std::ios::out | std::ios::trunc);
    if (!newTableFile.is_open()) {
        cerr << "Error: Could not open or create table file." << std::endl;
        return;
    }

    // --- Записываем заголовки только один раз (если файл пустой) ---
    if (isFileEmpty(tablePath)) {
        for (size_t j = 0; j < TABLECOLUMNS.GetSize(); ++j) {
            newTableFile << TABLECOLUMNS.getElement(j);
            if (j < TABLECOLUMNS.GetSize() - 1) {
                tableFile << ",";  // Запятая между заголовками
            }
        }
        newTableFile << '\n';  // Переход на новую строку после заголовков
    }

    // --- Записываем строки данных ---
    for (int i = 1; i < table.GetSize(); ++i) {
        for (size_t j = 0; j < TABLECOLUMNS.GetSize(); ++j) {
            newTableFile << table.getElement(i).HGET(TABLECOLUMNS.getElement(j));
            if (j < TABLECOLUMNS.GetSize() - 1) {
                newTableFile << ",";  // Запятая между значениями
            }
        }
        newTableFile << '\n';  // Переход на новую строку после каждой строки данных
    }

    newTableFile.close();  // Закрываем файл

    // Обновляем sequence-файл с новым количеством строк
    std::ofstream pk_seq_out(sequencePath, std::ios::out | std::ios::trunc);
    if (pk_seq_out.is_open()) {
        pk_seq_out << table.GetSize();  // Обновляем количество строк
        pk_seq_out.close();
    }
}

// void deleteFromTable(string tableName, string condition)
// {
//     lockTable(tableName);
//     List<string> conditionList = handleCondition(condition);
//     conditionList.LDEL(0);
//     List<HASHtable<string>> table = readTableContent(tableName);
//     for (int i = 0; i < table.GetSize(); i++)
//     {
//         if (checkCondition(conditionList, table.getElement(i)))
//         {   
//             table.getElement(i).HSET(conditionList.getElement(0),"EMPTY_CELL");
//             table.getElement(i).print();
//         }
//     }
//     writeOutTableFile(table,tableName,columnNames);
//     unlockTable(tableName);
// }

void insertIntoTable(List<HASHtable<string>>& table, const string& pathToDir,
                     List<string>& values)
{
    lockTable(pathToDir);  // Блокируем таблицу

    fstream tableFile("Схема 1/" + pathToDir + "/1.csv");
    if(tableFile.bad())
    {
        cerr << "Wrong tablename!" << endl;
        exit(-1);
    }
    string firstLine;
    getline(tableFile, firstLine);
    List<string> columnNames = splitLineIntoColumns(firstLine);
    tableFile.close();


    int tableWidth = columnNames.GetSize();
    HASHtable<string> loadline;

    if (tableWidth == values.GetSize()) {
        // Равное количество колонок и значений
        for (int i = 0; i < tableWidth; i++) {
            loadline.HSET(columnNames.getElement(i), values.getElement(i));
        }
        table.LPUSH(loadline);  // Добавляем строку в таблицу
    } 
    else if (tableWidth > values.GetSize()) {
        // Больше колонок, чем значений — заполняем пустыми ячейками
        for (int i = 0; i < values.GetSize(); i++) {
            loadline.HSET(columnNames.getElement(i), values.getElement(i));
        }
        for (int i = values.GetSize(); i < tableWidth; i++) {
            loadline.HSET(columnNames.getElement(i), "EMPTY_CELL");
        }
        table.LPUSH(loadline);
    } 
    else {
        // Больше значений, чем колонок — разделяем на несколько строк
        int index = 0;
        while (index < values.GetSize()) {
            loadline = {};  // Начинаем новую строку

            for (int i = 0; i < tableWidth && index < values.GetSize(); i++, index++) {
                loadline.HSET(columnNames.getElement(i), values.getElement(index));
            }

            // Если строка неполная, заполняем оставшиеся ячейки
            for (int i = loadline.size(); i < tableWidth; i++) {
                loadline.HSET(columnNames.getElement(i), "EMPTY_CELL");
            }

            table.LPUSH(loadline);  // Добавляем строку в таблицу
        }
    }

    // Увеличиваем счетчик строк и записываем таблицу в файл
    increaseSequence(pathToDir);
    writeOutTableFile(table, pathToDir);

    unlockTable(pathToDir);  // Разблокируем таблицу
}

List<string> handleUserInput(string& input)
{
    string command = "";
    string conditions = "";  // Переменная для хранения всего, что идёт после WHERE
    List<string> commandList{"empty"};
    bool isWhereClause = false;  // Флаг для определения секции WHERE

    for (int i = 0; i < input.size(); i++)
    {
        // Если встречаем WHERE, всё, что идёт после, должно быть одним элементом
        if (!isWhereClause && i + 5 <= input.size() && input.substr(i, 5) == "WHERE")
        {
            if (command.size() != 0) commandList.LPUSH(command);
            commandList.LPUSH("WHERE");  // Добавляем ключевое слово WHERE
            command = "";
            i += 5;  // Пропускаем слово WHERE
            isWhereClause = true;  // Активируем флаг секции WHERE
            continue;
        }

        if (isWhereClause)
        {
            // Всё, что идёт после WHERE, добавляем как одно целое в переменную conditions
            conditions += input[i];
        }
        else
        {
            // До WHERE обрабатываем, разделяя по символам-разделителям
            if (input[i] == ' ' || input[i] == '\'' || input[i] == ',' || 
                input[i] == '(' || input[i] == ')' || input[i] == '.' || input[i] == '=')
            {
                if (command.size() != 0) commandList.LPUSH(command);
                command = "";
            }
            else
            {
                command += input[i];
            }
        }
    }

    // Добавляем последнее значение в массив, если оно не пустое
    if (command.size() != 0 && !isWhereClause) commandList.LPUSH(command);

    // Добавляем всю секцию WHERE в массив команд как одно целое
    if (!conditions.empty()) commandList.LPUSH(conditions);

    commandList.LDEL(0);
    return commandList;
}

void handleCommands(List<string> commandList)
{
    
    if (commandList.GetSize() == 0)
    {
        cerr << "Empty query for program!" << endl;
        exit(0);
    }
    if (commandList.getElement(0) == "SELECT")
    {
        
        commandList.LDEL(0);
        cout << "SELECT FROM has been called!" << endl;
        commandList.Print();

        string firstTable = commandList.getElement(0);
        commandList.LDEL(0);
        string firstValue = commandList.getElement(0);
        commandList.LDEL(0);
        string secondTable = commandList.getElement(0);
        commandList.LDEL(0);
        string secondValue = commandList.getElement(0);
        commandList.LDEL(0);
        //List<string> condition = handleCondition(commandVector[0]);
        //selectColumns(firstTable, firstValue,  secondTable, secondValue,
        //condition);
    }
    else if (commandList.getElement(0) == "INSERT" && commandList.getElement(1) == "INTO")
    {
        commandList.LDEL(0);
        commandList.LDEL(0);

        cout << "INSERT INTO has been called!" << endl;
        string tableName = commandList.getElement(0);
        
        commandList.LDEL(0);

        if (commandList.getElement(0) != "VALUES")
        {
            cerr << "Wrong syntax for INSERT" << endl;
            return;
        }
        
        commandList.LDEL(0);
        
        List<HASHtable<string>> fullTable = readTableContent(tableName);
        insertIntoTable(fullTable,tableName,commandList);
        
    }
    else if (commandList.getElement(0) == "DELETE" && commandList.getElement(1) == "FROM")
    {
        commandList.LDEL(0);
        commandList.LDEL(0);
        cout << "DELETE FROM has been called!" << endl;
        string tableName = commandList.getElement(0);
        commandList.LDEL(0);
        commandList.LDEL(0);
        //deleteFromTable(tableName, commandList.getElement(0));
        
    }
}

int main()
{
    //createDB();
    List<HASHtable<string>> tablica = readTableContent("таблица1");
    string userInput;
    getline(cin, userInput);
    List<string> InputList = handleUserInput(userInput);
    handleCommands(InputList);
    
    
    
}

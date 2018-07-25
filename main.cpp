/**
* \author: Tom BEFIEUX
* \contact: contact@tom-befieux.fr
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <windows.h>

using namespace std;

int main()
{
    // vars
    unsigned int nbOfCols, cpt = 0;
    char separator;
    string filePath, tableName, resultFilePath, currentLine, finalResult;
    vector<string> colsName, colsType;
    vector< vector<string> > lines;

    // get the number of columns (only for windows)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    // display
    cout << endl;
    cout << "     _____  _______      __   __ _ _        _           _____  ____  _" << endl;
    cout << "    / ____|/ ____\\ \\    / /  / _(_) |      | |         / ____|/ __ \\| |" << endl;
    cout << "   | |    | (___  \\ \\  / /  | |_ _| | ___  | |_ ___   | (___ | |  | | |" << endl;
    cout << "   | |     \\___ \\  \\ \\/ /   |  _| | |/ _ \\ | __/ _ \\   \\___ \\| |  | | |" << endl;
    cout << "   | |____ ____) |  \\  /    | | | | |  __/ | || (_) |  ____) | |__| | |____" << endl;
    cout << "    \\_____|_____/    \\/     |_| |_|_|\\___|  \\__\\___/  |_____/ \\___\\_\\______|" << endl;
    cout << endl;
    cout << "    A software made by Tom BEFIEUX." << endl;
    cout << "    Contact: contact@tom-befieux.fr" << endl;
    cout << endl;
    for(int i = 0; i < columns; i++)
        cout << "/";
    cout << endl;
    cout << "    CSV file to SQL converter can convert a basic CVS file into a SQL file." << endl;
    cout << "    IMPORTANT: The first line of the file must contain the names of the columns." << endl;
    cout << endl;
    for(int i = 0; i < columns; i++)
        cout << "/";

    // ask informations
    cout << endl << " First, we need some informations:" << endl << endl;
    cout << "   The full path of the CSV file to convert (example: bdd.csv): ";
    cin >> filePath;
    cout << "   The full path of the final SQL file (example: bdd.sql): ";
    cin >> resultFilePath;
    cout << "   The separator character used in the CSV file (normally ';'): ";
    cin >> separator;
    cout << "   The number of columns: ";
    cin >> nbOfCols;
    cout << "   The name of the table: ";
    cin >> tableName;
    cout << endl;

    // open
    ifstream file(filePath.c_str(), ios::in);
    if(!file)
    {
        cerr << "   ERROR: Failed to open the CSV file :'(" << endl << endl;
        system("pause");
        return 1;
    }

    // get the colsName
    cout << " |            |       Get the names of the columns..." << '\r';
    getline(file, currentLine);
    if(currentLine.at(cpt) == separator) cpt++; // focus on the beginning
    for(unsigned int i = 0; i < nbOfCols; i++)
    {
        string currentCol;
        while(cpt < currentLine.size() && currentLine.at(cpt) != separator)
        {
            currentCol += currentLine.at(cpt);
            cpt++;
        }
        cpt++;
        colsName.push_back(currentCol);
    }

    // load the data
    cout << " |//          |       Load data...                           " << '\r';
    while(getline(file, currentLine))
    {
        vector<string> dataInLine;
        cpt = 0;
        if(currentLine.at(cpt) == separator) cpt++; // focus on the beginning
        for(unsigned int i = 0; i < nbOfCols; i++)
        {
            string currentCol;
            while(cpt < currentLine.size() && currentLine.at(cpt) != separator)
            {
                currentCol += currentLine.at(cpt);
                cpt++;
            }
            cpt++;
            dataInLine.push_back(currentCol);
        }

        lines.push_back(dataInLine);
    }

    // don't use the file anymore
    file.close();

    // analyze the data to get the type of the cols
    cout << " |////        |       Analyze data...                                    " << '\r';
    for(unsigned int j = 0; j < nbOfCols; j++)
    {
        bool isDecimal = true, isInteger = true;
        unsigned int maxLenght = lines.at(0).at(j).size();
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            // check if it's not decimal
            if(
                isDecimal &&  // doesn't do atof for nothing
                atof(lines.at(i).at(j).c_str()) == 0.0 &&
                lines.at(i).at(j) != "0.0" &&
                lines.at(i).at(j) != "0.00" &&
                lines.at(i).at(j) != "0.000" &&
                lines.at(i).at(j) != "0.0000" &&
                lines.at(i).at(j).size() != 0 &&
                lines.at(i).at(j) != "Null" &&
                lines.at(i).at(j) != "NULL" &&
                lines.at(i).at(j) != "null"
               )
                isDecimal = false;

            // check if it's not integer
            if(
                isInteger &&
                atoi(lines.at(i).at(j).c_str()) == 0 &&
                lines.at(i).at(j) != "0" &&
                lines.at(i).at(j).size() != 0 &&
                lines.at(i).at(j) != "Null" &&
                lines.at(i).at(j) != "NULL" &&
                lines.at(i).at(j) != "null"
               )
                isInteger = false; // doesn't do atoi for nothing

            // the max
            if(maxLenght < lines.at(i).at(j).size()) maxLenght = lines.at(i).at(j).size();
        }

        string colType;

        // type
        if(isDecimal) colType = "DECIMAL";
        else if(isInteger) colType = "INT";
        else colType = "VARCHAR";

        // max
        colType += "(" + to_string(maxLenght);
        if(isDecimal) colType += ".2";
        colType += ")";

        // add
        colsType.push_back(colType);
    }

    // create the table
    cout << " |//////      |       Create table...                                        " << '\r';
    finalResult = "/* CREATE TABLE */\n";
    finalResult += "CREATE TABLE IF NOT EXISTS " + tableName + "(\n";
    for(unsigned int i = 0; i < nbOfCols; i++)
    {
        finalResult += colsName.at(i) + " " + colsType.at(i);
        if(i != nbOfCols - 1) finalResult += ",";
        finalResult += "\n";
    }
    finalResult += ");\n\n";

    // add the inserts
    cout << " |////////    |       Create insert queries...                               " << '\r';
    finalResult += "/* The insert queries */\n";
    for(unsigned int i = 0; i < lines.size(); i++)
    {
        finalResult += "INSERT INTO " + tableName + "(";
        for(unsigned int j = 0; j < nbOfCols; j++)
        {
            finalResult += colsName.at(j);
            if(j != nbOfCols - 1) finalResult += ", ";
        }

        finalResult += ") VALUES(";
        for(unsigned int j = 0; j < nbOfCols; j++)
        {
            if(colsType.at(j).substr(0, 3) == "VAR")
            {
                finalResult += "\"";
                finalResult += lines.at(i).at(j);
                finalResult += "\"";
            }
            else
                finalResult += lines.at(i).at(j);

            if(j != nbOfCols - 1) finalResult += ", ";
        }

        finalResult += ");\n";
    }

    // write the result in the file
    cout << " |//////////  |       Create the SQL file...                             " << '\r';
    ofstream resultFile(resultFilePath.c_str(), ios::out);

    if(!resultFile)
    {
        cerr << "   ERROR: Failed to write the SQL file :'(" << endl << endl;
        system("pause");
        return 1;
    }

    //write
    resultFile << finalResult;

    // close the file and we're done ;)
    resultFile.close();

    cout << " |////////////|       Done!                                       " << endl << endl;

    system("pause");
    return 0;
}

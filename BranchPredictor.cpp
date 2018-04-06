/* Name: Tenzing Rabgyal
 * Net ID: tr1440
 * Course: EL-GY 6913-A “Computing Systems Architecture”
 * Instructor: Dr. Garg
 * Due Date: 12/22/2017
 * Lab2: Branch Prediction
 */

#include <iostream>
#include <fstream> // I/O stream class to operate on files
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

//FUNCTION PROTOTYPE
long long getPower(long long, long long);

void dumpResult(long long prediction)
{
    ofstream fileout;
    fileout.open("PredictionResults.txt",std::ios_base::app);
    if (fileout.is_open())
    {
        fileout << prediction << endl;
    }
    else cout<<"Unable to open file";
    fileout.close();
}


int main()
{
    
    long long mValue = 0, kValue = 0;
    string line, tempString, tempSubStrPC, tempSubStrTNT;
    ifstream getConfig, getTrace;
    long long tempValuePC, tempValueTNT;
    
    long long pcArray[548];
    long long tntArray[548];
    
    getConfig.open("config_new.txt");
    if(getConfig.is_open())
    {
        getConfig >> mValue >> kValue;
    }
    else cout << "There was an error while opening the config_new.txt File" << endl;
    getConfig.close();
    
    getTrace.open("trace.txt");
    if(getTrace.is_open())
    {
        int i = 0; // Incrementer for the Array
        while (getline(getTrace, line))
        {
            tempString = line;
            tempSubStrPC = tempString.substr(0,8);
            tempSubStrTNT = tempString.substr(9);
            
            istringstream buffer(tempSubStrPC);
            buffer >> hex >> tempValuePC; // tempValue is the decimal representation of the hex value from the file
            
            
            
            pcArray[i] = tempValuePC % (getPower(2, mValue)); // This formula will give us the decimal value of the m-bits of PC
            
            if ( ! (istringstream(tempSubStrTNT) >> tempValueTNT) ) tempValueTNT = 0; // Converts the string "tempSubStrTNT" to an integer value "tempValueTNT"
            
            tntArray[i] = tempValueTNT;
            i++;
        }
        
    }
    else cout << "There was an error while opening the trace.txt File" << endl;
    getTrace.close();
    
    cout << "m = " << mValue << endl;
    cout << "k = " << kValue << endl;
    
    long long numberOfRows = getPower(2, mValue); // Number of rows in the 2-bit saturating counter
    long long numberOfColumns = getPower(2, kValue)*2; // Number of columns in the 2-bit saturating table
    //long long numberOfColumns = getPower(2, kValue)*2;
    ////////////////////////
    long long saturatingTable [numberOfRows][numberOfColumns];
    /////////////////////
    for (int a = 0; a < numberOfRows; a++) // A nested for loop to initialize all values of saturatingTable to 1 (Taken with High Confidence).
    {
        for (int b = 0; b < numberOfColumns; b++)
        {
            saturatingTable [a][b] = 1;
        }
    }
    
    long long BHR [kValue];
    
    for (int c = 0; c < kValue; c++)
    {
        BHR [c] = 1; // We assume that the BHR is initialized assuming the previous k branches were Taken (1);
    }
    
    long long sumBHR, e, total, bhrPointer1, bhrPointer2, mPointer, firstSatValue, secondSatValue, PREDICTION, ACTUAL;
    double missRate = 0;
    
    for (int mainLoop = 0; mainLoop < 548; mainLoop++)
    {
        e = 0, total = 0, PREDICTION = 0, ACTUAL = 0;
        for (int d = 0; d < kValue; d++)
        {
            sumBHR = BHR[d]*getPower(2,e);  // Since the BHR is in an integer array. We use this loop to get the bitwise value of the 1s and 0s in the array;
            total += sumBHR;
            e++;
        }
        
        bhrPointer1 = total*2; // This variable will point to the first column of the two bit saturating table we need to access based on the BHR (Global History)
        bhrPointer2 = (total*2) + 1; // This variable will point to the second column of the two bit saturating table.
        
        mPointer = pcArray[mainLoop]; // This variable will point to the row of the two bit saturating table that we need to access.
        
        firstSatValue = saturatingTable[mPointer][bhrPointer1];
        secondSatValue = saturatingTable[mPointer][bhrPointer2];
        
        if (firstSatValue == 1 && secondSatValue == 1) // These are our prediction table
        {
            PREDICTION = 1; // Strongly Taken
        }
        else if (firstSatValue == 1 && secondSatValue == 0)
        {
            PREDICTION = 1; // Weakly Taken
        }
        else if (firstSatValue == 0 && secondSatValue == 1)
        {
            PREDICTION = 0; // Weakly NOT Taken
        }
        else if (firstSatValue == 0 && secondSatValue == 0)
        {
            PREDICTION = 0; // Strongly NOT Taken
        }
        
        ACTUAL = tntArray[mainLoop]; // The actual Taken / NOT Taken
        
        // Update the saturating tables based on the Actual Taken/Not Taken value, and the two bit values in the saturating table
        if (ACTUAL == 0 && firstSatValue == 1 && secondSatValue == 1)
        {
            saturatingTable[mPointer][bhrPointer1] = 1;
            saturatingTable[mPointer][bhrPointer2] = 0;
        }
        else if (ACTUAL == 0 && firstSatValue == 1 && secondSatValue == 0)
        {
            saturatingTable[mPointer][bhrPointer1] = 0;
            saturatingTable[mPointer][bhrPointer2] = 0;
        }
        else if (ACTUAL == 0 && firstSatValue == 0 && secondSatValue == 1)
        {
            saturatingTable[mPointer][bhrPointer1] = 0;
            saturatingTable[mPointer][bhrPointer2] = 0;
        }
        else     if (ACTUAL == 0 && firstSatValue == 0 && secondSatValue == 0)
        {
            saturatingTable[mPointer][bhrPointer1] = 0;
            saturatingTable[mPointer][bhrPointer2] = 0;
        }
        else if (ACTUAL == 1 && firstSatValue == 1 && secondSatValue == 1)
        {
            saturatingTable[mPointer][bhrPointer1] = 1;
            saturatingTable[mPointer][bhrPointer2] = 1;
        }
        else if (ACTUAL == 1 && firstSatValue == 1 && secondSatValue == 0)
        {
            saturatingTable[mPointer][bhrPointer1] = 1;
            saturatingTable[mPointer][bhrPointer2] = 1;
        }
        else if (ACTUAL == 1 && firstSatValue == 0 && secondSatValue == 1)
        {
            saturatingTable[mPointer][bhrPointer1] = 1;
            saturatingTable[mPointer][bhrPointer2] = 1;
        }
        else if (ACTUAL == 1 && firstSatValue == 0 && secondSatValue == 0)
        {
            saturatingTable[mPointer][bhrPointer1] = 0;
            saturatingTable[mPointer][bhrPointer2] = 1;
        }
        
        if (ACTUAL != PREDICTION)
        {
            missRate += 1;
        }
        
        //Update the BHR with the new values
        for (int f = kValue; f > 0; f--)
        {
            BHR [f] = BHR [f-1]; // Move value in BHR [2] to BHR [3];
        }
        BHR [0] = ACTUAL; // The BHR's first array will be updated with the most recent Taken/Not Taken
        
        
        dumpResult(PREDICTION);
    }
    
    cout << "Miss Prediction Rate = " << ((missRate/548)*100) << "%" << endl;
    
    return 0;
}

long long getPower(long long x, long long y)
{
    long long answer = 1;
    for (long long i = 0; i < y; i++)
    {
        answer *= x;
    }
    
    return answer;
}

#include "./fffcsvparser.h"

using std::string;
using std::stringstream;
using std::ifstream;
using std::vector;

FFFCsvParser::FFFCsvParser(const string& filePath, const char sep, const char dec) :
   filePath(filePath), sep(sep), dec(dec)
{
}

int FFFCsvParser::parseFile(uint *errorInLine)
{

   headLineEntries.clear();
   dataVectorList.clear();
   ifstream inpFile;
   inpFile.open(filePath.c_str());
   if(!inpFile.is_open()){
      FFFLog::logError(string("File could not be opened."));
      return 1;
   }
   string dataLine;
   std::getline(inpFile, dataLine);
   headLineEntries = split(dataLine, sep); // first line = header
   dataVectorList.resize(headLineEntries.size());
   //uint j=0;
   while(std::getline(inpFile, dataLine)){
      vector<string> dataSplit = split(dataLine, sep);
      for(uint i = 0; i < dataSplit.size(); ++i){
         double d = std::strtod(dataSplit[i].c_str(), nullptr);
         dataVectorList[i].push_back(d);
      }
   }
   return 0;
}


vector<string> FFFCsvParser::getHeadLines()
{
   return headLineEntries;
}

matD FFFCsvParser::getData() const
{
   return dataVectorList;
}


auto FFFCsvParser::findCharPos(std::string::const_iterator start,
                               std::string::const_iterator end,
                               const char c) const -> std::string::const_iterator
{
   do {
      if(c == *start) return start;
      ++start;
   } while(start != end);
   return end;
}


vector<string> FFFCsvParser::split(const string& s, const char delim, const bool keepEmpty) const {
   vector<string> result;
   string::const_iterator substart = s.begin();
   string::const_iterator subend = substart;
   while (subend != s.end()) {
      subend = findCharPos(substart, s.end(), delim);
      string subElem(substart, subend);
      if (keepEmpty || !subElem.empty())
         result.push_back(std::move(subElem));
      substart = subend + 1;
   }
   return result;
}



#include "lib.h"
#include "SLL.h"
#include "TextNormalizationOperators.h"
#include "PreprocessFIle.h"
#include "QuickSort.h"
using namespace std;

wstring discards[1942];
int nStopwords = 1942;
int convert[1 << 16];
wstring tokens = L"`~!@#$%^&*()–-_=+[]{}\\|;:'\",<.>/?“”•";
const float eps = 0.005;

SLL curList;

void addFIle (const wstring path){
    wstring meta = L"Crawl\\metadata\\";
    wstring index = L"Crawl\\index.txt";
    FILE* fileIndex = _wfopen(index.c_str(), L"a");
    fwprintf(fileIndex, L"%s\n", path.c_str());
    fclose(fileIndex);
    wstring cur = meta + magicString(path);
    FILE* fileMeta = _wfopen(cur.c_str(), L"w,ccs=UTF-8");
    FILE* curPath = _wfopen(path.c_str(), L"r,ccs=UTF-8");
    wstring s = L"";
    while (true){
        if (feof(curPath)){
            break;
        }
        s += fgetwc(curPath);
    }
    s.erase(s.size() - 1, 1);
    removeStopwords(s); unsignedDocument(s);
    int numWords = countInitialWords(s);
    wstring* words = new wstring[numWords]; 
    int cntWords = 0;
    wstring curWord = L"";
    for (int i = 0; i < s.length(); i++){
        if (s[i] == L' ' || s[i] == L'\n'){
            standardized(curWord);
            if (curWord.length() != 0 && curWord.length() <= 50){
                words[cntWords++] = curWord;
            }
            curWord = L"";
        }
        else curWord += s[i];
    }
    standardized(curWord);
    if (curWord.length() != 0 && curWord.length() <= 50) words[cntWords++] = curWord;
    quickSort(words, 0, cntWords - 1);
    fwprintf(fileMeta, L"%d\n", countWords(words, cntWords));
    curWord = words[0];
    int curNum = 1;
    for (int i = 1; i < cntWords; i++){
        if (words[i] != words[i - 1]){
            float weight = 1.00 * curNum / cntWords;
            if (weight >= eps && curWord.length() != 0){
                fwprintf(fileMeta, L"%s %f\n", curWord.c_str(), weight);
            }
            curWord = words[i];
            curNum = 1;
        }
        else curNum++;
    }
    float weight = 1.00 * curNum / cntWords;
    if (weight >= eps && curWord.size() != 0){
        fwprintf(fileMeta, L"%s %f\n", curWord.c_str(), weight);
    }
    delete[] words;
    fclose(fileMeta); fclose(curPath);
    addData(path, curList);
}

void removeFile(const wstring path){
    if (removePath(path, curList)){
        FILE* index = _wfopen(L"Crawl\\index.txt", L"w, css=UTF-8");
        Node* cur = curList.head;
        while (cur != NULL){
            fwprintf(index, L"%s\n", cur->path.c_str());
        }
    }
}

int binSearch(pack* a, int lo, int hi, wstring &key){
    int pos = -1;
    while (lo <= hi){
        int mi = (lo + hi) / 2;
        if (a[mi].word <= key){
            pos = mi;
            lo = mi + 1;
        }
        else hi = mi - 1;
    }
    if (pos == -1) return pos;
    if (a[pos].word != key) return -1;
    else return pos;
}

void searchData(SLL &curList, wstring s){
    unsignedDocument(s);
    int numWords = countInitialWords(s);
    wstring* words = new wstring[numWords]; 
    int cntWords = 0;
    wstring curWord = L"";
    for (int i = 0; i < s.length(); i++){
        if (s[i] == L' ' || s[i] == L'\n'){
            standardized(curWord);
            if (curWord.length() != 0 && curWord.length() <= 50){
                words[cntWords++] = curWord;
            }
            curWord = L"";
        }
        else curWord += s[i];
    }
    standardized(curWord);
    if (curWord.length() != 0 && curWord.length() <= 50) words[cntWords++] = curWord;

    FILE* ans = _wfopen(L"out.txt", L"w,ccs=UTF-8");
    Node* cur = curList.head;
    while (cur != NULL){
        float totalWeight = 0.00;
        for (int i = 0; i < cntWords; i++){
            wstring key = words[i];
            int id = binSearch(cur->listWord, 0, cur->nWords - 1, key);
            if (id != -1){
                totalWeight += cur->listWord[id].weight;
                //wcout << cur->path << L' ' << totalWeight << L'\n';
            }
        }
        fwprintf(ans, L"%s %f\n", cur->path.c_str(), totalWeight);
        cur = cur->nxt;
    }
}


void loadFileMeta(SLL &cur){
    FILE* fileIndex = _wfopen(L"Crawl\\index.txt", L"r,ccs=UTF-8");
    wchar_t buffer[1000];
    int cnt = 0;
    while (fgetws(buffer, 1000, fileIndex)){
        wstring path(buffer);
        if (path[path.size() - 1] == L'\n'){
            path.erase(path.size() - 1, 1);
        }
        cnt++;
        if (path.length() <= 4) continue; // ".txt" 
        addData(path, curList);
    }
    fclose(fileIndex);
}

int main()
{
    _setmode(_fileno(stdout), _O_WTEXT);
    InitList(curList);
    loadFileMeta(curList);
    prepare();
    wcout << L"Done !!!";
    cerr << "\nTime elapsed: " << 1000 * clock() / CLOCKS_PER_SEC << "ms\n";
    return 0;    
}
#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/types.h>
#include <fstream>
#include <bits/stdc++.h>
#include <sstream>
#include <stdio.h>
#include <cstdio>
#include <chrono>

using namespace std;

class Stat {
public:
    string clip;
    string codec;
    string conf;
    bool error;
    float time;
    float rapporto;
    float psnr;
    float ssim;
    float count;
    float count_i;
    float count_p;
    float count_b;
    float dimOriginale;
    float dimCompressa;
};

string getClipClass(string label);
bool nomeConfigurazioneUsato(string name);
char showMenu();
void creaConfigurazione();
void eliminaConfigurazione();
void mostraConfigurazioni();
void encode(string clip, string codec, int conf);
void eseguiCompressione();
bool replace(std::string& str, const std::string& from, const std::string& to);
void mostraRisultati();
void fullCompression();
int getConfNumber();
float getPSNRFromLine(std::string& line);
float getSSIMFromLine(std::string& line);
bool replaceAndCheck(std::string& str, const std::string& from, const std::string& to, int numero);
void clearDirs(string clip, string codec, string conf_name);
void createDirIfNotExists(string path);
void createDirs(string clip, string codec, string conf);
Stat getStats(string clip, string codec, string conf);
string getConfNameByIndex(int index);
int getConfIndexByName(string name);
float getFileSize(string path);
string format(string in, int totalLength);
string printLine(int len);
string formatLeft(string in, int len);
string printRound(float var, int decimal);
void clearOutput(string clip);
void clearAll();
void clearFolder(string path);
Stat getMediumStats(string codec, string conf);
void mostraRisultatiMedi();
void printStat(Stat stat);
void printRigaStatisticheMultiple(string label, Stat stat1, Stat stat2, Stat stat3, bool media);
int askConf();
string askCodec();
string askClip();
void mostraRisultato();
void mostraRisultatiConfigurazione();
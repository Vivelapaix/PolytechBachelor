#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/syslog.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>

struct ClassifierElem {
    std::vector<std::string> extList; // extension list
    std::string clName; // name of classifier
};

struct ConfigFileStruct{
    std::vector<ClassifierElem> subVec;
    std::string mainPath;
    int daemonInterval {0};

    ConfigFileStruct & operator=(ConfigFileStruct const & other) {
        ConfigFileStruct(other).swap(*this);
        return *this;
    }

    void swap(ConfigFileStruct & other) {
        std::swap(subVec, other.subVec);
        std::swap(mainPath, other.mainPath);
        std::swap(daemonInterval, other.daemonInterval);
    }
} configFile;

std::string configFilename;



bool stopLoop = false;
const char *pidFileName = "/var/run/mkd.pid";
const char *logFileName = "mkd";


void _runCommand(std::string &stringCommand) {
    if (system(stringCommand.c_str()) == -1) {
        syslog(LOG_ERR, "Command failed: %s", stringCommand.c_str());
    }
}

void DaemonInit() {
    // Open the log file.
    openlog(logFileName, LOG_PID, LOG_DAEMON);
    syslog(LOG_NOTICE, "Daemon run");

    std::ifstream fin(pidFileName, std::ios_base::in);
    if (fin.is_open()) {
        int ppid = 0;
        fin >> ppid;
        char tmpBuffer[256];
        snprintf(tmpBuffer, sizeof(tmpBuffer), "/bin/bash -c \"if [ -d /proc/%d/ ]; then kill -s SIGTERM %d; fi;\"",
                 ppid, ppid);
        std::string strCom = tmpBuffer;
        _runCommand(strCom);
        syslog(LOG_INFO, "The parent process was stopped: ppid = %d", ppid);
    }

    fin.close();
}

int LoadConfig(const char *file);
int Daemon();
int _readConfigFile();
void _forkP();
int _createPidFile();
void _runDaemon();

int _getDir(std::string &curDir, std::vector<std::string> &files);
bool _isExtension(std::string const & fileName, std::string const & extention);
void _folderClassification(std::vector<std::string> &fileList);


void StopDemon()
{
    struct stat buffer;

    if (!stat(pidFileName, &buffer) && remove(pidFileName) == -1) {
        syslog(LOG_ERR, "Error: file %s don't remove", pidFileName);
    }

    syslog(LOG_NOTICE, "Daemon stop");
    closelog();
}

int _getDir(std::string &curDir, std::vector<std::string> &files)
{
    DIR *dp;
    dirent *dirp;

    if ((dp  = opendir(curDir.c_str())) == nullptr) {
        std::cout << "Error(" << errno << ") opening " << curDir << std::endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != nullptr) {
        std::string name(dirp->d_name);
        if (name != "." && name != "..") {
            std::string curPath = curDir +"/"+name;

            if (dirp->d_type == DT_DIR) {
                _getDir(curPath,files);
            } else {
                curPath = curPath.substr(configFile.mainPath.size());
                files.push_back(curPath);
            }
        }
    }
    closedir(dp);
    return 0;
}

bool _isExtension(std::string const & fileName, std::string const & extention) {
    auto size_f = fileName.size();
    auto size_e = extention.size();
    return size_f > size_e &&
           !fileName.compare(size_f - size_e, size_e, extention.c_str()) &&
           fileName[size_f - size_e - 1] == '.';
}

bool _nInFolder(std::string const & fileName, std::string const & folderName) {
    auto size_f = fileName.size();
    auto size_e = folderName.size();
    return !(size_f > size_e &&
            fileName[size_e + 1] == '/' &&
            !fileName.compare(1, size_e, folderName.c_str()));
}

void _folderClassification(std::vector<std::string> &fileList)
{
    bool kflag;
    for (auto &fileElem : fileList) {
        kflag = false;
        for (auto &subVecEl : configFile.subVec) {
            for (auto &extentionEl : subVecEl.extList) {
                if (_isExtension(fileElem, extentionEl) && _nInFolder(fileElem, subVecEl.clName)) {
                    char tmpPathBuffer[256];
                    strcpy(tmpPathBuffer, fileElem.c_str());

                    std::string pathFile = dirname(tmpPathBuffer);
                    std::string fullPathFile = configFile.mainPath + "/" + subVecEl.clName + pathFile;
                    std::string syslr = "mkdir -p " + fullPathFile;
                    _runCommand(syslr);
                    std::string fullPathTmp = configFile.mainPath + fileElem;
                    syslr = "mv " + fullPathTmp + " " + fullPathFile;
                    _runCommand(syslr);
                    kflag = true;
                    break;
                }
            }
            if (kflag)
                break;
        }
    }
}

int LoadConfig(const char *file) {
    char tempFile[PATH_MAX] = {'\0'};
    if (!realpath(file, tempFile)) {
        syslog(LOG_ERR, "Real path to file failed: %s\n", strerror(errno));
        return 1;
    }
    configFilename = tempFile;

    return _readConfigFile();
}

void SignalHandlerUP(int signo) {
    _readConfigFile();
}

void SignalHandlerTERM(int signo) {
    stopLoop = true;
}

int Daemon()
{
    _forkP();

    int sid = setsid(); // Create a new SID for the child process.
    if (sid < 0) {
        syslog(LOG_ERR, "setsid fail");
        return 1;
    }

    syslog(LOG_INFO, "setsid success");

    _forkP();

    signal(SIGHUP, SignalHandlerUP);
    signal(SIGTERM, SignalHandlerTERM);

    umask(0);

    if (chdir("/") == -1) {
        syslog(LOG_ERR, "chdir fail");
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    if (_createPidFile()) {
        syslog(LOG_ERR, "Error in opening .pid file: %s", pidFileName);
        return 1;
    }
    _runDaemon();
    return 0;
}

/* Forking the parent process. */
void _forkP() {
    int pid = fork();

    if (pid < 0) {
        syslog(LOG_ERR, "Fork failure");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    syslog(LOG_INFO, "Fork success");
}

int _createPidFile()
{
    std::ofstream file(pidFileName);
    if (!file.is_open())
        return 1;

    int pid = getpid();
    file << pid;
    file.close();

    syslog(LOG_INFO, "%s: pid=%i have written", pidFileName, pid);
    return 0;
}

void _runDaemon()
{
    syslog(LOG_INFO, "Start daemon");

    while (!stopLoop) {
        std::vector<std::string> fileList;
        _getDir(configFile.mainPath, fileList);
        _folderClassification(fileList);
        sleep(configFile.daemonInterval);
    }

    syslog(LOG_INFO, "Stop daemon");
}

int _readConfigFile()
{
    syslog(LOG_INFO, "Open config file: %s\n", configFilename.c_str());
    std::ifstream fin(configFilename, std::ios_base::in);

    ConfigFileStruct tmpConfig;

    if (!fin.is_open()) {
        syslog(LOG_ERR, "Error: opening file");
        return 1;
    }

    std::string s("");

    try {
        std::getline(fin, s);
        std::istringstream s_str(s);
        s_str >> tmpConfig.daemonInterval;
    } catch (const std::exception &) {
        syslog(LOG_ERR, "Error: interval reading");
        return 1;
    }

    if (tmpConfig.daemonInterval <= 0) {
        syslog(LOG_ERR, "Error: invalid interval");
        return 1;
    }

    try {
        std::getline(fin, s);
        std::istringstream s_str(s);
        s_str >> tmpConfig.mainPath;
    } catch (const std::exception &) {
        syslog(LOG_ERR, "Error: folder path reading");
        return 1;
    }



    for (int kInd = 3; std::getline(fin, s); ++kInd) {
        ClassifierElem logE;
        std::istringstream istr(s);
        std::string tmp;
        istr >> logE.clName;
        istr >> tmp;
        if (tmp != "=") {
            syslog(LOG_ERR, "Error: error in %i line", kInd);
            continue;
        }
        for (int index = 1; istr >> tmp; ++index) {
            logE.extList.push_back(tmp);
        }
        tmpConfig.subVec.push_back(logE);
    }

    syslog(LOG_INFO, "Read file success");


    configFile = tmpConfig;

    fin.close();
    return 0;
}


int main(int argc, char **argv) {

   if (argc != 2) {
        printf("Invalid syntax.\nUsage: %s filename.cfg\n", argv[0]);
        return EXIT_FAILURE;
    }

    DaemonInit();

    if (LoadConfig(argv[1]) != 0) {
        StopDemon();
        return EXIT_FAILURE;
    }

    Daemon();
    StopDemon();
    return EXIT_SUCCESS;
}

